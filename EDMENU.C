#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
/*
#include <graphics.h>
*/
#include <dos.h>
#include "edit.h"

char mainMenu(int top, char *option);
char menu(int x, int y, char **opt,int num, char *title);

header()
{
	textcolor(LIGHTCYAN);
	textbackground(BLUE);
	gotoxy(1,1);
	cprintf("File  Edit  Search  Misc                                                      ");
	textcolor(YELLOW);
	gotoxy(1,1);
	cprintf("F");
	gotoxy(7,1);
	cprintf("E");
	gotoxy(13,1);
	cprintf("S");
	gotoxy(21,1);
	cprintf("M");
}
int instr(char *s, char c)
{
	int i,l;

	l=strlen(s);
	for(i=0; i<l; i++)
		if(c==s[i]) return i+1;
	return 0;
}


char mainMenu(int top, char *option)
{

char *opt[8]=
{
"New                ",
"Open..             ",
"Save               ",
"save As..          ",
"Print              ",
"eXit and save      ",
"Exit and abandon   "
};

char *opt2[8]=
{
"mark Start of block    ",
"mark End of block      ",
"cuT                    ",
"Copy                   ",
"Paste                  ",
"Delete                 ",
"Unmark                 "
};

char *opt3[4]=
{
"Search               ",
"Replace              ",
"repeat Last search   "
};

char *opt4[8]=
{
"Goto line number   ",
"insert Bookmark    ",
"Jump to bookmark   ",
"Ascii table        ",
"Insert ascii char  ",
"Word count         ",
"Options            "
};

char *words[4]=
{
"File ",
"Edit ",
"Search ",
"Misc "
};

char choice=1;
char ch;
char string[1000];

/*ch=scroll(1,24,79,string);
/*top=instr("gtsb",ch);*/
cursOff();
while(choice==1 || choice==2)
{
	header();
	textcolor(WHITE);
	textbackground(BLACK);
	if(top==1)
	{
		gotoxy(1,1); cprintf(words[top-1]);
		choice=menu(1,2,opt,7,"");
	}
	if(top==2)
	{
		textcolor(WHITE);
		gotoxy(7,1); cprintf(words[top-1]);
		choice=menu(7,2,opt2,7,"");
	}
	if(top==3)
	{
		textcolor(WHITE);
		gotoxy(13,1); cprintf(words[top-1]);
		choice=menu(13,2,opt3,3,"");
	}
	if(top==4)
	{
		textcolor(WHITE);
		gotoxy(21,1); cprintf(words[top-1]);
		choice=menu(21,2,opt4,7,"");
	}
	if(choice==1) top--;
	if(choice==2) top++;
	if(top<1) top=4;
	if(top>4) top=1;
}
cursOn();
header();
if(top==1) option[0]='F';
if(top==2) option[0]='E';
if(top==3) option[0]='S';
if(top==4) option[0]='M';

option[1]=choice;
if(choice=='\x1B') option[0]=0; /* for some unknown reason, when esc is
								   pressed choice becomes '\x1B'.  I have
								   no idea what '\x1B' means. */
return choice;
}

char menu(int x, int y, char **opt,int num, char *title)
{
/* **** Displays a scrolling menu ****
    x = x position on screen
    y = y position on screen
    opt = string array of the possible option
    num = the number of options
    title = an optional title bar at the top of the window
*/
int i,l;
void *mem;
void *mem2;
char ch='@';
int pos=1;
int j;
char keyz;

int backCol=BLUE;
int letter;
char cap[80];
int capPos[80];

/*cap=malloc(num);*/

l=strlen(opt[0]);
for(i=0; i<num; i++)
{
	for(letter=0; letter<strlen(opt[i]); letter++)
	{
		if(opt[i][letter]<97 && opt[i][letter]>48)
		{
			cap[i]=opt[i][letter];
			capPos[i]=letter;
			break;
		}
	}
}
mem=openWindow(x,y,x+l+3,y+num+1,"",WHITE,backCol);

textcolor(LIGHTBLUE);
ch=0;
while(instr(cap,ch)==0 && ch!=27 && ch!=1 && ch!=2)
{
    for(j=0; j<num; j++)
    {
		gotoxy(3,2+j);
        if(pos==j+1)
        {
			textcolor(BLACK);
			textbackground(WHITE);
        }
        else
        {
			textbackground(backCol);
			textcolor(LIGHTCYAN);
        }
        cprintf("%s",opt[j]);
        textcolor(YELLOW);
		gotoxy(3+capPos[j],2+j); cprintf("%c",cap[j]);
    }

    textbackground(BLACK);
	textcolor(BLACK);
	keyz=0;
	gotoxy(2,25);
	ch=getch();
	if (ch>=97 && ch<=122) ch=ch-32;
	if(ch==0) keyz=getch();
	if(keyz==80)
    {
        pos++;
        if(pos>num) pos=1;
    }
	if(keyz==72)
    {
        pos--;
        if(pos<1) pos=num;
	}
	if(keyz==75) ch=1;
	if(keyz==77) ch=2;
    if(ch==13)
        ch=cap[pos-1];
}
closeWindow(x,y,x+l+3,y+num+1,mem);
/*free(cap);*/
return ch;
}



