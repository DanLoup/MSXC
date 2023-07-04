#ifndef MEMORY_H
#define MEMORY_H
#pragma once
#include <vector>
#include <string>
#include "Devices.h"

struct RAM : MSXDev{
	void setup();
	void run(int linecount);
	void write(int addr,int page,int value, bool io);
	int read(int addr,int page, bool io);
	int spages[4];
    std::vector <mpage> m;
};

struct ROM : MSXDev{
	void setup();
	void run(int linecount);
	void write(int addr,int page,int value, bool io);
	int read(int addr,int page, bool io);
    std::vector <mpage> m;
};


struct cartblock{
	unsigned char m[8192];
};
struct CARTRIDGE : MSXDev{
	void setup();
	void run(int linecount);
	void write(int addr,int page,int value, bool io);
	int read(int addr,int page, bool io);
    std::vector <cartblock> m;
	int mapper=0,sccena=0;
	int pgs[8];
};

#endif