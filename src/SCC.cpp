#include <math.h>
#include <string.h>
#include "SCC.h"
extern short ab2[18192];
extern int aswapct;
extern int abuct;
unsigned int SCCRAM[1024];
unsigned int SCCREGS[64];
float samples[5][32];
float SCCchfr[5];
float SCCchct[5];
int SCCchvol[5];
int SCCsamplect[5];
float SCCchout[5];
extern float voltb[16];
int sccpage=0,sccpos=0;

struct SCCregtm{
	int time,addr,value;
};
SCCregtm SCCregst[1024];
int SCCregstct=0;
extern float audiotimer,laudio;

void SCC::setup(){
	id=devid++;type = DEV_SCC;
	memset(SCCchct,0,5*4);memset(SCCchfr,0,5*4);memset(SCCchvol,0,5*4);
	memset(SCCsamplect,0,5*4);memset(SCCchout,0,5*4);memset(SCCRAM,0,4096);memset(SCCREGS,0,256);
	memset(samples,0,5*32*4);
	sccpos=0;
}

void SCC::LoopSCC(int size){
	float bl=0;
	int sp=0;
	int strun=0;
	int mix=0;
	for (int a=0;a<size;a++){
		aout[a]=0;
		int mul=1;
		for (int b=0;b<5;b++){
			SCCchfr[b]=(((111860.0*32)/float(SCCREGS[(b*2)+0x0]+1+((SCCREGS[(b*2)+0x1]&15)*256)))/44100.0)*1.0;
			SCCchvol[b]=SCCREGS[b+0xA]&15;
			//if ((SCCREGS[0xF] & mul)==0){SCCchvol[b]=0;}
			mul*=2;
			
			SCCchout[b]=0;
			for (int c=0;c<1;c++){
				SCCchct[b]+=SCCchfr[b];
				sp=b;
				SCCchout[b]+=samples[sp][int(SCCchct[b])&31];
				SCCchct[b]=fmod(SCCchct[b],8192.0);
			}
			if (mix>32767){mix=32767;}if (mix<-32767){mix=-32767;}
			aout[a]+=(SCCchout[b]*0.15)*SCCchvol[b]*9;
		}
		if (aout[a]>0){
			int crashit=0;
		}
	}
}

int SCC::read(int addr,int page, bool io){
	return SCCRAM[addr];
}

void SCC::write(int addr,int page,int vale, bool io){
	unsigned char value = vale;
	SCCRAM[addr]=value;
	if (!io){ //Not a SCC+
		if (addr<0x80){
			samples[addr>>5][addr&31]=((signed char)value);	
			if ((addr>>5)==3){samples[4][addr&31]=((signed char)value);}

		}
		if (addr>0x7f && addr<0x90){SCCREGS[addr-0x80]=value;}
	}else{
		if (addr<0xA0){samples[addr>>5][addr&31]=((signed char)value);}
		if (addr>0x9f && addr<0xB0){SCCREGS[addr-0xA0]=value;}
	}


	if (addr>0x7F && addr<0x8A){
		int frr=(addr-0x80)>>1;
		//SCCchct[frr]=0;
	}
}

void SCC::run(int nb){}