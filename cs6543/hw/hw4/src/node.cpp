#include "node.h"
#include "udp.h"
#include <iostream>
#include <vector>
#include <cstdlib>


std::vector<int> lsaDB;


int node::addNeighbor(int nodeNum, int dist)
{
    neighbor newNeighbor;

    newNeighbor.nodeNum = nodeNum;
    newNeighbor.dist = dist;

    myNeighbors.push_back(newNeighbor);

    return 0;
}

int node::giveNeighbors(neighbor *&nTable, int &len)
{
    neighbor aNeighbor;
    vector<neighbor>::iterator iter;
    int index = 0;

    len = myNeighbors.size();
    nTable = new neighbor[len];

    for (iter = myNeighbor.begin(); iter != myNeighbor.end(); iter++)
    {
        nTable[index++] = *iter;
    }

    return 0;
}

int node::addAdjEntry(adj_entry aEntry)
{
    myTopo.push_back(aEntry);   
}

int node::computeRouteTable(route_entry *&routeTable)
{

}

int main(int argc, char**argv)
{
    if (argc != 2)
    {
        std::cerr<<"Usage:"<<argv[0]<<"<port num>. \n";
        exit(1);
    }

    node router;
    int nodeNum;
    int dist;

    router.setPort(atoi(argv[1]));
    
    std::cout<<"Enter neighbors:\n";

    do
    {
        std::cin>>nodeNum>>dist;
        router.addNeighbor(nodeNum, dist);
    }while (nodeNum != -1);

    std::cout<<"Flooding!\n";



    //receiving & forwarding & print network & routing table
    //need a thread

    //sending
    
    //with pthread_join to wait, otherwise router data will destroyed
}

void *sendNeighbor(void *param)
{
    node *aRouter = (node *)param;
    neighbor *sendTable;
    int tLen;
    char *msg;
    int mLen;
    
    aRouter.giveNeighbors(sendTable, tLen);

    mLen = sizeof(int) + tLen * sizeof(neighbor);
    msg = new char[mLen];
    (*(int *)msg) = tLen;
    memcpy(msg + sizeof(int), sendTable, tLen * sizeof(neighbor));

    for (int i = 0; i < tLen; i++)
    {
        udp aUdp(-1);//client
        aUdp.init("localhost", sendTable[i].nodeNum);
        aUdp.send(msg, mLen);
        aUdp.close();
    }

    return;
}

void *recvLoop(void *param)
{
    node *aRouter = (node *)param;
    neighbor *neighborList;
    int nLen;
    udp listener(1);//server mode
    char buf[500];
    int tLen;//neighbor list length
    int remotePort;
    bool ifForward = true;
    
    listener.inited(aRouter.getPort());

    while (1)
    {
        listener.recv(buf, 500);
        listener.senderInfo(NULL, remotePort);

        std::vector<int>::iterator iter;
        for (iter = lsaDB.begin(); iter != lsaDB.end(); iter++)
        {
            if (*iter == remotePort)//received before
            {
                ifForward = false;
                break;
            }
        }

        if (ifForward == true)//not got this before
        {
            //get neighbor list
            //send data to all neighbors except the soruce of the data


            //add info to myTopo


            //print new route table
        }

        

    }

}
