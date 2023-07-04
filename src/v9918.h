
#ifndef V9918_H
#define V9918_H
#pragma once
#include "Devices.h"
void Boot9918();
void DrawScanline9918(int scanid);
void Write9918(int port,int val);
int Read9918(int port);


struct V9938 : MSXDev{
	void setup();
	void run(int linecount);
	void write(int addr,int page,int value, bool io);
	int read(int addr,int page, bool io);

	void SetPixel(int xx,int yy,int col,int hs);
	int GetPixel(int xx,int yy,int hs);
	void Blitter(int cycles);
	void WriteVReg(int add,int val);

	int vblank;
	int Vmodel=0; //0-9918 1-9938
	unsigned char vram[65535*4];
	unsigned char pal[16][3];
	int wstages=0,fbyte=0,vaddr=0,memport=0; //Internal registers
	int spovf=-1,spover=0;
	int regs[64],ftb[8];
	int iscreen=0,irqpos=192,intline=0;

	//V9938
	int directreg=0,directreginc=0;
	int iscan=0,iscancur=0,iscancurrl=0;
	int statpage=0;
	int ysc=0; //xsc goes from 0 to 226, ysc goes from 0 to 262
	int hmem=0,palpt=0,scrolly=0;
	int hinterrupt=0;
	int blregs[40];
	int doffx=7,doffy=0; //Display offset
	int offkludge=0;

	//Blitter
	int blibusy=0,bliwri=1,blreturn=0;
	int cx,cy;
	float fcx,fcy;

	//Rendering
	unsigned char fbline[2048];
	unsigned char spscan[512];
	unsigned char spormask[512];
};

#endif