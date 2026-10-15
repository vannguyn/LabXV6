// diff - compare two files line by line (minimal version for xv6).
//
// Usage: diff file1 file2 [-q]
//   -q  only report whether the files differ
//
// Output for the line numbered i that differs:
//   file1:i: < <line of file1, or EOF>
//   file2:i: > <line of file2, or EOF>
// Identical files print nothing. Lines longer than MAXLINE-1 bytes are
// truncated for comparison purposes (the rest of the line is skipped).
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAXLINE 512

struct reader {
  int fd;
  int n;  // bytes currently in buf
  int i;  // next byte to consume
  char buf[512];
};

// Read the next byte; return -1 at EOF or on error.
static int
nextch(struct reader *r)
{
  if (r->i >= r->n) {
    r->n = read(r->fd, r->buf, sizeof(r->buf));
    r->i = 0;
    if (r->n <= 0) {
      r->n = 0;
      return -1;
    }
  }
  return (unsigned char)r->buf[r->i++];
}

// Read one line (without the '\n') into line. Return 1 if a line was
// read, 0 at EOF. A final line without '\n' still counts as a line.
static int
readline(struct reader *r, char *line, int max)
{
  int c, len = 0, got = 0;

  while ((c = nextch(r)) >= 0) {
    got = 1;
    if (c == '\n')
      break;
    if (len < max - 1)
      line[len++] = c;
  }
  line[len] = 0;
  return got;
}

int
main(int argc, char *argv[])
{
  char *names[2];
  int nfiles = 0, quiet = 0, i;
  struct reader *r[2];
  static char l1[MAXLINE], l2[MAXLINE];
  int lineno = 0, differ = 0;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-q") == 0) {
      quiet = 1;
    } else if (nfiles < 2) {
      names[nfiles++] = argv[i];
    } else {
      nfiles = 3;
      break;
    }
  }
  if (nfiles != 2) {
    fprintf(2, "usage: diff file1 file2 [-q]\n");
    exit(1);
  }

  for (i = 0; i < 2; i++) {
    r[i] = malloc(sizeof(struct reader));
    if (r[i] == 0) {
      fprintf(2, "diff: out of memory\n");
      exit(1);
    }
    r[i]->n = r[i]->i = 0;
    if ((r[i]->fd = open(names[i], O_RDONLY)) < 0) {
      fprintf(2, "diff: cannot open %s\n", names[i]);
      exit(1);
    }
  }

  for (;;) {
    int a = readline(r[0], l1, MAXLINE);
    int b = readline(r[1], l2, MAXLINE);

    // Do not count the extra read after both files reach EOF as a line.
    if (!a && !b)
      break;
    lineno++;

    if (a && b && strcmp(l1, l2) == 0)
      continue;

    differ = 1;
    if (quiet)
      break;

    // Print both sides at the same line number.  When one file has ended,
    // print EOF for that side and the remaining line for the other side.
    if (a && b) {
      printf("%s:%d: < %s\n", names[0], lineno, l1);
      printf("%s:%d: > %s\n", names[1], lineno, l2);
    } else if (a) {
      printf("%s:%d: < %s\n", names[0], lineno, l1);
      printf("%s:%d: > EOF\n", names[1], lineno);
    } else {
      printf("%s:%d: < EOF\n", names[0], lineno);
      printf("%s:%d: > %s\n", names[1], lineno, l2);
    }
  }

  if (differ && quiet)
    printf("diff: files differ\n");

  close(r[0]->fd);
  close(r[1]->fd);
  exit(differ ? 1 : 0);
}
