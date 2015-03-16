#include "lib.h"

int main(void) {
  const int len = 128;
  char buf[len], buf2[len];
  int tfs, gfs;
  const char *tfs_name = "[tfs]hello.txt";
  const char *gfs_name = "[disk]hello.txt";

  tfs = syscall_open(tfs_name);
  gfs = syscall_create(gfs_name, len);

  syscall_read(tfs, &buf, len);
  syscall_write(gfs, &buf, len);
  syscall_read(gfs, &buf2, len);

  if(strcmp(buf, buf2) != 0) {
    printf("Error, the copied file doesnt match\n");
    return 1;
  }

  printf("OK\n");

  syscall_halt();
  return 0;
}
