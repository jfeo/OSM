/*
 * Process startup.
 *
 * Copyright (C) 2003-2005 Juha Aatrokoski, Timo Lilja,
 *       Leena Salmela, Teemu Takanen, Aleksi Virtanen.
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
 * $Id: process.c,v 1.11 2007/03/07 18:12:00 ttakanen Exp $
 *
 */

#include "proc/process.h"
#include "proc/elf.h"
#include "kernel/thread.h"
#include "kernel/assert.h"
#include "kernel/interrupt.h"
#include "kernel/sleepq.h"
#include "kernel/config.h"
#include "fs/vfs.h"
#include "drivers/yams.h"
#include "vm/vm.h"
#include "vm/pagepool.h"


/** @name Process startup
 *
 * This module contains facilities for managing userland processes.
 */

process_control_block_t process_table[PROCESS_MAX_PROCESSES];

/* We need a spinlock to lock accesses to the process table. */
spinlock_t process_table_slock;

/**
 * Starts one userland process. The thread calling this function will
 * be used to run the process and will therefore never return from
 * this function. This function asserts that no errors occur in
 * process startup (the executable file exists and is a valid ecoff
 * file, enough memory is available, file operations succeed...).
 * Therefore this function is not suitable to allow startup of
 * arbitrary processes.
 *
 * @pid The process id describing the process to be run in userland
 */
void process_start(process_id_t pid) {
    thread_table_t *my_entry;
    pagetable_t *pagetable;
    uint32_t phys_page;
    context_t user_context;
    uint32_t stack_bottom;
    elf_info_t elf;
    openfile_t file;
    char *executable;

    int i;

    interrupt_status_t intr_status;

    my_entry = thread_get_current_thread_entry();
    /* Associate the process' kernel thread with the pid. */
    my_entry->process_id = pid;
    /* Get the executable from the process in the process table. */
    executable = process_table[pid].executable;

    /* If the pagetable of this thread is not NULL, we are trying to
       run a userland process for a second time in the same thread.
       This is not possible. */
    KERNEL_ASSERT(my_entry->pagetable == NULL);

    pagetable = vm_create_pagetable(thread_get_current_thread());
    KERNEL_ASSERT(pagetable != NULL);

    intr_status = _interrupt_disable();
    my_entry->pagetable = pagetable;
    _interrupt_set_state(intr_status);

    file = vfs_open((char *)executable);
    /* Make sure the file existed and was a valid ELF file */
    KERNEL_ASSERT(file >= 0);
    KERNEL_ASSERT(elf_parse_header(&elf, file));

    /* Trivial and naive sanity check for entry point: */
    KERNEL_ASSERT(elf.entry_point >= PAGE_SIZE);

    /* Calculate the number of pages needed by the whole process
       (including userland stack). Since we don't have proper tlb
       handling code, all these pages must fit into TLB. */
    KERNEL_ASSERT(elf.ro_pages + elf.rw_pages + CONFIG_USERLAND_STACK_SIZE
                  <= _tlb_get_maxindex() + 1);

    /* Allocate and map stack */
    for(i = 0; i < CONFIG_USERLAND_STACK_SIZE; i++) {
        phys_page = pagepool_get_phys_page();
        KERNEL_ASSERT(phys_page != 0);
        vm_map(my_entry->pagetable, phys_page,
               (USERLAND_STACK_TOP & PAGE_SIZE_MASK) - i*PAGE_SIZE, 1);
    }

    /* Allocate and map pages for the segments. We assume that
       segments begin at page boundary. (The linker script in tests
       directory creates this kind of segments) */
    for(i = 0; i < (int)elf.ro_pages; i++) {
        phys_page = pagepool_get_phys_page();
        KERNEL_ASSERT(phys_page != 0);
        vm_map(my_entry->pagetable, phys_page,
               elf.ro_vaddr + i*PAGE_SIZE, 1);
    }

    for(i = 0; i < (int)elf.rw_pages; i++) {
        phys_page = pagepool_get_phys_page();
        KERNEL_ASSERT(phys_page != 0);
        vm_map(my_entry->pagetable, phys_page,
               elf.rw_vaddr + i*PAGE_SIZE, 1);
    }

    /* Put the mapped pages into TLB. Here we again assume that the
       pages fit into the TLB. After writing proper TLB exception
       handling this call should be skipped. */
    /* intr_status = _interrupt_disable(); */
    /* tlb_fill(my_entry->pagetable); */
    /* _interrupt_set_state(intr_status); */

    /* Now we may use the virtual addresses of the segments. */

    /* Zero the pages. */
    memoryset((void *)elf.ro_vaddr, 0, elf.ro_pages*PAGE_SIZE);
    memoryset((void *)elf.rw_vaddr, 0, elf.rw_pages*PAGE_SIZE);

    stack_bottom = (USERLAND_STACK_TOP & PAGE_SIZE_MASK) -
        (CONFIG_USERLAND_STACK_SIZE-1)*PAGE_SIZE;
    memoryset((void *)stack_bottom, 0, CONFIG_USERLAND_STACK_SIZE*PAGE_SIZE);

    /* Copy segments */

    if (elf.ro_size > 0) {
        /* Make sure that the segment is in proper place. */
        KERNEL_ASSERT(elf.ro_vaddr >= PAGE_SIZE);
        KERNEL_ASSERT(vfs_seek(file, elf.ro_location) == VFS_OK);
        KERNEL_ASSERT(vfs_read(file, (void *)elf.ro_vaddr, elf.ro_size)
                      == (int)elf.ro_size);
    }

    if (elf.rw_size > 0) {
        /* Make sure that the segment is in proper place. */
        KERNEL_ASSERT(elf.rw_vaddr >= PAGE_SIZE);
        KERNEL_ASSERT(vfs_seek(file, elf.rw_location) == VFS_OK);
        KERNEL_ASSERT(vfs_read(file, (void *)elf.rw_vaddr, elf.rw_size)
                      == (int)elf.rw_size);
    }


    /* Set the dirty bit to zero (read-only) on read-only pages. */
    for(i = 0; i < (int)elf.ro_pages; i++) {
        vm_set_dirty(my_entry->pagetable, elf.ro_vaddr + i*PAGE_SIZE, 0);
    }

    /* Insert page mappings again to TLB to take read-only bits into use */
    /* intr_status = _interrupt_disable(); */
    /* tlb_fill(my_entry->pagetable); */
    /* _interrupt_set_state(intr_status); */

    /* Initialize the user context. (Status register is handled by
       thread_goto_userland) */
    memoryset(&user_context, 0, sizeof(user_context));
    user_context.cpu_regs[MIPS_REGISTER_SP] = USERLAND_STACK_TOP;
    user_context.pc = elf.entry_point;

    thread_goto_userland(&user_context);

    KERNEL_PANIC("thread_goto_userland failed.");
}

/* Prepare a process slot for a new process. */
void process_reset(process_id_t pid) {
    process_table[pid].state = PROCESS_FREE;
    process_table[pid].executable[0] = '\0';
    process_table[pid].retval = 0;
    process_table[pid].parent = -1;
}

/* Initialize process table and spinlock. */
void process_init() {
    process_id_t i;
    spinlock_reset(&process_table_slock);
    for (i = 0; i < PROCESS_MAX_PROCESSES; i++) {
        process_reset(i);
    }
}

/* Find a free slot in the process table.  Returns PROCESS_MAX_PROCESSES if the
 * table is full. */
process_id_t alloc_process_id() {
    int i;
    interrupt_status_t intr_status;

    intr_status = _interrupt_disable();
    spinlock_acquire(&process_table_slock);

    for (i = 0; i < PROCESS_MAX_PROCESSES; i++) {
        if (process_table[i].state == PROCESS_FREE) {
            process_table[i].state = PROCESS_RUNNING;
            break;
        }
    }

    spinlock_release(&process_table_slock);
    _interrupt_set_state(intr_status);

    return i;
}


/* Spawn a new process by inserting it into the process table, making a thread
   for it, and running it.  Returns the PID of the new process, or
   PROCESS_PTABLE_FULL if the table is full. */
process_id_t process_spawn(const char *executable) {
    TID_t thread;
    process_id_t pid = alloc_process_id();

    if (pid == PROCESS_MAX_PROCESSES) {
        return PROCESS_PTABLE_FULL;
    }

    /* Remember to copy the executable name for use in `process_start`. */
    stringcopy(process_table[pid].executable, executable, PROCESS_MAX_FILELENGTH);

    /* Set the parent to be able to have a check in `process_join`. */
    process_table[pid].parent = process_get_current_process();

    /* Make a thread that reads and runs the program in `executable`, and start
       it. */
    thread = thread_create((void (*)(uint32_t))(&process_start), pid);
    thread_run(thread);

    return pid;
}

/* Stop the current process and the thread it runs in.  Sets the return value as
   well. */
void process_finish(int retval) {
    interrupt_status_t intr_status;
    process_id_t pid = process_get_current_process();
    thread_table_t *thread = thread_get_current_thread_entry();

    intr_status = _interrupt_disable();
    spinlock_acquire(&process_table_slock);

    /* Save the return value so it can be read later by `process_join`. */
    process_table[pid].retval = retval;

    /* Make it a zombie to state that all it needs to die completely is a join
       from its parent. */
    process_table[pid].state = PROCESS_ZOMBIE;

    /* Destroy the pagetable!  We don't have proper virtual memory handling
       yet. */
    vm_destroy_pagetable(thread->pagetable);
    thread->pagetable = NULL;

    /* Move any `process_join` call lying in Buenos' sleep queue into the
       scheduler's ready-to-run list, so it can exit. */
    sleepq_wake_all(&process_table[pid]);

    /* [BONUS] Kill all children that haven't been joined.  Another choice would
       be to reassign their `parent`, but freeing seems a better choice when we
       have limited space for processes. */
    process_id_t i;
    for (i = 0; i < PROCESS_MAX_PROCESSES; i++) {
        if (process_table[i].parent == pid) {
            /* Make space for a future process. */
            process_reset(i);
            /* IMPORTANT: We do not free the kernel thread running the child
               process, as this would require changes to `kernel/thread.c`,
               which is much out of scope for G2.  However, it should be fairly
               easy to augment `thread_finish` to do the right thing (right now
               you can only finish the currently running thread). */
        }
    }

    spinlock_release(&process_table_slock);
    _interrupt_set_state(intr_status);

    thread_finish();
}

/* Wait for the child process to finish, then return its return value.  Returns
   PROCESS_ILLEGAL_JOIN if `pid` is invalid or if someone else than the parent
   tries to join.  This will also mark its process table entry as free. */
int process_join(process_id_t pid) {
    int retval;
    interrupt_status_t intr_status;

    /* Only join with valid pids. */
    if (pid < 0 || pid >= PROCESS_MAX_PROCESSES ||
        process_table[pid].parent != process_get_current_process()) {
        return PROCESS_ILLEGAL_JOIN;
    }

    intr_status = _interrupt_disable();
    spinlock_acquire(&process_table_slock);

    /* Wait for the child process to exit and become a zombie. */
    while (process_table[pid].state != PROCESS_ZOMBIE) {
        /* Move to Buenos' sleep queue and switch to another thread. */
        sleepq_add(&process_table[pid]);
        spinlock_release(&process_table_slock);
        thread_switch();
        spinlock_acquire(&process_table_slock);
    }

    /* Get the return value and prepare its slot for a future process. */
    retval = process_table[pid].retval;
    process_reset(pid);

    spinlock_release(&process_table_slock);
    _interrupt_set_state(intr_status);

    return retval;
}

process_id_t process_get_current_process(void) {
    return thread_get_current_thread_entry()->process_id;
}

process_control_block_t *process_get_current_process_entry(void) {
    return &process_table[process_get_current_process()];
}

/** @} */
