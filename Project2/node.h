typedef struct node node;

/**
 * @struct Weighted_arrow
 * Represents each sucessor
*/
typedef struct {
    unsigned vertexto;
    float weight;
} weighted_arrow;

/**
 * @struct Node
*/
struct node {
    unsigned long id; // Node identification
    double lat, lon;  // Node position
    int name_len;
    char* name; // Name of node
    int nsucc;  // Number of node successors; i. e. length of successors
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
