#include "arena.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

void init_arena(arena *a, int pages) {
  long page_size = sysconf(_SC_PAGE_SIZE) * pages;

  char *buf = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  a->ptr = 0;
  a->page = buf;
  a->size = page_size;

  return;
}

void *allocate(size_t size, arena *a) {
  if (a->ptr + size > a->size) {
    // INFO: Can not allocate more memory
    return NULL;
  }

  char *new_addr = a->page + a->ptr;

  a->ptr = a->ptr + size;

  return new_addr;
}

void reset_arena(arena *a) { a->ptr = 0; }

void destroy_arena(arena *a) {
  if (munmap(a->page, a->size) < 0) {
    perror("munmap");
  }
}
