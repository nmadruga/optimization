// readingmap1.c
// - counts the number of nodes in the map
// - allocates memory for the nodes and loads the information of each node (not the edges).
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct {
    unsigned long id; // Node identification
    char name[200];
    double lat, lon;  // Node position
    unsigned short nsucc;  // Number of node successors; i. e. length of successors
    unsigned long successors[9];
} node;

int main(int argc,char *argv[])
{
    clock_t start_time;
    FILE *mapfile;
    unsigned long nnodes;
    char *line=NULL;
    size_t len;

    start_time = clock();

    if(argc<2){
        mapfile = fopen("andorra.csv", "r");
        printf("Opening map andorra.csv.\n");
    }
    else{
        mapfile = fopen(argv[1], "r");
        printf("Opening map %s\n",argv[1]);
    }    if (mapfile == NULL)
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
    printf("Elapsed time: %f seconds\n", (float)(clock() - start_time) / CLOCKS_PER_SEC);

    rewind(mapfile);

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
            strcpy(nodes[index].name,field);
            for (int i = 0; i < 7; i++)
                field = strsep(&tmpline, "|");
            nodes[index].lat = atof(field);
            field = strsep(&tmpline, "|");
            nodes[index].lon = atof(field);

            nodes[index].nsucc = 0; // start with 0 successors

            index++;
        }
    }
    fclose(mapfile);
    printf("Assigned data to %ld\n", index);
    printf("Elapsed time: %f seconds\n", (float)(clock() - start_time) / CLOCKS_PER_SEC);
    printf("Last node has:\n id=%lu\n GPS=(%lf,%lf)\n Name=%s\n",nodes[index-1].id, nodes[index-1].lat, nodes[index-1].lon, nodes[index-1].name);
    return 0;
}
