#include <Cls.h>

int
main(int argc, char **argv)
{
  Cls ls;

  ls.process_args(argc, argv);

  ls.exec();

  return 0;
}
