#include <stdlib.h>

typedef char bool;
enum
{
    false,
    true
};

typedef struct
{
    float sum_weights;
    long unsigned parent;
} AStarPath;

typedef struct
{
    float cost;
    bool IsOpen;
} AStarControlData;

typedef struct QueueElementstruct
{
    unsigned vertex;
    struct QueueElementstruct *next;
} QueueElement;

typedef QueueElement *PriorityQueue;