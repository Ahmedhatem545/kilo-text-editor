#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
void die (char *s){
   perror(s);
   exit(1);
}

struct termios orig_termios;
void disableRawMode() {
  // reset the terminal to cannonical mode.
 if ( tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios)==-1)
    die("tcsetattr");
}


void enableRawMode() {

  // reset terminal mode on exiting the program.
 if( tcgetattr(STDIN_FILENO, &orig_termios)==-1)
    die("tcgetattr");
  atexit(disableRawMode);
  // make a copy of orig_termios.
  struct termios raw = orig_termios;

  // editt the new made copy tor remove some flags.
  raw.c_iflag &= ~(ICRNL | IXON | BRKINT | INPCK | ISTRIP);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  // set the new edited attribute.
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw)==-1)
     die("tcsetattr");
}




int main() {

  enableRawMode();
  // print input to the terminal and quits if 'q' is pressed.
  while (1) {
        char c = '\0';
    if (read(STDIN_FILENO, &c , 1)==-1 && errno != EAGAIN)
       die("read");
    // check if the key pressed is one of the control keys.
   
    if (iscntrl(c)) {
      // print the decimal value of the key.
      printf("%d\r\n", c);
    } else {
      // if the key pressed is not a control key print the dcimal vaule with the
      // actual character pressed.
      printf("%d ('%c')\r\n", c, c);
    }
    if(c =='q' ) break ;
  }
  return 1;
}
