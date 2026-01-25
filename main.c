#include <stdio.h>
#include "alloc.h"

typedef struct pos_t {
  float x;
  float z;
} pos_t;

/* Retrieves and stores the memory page-size value from the system. */
void init_pagesize(void);

int main(void) {
  // retrieve and store the page-size for this machine.
  init_pagesize();

  printf("Allocating memory for position object {x=15.0, y=20.5}.\n");
  pos_t *pos = alloc(sizeof(pos_t));
  if (pos == NULL) {
    printf("Unable to reserve memory for the object.\n");
    return -2;
  }
  pos->x = 15.0f;
  pos->z = 20.5f;
  printf("Position: {x=%.2f, z=%.2f} at %p.\n", pos->x, pos->z, pos);

  printf("Deallocating memory for object at addr %p.\n", pos);
  dealloc(pos);
  return 0;
}
