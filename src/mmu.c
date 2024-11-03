#include "mmu.h"
#include "pagesim.h"
#include "va_splitting.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"


fte_t *frame_table;

void system_init(void)
{
  

    memset(mem, 0, PAGE_SIZE * NUM_FRAMES);

    frame_table = (fte_t * ) mem;

    frame_table->protected = 1;
}


uint8_t mem_access(vaddr_t addr, char access, uint8_t data)
{
    

    pte_t * pte = (pte_t * )(mem + (PTBR * PAGE_SIZE)) + vaddr_vpn(addr);
    if (!pte->valid) {

        page_fault(addr);
    }
     pte->referenced = 1;

     pfn_t pfn = pte->pfn;

     paddr_t physical_address = (paddr_t) (pfn << OFFSET_LEN | vaddr_offset(addr));


    if (access == 'r')
    {
        stats.accesses++;
        return mem[physical_address];

    }
    else
    {
        stats.accesses++;
        mem[physical_address] = data;
        pte->dirty = 1; 
  
    }

    return 0;
}