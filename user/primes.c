#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int lpipe_read) __attribute__((noreturn));

void
primes(int lpipe_read)
{
  int p;
  if(read(lpipe_read, &p, sizeof(int)) <= 0) {
    close(lpipe_read);
    exit(0);
  }

  printf("prime %d\n", p);

  int rpipe[2];
  int has_child = 0;
  int n;

  while(read(lpipe_read, &n, sizeof(int)) > 0) {
    if(n % p != 0) {
      if(!has_child) {
        if(pipe(rpipe) < 0) {
          fprintf(2, "primes: pipe failed\n");
          close(lpipe_read);
          exit(1);
        }
        int pid = fork();
        if(pid < 0) {
          fprintf(2, "primes: fork failed\n");
          close(lpipe_read);
          close(rpipe[0]);
          close(rpipe[1]);
          exit(1);
        }
        if(pid == 0) {
          close(rpipe[1]);
          close(lpipe_read);
          primes(rpipe[0]);
        }
        close(rpipe[0]);
        has_child = 1;
      }
      if(write(rpipe[1], &n, sizeof(int)) != sizeof(int)) {
        fprintf(2, "primes: write failed\n");
        close(lpipe_read);
        close(rpipe[1]);
        exit(1);
      }
    }
  }

  close(lpipe_read);
  if(has_child) {
    close(rpipe[1]);
    wait(0);
  }
  exit(0);
}

int
main(int argc, char *argv[])
{
  int p[2];
  if(pipe(p) < 0) {
    fprintf(2, "primes: initial pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if(pid < 0) {
    fprintf(2, "primes: initial fork failed\n");
    exit(1);
  }

  if(pid == 0) {
    close(p[1]);
    primes(p[0]);
  } else {
    close(p[0]);
    for(int i = 2; i <= 280; i++) {
      if(write(p[1], &i, sizeof(int)) != sizeof(int)) {
        fprintf(2, "primes: feed write failed\n");
        close(p[1]);
        exit(1);
      }
    }
    close(p[1]);
    wait(0);
    exit(0);
  }
}
