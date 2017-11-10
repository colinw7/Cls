#include <Cls.h>

int
main(int argc, char **argv)
{
  // init locale aware routines
  setlocale(LC_ALL, "");

  Cls ls;

  ls.processArgs(argc, argv);

  if (! ls.exec())
    return 1;

  return 0;
}
