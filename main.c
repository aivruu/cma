#include <stdio.h>
#include "alloc.h"

typedef struct pos_t {
  float x;
  float z;
} pos_t;

/* Retrieves and stores the memory page-size value from the system. */
void init_pagesize(void);

char *parse_boolean_value(bool value) {
  // do i even need to explain this?
  return value ? "Yes" : "No";
}

int main(void) {
  // retrieve and store the page-size for this machine.
  init_pagesize();

  printf("Requesting memory for object-allocation...\n");
  size_t object_footprint = sizeof(pos_t);
  void *ptr = alloc(object_footprint);
  if (ptr == NULL) {
    printf("Unable to make a memory reserve.\n");
    return 1;
  }
  printf("Allocated %d bytes of memory starting at %p.\n", (int) object_footprint, ptr);
  printf("Printing block meta-data:\n");

  header_t *node = ptr - MEM_HEADER_OVERHEAD;
  printf("- is-free: %s\n", parse_boolean_value(node->is_free));
  printf("- full-block-size: %d\n", node->size);
  printf("- next-block-address: %p\n", node->next);

  pos_t *pos = ptr;
  pos->x = 15.0f;
  pos->z = 20.5f;
  printf("Metadata for position object: {x=%.2f, z=%.2f}.\n", pos->x, pos->z);

  printf("Deallocating memory for object at address %p.\n", pos);
  dealloc(pos);
  return 0;
}
