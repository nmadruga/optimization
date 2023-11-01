#ifndef DSF_H_   /* Include guard */
#define DSF_H_

typedef unsigned short u_short;

typedef struct
{
    char name;
    u_short nsucc, successors[3];
} graph_node;

int check_connected(graph_node *Graph, u_short order, u_short source);

int* DFS(graph_node *Graph, u_short order, u_short source, u_short showGraph);

#endif // DSF_H_