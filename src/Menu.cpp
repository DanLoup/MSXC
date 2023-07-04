#ifdef WIN32
#include "wdirent.h"
#else
#include <dirent.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "math.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "dir.h"
#include "Display.h"
#include "audio.h"
#include "Z80handler.h"
#include "Devices.h"
#include "Disk.h"
extern int f6key,done;
extern float mouse_x,mouse_y,mouse_bt,mouse_dbt,old_mouse_bt;
char filert[100];
std::string * selfile;
int bdir=0;
void LoadConfigs();
void BootMenu(){
	LoadConfigs();
}
extern int MSXRunning;
int cwindow=0,oldcwin=0,oldfwin=0;
void StartWindow();
void ConfigMedia();
void ConfigMSX();
void FileMenu();

void LoopMenu(){
	TexSiz(16);
	if (MSXRunning==0){
		if (cwindow==0){StartWindow();}
		if (cwindow==1){ConfigMSX();}
		if (cwindow==2){ConfigMedia();}
		if (cwindow==3){FileMenu();}

	}else{
		//Back to menu
		if (f6key>0){MSXRunning=0;oldcwin=-1;cwindow=2;}
	}
}
//Thats a thing of beauty
int macsel,macss;
int swbts[10];
int cnewmsx=0;
int oldsel=-1;
void StartWindow(){
	std::string mc;
	ColorBox (vset(100,100),vset(924,668),cset(0.5,0.5,0.5,0.8),cset(0.4,0.4,0.4,0.8),cset(0.3,0.3,0.3,0.8),cset(0.4,0.4,0.4,0.8));
	DrawButton(vset(100,100),vset(924,668),2);
	Color(1,1,1,1);
	for (int a = 0;a < cfgs.size();a++){mc+=cfgs[a].name;mc+="|";}
	ABLocate(300,124);PrintT("Machines:");
	RunListbox(vset(300,140),vset(900,500),mc.c_str(),&macsel,&macss);
	RunButton(vset(120,140),vset(290,180),"New MSX",&swbts[0]);
	RunButton(vset(120,190),vset(290,230),"Edit MSX",&swbts[1]);
	RunButton(vset(300,510),vset(470,550),"Set Media",&swbts[2]);
	RunButton(vset(110,620),vset(300,660),"Run MSX",&swbts[3]);
	RunButton(vset(736,620),vset(916,660),"Exit MSXC",&swbts[4]);
		if (swbts[0]==1){cwindow=1;cnewmsx=1;oldcwin=0;}
		if (swbts[1]==1){cwindow=1;cnewmsx=0;oldcwin=0;}	
		if (swbts[2]==1){cwindow=2;oldcwin=0;}
		if (swbts[3]==1){MSXRunning=1;BootDevices();BootZ80();cwindow=2;oldcwin=0;}
		if (swbts[4]==1){done=1;oldcwin=0;}
	if (oldsel!=macsel){ChangeMSXCFG(macsel);}
	oldsel=macsel;
}

int cmbts[10];
char tx[10][100];
int medbs[10];
void ConfigMedia(){
	ColorBox (vset(100,100),vset(924,668),cset(0.5,0.5,0.5,0.8),cset(0.4,0.4,0.4,0.8),cset(0.3,0.3,0.3,0.8),cset(0.4,0.4,0.4,0.8));
	DrawButton(vset(100,100),vset(924,668),2);
	Color(1,1,1,1);
	ABLocate(300,124);PrintT("Configure media");
	RunButton(vset(110,620),vset(300,660),"Back",&cmbts[0]);
	if (cmbts[0]==1){
		if (oldcwin==0){cwindow=0;memset(swbts,0,16);}
		if (oldcwin==-1){MSXRunning=1;ReloadMedia();}
		cmbts[0]=0;
	}
	int tloc=0;
	
	for (int a=0;a<Medias.size();a++){
		Color(1,1,1,1);
		//if (Medias[a].datatype==0){PrintT("ROM:");}
		ABLocate(120,(tloc*60)+160);
		if (Medias[a].datatype==1){PrintT("Cartridge:");}
		if (Medias[a].datatype==2){PrintT("Disk:");}
		if (Medias[a].datatype==3){PrintT("Cassete:");}
		if (Medias[a].datatype>0){
			RunTextBox(vset(120,(tloc*60)+180),vset(500,(tloc*60)+220),&Medias[a].filen);
			RunButton(vset(500,(tloc*60)+180),vset(570,(tloc*60)+220),"...",&medbs[a]);
			if (medbs[a]>0){
				selfile=&Medias[a].filen;medbs[a]=0;cwindow=3;oldfwin=2;memset(swbts,0,16);
			}
			tloc++;
		}
	}
	
	RunButton(vset(736,620),vset(916,660),"Main Menu",&cmbts[1]);
	if (cmbts[1]==1){cwindow=0;oldcwin=2;memset(swbts,0,16);}
	RunButton(vset(556,620),vset(730,660),"Reset MSX",&cmbts[2]);
	if (cmbts[2]==1){MSXRunning=1;BootDevices();BootZ80();cwindow=2;oldcwin=0;}
}

int cfbts[10];
void ConfigMSX(){
	ColorBox (vset(100,100),vset(924,668),cset(0.5,0.5,0.5,0.8),cset(0.4,0.4,0.4,0.8),cset(0.3,0.3,0.3,0.8),cset(0.4,0.4,0.4,0.8));
	DrawButton(vset(100,100),vset(924,668),2);
	Color(1,1,1,1);
	ABLocate(300,124);PrintT("Configure MSX");
	RunButton(vset(110,620),vset(300,660),"Back",&cfbts[0]);
	if (cfbts[0]==1){cwindow=0;cfbts[0]=0;memset(swbts,0,16);}
}

int fmbts[10];
int selfil=0,selfsc=0;
DIR *dir = NULL;struct dirent *drnt = NULL;
char cdir[30000];struct filestr{char filename[260];int type;};filestr cdirf[1000];
int cdirfct=0;
std::string curdir;
std::string curfile;
char tname[260];
void Refreshdir(){
	cdir[0]=0;cdirfct=0;
	char ct[260];
	GetCurrentDir(ct,260);dir=opendir(ct);drnt=readdir(dir);curdir = ct;

	while(drnt){
		if (drnt->d_type==DT_DIR){strcat(cdir,"<");strcpy(tname,drnt->d_name);tname[50]=0;strcat(cdir,tname);strcat(cdir,">|");strcpy(cdirf[cdirfct].filename,drnt->d_name);cdirf[cdirfct++].type=1;}drnt=readdir(dir);
	}
	rewinddir(dir);drnt=readdir(dir);
	while(drnt){
		if (drnt->d_type!=DT_DIR){strcpy(tname,drnt->d_name);tname[50]=0;strcat(cdir,tname);strcat(cdir,"|");strcpy(cdirf[cdirfct].filename,drnt->d_name);cdirf[cdirfct++].type=0;}drnt=readdir(dir);
	}
	closedir(dir);
}

void FileMenu(){
	ColorBox (vset(100,100),vset(924,668),cset(0.5,0.5,0.5,0.8),cset(0.4,0.4,0.4,0.8),cset(0.3,0.3,0.3,0.8),cset(0.4,0.4,0.4,0.8));
	DrawButton(vset(100,100),vset(924,668),2);
	Color(1,1,1,1);
	ABLocate(110,110);PrintT("File Select:");
	if (oldfwin==0){cwindow=0;}
	if (bdir==0){
		Refreshdir();bdir=1;
	}
	RunListbox(vset(120,170),vset(914,560),cdir,&selfil,&selfsc);
	RunButton(vset(600,620),vset(750,660),"Cancel",&fmbts[0]);
	RunButton(vset(750,620),vset(900,660),"Select",&fmbts[1]);

	RunTextBox(vset(120,130),vset(914,170),&curdir);
	RunTextBox(vset(120,560),vset(914,600),&curfile);

	if (fmbts[0]==1){fmbts[0]=0;fmbts[1]=0;cwindow=oldfwin;memset(swbts,0,16);*selfile="";}
	if (fmbts[1]==1){
		*selfile="";*selfile+=curdir;*selfile+="/";*selfile+=curfile;
		fmbts[0]=0;fmbts[1]=0;cwindow=oldfwin;memset(swbts,0,16);
	}

	if (mouse_bt>0 && selfil<cdirfct && mouse_x>120 && mouse_y>170 && mouse_x<850 && mouse_y<560){
		if (cdirf[selfil].type==0){
			curfile = cdirf[selfil].filename;
		}
		if (cdirf[selfil].type==1 && mouse_dbt>0 && old_mouse_bt==0 ){
			curdir+="/";curdir+=cdirf[selfil].filename;
			SetCurrentDir(curdir.c_str());Refreshdir();selfsc=0;selfil=0;
		}
	}

}