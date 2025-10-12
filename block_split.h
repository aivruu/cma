#ifndef CMA_BLOCK_SPLIT_H
#define CMA_BLOCK_SPLIT_H

#endif //CMA_BLOCK_SPLIT_H

#include "alloc.h"

/*
 * Tries to split the given block and return a pointer to the next-block with a lower size to accommodate the space in-memory requested.
 * Returns NULL if the block has run out of space.
 */
void *split_block(header_t *src, size_t reserve);
