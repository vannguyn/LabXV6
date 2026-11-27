#ifndef _PTREE_H_
#define _PTREE_H_

struct ptreeinfo {
  int pid;
  int ppid;
  int state;
  uint64 memsize;   // number of bytes of user memory of the process
  char name[16];
};

#endif

