# cma
A C memory-allocator that I written for practice-purposes and for memory-allocation understanding.

## Explanation
The functionality is very simple, two main functions to reserve and free memory in a bytes-size, if there's already a free-block that has enough space to accomodate the data, we give the user that ptr `(next + header-overhead)`. Otherwise, we make a memory-reserve, the given size is aligned within the memory page-size for the system (`x64` so `4096 bytes` in my case) so even if the user just wants, let's say `40` bytes, the smaller memory-piece the system will give is `4096 bytes`.

Each block is linked to the next-one, for the `alloc` and `dealloc` functions I implemented some algorithms for testing and try to optimize the space reserved.
