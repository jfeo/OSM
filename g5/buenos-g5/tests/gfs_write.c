#include "lib.h"

int main(void) {
  int res;
  int file;
  const char *name = "[disk]gwrite.txt";
  const char *str = "hello world";

  syscall_create(name, 1);
  file = syscall_open(name);

  res = syscall_write(file, str, strlen(str));
  syscall_seek(file, 0);

  char buf[128];
  res = syscall_read(file, &buf, strlen(str));

  if (0 != strcmp(buf, "hello world")) {
    printf("The strings are not the same!!\n\n");
    printf(buf);
    printf("\n\n");
    return 1;
  }

  printf("OK\n\n");

  syscall_halt();
  return res;
}
