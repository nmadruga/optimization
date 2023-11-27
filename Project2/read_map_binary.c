#ifndef READ_MAP_BINARY_H 
#define READ_MAP_BINARY_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "node.h"
#include "read_map_binary.h"

/**
 * @private deallocate_and_close
 * Free all allocated memory and close the file
 * @param graph Pointer to vector with all nodes
 * @param graph_size        Number of nodes in graph
 * @param binmapfile    Binaryfile to be closed
*/
void deallocate_and_close(node* graph, size_t graph_size, FILE* binmapfile)
{
     for (unsigned long i = 0; i < graph_size; i++) {
         free(graph[i].name);
         free(graph[i].successors);
     }
     free(graph);

    fclose(binmapfile);
}

int read_node(node* graph_node, FILE* file)
{
    // First read the node information
    if (fread(graph_node, sizeof(node), 1, file) != 1)
    {
         // When there is a problem - deallocate and exit
        perror("Error reading the node\n");
        return 1;
    }

    if(graph_node->name_len > 0)
    {
        // Allocate memory for the name and read it
        graph_node->name = (char *)malloc((graph_node->name_len+1)*sizeof(char));
        // When there is a problem - deallocate and exit
        if (fread(graph_node->name, sizeof(char), graph_node->name_len + 1, file) != graph_node->name_len + 1)
        {
            perror("Error reading the name\n");
            return 1;
        }
    }
    else
        graph_node->name = NULL;

    if(graph_node->nsucc > 0)
    {
        size_t nsucc = graph_node->nsucc;
        // Allocate memory for the list of successors nodes
        graph_node->successors = (weighted_arrow*)malloc(nsucc*sizeof(weighted_arrow));
        if (fread(graph_node->successors, sizeof(weighted_arrow), nsucc, file) != nsucc)
        {
            perror("Error reading sucessors\n");
            return 1;
        }
    }
    else
        graph_node->successors = NULL;

    return 0;
}

/**
 * @public read_map_binary
 * Read from csv file that stores nodes and edges
 * create a graph structure and save to binary file 
*/
node* read_map_binary(char * filename, size_t* graph_size) {
    clock_t start_time, end_time;
    double cpu_time_used;
    FILE *binmapfile;

    start_time = clock();

    printf("Reading binary file: %s\n", filename);
    binmapfile = fopen(filename,"rb");
    if (binmapfile == NULL) {
        perror("Error opening the binary binmapfile");
        return NULL;
    }

    if (fread(graph_size, sizeof(unsigned long), 1, binmapfile) != 1) {
        perror("Error reading the number of nodes");
        fclose(binmapfile);
        return NULL;
    }

    printf("Num nodes: %lu\n",*graph_size);

    // Allocate the structure for the graph
    node *nodes = (node *)malloc((*graph_size) * sizeof(node));
    if (nodes == NULL) {
        perror("Error allocating memory for nodes");
        deallocate_and_close(nodes, *graph_size, binmapfile);
        return NULL;
    }

    // Start reading each node
    for (unsigned long i = 0; i < *graph_size; i++)
    {
        if(read_node(&nodes[i], binmapfile) == 1)
        {
            deallocate_and_close(nodes, *graph_size, binmapfile);
            return NULL;
        }
    }

    // Call close file in the end
    fclose(binmapfile);

    // Record the end time
    end_time = clock();

    // Calculate the CPU time used
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    // Display the result
    printf("Read_map_binary - CPU Time Used: %f seconds\n", cpu_time_used);

    // Does not deallocate because we are returning the structure of graph
    return nodes;
}

#endif
