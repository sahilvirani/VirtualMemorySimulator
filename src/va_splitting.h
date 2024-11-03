#pragma once

#include "mmu.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"


static inline vpn_t vaddr_vpn(vaddr_t addr) {


    return addr / PAGE_SIZE;
  
}

static inline uint16_t vaddr_offset(vaddr_t addr) {

    
    return addr % PAGE_SIZE;
}

#pragma GCC diagnostic pop