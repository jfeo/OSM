#include "tests/lib.h"
#include "proc/syscall.h"

int main(void)
{
	char buf[10];
	int read = syscall_read(FILEHANDLE_STDIN, &buf, 10);
	syscall_write(FILEHANDLE_STDOUT, &buf, read);
  syscall_halt();
	return 0;
}
