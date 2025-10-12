#include <sys/mman.h>
#include <stdint.h>

#include "block_split.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// the first node.
header_t *first = NULL;
// the last node.
header_t *last = NULL;

void align_to_pagesize(size_t *src) {
  // -------------------------
  // Page-size alignment.
  // -------------------------
  //
  // To provide a valid size for the reserve, we need to align the value to be a multiple of the page-size for this machine, usually 4096 bytes (4KB) for
  // x64 systems.
  //
  // If the user provides a number that isn't a multiple of the page-size, the operation below will round it up to a valid multiple,
  // so the value may be 4096, 8192, etc. No matter what size the user provides as it will be rounded to a valid multiple.
  //
  // But, this operation could return 0 if the user provides a negative value, but always we ensure the size is valid (>= 0), so it won't be a problem.
  *src = (*src | pagesize - 1) + 1;
}

void *alloc(const size_t size) {
  if (size <= 0) return NULL;

  // -------------------------
  // Block-splitting algorithm logic.
  // -------------------------
  //
  // If there's a free-block that meets the size requested by the user, we can either give the user the entire block (if sizes match), or instead,
  // we split the block, and we gave the user the addr to that space for write the data.
  header_t *free_block = find_free_block(size);
  if (free_block != NULL) {
    if (free_block->size == size) {
      free_block->is_free = 0;
      return (void *) free_block + MEM_HEADER_OVERHEAD;
    }
    void *next = split_block(free_block, size);
    if (next != NULL) {
      if (last == free_block) {
        last = next;
      }
      // return the addr to the next block, this block was split from the original so its size is reduced to store the user's data.
      // next-block's addr + new block's header overhead = user's addr
      return next;
    }
    // no enough space in the block, so we need to reserve more memory.
  }

  // -------------------------
  // Memory-reserve logic.
  // -------------------------
  //
  // As no free-block existed for the user's reserve, we need to request another memory-region to the operating-system, at that region start we'll
  // write the metadata for the header (such as size, is_free or the next-block). Then we give the user the addr after the header to write his data.
  size_t total_size = size + MEM_HEADER_OVERHEAD;
  align_to_pagesize(&total_size);
  // as the mapping is anonymous, we set -1 for the fd and 0 for the offset;
  void *ptr = mmap(NULL, total_size, MEM_PROT_FLAGS, MEM_MAP_FLAGS, -1, 0);
  if (ptr == MAP_FAILED /* or (void *) -1 */) {
    perror("mmap");
    // failed to reserve, might be due to no memory available.
    return NULL;
  }
  header_t *node = ptr;
  node->size = size;
  node->is_free = 0;
  node->next = NULL;

  // update linked-list.
  if (first == NULL) {
    first = node;
    last = node;
  } else {
    last->next = node;
    last = node;
  }

  return ptr + MEM_HEADER_OVERHEAD;
}

void *find_free_block(const size_t size) {
  header_t *start = first;
  while (start != NULL) {
    if (start->is_free && start->size >= size) {
      return start;
    }
    start = start->next;
  }
  return NULL;
}

void dealloc(void *addr) {
  if (addr == NULL) return;

  // Get the header for this block.
  //
  // It shouldn't be null as the given address is valid.
  header_t *node = addr - MEM_HEADER_OVERHEAD;
  node->is_free = 1;

  // TODO: implement block-merging logic

  // if the node is not the last block, just mark it as free.
  if (node != last) {
    return;
  }
  // there's just one block left?
  if (first == last) {
    first = NULL;
    last = NULL;
    return;
  }
  // the block is neither the first nor the last, so we need to find it.
  header_t *current = first;
  while (current->next != NULL && current->next != last) {
    current = current->next;
  }
  last = current;

  // we could implement a logic to handle the sbrk here as this block is in the "middle" of the memory-region, but as I'm using mmap, I won't do it by now.
}
