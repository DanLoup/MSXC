//#include "windows.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "math.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "Display.h"
#include "Memory.h"
GLuint fb=0,font=0,blank=0,mask=0;
float resx=256,resy=192;
unsigned char fbuffer[8000000];
unsigned char fonttex[200000];
//struct vec{float x,y;};
//struct color{float r,g,b,a;};
void DrawButton(vec uc,vec dc,int pressed);
void ColorBox(vec uc,vec dc,color c1,color c2,color c3,color c4);
void RunButton(vec uc,vec dc,char * text,int * memory);
void RunMenu(vec uc,vec dc,char * text,int * memory, int *lastsel);
void RunScroll(vec uc,vec dc,int * memory);
void RunToggle(vec uc,vec dc,char * text,int * memory);
void RunRadio(vec uc,vec dc,char * text,int * memory,int id);
vec vset(float x,float y){vec out;out.x=x;out.y=y;return out;}
vec vadd(vec in,float x,float y){vec out;out.x=x+in.x;out.y=y+in.y;return out;}
color cset(float r,float g,float b,float a){color out;out.r=r;out.g=g;out.b=b;out.a=a;return out;}

void BootDisplay(){
	glGenTextures (1,&blank);
	glBindTexture(GL_TEXTURE_2D,blank);
	memset(fbuffer,255,256*256*4);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,fbuffer);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenTextures (1,&mask);glBindTexture(GL_TEXTURE_2D,mask);
	int ct=0;
	for (int a=0;a<64;a++){for (int b=0;b<64;b++){fbuffer[ct++]=0;fbuffer[ct++]=0;fbuffer[ct++]=0;fbuffer[ct++]=(a & 1)*255;}}
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,64,64,0,GL_RGBA,GL_UNSIGNED_BYTE,fbuffer);glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenTextures (1,&font);glBindTexture(GL_TEXTURE_2D,font);SDL_Surface * textu=SDL_LoadBMP("font.bmp");memcpy(fonttex,textu->pixels,256*128*3);
	ct=0;for (int a=0;a<256*192*3;a+=3){fbuffer[ct++]=fonttex[a+2];fbuffer[ct++]=fonttex[a+1];fbuffer[ct++]=fonttex[a];fbuffer[ct++]=fonttex[a];}
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,128,0,GL_RGBA,GL_UNSIGNED_BYTE,fbuffer);glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenTextures (1,&fb);glBindTexture(GL_TEXTURE_2D,fb);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1024,1024,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);memset(fbuffer,0,200000);

}

char btext[65535];
float texsiz=24,texx=0,texy=0;
void TexSiz(float siz){texsiz=siz;}
void ABLocate(float x,float y){texx=x;texy=y;}
void Locate(float x,float y){texx=x*texsiz;texy=y*texsiz;}
void PrintT(std::string text,...){
	va_list vava;va_start(vava,text);
	vsnprintf(btext,65534,text.c_str(),vava);
	int tsiz=strlen(btext);
	glBindTexture(GL_TEXTURE_2D,font);
	glBegin(GL_TRIANGLES);
	float tpx=(texx-512)/512,tpy=0-((texy-384)/384);
	float fsizx=texsiz/512,fsizy=texsiz/384;
	float tx=1.0/16.0,ty=1.0/8.0,dex,dey;
	for (int a=0;a<tsiz;a++){
		tpx=(texx-512)/512;tpy=0-(((texy+texsiz)-384)/384);
		if (btext[a]>32 && btext[a]<128){
			dex=float((btext[a]-33) & 15)/16;dey=float(((btext[a]-33)/16) & 15)/8;
			glTexCoord2f(dex,dey);glVertex2f(tpx,tpy+fsizy);
			glTexCoord2f(dex+tx,dey);glVertex2f(tpx+fsizx,tpy+fsizy);
			glTexCoord2f(dex+tx,dey+ty);glVertex2f(tpx+fsizx,tpy);
			glTexCoord2f(dex,dey);glVertex2f(tpx,tpy+fsizy);
			glTexCoord2f(dex+tx,dey+ty);glVertex2f(tpx+fsizx,tpy);
			glTexCoord2f(dex,dey+ty);glVertex2f(tpx,tpy);
		}
		texx+=texsiz;
	}
	glEnd();
	texx=0;texy+=texsiz;
}
void LoopDisplay(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D,fb);
	glColor3f(1,1,1);
	glDisable(GL_BLEND);
	float frx=resx/1024,fry=resy/1024;
	glTexSubImage2D(GL_TEXTURE_2D,0,0,0,resx,resy,GL_RGBA,GL_UNSIGNED_BYTE,fbuffer);
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0,0);glVertex2f(-1,1);
		glTexCoord2f(frx,0);glVertex2f(1,1);
		glTexCoord2f(frx,fry);glVertex2f(1,-1);
		glTexCoord2f(0,0);glVertex2f(-1,1);
		glTexCoord2f(frx,fry);glVertex2f(1,-1);
		glTexCoord2f(0,fry);glVertex2f(-1,-1);
	glEnd();

	float mx=(256/64.0),my=(192/32.0);
	glBindTexture(GL_TEXTURE_2D,mask);
	glEnable(GL_BLEND);
	glColor4f(1,1,1,0.2);
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0,0);glVertex2f(-1,1);
		glTexCoord2f(mx,0);glVertex2f(1,1);
		glTexCoord2f(mx,my);glVertex2f(1,-1);
		glTexCoord2f(0,0);glVertex2f(-1,1);
		glTexCoord2f(mx,my);glVertex2f(1,-1);
		glTexCoord2f(0,my);glVertex2f(-1,-1);
	glEnd();
}

void Color(float r,float g,float b, float a){
	glColor4f(r,g,b,a);
}


#include "gui.h"