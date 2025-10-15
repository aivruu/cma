#include <sys/mman.h>

#include "block_algorithm.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

/* The first node. */
header_t *first = NULL;
/* The last node. */
header_t *last = NULL;
/* The memory page-size for the system. */
unsigned long pagesize;

void init_pagesize() {
  pagesize = sysconf(_SC_PAGESIZE);
  printf("Page-size in memory for the system: %ld bytes\n", pagesize);
}

unsigned int align(const unsigned int src) {
  /* ----------------------
   *  Page-size alignment.
   * ----------------------
   *
   * Simple operation to align the user's bytes-amount allocated with the in memory page-size (4096 bytes for x64 systems).
   *
   * The size is "added" with the page-size (minus 1 because it could be out of bound), plus 1 to align it correctly, this operation
   * automatically rounds up the number to be a multiple of the page-size, so if the user wants to reserve just 40 bytes, the system will
   * give him a full chunk of 4K (4096 bytes).
   *
   * This operation also could return 0 if the user provides a negative value, but always we ensure the size is valid. so it won't be a
   * problem.
   */
  return (src | pagesize - 1) + 1;
}

void *alloc(unsigned int size) {
  if (size == 0) return NULL;

  size += MEM_HEADER_OVERHEAD;
  /*
   * Before reserve any memory-amount, we check if there's a free-block that has enough size to store the amount of data the user wants to,
   * if no block was found with such specifications, request a new memory-region to the OS, otherwise.
   *
   * Instead of split the block directly, we can give the user that block if the size of the node is the same as the user requested (with
   * the header-overhead included). Otherwise, we split the block to accommodate the user's data
   */
  header_t *free_block = find_free_block(size);
  if (free_block != NULL) {
    if (free_block->size == size) {
      free_block->is_free = false;
      return (void *) free_block + MEM_HEADER_OVERHEAD;
    }
    /* ----------------------------------
     *  Block-splitting algorithm logic.
     * ----------------------------------
     *
     * If there's a free-block that meets the size requested by the user, we can either give the user the entire block (if sizes match), or
     * instead, we split the block, and we gave the user the addr to that space for write the data.
     */
    header_t *next = split(free_block, size);
    if (next != NULL) {
      if (last == free_block) {
        last = next;
      }
      return (void *) next + MEM_HEADER_OVERHEAD; // user's data space
    }
  }

  /* -----------------------
   *  Memory-reserve logic.
   * -----------------------
   *
   * It is very simple, to request memory to the system first we must ensure the in-bytes amount of memory to reserve is aligned with the
   * system's in memory page-size, so the memory-accesses can be made as quickly as possible. For the flags we just specify that our memory
   * space must be private and non-shared between processes, and that the region may be readable and writable.
   *
   * As the mapping is anonymous, we set -1 for the fd and 0 for the offset;
   *
   * If the alloc fails, the most common reason is because the system doesn't have enough memory to provide, other reasons may involve
   * invalid arguments (that may or not be related to the system config).
   *
   * After the memory is allocated, we write all our metadata (that will be used to locate the block and manipulate it) at the start of the
   * address, and the next-addr (addr + header size in bytes) is which we give the user to write his data in it.
   */
  void *ptr = mmap(NULL, align(size), MEM_PROT_FLAGS, MEM_MAP_FLAGS, -1, 0);
  if (ptr == MAP_FAILED /* or (void *) -1 */) {
    perror("mmap");
    // failed to reserve, might be due to no memory available.
    return NULL;
  }
  header_t *node = ptr;
  node->size = size;
  node->is_free = false;
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

void *find_free_block(const unsigned int size) {
  header_t *start = first;
  while (start != NULL) {
    if (start->size >= size) {
      return start;
    }
    start = start->next;
  }
  return NULL;
}

void dealloc(void *addr) {
  if (addr == NULL) return;

  header_t *node = addr - MEM_HEADER_OVERHEAD;
  if (munmap(node, node->size) == -1) {
    perror("munmap");
    return;
  }
  // there's just one node left?
  if (node == last) {
    first = NULL;
    last = NULL;
    return;
  }
  header_t *current = first;
  while (current->next != NULL && current->next != last) {
    if (current->next == node) {
      merge(node, current);
    }
    current = current->next;
  }
  last = current;
}
