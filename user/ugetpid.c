#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid_sys = getpid();
  int pid_fast = ugetpid();

  printf("=== Testing accelerated system call ===\n");
  printf("  getpid() (standard syscall) = %d\n", pid_sys);
  printf("  ugetpid() (shared USYSCALL) = %d\n", pid_fast);

  if(pid_sys == pid_fast){
    printf("SUCCESS: PID values match! USYSCALL mapping is functioning properly.\n");
    exit(0);
  } else {
    printf("FAILURE: PID values mismatch!\n");
    exit(1);
  }
}
