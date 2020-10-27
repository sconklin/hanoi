/*
 * Name:	hanoi.c
 *
 * Author:	Steve Conklin
 *
 * Purpose:     This is the main file for the hanoi program.
 *		It contains all of the code to create and manipulate
 *		the data structures for the towers, to make the moves,
 *		and handle user input, etc. All display function are
 *		in another module so that different displays, graphics,
 *		etc may be supported.
 *
 * History:	8-6-91		Creation
 *		8-7-91		Lot of work! done, needs clean-up
 *		8-8-91		Added float_disk stuff. Done!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include "hanoi.h"
#include "display.h"

/* =================================================================== */

/* initialize data structures for up to MAXDISKS disks */
void	init_stacks(stack *tower, int disks)
{
	int	i,j;	/* loop counters */

	/* check the value passed in */
	if((disks > MAXDISKS) || (disks <=0))
	{
		printf("Error in function init_stacks()\n");
		printf("requested number of disks = %d/n",disks);
		printf("MAXDISKS = %d\n",MAXDISKS);
		exit(1);
	}
	/* set up the two empty towers */
	tower[TARGET].top = tower[TEMP].top = 0;
	for(i=0;i<MAXDISKS;i++)
	{
		tower[TARGET].layer[i] = 0;
		tower[TEMP].layer[i] = 0;
	}
	/* set up the starting stack */
	tower[SOURCE].top = disks;
	for(i=0,j=disks;j>=0;i++,j--)
		tower[SOURCE].layer[i] = j;
	for(;i<MAXDISKS;i++)
		tower[SOURCE].layer[i] = 0;
}
/* ===================================================================== */

#ifdef HDEBUG
/*  function dump_tower(tow,x,y) - for debugging
 *  Dumps tower number tow at location x,y on the screen.
 *  Warning - this will probably have to change if graphics display
 * support is added. This should probably be in display.c
 */

void dump_tower(stack tow,int x,int y)
{
	int i;
	gotoxy(x,y++);
	cprintf("dump of tower");
	gotoxy(x,y+=2);
	cprintf("top = %d",tow.top);
	y++;
	for(i=0;i<MAXDISKS;i++,y++)
	{
		gotoxy(x,y);
		cprintf("tow.layer[%d] = %d",i,tow.layer[i]);
	}
}
#endif

/* ===================================================================== */

/*  print a usage message - accepts the maximum number of disks that
 *  can be accomodated
 */

void usage(int max)
{
	printf("\nhanoi - solves the towers of hanoi\n");
	printf("usage: hanoi [num_disks] [speed]\n\n");
	printf("where:\n\tnum_disks is the number of disks to solve for, ");
	printf("up to a maximum of %d.\n",max);
	printf("\tand speed is one of the following values:\n\n");
	printf("\t1 - No delay between moves, for large numbers of disks.\n");
	printf("\t2 - 1 second delay between moves\n");
	printf("\t3 - press a key to continue with each move\n");
	printf("\t4 - animated display - cute, but slow\n");
	printf("\nIf a number of disks is not specified, it defaults to %d\n",
		DEFDISKS);
	printf("If a speed is not specified, it defaults to 4\n");
}

/* ===================================================================== */

/* the user interrupt handler  - we come here if ^C hit */
int c_brk(void)
{
	close_display();
	printf("User Interrupt.\n");
	return(0);
}
/* ===================================================================== */

/* This is the workhorse */

main(int argc, char *argv[])

{
	unsigned long moves = 0L; /* the current number of moves made	*/
	int	disks;		/* the number of disks to be used	*/
	int	max_can_do;	/* smaller of MAXDISKS or display capability */
	int	smallon;	/* the tower which the smallest disk is on */
	int	tmp;		/* general purpose temp variable	*/
	int	a,b;		/* tower numbers to determine moves	*/
	int	asize,bsize;	/* for size of top disk on towers a,b	*/
	stack	tower[3];	/* the towers				*/
	int	dir;		/* determines cw or ccw order of towers */
				/* 1 for cw (even # of disks)		*/
				/* or 2 for ccw (odd # of disks)	*/
	int	speed = 0;	/* selects display method		*/
	/* these are used to keep track of what was done for display later */
	int	fr_tow;		/* tower the move was from		*/
	int	to_tow;		/* tower the move was to		*/
	int	fr_h;		/* height the move was from		*/
	int	to_h;		/* height the move was to		*/
	int	size_moved;	/* size of the disk moved		*/

	/* turn on the user interrupt handler */
	ctrlbrk(c_brk);
	setcbrk(1);

	tmp = max_disp_disks();	/* find out how many the display can handle */
	max_can_do = (MAXDISKS>tmp)?tmp:MAXDISKS; /* select the smaller */

	/* check the command line */
	switch(argc)
	{
		case 1:				/* use defaults	*/
			disks = DEFDISKS;
			break;
		case 3:				/* everything specified */
			speed = atoi(argv[2]);
		case 2:				/* # of disks specified */
			disks = atoi(argv[1]);
			break;
		default:			/* oops */
			usage(max_can_do);
			exit(1);
			break;
	}

	/* check the number of disks specified */
	if((disks<1) || (disks > max_can_do))
	{
		printf("Error in number of disks.\n");
		printf("Requested numer = %d\n",disks);
		printf("Must be in the range 1 <= disks <= %d\n\n",
			max_can_do);
		usage(max_can_do);
		exit(1);
	}

	/* check the speed specified */
	if((!speed) || (speed>4))
		speed = 4;	/* default to animated display */

	/*  in order to make sure the final stack ends up on the TARGET
	 *  peg, we have to set the direction depending on whether the
	 *  number of disks is odd or even - see the macro AFTER(x)
	 */
	if(disks & 1)	/* odd number of disks */
		dir = 1;	/* cw */
	else
		dir = 2;	/* ccw */

	/* initalize the data structures and display */
	init_stacks(tower,disks);
	init_display(disks);
	/* do the initial display and pause to give a good look */
	show_towers(tower);
	sleep(1);

	/* we start with the small disk on the SOURCE peg */
	smallon = 0;
	/* wait for a keypress if appropriate */
	if(speed == 3)
	{
		press_msg();
		while(!kbhit());
		getch();
	}

	/*  The algorithm used here is not recursive, but yields the
	 *  exact same sequence of moves. It was used to minimize
	 *  stack use and increase speed. This algorithm (as well as
	 *  the recursive implementation) may be found in "Data
	 *  Structures and Algorithms" by Aho, Hopcroft, and Ullman.
	 *  The algorithm is this:
	 *  On odd numbered moves - move the smallest disk one peg
	 *  clockwise (ccw for even).
	 *  On even numbered moves - make the only legal move not
	 *  involving the smallest disk.
	 *  By selecting CW or CCW movement, I make sure the final stack
	 *  always ends up on the TARGET tower.
	 */

	while(!(STACK_EMPTY(SOURCE) && STACK_EMPTY(TEMP)))
	{
		moves++;
		tmp = AFTER(smallon);	/* the next peg */
		if(moves & 1L)		/* it's an odd numbered move */
		{
			/* set up the variables for the display stuff */
			fr_tow = smallon;		/* from tower */
			to_tow = tmp;			/* to tower */
			fr_h = tower[fr_tow].top-1;	/* from height */
			to_h = tower[to_tow].top;	/* to height */
			size_moved = 1;			/* always */
			/* and move the small disk over one position */
			push_stack(tmp,pop_stack(smallon));
			smallon = tmp;			/* for next time */
		}
		else	/* it's an odd numbered move */
		{
			/* set a and b to the towers without the small disk */
			a = tmp;
			b = AFTER(tmp);
			/*  get the disk sizes - TOP_SIZE returns a large
			 *  number if the tower is empty, since a move to an
			 *  empty tower is always legal.
			 */
			asize = TOP_SIZE(a);
			bsize = TOP_SIZE(b);
			/* determine which way to move the disk */
			if(asize>bsize)
			{
				to_tow = a;
				fr_tow = b;
			}
			else
			{
				to_tow = b;
				fr_tow = a;
			}
			fr_h = tower[fr_tow].top-1;
			to_h = tower[to_tow].top;
			size_moved = TOP_SIZE(fr_tow);
			push_stack(to_tow,pop_stack(fr_tow));/* do the move */
		}
		show_move(moves);	/* display the move number */
		switch(speed)	/* select the display update method */
		{
			case 1:	/* fastest */
				remove_disk(fr_tow,fr_h);
				put_disk(to_tow,to_h,size_moved);
				break;
			case 2:	/* delay 1 sec for each move */
				remove_disk(fr_tow,fr_h);
				put_disk(to_tow,to_h,size_moved);
				sleep(1);
				break;
			case 3:	/* wait for ketpress */
				remove_disk(fr_tow,fr_h);
				put_disk(to_tow,to_h,size_moved);
				press_msg();
				while(!kbhit());
				getch();
				break;
			case 4:	/* animated display */
				float_disk(fr_tow,to_tow,fr_h,to_h);
				break;
			default: /* should never get here */
				printf("\007Error in case\n");
				break;
		}
	}
	/* we are done - press a key before exiting if needed */
	if(speed != 3)
	{
		press_msg();
		while(!kbhit());
		getch();
	}
	/* shut down the display and quit */
	close_display();
	return(0);
}
