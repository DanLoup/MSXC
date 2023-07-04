#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "math.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <string>
int winx=1024,winy=768;
SDL_Surface *screen;
SDL_Thread * sthread;
void MainLoop();
void BootMSX();
void ConfigMatrix();
void StartCounter();
double GetCounter();
extern int step;
int kbmatrix[16][8];
int skb[10][8]; 
char romname[128];
int gottarom=0;
int killthread=0;
int mousecombo=0;
int keybuni=0;
int keyb=0;
int f6key=0,f7key=0,f8key=0;
float mouse_x,mouse_y,mouse_bt,old_mouse_bt,mouse_dbt=0;
#ifdef WIN32
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	char ** mander;
	return main(0,mander);
}
#endif //Thanks SDL1

int done=0;
int main( int argc, char* argv[] )
{
	StartCounter();
	SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_TIMER);
	SDL_WM_SetCaption("MSXSC","pip");
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
	putenv(strdup("SDL_VIDEO_CENTERED=1"));
	screen = SDL_SetVideoMode(winx, winy, 32, SDL_OPENGL|SDL_RESIZABLE);
	SDL_EnableUNICODE(1);
	glViewport( 0, 0, winx, winy);glClearColor (0.5f,0.6f,0.9f,1.0f);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_TEXTURE_2D);
	BootMSX();
	ConfigMatrix();
	SDL_Event event;
	
	while(done==0){
	while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
			case SDL_KEYDOWN:
				keyb=event.key.keysym.sym;
				if (keyb==SDLK_F6){f6key=1;}
				if (keyb==SDLK_F7){f7key=1;}
				if (keyb==SDLK_F8){f8key=1;}
				for (int a=0;a<10;a++){
					for (int b=0;b<8;b++){
						if (skb[a][b]==keyb){kbmatrix[a][b]=1;}
					}
				}
				break;
			case SDL_KEYUP:
				keyb=event.key.keysym.sym;
				if (keyb==SDLK_F6){f6key=0;}
				if (keyb==SDLK_F7){f7key=0;}
				if (keyb==SDLK_F8){f8key=0;}
				for (int a=0;a<10;a++){
					for (int b=0;b<8;b++){
						if (skb[a][b]==keyb){kbmatrix[a][b]=0;}
					}
				}
				break;
			case SDL_MOUSEMOTION:
				mouse_x=((float(event.motion.x)));
				mouse_y=((float(event.motion.y)));

				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse_bt=event.button.button;
				if (mousecombo>0){mouse_dbt=1;}
				break;
			case SDL_MOUSEBUTTONUP:
				if (mouse_dbt==0){mousecombo=20;}else{mousecombo=0;mouse_dbt=0;}
				mouse_bt=0;
				break;
			case SDL_VIDEORESIZE:
				screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 32, SDL_OPENGL|SDL_RESIZABLE);
				winx=event.resize.w;winy=event.resize.h;
				glViewport(0,0,winx,winy);
				break;
			case SDL_QUIT:
				done=1;
			}
		}
		MainLoop();		
		SDL_GL_SwapBuffers ();
		glFinish();
		old_mouse_bt=mouse_bt;
		if (mousecombo>0){mousecombo--;}
	}
	killthread=1;
	SDL_CloseAudio();
return 0;
}

void ConfigMatrix(){
	int x=0,y=0;
	skb[y][x++]=SDLK_7;skb[y][x++]=SDLK_6;skb[y][x++]=SDLK_5;skb[y][x++]=SDLK_4;skb[y][x++]=SDLK_3;skb[y][x++]=SDLK_2;skb[y][x++]=SDLK_1;skb[y++][x++]=SDLK_0;x=0;
	skb[y][x++]=SDLK_SEMICOLON;skb[y][x++]=SDLK_RIGHTBRACKET;skb[y][x++]=SDLK_LEFTBRACKET;skb[y][x++]=SDLK_BACKSLASH;skb[y][x++]=SDLK_EQUALS;skb[y][x++]=SDLK_MINUS;skb[y][x++]=SDLK_9;skb[y++][x++]=SDLK_8;x=0; 
	skb[y][x++]=SDLK_b;skb[y][x++]=SDLK_a;skb[y][x++]=SDLK_F6;skb[y][x++]=SDLK_SLASH;skb[y][x++]=SDLK_PERIOD;skb[y][x++]=SDLK_COMMA;skb[y][x++]=SDLK_BACKQUOTE;skb[y++][x++]=SDLK_QUOTE; x=0;
	skb[y][x++]=SDLK_j;skb[y][x++]=SDLK_i;skb[y][x++]=SDLK_h;skb[y][x++]=SDLK_g;skb[y][x++]=SDLK_f;skb[y][x++]=SDLK_e;skb[y][x++]=SDLK_d;skb[y++][x++]=SDLK_c; x=0;
	skb[y][x++]=SDLK_r;skb[y][x++]=SDLK_q;skb[y][x++]=SDLK_p;skb[y][x++]=SDLK_o;skb[y][x++]=SDLK_n;skb[y][x++]=SDLK_m;skb[y][x++]=SDLK_l;skb[y++][x++]=SDLK_k; x=0;
	skb[y][x++]=SDLK_z;skb[y][x++]=SDLK_y;skb[y][x++]=SDLK_x;skb[y][x++]=SDLK_w;skb[y][x++]=SDLK_v;skb[y][x++]=SDLK_u;skb[y][x++]=SDLK_t;skb[y++][x++]=SDLK_s; x=0;
	skb[y][x++]=SDLK_F3;skb[y][x++]=SDLK_F2;skb[y][x++]=SDLK_F1;skb[y][x++]=SDLK_LALT;skb[y][x++]=SDLK_CAPSLOCK;skb[y][x++]=SDLK_RALT;skb[y][x++]=SDLK_LCTRL;skb[y++][x++]=SDLK_LSHIFT; x=0;
	skb[y][x++]=SDLK_RETURN;skb[y][x++]=SDLK_PAGEUP;skb[y][x++]=SDLK_BACKSPACE;skb[y][x++]=SDLK_F7;skb[y][x++]=SDLK_TAB;skb[y][x++]=SDLK_ESCAPE;skb[y][x++]=SDLK_F5;skb[y++][x++]=SDLK_F4; x=0;
	skb[y][x++]=SDLK_RIGHT;skb[y][x++]=SDLK_DOWN;skb[y][x++]=SDLK_UP;skb[y][x++]=SDLK_LEFT;skb[y][x++]=SDLK_DELETE;skb[y][x++]=SDLK_INSERT;skb[y][x++]=SDLK_HOME;skb[y++][x++]=SDLK_SPACE; x=0;
	memset(kbmatrix,0,sizeof(kbmatrix));
}   




int oldkey=0,keylock,keyunlock=0;
std::string * kbuffa;int kbuffsiz=0,kbmultiline=0;
int cursorpos=0;
void ConnectKB(std::string * buff,int siz,int multiline){
	kbuffa=buff;kbuffsiz=siz;kbmultiline=multiline;
}

int GetCurPos(){
	return cursorpos;
}
void SetCurPos(int pos){
	cursorpos=pos;
}
std::string GetCurbuff(){if (kbuffa){return *kbuffa;}std::string sti;return sti;}

void KBHandle(){

char te[2];
if (kbuffsiz>0){
	if ((keylock==0 || (keyunlock>30 && (keyunlock % 2)==0)) && (keybuni+keyb)>0){
		if (keybuni>31 && keybuni<127){
			te[0]=keyb;te[1]=0;
			if (kbuffa->size()<kbuffsiz-2){ 
				for (int c=kbuffa->size();c>=cursorpos;c--){(*kbuffa)[c+1]=(*kbuffa)[c];}
				kbuffa+=keybuni;
			}	
		}
			if (keyb==8){
				if (kbuffa->size()>0 && cursorpos>0){
					cursorpos--;for (int c=cursorpos;c<kbuffa->size();c++){(*kbuffa)[c]=(*kbuffa)[c+1];}
				}
			}
			if (keyb==SDLK_DELETE){
				if (kbuffa->size()>0 && cursorpos<kbuffa->size()){
					for (int c=cursorpos;c<kbuffa->size();c++){(*kbuffa)[c]=(*kbuffa)[c+1];}
				}
			}
			if (keyb==13 && kbmultiline==1){
				for (int c=kbuffa->size();c>=cursorpos;c--){(*kbuffa)[c+1]=(*kbuffa)[c];}
				kbuffa[cursorpos++]='\n';
			}
			if (keyb==SDLK_LEFT){if (cursorpos>0){cursorpos--;}}
			if (keyb==SDLK_RIGHT){if (cursorpos<kbuffa->size()){cursorpos++;}}
		if (keyb==SDLK_UP){
			int countdr=0,countadr=0,md=0;
			for (int c=cursorpos;c>-1;c--){
				if ((*kbuffa)[c]==10 && c<cursorpos){md++;}
				if (md==0){countdr++;}if (md==1){countadr++;}
				if (md==2){if (countdr>countadr){countdr=countadr;}cursorpos=c+countdr;break;}
			}
			if (md==0 || cursorpos==countdr){cursorpos=0;md=0;}if (md==1){cursorpos=countdr;}
		}

		if (keyb==SDLK_DOWN){
			int countdr=0,md=0;
			for (int c=cursorpos;c>-1;c--){
				countdr++;
				if ((*kbuffa)[c]==10 && c<cursorpos){break;}
			}
			for (int c=cursorpos;c<kbuffa->size();c++){
				if ((*kbuffa)[c]==10){md++;}
				if (md==1){cursorpos=c+countdr;break;}
			}
			if (md==0){cursorpos=kbuffa->size();}
		}
	
		keylock=1;oldkey=keyb;
		//keybtrigger=1;

	}
	if (keylock==1){keyunlock++;}
	if (keylock==1 && ((keyb+keybuni)==0 || keyb!=oldkey)){keylock=0;keyunlock=0;}
}

}

#ifdef WIN32
double PCFreq = 0.0;
__int64 CounterStart = 0;
void StartCounter(){
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	PCFreq = double(li.QuadPart)/1000000.0;
	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}
double GetCounter(){
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart-CounterStart)/PCFreq;
}
#else
#include <time.h>
struct timespec ti;
void StartCounter(){
	struct timespec ti;
	clock_gettime(CLOCK_REALTIME, &ti);
}

double GetCounter(){
	struct timespec tim;
	clock_gettime(CLOCK_REALTIME, &tim);
	return ((tim.tv_sec-ti.tv_sec)*1000000)+((tim.tv_nsec-ti.tv_nsec)/1000000);
}
#endif
