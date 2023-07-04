#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "math.h"
#include "PSG.h"
#include "SCC.h"

short audiobuff[8192];
int aswapct=0;
int genpos=0; //Looping audio generation counter
int playpos=0;
float audiotimer=0;
int apage=7;
extern int psgpage;
extern int MSXRunning;
void SDL_AudioCallbackeon(void*  userdata,Uint8* stream,int len);

int audioud=0;
int sasiz=512;
int adg=0,ado=0; //Audio buffer counters
void BootAudio(){
	SDL_AudioSpec buff;
	SDL_memset(&buff, 0, sizeof(buff));
	buff.channels=1;
	buff.format=AUDIO_S16;
	buff.freq=44100;
	buff.samples=sasiz;
	buff.userdata=NULL;
	buff.callback=SDL_AudioCallbackeon;
	SDL_OpenAudio(&buff,NULL);
	SDL_PauseAudio(0);
}
void SDL_AudioCallbackeon(void*  userdata,Uint8* stream,int len){
	if (MSXRunning==1){
		int ilen=len/2;
		if ((ilen + playpos)>8191){
			memcpy(stream,&audiobuff[playpos],(8192-playpos)*2); //Number of samples required 
			memcpy(stream,audiobuff,len - ((8192-playpos)*2)); //wrap around the 8KB buffer
			playpos=(playpos + ilen) & 8191;
		}else{
			memcpy(stream,&audiobuff[playpos],len); //Consumes some samples generated by the system
			playpos+=ilen;
		}
		ado+=ilen;if (ado > 2000000000){adg=adg-ado;ado=0;adg=0;} //Counts the samples consumed by the SDL
	}else{
		memset(stream,0,len);
	}
}

void LoopAudio(int siz){
	int acor = 1; //Audio correction
	if ((adg-ado)<500){acor=64;}  //Too few samples generated
	if ((adg-ado)>2000){return;}  //Too many samples generater
	int aa=genpos,ab=genpos;
	for (int a = 0 ;a < siz * acor; a++){
		audiobuff[ab]=0;ab=(ab+1) & 8191; //Cleans up the buffer to mix the audio chips
	}
	for (int a = 0; a < devs.size();a++){
		if (devs[a]->type==DEV_PSG){
			PSG * p = (PSG*)devs[a];p->LoopPSG(siz * acor);aa=genpos;
			for (int b = 0; b < siz + acor;b++){
				audiobuff[aa]+=p->aout[b];aa=(aa + 1) & 8191;
			}
		}
		if (devs[a]->type==DEV_SCC){
			SCC * p = (SCC*)devs[a];p->LoopSCC(siz * acor);aa=genpos;
			for (int b = 0; b < siz + acor;b++){
				audiobuff[aa]+=p->aout[b];aa=(aa + 1) & 8191;
			}
		}

	}
	genpos=ab;
	adg+=siz*acor; // Counts the samples generated by the emulator
}