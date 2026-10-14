#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p2c[2]; // pipe: parent -> child
  int c2p[2]; // pipe: child -> parent

  if(pipe(p2c) < 0 || pipe(c2p) < 0) {
    fprintf(2, "pingpong: pipe creation failed\n");
    exit(1);
  }

  int pid = fork();
  if(pid < 0) {
    fprintf(2, "pingpong: fork failed\n");
    exit(1);
  }

  if(pid == 0) {
    // Child process
    close(p2c[1]);
    close(c2p[0]);

    char buf;
    if(read(p2c[0], &buf, 1) != 1) {
      fprintf(2, "pingpong: child read error\n");
      exit(1);
    }
    printf("%d: received ping\n", getpid());

    if(write(c2p[1], &buf, 1) != 1) {
      fprintf(2, "pingpong: child write error\n");
      exit(1);
    }

    close(p2c[0]);
    close(c2p[1]);
    exit(0);
  } else {
    // Parent process
    close(p2c[0]);
    close(c2p[1]);

    char buf = 'x';
    if(write(p2c[1], &buf, 1) != 1) {
      fprintf(2, "pingpong: parent write error\n");
      exit(1);
    }

    if(read(c2p[0], &buf, 1) != 1) {
      fprintf(2, "pingpong: parent read error\n");
      exit(1);
    }
    printf("%d: received pong\n", getpid());

    close(p2c[1]);
    close(c2p[0]);
    wait(0);
    exit(0);
  }
}
