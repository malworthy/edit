#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <graphics.h>
#include <dos.h>

const int bkCol99=BLACK;

char *right(char *string,int n)
{
/*	Copy of right$ in BASIC */
	return &string[strlen(string)-n];
}

char *left(char *string,int n)
{
/*	Copy of LEFT$ in BASIC */
	char s[80];
	strcpy(s,string);
	s[n]='\0';
	return s;
}

void aBox(int x1,int y1,int x2,int y2,int foreCol,int bakCol)
{
	const int border=8;

	setfillstyle(SOLID_FILL,bakCol);
	bar(x1+border,y1+border+2,x2+border,y2+border+2);
	setfillstyle(SOLID_FILL,foreCol);
	bar(x1,y1,x2,y2);
	setfillstyle(SOLID_FILL,bakCol);
	rectangle(x1+1,y1+1,x2-1,y2-1);
	rectangle(x1+3,y1+3,x2-3,y2-3);
}

char *itos(int n)
{
	char s[80];

	sprintf(s,"%d",n);
	return s;
}

void *openWindow(int x1,int y1,int x2,int y2, char *heading, int pen, int paper)
{
	void *mem;
	int i;

	if(x2>79) x2=29;
	if(y2>24) y2=24;

	mem=malloc((1+x2-x1)*(1+y2-y1)*4);              /* Free enough memory for window */
	gettext(x1,y1,x2+1,y2+1,mem);
	textcolor(DARKGRAY);
	textbackground(BLACK);
	for(i=y1+1; i<=y2+1; i++)
	{
		gotoxy(x2+1,i);
		putch(219);
	}
	for(i=x1+1; i<=x2+1; i++)
	{
		gotoxy(i,y2+1);
		putch(223);
	}
	window(x1,y1,x2,y2);
	textcolor(pen);
	textbackground(paper);
	clrscr();
	gotoxy(1,1);
	for(i=0; i<x2-x1; i++) putch(205);
	gotoxy(1,(y2-y1)+1);
	for(i=0; i<x2-x1; i++) putch(205);
	for(i=1; i<y2-y1; i++)
	{
		gotoxy(1,i+1);
		putch(186);
	}
	for(i=1; i<y2-y1; i++)
	{
		gotoxy(1+x2-x1,i+1);
		putch(186);
	}
	gotoxy(1,1);
	putch(201);
	gotoxy(1+x2-x1,1);
	putch(187);
	gotoxy(1,(y2-y1)+1);
	putch(200);
	gotoxy(1+x2-x1,(y2-y1)+1);
	/*putch(188);*/
	printf("%c",188);
	gotoxy(4,1);
	textcolor(YELLOW);
	cprintf("%s",heading);

	return mem;
}

void closeWindow(int x1,int y1,int x2,int y2,void *mem)
{
	if(x2>79) x2=29;
	if(y2>24) y2=24;

	puttext(x1,y1,x2+1,y2+1,mem);
	window(1,1,80,25);
	textbackground(bkCol99);
	free(mem);
}

char input(char *s,int x,int y, int max)
{
    /* decent input routine, returns control key code
       can use del,backspace,left & right curs keys to edit text */

	int currentPos=0,endPos=0,a=0;
	char c=1;
	int ctrlChar=0;
	int insert=0; /* not used as yet */

    /*s[0]='\0';*/
	endPos=strlen(s);
	gotoxy(x,y); cprintf(s); gotoxy(x,y);
    while(c!=13 && c!=9 && c!=27 && ctrlChar!=59 && ctrlChar!=60
			&& ctrlChar!=72 && ctrlChar!=80 && ctrlChar!=68
			&& ctrlChar!=82)
    {
        ctrlChar=0;
        c=getch();
        if(c==0)
        {
            ctrlChar=getch();
		}
		if(ctrlChar==83 && currentPos<endPos) /* del key */
        {
			for(a=currentPos; a<endPos; a++)
                s[a]=s[a+1];
			endPos--;
            gotoxy(x,y);
            cprintf(s);cprintf(" ");
        }
		if(c==8 && currentPos>0) /* backspace key */
        {
			currentPos--;
			for(a=currentPos; a<endPos; a++)
                s[a]=s[a+1];
			endPos--;
            gotoxy(x,y);
            cprintf(s);cprintf(" ");
        }
		if(ctrlChar==75 && currentPos>0) currentPos--; /* cursor left */
		if(ctrlChar==77 && currentPos<endPos) currentPos++; /* cursor right */
		if(c>=32 && c<=127 && endPos<max)
		{
			if(insert) /* move characters > 1 */
			{
				for(a=endPos; a>=currentPos; a--)
					s[a+1]=s[a];
				s[currentPos+1]=s[currentPos];
			}
			s[currentPos]=c;
			currentPos++;
			if (insert) endPos++;
			if(currentPos>endPos) endPos=currentPos;
			s[endPos]='\0';
            gotoxy(x,y);
            cprintf(s);
        }
		gotoxy(x+currentPos,y);
    }
    if(c==27 || c==9 || c==13) return c;
    return ctrlChar;
}

void cursOff()
/* this function is very badly missing from TURBO C */
{
	union REGS r;
	r.h.ah=1;
	r.h.ch=1;
	r.h.cl=0;
    int86(0x10,&r,&r);
}

void cursOn()
/* this function is very badly missing from TURBO C */
{
	union REGS r;
	r.h.ah=1;
	r.h.ch=6;
	r.h.cl=7;
    int86(0x10,&r,&r);
}

