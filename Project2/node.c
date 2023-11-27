#include <stdio.h>
#include <stdlib.h>
#include "node.h"

/**
 * @public search_node
 * Receives the node id and look for its index
 * in the graph to be returned
 * @param id    Id for the node searched
 * @param nodes Graph with all the nodes
 * @param nnodes    Size od the graph
 * @return index of the node searched
*/
unsigned long search_node(unsigned long id, node *nodes, unsigned long nnodes)
{
    // we know that the nodes where numrically ordered by id, so we can do a binary search.
    unsigned long l = 0, r = nnodes - 1, m;
    while (l <= r)
    {
        m = l + (r - l) / 2;
        if (nodes[m].id == id) return m;
        if (nodes[m].id < id)
            l = m + 1;
        else
            r = m - 1;
    }

    // id not found, we return nnodes+1
    return nnodes+1;
}
