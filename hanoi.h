/*
 * Name:	hanoi.h
 *
 * Author:	Steve Conklin
 *
 * Purpose:     This file is the include file for the program hanoi.c
 *		It defines the structure used to represent the stacks
 *		of disks. Each stack of disks looks like this:
 *
 *			         |
 *			        1|1
 *			       22|22
 *			      333|333
 *			     4444|4444
 *			--------------------
 * 		and is represented by the stack structure.
 *		The stack structure shown on the assignment sheet stored
 *		the strings used to make up the disks, but I chose to
 *		store the size of the disks instead. This allows flexibility
 *		in the representation of the disks with text or graphics.
 *		The display routines are all isolated in display.c, and are
 *		described in display.h.
 *
 * History:	8-6-91		Creation
 *		8-7-91		More work
 *
 */

#define	MAXDISKS	15	/* max number of disks on a stack */
#define	DEFDISKS	4	/* default number of disks to solve for */

#define	SOURCE	0	/* these define names for the three stacks */
#define	TARGET	1	/* they represent indices for the towers */
#define	TEMP	2

/* structure defining the stacks */
typedef struct stacks {
	int layer[MAXDISKS]; /* contains the size of the disk at this layer */
	int top;	/* index of next layer to be filled */
} stack;

/* macros to perform stack functions */
#define STACK_EMPTY(x)	!(tower[(x)].top)  /* will be zero if !empty */

/* TOP_SIZE returns a number higher than any disk number if the stack
 * is empty. This is because we can put any size disk on an empty stack
 */
#define TOP_SIZE(x) (tower[(x)].top)?tower[(x)].layer[tower[(x)].top-1]:100

/* push disk size d onto stack s */
#define	push_stack(s,d)	tower[(s)].layer[(tower[(s)].top)++] = (d)

/* pop disk off stack s, return the disk size */
#define pop_stack(s) tower[(s)].layer[--(tower[(s)].top)]

/* show size of disk on top of stack */
#define TOPDISK(s) tower[(s)].layer[tower[(s)].top-1]

/* calculate the next stack number ( modulo 3 )
 * if dir is 1, counts up - if dir is 2, counts down
 */
#define AFTER(x)	(((x)+dir)%3)
