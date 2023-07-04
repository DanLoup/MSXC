#ifndef DEVICES_H
#define DEVICES_H
#pragma once
#include <vector>
#include <string>
enum {DEV_RAM,DEV_ROM,DEV_MM,DEV_FFMM,DEV_SUB,DEV_PSG,DEV_SCC,DEV_V9918,DEV_V9938,DEV_CARTPORT,DEV_DISK,DEV_PPI,DEV_RTC,DEV_KANJI};

struct mpage{
    unsigned char m[16384];
    bool locked=false;
    int r(int a){return m[a & 16383];}
    void w(int a,int d){if (!locked){m[a & 16383] = d;}}
};

struct mempos{int slot,page;
    mempos(){};
    mempos(int slotin,int pagein){slot = slotin;page = pagein;}
};

struct Media{
	std::vector<unsigned char> data;
	int datatype; //0-ROM 1-Cartridge 2-Disk 3-Cassete
	std::string filen;
	Media(){datatype=0;};
	Media(std::string file,int dt){datatype = dt;filen = file;}
	int Load();

};

struct DevDesc{
	int devtype;
	std::vector <mempos> blini,blend;
	std::vector <int> pini,pend;
	std::vector <Media> mds;
	int ecfg[16];
	DevDesc();
	DevDesc(int dt){devtype = dt;}
	DevDesc(int dt, int pi, int po){devtype = dt;pini.push_back(pi);pend.push_back(po);}

};

struct NMSXdef{
	std::string name;
	std::vector <DevDesc> dvs;
	std::vector <Media> emedia;
};

extern std::vector <NMSXdef> cfgs;
extern std::vector<Media> Medias;
extern int msxcfgc;
extern int devid;

struct dzone{
	int ini,end;
	int device;
};

struct MSXDev{
	int id;
	int type;
	bool everyscan=false;
	std::vector <int> ports;
	std::vector <mempos> slots;
	std::vector <Media> mds;
	int params[4];
	virtual void setup();
	virtual void run(int linecount);
	virtual void write(int addr,int page,int value, bool io);
	virtual int read(int addr,int page, bool io);
};

struct PPI : MSXDev{
	void setup();
	void run(int linecount);
	void write(int addr,int page,int value, bool io);
	int read(int addr,int page, bool io);
	int pri=0,sub=0;
};

struct RTC : MSXDev{
	void setup();
	void run(int linecount);
	void write(int addr,int page,int value, bool io);
	int read(int addr,int page, bool io);
	int rtcmem[4][16],rtcpos=0,rtcpage=0;
};


extern int nslots[8][4];
extern int nslotspg[8][4];
extern int nports[256];
extern int nportsnb[256];
extern std::vector<MSXDev*> devs;
extern std::vector <dzone> dzs;
extern int pcadress;
extern int interrupt;
void BootDevices();
void AllocMem(int devid,std::vector<mempos> pgs);
void AllocPorts(int devid,std::vector<int> port);


void SetupMSXN(int m);
int readbus(int addr, bool io);
void writebus(int addr,unsigned char data, bool io);
void RunDevs(int scan);
void ReloadMedia();
void ChangeMSXCFG(int m);

#endif