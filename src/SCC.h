#ifndef SCC_H
#define SCC_H
#pragma once
#include "Devices.h"

struct SCC : MSXDev{
	void setup();
	void run(int linecount);
	void write(int addr,int page,int value, bool io);
	int read(int addr,int page, bool io);
	void LoopSCC(int size);
	short aout[18192];
};

#endif 