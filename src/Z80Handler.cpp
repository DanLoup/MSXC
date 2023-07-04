#define EXECZ80
typedef unsigned char byte;
typedef unsigned short word;

//For now using the FMSX Z80 core
#include "Z80.h"
#include "Memory.h"

extern int xsc,ysc;
int pcadress=0;
int interrupt=0;
Z80 Tz80;

void BootZ80(){
	//Tz80.IPeriod=228;
	ResetZ80(&Tz80);
	int pu=2;
}
int intpick=INT_NONE;

void LoopZ80CPU(){
	if (interrupt>0){interrupt=0;
		IntZ80(&Tz80,INT_RST38);
	}
	ExecZ80(&Tz80,228);
}


void InterrupZ80(){
//    Tz80.IFF=(Tz80.IFF&~IFF_EI)|IFF_1; 
	IntZ80(&Tz80,INT_RST38);
//	Tz80.IRequest=INT_RST38;
	//intpick=INT_RST38;
}

void WrZ80( word Addr, byte Value){
//		wrm(Addr,Value);
	pcadress=Tz80.PC.W;
	writebus(Addr,Value,false);
}
byte RdZ80( word Addr){
//return rdm(Addr);
	pcadress=Tz80.PC.W;
	return readbus(Addr,false);
}
void OutZ80( word Port, byte Value){
	xsc=228-Tz80.ICount;
	//out(Port&0xff,Value);
	writebus(Port & 0xff,Value,true);
}
byte InZ80( word Port){
	xsc=228-Tz80.ICount;
	//return in(Port&0xff);
	return readbus(Port & 0xff,true);
}

word LoopZ80( Z80 *R){
	return INT_NONE;
}

/*
void PatchZ80( Z80 *R){
}
*/