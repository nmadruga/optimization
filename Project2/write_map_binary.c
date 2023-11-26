#ifndef WRITE_MAP_BINARY_H 
#define WRITE_MAP_BINARY_H 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "node.h"
#include "file_util.h"
#include "write_map_binary.h"

#define RADIUS_EARTH 6371.0  // Earth radius in kilometers

/**
 * @private function to_radians
 * Convert distance from degrees to radians
 * @param degree distance between 2 points in degrees
 * @return value in meters corresponding to degrees
*/
double to_radians(double degree) {
    return degree * M_PI / 180.0;
}

/**
 * @private function calculate_weight
 * Calculate distance between 2 vertex in 
 * the graph (that represents a map). Uses 
 * latitute and longitude to calculate value.
 * @param source    Node with starting point
 * @param destine   Node with ending point
 * @return  vaue of distance in meters
*/
double calculate_weight(node* source, node* destine)
{
    double dlat = to_radians(destine->lat - source->lat);
    double dlon = to_radians(destine->lon - source->lon);

    double a = sin(dlat / 2.0) * sin(dlat / 2.0) +
               cos(to_radians(source->lat)) * cos(to_radians(destine->lat)) *
               sin(dlon / 2.0) * sin(dlon / 2.0);

    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

    return RADIUS_EARTH * c;
}

/**
 * @private add_to_successors
 * Add a new successor to the vector reallocating the structure
 * @param node  Current node in graph to add new successor
 * @param vertexto  Vertex index of the sucessor 
 * @param weight    The weight pre calculated between node and successor
*/
void add_to_successors(node* node, unsigned int vertexto, double weight)
{
    unsigned long nsucc = node->nsucc;
    weighted_arrow* aux = node->successors;
    node->successors = (weighted_arrow*) malloc((nsucc+1)*sizeof(weighted_arrow));

    // Copy previous elements from old vector
    for (size_t i = 0; i < nsucc; i++)
        node->successors[i] = aux[i];

    // Deallocate previous vector
    free(aux);
    
    node->successors[nsucc].vertexto=vertexto;
    node->successors[nsucc].weight=weight;
    node->nsucc++;
}

/**
 * @private function write_node 
 * Write one node to file
 * @param graph_node    Pointer with values to be written
 * @param file          The file where to write
*/
void write_node(node* graph_node, FILE* file)
{
     fwrite(graph_node, sizeof(node), 1, file);
     fwrite(graph_node->name, sizeof(char), graph_node->name_len, file);
     fwrite(graph_node->successors, sizeof(weighted_arrow), graph_node->nsucc, file);
}

/**
 * @public function write_map_binary
 * Function to read csv with nodes and edges
 * construct a graph and save to binary file
*/
int write_map_binary(char* filename)
{
    clock_t start_time, end_time;
    double cpu_time_used;
    FILE *mapfile;
    unsigned long nnodes;
    char *line=NULL;
    size_t len;

    start_time = clock();

    printf("Reading csv file: %s\n", filename);
    mapfile = fopen(filename, "r");
    if (mapfile == NULL)
    {
        printf("Error when opening the file\n");
        return 1;
    }

    nnodes=0UL;
    while (getline(&line, &len, mapfile) != -1)
    {
        if (strncmp(line, "node", 4) == 0)
        {
            nnodes++;
        }
    }
    printf("Total number of nodes is %ld\n", nnodes);

    rewind(mapfile);
    
    start_time = clock();

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
            nodes[index].name_len = 0;
            if(strlen(field) > 0)
            {
                nodes[index].name_len = strlen(field);
                nodes[index].name = (char*) malloc((nodes[index].name_len+1)*sizeof(char));
                strcpy(nodes[index].name,field);
            }

            for (int i = 0; i < 7; i++)
                field = strsep(&tmpline, "|");
            nodes[index].lat = atof(field);
            field = strsep(&tmpline, "|");
            nodes[index].lon = atof(field);

            nodes[index].nsucc = 0; // start with 0 successors
            nodes[index].successors = (weighted_arrow*) malloc(MAXSUCC*sizeof(weighted_arrow));

            index++;
        }
    }
    printf("Assigned data to %ld nodes\n", index);
    printf("Elapsed time: %f seconds\n", (float)(clock() - start_time) / CLOCKS_PER_SEC);

    rewind(mapfile);
    
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
            origin = search_node(originId,nodes,nnodes);
            while(1)
            {
                field = strsep(&tmpline, "|");
                if (field == NULL) break;
                destId = strtoul(field, &ptr, 10);
                dest = search_node(destId,nodes,nnodes);
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
                    double weight = calculate_weight(&nodes[origin],&nodes[dest]);
                    add_to_successors(&nodes[origin], dest, weight);
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
                    if(newor)
                    {
                        double weight = calculate_weight(&nodes[dest],&nodes[origin]);
                        add_to_successors(&nodes[dest], origin, weight);
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

    strcpy(binmapname,filename);
    rename_extension(binmapname,".bin");

    printf("Writing result in binary file: %s\n", binmapname);
    binmapfile = fopen(binmapname,"wb");
    fwrite(&nnodes,sizeof(unsigned long),1,binmapfile);
    for (size_t i = 0; i < nnodes; i++)
    {
        write_node(&nodes[i],binmapfile);
        if (i < 15)
            printf("Writing node: %ld\nName: %s name_len: %d\n nsucc: %d\nlat: %f lon: %f\n",i,nodes[i].name, nodes[i].name_len,  nodes[i].nsucc, nodes[i].lat, nodes[i].lon);
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

    // Record the end time
    end_time = clock();

    // Calculate the CPU time used
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    // Display the result
    printf("Write_map_binary - CPU Time Used: %f seconds\n", cpu_time_used);

    return 0;
}

#endif
