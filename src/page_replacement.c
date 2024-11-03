#include "types.h"
#include "pagesim.h"
#include "mmu.h"
#include "swapops.h"
#include "stats.h"
#include "util.h"

pfn_t select_victim_frame(void);

pfn_t last_evicted = 0;

pfn_t free_frame(void)
{
    pfn_t victim_pfn;
    victim_pfn = select_victim_frame();

 
    if (frame_table[victim_pfn].mapped)
    {
        pte_t * pte = (pte_t * ) (((frame_table[victim_pfn].process)->saved_ptbr * PAGE_SIZE) + mem)
         + (frame_table[victim_pfn].vpn);

         if (pte->dirty == 1) {

                swap_write(pte, mem + pte->pfn * PAGE_SIZE);
                stats.writebacks++;
            
         }
        frame_table[victim_pfn].mapped = 0;
     
        pte->valid = 0;
        
    }
    
    frame_table[victim_pfn].ref_count = 0;
    return victim_pfn;



}


pfn_t select_victim_frame()
{
    /* See if there are any free frames first */
    size_t num_entries = MEM_SIZE / PAGE_SIZE;
    for (size_t i = 0; i < num_entries; i++)
    {
        if (!frame_table[i].protected && !frame_table[i].mapped)
        {
            return i;
        }
    }


    if (replacement == RANDOM)
    {
        /* Play Russian Roulette to decide which frame to evict */
        pfn_t unprotected_found = NUM_FRAMES;
        for (pfn_t i = 0; i < num_entries; i++)
        {
            if (!frame_table[i].protected)
            {
                unprotected_found = i;
                if (prng_rand() % 2)
                {
                    return i;
                }
            }
        }
        /* If no victim found yet take the last unprotected frame
           seen */
        if (unprotected_found < NUM_FRAMES)
        {
            return unprotected_found;
        }
    }
    else if (replacement == APPROX_LRU)
    {

  
        pfn_t victim = num_entries;
         int min = 0xFF;
        

        

        for (size_t i = 0; i < num_entries; i++) {
            if (frame_table[i].protected == 0 && frame_table[i].ref_count < min && frame_table[i].mapped) {
                min = frame_table[i].ref_count; 
                victim = i;
            
            }
            }

        frame_table[victim].ref_count = 0;
         return victim;
        }
 


    
    else if (replacement == CLOCKSWEEP)
    {
    
         static pfn_t sweep_index = 0;  


    for (pfn_t i = 0; i < NUM_FRAMES; i++) {
        pfn_t curr = (sweep_index + i) % NUM_FRAMES; 
        
        if (!frame_table[curr].protected && frame_table[curr].mapped) {

         
             
            pte_t *pte = (pte_t *)(mem + (frame_table[curr].process->saved_ptbr * PAGE_SIZE)) +
             frame_table[curr].vpn;  

       
            if (pte->referenced) {
          
                pte->referenced = 0;
            } else {

                sweep_index = (curr + 1) % NUM_FRAMES; 
                last_evicted = curr; 
                return curr; 
            }
        }
    }


    for (pfn_t i = 0; i < NUM_FRAMES; i++) {
        pfn_t curr = (sweep_index + i) % NUM_FRAMES;  

        if (!frame_table[curr].protected && frame_table[curr].mapped) {
            sweep_index = (curr + 1) % NUM_FRAMES;  
            return curr;  
        }
    }
    }

   
    panic("System ran out of memory\n");
    exit(1);

}

void daemon_update(void)
{
 
    

         for (pfn_t i = 0; i < NUM_FRAMES; i++) {

            if (frame_table[i].mapped && !frame_table[i].protected ) {

                frame_table[i].ref_count >>=1;
                pte_t *pte = (pte_t *)(mem + frame_table[i].process->saved_ptbr * PAGE_SIZE) + frame_table[i].vpn;

            

          
            frame_table[i].ref_count |= pte->referenced<<7 ; 
             
                
             
             pte->referenced = 0;  

            }




         }

}
