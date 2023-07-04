#ifndef PSG_H
#define PSG_H
#pragma once
#include "Devices.h"
void BootSCC();
void LoopSCC(int size);
int ReadSCC(int addr);
void WriteSCC(int addr,unsigned char value,int sccmode);
extern int sccena;


struct PSG : MSXDev{
	void setup();
	void run(int linecount);
	void write(int addr,int page,int value, bool io);
	int read(int addr,int page, bool io);
	void LoopPSG(int size);
	short aout[18192];

	float ch1ct=0,ch2ct=0,ch3ct=0,noisect=0;
	signed short ch1fl=0,ch2fl=0,ch3fl=0;
	signed short vol1,vol2,vol3;
	float envct=0,envfl=0,envol=0,noiseco=0;
	float chout1,chout2,chout3;
	float vf1,vf2,vf3,fa1,fa2,fa3,midnoise=0;

};

#endif 