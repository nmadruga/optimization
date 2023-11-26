#ifndef READ_MAP_BINARY_H 
#define READ_MAP_BINARY_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "node.h"
#include "read_map_binary.h"

// void update_parent_nodes(node* graph, size_t GraphOrder)
// {
//     for(size_t i = 0; i < GraphOrder; i++)
//     {
//         if (graph[i].nsucc > 0)
//         {
//             for(int j = 0; j < graph[i].nsucc; j++)
//             {
//                 unsigned int successor_index = graph[i].successors[j].vertexto;
//                 graph[i].successors[j].node_succ = &graph[successor_index];
//             }
//         }
//     }
// }

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

/**
 * @public read_map_binary
 * Read from csv file that stores nodes and edges
 * create a graph structure and save to binary file 
*/
node* read_map_binary(char * filename, size_t* size) {
    clock_t start_time, end_time;
    double cpu_time_used;
    unsigned long nnodes = (*size);
    FILE *binmapfile;

    start_time = clock();

    printf("Reading binary file: %s\n", filename);
    binmapfile = fopen(filename,"rb");
    if (binmapfile == NULL) {
        perror("Error opening the binary binmapfile");
        return NULL;
    }

    if (fread(size, sizeof(size_t), 1, binmapfile) != 1) {
        perror("Error reading the number of nodes");
        fclose(binmapfile);
        return NULL;
    }

    printf("Num nodes: %lu\n",*size);
    node *nodes = (node *)malloc(nnodes * sizeof(node));
    if (nodes == NULL) {
        perror("Error allocating memory for nodes");
        deallocate_and_close(nodes, nnodes, binmapfile);
        return NULL;
    }

    // Start reading each node
    for (unsigned long i = 0; i < nnodes; i++) {
        
        // First read the node information
        if (fread(&nodes[i], sizeof(node), 1, binmapfile) != 1) {
             // When there is a problem - deallocate and exit
            perror("Error reading the node");
            deallocate_and_close(nodes, nnodes, binmapfile);
            return NULL;
        }

        if(nodes[i].name_len > 0) {
            size_t name_len = nodes[i].name_len;
            // Allocate memory for the name and read it
            nodes[i].name = (char *)malloc((name_len+1)*sizeof(char));
            // When there is a problem - deallocate and exit
            if (fread(nodes[i].name, sizeof(char), name_len, binmapfile) != name_len) {
                perror("Error reading the name");
                deallocate_and_close(nodes, nnodes, binmapfile);
                return NULL;
            }
        }
        else
            nodes[i].name = NULL;

        if(nodes[i].nsucc > 0) {
            size_t success_len = nodes[i].nsucc;
            // Allocate memory for the list of successors nodes
            nodes[i].successors = (weighted_arrow*)malloc(success_len*sizeof(weighted_arrow));
            if (fread(nodes[i].successors, sizeof(weighted_arrow), success_len, binmapfile) != success_len) {
                perror("Error reading the successors");        
                deallocate_and_close(nodes, nnodes, binmapfile);
                return NULL;
            }
        }
        else
            nodes[i].successors = NULL;
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