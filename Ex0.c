#include <stdio.h>
#include <stdlib.h>

typedef struct{
    unsigned nedges;
    unsigned edges[8]; // limit of edges in a node = 8
}node;

int main(int argc,char *argv[]){
    FILE * defgraph;
    node * nodelist;
    unsigned i,j;
    unsigned gsize,gorder,or,dest;
    
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
    for (i=0; i<gorder; i++) nodelist[i].nedges = 0;
    for (j=0; j<gsize; j++){
        fscanf(defgraph,"%u %u",&or,&dest);
        nodelist[or].edges[nodelist[or].nedges]=dest;
        nodelist[or].nedges++;
        nodelist[dest].edges[nodelist[dest].nedges]=or;
        nodelist[dest].nedges++;       
    }    
    fclose(defgraph);
    for (i=0; i<gorder; i++){
        printf("Node %u has %u edges:\n",i,nodelist[i].nedges);
        for (j=0; j<nodelist[i].nedges;j++){
            printf("  %u --> %u\n",i,nodelist[i].edges[j]);
        }
    }    
    return 0;
}
