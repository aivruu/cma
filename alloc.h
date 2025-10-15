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
  unsigned int size;      /* The total-size that this chunk uses (header + user's space). */
  bool is_free;           /* Whether the block is free for new data or not. */
  struct header_t *next;  /* A pointer to the next node. */
} header_t;

/* Retrieves and stores the memory page-size value from the system. */
void init_pagesize();

/*
 * Aligns the given bytes-amount to be a multiple of the page-size for this machine.
 * e.g. x64 systems -> 4096 bytes (in most cases).
 * The provided value is changed in-place.
 */
unsigned int align(unsigned int src);

/*
 * Basic alloc-functionality, allocates the requested numbers of bytes and returns a pointer to it.
 * Returns NULL if the allocation failed (e.g. out of memory). Otherwise, a pointer to the memory-block is returned.
 */
void *alloc(unsigned int size);

/*
 * Finds a free-block (header_t) which is large enough to hold the requested size.
 * Returns NULL if no suitable block was found. Otherwise, a pointer to that node is returned.
 */
void *find_free_block(unsigned int size);

/*
 * Deallocates the memory at the given pointer.
 * Returns 0 on success, -1 on failure (e.g. invalid address).
 */
void dealloc(void *addr);