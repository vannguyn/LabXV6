// du - summarize disk usage (minimal version for xv6).
//
// Usage: du [path] [-a] [-s]
//   -a  also print every file, not only directories
//   -s  print only the total for path
//
// Size convention: the size of a directory is the sum of the sizes of
// the entries inside it (file sizes come from st.size, in bytes). The
// size of the directory inode itself is ignored.
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#define PATHMAX 256

static int opt_all;
static int opt_sum;

// Return the total number of bytes under path. When print is non-zero,
// also print the entries selected by -a (files) and always directories.
long
du(char *path, int print)
{
  char buf[PATHMAX], *p;
  int fd;
  struct dirent de;
  struct stat st;
  long total = 0;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "du: cannot open %s\n", path);
    return 0;
  }
  if (fstat(fd, &st) < 0) {
    fprintf(2, "du: cannot stat %s\n", path);
    close(fd);
    return 0;
  }

  if (st.type != T_DIR) {
    total = st.size;
    // A file given explicitly on the command line is always printed.
    if (print)
      printf("%d\t%s\n", (int)total, path);
    close(fd);
    return total;
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
    fprintf(2, "du: path too long\n");
    close(fd);
    return 0;
  }
  strcpy(buf, path);
  p = buf + strlen(buf);
  if (p == buf || p[-1] != '/')
    *p++ = '/';

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if (strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
      continue;
    if (stat(buf, &st) < 0) {
      fprintf(2, "du: cannot stat %s\n", buf);
      continue;
    }
    if (st.type == T_DIR) {
      total += du(buf, print);
    } else {
      total += st.size;
      if (print && opt_all)
        printf("%d\t%s\n", (int)st.size, buf);
    }
  }
  close(fd);

  if (print)
    printf("%d\t%s\n", (int)total, path);
  return total;
}

int
main(int argc, char *argv[])
{
  char *path = ".";
  int i, have_path = 0;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-a") == 0) {
      opt_all = 1;
    } else if (strcmp(argv[i], "-s") == 0) {
      opt_sum = 1;
    } else if (argv[i][0] == '-' && argv[i][1] != 0) {
      fprintf(2, "usage: du [path] [-a] [-s]\n");
      exit(1);
    } else if (!have_path) {
      path = argv[i];
      have_path = 1;
    } else {
      fprintf(2, "usage: du [path] [-a] [-s]\n");
      exit(1);
    }
  }

  if (opt_sum) {
    // -s wins over -a: only the grand total is printed.
    long total = du(path, 0);
    printf("%d\t%s\n", (int)total, path);
  } else {
    du(path, 1);
  }
  exit(0);
}
