//Very basic WD1793 drive emulation (same as FMSX)

#include <stdio.h>
#include <string.h>
#include "Devices.h"
#include "Memory.h"
#include "Z80handler.h"
#include "Disk.h"


void Disc::setup(){
	id = devid++;
	m.clear();m.resize(slots.size());
	if (mds.size()>0){
		for (int b = 0;b < 16384; b++){m[0].m[b]=mds[0].data[b];}
	}
	AllocMem(id,slots);
}
void Disc::run(int linecount){
}

int Disc::read(int addr,int page, bool io){
	int dt=m[page].m[addr & 16383];
	if ((addr & 16383)>0x3ff7 && Medias[params[0]].data.size()>0){
		int dblsize = Medias[params[0]].data[0x1a]-1; //360KB single/dual sided disks are a bit different
		int add=(addr & 0xf) - 8;
		if (add == 3 && readct>0){
			readct--;dregs[add] = Medias[params[0]].data[readpt++];
			if (readct==0){dcmd=0;dregs[0]&=255-3;dirq=128;}
		}
		if (add<5){dt = dregs[add];}
		if (add==7){dt = dirq;}
	}	


	return dt;
}
void Disc::write(int addr,int page,int val, bool io){
	if ((addr & 16383)>0x3ff7 && Medias[params[0]].data.size()>0){
//		int dblsize = int(Medias[params[0]].data.size()/370000);
		int dblsize = Medias[params[0]].data[0x1a]-1; //360KB single/dual sided disks are a bit different
		
		int add=(addr & 0xf) - 8;
		if (add>0 && add<4){
			dregs[add]=val;
		}//Sector/track/data
		if (add==4){
			sysst=val;}//Controller
		if (add==WD_CMD){
			dirq=0;
			if ((val/16)==13){ //Interrupt process
				dcmd = 0;dregs[0]=2;
				if (dtrack < 1){dregs[0]|=4;dtrack=0;} 
				if (val & 8){dirq=128;}
				goto werebusy; //Skipping setting the dcmd to 6
			}

			if (dregs[0]&WD_BUSY){goto werebusy;} //We're busy
			dregs[0]=0; //Clean status register
			int cmi = (val / 32) & 7,cmia = val&31;
			if (dcmd == 0){
				dcmd = cmi;dcmdarg = cmia;
				switch (dcmd){
					case WD_CMD_SEEK:
						if (dcmdarg & 16){
							//Seek
							//dtrack += dregs[WD_RDATA]-dregs[WD_RTRA];//blind drive blindly goes where it thinks it should go
							dtrack=dregs[WD_RDATA];
							dregs[WD_RTRA] = dregs[WD_RDATA];
							dcmd=0;
							if (dtrack < 1){dregs[0]|=4;dtrack=0;} //We banged the head against sector zero
							dregs[0]|=2;
							dirq = 128;
						}else{
							//Restore
							dtrack=0;dregs[WD_RTRA]=0;dcmd=0;
							dregs[WD_RSTAT]|=4; //Zero		
							dregs[0]|=2;dirq = 128;
						}
					break;
					case WD_CMD_STEP:
						dtrack+=stepvec;
						if (dcmdarg & 16){dregs[WD_RTRA]+=stepvec;}
						if (dtrack < 1){dregs[0]|=4;dtrack=0;} 
						dregs[0]|=2;
						dcmd=0;dirq = 128;
					break;
					case WD_CMD_STPIN:
						dtrack++;stepvec=1;
						if (dcmdarg & 16){dregs[WD_RTRA]++;}
						dregs[0]|=2;
						dcmd=0;dirq = 128;
					break;
					case WD_CMD_STPOUT:
						dtrack--;stepvec=-1;
						if (dcmdarg & 16){dregs[WD_RTRA]--;}
						if (dtrack < 1){dregs[0]|=4;dtrack=0;} 
						dregs[0]|=2;
						dcmd=0;dirq = 128;
					break;
					case WD_CMD_RS:
						if (dcmdarg & 16){
						}
						if (dregs[WD_RSEC]>0 && dregs[WD_RTRA] == dtrack){ //Can't read the track if you're not in it
							readct = 512;
							readpt = (dtrack * 9 * (1+dblsize))+(dregs[WD_RSEC]-1);
							if (sysst & 1 && dblsize>0){readpt+=9;}
							readpt*=512; //It's sectors, not bytes lol
							dregs[0]|=WD_DRQ|WD_BUSY;
							dirq = 64;
						}
					break;
					case WD_CMD_WS:dcmd=0;break;
					case WD_CMD_RA:
					dcmd=0;break;
					case WD_CMD_RT:
						dcmd=0;
					break;
				}
				if (dtrack<0){dtrack=0;}if (dtrack>76){dtrack=76;} //Whoops, let's not let the head fly out of the drive

			}
		}
		werebusy:;
	
	}
}