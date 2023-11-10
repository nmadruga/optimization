// readingmap0.c just counts the number of nodes in the map
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


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
    }

    if (mapfile == NULL)
    {
        printf("Error when opening the file");
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
    fclose(mapfile);
    return 0;
}
