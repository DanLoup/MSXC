#include <math.h>
#include <string.h>
#include "PSG.h"

extern short audiobuff[18192];
extern int aswapct;
extern int abuct;
int selport=0;
int psgregs[16];
float sq2=0;
float voltb[16];
int noiseseed=0,noiseout=0;
extern float audiotimer;
int psgpage=0,psgpos=0;

void PSG::setup(){
	id = devid++;
	AllocPorts(id,ports);
	sq2=sqrt(2.0);
	for(int a=0;a<16;a++){voltb[a]=1/pow(sq2,15-(a));}
	voltb[0]=0;int muu=0;noiseseed=3084;
	memset(psgregs,0,64);
	memset(audiobuff,0,8192);
	psgpos=0;
}

void PSG::LoopPSG(int size){
	float ch1co=0,ch2co=0,ch3co=0,envco=0;
	int ss=1,nb=0;
	if ((psgregs[0]+psgregs[1])>0){ch1co=((111860.0/float(psgregs[0]+(psgregs[1]&15)*256))/22050.0)/ss;}
	if ((psgregs[2]+psgregs[3])>0){ch2co=((111860.0/float(psgregs[2]+(psgregs[3]&15)*256))/22050.0)/ss;}
	if ((psgregs[4]+psgregs[5])>0){ch3co=((111860.0/float(psgregs[4]+(psgregs[5]&15)*256))/22050.0)/ss;}
	if ((psgregs[0xb]+psgregs[0xc])>0){envco=(111860.0/float(psgregs[0xb]+(psgregs[0xc])*256))/22050.0;}
	noiseco=(111860.0/float((psgregs[0x6]&31)|1));noiseco/=(22050.0*ss*2);
	signed short ain;	
	for (int a=0;a<size;a++){
	envct+=envco;
	if (envct>16383.0){envct=fmod(envct,16384.0)+15.0;}
	fa1=0;fa2=0;fa3=0;
	for (int c=0;c<ss;c++){
		noisect+=noiseco;
		if (noisect>1){
			noisect=fmod(noisect,1.0);midnoise=0;
			noiseout^=(noiseseed&1);nb=(noiseseed&1)^((noiseseed&4)/4);noiseseed=(noiseseed>>1)+(nb<<16);
		}
		ch1ct+=ch1co;ch2ct+=ch2co;ch3ct+=ch3co;
		if (ch1ct>1){ch1ct=fmod(ch1ct,1.0);ch1fl=(ch1fl+1)&1;}
		if (ch2ct>1){ch2ct=fmod(ch2ct,1.0);ch2fl=(ch2fl+1)&1;}
		if (ch3ct>1){ch3ct=fmod(ch3ct,1.0);ch3fl=(ch3fl+1)&1;}
		chout1=(ch1fl|(psgregs[0x7]&1)) & (noiseout|((psgregs[0x7]&8)>>3));
		chout2=(ch2fl|((psgregs[0x7]&2)>>1)) & (noiseout|((psgregs[0x7]&16)>>4));
		chout3=(ch3fl|((psgregs[0x7]&4)>>2)) & (noiseout|((psgregs[0x7]&32)>>5));
		fa1+=chout1;fa2+=chout2;fa3+=chout3;
	}
		fa1*=1.0/2.0;fa2*=1.0/2.0;fa3*=1.0/2.0;
	vol1=psgregs[0x8];vol2=psgregs[0x9];vol3=psgregs[0xA];
			if ((psgregs[0xd]>-1 && psgregs[0xd]<4) || psgregs[0xd]==9){envol=15-envct;if (envol<0){envol=0;}}
			if ((psgregs[0xd]>3 && psgregs[0xd]<8) || psgregs[0xd]==0xf){envol=envct;if (envol>15){envol=0;}}
			if (psgregs[0xd]==8){envol=15-int(envct)%16;}
			if (psgregs[0xd]==0xa){envol=int(envct)%32;if (envol>15){envol=32-envol;}envol=15-envol;}
			if (psgregs[0xd]==0xb){envol=15-envct;if (envol<0){envol=15;}}
			if (psgregs[0xd]==0xc){envol=int(envct)%16;}
			if (psgregs[0xd]==0xd){envol=envct;if (envol>15){envol=15;}}
			if (psgregs[0xd]==0xe){envol=int(envct)%32;if (envol>15){envol=32-envol;}}
			if (vol1&16){vol1=envol;}
			if (vol2&16){vol2=envol;}
			if (vol3&16){vol3=envol;}
			vf1=voltb[vol1&15]*24000;vf2=voltb[vol2&15]*24000;vf3=voltb[vol3&15]*24000;
			ain=(fa1*vf1*0.33)+(fa2*vf2*0.33)+(fa3*vf3*0.33);
		aout[a]=ain*2;
		//if (psgpos>8191){psgpos=0;}
	}
}
void PSG::write(int addr,int page,int value, bool io){
	if (io){
		if (addr==0x0){selport=value&0xf;}
		if (addr==0x1){
			psgregs[selport]=value;
			if (selport==0x0D){envct=0;}
		}
	}
}

int PSG::read(int addr,int page, bool io){
	int retu=0xff;
	if (io){
		if (addr==0x2){
			psgregs[0xe]=0xff;
			retu=psgregs[selport];
		}
	}
	return retu;
}

void PSG::run(int linecount){
	//LoopPSG(735);
}
