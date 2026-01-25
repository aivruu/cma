#ifndef CMA_ALLOC_H
#define CMA_ALLOC_H

#include <stdbool.h>

#endif

/* Allocated memory is only readable and writable. */
#define MEM_PROT_FLAGS (PROT_READ | PROT_WRITE)
/* Ensure memory is private and is not shared between processes. */
#define MEM_MAP_FLAGS (MAP_PRIVATE | MAP_ANONYMOUS)
/* The bytes that are used for the block's header. */
#define MEM_HEADER_OVERHEAD sizeof(header_t);

/* A struct that represents a metadata-container for a block in a memory-region. */
typedef struct header_t {
  unsigned int size;     /* The total-size that this chunk uses (header + user's space). */
  bool is_free;          /* Whether the block is free for new data or not. */
  struct header_t *next; /* A pointer to the next node. */
} header_t;

/*
 * Aligns the value to be a multiple of this machine's page-size.
 * 
 * The value must be aligned if we want to access fast to read some value, the page-size
 * may change depending on the machine's architecture. For x86-64 and major machines, the page-size in
 * virtual-memory is of 4096 bytes (or 4KiB).
 *
 * The function will perform the alignment over the given value, mutating it, so no new value is returned
 * as result.
 */
void align_to_pagesize(unsigned int *value);

/*
 * Reserves the specified amount of memory (bytes) and returns a pointer to the
 * memory-block's address.
 *
 * If there's a free-block that meets the expected-size, then the block will be
 * marked as occuped and the function will return a pointer to the address ahead
 * that block.
 *
 * If the block has available-size to accomodate the user's reserved-memory, then
 * it will be splitted into another block and a pointer will be returned by the
 * function.
 *
 * If no free-block was found, we'll request another region to the operating-system,
 * as the region's size will depend on the machine virtual-memory's page-size (x64 or x86),
 * then the function will write some metadata at the start of the block to store information
 * about it and help to identify it, finally, the function will return a pointer to the address
 * ahead of the block for the user to write.
 *
 * Returns a pointer to a writable memory-chunk, or NULL if no more memory could've requested.
 */
void *alloc(unsigned int size);

/*
 * Finds a free-block which is large enough to hold the requested data amount.
 * Returns NULL if no suitable block was found. Otherwise, a pointer to that node is returned.
 */
void *find_free_block(unsigned int size);

/*
 * Frees the memory ahead of the given pointer.
 *
 * If there's a region of memory-chunks behind the released block, then it will
 * be merged within the region-block.
 */
void dealloc(void *addr);
