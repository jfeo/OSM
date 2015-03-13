#include "lib.h"

int main(void) {
  int res;
  int file;
  const char *name = "[disk]y.txt";
  const char *str = "hello world";

  syscall_create(name, sizeof(str));

  file = syscall_open(name);

  res = syscall_write(file, str, sizeof(str));
  if (res != 0) {
    printf("syscall_write failed!\n\n");
    printf("res: %d\n\n", res);
    return 1;
  }

  char buf[128];
  res = syscall_read(file, &buf, 128);
  if (res != 0) {
    printf("syscall_read failed!\n\n");
    printf("res: %d\n\n", res);
    return 1;
  }

  printf(buf);

  syscall_halt();
  return 0;
}
