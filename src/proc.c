#include "proc.h"
#include "mmu.h"
#include "pagesim.h"
#include "va_splitting.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"


void proc_init(pcb_t *proc) {


    pfn_t page_table = free_frame();

    memset((PAGE_SIZE * page_table) + mem, 0, PAGE_SIZE);

    proc->saved_ptbr = page_table;

    fte_t * total = (frame_table + page_table);

    total->protected = 1;
    total->mapped = 1;
    total->process = proc;    

}


void context_switch(pcb_t *proc) {
  
    PTBR = proc->saved_ptbr;


}


void proc_cleanup(pcb_t *proc) {

      pte_t *page_table = (pte_t *)(mem + (proc->saved_ptbr * PAGE_SIZE));
    for (size_t i = 0; i < NUM_PAGES; i++) {

      pte_t * p_entry = &page_table[i];

      if (p_entry-> valid) {

        frame_table[(p_entry->pfn)].mapped = 0;
      }


    if (swap_exists(p_entry)) {
        swap_free(p_entry);
    }



    }

    (frame_table + proc->saved_ptbr)->protected = 0;

    (frame_table + proc->saved_ptbr)->mapped = 0;


}

#pragma GCC diagnostic pop
