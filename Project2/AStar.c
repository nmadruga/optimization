#ifndef A_STAR_H 
#define A_STAR_H 

#include <stdio.h>
#include <stdlib.h>
#include <values.h> // For MAXFLOAT = \infty and UINT_MAX = \infty
#include "node.h"
#include "write_map_binary.h"
#include "read_map_binary.h"

typedef char bool;

enum
{
    false,
    true
};

// typedef struct
// {
//     unsigned vertexto;
//     float weight;
// } weighted_arrow;

// typedef struct
// {
//     char name;
//     unsigned arrows_num; SAME AS NSUCC
//     weighted_arrow arrow[5];
// } graph_vertex;

typedef struct
{
    float f;
    bool IsOpen;
} AStarControlData;

typedef struct
{
    float g;
    long unsigned parent;
} AStarPath;

void ExitError(const char *miss, int errcode)
{
    fprintf(stderr, "\nERROR: %s.\nStopping...\n\n", miss);
    exit(errcode);
}

typedef struct QueueElementstruct
{
    unsigned v;
    struct QueueElementstruct *seg;
} QueueElement;
typedef QueueElement *PriorityQueue;

float heuristic(node *Graph, unsigned vertex, unsigned goal)
{
    register unsigned short i;
    if (vertex == goal)
        return 0.0;
    float minw = Graph[vertex].successors[0].weight;
    for (i = 1; i < Graph[vertex].nsucc; i++)
    {
        if (Graph[vertex].successors[i].weight < minw)
            minw = Graph[vertex].successors[i].weight;
    }
    return minw;
}

bool IsEmpty(PriorityQueue Pq)
{
    return ((bool)(Pq == NULL));
}

unsigned extract_min(PriorityQueue *Pq)
{
    PriorityQueue first = *Pq;
    unsigned v = first->v;
    *Pq = (*Pq)->seg;
    free(first);
    return v;
}

bool add_with_priority(unsigned v, PriorityQueue *Pq, AStarControlData *Q)
{
    register QueueElement *q;
    QueueElement *aux = (QueueElement *)malloc(sizeof(QueueElement));
    if (aux == NULL)
        return false;
    aux->v = v;
    float costv = Q[v].f;
    Q[v].IsOpen = true;
    if (*Pq == NULL || !(costv > Q[(*Pq)->v].f))
    {
        aux->seg = *Pq;
        *Pq = aux;
        return true;
    }
    for (q = *Pq; q->seg && Q[q->seg->v].f < costv; q = q->seg)
        ;
    aux->seg = q->seg;
    q->seg = aux;
    return true;
}

void requeue_with_priority(unsigned v, PriorityQueue *Pq, AStarControlData *Q)
{
    register QueueElement *prepv;
    if ((*Pq)->v == v)
        return;
    for (prepv = *Pq; prepv->seg->v != v; prepv = prepv->seg)
        ;
    QueueElement *pv = prepv->seg;
    prepv->seg = pv->seg;
    free(pv);
    add_with_priority(v, Pq, Q);
}

bool AStar(node* Graph, AStarPath *PathData,
           unsigned node_start, unsigned node_goal)
{
    register unsigned i;
    PriorityQueue Open = NULL;
    AStarControlData *Q;
    if ((Q = (AStarControlData *)malloc(GrOrder * sizeof(AStarControlData))) == NULL)
        ExitError("when allocating memory for the AStar Control Data vector", 73);
    for (i = 0; i < GrOrder; i++)
    {
        PathData[i].g = MAXFLOAT;
        Q[i].IsOpen = false;
    }
    PathData[node_start].g = 0.0;
    PathData[node_start].parent = ULONG_MAX;
    Q[node_start].f = heuristic(Graph, node_start, node_goal);
    if (!add_with_priority(node_start, &Open, Q))
        return -1;
    while (!IsEmpty(Open))
    {
        unsigned node_curr;
        if ((node_curr = extract_min(&Open)) == node_goal)
        {
            free(Q);
            return true;
        }
        for (i = 0; i < Graph[node_curr].nsucc; i++)
        {
            unsigned node_succ = Graph[node_curr].successors[i].vertexto;
            float g_curr_node_succ = PathData[node_curr].g + Graph[node_curr].successors[i].weight;
            if (g_curr_node_succ < PathData[node_succ].g)
            {
                PathData[node_succ].parent = node_curr;
                Q[node_succ].f = g_curr_node_succ + ((PathData[node_succ].g == MAXFLOAT) ? heuristic(Graph, node_succ, node_goal) : (Q[node_succ].f - PathData[node_succ].g));
                PathData[node_succ].g = g_curr_node_succ;
                if (!Q[node_succ].IsOpen)
                {
                    if (!add_with_priority(node_succ, &Open, Q))
                        return -1;
                }
                else
                    requeue_with_priority(node_succ, &Open, Q);
            }
        }
        Q[node_curr].IsOpen = false;
    } /* Main loop while */
    return false;
}

int main(int argc,char *argv[])
{
    write_map_binary("data/andorra.csv");
    node* graph = read_map_binary("data/andorra.csv.bin");
    if (graph) {
        // graph_vertex Graph[GraphOrder] = 
        // AStarPath PathData[GraphOrder]; 
        AStarPath* PathData;
        unsigned node_start = 0U, node_goal = 20U;
        // bool r = AStar(graph, PathData, GraphOrder, node_start, node_goal);
        bool result = AStar(graph, PathData, node_start, node_goal);
        if (result == -1)
            ExitError("in allocating memory for the OPEN list in AStar", 21);
        else if (!result)
            ExitError("no solution found in AStar", 7);
        register unsigned v = node_goal, pv = PathData[v].parent, ppv;
        PathData[node_goal].parent = UINT_MAX;
        while (v != node_start)
        {
            ppv = PathData[pv].parent;
            PathData[pv].parent = v;
            v = pv;
            pv = ppv;
        }
        printf("Optimal path found:\nNode name | Distance\n----------|---------\n");
        printf(" %s (%3.3u) | Source\n", graph[node_start].name, node_start);
        for (v = PathData[node_start].parent; v != UINT_MAX; v = PathData[v].parent)
            printf(" %s (%3.3u) | %7.3f\n", graph[v].name, v, PathData[v].g);
        return 0;
    }
}

#endif