#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
print_escaped(char *s, int quoted)
{
  int i, n = strlen(s);
  int start = 0, end = n;
  char c;

  // xv6's shell does not remove double quotes, so handle them here for
  // commands such as: echo -e "apple\nbanana".
  if (quoted && n >= 2 && s[0] == '"' && s[n - 1] == '"') {
    start = 1;
    end = n - 1;
  }

  for (i = start; i < end; i++) {
    c = s[i];
    if (c == '\\' && i + 1 < end) {
      i++;
      if (s[i] == 'n')
        c = '\n';
      else if (s[i] == 't')
        c = '\t';
      else if (s[i] == '\\')
        c = '\\';
      else if (s[i] == '"')
        c = '"';
      else {
        write(1, "\\", 1);
        c = s[i];
      }
    }
    write(1, &c, 1);
  }
}

int
main(int argc, char *argv[])
{
  int i, interpret = 0;

  if (argc > 1 && strcmp(argv[1], "-e") == 0) {
    interpret = 1;
    i = 2;
  } else {
    i = 1;
  }

  for (; i < argc; i++) {
    if (interpret)
      print_escaped(argv[i], 1);
    else
      write(1, argv[i], strlen(argv[i]));
    if (i + 1 < argc) {
      write(1, " ", 1);
    }
  }
  write(1, "\n", 1);
  exit(0);
}
