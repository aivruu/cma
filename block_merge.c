#include "block_merge.h"

void *merge_blocks(header_t *first, header_t *second) {
  if (first == NULL || second == NULL) return NULL;

  if (!first->is_free || !second->is_free) {
    return NULL;
  }
  first->size += second->size;
  first->next = second->next;
  return (void *) first + MEM_HEADER_OVERHEAD;
}