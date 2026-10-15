// tree - print a directory tree (minimal version for xv6).
//
// Usage: tree [path] [-L depth] [-d]
//   -L depth  descend at most depth levels below path
//   -d        list directories only
//
// Style: "├── " / "└── " branches with "│   " continuation, entries in
// directory order. "." and ".." are never listed.
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#define PATHMAX 200
#define PREFIXMAX 256

static int only_dir;
static int max_depth = 1 << 30;
static char prefix[PREFIXMAX];
static int plen;

// Read the next entry of the open directory fd (located at dir) that
// should be shown. Return 1 and fill name/isdir, or 0 at the end.
static int
next_entry(int fd, char *dir, char *name, int *isdir)
{
  struct dirent de;
  struct stat st;
  char buf[PATHMAX];
  int dl = strlen(dir);

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;
    memmove(name, de.name, DIRSIZ);
    name[DIRSIZ] = 0;
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
      continue;
    if (dl + 1 + strlen(name) + 1 > sizeof buf)
      continue;
    strcpy(buf, dir);
    buf[dl] = '/';
    strcpy(buf + dl + 1, name);
    if (stat(buf, &st) < 0) {
      fprintf(2, "tree: cannot stat %s\n", buf);
      continue;
    }
    if (st.type == T_DIR) {
      *isdir = 1;
    } else {
      if (only_dir)
        continue;
      *isdir = 0;
    }
    return 1;
  }
  return 0;
}

static void
tree(char *path, int level)
{
  char cur[DIRSIZ + 1], nxt[DIRSIZ + 1];
  int cur_dir = 0, nxt_dir = 0;
  int fd, have;
  char child[PATHMAX];

  if (level >= max_depth)
    return;
  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "tree: cannot open %s\n", path);
    return;
  }

  // Look one entry ahead so we know which entry is the last one.
  have = next_entry(fd, path, cur, &cur_dir);
  while (have) {
    int more = next_entry(fd, path, nxt, &nxt_dir);
    int last = !more;

    printf("%s%s%s\n", prefix, last ? "└── " : "├── ", cur);

    if (cur_dir && level + 1 < max_depth &&
        strlen(path) + 1 + strlen(cur) + 1 <= sizeof child) {
      int saved = plen;
      const char *ext = last ? "    " : "│   ";
      int el = strlen(ext);
      strcpy(child, path);
      child[strlen(path)] = '/';
      strcpy(child + strlen(path) + 1, cur);
      if (plen + el < PREFIXMAX) {
        memmove(prefix + plen, ext, el + 1);
        plen += el;
      }
      tree(child, level + 1);
      plen = saved;
      prefix[plen] = 0;
    }

    memmove(cur, nxt, sizeof(cur));
    cur_dir = nxt_dir;
    have = more;
  }
  close(fd);
}

static void
usage(void)
{
  fprintf(2, "usage: tree [path] [-L depth] [-d]\n");
  exit(1);
}

int
main(int argc, char *argv[])
{
  char *path = ".";
  int i, have_path = 0;
  struct stat st;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-d") == 0) {
      only_dir = 1;
    } else if (strcmp(argv[i], "-L") == 0) {
      if (i + 1 >= argc)
        usage();
      max_depth = atoi(argv[++i]);
      if (max_depth < 0)
        usage();
    } else if (argv[i][0] == '-' && argv[i][1] != 0) {
      usage();
    } else if (!have_path) {
      path = argv[i];
      have_path = 1;
    } else {
      usage();
    }
  }

  if (stat(path, &st) < 0) {
    fprintf(2, "tree: cannot open %s\n", path);
    exit(1);
  }
  printf("%s\n", path);
  if (st.type == T_DIR)
    tree(path, 0);
  exit(0);
}
