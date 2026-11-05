#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/sysinfo.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  struct sysinfo info;

  if(argc > 1){
    fprintf(2, "usage: sysinfo (no arguments expected)\n");
    exit(1);
  }

  if(sysinfo(&info) < 0){
    fprintf(2, "sysinfo: call failed\n");
    exit(1);
  }

  printf("sysinfo: free memory    = %ld bytes (%ld KB / %ld MB)\n",
         info.freemem, info.freemem / 1024, info.freemem / (1024 * 1024));
  printf("sysinfo: active process = %ld\n", info.nproc);
  exit(0);
}
