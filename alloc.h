#ifndef CMA_ALLOC_H
#define CMA_ALLOC_H
#include <stdbool.h>
#endif

#include <stddef.h>
#include <unistd.h>

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

/*
 * Page size of the system that is used to align the memory allocations to work with the system.
 *
 * As far as I know, some syscalls for memory-allocation may not provide a correct or aligned block-size (memory) within the page-size,
 * so we need to align that value to be "valid" with page-size which may variate depending on the system-architecture (x86 or x64).
 */
static unsigned long pagesize;
static void get_pagesize() {
  pagesize = sysconf(_SC_PAGESIZE);
}

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