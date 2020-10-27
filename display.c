/*
 * Name:	display.c
 *
 * Author:	Steve Conklin
 *
 * Purpose:     This file contains all of the display routines for hanoi.
 *		This was done in order that other displays or graphics
 *		might be accomodated in the future. These routines all use
 *		only the standard 80x25 text screen on the PC, and use
 *		the extended character set. See display.h for details.
 *
 * History:	8-6-91		Creation
 *		8-7-91		More work
 *		8-8-91		Added the float stuff
 *
 */

#include "hanoi.h"
#include "display.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
// #include <dos.h>

/* global variables which all display routines might need */
static	int	bottom_row,	/* line to display the base on		*/
		tower_bot_row,	/* the row the bottom disk goes on	*/
		tower_top_row,	/* the top of the pole			*/
		float_row,	/* the row used to float disks		*/
		moves_row,	/* the row used to display move #	*/
		moves_col,	/* the column for the move #		*/
		tower_col[3],	/* the columns for each pole		*/
		numdisks,	/* the number of disks we're using 	*/
		text_len;	/* length of disk text with null	*/
char	buffer[81];		/* used for drawing the base		*/

/*  these are used to point to text string images of each of the disks.
 *  This greatly speeds display over building them each time a move
 *  is made. The disks are built by makedisk()
 */
static char	**disk;		/* array of pointers to strings		*/
static char	*empty;		/* pointer to the empty string		*/

extern	int	_wscroll;	/* system variable to disable scrolling */

/* ==================================================================== */

/*  makedisk builds a string (2*numdisks)+1 char long in the
 *  char array buffer, with a disk of size disksize in the middle.
 *  This way, all disks have the same length string representing them,
 *  which makes display easier. The disk string has a pole character
 *  (defined by POLE in display.h) in the center, with <disksize>
 *  DISK characters on each side. Both ends are padded with blanks.
 *  With disksize=0, it generates only a pole.
 */

void makedisk(char *buffer, int num, int disksize)
{
	int i;
	for(i=0;(i<(num-disksize));i++)	/* leading blanks	*/
	       *buffer++ = ' ';
	for(i=0;i<disksize;i++)		/* left half disk	*/
		*buffer++ = DISK;
	*buffer++ = POLE;		/* the pole		*/
	for(i=0;i<disksize;i++)		/* right half disk	*/
		*buffer++ = DISK;
	for(i=0;(i<(num-disksize));i++)	/* trailing blanks	*/
		*buffer++ = ' ';
	*buffer = '\0';			/* terminator		*/
}

/* ==================================================================== */

/*  close_display() returns the display to the mode it was in when we
 *  started, frees up allocated memory, and clears the screen.
 */

void close_display(void)
{
	if(disk)
	{
		free(disk[0]);
		free(disk);
	}
	if(empty)
		free(empty);
	textmode(LASTMODE);
	_wscroll = 1;
	_setcursortype(_NORMALCURSOR);
	clrscr();
}

/* ==================================================================== */

/* show_move displays the move number at the correct place */

void show_move(int move)
{
	gotoxy(moves_col,moves_row);
	cprintf("%d",move);
}

/* ==================================================================== */

/*  init_display() accepts the number of disks to be used, and initializes
 *  all of the display variables. It must be called before any other
 *  display function except max_disp_disks
 */

void init_display(int num)
{
	struct text_info ti;	/* struct to hold info about window */
	int i;

	numdisks = num;		/* save this in our private variable */
	text_len = (numdisks*2)+2; /* disk + pole + null */

	/* allocate space for storage of the disk strings */
	if((disk = (char **)malloc(numdisks*sizeof(char *))) == NULL)
	{
		printf("malloc failure 1 in init_display()\n");
		exit(1);
	}
	if((disk[0] = (char *)malloc(numdisks*text_len)) == NULL)
	{
		printf("malloc failure 2 in init_display()\n");
		exit(1);
	}
	if((empty = (char *)malloc(text_len)) == NULL)
	{
		printf("malloc failure 3 in init_display()\n");
		exit(1);
	}

	/* fill in the array of pointers */
	for(i=1;i<numdisks;i++)
		disk[i] = disk[0] + (i * text_len);

	/* build the disk strings */
	for(i=0;i<numdisks;i++)
		makedisk(disk[i],numdisks,i+1);
	makedisk(empty,numdisks,0);

	/* set up the location of the poles */
	/* this could be calculated, but I assume an 80 column display */
	tower_col[0] = 14;
	tower_col[1] = 40;
	tower_col[2] = 66;

	for(i=0;i<80;i++)				/* build base */
		buffer[i] = BASE;
	for(i=0;i<3;i++)				/* and poles */
		buffer[tower_col[i]-1] = BASEWPOLE;
	buffer[80] = '\0';           			/* and terminator */

	textmode(BW80);			/* set display to 80x25 */
	gettextinfo(&ti);		/* get the display info	*/
	bottom_row = ti.screenheight;	/* bottom row on screen */
	tower_bot_row = bottom_row - 1;	/* lowest row on towers	*/
	tower_top_row = tower_bot_row - num; /* top of pole	*/
	float_row = tower_top_row - 2;	/* for animated display	*/
	_wscroll = 0;			/* turn of scrolling	*/
	_setcursortype(_NOCURSOR);	/* hide the cursor	*/

	/* clear screen and display the text */
	clrscr();
	gotoxy(1,bottom_row);
	cprintf("%s",buffer);
	gotoxy(31,1);
	cprintf("The Towers of Hanoi");
	gotoxy(28,2);
	cprintf("Programmer: Steve Conklin");
	gotoxy(37,5);
	cprintf("Moves: ");
	moves_col = wherex(); /* save the location for the moves */
	moves_row = wherey();
	cprintf("0");
}

/* ==================================================================== */

/*  max_disp_disks() returns the maximum number of disks that the display
 *  can handle. Since I assume an 80 column screen, I hard-coded to 12.
 */

int	max_disp_disks(void)
{
	return(12);
}

/* ==================================================================== */

/* press_msg() displays the message asking the user to press a key */

void press_msg(void)
{
	gotoxy(10,8);
	cprintf("Press any key to continue.");
}

/* ==================================================================== */

/* remove_disk removes a disk from a pole by drawing a blank disk */

void remove_disk(int tower, int height)
{
	/* height starts at 0 for lowest row */
	gotoxy(tower_col[tower]-numdisks,tower_bot_row-height);
	cputs(empty);
}

/* ==================================================================== */

/* put_disk() draws a disk on a tower at the given height */

void put_disk(int tower, int height, int size)
{
	gotoxy(tower_col[tower]-numdisks,tower_bot_row-height);
	cputs(disk[size-1]);
}

/* ==================================================================== */

/*  float_disk moves a disk by floating it up the pole, over to the
 *  new pole, and down again. The disk is moved from tower fr_tow
 *  and height fr_h to tower to_tow and height to_h. The speed of
 *  the movement is governed by VDEL and HDEL which are defined in
 *  display.h
 */

void float_disk(int fr_tow, int to_tow, int fr_h, int to_h)
{
	int	fr_col;	/* from column, used to track location	*/
	int	to_col;	/* to column				*/
	int	len;	/* length of text to move		*/
	int	dir;	/* direction to float disk		*/

	len = (2*numdisks)+1;	/* the length of the text to move */

	/* set up the direction to move (left or right) */
	if((fr_tow==0)||(to_tow==2))
		dir = RIGHT;		/* to the right */
	else
		dir = LEFT;		/* to the left	*/

	/* calculate actual screen locations */
	fr_col = tower_col[fr_tow] - numdisks;
	to_col = tower_col[to_tow] - numdisks;
	fr_h = tower_bot_row-fr_h;
	to_h = tower_bot_row-to_h;

	/* move the disk up to the float row */
	while(fr_h >= tower_top_row)
	{
		delay(VDEL);
		movetext(fr_col,fr_h,fr_col+len,fr_h,fr_col,fr_h-1);
		gotoxy(fr_col,fr_h);
		puts(empty);
		fr_h--;
	}
	/* we are just above the pole, so remove the pole char */
	gotoxy(fr_col+numdisks,fr_h);
	putch(' ');	/* make a hole in the disk */
	while(fr_h > float_row)
	{
		delay(VDEL);
		movetext(fr_col,fr_h,fr_col+len,fr_h,fr_col,fr_h-1);
		gotoxy(fr_col,fr_h);
		puts(empty);
		gotoxy(fr_col+numdisks,fr_h);
		putch(' ');
		fr_h--;
	}

	/* move the disk over the destination pole */
	while(fr_col != to_col)
	{
		delay(HDEL);
		/* index fudges on next lines prevent leftover chars */
		if(dir == RIGHT) /* moving right */
			movetext(fr_col-1,fr_h,fr_col+len,fr_h,
				fr_col,fr_h);
		else      /* moving left */
			movetext(fr_col,fr_h,fr_col+len+1,fr_h,
				fr_col-1,fr_h);
		fr_col += dir;
	}

	/* lower the disk to the correct height */
	while(fr_h < tower_top_row)
	{
		delay(VDEL);
		movetext(fr_col,fr_h,fr_col+len,fr_h,fr_col,fr_h+1);
		gotoxy(fr_col,fr_h);
		puts(empty);
		gotoxy(fr_col+numdisks,fr_h);
		putch(' ');
		fr_h++;
	}
	/* put the pole char back into the disk */
	gotoxy(to_col+numdisks,fr_h);
	putch(POLE);
	while(fr_h < to_h)
	{
		delay(VDEL);
		movetext(fr_col,fr_h,fr_col+len,fr_h,fr_col,fr_h+1);
		gotoxy(fr_col,fr_h);
		puts(empty);
		fr_h++;
	}
}

/* ==================================================================== */

/*  show_towers draws all three towers on the screen */

void show_towers(stack tower[])
{
	int	start_col;	/* beginning of text for a tower */
	int	i,j;		/* loop counters */
	int	row;		/* loop counter */
	int	disksize;	/* size of disk to draw */

	for(i=0;i<3;i++)	/* for each tower */
	{
		start_col = tower_col[i] - numdisks;
		/* draw any disks that are on the tower */
		for(j=0,row=tower_bot_row;j<tower[i].top;j++,row--)
		{
			disksize = tower[i].layer[j];
			gotoxy(start_col,row);
			cputs(disk[disksize-1]);
		}
		/* and extend the pole to the top */
		while(row >= tower_top_row)
		{
			gotoxy(start_col,row--);
			cputs(empty); /* blank lines with pole */
		}
	}
}
