typedef struct node node;

/**
 * @struct Weighted_arrow
 * Represents each sucessor
*/
typedef struct {
    unsigned vertexto;
    node* node_succ;
    float weight;
} weighted_arrow;

/**
 * @struct Node
*/
struct node {
    unsigned long id; // Node identification
    char* name; // in an optimal implementation it must change to a pointer
    unsigned long name_len;
    double lat, lon;  // Node position
    unsigned short nsucc;  // Number of node successors; i. e. length of successors
    weighted_arrow* successors;
};

/**
 * @public search_node
 * Search for a node using binary search
 * @param id Number of identificator of node searched
 * @param nodes Pointer to graph with all nodes
 * @param nnodes Number of nodes in graph nodes
 * @return index of searched node on the graph nodes
*/
unsigned long search_node(unsigned long id, node *nodes, unsigned long nnodes);
