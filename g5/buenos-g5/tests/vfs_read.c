#include "lib.h"

int main(void) {
  int filehandle = syscall_open("[disk]test.txt");

  if (filehandle < 0) {
    printf("Error opening file: %d\n", filehandle);
    return filehandle;
  }

  char buf[128];
  syscall_read(filehandle, &buf, 128);
  printf(buf);

  return 0;
}
