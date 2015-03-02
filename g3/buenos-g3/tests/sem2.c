#include "tests/lib.h"

// Only works for constants and arrays, so watch out.
#define write(s) syscall_write(1, s, sizeof(s))

int main() {
  /* the semaphore is created in the main test-file */
  usr_sem_t *sem = syscall_sem_open("test", -1);
  int i;
  for (i = 0; i < 0x20; i++) {
    syscall_sem_p(sem);

    write("Test 2 has the semaphore.\n");

    int j;
    for (j = 0; j < 10; j++)  {
      write("2 ...\n");
    }

    syscall_sem_v(sem);
  }
  syscall_sem_destroy(sem);

  return 0;
}
