#ifndef WRITE_MAP_BINARY_H 
#define WRITE_MAP_BINARY_H 

// readingmap2.c
// - counts the number of nodes in the map
// - allocates memory for the nodes and loads the information of each node.
// - loads the edges and shows the information of a particular node.
// - writes the information in a binary file
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "node.h"
#include "write_map_binary.h"

// Function to write a single node to a binary file
void write_node(node* n, FILE* file) {
    fwrite(n, sizeof(node), 1, file);
    fwrite(n->name, sizeof(char), n->name_len, file);
    fwrite(n->successors, sizeof(weighted_arrow), n->nsucc, file);
}

int write_map_binary(char* file_name)
{
    clock_t start_time;
    FILE *mapfile;
    unsigned long nnodes;
    char *line=NULL;
    size_t len;

    start_time = clock();

    mapfile = fopen(file_name, "r");
    if (mapfile == NULL)
    {
        printf("Error when opening the file\n");
        return 1;
    }
    // count the nodes
    nnodes=0UL;
    while (getline(&line, &len, mapfile) != -1)
    {
        if (strncmp(line, "node", 4) == 0)
        {
            nnodes++;
        }
    }
    printf("Total number of nodes is %ld\n", nnodes);
    // printf("Elapsed time: %f seconds\n", (float)(clock() - start_time) / CLOCKS_PER_SEC);

    rewind(mapfile);
    
    // start_time = clock();
    node *nodes;
    char *tmpline , *field , *ptr;
    unsigned long index=0;

    nodes = (node*) malloc(nnodes*sizeof(node));
    if(nodes==NULL){
        printf("Error when allocating the memory for the nodes\n");
        return 2;
    }

    while (getline(&line, &len, mapfile) != -1)
    {
        if (strncmp(line, "#", 1) == 0) continue;
        tmpline = line; // make a copy of line to tmpline to keep the pointer of line
        field = strsep(&tmpline, "|");
        if (strcmp(field, "node") == 0)
        {
            field = strsep(&tmpline, "|");
            nodes[index].id = strtoul(field, &ptr, 10);
            field = strsep(&tmpline, "|");
            nodes[index].name_len = strlen(field);
            if(field != "") nodes[index].name = (char*) malloc((nodes[index].name_len+1)*sizeof(char));
            strcpy(nodes[index].name,field);

            for (int i = 0; i < 7; i++)
                field = strsep(&tmpline, "|");
            nodes[index].lat = atof(field);
            field = strsep(&tmpline, "|");
            nodes[index].lon = atof(field);

            nodes[index].nsucc = 0; // start with 0 successors

            /** TODO: Use nodes[index].successors = realloc()*/
            nodes[index].successors = (weighted_arrow*) malloc(MAXSUCC*sizeof(weighted_arrow));

            index++;
        }
    }
    printf("Assigned data to %ld nodes\n", index);
    printf("Elapsed time: %f seconds\n", (float)(clock() - start_time) / CLOCKS_PER_SEC);
    printf("Last node has:\n id=%lu\n GPS=(%lf,%lf)\n Name=%s\n",nodes[index-1].id, nodes[index-1].lat, nodes[index-1].lon, nodes[index-1].name);
    
    rewind(mapfile);
    
    // start_time = clock();
    int oneway;
    unsigned long nedges = 0, origin, dest, originId, destId;
    while (getline(&line, &len, mapfile) != -1)
    {
        if (strncmp(line, "#", 1) == 0) continue;
        tmpline = line; // make a copy of line to tmpline to keep the pointer of line
        field = strsep(&tmpline, "|");
        if (strcmp(field, "way") == 0)
        {
            for (int i = 0; i < 7; i++) field = strsep(&tmpline, "|"); // skip 7 fields
            if (strcmp(field, "") == 0) oneway = 0; // no oneway
            else if (strcmp(field, "oneway") == 0) oneway = 1;
            else continue; // No correct information
            field = strsep(&tmpline, "|"); // skip 1 field
            field = strsep(&tmpline, "|");
            if (field == NULL) continue;
            originId = strtoul(field, &ptr, 10);
            origin = searchNode(originId,nodes,nnodes);
            while(1)
            {
                field = strsep(&tmpline, "|");
                if (field == NULL) break;
                destId = strtoul(field, &ptr, 10);
                dest = searchNode(destId,nodes,nnodes);
                if((origin == nnodes+1)||(dest == nnodes+1))
                {
                    originId = destId;
                    origin = dest;
                    continue;
                }
                if(origin==dest) continue;
                // Check if the edge did appear in a previous way
                int newdest = 1;
                for(int i=0;i<nodes[origin].nsucc;i++)
                    if(nodes[origin].successors[i].vertexto==dest){
                        newdest = 0;
                        break;
                    }
                if(newdest){

                    // if(nodes[origin].nsucc>=MAXSUCC){
                    //     printf("Maximum number of successors (%d) reached in node %lu.\n",MAXSUCC,nodes[origin].id);
                    //     return 5;
                    // }
                    nodes[origin].successors[nodes[origin].nsucc].vertexto=dest;
                    /** TODO: Find the correct weight ? where is it? Distance? */
                    nodes[origin].successors[nodes[origin].nsucc].weight = 1;
                    nodes[origin].nsucc++;
                    nedges++;
                }
                if(!oneway)
                {   
                    // Check if the edge did appear in a previous way
                    int newor = 1;
                    for(int i=0;i<nodes[dest].nsucc;i++)
                        if(nodes[dest].successors[i].vertexto==origin){
                            newor = 0;
                            break;
                        }
                    if(newor){
                        if(nodes[dest].nsucc>=MAXSUCC){
                            printf("Maximum number of successors (%d) reached in node %lu.\n",MAXSUCC,nodes[dest].id);
                            return 5;
                        }
                        nodes[dest].successors[nodes[dest].nsucc].vertexto=origin;
                        nodes[dest].nsucc++;
                        nedges++;
                    }
                }
                originId = destId;
                origin = dest;
            }
        }
    }
    
    fclose(mapfile);
    printf("Assigned %ld edges\n", nedges);
    printf("Elapsed time: %f seconds\n", (float)(clock() - start_time) / CLOCKS_PER_SEC);

    FILE *binmapfile;
    char binmapname[80];

    strcpy(binmapname,file_name);
    strcat(binmapname,".bin");

    binmapfile = fopen(binmapname,"wb");
    fwrite(&nnodes,sizeof(unsigned long),1,binmapfile);
    for (size_t i = 0; i < nnodes; i++)
    {
        write_node(&nodes[i],binmapfile);
    }

    fclose(binmapfile);

    for (size_t i = 0; i < nnodes; i++)
    {
        if (nodes[i].name) {
            free(nodes[i].name);
        }
        if (nodes[i].successors) {
            free(nodes[i].successors);
        }
    }
    free(nodes);

    return 0;
}

#endif