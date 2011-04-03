/*
 * Author: Zi Yan
 * Date: 03/20/2011
 *
 */
#include "node.h"
#include "udp.h"
#include "dijkstra.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <pthread.h>


//store the received msgs' hashes
std::vector<unsigned int> lsaDB;


/*
 * add a neighbor info into database
 * 
 * @param nodeNum router number
 * @param dist distance between remote node and current one
 *
 * @ret preserved
 */
int node::addNeighbor(int nodeNum, int dist)
{
    neighbor newNeighbor;

    newNeighbor.nodeNum = nodeNum;
    newNeighbor.dist = dist;

    myNeighbors.push_back(newNeighbor);

    return 0;
}

/*
 * output neighbors as a list
 * 
 * @param nTable output list
 * @param len list length
 *
 * @ret preserved
 */
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

/*
 * give a list of ports to be forward to
 *
 * @param msgSrc where the message comes from, excluded from the list
 * @param portList output list
 * @param len list length
 *
 * @ret preserved
 */
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

/*
 * add a adjacency entry into my network topology
 *
 * @param nodeNum node number in this entry
 * @param adjList a list of nodes which are adjacent to above node
 * @param listLen the lenght of the list
 *
 * @ret preserved
 */
int node::addAdjEntry(int nodeNum, neighbor *adjList, int listLen)
{
    adj_entry newEntry;
    //neighbor oneNeignbor;

    newEntry.nodeNum = nodeNum;
    
    for (int i = 0; i < listLen; i ++)
    {
        //oneNeignbor =adjList[i];
        newEntry.neighborList.push_back(adjList[i]);
    }

    myTopo.push_back(newEntry);

    return 0;
}

/*
 * use dijkstra algorithm to compute the route table
 *
 * @ret preserved
 *
 */
int node::computeRouteTable()
{
    vector<adj_entry>::iterator iter1;
    vector<neighbor>::iterator iter2;
    dijkstra aPath;


    for (iter1 = myTopo.begin(); iter1 != myTopo.end(); iter1++)
    {
        char name[50];

        sprintf(name, "Node:%d", (*iter1).nodeNum);
        aPath.addVertex((*iter1).nodeNum, name);
        for (iter2 = (*iter1).neighborList.begin(); 
             iter2 != (*iter1).neighborList.end();
             iter2++)
        {
            aPath.addEdge((*iter1).nodeNum, 
                          (*iter2).nodeNum, (*iter2).dist);
            aPath.addEdge((*iter2).nodeNum, 
                          (*iter1).nodeNum, (*iter2).dist);
        }

    }

    aPath.computePaths(getPort());
//    aPath.printShortestPath();
    aPath.printRouteTable();

}


/*
 * print out current network topology
 *
 * @ret preserved
 *
 */
int node::printTopo()
{
    vector<adj_entry>::iterator iter1;
    vector<neighbor>::iterator iter2;

    std::cout<<"\nNew topology is found:\n";

    for (iter1 = myTopo.begin(); iter1 != myTopo.end(); iter1++)
    {
        std::cout<<"Node ("<<(*iter1).nodeNum<<") has:";
        for (iter2 = (*iter1).neighborList.begin(); 
             iter2 != (*iter1).neighborList.end();
             iter2++)
        {
            std::cout<<(*iter2).nodeNum<<", ";
        }

        std::cout<<"\b \n";
    }

    return 0;
}

int main(int argc, char**argv)
{
    //handle arguments
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

    //get neighbor info
    do
    {
        std::cin>>nodeNum>>dist;
        if (nodeNum != -1)
            router.addNeighbor(nodeNum, dist);
    }while (nodeNum != -1);

    //insert neighbor info hash value into lsaDB
    neighbor *tmp;
    int tLen;
    char *ownHash;
    int oLen;
    unsigned int hashValue;

    router.giveNeighbors(tmp, tLen);
    oLen = 2 * sizeof(int) + tLen * sizeof(neighbor);
    ownHash = new char[oLen];
    (*(int *)ownHash) = tLen;
    *(((int *)ownHash) + 1) = router.getPort();
    memcpy(ownHash + 2 * sizeof(int), tmp, tLen * sizeof(neighbor));
    hashValue = JSHash(ownHash, oLen);
    lsaDB.push_back(hashValue);

    //add my own topo
    router.addAdjEntry(router.getPort(),
                       tmp,
                       tLen);
    delete []tmp;
    delete []ownHash;



    std::cout<<"Flooding!\n";

    
    pthread_t thread_id;
    //receiving & forwarding & print network & routing table
    //need a thread
    pthread_create(&thread_id, NULL, recvLoop, &router);

//#ifdef SEND
    //sending
    std::cout<<"Enter any key to start flooding\n";
    getchar();
    getchar();
    sendNeighbor(&router);
//#endif
    //with pthread_join to wait, otherwise router data will destroyed
    pthread_join(thread_id, NULL);
}

/*
 * sending neighbor info to others, at the beginning
 *
 * @param param the pointer to router data structure
 *
 */
void *sendNeighbor(void *param)
{
    node *aRouter = (node *)param;
    neighbor *sendTable = NULL;
    int tLen;
    char *msg;
    lsa_msg *aMsg;
    int mLen;
    
    //gather neighbor info, and send
    aRouter->giveNeighbors(sendTable, tLen);

    mLen = 3 * sizeof(int) + tLen * sizeof(neighbor);
    msg = new char[mLen];
    aMsg = (lsa_msg *)msg;
    aMsg->src = aRouter->getPort();
    aMsg->nLen = tLen;
    aMsg->node = aRouter->getPort();
    memcpy(msg + 3 * sizeof(int), sendTable, tLen * sizeof(neighbor));
    

    for (int i = 0; i < tLen; i++)
    {
        udp aUdp(-1);//client
        aUdp.init("localhost", sendTable[i].nodeNum);
        aUdp.send(msg, mLen);
        aUdp.close();
    }

    delete []sendTable;
    delete []msg;

    return NULL;
}

/*
 * get others' network info, and forward them selectively
 *
 * @para param the pointer to router data structure
 *
 */
void *recvLoop(void *param)
{
    node *aRouter = (node *)param;
    int *neighborList = NULL;
    int nLen;
    udp listener(1);//server mode
    char buf[500];
    int bLen;//received msg length
    lsa_msg *rMsg;//remote message
    int remotePort;
    bool ifForward = true;
    
    //listen
    listener.init(aRouter->getPort());

    while (1)
    {
        ifForward = true;
        bLen = listener.recv(buf, 500);
        rMsg = (lsa_msg *)buf;

        remotePort = rMsg->src;
        nLen = rMsg->nLen;
        if (nLen * sizeof(neighbor) + 3 * sizeof(int) != bLen)
        {
            std::cerr<<"Packet corrupted.\n";
            exit(1);
        }

        //hash needed, I don't want to compare the entries
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
            //add hashValue into lsaDB
            lsaDB.push_back(hashValue);
            //get neighbor list
            aRouter->giveForwardList(remotePort, neighborList, nLen);
            //send data to all neighbors except the soruce of the data
            rMsg->src = aRouter->getPort();
            for (int i = 0; i < nLen; i++)
            {
                udp aForward(-1);
                aForward.init("localhost", neighborList[i]);
                aForward.send(buf, bLen);
            }

            delete []neighborList;

            //add info to myTopo
            aRouter->addAdjEntry(rMsg->node, 
                                 (neighbor *)(buf + 3 * sizeof(int)),
                                 rMsg->nLen);
            aRouter->printTopo();
            //print new route table
            aRouter->computeRouteTable();

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
