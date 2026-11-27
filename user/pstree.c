#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/ptree.h"
#include "user/user.h"

#define MAX_PROCS 64

static struct ptreeinfo procs[MAX_PROCS];
static int nprocs;

// Recursively print the process tree rooted at pid with given indentation level.
void
printtree(int pid, int level)
{
  for(int i = 0; i < nprocs; i++){
    if(procs[i].pid == pid){
      // Print indentation
      for(int j = 0; j < level; j++)
        printf("  ");

      printf("%d %s state=%d mem=%ld\n",
             procs[i].pid, procs[i].name, procs[i].state, procs[i].memsize);
      break;
    }
  }

  // Find and print all children of this pid
  for(int i = 0; i < nprocs; i++){
    if(procs[i].ppid == pid && procs[i].pid != pid){
      printtree(procs[i].pid, level + 1);
    }
  }
}

int
main(int argc, char *argv[])
{
  nprocs = ptree(procs, MAX_PROCS);
  if(nprocs < 0){
    printf("ptree: syscall failed\n");
    exit(1);
  }

  // Find root process(es) — those with ppid == 0 or ppid == self
  for(int i = 0; i < nprocs; i++){
    if(procs[i].ppid == 0 || procs[i].ppid == procs[i].pid){
      printtree(procs[i].pid, 0);
    }
  }

  exit(0);
}
