#include <stdio.h>
#include <stdlib.h>

static int MB = 1024*1024;

static int sum = 0;

void* tryAllocate(size_t size)
{
  sum += size/MB;
  void* ptr = malloc(size);
  if (!ptr) {
    printf("FAILED allocating %i MB.\n", size/MB);
    return 0;
  }
  
  printf("allocated %i MB.\n", size/MB);
  return ptr;
}

int main(int argc, char *argv[])
{
  size_t size = 100;
  if (argc == 2) {
    size = atoi(argv[1]);
  }
  
  void* ptr1 = tryAllocate(size * MB);
  void* ptr2 = tryAllocate(1500 * MB);
  size_t re = 10 * size;
  void* ptr3 = realloc(ptr1, re  * MB);
  sum += re - size;
  
  int ret = 0;
  if (!ptr3) {
    printf("FAILED reallocating %i MB.\n", re);
    ret = 1;
  } else {
    printf("reallocated %i MB.\n", re);
  }
  
  if (ret == 0)
    printf("allocated %i MB in sum\n", sum);
   else
    printf("FAILED to allocate %i MB in sum\n", sum);
   
  return 0;
}
