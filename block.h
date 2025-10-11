#ifndef CMA_BLOCK_H
#define CMA_BLOCK_H
#include <stddef.h>

#endif //CMA_BLOCK_H

// A memory-header struct which contains metadata about the block.
typedef struct Header {
  size_t size;
  struct Content *content;
  struct Header *next;
} Header;

// The actual content of a memory-block.
typedef struct Content {
  int *bytes;
} Content;

// A memory-region which contains multiple blocks.
typedef struct Allocator {
  Header *first;
  Header *last;
} Allocator;