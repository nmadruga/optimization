#include <stdio.h>
#include <stdlib.h> // For exit() and malloc()
#include <limits.h> // For USHRT_MAX
#include <string.h> // For memset
#include "dfs.h"

typedef struct StackElementstructure
{
    unsigned short vertex;
    struct StackElementstructure *lower;
} StackElement;

typedef StackElement *Stack;

/**
 * Check if stack is empty - no elements left
 */
int IsEmpty(Stack S) { return (S == NULL); }

/**
 * Remove last element from stack
 */
unsigned short pop(Stack *S)
{
    Stack aux = *S;
    unsigned short v = (*S)->vertex;
    *S = (*S)->lower;
    free(aux);
    return v;
}

/**
 * Add new element to end of stack
 */
int push(unsigned short vert2S, Stack *S)
{
    StackElement *aux = (StackElement *)malloc(sizeof(StackElement));
    if (aux == NULL)
        return 0;
    aux->vertex = vert2S;
    aux->lower = *S;
    *S = aux;
    return 1;
}

/**
 * Print one node in graph formatted
*/
void graph_node_print(graph_node *G, u_short v, u_short p, u_short s)
{
    if (v == s)
    {
        fprintf(stdout,
                "\n\n ordering | parent\n----------|-------\n%4c (%u) |\n", G[v].name, v);
    }
    else
        fprintf(stdout, "%4c (%u) | %c (%u)\n", G[v].name, v, G[p].name, p);
}

/**
 * Depth-first Search algorithm
 * Return vector of nodes that have been visited
*/
int* DFS(graph_node *Graph, u_short order, u_short source, u_short showGraph)
{
    register u_short i;
    u_short parent[order];
    Stack St = NULL;

    // Array of visited nodes (0 - not visited/ 1 - visited)
    int* visited = calloc(order, sizeof(unsigned int));

    // memset(visited, 0, order);
    push(source, &St);
    parent[source] = USHRT_MAX;
    while (!IsEmpty(St))
    {
        u_short node = pop(&St);
        if (visited[node])
            continue;
        visited[node] = 1;
        if (showGraph) (Graph, node, parent[node], source);
        for (i = 0; i < Graph[node].nsucc; i++)
        {
            u_short adj = Graph[node].successors[i];
            if (!visited[adj])
            {
                push(adj, &St);
                parent[adj] = node;
            }
        }
    }
    return visited;
}

int check_connected(graph_node *Graph, u_short order, u_short source)
{
    u_short showGraph = 0;
    int* visited = DFS(Graph, order, source, showGraph);
    int count = 0;
    for (size_t i = 0; i < order; i++)
        if(visited[i]) count++;

    free(visited);

    return count == order;
}
