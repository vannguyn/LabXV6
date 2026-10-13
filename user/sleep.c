#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int
main(int argc, char *argv[])
{
  int ticks;

  if(argc != 2){
    fprintf(2, "sleep: expected one non-negative ticks argument\n");
    exit(1);
  }

  if(argv[1][0] == '-'){
    fprintf(2, "sleep: expected one non-negative ticks argument\n");
    exit(1);
  }

  ticks = atoi(argv[1]);

  printf("Sleeping for %d ticks\n", ticks);
  sleep(ticks);
  printf("Woke up!\n");
  exit(0);
}
