#include <stdio.h>
#include <stdlib.h>

#include "rv_emu.h"
    
void cache_init(struct cache_st *csp) {

    if (csp->type == CACHE_NONE) {
        return;
    }

    csp->block_mask = (csp->block_size) - 1;
    csp->index_mask = ((csp->size / csp->block_size) / csp->ways) - 1;

    csp->index_bits = 0;
    while (csp->index_mask & (1 << csp->index_bits)) {
              csp->index_bits++;
    }

    csp->block_bits = 0;
    while (csp->block_mask & (1 << csp->block_bits)) {
              csp->block_bits++;
    }

    for (int i = 0; i < CACHE_MAX_SLOTS; i++) {
        csp->slots[i].valid = 0;
        csp->slots[i].tag = 0;
        for (int j = 0; j < CACHE_MAX_BLOCK_SIZE; j++) {
            csp->slots[i].block[j] = 0;
        }
        // timestamp only used for SA cache
        csp->slots[i].timestamp = 0;
    }

    csp->refs = 0;
    csp->hits = 0;
    csp->misses = 0;
    csp->misses_cold = 0;
    csp->misses_hot = 0;

    verbose("Cache initialized.\n");
}

void cache_print(struct cache_st *csp, char *name) {
	int num_slots;
    int num_slots_used = 0;
    int i;

    num_slots = csp->size / csp->block_size;

    for (i = 0; i < num_slots; i++) {
        if (csp->slots[i].valid == 1) {
            num_slots_used += 1;
        }
    }

    printf("=== Cache %s\n", name);
    printf("Type          = ");
    if (csp->type == CACHE_DM) {
        printf("direct mapped\n");
    } else if (csp->type == CACHE_SA) {
        printf("set associative\n");
    }
    printf("Size          = %d slots\n", csp->size);
    printf("Block size    = %d words\n", csp->block_size);
    printf("Ways          = %d\n", csp->ways);
    printf("References    = %d\n", csp->refs);
    printf("Hits          = %d (%.2f%% hit ratio)\n", csp->hits, 
           ((double) csp->hits / (double) csp->refs) * 100.00);
    printf("Misses        = %d (%.2f%% miss ratio)\n", csp->misses,
           ((double) csp->misses / (double) csp->refs) * 100.00);    
    printf("Misses (cold) = %d\n", csp->misses_cold);
    printf("Misses (hot)  = %d\n", csp->misses_hot);
    printf("%% Used        = %.2f%%\n", ((double) num_slots_used / (double) num_slots) * 100.0);    
}

struct cache_slot_st* find_lru_slot(struct cache_st *csp, int set_base) {
	struct cache_slot_st *lru_slot = &csp->slots[set_base];

	for (int i = 1; i < csp->ways; i++) {
		if (csp->slots[set_base + i].timestamp < lru_slot->timestamp) {
			lru_slot = &csp->slots[set_base + i];
		}
	}

	return lru_slot;
}

// Direct mapped lookup
uint32_t cache_lookup_dm(struct cache_st *csp, uint64_t addr) {
    uint64_t tag;
    uint64_t index;
    uint64_t b_index;
    struct cache_slot_st *slot;
    uint32_t data = 0;

    // Calculate the block index from the address
    b_index = (addr >> 2) & csp->block_mask;
    
    // Calculate the cache index from the address
    index = (addr >> (csp->block_bits + 2)) & csp->index_mask;
    
    // Calculate the tag from the address
    tag = addr >> (csp->index_bits + csp->block_bits + 2);

    // Get the cache slot corresponding to the index
    slot = &csp->slots[index];

    // Increment the reference count
    csp->refs += 1;
    
    // Check if the slot is valid and the tag matches
    if (slot->valid && (slot->tag == tag)) {
        // Cache hit
        csp->hits += 1;
        verbose("  cache tag hit for index %d tag %X addr %lX\n", index, tag, addr);
    } else {
        // Cache miss
        csp->misses += 1;
        if (slot->valid == 0) {
            // Cold miss (slot was not previously used)
            csp->misses_cold += 1;
            verbose("  cache tag (%X) miss for index %d tag %X addr %X (cold)\n", slot->tag, index, tag, addr);
        } else {
            // Hot miss (slot was previously used but with a different tag)
            csp->misses_hot += 1;
            verbose("  cache tag (%X) miss for index %d tag %X addr %X (hot)\n", slot->tag, index, tag, addr);
        }
        // Update the slot with the new tag
        slot->valid = 1;
        slot->tag = tag;

        // Load the block into the cache slot
        for (int i = 0; i < csp->block_size; i++) {
        	// shift block mask by 2 (or mult by 4)
			uint64_t shifted = csp->block_mask << 2;

			// invert shifted block mask 
			uint64_t inverted = ~shifted;

			// align address by masking it with inverted block mask
			uint64_t aligned = addr & inverted;

			// calc offset by adding i and 4 
			// moves forward by i 32 bit words in mem 
			uint32_t *add_ptr = (uint32_t *)aligned;

			//update slot block at i
			slot->block[i] = add_ptr[i];
        }
    }

    // Return the data from the block
    data = slot->block[b_index];
    return data;
}

uint32_t cache_lookup_sa(struct cache_st *csp, uint64_t addr) {
    bool hit = false;
    uint32_t value;

    // If cache size is zero, return a miss
    if (0 == csp->size) {
        csp->misses += 1;
        return *((uint32_t *) addr);
    }

    // Increment the reference count
    csp->refs += 1;

    // Calculate the tag from the address
    uint64_t tag = addr >> (csp->index_bits + csp->block_bits + 2);

    // Calculate the block index from the address
    uint64_t b_index = (addr >> 2) & csp->block_mask;

    // Calculate the set index from the address
    int set_index = (addr >> (csp->block_bits + 2)) & csp->index_mask;
    int set_base = set_index * csp->ways;

    struct cache_slot_st *slot = NULL;
    struct cache_slot_st *slot_invalid = NULL;

    // Check each slot in the set
    for (int i = 0; i < csp->ways; i += 1) {
        slot = &csp->slots[set_base + i];
        if (slot->valid) {
            if (tag == slot->tag) {
                // Cache hit
                verbose("  cache tag hit for set %d way %d tag %X addr %lX\n", set_index, i, tag, addr);
                hit = true;
                csp->hits++;
                break;
            }
        } else {
            // Save invalid slot in case of miss
            slot_invalid = slot;
        }
    }

    if (!hit) {
        if (slot_invalid) {
            // Use the invalid slot for the new data
            slot = slot_invalid;
            verbose("  cache tag (%X) miss for set %d tag %X addr %X (fill invalid slot)\n", slot->tag, set_index, tag, addr);
			csp->misses += 1;
            csp->misses_cold += 1;
        } else {
            slot = find_lru_slot(csp, set_base);
            verbose("  cache tag (%X) miss for set %d tag %X addr %X (evict address %X)\n", slot->tag, set_index, tag, addr, ((slot->tag << (csp->index_bits + 2)) | (set_index << 2)));
            csp->misses += 1;
            csp->misses_hot += 1;
        }
        // Update the slot with the new tag and data
        for (int i = 0; i < csp->block_size; i++) {
	       	// shift block mask by 2 (or mult by 4)
			uint64_t shifted = csp->block_mask << 2;

			// invert shifted block mask 
			uint64_t inverted = ~shifted;

			// align address by masking it with inverted block mask
			uint64_t aligned = addr & inverted;

			// calc offset by adding i and 4 
			// moves forward by i 32 bit words in mem 
			uint32_t *add_ptr = (uint32_t *)aligned;

			//update slot block at i
			slot->block[i] = add_ptr[i];
        }  
        
        slot->tag = tag;
        slot->valid = true;
    }

    // Return the data from the block
    value = slot->block[b_index];        
    slot->timestamp = csp->refs;
    return value;
}


// Cache lookup
uint32_t cache_lookup(struct cache_st *csp, uint64_t addr) {
    uint32_t data;

    if (csp->type == CACHE_DM) {
        data = cache_lookup_dm(csp, addr);
    } else if (csp->type == CACHE_SA) {
        data = cache_lookup_sa(csp, addr);
    } else {
        data = *((uint32_t *) addr);
    }
    return data;
}
