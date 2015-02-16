#include "tests/lib.h"

int main(void)
{
  char msgbuf1[28] = "Please enter 10 characters. ";
  syscall_write(stdout, &msgbuf1, 28);

	char buf[11];
	int read = syscall_read(stdin, &buf, 10);

  char msgbuf2[12] = "\nYou wrote: ";
  syscall_write(stdout, &msgbuf2, 12);
	syscall_write(stdout, &buf, read);
  char nl[] = "\n";
  syscall_write(stdout, &nl, 1);
  syscall_halt();
	return 0;
}
