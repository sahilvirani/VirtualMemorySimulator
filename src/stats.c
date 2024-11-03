#include "stats.h"

stats_t stats;


void compute_stats() {
    
 stats.amat = (double)((stats.writebacks * DISK_PAGE_WRITE_TIME +
 stats.accesses * MEMORY_ACCESS_TIME + 
 stats.page_faults * DISK_PAGE_READ_TIME
) / (double) stats.accesses);


} 
