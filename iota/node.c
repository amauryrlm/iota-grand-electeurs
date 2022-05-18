#include<stdio.h>
#include "graph.h"
#include<time.h>
#include<stdlib.h>




#define MAXNODE 30
#define OPINION 0.3
#define NBNEIGHBOUR 5

static bool get_edge(struct graph_t *g, int i, int j)
{
  return g->mat[(min(i,j)*g->size)+max(i,j)];
}


struct Node{

    int rank;
    int opinion;
    struct Node **neighbours ;

};

struct Node * create_node(int rank){
    srand(time(NULL));
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    new_node->rank = rank;
    new_node->opinion = rand()%2;
    new_node->neighbours = (struct Node **)malloc(sizeof(struct Node *)*MAXNODE);
    return new_node;

}




void set_neighbours(struct Node *node, struct graph_t *g,struct Node ** nodes_of_network){
    int cnt=0;
    for(int i=0; i<g->size; i++){
       
        if(get_edge(g,node->rank,i)){
            node->neighbours[cnt] = nodes_of_network[i];
            cnt++;

        }
    }
}






int main(){

    struct Node ** nodes_of_network;

    nodes_of_network = (struct Node **) malloc(MAXNODE * sizeof(struct Node *));



    for(int i=0; i<MAXNODE; i++){
        nodes_of_network[i] = create_node(i);
    }


    int num_vertices=30; 
    int edges_per_vertex=5;
    float prob_of_swap=0.3;
    struct graph_t *g;

    g = build_regular_graph(num_vertices, edges_per_vertex);
    randomise_graph(g, prob_of_swap);

    print_graph(stdout, g);

    delete_graph(g);

    return 0;




}