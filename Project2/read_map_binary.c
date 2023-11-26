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
 * @param nnodes        Number of nodes in graph
 * @param binmapfile    Binaryfile to be closed
*/
void deallocate_and_close(node* graph, size_t nnodes, FILE* binmapfile)
{
     for (unsigned long i = 0; i < nnodes; i++) {
         free(graph[i].name);
         free(graph[i].successors);
     }
     free(graph);

    fclose(binmapfile);
}

int read_node(node* node, FILE* file)
{
    // First read the node information
    if (fread(node, sizeof(node), 1, file) != 1)
    {
         // When there is a problem - deallocate and exit
        perror("Error reading the node\n");
        return 1;
    }
    printf("Reading node: name_len: %d\n nsucc: %d\nlat: %f lon: %f\n",node->name_len,  node->nsucc, node->lat, node->lon);

    if(node->name_len > 0)
    {
        size_t name_len = node->name_len;
        // Allocate memory for the name and read it
        node->name = (char *)malloc((name_len+1)*sizeof(char));
        // When there is a problem - deallocate and exit
        if (fread(node->name, sizeof(char), name_len, file) != name_len)
        {
            perror("Error reading the name\n");
            return 1;
        }
    }
    else
        node->name = NULL;

    if(node->nsucc > 0)
    {
        size_t nsucc = node->nsucc;
        // Allocate memory for the list of successors nodes
        node->successors = (weighted_arrow*)malloc(nsucc*sizeof(weighted_arrow));
        if (fread(node->successors, sizeof(weighted_arrow), nsucc, file) != nsucc)
        {
            perror("Error reading sucessors\n");
            return 1;
        }
    }
    else
        node->successors = NULL;

    return 0;
}

/**
 * @public read_map_binary
 * Read from csv file that stores nodes and edges
 * create a graph structure and save to binary file 
*/
node* read_map_binary(char * filename, size_t* nnodes) {
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

    if (fread(nnodes, sizeof(unsigned long), 1, binmapfile) != 1) {
        perror("Error reading the number of nodes");
        fclose(binmapfile);
        return NULL;
    }

    printf("Num nodes: %lu\n",*nnodes);
    node *nodes = (node *)malloc((*nnodes) * sizeof(node));
    if (nodes == NULL) {
        perror("Error allocating memory for nodes");
        deallocate_and_close(nodes, *nnodes, binmapfile);
        return NULL;
    }

    // Start reading each node
    for (unsigned long i = 0; i < *nnodes; i++)
    {
        if(read_node(&nodes[i], binmapfile) == 1)
        {
            deallocate_and_close(nodes, *nnodes, binmapfile);
            return NULL;
        }
    }

    // update_parent_nodes(nodes, nnodes);

    // Call close file in the end
    fclose(binmapfile);

    // Record the end time
    end_time = clock();

    // Calculate the CPU time used
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    // Display the result
    printf("Read_map_binary - CPU Time Used: %f seconds\n", cpu_time_used);
    return nodes;
}

#endif
