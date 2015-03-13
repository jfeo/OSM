/*
 * Userland helloworld
 */

#include "tests/lib.h"
#define write(s) syscall_write(1, s, sizeof(s))
#define PRINT(p) write("#p")

#define VFS_OK              0
#define VFS_NOT_SUPPORTED   -1
#define VFS_ERROR           -2
#define VFS_INVALID_PARAMS  -3
#define VFS_NOT_OPEN        -4
#define VFS_NOT_FOUND       -5
#define VFS_NO_SUCH_FS      -6
#define VFS_LIMIT           -7
#define VFS_IN_USE          -8
#define VFS_UNUSABLE        -9

void err_(int code) {
  switch(code) {
    case VFS_OK: write("Error: VFS_OK\n"); break;
    case VFS_NOT_SUPPORTED: write("Error: VFS_NOT_SUPPORTED\n"); break;
    case VFS_ERROR: write("Error: VFS_ERROR\n"); break;
    case VFS_INVALID_PARAMS: write("Error: VFS_INVALID_PARAMS\n"); break;
    case VFS_NOT_OPEN: write("Error: VFS_NOT_OPEN\n"); break;
    case VFS_NOT_FOUND: write("Error: VFS_NOT_FOUND\n"); break;
    case VFS_NO_SUCH_FS: write("Error: VFS_NO_SUCH_FS\n"); break;
    case VFS_LIMIT: write("Error: VFS_LIMIT\n"); break;
    case VFS_IN_USE: write("Error: VFS_IN_USE\n"); break;
    case VFS_UNUSABLE: write("Error: VFS_UNUSABLE\n"); break;
    default:
                       printf("Unhandled error. Code: %d\n", code);
  }
}

#define err(code) printf("Error at line: %d: ", __LINE__); err_(code);

int main(void)
{
  int file = syscall_open("[disk]test.txt");
  if(file <= 2) {
    err(file);
    return file;
  }

  char b[] = "WriteTest\n";
  int error = syscall_write(file, &b, sizeof(b));
  if(error < 0) {
    err(error);
    return 0;
  }

  char c[11]; c[10] = '\0';
  syscall_seek(file, 0);
  syscall_read(file, &c, 10);
  if(strcmp(c, b) != 0) {
    printf("Write test failed.\n");
    return 0;
  }

  int tell = syscall_tell(file);
  if(tell != 10) {
    printf("Tell test failed: tell returned unexcepted value %d\n", tell);
    return 0;
  }

  error = syscall_close(file);
  if(error < 0) {
    err(error);
    return 0;
  }
  printf("Test OK.\n");
  return 0;
}
