#ifndef __NODE_H__
#define __NODE_H__

#include <vector>

using std::vector;

typedef struct neighbor{
    int nodeNum;
    int dist;
} neighbor;

typedef struct adj_entry{
    int nodeNum;
    vector<neighbor> neighborList;
} adj_entry;

typedef struct route_entry{
    int dest;
    int nextHop;
    int dist;
} route_entry;

class node
{
private:
    vector<neighbor> myNeighbors;
    vector<adj_entry> myTopo;

    int myPort;

public:
    int addNeighbor(int nodeNum, int dist);
    int giveNeighbors(neighbor *&nTable, int &len);
    int addAdjEntry(adj_entry aEntry);
    int computeRouteTable(route_entry *&routeTable);

    int setPort(int aPort){ myPort = aPort; };
    int getPort(){ return myPort; };
};

#endif
