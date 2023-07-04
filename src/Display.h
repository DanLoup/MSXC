#ifndef DISPLAY_H
#define DISPLAY_H
#pragma once
#include <string>
#include <vector>
void TexSiz(float siz);
void ABLocate(float x,float y);
void Locate(float x,float y);
void PrintT(std::string text,...);
void Color(float r,float g,float b,float a);
void BootDisplay();
void LoopDisplay();
extern float resx,resy;
extern unsigned char fbuffer[8000000];

struct vec{float x,y;};
struct color{float r,g,b,a;};
void DrawButton(vec uc,vec dc,int pressed);
void ColorBox(vec uc,vec dc,color c1,color c2,color c3,color c4);
void RunButton(vec uc,vec dc,std::string text,int * memory);
void RunMenu(vec uc,vec dc,std::string text,int * memory, int *lastsel);
void RunScroll(vec uc,vec dc,int * memory);
void RunToggle(vec uc,vec dc,std::string text,int * memory);
void RunRadio(vec uc,vec dc,std::string text,int * memory,int id);
int RunTextBox(vec uc,vec dc,std::string * text);
void RunListbox(vec uc,vec dc,std::string text,int * memory,int * scmemory);
vec vset(float x,float y);
vec vadd(vec in,float x,float y);
color cset(float r,float g,float b,float a);

#endif