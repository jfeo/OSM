#include "lib.h"

/* the two test programs */
static const char test1[] = "[disk]sem1";
static const char test2[] = "[disk]sem2";

int main() {
  int pid1 = syscall_exec(test1);
  int pid2 = syscall_exec(test2);

  /* join test processes, so we don't halt too early */
  syscall_join(pid2);
  syscall_join(pid1);

  /* halt the system to prevent 'run out of initprog error' */
  syscall_halt();

  /* dummy return statement */
  return 0;
}
