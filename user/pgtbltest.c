#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/riscv.h"
#include "kernel/memlayout.h"
#include "user/user.h"

void
ugetpid_test()
{
  int i;
  printf("ugetpid_test starting\n");
  
  if (getpid() != ugetpid()) {
    printf("ugetpid() failed in initial process: getpid()=%d, ugetpid()=%d\n", getpid(), ugetpid());
    exit(1);
  }

  for (i = 0; i < 64; i++) {
    int ret = fork();
    if (ret != 0) {
      wait(&ret);
      if (ret != 0)
        exit(1);
      continue;
    }
    if (getpid() != ugetpid()) {
      printf("ugetpid() failed: getpid()=%d, ugetpid()=%d\n", getpid(), ugetpid());
      exit(1);
    }
    exit(0);
  }
  printf("ugetpid_test: OK\n");
}

int
main(int argc, char *argv[])
{
  printf("pgtbltest: start\n");
  ugetpid_test();
  printf("pgtbltest: OK\n");
  exit(0);
}
