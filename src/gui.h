extern float mouse_x,mouse_y,mouse_bt,old_mouse_bt;
int pressme=0,lastsel=0;
int prassme=0,prussme=0;
color bgcol;
void ConnectKB(char * buff,int siz,int multiline);
int GetCurPos();
void SetCurPos(int pos);
char * GetCurbuff();

void ColorBox(vec uuc,vec ddc,color c1,color c2,color c3,color c4){
	glBindTexture(GL_TEXTURE_2D,blank);
	vec uc,dc;
	uc.x=(uuc.x/512)-1;uc.y=0-((uuc.y/384)-1);
	dc.x=(ddc.x/512)-1;dc.y=0-((ddc.y/384)-1);

	glBegin (GL_TRIANGLES);
		glColor4f(c1.r,c1.g,c1.b,c1.a);glVertex2f(uc.x,uc.y);
		glColor4f(c2.r,c2.g,c2.b,c2.a);glVertex2f(dc.x,uc.y);
		glColor4f(c3.r,c3.g,c3.b,c3.a);glVertex2f(dc.x,dc.y);
		glColor4f(c1.r,c1.g,c1.b,c1.a);glVertex2f(uc.x,uc.y);
		glColor4f(c3.r,c3.g,c3.b,c3.a);glVertex2f(dc.x,dc.y);
		glColor4f(c4.r,c4.g,c4.b,c4.a);glVertex2f(uc.x,dc.y);
	glEnd();
}


void DrawButton(vec uc,vec dc,int pressed){
	glBindTexture(GL_TEXTURE_2D,blank);
	vec op1,op2;
	color dark=cset(0,0,0,0.5),light=cset(1,1,1,0.5),mid=cset(0.5,0.5,0.5,0);
	
	if (pressed==0){
		op1=vset(uc.x+2,uc.y);op2=vset(dc.x-3,uc.y+3);ColorBox(op1,op2,mid,mid,dark,dark);
		op1=vset(uc.x+3,uc.y+3);op2=vset(dc.x-2,uc.y+6);ColorBox(op1,op2,light,light,mid,mid);
		op1=vset(uc.x+2,dc.y-6);op2=vset(dc.x-3,dc.y-3);ColorBox(op1,op2,mid,mid,dark,dark);
		op1=vset(uc.x+3,dc.y-3);op2=vset(dc.x-2,dc.y);ColorBox(op1,op2,light,light,mid,mid);
		op1=vset(uc.x,uc.y+3);op2=vset(uc.x+3,dc.y-4);ColorBox(op1,op2,mid,dark,dark,mid);
		op1=vset(uc.x+3,uc.y+3);op2=vset(uc.x+6,dc.y-4);ColorBox(op1,op2,light,mid,mid,light);
		op1=vset(dc.x-6,uc.y+3);op2=vset(dc.x-3,dc.y-4);ColorBox(op1,op2,mid,dark,dark,mid);
		op1=vset(dc.x-3,uc.y+3);op2=vset(dc.x,dc.y-4);ColorBox(op1,op2,light,mid,mid,light);
	}
	if (pressed==1){
		op1=vset(uc.x+2,uc.y);op2=vset(dc.x-3,uc.y+3);ColorBox(op1,op2,mid,mid,dark,dark);
		op1=vset(uc.x+2,uc.y+3);op2=vset(dc.x-3,uc.y+10);ColorBox(op1,op2,dark,dark,mid,mid);
		op1=vset(uc.x+3,dc.y-3);op2=vset(dc.x-2,dc.y);ColorBox(op1,op2,light,light,mid,mid);
		op1=vset(uc.x,uc.y+3);op2=vset(uc.x+3,dc.y-4);ColorBox(op1,op2,mid,dark,dark,mid);
		op1=vset(uc.x+3,uc.y+3);op2=vset(uc.x+10,dc.y-4);ColorBox(op1,op2,dark,mid,mid,dark);
		op1=vset(dc.x-3,uc.y+3);op2=vset(dc.x,dc.y-3);ColorBox(op1,op2,light,mid,mid,light);
	}
	if (pressed==2){
		op1=vset(uc.x+3,uc.y);op2=vset(dc.x-2,uc.y+6);ColorBox(op1,op2,light,light,mid,mid);
		op1=vset(uc.x+2,dc.y-6);op2=vset(dc.x-3,dc.y);ColorBox(op1,op2,mid,mid,dark,dark);
		op1=vset(uc.x,uc.y+3);op2=vset(uc.x+6,dc.y-4);ColorBox(op1,op2,light,mid,mid,light);
		op1=vset(dc.x-6,uc.y+3);op2=vset(dc.x,dc.y-4);ColorBox(op1,op2,mid,dark,dark,mid);
		op1=vset(uc.x,uc.y);op2=vset(uc.x+3,uc.y+3);ColorBox(op1,op2,mid,light,light,light);
		op1=vset(dc.x-3,uc.y);op2=vset(dc.x,uc.y+3);ColorBox(op1,op2,light,mid,mid,dark);
		op1=vset(dc.x-3,dc.y-4);op2=vset(dc.x,dc.y);ColorBox(op1,op2,dark,mid,mid,dark);
		op1=vset(uc.x,dc.y-4);op2=vset(uc.x+3,dc.y);ColorBox(op1,op2,light,dark,mid,mid);

	}
	Color(1,1,1,1);
}

void RunButton(vec uc,vec dc,std::string text,int * memory){
	if (mouse_x>uc.x && mouse_x<dc.x && mouse_y>uc.y && mouse_y<dc.y && mouse_bt==1 && old_mouse_bt==0){*memory=1;}
	if (mouse_bt==0){*memory=0;}
	int tx=((uc.x+dc.x)*0.5)-((text.size()*texsiz)*0.5),ty=((uc.y+dc.y)*0.5)-(texsiz*0.5);//tx+=uc.x;ty+=uc.y;
	if (*memory==1){
		DrawButton(uc,dc,1);
		Color(1,1,1,0.5);ABLocate(tx-1,ty-1);PrintT(text);Color(0,0,0,0.7);ABLocate(tx,ty);PrintT(text);
	}else{
		DrawButton(uc,dc,0);
		Color(1,1,1,0.5);ABLocate(tx-2,ty-2);PrintT(text);Color(0,0,0,0.7);ABLocate(tx-1,ty-1);PrintT(text);
	}
}

void RunToggle(vec uc,vec dc,std::string text,int * memory){
	if (mouse_x>uc.x && mouse_x<dc.x && mouse_y>uc.y && mouse_y<dc.y && mouse_bt==1 && old_mouse_bt==0){*memory^=1;}
	int tx=((uc.x+dc.x)*0.5)-((text.size()*texsiz)*0.5),ty=((uc.y+dc.y)*0.5)-(texsiz*0.5);//tx+=uc.x;ty+=uc.y;
	if (*memory==1){
		DrawButton(uc,dc,1);
		Color(1,1,1,0.5);ABLocate(tx-1,ty-1);PrintT(text);Color(0,0,0,0.7);ABLocate(tx,ty);PrintT(text);
	}else{
		DrawButton(uc,dc,0);
		Color(1,1,1,0.5);ABLocate(tx-2,ty-2);PrintT(text);Color(0,0,0,0.7);ABLocate(tx-1,ty-1);PrintT(text);
	}
}

void RunRadio(vec uc,vec dc,std::string text,int * memory,int id){
	if (mouse_x>uc.x && mouse_x<dc.x && mouse_y>uc.y && mouse_y<dc.y && mouse_bt==1 && old_mouse_bt==0 && *memory!=id){*memory=id;}
	int tx=((uc.x+dc.x)*0.5)-((text.size()*texsiz)*0.5),ty=((uc.y+dc.y)*0.5)-(texsiz*0.5);//tx+=uc.x;ty+=uc.y;
	if (*memory==id){
		DrawButton(uc,dc,1);
		Color(1,1,1,0.5);ABLocate(tx-1,ty-1);PrintT(text);Color(0,0,0,0.7);ABLocate(tx,ty);PrintT(text);
	}else{
		DrawButton(uc,dc,0);
		Color(1,1,1,0.5);ABLocate(tx-2,ty-2);PrintT(text);Color(0,0,0,0.7);ABLocate(tx-1,ty-1);PrintT(text);
	}
}


void RunMenu(vec uc,vec dc,std::string text,int * memory, int *lastsel){
	char menu[32][32];
	int mnuct=0,texct=0,selit=0,jopen=0;
	color dark=cset(0,0,0,0.5),ldark=cset(0,0,0,0.3);
	vec op1,op2;vec op3,op4;
	for (int a=0;a<text.size();a++){if (text[a]=='|'){menu[mnuct++][texct++]=0;texct=0;}else{menu[mnuct][texct++]=text[a];}}menu[mnuct++][texct++]=0;
	if (mouse_x>uc.x && mouse_x<dc.x && mouse_y>uc.y && mouse_y<dc.y && mouse_bt==1 && old_mouse_bt==0 && *memory==0){*memory=1;jopen=1;}
//	int tx=((uc.x+dc.x)*0.5)-((strlen(menu[0])*texsiz)*0.5),ty=((uc.y+dc.y)*0.5)-(texsiz*0.5);tx+=uc.x;ty+=uc.y;
	int tx=((uc.x+dc.x)*0.5)-((strlen(menu[0])*texsiz)*0.5),ty=((uc.y+dc.y)*0.5)-(texsiz*0.5);
	if (*memory==1){
		DrawButton(uc,dc,1);
		Color(1,1,1,0.5);ABLocate(tx-1,ty-1);PrintT(menu[0]);Color(0,0,0,0.7);ABLocate(tx,ty);PrintT(menu[0]);
		op1=vset(uc.x,dc.y);op2=vset(uc.x+250,dc.y+((mnuct-1)*texsiz)+8);ColorBox(op1,op2,bgcol,bgcol,bgcol,bgcol);DrawButton(op1,op2,2);
		if (mouse_y>op1.y && mouse_y<op2.y-texsiz){
			selit=(mouse_y-op1.y)/texsiz;
			op3=vset(op1.x+4,op1.y+(selit*texsiz)+4);op4=vset(op2.x-4,op1.y+((selit+1)*texsiz)+4);ColorBox(op3,op4,ldark,ldark,dark,dark);
			if (mouse_bt==1 && old_mouse_bt==0 && jopen==0){*memory=0;*lastsel=selit+1;}
		}else{if (mouse_bt==1 && old_mouse_bt==0 && jopen==0){*memory=0;*lastsel=0;}}
		for (int a=1;a<mnuct;a++){
			tx=uc.x+4; ty=dc.y+((a-1)*texsiz)+4;
			Color(1,1,1,0.5);ABLocate(tx-1,ty-1);PrintT(menu[a]);Color(0,0,0,0.7);ABLocate(tx,ty);PrintT(menu[a]);
		}
	}else{
		DrawButton(uc,dc,0);
		Color(1,1,1,0.5);ABLocate(tx-2,ty-2);PrintT(menu[0]);Color(0,0,0,0.7);ABLocate(tx-1,ty-1);PrintT(menu[0]);
	}
}
int scmode=0;
void RunScroll(vec uc,vec dc,int * memory){
	int mode=0;
	float siz=32,lim=32;color dark=cset(0,0,0,0.5),mid=cset(0.5,0.5,0.5,0);
	float cpos=0;
	vec op1,op2;
	if (abs(uc.y-dc.y)>abs(uc.x-dc.x)){mode=1;}
	if (mode==0){
		//X scroll
		siz=dc.y-uc.y;
		lim=((dc.x-uc.x)-(siz*3))+8;
		cpos=(float(*memory)/1024)*lim;
		op1=vset(uc.x+siz-4,uc.y);op2=vset(dc.x-siz+4,dc.y);
		DrawButton(op1,op2,1);
		op1=vset(uc.x,uc.y);op2=vset(uc.x+siz,dc.y);DrawButton(op1,op2,0);
		op1=vset(dc.x-siz,uc.y);op2=vset(dc.x,dc.y);DrawButton(op1,op2,0);
		op1=vset((uc.x+(siz*2)-8)+cpos,uc.y+3);op2=vset((uc.x+siz*2)+2+cpos,dc.y-3);
		if ((lim-cpos)>2){ColorBox(op1,op2,dark,mid,mid,dark);}
		op1=vset(uc.x+siz+cpos,uc.y+3);op2=vset(((uc.x+siz*2)-6)+cpos,dc.y-3);
		ColorBox(op1,vset(((uc.x+siz*2)-8)+cpos,dc.y-3),bgcol,bgcol,bgcol,bgcol);
		DrawButton(op1,op2,2);
		if (mouse_x>uc.x && mouse_x<dc.x && mouse_y>uc.y && mouse_y<dc.y){
			if (mouse_bt==1 && old_mouse_bt==0){
				op1=vset(uc.x+siz-4,uc.y);op2=vset(dc.x-siz+4,dc.y);if (mouse_x>op1.x && mouse_x<op2.x && mouse_y>op1.y && mouse_y<op2.y){scmode=1;}
				op1=vset(uc.x,uc.y);op2=vset(uc.x+siz,dc.y);if (mouse_x>op1.x && mouse_x<op2.x && mouse_y>op1.y && mouse_y<op2.y){scmode=2;}
				op1=vset(dc.x-siz,uc.y);op2=vset(dc.x,dc.y);if (mouse_x>op1.x && mouse_x<op2.x && mouse_y>op1.y && mouse_y<op2.y){scmode=3;}
				op1=vset(uc.x+siz+cpos,uc.y+3);op2=vset(((uc.x+siz*2)-6)+cpos,dc.y-3);if (mouse_x>op1.x && mouse_x<op2.x && mouse_y>op1.y && mouse_y<op2.y){scmode=4;}
			}
			if (mouse_bt==1){
				float mopo=(((mouse_x-siz*0.5)-(uc.x+siz))/lim)*1024;
				if (scmode==1){if (abs(*memory-mopo)>siz){if (mopo>*memory){*memory+=64;}else{*memory-=64;}}}
				if (scmode==2){*memory-=8;}if (scmode==3){*memory+=8;}
				if (scmode==4){*memory=mopo;}
			}
			if (*memory<0){*memory=0;}
			if (*memory>1023){*memory=1023;}
		}
	}else{
		//Y scroll
		siz=dc.x-uc.x;lim=((dc.y-uc.y)-(siz*3))+8;
		cpos=(float(*memory)/1024)*lim;
		op1=vset(uc.x,uc.y+siz-4);op2=vset(dc.x,dc.y-siz+4);
		DrawButton(op1,op2,1);
		op1=vset(uc.x,uc.y);op2=vset(dc.x,uc.y+siz);DrawButton(op1,op2,0);
		op1=vset(uc.x,dc.y-siz);op2=vset(dc.x,dc.y);DrawButton(op1,op2,0);
		op1=vset(uc.x+3,(uc.y+(siz*2)-8)+cpos);op2=vset(dc.x-3,(uc.y+siz*2)+2+cpos);
		if ((lim-cpos)>2){ColorBox(op1,op2,dark,dark,mid,mid);}
		op1=vset(uc.x+3,uc.y+siz+cpos);op2=vset(dc.x-3,((uc.y+siz*2)-6)+cpos);
		ColorBox(op1,vset(dc.x-3,((uc.y+siz*2)-8)+cpos),bgcol,bgcol,bgcol,bgcol);
		DrawButton(op1,op2,2);
		if (mouse_x>uc.x && mouse_x<dc.x && mouse_y>uc.y && mouse_y<dc.y){
			if (mouse_bt==1 && old_mouse_bt==0){
				op1=vset(uc.x,uc.y+siz-4);op2=vset(dc.x,dc.y-siz+4);if (mouse_x>op1.x && mouse_x<op2.x && mouse_y>op1.y && mouse_y<op2.y){scmode=1;}
				op1=vset(uc.x,uc.y);op2=vset(dc.x,uc.y+siz);if (mouse_x>op1.x && mouse_x<op2.x && mouse_y>op1.y && mouse_y<op2.y){scmode=2;}
				op1=vset(uc.x,dc.y-siz);op2=vset(dc.x,dc.y);if (mouse_x>op1.x && mouse_x<op2.x && mouse_y>op1.y && mouse_y<op2.y){scmode=3;}
				op1=vset(uc.x+siz+cpos,uc.y+3);op2=vset(((uc.x+siz*2)-6)+cpos,dc.y-3);if (mouse_x>op1.x && mouse_x<op2.x && mouse_y>op1.y && mouse_y<op2.y){scmode=4;}
			}
			if (mouse_bt==1){
				float mopo=(((mouse_y-siz*0.5)-(uc.y+siz))/lim)*1024;
				if (scmode==1){if (abs(*memory-mopo)>siz){if (mopo>*memory){*memory+=64;}else{*memory-=64;}}}
				if (scmode==2){*memory-=8;}if (scmode==3){*memory+=8;}
				if (scmode==4){*memory=mopo-(siz*0.5);}
			}
			if (*memory<0){*memory=0;}
			if (*memory>1023){*memory=1023;}
		}
	}
}

void RunSignedBar(vec uc,vec dc,int * memory){
	vec op1,op2,op3,op4;
	DrawButton(uc,dc,1);
	color black=cset(0,0,0,1),gray=cset(0.8,0.8,0.8,1),white=cset(1,1,1,1);
	op1=vset(uc.x+5,uc.y+5);op2=vset(dc.x-4,dc.y-4);
	ColorBox(op1,op2,black,black,black,black);
	float cet=(op2.x-op1.x)/32,ct=0,mid=(op1.y+op2.y)*0.5;
	float eet=(op2.y-op1.y)/256.0;
	for (int a=0;a<32;a++){
		op3=vset(ct+op1.x,mid);op4=vset(ct+cet+op1.x,(mid+3+((memory[a])*eet)));
		ColorBox(op3,op4,white,gray,gray,gray);
		ct+=cet;
	}
	if (mouse_bt==1){
		if (mouse_x>op1.x && mouse_x<op2.x && mouse_y>op1.y && mouse_y<op2.y){
			int mox=(mouse_x-op1.x)/cet,moy=((mouse_y-op1.y)/eet)-128;
			if (abs(moy)<10){moy=0;}
			memory[mox]=moy;
		}
	}
}
void RunUnSignedBar(vec uc,vec dc,int * memory){
		vec op1,op2,op3,op4;
	DrawButton(uc,dc,1);
	color black=cset(0,0,0,1),gray=cset(0.8,0.8,0.8,1),white=cset(1,1,1,1);
	op1=vset(uc.x+5,uc.y+5);op2=vset(dc.x-4,dc.y-4);
	ColorBox(op1,op2,black,black,black,black);
	float cet=(op2.x-op1.x)/32,ct=0,mid=op2.y;
	float eet=(op2.y-op1.y)/256.0;
	for (int a=0;a<32;a++){
		op3=vset(ct+op1.x,mid);op4=vset(ct+cet+op1.x,(mid-(memory[a])*eet));
		ColorBox(op3,op4,white,gray,gray,gray);
		ct+=cet;
	}
	if (mouse_bt==1){
		if (mouse_x>op1.x && mouse_x<op2.x && mouse_y>op1.y && mouse_y<op2.y){
			int mox=(mouse_x-op1.x)/cet,moy=256-((mouse_y-op1.y)/eet);
			if (abs(moy)<10){moy=0;}
			memory[mox]=moy;
		}
	}
}

void Run64SignedBar(vec uc,vec dc,int * memory){
	vec op1,op2,op3,op4;
	DrawButton(uc,dc,1);
	color black=cset(0,0,0,1),gray=cset(0.8,0.8,0.8,1),white=cset(1,1,1,1);
	op1=vset(uc.x+5,uc.y+5);op2=vset(dc.x-4,dc.y-4);
	ColorBox(op1,op2,black,black,black,black);
	float cet=(op2.x-op1.x)/64,ct=0,mid=(op1.y+op2.y)*0.5;
	float eet=(op2.y-op1.y)/256.0;
	for (int a=0;a<64;a++){
		op3=vset(ct+op1.x,mid);op4=vset(ct+cet+op1.x,(mid+3+((memory[a])*eet)));
		ColorBox(op3,op4,white,gray,gray,gray);
		ct+=cet;
	}
	if (mouse_bt==1){
		if (mouse_x>op1.x && mouse_x<op2.x && mouse_y>op1.y && mouse_y<op2.y){
			int mox=(mouse_x-op1.x)/cet,moy=((mouse_y-op1.y)/eet)-128;
			if (abs(moy)<10){moy=0;}
			memory[mox]=moy;
		}
	}
}
int textsel=0;
int RunTextBox(vec uc,vec dc,std::string * text){
	int ret=0;
	vec q1,q2;
	color black=cset(0,0,0,1);
	q1=vset(uc.x+4,uc.y+4);q2=vset(dc.x-4,dc.y-4);
	ColorBox(q1,q2,black,black,black,black);
	DrawButton (uc,dc,1);
	int ty=((uc.y+dc.y)*0.5)-(texsiz*0.5);
	ABLocate(q1.x,ty);Color(1,1,1,1);
	int off=0;
	if (text->size()>((dc.x-uc.x)/texsiz)){off=(text->size()-((dc.x-uc.x)/texsiz))+1;}
/*
	if (text==GetCurbuff()){
		PrintT(&text[off]);
		ABLocate(q1.x+GetCurPos()*texsiz,ty);Color(1,1,1,1);
		PrintT("_");
		ret=1;
	}else{
*/
	PrintT(text->substr(off));
	if (mouse_bt>0 && mouse_x>uc.x && mouse_x<dc.x && mouse_y>uc.y && mouse_y<dc.y){
//		ConnectKB(text,50,0);
//		SetCurPos(strlen(text));
	}
	return ret;
}
char cpltex[100];
void RunListbox(vec uc,vec dc,std::string text,int * memory,int * scmemory){
	vec q1,q2;
	color white=cset(1,1,1,1),notsowhite=cset(0.8,0.8,0.8,1),blue=cset(0,0,1,0.5);
	q1=vset(uc.x+4,uc.y+4);q2=vset(dc.x-34,dc.y-4);
	ColorBox(q1,q2,white,white,notsowhite,white);
	RunScroll(vset(dc.x-30,uc.y),dc,scmemory);
	int ct=1;
	int ln=text.size();
	for (int a=0;a<ln;a++){if (text[a]=='|'){ct++;}}
	ct-=(((q2.y-q1.y)/texsiz)-1);
	int absol=int(float(*scmemory)/((1024.0/float(ct))));
	if (*scmemory==0){absol=0;}
	int absel=*memory-absol;
	if (absel>-1 && absel<((q2.y-q1.y)/16)){ColorBox(vset(q1.x,q1.y+(absel*16)),vset(q2.x,q1.y+((absel+1)*16)),blue,blue,blue,blue);}
	DrawButton (uc,vset(dc.x-30,dc.y),1);
	int cata=0-absol;
	int ctt=0;
	Color(0,0,0,1);
	for (int a=0;a<ln;a++){
		if (text[a]=='|'){
			if (cata>-1 && cata<(q2.y-q1.y)/16){
				cpltex[ctt]=0;ABLocate(q1.x+4,(q1.y)+cata*16);
				if (ctt>(((dc.x-34)-uc.x)/texsiz)){cpltex[int(((dc.x-34)-uc.x)/texsiz)]=0;}PrintT(cpltex);
			}
			cata++;
			ctt=0;
		}else{
			cpltex[ctt++]=text[a];
		}
	}
	if (ctt>0){
		cpltex[ctt]=0;ABLocate(q1.x+4,(q1.y)+cata*16);PrintT(cpltex);
	}
	Color(1,1,1,1);
	if (mouse_x>q1.x && mouse_x<(q2.x-30) && mouse_y>q1.y && mouse_y<q2.y && mouse_bt==1){*memory=((mouse_y-(q1.y))/16)+absol;}
}
