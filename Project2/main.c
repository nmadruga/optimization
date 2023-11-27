#ifndef A_STAR_H 
#define A_STAR_H 

#include <stdio.h>
#include <stdlib.h>
#include <values.h> // For MAXFLOAT = \infty and UINT_MAX = \infty
#include <time.h>
#include "node.h"
#include "main.h"
#include "file_util.h"
#include "write_map_binary.h"
#include "read_map_binary.h"

/**
 * @private exit_error
 * Show error to user before existing
 * @param miss Text with error
 * @param errcode code for error
*/
void exit_error(const char *miss, int errcode)
{
    fprintf(stderr, "\nERROR: %s.\nStopping...\n\n", miss);
    exit(errcode);
}

/**
 * @private heuristic
 * Find which vertex has best heuristic value
 * to be selected as the minimal value returned
 * @param Graph Pointer to graph with nodes
 * @param vertex Index of node to be analised
 * @param goal   Index of Node of goal in map 
*/
float heuristic(node *Graph, unsigned vertex, unsigned goal)
{
    register unsigned short i;
    if (vertex == goal)
        return 0.0;
    if(Graph[vertex].nsucc == 0)
        return 0.0;
    float minw = Graph[vertex].successors[0].weight;
    for (i = 1; i < Graph[vertex].nsucc; i++)
    {
        if (Graph[vertex].successors[i].weight < minw)
            minw = Graph[vertex].successors[i].weight;
    }
    return minw;
}

/**
 * @private is_empty
 * Check if PriorityQueue is empty
 * @param Pq PriorityQueue pointer
 * @return boolen True/False
*/
bool is_empty(PriorityQueue Pq)
{
    return ((bool)(Pq == NULL));
}

/**
 * @private extract_min
 * Remove minimal and extract from Priority Queue
 * @param Pq PriorityQueue poiter
 * @return cost of minimal vertex
*/
unsigned extract_min(PriorityQueue *Pq)
{
    PriorityQueue first = *Pq;
    unsigned v = first->vertex;
    *Pq = (*Pq)->next;
    free(first);
    return v;
}

/**
 * @private add_with_priority
 * Add vertex in PriorityQueue sorted by smaller costs
 * @param vertex    Index of vertex to be analised 
 * @param Pq PriorityQueue pointer
 * @param Q  vector with already seen vertexes from PriorityQueue
 * @return boolean if the vertex was added successfully
*/
bool add_with_priority(unsigned vertex, PriorityQueue *Pq, AStarControlData *Q)
{
    register QueueElement *q;
    QueueElement *aux = (QueueElement *)malloc(sizeof(QueueElement));
    if (aux == NULL)
        return false;
    aux->vertex = vertex;
    float costv = Q[vertex].cost;
    Q[vertex].IsOpen = true;
    if (*Pq == NULL || !(costv > Q[(*Pq)->vertex].cost))
    {
        aux->next = *Pq;
        *Pq = aux;
        return true;
    }
    for (q = *Pq; q->next && Q[q->next->vertex].cost < costv; q = q->next)
        ;
    aux->next = q->next;
    q->next = aux;
    return true;
}

/**
 * @private requeue_with_priority
 * Function to re-queue vertex in PriorityQueue
 * @param vertex    Index of vertex to be analised 
 * @param Pq PriorityQueue pointer
 * @param Q  vector with already seen vertexes from PriorityQueue
 * @return boolean if the vertex was added successfully
*/
void requeue_with_priority(unsigned vertex, PriorityQueue *Pq, AStarControlData *Q)
{
    register QueueElement *prepv;
    if ((*Pq)->vertex == vertex)
        return;
    for (prepv = *Pq; prepv->next->vertex != vertex; prepv = prepv->next)
        ;
    QueueElement *pv = prepv->next;
    prepv->next = pv->next;
    free(pv);
    add_with_priority(vertex, Pq, Q);
}

/**
 * @private a_star 
 * Runs A-Star algorith looks for the optimal path
 * in the Graph between node start and node goal.
 * It stores the optimal path on returned PathData
 * @param Graph     Pointer to graph of nodes
 * @param Graphsize Number of nodes in Graph
 * @param PathData  Pointer to vertex which will store optimal path
 * @param node_start    Index of start node (of the path)
 * @param node_node     Index of goal node (of the path) 
*/
bool AStar(node* Graph, unsigned long Graphsize, AStarPath *PathData, 
           unsigned node_start, unsigned node_goal)
{
    clock_t start_time, end_time;
    double cpu_time_used;
    register unsigned i;
    PriorityQueue Open = NULL;
    AStarControlData *Q;

    start_time = clock();

    if ((Q = (AStarControlData *)malloc(Graphsize * sizeof(AStarControlData))) == NULL)
        exit_error("when allocating memory for the AStar Control Data vector", 73);
    for (i = 0; i < Graphsize; i++)
    {
        PathData[i].sum_weights = MAXFLOAT;
        Q[i].IsOpen = false;
    }
    PathData[node_start].sum_weights = 0.0;
    PathData[node_start].parent = ULONG_MAX;
    Q[node_start].cost = heuristic(Graph, node_start, node_goal);
    if (!add_with_priority(node_start, &Open, Q))
        return -1;
    while (!is_empty(Open))
    {
        unsigned node_curr;
        if ((node_curr = extract_min(&Open)) == node_goal)
        {
            printf("Found goal node: %d\n", node_curr);
            free(Q);

            // Record the end time
            end_time = clock();

            // Calculate the CPU time used
            cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

            // Display the result
            printf("AStar - CPU Time Used: %f seconds\n", cpu_time_used);

            return true;
        }
        for (i = 0; i < Graph[node_curr].nsucc; i++)
        {
            unsigned node_succ = Graph[node_curr].successors[i].vertexto;
            float g_curr_node_succ = PathData[node_curr].sum_weights + Graph[node_curr].successors[i].weight;
            if (g_curr_node_succ < PathData[node_succ].sum_weights)
            {
                PathData[node_succ].parent = node_curr;
                Q[node_succ].cost = g_curr_node_succ + ((PathData[node_succ].sum_weights == MAXFLOAT) ? heuristic(Graph, node_succ, node_goal) : (Q[node_succ].cost - PathData[node_succ].sum_weights));
                PathData[node_succ].sum_weights = g_curr_node_succ;
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

/**
 * @public Main
*/
int main(int argc,char *argv[])
{
    if (argc != 4)
    {
        exit_error("Error: Invalid number of arguments.\n",1);
        return 1; // Return an error code
    }

    // Check if the second and third arguments are integers
    char *endptr1, *endptr2;
    long int1 = strtol(argv[2], &endptr1, 10);
    long int2 = strtol(argv[3], &endptr2, 10);

    // Check for conversion errors
    if (*endptr1 != '\0' || *endptr2 != '\0')
    {
        exit_error("Error: Invalid integer argument(s).\n", 1);
        return 1; // Return an error code
    }

    char* filename = argv[1];
    // printf("Start reading csv file\n");
    // write_map_binary(filename);
    rename_extension(filename,".bin");
    unsigned long graph_size;
    node* graph = read_map_binary(filename, &graph_size);
    if (graph && graph_size) {
        AStarPath* PathData = malloc(graph_size*sizeof(AStarPath));
        if (PathData == NULL)
        {
            exit_error("in allocating memory for the PATH_DATA structure", 21);
        }
        unsigned long node_start_index = search_node(int1, graph, graph_size);
        unsigned long node_goal_index = search_node(int2, graph, graph_size);
        printf("Start node: %ld\nEnd node: %ld\n",node_start_index, node_goal_index);


        bool result = AStar(graph, graph_size, PathData, node_start_index, node_goal_index);
        if (result == -1)
            exit_error("in allocating memory for the OPEN list in AStar", 21);
        else if (!result)
            exit_error("no solution found in AStar", 7);
        
        register unsigned v = node_goal_index, pv = PathData[v].parent, ppv;
        PathData[node_goal_index].parent = UINT_MAX;
        unsigned long count = 0;
        while (v != node_start_index)
        {
            ppv = PathData[pv].parent;
            PathData[pv].parent = v;
            v = pv;
            pv = ppv;
            count++;
        }
        printf("Optimal path found with %ld nodes\n",count);
        rename_extension(filename,"_result.csv");
            // Open the CSV file for writing
        FILE *csvFile = fopen(filename, "w");

        // Check if the file was opened successfully
        if (csvFile == NULL) {
            perror("Error opening the file");
            return 1;
        }

        // Write header to the CSV file
        fprintf(csvFile, "Latitude,Longitude\n");

        // Write data to the CSV file
        for (v = PathData[node_start_index].parent; v != UINT_MAX; v = PathData[v].parent) 
            fprintf(csvFile, "%f,%f\n", graph[v].lat, graph[v].lon);

        // Close the CSV file
        fclose(csvFile);

        printf("Data has been written to output.csv\n");
        return 0;
    }
}

#endif
