#include "dijkstra.h"

int main()
{
    dijkstra aExample;

/*    aExample.addVertex("Harrisbur");
    aExample.addVertex("Baltimore");
    aExample.addVertex("Washington");
    aExample.addVertex("Philadephia");
    aExample.addVertex("Binghamton");
    aExample.addVertex("Allentown");
    aExample.addVertex("New York");

    aExample.addEdge(0, 1, 79);
    aExample.addEdge(0, 5, 81);
    aExample.addEdge(1, 0, 78);
    aExample.addEdge(1, 2, 39);
    aExample.addEdge(1, 3, 103);
    aExample.addEdge(2, 1, 38);
    aExample.addEdge(3, 1, 102);
    aExample.addEdge(3, 5, 61);
    aExample.addEdge(3, 6, 96);
    aExample.addEdge(4, 5, 133);
    aExample.addEdge(5, 0, 82);
    aExample.addEdge(5, 3, 62);
    aExample.addEdge(5, 4, 134);
    aExample.addEdge(5, 6, 91);
    aExample.addEdge(6, 3, 97);
    aExample.addEdge(6, 5, 87);*/

    aExample.addVertex(1111, "node 1111");
    aExample.addEdge(1111,2222, 4);
    aExample.addEdge(2222,1111, 4);
    aExample.addVertex(2222, "node 2222");
    aExample.addEdge(2222,1111, 3);
    aExample.addEdge(1111,2222, 3);

    aExample.computePaths(1111);
    aExample.printShortestPath();
}
