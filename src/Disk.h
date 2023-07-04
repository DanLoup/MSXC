#ifndef DISK_H
#define DISK_H
#pragma once
#include "Devices.h"
//void InsertDisk();

struct Disc : MSXDev{
	void setup();
	void run(int linecount);
	void write(int addr,int page,int value, bool io);
	int read(int addr,int page, bool io);
    std::vector <mpage> m;

	int dregs[5]; //Registers
	int sysst=0; //Controller register
	int dirq=0; //IRQ line (physically mapped)
	int dtrack=0; //Current disk track
	int dcmd=0,dcmdarg=0; //Command being executed
	int readct=0,readpt=0; //Data reading variables
	int stepvec=1;

	enum {WD_BUSY = 1,WD_HEAD = 0x20, WD_DRQ = 2, WD_LOSTDATA = 4};
	enum {WD_CMD = 0, WD_STATUS = 0, WD_TRACK = 1,WD_SECTOR = 2, WD_DATA = 3,WD_SYSTEM = 4,WD_READY = 4};
	enum {WD_CMD_SEEK,WD_CMD_STEP,WD_CMD_STPIN,WD_CMD_STPOUT,WD_CMD_RS,WD_CMD_WS,WD_CMD_RA,WD_CMD_RT};
	enum {WD_RSTAT = 0,WD_RTRA = 1,WD_RSEC = 2,WD_RDATA = 3};

};


#endif