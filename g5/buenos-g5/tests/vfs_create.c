#include "lib.h"

int main(void) {
  int filehandle;
  const char *name = "[disk]x.txt";

  filehandle = syscall_remove(name);
  if (filehandle == 0) {
    printf("syscall_remove could remove a file that doesn't exist!\n\n");
    printf("filehandle: %d\n\n", filehandle);
    return 1;
  }

  filehandle = syscall_create(name, 128);
  if (filehandle != 0) {
    printf("syscall_create didn't create the file!\n\n");
    printf("filehandle: %d\n\n", filehandle);
    return 1;
  }

  filehandle = syscall_create(name, 128);
  if (filehandle == 0) {
    printf("syscall_create should have failed recreating a file, but didn't!\n\n");
    printf("filehandle: %d\n\n", filehandle);
    return 1;
  }

  filehandle = syscall_remove(name);
  if (filehandle != 0) {
    printf("syscall_remove doesn't work!\n\n");
    printf("filehandle: %d\n\n", filehandle);
    return 1;
  }

  printf("OK\n\n");

  syscall_halt();
  return 0;
}
