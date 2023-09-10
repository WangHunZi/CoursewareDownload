/* Set terminal (tty) into "raw" mode: no line or other processing done
   Terminal handling documentation:
       curses(3X)  - screen handling library.
       tput(1)     - shell based terminal handling.
       terminfo(4) - SYS V terminal database.
       termcap     - BSD terminal database. Obsoleted by above.
       termio(7I)  - terminal interface (ioctl(2) - I/O control).
       termios(3)  - preferred terminal interface (tc* - terminal control).
*/

#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void tty_atexit(void);
int tty_reset(void);
void tty_raw(void);
int screenio(void);
void fatal(char *mess);

static struct termios orig_termios;  /* TERMinal I/O Structure */
static int ttyfd = STDIN_FILENO;     /* STDIN_FILENO is 0 by default */

int main()
   {
    /* check that input is from a tty */
    if (! isatty(ttyfd)) fatal("not on a tty");

    /* store current tty settings in orig_termios */
    if (tcgetattr(ttyfd,&orig_termios) < 0) fatal("can't get tty settings");

    /* register the tty reset with the exit handler */
    if (atexit(tty_atexit) != 0) fatal("atexit: can't register tty reset");

    tty_raw();      /* put tty in raw mode */
    screenio();     /* run application code */
    return 0;       /* tty_atexit will restore terminal */
   }


/* exit handler for tty reset */
void tty_atexit(void)  /* NOTE: If the program terminates due to a signal   */
{                      /* this code will not run.  This is for exit()'s     */
   tty_reset();        /* only.  For resetting the terminal after a signal, */
}                      /* a signal handler which calls tty_reset is needed. */

/* reset tty - useful also for restoring the terminal when this process
   wishes to temporarily relinquish the tty
*/
int tty_reset(void)
   {
    /* flush and reset */
    if (tcsetattr(ttyfd,TCSAFLUSH,&orig_termios) < 0) return -1;
    return 0;
   }


/* put terminal in raw mode - see termio(7I) for modes */
void tty_raw(void)
   {
    struct termios raw;

    raw = orig_termios;  /* copy original and then modify below */

    /* input modes - clear indicated ones giving: no break, no CR to NL, 
       no parity check, no strip char, no start/stop output (sic) control */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    /* output modes - clear giving: no post processing such as NL to CR+NL */
    raw.c_oflag &= ~(OPOST);

    /* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);

    /* local modes - clear giving: echoing off, canonical off (no erase with 
       backspace, ^U,...),  no extended functions, no signal chars (^Z,^C) */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    /* control chars - set return condition: min number of bytes and timer */
    raw.c_cc[VMIN] = 5; raw.c_cc[VTIME] = 8; /* after 5 bytes or .8 seconds
                                                after first byte seen      */
    raw.c_cc[VMIN] = 0; raw.c_cc[VTIME] = 0; /* immediate - anything       */
    raw.c_cc[VMIN] = 2; raw.c_cc[VTIME] = 0; /* after two bytes, no timer  */
    raw.c_cc[VMIN] = 0; raw.c_cc[VTIME] = 8; /* after a byte or .8 seconds */

    /* put terminal in raw mode after flushing */
    if (tcsetattr(ttyfd,TCSAFLUSH,&raw) < 0) fatal("can't set raw mode");
   }


/* Read and write from tty - this is just toy code!! 
   Prints T on timeout, quits on q input, prints Z if z input, goes up
   if u input, prints * for any other input character
*/
int screenio(void)
   {
    int bytesread;
    char c_in, c_out, up[]="\033[A";
    char eightbitchars[256];                  /* will not be a string! */

    /* A little trick for putting all 8 bit characters in array */
    {int i;  for (i = 0; i < 256; i++) eightbitchars[i] = (char) i; }

    for (;;)
       {bytesread = read(ttyfd, &c_in, 1 /* read up to 1 byte */);
        if (bytesread < 0) fatal("read error");
        if (bytesread == 0)        /* 0 bytes inputed, must have timed out */
           {c_out = 'T';           /* straight forward way to output 'T' */
            write(STDOUT_FILENO, &c_out, 1);
           }
        else switch (c_in)         /* 1 byte inputed */
           {case 'q' : return 0;   /* quit - no other way to quit - no EOF */
            case 'z' :             /* tricky way to output 'Z' */
                write(STDOUT_FILENO, eightbitchars + 'Z', 1);
                break;
            case 'u' :
            	write(STDOUT_FILENO, up, 3);  /* write 3 bytes from string */
            	break;
            default :
                c_out = '*';
                write(STDOUT_FILENO, &c_out, 1);
           }
       }
   }

void fatal(char *message)
   {
    fprintf(stderr,"fatal error: %s\n",message);
    exit(1);
   }
