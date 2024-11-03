#include "mmu.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void page_fault(vaddr_t addr)
{
   

    pte_t * p = (pte_t *)(mem + (PTBR * PAGE_SIZE)) + vaddr_vpn(addr);
    pfn_t pfn = free_frame();
    stats.page_faults++;


   if (swap_exists(p))
   {
      swap_read(p, mem + ( pfn * PAGE_SIZE));
   


   }
   else
   {
      memset(mem + (pfn * PAGE_SIZE), 0, PAGE_SIZE);
   }
   
    p->dirty = 0;
    p->pfn = pfn;
    p->valid = 1;       
    p->referenced = 1;   

    fte_t *ft = frame_table + pfn;

    ft->protected = 0;       
    ft->mapped = 1;  
    ft->process = current_process;
    ft->vpn = vaddr_vpn(addr);  

}

#pragma GCC diagnostic pop
