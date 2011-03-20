#include "node.h"
#include "udp.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <pthread.h>


std::vector<unsigned int> lsaDB;


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
    vector<neighbor>::iterator iter;
    int index = 0;

    len = myNeighbors.size();
    nTable = new neighbor[len];

    for (iter = myNeighbors.begin(); iter != myNeighbors.end(); iter++)
    {
        nTable[index++] = *iter;
    }

    return 0;
}

int node::giveForwardList(int msgSrc, int *&portList, int &len)
{
    vector<neighbor>::iterator iter;
    int index = 0;

    len = myNeighbors.size();
    portList = new int[len];

    for (iter = myNeighbors.begin(); iter != myNeighbors.end(); iter++)
    {
        if ((*iter).nodeNum != msgSrc)
            portList[index++] = (*iter).nodeNum;
    }

    len = index;

    return 0;
}

int node::addAdjEntry(int nodeNum, neighbor *adjList, int listLen)
{
    adj_entry newEntry;
    neighbor oneNeignbor;

    newEntry.nodeNum = nodeNum;
    
    for (int i = 0; i < listLen; i ++)
    {
        oneNeignbor =adjList[i];
    }

    return 0;
}

int node::computeRouteTable(route_entry *&routeTable)
{

}


int node::printTopo()
{
}

int main(int argc, char**argv)
{
    if (argc != 2)
    {
        std::cerr<<"Usage:"<<argv[0]<<" <port num>. \n";
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
        if (nodeNum != -1)
            router.addNeighbor(nodeNum, dist);
    }while (nodeNum != -1);

    std::cout<<"Flooding!\n";

    
    pthread_t thread_id;
    //receiving & forwarding & print network & routing table
    //need a thread
    //pthread_create(&thread_id, NULL, recvLoop, &router);
#ifdef RECV
    recvLoop(&router);
#endif

#ifdef SEND
    //sending
    sendNeighbor(&router);
#endif
    //with pthread_join to wait, otherwise router data will destroyed

    //pthread_join(thread_id, NULL);
}

void *sendNeighbor(void *param)
{
    node *aRouter = (node *)param;
    neighbor *sendTable = NULL;
    int tLen;
    char *msg;
    int mLen;
    
    aRouter->giveNeighbors(sendTable, tLen);

    mLen = 2 * sizeof(int) + tLen * sizeof(neighbor);
    msg = new char[mLen];
    (*(int *)msg) = aRouter->getPort();
    *(((int *)msg) + 1) = tLen;
    memcpy(msg + 2 * sizeof(int), sendTable, tLen * sizeof(neighbor));

    for (int i = 0; i < tLen; i++)
    {
        udp aUdp(-1);//client
        aUdp.init("localhost", sendTable[i].nodeNum);
        aUdp.send(msg, mLen);
        aUdp.close();
    }

    delete sendTable[];

    return NULL;
}

void *recvLoop(void *param)
{
    node *aRouter = (node *)param;
    int *neighborList = NULL;
    int nLen;
    udp listener(1);//server mode
    char buf[500];
    int bLen;//received msg length
    int remotePort;
    bool ifForward = true;
    
    listener.init(aRouter->getPort());

    while (1)
    {
        bLen = listener.recv(buf, 500);
        //listener.senderInfo(NULL, remotePort);
        remotePort = (*(int *)buf);
        nLen = *(((int *)buf) + 1);
        if (nLen * sizeof(neighbor) + 2 * sizeof(int) != bLen)
        {
            std::cerr<<"Packet corrupted.\n";
            exit(1);
        }

        //hash needed
        std::vector<unsigned int>::iterator iter;
        unsigned int hashValue = JSHash(buf + sizeof(int), 
                                        bLen - sizeof(int));
        for (iter = lsaDB.begin(); iter != lsaDB.end(); iter++)
        {
            if (*iter == hashValue)//received before
            {
                ifForward = false;
                break;
            }
        }

        if (ifForward == true)//not got this before
        {
            //get neighbor list
            aRouter->giveForwardList(remotePort, neighborList, nLen);
            //send data to all neighbors except the soruce of the data
            (*(int *)buf) = aRouter->getPort();
            for (int i = 0; i < nLen; i++)
            {
                udp aForward(-1);
                aForward.init("localhost", neighborList[i]);
                aForward.send(buf, bLen);
            }

            delete neighborList[];

            //add info to myTopo
            aRouter->addAdjEntry(remotePort, 
                                 (neighbor *)(buf + 2 * sizeof(int)),
                                 nLen);
            aRouter->printTopo();
            //print new route table
            //aRouter->computeRouteTable(NULL);
        }

        

    }

    return NULL;

}

/*
 * Get this hash function from:
 * http://www.partow.net/programming/hashfunctions *
 *
 */
unsigned int JSHash(char* str, unsigned int len)
{
    unsigned int hash = 1315423911;
    unsigned int i    = 0;

    for(i = 0; i < len; str++, i++)
    {
        hash ^= ((hash << 5) + (*str) + (hash >> 2));
    }

    return hash;
}
