#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <stdio.h>

#define min(x,y) ({ \
  typeof (x) __x = (x); \
  typeof (y) __y = (y); \
  (__x < __y) ? __x : __y; \
})
#define max(x,y) ({ \
  typeof (x) __x = (x); \
  typeof (y) __y = (y); \
  (__x > __y) ? __x : __y; \
})

struct histogram_t {
  unsigned int bins;
  unsigned int *data;
};

void delete_histogram(struct histogram_t*);
struct histogram_t* create_empty_histogram(unsigned int);
void increment_bin(struct histogram_t*, unsigned int);
void print_histogram(FILE*, struct histogram_t*);

#endif
