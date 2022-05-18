#ifndef GRAPH_H
#define GRAPH_H

#include "histogram.h"
#include <stdio.h>
#include <stdbool.h>

struct graph_t {
  bool *mat;
  unsigned int *dist_mat;
  int size;
  int edges_per_vertex;
};

struct graph_t* build_unconnected_graph(int num_vertices);
struct graph_t* build_regular_graph(int num_vertices, int edges_per_vertex);
double get_average_path_length(struct graph_t*);
struct histogram_t* get_degree_distribution(struct graph_t*);
double get_local_clustering_coefficient(struct graph_t*, int);
double get_global_clustering_coefficient(struct graph_t*);
void randomise_graph(struct graph_t*, float);
unsigned int get_degree(struct graph_t*, int);
unsigned int get_distance(struct graph_t*, int, int);
void print_graph(FILE*, struct graph_t*);
void delete_graph(struct graph_t*);
static bool get_edge(struct graph_t *g, int i, int j);
#endif

