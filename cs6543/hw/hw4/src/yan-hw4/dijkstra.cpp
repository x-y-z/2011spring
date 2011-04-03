/*
 * Author: Zi Yan
 * Date:04/01/2011
 */
#include "dijkstra.h"

/*
 * compute the shortest path from one vertex
 * @param source a source vertex
 *
 */
void dijkstra::computePaths(int source)
{
    adj_list::iterator vertex_iter;
    //give maximum int value to all minimal distance
    for (vertex_iter= adjacency_list.begin();
         vertex_iter != adjacency_list.end();
         vertex_iter++)
    {
        int v = vertex_iter->first;
        min_dist[v] = std::numeric_limits< int >::max();
    }

    //set source one to 0
    min_dist[source] = 0;
    //use set to automatically sort, first is min_dist,
    //second is vector number
    std::set< std::pair<int, int>,
              pair_cmp> vertex_queue;
    for (vertex_iter = adjacency_list.begin();
         vertex_iter != adjacency_list.end();
         vertex_iter++)
    {
        int v = vertex_iter->first;
        vertex_queue.insert(std::pair<int, int>(min_dist[v], v));
    }

    //while there are still vertices left
    while (!vertex_queue.empty()) 
    {
        int u = vertex_queue.begin()->second;
        vertex_queue.erase(vertex_queue.begin());
        
        std::list<edge>::iterator edge_iter;
        // visit each edge adjacent to u
        for (edge_iter = adjacency_list[u].begin();
             edge_iter != adjacency_list[u].end();
             edge_iter++)
        {
            int v = edge_iter->target;
            int weight = edge_iter->weight;
            int alter_dist = min_dist[u] + weight;
	    if (alter_dist< min_dist[v]) {
            //remove the old min_dist
	        vertex_queue.erase(std::pair<int, int>(min_dist[v], v));

	        min_dist[v] = alter_dist;
	        pre[v] = u;
            //add new min_dist
	        vertex_queue.insert(std::pair<int, int>(min_dist[v], v));

	    }

        }
    }
}

/*
 * get shortest path from pre[]
 * @param target the target vertex
 *
 * @return the shortest path
 */
std::list<int> dijkstra::getShortestPathTo(int target)
{
    std::list<int> path;
    std::map<int, int>::iterator prev;
    int vertex = target;
    path.push_front(vertex);
    while((prev = pre.find(vertex)) != pre.end())
    {
        vertex = prev->second;
        path.push_front(vertex);
    }
    return path;
}


/*
 * give the route table from calculate shortest path
 *
 */
void dijkstra::printRouteTable()
{
    adj_list::iterator vertex_iter;
	for (vertex_iter = adjacency_list.begin();
         vertex_iter != adjacency_list.end();
         vertex_iter++)
    {
        int v = vertex_iter->first;
        std::list<int> path =
            getShortestPathTo(v);
        std::list<int>::iterator path_iter = path.begin();
        if (path.size() > 1)
            path_iter++;
        std::cout << "Distance to " << vertex_names[v] << ": " <<
                     min_dist[v] << ", next hop is: "<< 
                     vertex_names[*path_iter] <<std::endl;
    }
}

/*
 * add a vertex into the graph
 *
 * @param aVertex vertex number, namely router number
 * @param aName name for the router, just the router number
 *
 */
void dijkstra::addVertex(int aVertex, std::string aName)
{
	vertex_names[aVertex].assign(aName);
}

/*
 * add a edge into the graphe
 *
 * @param start starting vertex
 * @param end ending vertex
 * @param weight edge weight
 *
 */
void dijkstra::addEdge(int start, int end, int weight)
{
	adjacency_list[start].push_back(edge(end, weight));
}
