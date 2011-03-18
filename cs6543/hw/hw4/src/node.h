#ifndef __NODE_H__
#define __NODE_H__

#include <vector>

typedef struct neighbor{
    int nodeNum;
    int dist;
} neighbor;

typedef struct adj_entry{
    int nodeNum;
    vector<neighbor> neighborList;
} adj_entry;

class node
{
private:
    vector<neighbor> myNeighbors;
    vector<adj_entry> myTopo;
};

#endif
