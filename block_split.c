#include "block_split.h"

void *split_block(header_t *src, const size_t reserve) {
  const size_t new_size = reserve + MEM_HEADER_OVERHEAD;
  if (src->size < new_size) {
    return NULL;
  }
  // Now that we know the block has enough space to support the user's data, we need to return an addr
  // to that new block for the user.
  //
  // current-block: | header | user-data | remaining-space
  header_t *next = src + new_size;
  next->size = src->size - reserve;
  next->is_free = 0;
  src->next = next;
  return next + MEM_HEADER_OVERHEAD;
}