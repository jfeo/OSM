#include "lib.h"

int main(void) {
  int res;
  int file;
  const char *name = "[disk]y.txt";
  const char *str = "hello world";

  syscall_create(name, strlen(str));
  file = syscall_open(name);

  res = syscall_write(file, str, strlen(str));
  syscall_seek(file, 0);

  char buf[128];
  res = syscall_read(file, &buf, strlen(str));
  
  printf(buf);
  syscall_halt();
  return res;
}
