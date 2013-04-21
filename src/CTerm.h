#ifndef CTERM_H
#define CTERM_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

extern "C" {
# include "termios.h"

  extern int tgetent(char *, char *);
  extern int tgetnum(char *);
};

namespace CTerm {
  bool getCharSize(int fd, int *rows, int *cols) {
    struct winsize ts;

    if (ioctl(fd, TIOCGWINSZ, &ts) == -1)
      return false;

    *rows = ts.ws_row;
    *cols = ts.ws_col;

    return true;
  }

  int getNumColumns() {
    static char term_buffer[2048];

    const char *term;

    if ((term = getenv("TERM")) == NULL)
      term = "vt100";

    int no = tgetent(term_buffer, (char *) term);

    if (no <= 0)
      return 0;

    int cols = tgetnum((char *) "cols");

    return cols;
  }

  bool getCharSize(int *rows, int *cols) {
    if (isatty(STDIN_FILENO)) {
      if (getCharSize(STDIN_FILENO, rows, cols))
        return true;
    }

#ifdef CURSES
    WINDOW *w;

    use_env(false);

    w = initscr();

    endwin();

    *rows = LINES;
    *cols = COLS;
#else
    *rows = 60;
    *cols = 0;

    char *columns;

    if ((columns = getenv("COLUMNS")) != NULL)
      *cols = atoi(columns);

    if (*cols == 0)
      *cols = getNumColumns();

    if (*cols == 0)
      *cols = 80;
#endif

    return false;
  }

  static const char *color_xterm_colors[] = {
    "[0m" , /* White   */
    "[31m", /* Red     */
    "[32m", /* Green   */
    "[33m", /* Yellow  */
    "[34m", /* Blue    */
    "[35m", /* Magenta */
    "[36m", /* Cyan    */
  };

  static const char *normal_xterm_colors[] = {
    "[0m", /* Normal    */
    "[4m", /* Underline */
    "[1m", /* Bold      */
    "[1m", /* Bold      */
    "[7m", /* Invert    */
    "[0m", /* Normal    */
    "[7m", /* Invert    */
  };

  const char **getColorStrs() {
    static const char **colors_ = NULL;

    if (colors_ == NULL) {
      const char *term;

      if ((term = getenv("TERM")) == NULL)
        term = "vt100";

      if      (strcmp(term, "xterm") == 0 || strcmp(term, "xterm-color") == 0) {
        bool colored = true;

        char *color_xterm;

        if ((color_xterm = getenv( "COLOR_XTERM")) == NULL &&
            (color_xterm = getenv("COLOUR_XTERM")) == NULL)
          colored = false;

        if (colored)
          colors_ = color_xterm_colors;
        else
          colors_ = normal_xterm_colors;
      }
      else
        colors_ = normal_xterm_colors;
    }

    return colors_;
  }

  const char *getColorStr(int color) {
    if (color < 0 || color > 6)
      return "";

    const char **colors = getColorStrs();

    return colors[color];
  }
}

#endif
