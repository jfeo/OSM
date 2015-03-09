/*
 * TLB handling
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
 * $Id: tlb.c,v 1.6 2004/04/16 10:54:29 ttakanen Exp $
 *
 */

#include "kernel/panic.h"
#include "kernel/assert.h"
#include "kernel/thread.h"
#include "vm/tlb.h"
#include "vm/pagetable.h"

void tlb_modified_exception(void)
{
    KERNEL_PANIC("Unhandled TLB modified exception");
}

void tlb_exception(void) {

    thread_table_t *threadtbl = thread_get_current_thread_entry();
    KERNEL_ASSERT(threadtbl != NULL);

    pagetable_t *ptbl = threadtbl->pagetable;
    KERNEL_ASSERT(ptbl != NULL); // not a kernel thread

    tlb_exception_state_t state;
    _tlb_get_exception_state(&state);

    unsigned int i;
    kprintf("Current pagetable ASID: %d\n", ptbl->ASID);
    kprintf("Current exception failed ASID: %d\n", state.asid);
    kprintf("Current thread ID: %d\n", thread_get_current_thread());
    kprintf("Pagetable has %d valid entries \n", ptbl->valid_count);
    kprintf("The failing virtual address: %p\n", state.badvaddr);
    kprintf("Searching in pagetable for entry with VPN2=%d\n", state.badvpn2);

    for(i = 0 ; i < ptbl->valid_count ; ++i) {
        tlb_entry_t *entry = &ptbl->entries[ i ];
        int evenpage = state.badvaddr & 4096;
        int isvalid = (evenpage == 0) ? entry->V0 : entry->V1;
        kprintf("Entry VPN2: %d, V0: %d, V1: %d, VALID: %s\n", entry->VPN2, entry->V0, entry->V1, isvalid ? "yes" : "no" );
        if((entry->VPN2 == state.badvpn2) && isvalid) {
            int index = _tlb_probe(entry);
            if(index < 0)
                _tlb_write_random(entry);
            else {
                int entries_written = _tlb_write(entry, index, 1);
                if(entries_written <= 0) {
                    KERNEL_PANIC("Page fault, Error while writing to existing entry in TLB.\n");
                }
                kprintf("Wrote %d existing entries to TLB\n", entries_written);
            }
           //_tlb_set_asid(entry->ASID);
           return;
        }
    }

    if(threadtbl->user_context->status & USERLAND_ENABLE_BIT) {
        KERNEL_PANIC("Page fault (Access violation)\n");
    }
    KERNEL_PANIC("Page fault\n"); 
}

void tlb_store_exception(void)
{
    kprintf("TLB Store exception:\n");
    tlb_exception();
}

void tlb_load_exception(void)
{
    kprintf("TLB Load exception:\n");

    tlb_exception();
}
/**
 * Fill TLB with given pagetable. This function is used to set memory
 * mappings in CP0's TLB before we have a proper TLB handling system.
 * This approach limits the maximum mapping size to 128kB.
 *
 * @param pagetable Mappings to write to TLB.
 *
 */

void tlb_fill(pagetable_t *pagetable)
{

    if(pagetable == NULL)
	return;

    /* Check that the pagetable can fit into TLB. This is needed until
     we have proper VM system, because the whole pagetable must fit
     into TLB. */
    KERNEL_ASSERT(pagetable->valid_count <= (_tlb_get_maxindex()+1));

    _tlb_write(pagetable->entries, 0, pagetable->valid_count);

    /* Set ASID field in Co-Processor 0 to match thread ID so that
       only entries with the ASID of the current thread will match in
       the TLB hardware. */
    _tlb_set_asid(pagetable->ASID);
}
