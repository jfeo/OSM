#include "tests/lib.h"

int main(void) {
	usr_sem_t* s = syscall_sem_open("simon", 0);
	printf("Semaphore: %p\n", s);
	s = syscall_sem_open("simon", 0);
	printf("Semaphore: %p\n", s);
	return 0;
}
