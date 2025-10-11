#include <sys/mman.h>
#include <stdint.h>

#include "alloc.h"
#include "block.h"

Allocator blocks = {NULL, NULL};

void align_to_pagesize(size_t *src) {
  // if the src is already aligned the value is returned as-is.
  //
  // Otherwise, the value is packed with the page-size (-1 to keep the size valid (125b -> 4096)), then we add 1
  // to get a valid page-size multiple.
  *src = (*src | pagesize - 1) + 1;
}

void *alloc(const size_t bytes_to_reserve) {
  size_t total_size = sizeof(Header) + bytes_to_reserve;
  // align the size to reserve to be a multiple of the page-size (4096 bytes for x64 systems).
  align_to_pagesize(&total_size);

  const void *ptr = mmap(NULL, total_size, MEM_PROT_FLAGS, MEM_MAP_FLAGS, -1, 0);
  if (ptr == MAP_FAILED) {
    // failed to reserve, probably has run out of memory.
    return NULL;
  }
  Header *block = mmap(NULL, total_size, MEM_PROT_FLAGS, MEM_MAP_FLAGS, -1, 0);
  block->size = total_size;
  if (blocks.first == NULL) {
    blocks.first = block;

  }
  if (blocks.first == blocks.last) {
    blocks.first = block;
    blocks.last = block;
  } else {
    blocks.last->next = block;
    blocks.last = block;
  }
  return ptr;
}

int dealloc(void *addr) {
  if (addr == NULL) return -1;

  // const Header *block_at_addr = find_block(addr);
  // return (block_at_addr == NULL) ? -1 : munmap(addr, block_at_addr->size + sizeof(Header));
  return 0;
}