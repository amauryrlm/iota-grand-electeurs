#include "histogram.h"
#include <stdlib.h>

/**
 * Creates an empty histogram with number of bins given as a 
 * parameter
 */
struct histogram_t* create_empty_histogram(unsigned int bins) 
{
  struct histogram_t *h = (struct histogram_t*) malloc(sizeof(struct histogram_t));
  h->data = (unsigned int*) calloc(bins, sizeof(unsigned int));
  h->bins = bins;
  return h;
}

/**
 * Increments the corresponding bin in the histogram
 */
void increment_bin(struct histogram_t *h, unsigned int bin)
{
  h->data[bin]++;
}

/**
 * Prints the histogram to file in a fairly crap way
 * but gives a quick look at the distribution of degrees
 */
void print_histogram(FILE *f, struct histogram_t *h)
{
  unsigned int max = 0;
  for (int i = 0; i < h->bins; i++)
    max = max(max, h->data[i]);

  const int LINES = 20;
  for (int i = 0; i < LINES; i++)
  {
    for (int j = 0; j < h->bins; j++)
      fprintf(f, (double) h->data[j] / max >= (double) (LINES - i) / LINES ? "[ ]" : "   ");
    fprintf(f, "\n");
  }
  for (int i = 0; i < h->bins; i++)
    fprintf(f, "%2u ", i);
  fprintf(f, "\n");
}

void delete_histogram(struct histogram_t* h) 
{
  free(h->data);
  free(h);
}


