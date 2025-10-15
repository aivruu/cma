#include "block_algorithm.h"

#include <stddef.h>

header_t *split(header_t *src, const unsigned int reserve) {
  const unsigned int size_without_header = reserve - MEM_HEADER_OVERHEAD;
  if (src->size < size_without_header) {
    return NULL;
  }
  // HEADER | User_Data | HEADER
  header_t *next = src->next;
  next->size = src->size - size_without_header;
  next->is_free = false;
  src->next = next;
  return next;
}

void merge(header_t *first, header_t *second) {
  if (first == NULL || second == NULL) return;

  if (!first->is_free || !second->is_free) {
    return;
  }
  second->size += first->size - MEM_HEADER_OVERHEAD;
  second->next = second->next;
}