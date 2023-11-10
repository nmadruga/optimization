#ifndef READ_MAP_BINARY_H 
#define READ_MAP_BINARY_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "node.h"
#include "read_map_binary.h"

node* read_map_binary(char * file_name) {
    clock_t start_time;
    unsigned long nnodes;

    start_time = clock();

    FILE *binmapfile;
    start_time = clock();

    binmapfile = fopen(file_name,"rb");
    if (binmapfile == NULL) {
        perror("Error opening the binary binmapfile");
        /** TODO: ALSO SHOULD DEALLOCATE */
        return NULL;
    }

    unsigned long numNodes;
    if (fread(&numNodes, sizeof(unsigned long), 1, binmapfile) != 1) {
        perror("Error reading the number of nodes");
        fclose(binmapfile);
        /** TODO: ALSO SHOULD DEALLOCATE */
        return NULL;
    }

    printf("Num nodes: %lu\n",numNodes);

    node *nodes = (node *)malloc(numNodes * sizeof(node));
    if (nodes == NULL) {
        perror("Error allocating memory for nodes");
        fclose(binmapfile);
        /** TODO: ALSO SHOULD DEALLOCATE */
        return NULL;
    }

    for (unsigned long i = 0; i < numNodes; i++) {
        
        // First read the node information
        if (fread(&nodes[i], sizeof(node), 1, binmapfile) != 1) {
             // When there is a problem - deallocate and exit
            perror("Error reading the node");
            free(nodes);
            fclose(binmapfile);
            /** TODO: ALSO SHOULD DEALLOCATE */
            return NULL;
        }

        if(nodes[i].name_len > 0) {
            size_t name_len = nodes[i].name_len;
            // Allocate memory for the name and read it
            nodes[i].name = (char *)malloc((name_len+1)*sizeof(char));
            // When there is a problem - deallocate and exit
            if (fread(nodes[i].name, sizeof(char), name_len, binmapfile) != name_len) {
                perror("Error reading the name");
                free(nodes[i].name);
                free(nodes);
                fclose(binmapfile);
                /** TODO: ALSO SHOULD DEALLOCATE */
                return NULL;
            }
        }
        else
            nodes[i].name = NULL;
            
    }

    // Properly allocate successor's pointers :)

    // Now, you have the map with all nodes on the graph

    /** TODO: DEALLOCATION FUNCTION */
    // // Don't forget to free the memory when you're done
    // for (unsigned long i = 0; i < numNodes; i++) {
    //     free(nodes[i].name);
    // }
    // free(nodes);

    fclose(binmapfile);
    return nodes;
}

#endif