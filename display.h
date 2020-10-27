



/*
 * Name:	display.h
 *
 * Author:	Steve Conklin
 *
 * Purpose:     This is the header file which contains the function
 *		prototypes for the display functions.
 *
 * History:	8-6-91		Creation
 *		8-7-91		More work
 *		8-8-91		added float_disk
 *
 */

/*  NOTE: For all display functions, towers are numbered 0 thru 2,
 *  and disk heights start with 0 as the lowest position in the pole.
 */

/* these control the speed of the animated disk movement */
#define VDEL	70		/* delay in mS for each vert float move */
#define HDEL	30		/* delay for horiz float move */

#define	LEFT	-1		/* direction for float movement	*/
#define	RIGHT	1

/*  These are the octal character codes for the text representations
 *  of various parts to be drawn. They are from the PC extended character
 *  set.
 */

#define BASE		'\315'	/* the baseplate for all the towers	*/
#define BASEWPOLE	'\317'	/* where the base and pole meet		*/
#define DISK		'\334'	/* the solid block for the disk		*/
#define POLE		'\263'	/* the pole character			*/

/*  init_display() accepts the number of disks to be used and initializes
 *  the display software and hardware. This function must be called
 *  before any other display function except max_disp_disks()
 */
 
void init_display(int num);

/*  show_move() displays the integer passed to it as text at the
 *  end of the "Moves: " on the screen. This is called to display
 *  the current move number.
 */

void show_move(int move);

/*  makedisk builds a disk image in a character array. num is the number
 *  of disks we are using, and disksize is the size of the disk we are
 *  building. The array must be (2*num)+2 characters long.
 */

void makedisk(char *buffer, int num, int disksize);





/*  show_towers() draws all three towers and the disks on them as
 *  represented by the tower data structure.
 */

void show_towers(stack tower[]);

/*  close_display() frees up memory allocated by the display functions
 *  and restores the display to the state it was in when we started.
 */

void close_display(void);

/*  press_msg() displays a message asking the user to press any key
 *  to continue
 */
 
void press_msg(void);

/*  remove_disk() removes a disk from a tower by writing a blank disk
 *  over it.
 */

void remove_disk(int tower, int height);

/*  put_disk() draws a disk at the given height on a given tower.
 */

void put_disk(int tower, int height, int size);

/*  float_disk() moves a disk by floating it up the pole, over to the
 *  new pole, and down again. The disk is moved from tower fr_tow
 *  and height fr_h to tower to_tow and height to_h. The speed of
 *  the movement is governed by VDEL and HDEL which are defined in
 *  display.h
 */

void float_disk(int fr_tow, int to_tow, int fr_h, int to_h);

/*  max_disp_disks returns the maximum number of disks that the
 *  display can handle
 */

int max_disp_disks(void);
