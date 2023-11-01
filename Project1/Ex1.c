#include <stdio.h>
#include <stdlib.h>
#include "dfs.h"

typedef struct{
    unsigned nedges;
    unsigned edges[8]; // limit of edges in a node = 8
}node;

int main(int argc,char *argv[]){
    FILE * defgraph;
    node * nodelist;
    unsigned i,j;
    unsigned gsize,gorder,index,dest;
    
    defgraph=fopen(argv[1],"r");
    if(defgraph==NULL){
        printf("\nERROR: Data file not found.\n");
        return -1;
    }    
    fscanf(defgraph,"%u",&gorder);
    fscanf(defgraph,"%u",&gsize);
    if ((nodelist = (node *) malloc(gorder*sizeof(node))) == NULL){
        fprintf(stderr, "\nERROR: no enought memory for allocating the nodes\n\n");
        return 2;
    }
    printf("%s defines a graph with %d nodes and %d edges.\n",argv[1],gorder,gsize);
    
    graph_node* graph = (graph_node*) malloc(gorder*sizeof(graph_node));
    char alphabet[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    
    // Initialise graph
    for (i=0; i<=gsize; i++)
    {
        graph[i].name = alphabet[i];
        graph[i].nsucc = 0;
    }

    // Creates Graph structure
    for(j=0; j<=gsize;j++)
    {
        fscanf(defgraph,"%u %u",&index,&dest);
        
        if (sizeof(graph[index].successors) == 3)
        {
            printf("\nERROR: Node %c contains more than 3 edges.\n",graph[index].name);
            return -1;
        }

        graph[index].successors[graph[index].nsucc] = dest;
        graph[index].nsucc++;
    }
    fclose(defgraph);

    // Use Depth-first search to check for graph connectivity
    int connected = check_connected(graph, gorder, 0);
    printf("%d\n", connected);
    
    return 1;
}
