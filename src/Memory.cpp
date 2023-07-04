#include "Memory.h"

void RAM::setup(){
	id = devid++;
	if (params[0]==0){params[0]=4;}
	m.clear();m.resize(params[0]);
	for (int a = 0; a < m.size();a++){for (int b = 0;b < 16384; b++){m[a].m[b]=255;}}
	AllocMem(id,slots);
	for (int a = 0;a < slots.size(); a++){spages[a] = a % m.size();}
	if (params[0]>4){
		ports.push_back(0xfc);ports.push_back(0xfd);ports.push_back(0xfe);ports.push_back(0xff);
		AllocPorts(id,ports);
	}
}

void RAM::run(int linecount){}
void RAM::write(int addr,int page,int value, bool io){
	if (!io){
		m[spages[page]].m[addr & 16383] = value;
	}else{
		spages[addr & 3] = value % m.size();
	}
}
int RAM::read(int addr,int page, bool io){
	int val=0;
	if (!io){
		val = m[spages[page]].m[addr & 16383];
	}else{
		val=spages[addr & 3];
	}
	return val;
}
//ROM device
void ROM::setup(){
	id = devid++;
	m.clear();m.resize(slots.size());
	int rct=0;
	if (mds.size()>0){
		for (int a = 0; a < m.size();a++){for (int b = 0;b < 16384; b++){
			m[a].m[b]=mds[0].data[rct++];if (rct>mds[0].data.size()){break;}
		}}
	}
	AllocMem(id,slots);
}
void ROM::run(int linecount){}
void ROM::write(int addr,int page,int value, bool io){}
int ROM::read(int addr,int page, bool io){
	return m[page].m[addr & 16383];
}
#include "SCC.h"

void CARTRIDGE::setup(){
	id = devid++;
	for (int a = 0; a < 4; a++){slots.push_back(mempos(a,params[0]+1));}
	AllocMem(id,slots);
	if (Medias[params[1]].data.size()>0){
		int rct=0;
		m.resize(Medias[params[1]].data.size()/0x2000); //Most mappers just use 8KB blocks instead of 16
		for (int b = 0;b < m.size();b++){for (int c = 0; c < 0x2000;c++){m[b].m[c]=Medias[params[1]].data[rct++];}}
		for (int b = 2; b < 8;b++){pgs[b]=(b-2) % m.size();}
	}
	if (m.size()>4){
		mapper = 1;	sccena = 1;	
		devs.push_back(new SCC());
		params[2] = devid;devs[params[2]]->setup();//Adding an SCC
	}

}
extern int sccena;

void CARTRIDGE::run(int linecount){}
int CARTRIDGE::read(int addr,int page, bool io){
	int val = 255;
	if (m.size()>0){
		int ipage=addr/0x2000;
		val = m[pgs[ipage] % m.size()].m[addr & 0x1fff];
		if (mapper == 1 && sccena>0){
			if (pgs[ipage]==0x3f && ipage==4 && (addr&0xfff)>0x7ff){
				val=devs[params[2]]->read((addr&0xff),0,false);
			}
			if (pgs[ipage]==0x80 && ipage==5 && (addr&0xfff)>0x7ff){
				val=devs[params[2]]->read((addr&0xff),0,true);
			}

		}
	}
	return val;
}


void CARTRIDGE::write(int addr,int page,int value, bool io){
	int ipage=addr/0x2000; 
	int sccv = addr & 0xfff;
	int hf = addr & 0x1fff;
	if (ipage>1 && sccv < 0x800 && m.size()>0 && (hf>0xfff || hf==0)){
		pgs[ipage] = value;
		if ((value == 0x80 || value == 0x3f) && mapper == 0){
			mapper = 1;sccena = 1;
			params[2] = devid;devs[params[2]]->setup();//Adding an SCC (again, and hopefully not accumulating)

		}
	}
	if (sccena >0 && ipage == 5 && pgs[ipage]==0x80 && (addr&0xfff)>0x7ff){
		devs[params[2]]->write((addr&0xff),0,value,true); //Writing to the SCC+

	}

	if (sccena >0 && ipage == 4 && pgs[ipage]==0x3f && (addr&0xfff)>0x7ff){
		devs[params[2]]->write((addr&0xff),0,value,false); //Or regular SCC
	}
}

