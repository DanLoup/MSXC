#include "Display.h"
#include "v9918.h"
#include <stdio.h>
#include <string.h>
int xsc=0;
unsigned int vdppal[]={0x0,0x0,0x3EB849,0x74D07D,0xDB6559,0xFF897D,0x002B99,0xDED087,0x5955E0,0x8076F1,0x60C2CB,0x65DBEF,0x3AA241,0xB766B5,0xCCCCCC,0xFFFFFF};
extern float resx,resy;

void V9938::setup(){
	id = devid++;
	AllocPorts(id,ports);
	everyscan=true;
	Vmodel = params[0];
	wstages=0;vaddr=0;

	for (int a=0;a<16;a++){pal[a][0]=vdppal[a] & 255;pal[a][1]=(vdppal[a]/256) & 255;pal[a][2]=(vdppal[a]/65536) & 255;}
	int mu=1;for (int a=0;a<8;a++){ftb[a]=mu;mu*=2;}
	memset(regs,0,64*4);
	iscancur=0;iscan=0;memset(blregs,0,160);irqpos=192;resx=256;resy=192;hmem=0;statpage=0;palpt=0;scrolly=0;hinterrupt=0;iscan=0;iscancur=0;
	intline=0;vblank=0;offkludge=0;
}

void V9938::SetPixel(int xx,int yy,int col,int hs){
	int addr=0,addr2=0,sadd1=0,rett=0;
	int tb1[2]={16,1},tb2[4]={64,16,4,1};
	int ftb1[2]={15,240},ftb2[4]={63,207,243,252};
	int tr=blregs[0xe]&8;
	int lg=blregs[0xe]&3;
	int x=xx&int(resx-1);
	int y=yy&1023;
	if (hs==0){
		if (tr==0 || col>0){
			if (iscreen==12){addr=(x/2)+y*128;rett=(col&15)*tb1[x&1];vram[addr]=(rett|(vram[addr]&ftb1[x&1]));}//Screen 5
			if (iscreen==16){addr=(x/4)+y*128;rett=(col&3)*tb2[x&3];vram[addr]=(rett|(vram[addr]&ftb2[x&3]));}//Screen 6
			if (iscreen==20){addr=(x/2)+y*256;rett=(col&15)*tb1[x&1];vram[addr]=(rett|(vram[addr]&ftb1[x&1]));}//Screen 7
			if (iscreen==28){addr=(x)+y*256;vram[addr]=col;}//Screen 8
		}
	}else{
		if (iscreen==12){addr=(x/2)+y*128;vram[addr]=col;}//Screen 5
		if (iscreen==16){addr=(x/4)+y*128;vram[addr]=col;}//Screen 6
		if (iscreen==20){addr=(x/2)+y*256;vram[addr]=col;}//Screen 7
		if (iscreen==28){addr=(x)+y*256;vram[addr]=col;}//Screen 8
	}
}

int V9938::GetPixel(int xx,int yy,int hs){
	int addr=0,addr2=0,sadd1=0,rett=0;
	int tb1[2]={16,1},tb2[4]={64,16,4,1};
	int x=xx&int(resx-1);
	int y=yy&1023;
	if (hs==0){
		if (iscreen==12){addr=(x/2)+y*128;rett=vram[addr];rett/=tb1[x&1];rett&=15;}//Screen 5
		if (iscreen==16){addr=(x/4)+y*128;rett=vram[addr];rett/=tb2[x&3];rett&=3;}//Screen 6
		if (iscreen==20){addr=(x/2)+y*256;rett=vram[addr];rett/=tb1[x&1];rett&=15;}//Screen 7
		if (iscreen==28){addr=(x)+y*256;rett=vram[addr];}//Screen 8
	}else{
		if (iscreen==12){addr=(x/2)+y*128;rett=vram[addr];}//Screen 5
		if (iscreen==16){addr=(x/4)+y*128;rett=vram[addr];}//Screen 6
		if (iscreen==20){addr=(x/2)+y*256;rett=vram[addr];}//Screen 7
		if (iscreen==28){addr=(x)+y*256;rett=vram[addr];}//Screen 8
	}
	return rett;
}

void V9938::Blitter(int cycles){
	int x1,y1,x2,y2,xc,yc;
	x1=blregs[0]+(blregs[1]&1)*256;y1=blregs[2]+(blregs[3]&3)*256;
	x2=blregs[4]+(blregs[5]&1)*256;y2=blregs[6]+(blregs[7]&3)*256;
	xc=blregs[8]+(blregs[9]&1)*256;yc=blregs[0xa]+(blregs[0xb]&3)*256;
	int cmd=(blregs[0xe]/16)&15,arg=blregs[0xe]&7,tr=blregs[0xe]&8;
	int sbcm=blregs[0xd];
	int tmm=0; //This defines how fast this thing actually is
	if (cmd==0xe){xc=resx-x2;} //y copy line

	if ((cmd==0xa || cmd==0xb || cmd==0xf)){ //Data upload/download blitter functions
		if (cycles==3){
			cx=0;cy=0;fcx=0;fcy=0;blibusy=1; 
			if ((x2+xc)>resx){xc=xc-((x2+xc)-resx);}
			tmm=1;
		}
		if (cycles==1){tmm=1;}	
	}
	if (!(cmd==0xa || cmd==0xb || cmd==0xf)){ //Everything else
		if (cycles==2 && tmm==0){tmm=16;}
		if (cycles==3 && tmm==0){
			tmm=1;cx=0;cy=0;fcx=0;fcy=0;blibusy=1;
			if (cmd!=0x7){
				if ((x2+xc)>(resx-1)){xc=xc-((x2+xc)-(resx-1));} //Clip if too big
			}
		}
	}
	int mvmode=0,hs=0;
	if (tmm>0){
		while (cmd>0 && tmm>0){	
			hs=0;
			switch(cmd){
				case 4:blreturn=GetPixel(x1+cx,y1,0);break; //get pixel
				case 5:SetPixel(x2+cx,y2,blregs[0xc],0);break; //put pixel
				case 6:SetPixel(x2+cx,y2,blregs[0xc],0);mvmode=1;break; //Seach for pixel
				case 7:SetPixel(x2+cx,y2+cy,blregs[0xc],0);mvmode=2;break;//Draw line
				case 8:SetPixel(x2+cx,y2,blregs[0xc],0);mvmode=3;break;//Fill retangle with bit ops
				case 9:SetPixel(x2+cx,y2,GetPixel(x1+cx,y1,0),0);mvmode=3;break;//copy retangle with bit ops
				case 0xa:blreturn=GetPixel(x1+cx,y1,0);mvmode=3;break;//get pixels with bit ops
				case 0xb:SetPixel(x2+cx,y2,blregs[0xc],0);mvmode=3;break;//put pixels with bit ops
				case 0xc:SetPixel(x2+cx,y2,blregs[0xc],1);mvmode=3;hs=1;break;//high speed fill retangle
				case 0xd:SetPixel(x2+cx,y2,GetPixel(x1+cx,y1,1),1);mvmode=3;hs=1;break;//high speed copy retangle
				case 0xe:SetPixel(x2+cx,y2,GetPixel(x2+cx,y1,1),1);mvmode=3;hs=1;break;//higher speed copy retangle (but limited X coordinate)
				case 0xf:SetPixel(x2+cx,y2,blregs[0xc],1);mvmode=3;hs=1;break;//high speed put bytes
			}
			if (mvmode<2){blibusy=0;blregs[0xe]=0;} //It's only a single pixel
			if (mvmode==2){ //Line drawing routines (it's all bresehan)
				if ((sbcm&1)==0){
					float dy=float(yc+1)/float(xc+1);
					fcx++;fcy+=dy;cx=fcx;cy=fcy;
					if (cx>xc){blregs[0xe]=0;blibusy=0;}
				}else{
					float dy=float(yc+1)/float(xc+1);
					fcx++;fcy+=dy;cy=fcx;cx=fcy;
					if (cy>xc){blregs[0xe]=0;blibusy=0;}
				}
			}
			if (mvmode==3){
				cx+=1+hs;
				if (cx>xc-(1+hs)){
					cx=0;yc--;y1++;y2++;
					if (yc<1){blregs[0xe]=0;blibusy=0;}
				}
				if (y1>1023){y1=0;}
				if (y2>1023){y2=0;}
			}
			tmm--;
			cmd=(blregs[0xe]/16)&15;
		}
	}
	blregs[2]=y1 & 255;blregs[3]=(y1/256) & 3; //Rewrite the registers
	blregs[6]=y2 & 255;blregs[7]=(y2/256) & 3;
	blregs[0xa]=yc & 255;blregs[0xb]=(yc/256) & 3;
}


void V9938::WriteVReg(int add,int val){
	if ((add<8 && Vmodel==0) || (add<64 && Vmodel>0)){
		regs[add]=val; //registers in general
		intline=(regs[1]&32)/32;

		if (Vmodel>0){iscan=(regs[0]&16)/16;}else{iscan=0;}
		if ((add & 7)==0 || (add & 7)==1){ //Horizontal resolution
			iscreen=((regs[0]&14)*2)+((regs[1]&8)/4)+(regs[1]&16)/16; 
			resx=256;
			if (iscreen>8){resx=256;}
			if (iscreen==16){resx=512;}
			if (iscreen==20){resx=512;}
		}
		if (add==0x09){ //Vertical resolution
			if ((val&2)>0){irqpos=255;}
			if ((val&128)>0){irqpos=212;resy=212;}else{irqpos=192;resy=192;}
		}
		if (add==0x11){directreg=val & 63;directreginc=val & 128;} //Register to access registers quickly
		if (add==0x13){ //Hblank line
			iscancur=((val-scrolly)) % irqpos; iscancurrl=val;
		}
		if (add==0x10){palpt=val;} //Change current color to be acessed
		if (add==0x12){ //Screen adjust register (used by some games to perform horizontal scrolling)
			doffx=(val&15);doffy=0-(((val/16)&15)); doffx-=7;
			if (offkludge==0){offkludge=1;doffx=0;}
		}
		if (add==0x17){scrolly=val;} //Vertical scroll
		if (add==0x0f){statpage=val;} //Change the status page to be read 
		if (add==0x0e){hmem=((val&7)*0x4000);} //Set the VRAM page to be written to/read
		if (add>0x1f){ //Blitter commands
			if (add==0x2c){Blitter(1);} //Port to write bytes to the VRAM directly (but neatly formatted)
			blregs[add-0x20]=val;
			if (add==0x2e){Blitter(3);} //Set the blitter command
		}
	}
}
int cstages=0;
void V9938::write(int addr,int page,int value, bool io){
	if (io){
		if (addr==1){
			if (wstages==0){
				wstages=1;fbyte=value; //You write two bytes to the address/register access port
			}else{
				if (value & 128){
					//Registers
					value&=0x7f;WriteVReg(value,fbyte);
				}else{
					//Memory write
					vaddr=fbyte+((value & 63)*256);
					if (value & 64){memport=vram[hmem+vaddr];}
				}
				wstages=0;
			}
		}
		if (addr==0){
			vram[hmem+vaddr++]=value; //Vram writing
			if (vaddr>0x3fff){vaddr=0;if (iscreen>7){hmem+=0x4000;}} //And on MSX 2, the page changes if you overflow
			if (hmem>0x1c000){hmem=0;}
			wstages=0;
		}
		if (Vmodel>0){
			if (addr==2){ //Palette swap
				if (cstages==0){
					pal[palpt][2]=(value&7)*31;pal[palpt][0]=((value/16)&7)*31;
				}	

				if (cstages==1 && palpt<16){ //Finish setting a palette
					pal[palpt][1]=(value&7)*31;
					palpt++;
				}
				cstages=(cstages+1)&1;
			}
			if (addr==3){
				if (directreg!=0x11){WriteVReg(directreg,value);}
				if (directreginc==0){directreg=(directreg+1);}
			}
		}
	}
}
int V9938::read(int addr,int page, bool io){
	int retval=0;
	if (addr==0){ //Vram read
		retval=memport;retval=vram[hmem+(vaddr++)];
		if (vaddr>0x3fff){vaddr=0;if (iscreen>7){hmem+=0x4000;}}
		if (hmem>0x1c000){hmem=0;}
		wstages=0;
	}

	if (addr==1){ //Status register read
		if (statpage==0){
			retval=vblank*128;retval+=spover*32;if (spovf>-1){retval+=spovf+64;}
			vblank=0;wstages=0;spover=0;
		}
		if (statpage==1){
			retval=hinterrupt;
			hinterrupt=0;
		}
		if (statpage==2){
			retval=0;
			if (ysc>irqpos){retval|=64;}
			if (xsc<16){retval|=32;}
			retval|=blibusy, //Busy plotting
			retval|=bliwri*128;//Blitter wants a new byte			
		}
	}
	return retval;
}
extern int step;

void V9938::run(int scanid){
int col=0;
int fline=(scanid+scrolly)&255;
int addr=0,bg=regs[7]&15,fr=(regs[7]/16)&15,scx,scy=fline & 7;
int maddr=((regs[2]&127)*1024),taddr=((regs[4]&63)*2048),coltb=((regs[3]&255)*64),spatt=((regs[5]&255)*128),sptb=((regs[6]&63)*2048);
int page=fline/64,xx;
int spriteen=1,spmode=8+(regs[1]&2)*4,spcx=0,spcy=0,faddr=0;
if (page==1 && (regs[4]&1)==0){page=0;}
if (page==2 && (regs[4]&2)==0){page=0;}
if (iscreen==4 || iscreen==8){taddr&=0x2000;coltb=0x2000-taddr;}
if (iscreen==1 || (regs[8]&2)>0){spriteen=0;}
int xtile,ytile=fline>>3,tileval;
ysc=scanid;

//0=screen 1, 1=screen 0, 4=Screen 2, 2=screen 3
spovf=-1;spover=0;
int spypos;
if (scanid<irqpos){
	if (spriteen>0){
		//Preprocessing sprite pixels because slower than plotting there
		memset(spscan,0,256);
		int spct=0,maxsp=31;
		for (int a=0;a<32;a++){if (vram[spatt+(a*4)]==0xd0){maxsp=a;break;}}
		if (iscreen<8){
			//V9918 sprites
			for (int a=maxsp;a>-1;a--){
				spypos=vram[spatt+(a*4)];
				if (spypos>irqpos){spypos=(0-(255-spypos));}else{spypos++;}
				if (fline>=spypos && fline<spypos+spmode && vram[spatt+(a*4)]!=0xd1 && spct<5 && vram[spatt+(a*4)+3]>0){
					spcy=fline-spypos;spct++;if (spct==5){spovf=a;}
					for (int b=vram[spatt+(a*4)+1];b<spmode+vram[spatt+(a*4)+1];b++){
						xx=b;
						if (vram[spatt+(a*4)+3]&128){xx-=32;}
						if (xx>-1 && xx<256){
							spcx=(b-vram[spatt+(a*4)+1])+1;
							if (spmode>8){if (spcx>8){faddr=((vram[spatt+(a*4)+2]&253)*8)+spcy+16;spcx=8-(spcx-8);}else{faddr=((vram[spatt+(a*4)+2]&253)*8)+spcy;spcx=8-spcx;}}else{faddr=(vram[spatt+(a*4)+2]*8)+spcy;spcx=8-spcx;}
							if (vram[sptb+faddr]&ftb[spcx]){if (spscan[xx]>0){spover=1;}spscan[xx]=(vram[spatt+(a*4)+3]&15);}
						}
					}
				}
			}
		}else{
			//V9938 sprites
			spatt=((regs[5]&252)*128)+(regs[0xb]&3)*0x8000;
			memset(spormask,0,256);
			int sppal=spatt-0x200;
			for (int a=maxsp;a>-1;a--){
				spypos=vram[spatt+(a*4)];
				if (spypos>(irqpos+scrolly)&255){spypos=(0-(255-spypos));}else{spypos++;}
				if (fline>=spypos && fline<spypos+spmode && vram[spatt+(a*4)]!=0xd1 && spct<9){
					spcy=fline-spypos;spct++;if (spct==9){spovf=a;}
					for (int b=vram[spatt+(a*4)+1];b<spmode+vram[spatt+(a*4)+1];b++){
						xx=b;
						if (vram[sppal+(a*16)+spcy]&128){xx-=32;}
						if (xx>-1 && xx<256){
							spcx=(b-vram[spatt+(a*4)+1])+1;
							if (spmode>8){if (spcx>8){faddr=((vram[spatt+(a*4)+2]&253)*8)+spcy+16;spcx=8-(spcx-8);}else{faddr=((vram[spatt+(a*4)+2]&253)*8)+spcy;spcx=8-spcx;}}else{faddr=(vram[spatt+(a*4)+2]*8)+spcy;spcx=8-spcx;}
							if (vram[sptb+faddr]&ftb[spcx]){
								if (spscan[xx]>0 && spormask[xx]==0){spover=1;}
								if (spormask[xx]==0 || vram[sppal+(a*16)+spcy]&64){spscan[xx]=(vram[sppal+(a*16)+spcy]&15);if (vram[sppal+(a*16)+spcy]&64){spormask[xx]=1;}}
								if (spormask[xx]>0 && (vram[sppal+(a*16)+spcy]&64)==0){spscan[xx]|=(vram[sppal+(a*16)+spcy]&15);}
							}
						}
					}
				}
			}
		}
	}
		if (regs[1]&64){
			col=bg;
			if (iscreen==1){
			//  Screen 0
	 		for (int a=0;a<256;a++){
					addr=a*4;col=bg;
					xtile=int((a+2)/6)+1;
					tileval=vram[maddr+(((xtile)+ytile*40))-4];
					scx=((a+2)%6);
					if ((vram[taddr+(tileval*8)+scy] & ftb[8-scx]) && a>15){col=fr;}
					
					fbline[addr++]=pal[col][0];fbline[addr++]=pal[col][1];fbline[addr++]=pal[col][2];fbline[addr++]=255;
				}
			}
			if (iscreen==0){
			//Screen 1
		 		for (int a=0;a<256;a++){
					addr=a*4;col=bg;
					xtile=int(a>>3);
					tileval=vram[maddr+(((xtile)+ytile*32))];scx=a&7;
					col=vram[coltb+(tileval>>3)]&15;
					if (vram[taddr+(tileval*8)+scy] & ftb[7-scx]){col=(vram[coltb+(tileval>>3)]>>4)&15;}
					if (spriteen==1){if (spscan[a]>0){col=spscan[a];}}
					fbline[addr++]=pal[col][0];fbline[addr++]=pal[col][1];fbline[addr++]=pal[col][2];fbline[addr++]=255;
				}
			}
			if (iscreen==4 || (iscreen==8 && Vmodel>0)){
			//Screen 2/4
				taddr=((regs[4]&60)*2048);
				coltb=((regs[3]&128)*64)+((regs[0x0a]&7)*0x4000);
		 		for (int a=0;a<256;a++){
					addr=a*4;col=bg;
					xtile=int(a/8);
					tileval=vram[maddr+(((xtile)+ytile*32))]+page*256;scx=a%8;
					col=vram[coltb+(tileval*8)+scy]&15;
					if (vram[taddr+(tileval*8)+scy] & ftb[7-scx]){col=(vram[coltb+(tileval*8)+scy]/16)&15;}
					if (spriteen==1){if (spscan[a]>0){col=spscan[a];}}
					fbline[addr++]=pal[col][0];fbline[addr++]=pal[col][1];fbline[addr++]=pal[col][2];fbline[addr++]=255;
				}
			}
			if (iscreen==2){
			//Screen 3
				if (scanid==24){
					Locate(1,1);
					Color(1,1,1,1);
					//PrintT("Indie game mode not supported yet");
				}
			}
			if (Vmodel>0){
				if (iscreen==12){
					//screen 5
					for (int a=0;a<256;a++){
						addr=a*4;col=bg;
						if ((a&1)!=0){col=vram[(a/2)+(fline*128)+((regs[2]&96)*1024)]&15;}else{col=(vram[(a/2)+(fline*128)+((regs[2]&96)*1024)]/16)&15;}
						if (spriteen==1){if (spscan[a]>0){col=spscan[a];}}
						if (col == 0){col = regs[7] & 15;}
						fbline[addr++]=pal[col][0];fbline[addr++]=pal[col][1];fbline[addr++]=pal[col][2];fbline[addr++]=255;
					}
				}
				if (iscreen==16){
					//screen 6
					for (int a=0;a<512;a++){
						addr=a*4;col=bg;
						if ((a&3)==3){col=vram[(a/4)+(fline*128)+((regs[2]&96)*1024)]&3;}
						if ((a&3)==2){col=(vram[(a/4)+(fline*128)+((regs[2]&96)*1024)]/4)&3;}
						if ((a&3)==1){col=(vram[(a/4)+(fline*128)+((regs[2]&96)*1024)]/16)&3;}
						if ((a&3)==0){col=(vram[(a/4)+(fline*128)+((regs[2]&96)*1024)]/64)&3;}
						fbline[addr++]=pal[col][0];fbline[addr++]=pal[col][1];fbline[addr++]=pal[col][2];fbline[addr++]=255;
					}
				}
				if (iscreen==20){
					//screen 7
					for (int a=0;a<512;a++){
						addr=a*4;col=bg;
						if ((a&1)==1){col=vram[(a/2)+(fline*256)+((regs[2]&96)*1024)]&15;}
						if ((a&1)==0){col=(vram[(a/2)+(fline*256)+((regs[2]&96)*1024)]/16)&15;}
						fbline[addr++]=pal[col][0];fbline[addr++]=pal[col][1];fbline[addr++]=pal[col][2];fbline[addr++]=255;
					}
				}
				if (iscreen==28){
					//screen 8
					for (int a=0;a<256;a++){
						addr=a*4;col=bg;
						col=vram[a+(fline*256)+((regs[2]&96)*1024)];
						fbline[addr++]=((col/4)&7)*32;fbline[addr++]=((col/32)&7)*32;fbline[addr++]=((col)&3)*64;fbline[addr++]=255;
					}
				}
			}

		}else{
			for (int a=0;a<resx;a++){
				addr=(a+((scanid)*resx))*4;col=bg;
				fbuffer[addr++]=pal[col][0];fbuffer[addr++]=pal[col][1];fbuffer[addr++]=pal[col][2];fbuffer[addr++]=255;
			}
		}

		float gre=0,fr,fg,fb;
		for (int a = 0; a < resx;a++){
			addr=(a+((scanid)*resx))*4;col=regs[7] & 15;
			fr=pal[col][0];fg=pal[col][1];fb=pal[col][2];
			int adr=a+doffx;
			if ((regs[1]&64)!=0 && adr>-1 && adr<resx){
				addr=(a+((scanid)*resx))*4;col=regs[7] & 15;
				fr=fbline[(adr*4)+0];fg=fbline[(adr*4)+1];fb=fbline[(adr*4)+2];
			}
			//brightness amplification to compensate for the scanline filter
			fr*=1.2;fg*=1.2;fb*=1.2;
			if (fr>255){fr=255;}if (fg>255){fg=255;}if (fb>255){fb=255;}
			fbuffer[addr+0]=fr;fbuffer[addr+1]=fg;fbuffer[addr+2]=fb;fbuffer[addr+3]=255;
		}
	}
	if (Vmodel>0){Blitter(2);}
	//Interrupt stuff
	if (scanid==irqpos){vblank=1;}
	if (scanid>irqpos-1 && intline==1 && vblank==1){interrupt=1;}
	if (scanid==iscancur && iscan>0 && (iscancurrl>0 && iscancurrl<irqpos+32)){hinterrupt=1;}
	if (scanid>iscancur-1 && iscan>0 && hinterrupt>0){interrupt=1;}
}
