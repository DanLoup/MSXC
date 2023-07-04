#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Devices.h"
#include "Memory.h"
#include "v9918.h"
#include "PSG.h"
#include "SCC.h"
#include "audio.h"
//#include "msxdef.h"
#include "Disk.h"
#include "dir.h"

std::vector <NMSXdef> cfgs; //All the configurations
int curmsxcfg=0; //Current configuration
int nslots[8][4]; //Blocks assigned to each device (plus secondary slots)
int nslotspg[8][4]; //Local block numbers
int nports[256]; //Ports assigned to each device
int nportsnb[256]; //Local port numbers
int porcfg[4],sporcfg[4]; //Current selected slots and subslots
std::vector<MSXDev*> devs; //Devices
int devid=0; //Counter used to allocate devices
std::vector<Media> Medias; //External media that can be swapped on the menu
std::vector <dzone> dzs; //Non-standard memory zones allocated to devices (mostly being used by 0xffff subslot detection)

void LoadConfigs(){
	int dvct=0;
	char biosdir[260];
	GetCurrentDir(biosdir,260);
	std::string fd;
	Medias.clear();
		cfgs.push_back(NMSXdef());
	cfgs[0].name = "MSX 1";
	cfgs[0].dvs.push_back(DevDesc(DEV_RAM));cfgs[0].dvs.back().ecfg[0]=4;cfgs[0].dvs.back().blini.push_back(mempos(0,3));cfgs[0].dvs.back().blend.push_back(mempos(3,3));
	cfgs[0].dvs.push_back(DevDesc(DEV_ROM));cfgs[0].dvs.back().blini.push_back(mempos(0,0));cfgs[0].dvs.back().blend.push_back(mempos(1,0));
	fd=biosdir;fd+="/BIOS/MSX.ROM";cfgs[0].dvs.back().mds.push_back(Media(fd,0));
	cfgs[0].dvs.push_back(DevDesc(DEV_V9938,0x98,0x98+3));
	cfgs[0].dvs.push_back(DevDesc(DEV_PPI,0xA8,0xA8+3));
	cfgs[0].dvs.push_back(DevDesc(DEV_PSG,0xA0,0xA4));
	cfgs[0].dvs.push_back(DevDesc(DEV_DISK));cfgs[0].dvs.back().blini.push_back(mempos(1,1));cfgs[0].dvs.back().blend.push_back(mempos(2,1));
	fd=biosdir;fd+="/BIOS/DISK.ROM";cfgs[0].dvs.back().mds.push_back(Media(fd,0));
	cfgs[0].dvs.back().ecfg[0]=1;
	cfgs[0].dvs.push_back(DevDesc(DEV_CARTPORT));cfgs[0].dvs.back().ecfg[0]=1;cfgs[0].dvs.back().ecfg[1]=0;
	cfgs[0].emedia.push_back(Media("",1));
	cfgs[0].emedia.push_back(Media("",2));

	cfgs.push_back(NMSXdef());
	cfgs[1].name = "MSX 2";
	cfgs[1].dvs.push_back(DevDesc(DEV_RAM));cfgs[1].dvs.back().ecfg[0]=16;cfgs[1].dvs.back().blini.push_back(mempos(0,7));cfgs[1].dvs.back().blend.push_back(mempos(3,7));
	cfgs[1].dvs.push_back(DevDesc(DEV_ROM));cfgs[1].dvs.back().blini.push_back(mempos(0,0));cfgs[1].dvs.back().blend.push_back(mempos(1,0));
	fd=biosdir;fd+="/BIOS/MSX2.ROM";cfgs[1].dvs.back().mds.push_back(Media(fd,0));
	cfgs[1].dvs.push_back(DevDesc(DEV_ROM));cfgs[1].dvs.back().blini.push_back(mempos(0,5));cfgs[1].dvs.back().blend.push_back(mempos(0,5));
	fd=biosdir;fd+="/BIOS/MSX2EXT.ROM";cfgs[1].dvs.back().mds.push_back(Media(fd,0));
	cfgs[1].dvs.push_back(DevDesc(DEV_V9938,0x98,0x9b));cfgs[1].dvs.back().ecfg[0] = 1; //9938
	cfgs[1].dvs.push_back(DevDesc(DEV_PPI,0xA8,0xA8+3));cfgs[1].dvs.back().ecfg[0] = 1; //Enable second slots
	cfgs[1].dvs.push_back(DevDesc(DEV_PSG,0xA0,0xA4));
	cfgs[1].dvs.push_back(DevDesc(DEV_RTC,0xb4,0xb5));
	cfgs[1].dvs.push_back(DevDesc(DEV_DISK));cfgs[1].dvs.back().blini.push_back(mempos(1,5));cfgs[1].dvs.back().blend.push_back(mempos(2,5)); //MSXDOS insists in reading the disk in the wrong page
	fd=biosdir;fd+="/BIOS/DISK.ROM";cfgs[1].dvs.back().mds.push_back(Media(fd,0));
	cfgs[1].dvs.back().ecfg[0]=1;
	cfgs[1].dvs.push_back(DevDesc(DEV_CARTPORT));cfgs[1].dvs.back().ecfg[0]=0;cfgs[1].dvs.back().ecfg[1]=0;
	cfgs[1].emedia.push_back(Media("",1));
	cfgs[1].emedia.push_back(Media("",2));
}

void BootDevices(){
	SetupMSXN(curmsxcfg);
	FILE * loadrom;
	int sz=0;
	int runrom=0;
}
extern float audiotimer;
//int underrunct=0;
//float laudio=0;

int vac=0,setp=0,keyun=0,keypr=0;
int psat[4];
int ffports[4],ffmem=0;

int LoadMedia(int media){
	int sz=0,loaded=0;
	Medias[media].data.clear();
	FILE * fil=fopen(Medias[media].filen.c_str(),"rb");
	if (fil){
		fseek(fil, 0L, SEEK_END);sz = ftell(fil);fseek(fil, 0L, SEEK_SET);
		Medias[media].data.resize(sz+10);
		fread(Medias[media].data.data(),1,sz,fil);
		fclose(fil);
		loaded=1;
	}
	return loaded;
}

int Media::Load(){
	int sz=0,loaded=0;
	data.clear();
	if (filen.size()>0){
		FILE * fil=fopen(filen.c_str(),"rb");
		if (fil){
			fseek(fil, 0L, SEEK_END);sz = ftell(fil);fseek(fil, 0L, SEEK_SET);
			data.resize(sz+10);
			fread(data.data(),1,sz,fil);
			fclose(fil);
			loaded=1;
		}
	}
	return loaded;
}

void ChangeMSXCFG(int m){
	if (m<cfgs.size()){
		Medias.clear();
		for (int a = 0; a < cfgs[m].emedia.size();a++){Medias.push_back(cfgs[m].emedia[a]);}
		curmsxcfg = m;
	}
	ReloadMedia();
}

void SetupMSXN(int m){
	memset(nslots,0,8*4*4);memset(nslotspg,0,8*4*4);
	memset(nports,0,256);memset(nportsnb,0,256);
	memset(porcfg,0,16);memset(sporcfg,0,16);
	ReloadMedia();
	devid=0;
	devs.clear();
	dzs.clear();
	for (int a = 0; a < cfgs[m].dvs.size();a++){
		bool nd=false;
		switch(cfgs[m].dvs[a].devtype){
			case DEV_RAM:devs.push_back((MSXDev*) new RAM());nd=true;break;
			case DEV_ROM:devs.push_back((MSXDev*) new ROM());nd=true;break;
			case DEV_PSG:devs.push_back((MSXDev*) new PSG());nd=true;break;
			case DEV_V9938:devs.push_back((MSXDev*) new V9938());nd=true;break;
			case DEV_PPI:devs.push_back((MSXDev*) new PPI());nd=true;break;
			case DEV_RTC:devs.push_back((MSXDev*) new RTC());nd=true;break;
			case DEV_CARTPORT:devs.push_back((MSXDev*) new CARTRIDGE());nd=true;break;
			case DEV_DISK:devs.push_back((MSXDev*) new Disc());nd=true;break;
		}
		if (nd){
			devs.back()->type = cfgs[m].dvs[a].devtype;
			devs.back()->slots.clear();
			for (int b = 0; b < cfgs[m].dvs[a].blini.size();b++){
				for (int x = cfgs[m].dvs[a].blini[b].slot; x < cfgs[m].dvs[a].blend[b].slot+1; x++){
					for (int y = cfgs[m].dvs[a].blini[b].page; y < cfgs[m].dvs[a].blend[b].page+1; y++){
						devs.back()->slots.push_back(mempos(x,y));
					}
				}
			}
				devs.back()->ports.clear();
				for (int b = 0; b < cfgs[m].dvs[a].pini.size();b++){
				for (int c = cfgs[m].dvs[a].pini[b]; c < cfgs[m].dvs[a].pend[b]+1;c++){devs.back()->ports.push_back(c);}
			}
			devs.back()->mds.clear();
			for (int b = 0; b < cfgs[m].dvs[a].mds.size();b++){
				devs.back()->mds.push_back(cfgs[m].dvs[a].mds[b]);
					devs.back()->mds.back().Load();
			}
			for (int b = 0; b < 4;b++){
				devs.back()->params[b]=cfgs[m].dvs[a].ecfg[b];
			}
			devs.back()->setup();
		}
	}
}

int readbus(int addr, bool io){
	int val = 255;
	if (addr==0xf7 && io){val=0;}
	for (int a = 0; a < dzs.size();a++){
		if (dzs[a].ini>=addr && dzs[a].end<=addr){
			return devs[dzs[a].device]->read(addr,0,false);
		}
	}
	if (!io){
		int pg = (addr/16384);
		int v = nslots[porcfg[pg]][pg];int p = nslotspg[porcfg[pg]][pg];
		if (v>0){val =devs[v-1]->read(addr,p,io);}
	}else{
		int v = nports[addr];
		if (v>0){val =devs[v-1]->read(nportsnb[addr],0,io);}	}
	return val;
}
void writebus(int addr,unsigned char data, bool io){
	for (int a = 0; a < dzs.size();a++){
		if (dzs[a].ini>=addr && dzs[a].end<=addr){
			devs[dzs[a].device]->write(addr,0,data,false);
			return;
		}
	}
	if (!io){
		int pg = (addr/16384);
		int v = nslots[porcfg[pg]][pg];int p = nslotspg[porcfg[pg]][pg];
		if (v>0){devs[v-1]->write(addr,p,data,io);}
	}else{
		int v = nports[addr];
		if (v>0){devs[v-1]->write(nportsnb[addr],0,data,io);}
	}

}

void RunDevs(int scan){
	for (int a = 0; a < devs.size();a++){
		if (devs[a]->everyscan || scan == 0){devs[a]->run(scan);}
	}
	if ((scan % 13)==0){LoopAudio(35);}

}

void AllocMem(int devid,std::vector<mempos> pgs){
	for (int a = 0; a < pgs.size();a++){
		nslots[pgs[a].page][pgs[a].slot]= devid + 1;
		nslotspg[pgs[a].page][pgs[a].slot]= a;
	}
	int loca=0;

}

void AllocPorts(int devid,std::vector<int> port){
	for (int a = 0; a < port.size();a++){
		nports[port[a]]=devid+1;
		nportsnb[port[a]]=a;
	}
}

void ReloadMedia(){
	for (int a = 0 ;a < Medias.size();a++){Medias[a].Load();}
}

void MSXDev::setup(){id = devid++;}
void MSXDev::run(int linecount){}
void MSXDev::write(int addr,int page,int value, bool io){}
int MSXDev::read(int addr,int page, bool io){return 0;}

extern int kbmatrix[16][8];

void PPI::setup(){
	id = devid++;AllocPorts(id,ports);
	if (params[0]==1){dzs.push_back(dzone());dzs.back().ini = 0xffff;dzs.back().end = 0xffff;dzs.back().device = id;}
}



void PPI::run(int linecount){}
void PPI::write(int addr,int page,int value, bool io){
	int va=value;
	if (io){
		if (addr==0){
			for (int a =0; a < 4;a++){porcfg[a]=va & 3;va/=4;}pri = value;}		
		if (addr==2){keyun=(value&15);}
	}else{
		if (addr==0xffff && params[0]==1 && ((pri/64) & 3)==3){sub = value;}
	}
	if (params[0]==1){
		va = pri;int va2=sub;
		for (int a = 0; a < 4; a++){
			if ((va & 3)==3){porcfg[a]=(va2 & 3)+4;}
			va/=4;va2/=4;
		}
	}
}
int PPI::read(int addr,int page, bool io){
	int val =255;
	if (io){
		if (addr==0){val = pri;}
		if (addr==1){
			if (keyun<9){
				int dkeyb=256,acc=0;
				for (int a=0;a<8;a++){dkeyb/=2;acc+=(1-kbmatrix[keyun][a])*dkeyb;}val=acc; //Keyboard emulation
			}
		}
	}else{
		if (addr==0xffff && params[0]==1 && ((pri/64) & 3)==3){val = sub^0xff;}
	}
	return val;
}


void RTC::setup(){id=devid++;AllocPorts(id,ports);}
void RTC::run(int lines){}
void RTC::write(int addr,int page, int value, bool io){
	if (addr==0){rtcpos=value & 15;}
	if (addr==1){
		rtcmem[rtcpage][rtcpos]=value;
		if (rtcpos==0xd){rtcpage=value & 3;}
	}
}
int RTC::read(int addr,int page, bool io){
	int val =255;
	if (addr==1){val = rtcmem[rtcpage][rtcpos];
		if (rtcpos>0xc){val=0x20;}
		if (rtcpos>0xd){val=rtcpage;}
	}
	return val;
}

