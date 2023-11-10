#define MAXSUCC 23  // in an optimal implementation it must be removed

typedef struct
{
    unsigned vertexto;
    float weight;
} weighted_arrow;

typedef struct {
    unsigned long id; // Node identification
    char* name; // in an optimal implementation it must change to a pointer
    unsigned long name_len;
    double lat, lon;  // Node position
    unsigned short nsucc;  // Number of node successors; i. e. length of successors
    // unsigned long* old_successors in an optimal implementation it must change to a pointer
    /** TODO: Change to use pointers*/
    weighted_arrow* successors;
    // weighted_arrow arrow[5];
} node;

/**
 * Search for a node using binary search
*/
unsigned long searchNode(unsigned long id, node *nodes, unsigned long nnodes);