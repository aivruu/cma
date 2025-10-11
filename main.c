#include <stdio.h>
#include "alloc.h"

int main() {
  // retrieve and store the page-size for this machine.
  get_pagesize();

  printf("Hello World!\n");
  printf("Page-size in memory for this machine: %ld bytes\n", pagesize);
  return 0;
}