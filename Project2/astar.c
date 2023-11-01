typedef char bool;

enum
{
    false,
    true
};

typedef struct
{
    unsigned vertexto;
    float weight;
} weighted_arrow;

typedef struct
{
    char name;
    unsigned arrows_num;
    weighted_arrow arrow[5];
} graph_vertex;

typedef struct
{
    float g;
    unsigned parent;
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
typedef struct
{
    float f;
    bool IsOpen;
} AStarControlData;

float heuristic(graph_vertex *Graph, unsigned vertex, unsigned goal)
{
    register unsigned short i;
    if (vertex == goal)
        return 0.0;
    float minw = Graph[vertex].arrow[0].weight;
    for (i = 1; i < Graph[vertex].arrows_num; i++)
    {
        if (Graph[vertex].arrow[i].weight < minw)
            minw = Graph[vertex].arrow[i].weight;
    }
    return minw;
}

#define GraphOrder 21
int main() {
graph_vertex Graph[GraphOrder] = {
{’A’, 3, { {1, 0.528}, {2, 0.495}, {3, 0.471} }},
{’B’, 2, { {0, 0.528}, {3, 0.508} }},
{’U’, 2, { {18, 2.510}, {19, 13.313} }} };
AStarPath PathData[GraphOrder];
unsigned node_start = 0U, node_goal = 20U;
bool r = AStar(Graph, PathData, GraphOrder, node_start, node_goal);
if(r == -1) ExitError("in allocating memory for the OPEN list in AStar", 21);
else if(!r) ExitError("no solution found in AStar", 7);
register unsigned v=node_goal, pv=PathData[v].parent, ppv; PathData[node_goal].parent=UINT_MAX;
while(v != node_start) { ppv=PathData[pv].parent; PathData[pv].parent=v; v=pv; pv=ppv; }
printf("Optimal path found:\nNode name | Distance\n----------|---------\n");
printf(" %c (%3.3u) | Source\n", Graph[node_start].name, node_start);
for(v=PathData[node_start].parent ; v !=UINT_MAX ; v=PathData[v].parent)
printf(" %c (%3.3u) | %7.3f\n", Graph[v].name, v, PathData[v].g);
return 0; }
