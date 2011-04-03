#ifndef __NODE_H__
#define __NODE_H__

#include <vector>

using std::vector;

//neighbor information
//
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

typedef struct lsa_msg{
    int src;
    int nLen;
    int node;
} lsa_msg;

void *sendNeighbor(void *param);
void *recvLoop(void *param);
unsigned int JSHash(char* str, unsigned int len);


class node
{
private:
    vector<neighbor> myNeighbors;
    vector<adj_entry> myTopo;

    int myPort;

public:
    int addNeighbor(int nodeNum, int dist);
    int giveNeighbors(neighbor *&nTable, int &len);
    int addAdjEntry(int nodeNum, neighbor *adjList, int listLen);
    int printTopo();
    int computeRouteTable();

    int setPort(int aPort){ myPort = aPort; };
    int getPort(){ return myPort; };

    int giveForwardList(int msgSrc, int *&portList, int &len);

};

#endif
