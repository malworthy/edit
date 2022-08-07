/*  TEXT EDITOR - By Malcolm Worthy
	Start Date: Feb 1999 */

#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <alloc.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include "edit.h"

#define true -1
#define false 0
#define MAX_WIDTH 150
#define SCREEN_COLS 79
#define PAGE_LENGTH 23
#define EOF_CHAR -1
#define TOP_PAGE 1

char *text;
char *startPointer;
char *startBlock;
char *endBlock;
void editor();
void printChar(char ch);
void cursOff();
void cursOn();
char charUnderCursor(int row, int col);
void printAscii(int row,int col,int scroll_Y);
char *addressUnderCursor(int row, int col);

void printChar(char ch)
{
	/* prints a character on the screen - faster than turbo C routines */
	/*putch(ch);*/

	union REGS r;

	r.h.ah=14;
	/*r.h.bh=0;
	r.h.bl=1;*/
	r.h.al=ch;
	int86(0x10,&r,&r);
}

main(int argc,char *argv[])
{
	if(!(text=(char *)malloc(10000*MAX_WIDTH)))
	{
		printf("Not enough memory");
		exit(1);
	}
	startPointer=text;
	load(argv[1]);
	textbackground(BLACK);
	textcolor(LIGHTGRAY);
	clrscr();
	printScreen(1,1);
	editor();
	free(startPointer);
}

displayFooter()
{
	gotoxy(1,25);
	textbackground(RED);
	textcolor(YELLOW);
	cprintf("                                                                               ");
	textbackground(BLACK);
	textcolor(LIGHTGRAY);
}

void editor()
{
	char ch;
	char ctrlKey;
	int x=1; /* x position of cursor on screen */
	int y=1; /* y position of cursor on screen */
	int scroll_X=1;
	int scroll_Y=1;
	int row=1;
	int col=1;
	int theLastCol;
	char dontExit=true;
	char reachedEnd=false;
	char charUnderCurs;
	int lastLine;
	char menuopt[2];

	header();
	displayFooter();
	while(dontExit)
	{
		printAscii(row,col,scroll_Y);
		gotoxy(x,y+TOP_PAGE);
		ch=getch();
		if(ch==0) ctrlKey=getch(); else ctrlKey=0;
	/* ***** Process keys ***** */
		switch(ch)
		{
			/* ********* CTRL-B Start Block ********* */
			case 2: startBlock=addressUnderCursor(row-scroll_Y+1,col);
            	    printScreen(scroll_X);
					break;
			/* ********* CTRL-K End Block ********** */
			case 11: endBlock=addressUnderCursor(row-scroll_Y+1,col);
					 printScreen(scroll_X);
					 break;
			case 27: dontExit=0;

					 break;
			case 0:  break;
			case '\b' : if(row==1 && col==1) break;
						--x;
						theLastCol=lastCol(y-1);
            			backSpace(y,col);
						if(x<1)
						{
							y--;
							x=theLastCol;
						}
						printScreen(scroll_X);
						break;
			default: insertChar(y,col,ch); /*putch(ch);*/
					 printScreen(scroll_X);
					 if(ch==13) /* carrige return */
					 {
						y++;
						x=1;
					 }
					 else
						x++;
		}
	/* **** Process Control keys ***** */

		switch(ctrlKey)
		{
			/* ******** File menu ********* */
			case 33: mainMenu(1,menuopt);
					 break;
			case 0: break;
			/* ********* HOME ************ */
			case 71: x=1;
					 scroll_X=1;
					 printScreen(scroll_X);
					 break;
			/* ********** END ************ */
			case 79: x=lastCol(y);
					 break;
			/* ******** PAGE DOWN *********** */
			case 81: if(scroll(PAGE_LENGTH)) scroll_Y+=PAGE_LENGTH;
					 lastLine=printScreen(scroll_X);
					 if(lastLine>0) y=lastLine;
					 break;
			/* ******** PAGE UP *********** */
			case 73: if(scroll_Y>PAGE_LENGTH)
					 {
						scroll(-PAGE_LENGTH);
						scroll_Y-=PAGE_LENGTH;
					 }
					 else
					 {
						scroll_Y=1;
						text=startPointer;
					 }
					 printScreen(scroll_X);
					 break;
			/* ********* CTRL-HOME ******** */
			case 119: scroll_Y=1;
					  scroll_X=1;
					  y=1;
					  x=1;
					  text=startPointer;
					  printScreen(scroll_X);
					  break;
			/* ********* CTRL-END ********* */
			case 117: scroll_Y=gotoEnd();
					  y=PAGE_LENGTH;
					  scroll_X=1;
					  x=1;
					  printScreen(scroll_X);
					  break;
			/* ********* LEFT ARROW ******* */
			case 75: x--; /* left */
					 break;
			/* *********** RIGHT ARROW ************ */
			case 77: charUnderCurs=charUnderCursor(row-scroll_Y+1,col);
					 if(charUnderCurs!=EOF_CHAR) x++; /* right */
					 if(charUnderCurs=='\n')
					 {
						 y++;
						 x=1;
						 scroll_X=1;
						 printScreen(scroll_X);
					 }
					 break;
			/* *********** UP ARROW *********** */
			case 72: y--; /* up */
					 break;
			/* ************ DOWN ARROW ************* */
			case 80: if(charUnderCursor(row-scroll_Y+1,lastCol(y))!=EOF_CHAR) y++; /* down */
					 break;
		}
		/* ********** PROCESS THE PULL DOWN MENU ********** */
		if(menuopt[0]!=0)
		{
			switch(menuopt[0])
			{
				case 'M' :  switch(menuopt[1])
							{
								case 'G' : scroll_Y=gotoLine(row);
										   printScreen(scroll_X);
										   break;
								case 'W' : wordCount();
										   break;
							}
							break;
			}
			processMenu(menuopt);
			menuopt[0]=0;
			ch=0;
		}
		/* ********* END OF PROCESSING THE PULL DOWN MENU *********** */

		theLastCol=lastCol(y);
		if(x+scroll_X>theLastCol)
		{
			if(theLastCol<scroll_X) scroll_X=theLastCol-1;
			x=theLastCol-scroll_X+1;
			printScreen(scroll_X);
		}
		if(x<1) /* scroll page to left */
		{
			x=1;
			scroll_X--;
			if(scroll_X<1)
			{
				scroll_X=1;
				y--;
				if(y>0) x=lastCol(y);
				if(x>SCREEN_COLS)
				{
					scroll_X=x-SCREEN_COLS+1;
					x=SCREEN_COLS;
					printScreen(scroll_X);
				}
			}
			else
			{
				printScreen(scroll_X);
			}
		}
		if(x>SCREEN_COLS+1)
		{
			scroll_X=x-SCREEN_COLS+1;
			x=SCREEN_COLS;
			printScreen(scroll_X);
		}
		if(x==SCREEN_COLS+1) /* this means you are scrolling with arrow keys */
		{
			/*scroll_X=SCREEN_COLS-x+1;*/
			x=SCREEN_COLS;
			scroll_X++;
			printScreen(scroll_X);
		}
		if(y>PAGE_LENGTH)
		{
			y=PAGE_LENGTH;
			scroll_Y++;
			if(scroll_Y<1) scroll_Y=1;
			scroll(1);
			printScreen(scroll_X);
		}
		if(y<1)
		{
			reachedEnd=false;
			y=1;
			scroll_Y--;
			if(scroll_Y<1) scroll_Y=1; else scroll(-1);
			printScreen(scroll_X);
		}
		col=scroll_X+x-1;
		row=scroll_Y+y-1;
	}
}
processMenu(char *menuopt)
{
	switch(menuopt[0])
	{
		case 'M' : processMiscMenu(menuopt[1]);
				   break;
	}
}
processMiscMenu(char choice)
{
	switch(choice)
	{
		case 'W' : wordCount();
	}
}

void printAscii(int row,int col,int scroll_Y)
{
	char ch;

	textcolor(YELLOW);
	textbackground(RED);
    gotoxy(5,25);
	cprintf("Row %d    Col %d   ",row,col);
	gotoxy(50,25);
	ch=charUnderCursor(row-scroll_Y+1,col);
	cprintf("ASCII: %d  %d  ",ch,lastCol(row-scroll_Y+1));
	textcolor(LIGHTGRAY);
	textbackground(BLACK);
}
int lastCol(int row)
{
	int rowCount=1;
	char *tempPointer;
	int col=1;

	tempPointer=text;

	while(rowCount<row)
	{
		if(*tempPointer=='\n') ++rowCount;
		++tempPointer;
	}
	while(*tempPointer!='\n' && *tempPointer!=EOF_CHAR)
	{
		tempPointer++;
		col++;
	}
	return col;
}
char charUnderCursor(int row, int col)
{
	int rowCount=1;
	char *tempPointer;
	tempPointer=text;

	while(rowCount<row)
	{
		if(*tempPointer=='\n') ++rowCount;
		++tempPointer;
	}
	col--;
	tempPointer+=col;
	return *tempPointer;
}
char *addressUnderCursor(int row, int col)
{
	int rowCount=1;
	char *tempPointer;
	tempPointer=text;

	while(rowCount<row)
	{
		if(*tempPointer=='\n') ++rowCount;
		++tempPointer;
	}
	col--;
	tempPointer+=col;
	return tempPointer;
}
insertChar(int row, int col, char ch)
{
	int rowCount=1;
	char *tempPointer;
	char lastChar;
	char thisChar;
	tempPointer=text;

	if(ch=='\r') ch='\n'; /* enter must be converted */
	while(rowCount<row)
	{
		if(*tempPointer=='\n') ++rowCount;
		++tempPointer;
	}
	col--;
	tempPointer+=col;
	lastChar=ch;
	/*while(*tempPointer>0)*/
	while(lastChar!=EOF_CHAR)
	{
		thisChar=*tempPointer;
		*tempPointer=lastChar;
		lastChar=thisChar;
		tempPointer++;
	}
	*tempPointer=lastChar;
}
backSpace(int row, int col)
{
	int rowCount=1;
	char *tempPointer;
	tempPointer=text;

	while(rowCount<row)
	{
		if(*tempPointer=='\n') ++rowCount;
		++tempPointer;
	}
	col--;
	tempPointer+=col;
	while(*tempPointer>0)
	{
		*(tempPointer-1)=*(tempPointer);
		++tempPointer;
	}
	*(tempPointer-1)=*tempPointer;
}

int scroll(int lines)
{
	int counter;
	char success=true;
	char *tempPointer;

	tempPointer=text;
	for(counter=0; counter<abs(lines); counter++)
	{
		if(lines>0)
		{
			while(*text!='\n')
			{
				++text;
				if(*text==EOF_CHAR) success=false;
			}
		}
		else
		{
			--text;
			--text;
			/*--text;*/
			while((*text!='\n') && text>=startPointer) text--;
		}
		++text;
	}
	if(success==false)
	{
		text=tempPointer;
		return false;
	}
	return success;
}

int gotoEnd()
{
	int counter;
	int numberLines=0;

	while(*text!=EOF_CHAR)
	{
		if(*text=='\n') numberLines++;
		++text;
	}
	for(counter=1; counter<=PAGE_LENGTH; counter++)
	{
		--text;
		--text;
		while((*text!='\n') && text>=startPointer) text--;
	}
	++text;
	return numberLines-PAGE_LENGTH+1;
}

int printScreen(int startCol)
{
	int row;
	int col=1;
	int screenLine=1;
	char temp;
	char lineText[MAX_WIDTH];
	char *startPointer;
	int i;
	int lastLine=0;

	gotoxy(1,1+TOP_PAGE);
	startPointer=text;
	cursOff();
	while(screenLine<=PAGE_LENGTH)
	{
		if(*text!='\n')
		{
			if(text>startBlock && text<endBlock)
			{
				textcolor(BLACK);
				textbackground(LIGHTGRAY);
			}
			else
			{
				textcolor(LIGHTGRAY);
				textbackground(BLACK);
			}
			if(*text!=-1)
			{
				if(col>=startCol && col<SCREEN_COLS+startCol) printChar(*text);
			}
			else
			{
				lastLine=screenLine;
				while(screenLine<=PAGE_LENGTH)
				{
					clreol();
					printChar('\n');
					++screenLine;
				}
				break;
			}
		}
		else
		{
			clreol();
			screenLine++;
			col=0;
			gotoxy(1,screenLine+TOP_PAGE);
		}
		text++;
		col++;
	}
	cursOn();
	text=startPointer;
	return lastLine;
}

int load(char *filename)
{
	FILE *fp;
	int counter=1;
	char string[MAX_WIDTH];
	char *startText;

	startText=text;
	if(!(fp=fopen(filename,"r")))
	{
		printf("Cannot open file\n");
		exit(1);
	}
	while(!feof(fp))
	{
		/*fgets(string,MAX_WIDTH,fp);
		strcpy(&text,string);
		printf(&text);
		text+=strlen(string);
		printf(&text);*/
		*text=(char)getc(fp);
		/*putch(*text);*/
		++text;
	}
	text=startText;
	printf(&text);
	return counter;
}
int gotoLine(int currentLine)
{
	void *mem;
	int lineNumber;
	int ok;

	mem=openWindow(20,10,60,14,"Goto Line",WHITE,BLUE);
	gotoxy(5,3);
	cprintf("     Enter line number: ");
	scanf("%d",&lineNumber);
	ok=scroll(lineNumber-currentLine);
	closeWindow(20,10,60,14,mem);
	if(ok)
		return lineNumber;
	else
		return currentLine;
}

wordCount()
{
	char *tempPointer;
	void *mem;
	long numWords=0;
	int newWord=0;
	long numLetters=0;
	long numSentence=0;
	long numLines=1;

	tempPointer=startPointer;
	while(*tempPointer!=EOF_CHAR)
	{
		if((*tempPointer>='A' && *tempPointer<='Z') || (*tempPointer>='a' && *tempPointer<='z'))
		{
			if(newWord!=2) newWord=1;
			numLetters++;
		}
		else
		{
			newWord=0;
		}
		if(newWord==1)
		{
			numWords++;
			newWord=2;
		}
		if(*tempPointer=='.') ++numSentence;
		if(*tempPointer=='\n') ++numLines;
		tempPointer++;
	}
	mem=openWindow(20,8,60,16,"Word Count",WHITE,BLUE);
	gotoxy(5,3);
	cprintf("     Number of Words: %d",numWords);
	gotoxy(5,4);
	cprintf("   Number of Letters: %d",numLetters);
	gotoxy(5,5);
	cprintf(" Number of sentences: %d",numSentence);
	gotoxy(5,6);
	cprintf("     Number of lines: %d",numLines);
/*	gotoxy(5,7);
	cprintf("Number of Paragraphs: %d",numPara);*/
	gotoxy(5,7);
	cprintf(" Average Word length: %3.1f",(float)numLetters/numWords);
	cursOff();
	getch();
	cursOn();
	closeWindow(20,8,60,16,mem);
}
