#define MAXSUCC 23  // in an optimal implementation it must be removed

typedef struct {
    unsigned long id; // Node identification
    char* name; // in an optimal implementation it must change to a pointer
    double lat, lon;  // Node position
    unsigned short nsucc;  // Number of node successors; i. e. length of successors
    unsigned long* successors; // in an optimal implementation it must change to a pointer
} node;

/**
 * Search for a node using binary search
*/
unsigned long searchNode(unsigned long id, node *nodes, unsigned long nnodes);