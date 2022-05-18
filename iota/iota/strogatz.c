#include <stdio.h>
#include <stdlib.h>
#include "graph.h"



int main(int argc, char** argv)
{
  int num_vertices=30; 
  int edges_per_vertex=5;
  float prob_of_swap=0.3;
  struct graph_t *g;


  fprintf(stderr,
      "Configuration:\n"
      "  Vertices:         %d\n"
      "  Edges per vertex: %d\n"
      "  Swap probability: %f\n\n",
      num_vertices, edges_per_vertex, prob_of_swap);

  g = build_regular_graph(num_vertices, edges_per_vertex);
  randomise_graph(g, prob_of_swap);

  double avg_path = get_average_path_length(g);
  double global_cc = get_global_clustering_coefficient(g);
  
  fprintf(stderr,
      "Statistics:\n"
      "  Average path length:            %f\n"
      "  Global clustering coefficient:  %f\n"
      "  Degree distribution:\n\n",
      avg_path, global_cc);


  print_graph(stdout, g);

  delete_graph(g);

  return 0;


}
