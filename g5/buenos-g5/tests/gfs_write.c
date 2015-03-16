#include "lib.h"
#define LEN 11000

char srcbuf[LEN];
char buf[LEN];

int main(void) {
  int res;
  int file, srcfile;
  const char *name = "[disk]gwrite.txt";
  const char *source = "[disk]data";
  //const char *str = "hello world";
  

  srcfile = syscall_open(source);
  syscall_read(srcfile, &srcbuf, LEN);

  syscall_create(name, 1);
  file = syscall_open(name);

  res = syscall_write(file, &srcbuf, LEN);
  syscall_seek(file, 0);
  printf("Wrote: %d\n", res);
  
  res = syscall_read(file, &buf, LEN);
  printf("Read: %d\n", res);
  //printf("Text: %s\n", buf);

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
