#include <stdio.h>
#include <sys/utsname.h>
#include "alloc.h"

typedef struct pos_t {
  float x;
  float z;
} pos_t;

int main() {
  struct utsname sysinfo;
  if (uname(&sysinfo) == -1) {
    perror("uname");
    return -1;
  }

  printf("Getting page-size for this machine (%s architecture).\n", sysinfo.machine);
  // retrieve and store the page-size for this machine.
  get_pagesize();
  printf("Page-size in memory for the system: %ld bytes\n", pagesize);

  printf("Allocating memory for position object {x=15.0, y=20.5}.\n");
  pos_t *pos = alloc(sizeof(pos_t));
  if (pos == NULL) {
    printf("Unable to reserve memory for the object.\n");
    return -2;
  }
  pos->x = 15.0f;
  pos->z = 20.5f;
  printf("Position: {x=%.2f, z=%.2f}\n", pos->x, pos->z);

  printf("Deallocating memory for object at addr %p.\n", pos);
  dealloc(pos);
  return 0;
}