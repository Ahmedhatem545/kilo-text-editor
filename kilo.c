/*** includes ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

/*** defines ****/
#define CTRL_KEY(k) ((k) & 0x1f)
/*** data ***/

struct editorConfig {

struct termios orig_termios;
};

struct editorConfig E;

/*** terminal ****/
void die(char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
  // reset the terminal to cannonical mode.
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {

  // reset terminal mode on exiting the program.
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
    die("tcgetattr");
  atexit(disableRawMode);
  // make a copy of orig_termios.
  struct termios raw = E.orig_termios;

  // editt the new made copy tor remove some flags.
  raw.c_iflag &= ~(ICRNL | IXON | BRKINT | INPCK | ISTRIP);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  // set the new edited attribute.
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

char editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN)
      die("read");
  }
  return c;
}

/*** output ***/

void editorDrawRows(){
   int y ;
   for (y =0 ; y<24 ; y++){
      write(STDOUT_FILENO ,"~\r\n",3);
   }
}
void editorRefreshScreen() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  editorDrawRows();
  write(STDOUT_FILENO, "\x1b[H",3);
  
}

/*** input ***/
void editorProcessKeyPress() {
  char c = editorReadKey();
  switch (c) {
  case CTRL_KEY('q'):
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    exit(0);
    break;
  }
}

/*** init ***/
int main() {

  enableRawMode();
  // print input to the terminal and quits if 'q' is pressed.
  while (1) {
    editorRefreshScreen();
    editorProcessKeyPress();
  }
  return 1;
}
