/*
 * System calls.
 *
 * Copyright (C) 2003 Juha Aatrokoski, Timo Lilja,
 *   Leena Salmela, Teemu Takanen, Aleksi Virtanen.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: syscall.c,v 1.3 2004/01/13 11:10:05 ttakanen Exp $
 *
 */
#include "fs/vfs.h"
#include "kernel/cswitch.h"
#include "kernel/halt.h"
#include "kernel/panic.h"
#include "lib/libc.h"
#include "proc/io.h"
#include "proc/syscall.h"
#include "proc/process.h"
#include "kernel/assert.h"
#include "drivers/device.h"
#include "drivers/gcd.h"
#include "proc/usr_sem.h"

#define A0 user_context->cpu_regs[MIPS_REGISTER_A0]
#define A1 user_context->cpu_regs[MIPS_REGISTER_A1]
#define A2 user_context->cpu_regs[MIPS_REGISTER_A2]
#define A3 user_context->cpu_regs[MIPS_REGISTER_A3]
#define V0 user_context->cpu_regs[MIPS_REGISTER_V0]

static const int filehandle_mapping = 3;

process_id_t syscall_exec(const char *filename) {
  return process_spawn(filename);
}

void syscall_exit(int retval) {
  process_finish(retval);
}

int syscall_join(process_id_t pid) {
  return process_join(pid);
}

int syscall_close(int file) {
  int error = vfs_close(file - filehandle_mapping);
  return error;
}

int syscall_open(const char* path) {
  openfile_t file = vfs_open(path);
  if (file < 0) /* return vfs error */
    return file;

  return file + filehandle_mapping; /* map to avoid conflict with stdin/stdout/stderr */
}

int syscall_write(int filehandle, void* buf, int length) {
  int written = vfs_write(filehandle-filehandle_mapping, buf, length);
  return written;
}

int syscall_read(int filehandle, void* buf, int length) {
  int readbytes = vfs_read(filehandle-filehandle_mapping, buf, length);
  return readbytes;
}

int syscall_create(const char* pathname, int size) {
  int res = vfs_create(pathname, size);
  return res;
}

int syscall_remove(const char* pathname) {
  return vfs_remove(pathname);
}

int syscall_seek(int filehandle, int offset) {
  return vfs_seek(filehandle-filehandle_mapping, offset);
}

int syscall_tell(int filehandle) {
  return vfs_tell(filehandle-filehandle_mapping);
}

/**
 * Handle system calls. Interrupts are enabled when this function is
 * called.
 *
 * @param user_context The userland context (CPU registers as they
 * where when system call instruction was called in userland)
 */
void syscall_handle(context_t *user_context)
{
  /* When a syscall is executed in userland, register a0 contains
   * the number of the syscall. Registers a1, a2 and a3 contain the
   * arguments of the syscall. The userland code expects that after
   * returning from the syscall instruction the return value of the
   * syscall is found in register v0. Before entering this function
   * the userland context has been saved to user_context and after
   * returning from this function the userland context will be
   * restored from user_context.
   */
  switch (A0) {
  case SYSCALL_HALT:
    halt_kernel();
    break;
  case SYSCALL_READ:
    {
      int handle = (int) A1;

      if (handle > filehandle_mapping - 1) {
        V0 = syscall_read(handle, (void*) A2, (int) A3);
      } else {
        V0 = io_read((int) A1, (void*) A2, (int) A3);
      }
      break;
    }
  case SYSCALL_WRITE:
  {
    int handle = (int) A1;
    //kprintf("Handle: %d\n", handle);
    if(handle > filehandle_mapping - 1) {
      V0 = syscall_write((int) A1, (void*) A2, (int) A3);
    }
    else {
      V0 = io_write((int) A1, (void*) A2, (int) A3);
    }    
    break;
  }
  case SYSCALL_EXEC:
    V0 = syscall_exec((char*) A1);
    break;
  case SYSCALL_EXIT:
    syscall_exit((int) A1);
    break;
  case SYSCALL_JOIN:
    V0 = syscall_join((process_id_t) A1);
    break;
  case SYSCALL_SEM_OPEN:
    V0 = (uint32_t) usr_sem_open((char*) A1, A2);
    break;
  case SYSCALL_SEM_PROCURE:
    V0 = usr_sem_p((usr_sem_t*) A1);
    break;
  case SYSCALL_SEM_VACATE:
    V0 = usr_sem_v((usr_sem_t*) A1);
    break;
  case SYSCALL_SEM_DESTROY:
    V0 = usr_sem_destroy((usr_sem_t*) A1);
    break;
  case SYSCALL_OPEN:
    V0 = syscall_open((const char*) A1);
    break;    
  case SYSCALL_CLOSE:
    V0 = syscall_close((int) A1);
    break;   
  case SYSCALL_SEEK:
    V0 = syscall_seek((int) A1, (int) A2);
    break;      
  case SYSCALL_CREATE:
    V0 = syscall_create((const char*) A1, A2);
    break;  
  case SYSCALL_REMOVE:
    V0 = syscall_remove((const char*) A1);
    break;  
  case SYSCALL_TELL:
    V0 = syscall_tell((int) A1);
    break;    
  default:
    KERNEL_PANIC("Unhandled system call\n");
  }

  /* Move to next instruction after system call */
  user_context->pc += 4;
}
