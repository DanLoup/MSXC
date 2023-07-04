#define WIN32_LEAN_AND_MEAN
//#include "windows.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "math.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "Display.h"
#include "audio.h"
#include "Z80handler.h"
#include "Devices.h"
#include "menu.h"
void BootMSX(){
	BootDisplay();
	BootAudio();
	BootMenu();
}

int MSXRunning=0;
void MainLoop(){
	LoopDisplay();
	if (MSXRunning==1){
		for (int a=0;a<262;a++){
			LoopZ80CPU();
			RunDevs(a);
		}
	}
	LoopMenu();
}

