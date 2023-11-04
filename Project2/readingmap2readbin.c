#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "node.h"

int main(int argc,char *argv[]) {
    clock_t start_time;
    unsigned long nnodes;

    start_time = clock();

    char binmapname[80];
    strcpy(binmapname,"andorra.csv.bin");

    if(argc>1) strcpy(binmapname,argv[1]);

    FILE *binmapfile;
    start_time = clock();

    binmapfile = fopen(binmapname,"rb");
    if (binmapfile == NULL) {
        perror("Error opening the binary binmapfile");
        return 1;
    }

    unsigned long numNodes;
    if (fread(&numNodes, sizeof(unsigned long), 1, binmapfile) != 1) {
        perror("Error reading the number of nodes");
        fclose(binmapfile);
        return 1;
    }

    printf("Num nodes: %lu\n",numNodes);

    node *nodes = (node *)malloc(numNodes * sizeof(node));
    if (nodes == NULL) {
        perror("Error allocating memory for nodes");
        fclose(binmapfile);
        return 1;
    }

    for (unsigned long i = 0; i < numNodes; i++) {
        
        // First read the node information
        if (fread(&nodes[i], sizeof(node), 1, binmapfile) != 1) {
             // When there is a problem - deallocate and exit
            perror("Error reading the node");
            free(nodes);
            fclose(binmapfile);
            return 1;
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
                return 1;
            }
        }
        else
            nodes[i].name = NULL;
            
    }

    // Properly allocate successor's pointers :)

    // Now, you have the map with all nodes on the graph

    // Don't forget to free the memory when you're done
    for (unsigned long i = 0; i < numNodes; i++) {
        free(nodes[i].name);
    }
    free(nodes);

    fclose(binmapfile);
    return 0;
}