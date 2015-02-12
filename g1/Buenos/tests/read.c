#include "tests/lib.h"
#include "proc/syscall.h"

int main(void)
{
	char buf[255];
	int read = syscall_read(FILEHANDLE_STDIN, &buf, 10);
	syscall_write(FILEHANDLE_STDOUT, &buf, read);
	return 0;
}
