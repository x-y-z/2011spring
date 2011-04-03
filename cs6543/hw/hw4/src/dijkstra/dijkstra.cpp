#include "dijkstra.h"

void dijkstra::computePaths(vertex_t source)
{
    for (adjacency_map_t::iterator vertex_iter = adjacency_map.begin();
         vertex_iter != adjacency_map.end();
         vertex_iter++)
    {
        vertex_t v = vertex_iter->first;
        min_distance[v] = std::numeric_limits< int >::max();
    }

    min_distance[source] = 0;
    std::set< std::pair<weight_t, vertex_t>,
              pair_first_less<weight_t, vertex_t> > vertex_queue;
    for (adjacency_map_t::iterator vertex_iter = adjacency_map.begin();
         vertex_iter != adjacency_map.end();
         vertex_iter++)
    {
        vertex_t v = vertex_iter->first;
        vertex_queue.insert(std::pair<weight_t, vertex_t>(min_distance[v], v));
    }

    while (!vertex_queue.empty()) 
    {
        vertex_t u = vertex_queue.begin()->second;
        vertex_queue.erase(vertex_queue.begin());


        // Visit each edge exiting u
        for (std::list<edge>::iterator edge_iter = adjacency_map[u].begin();
             edge_iter != adjacency_map[u].end();
             edge_iter++)
        {
            vertex_t v = edge_iter->target;
            weight_t weight = edge_iter->weight;
            weight_t distance_through_u = min_distance[u] + weight;
	    if (distance_through_u < min_distance[v]) {
	        vertex_queue.erase(std::pair<weight_t, vertex_t>(min_distance[v], v));

	        min_distance[v] = distance_through_u;
	        previous[v] = u;
	        vertex_queue.insert(std::pair<weight_t, vertex_t>(min_distance[v], v));

	    }

        }
    }
}


std::list<vertex_t> dijkstra::getShortestPathTo(vertex_t target)
{
    std::list<vertex_t> path;
    std::map<vertex_t, vertex_t>::iterator prev;
    vertex_t vertex = target;
    path.push_front(vertex);
    while((prev = previous.find(vertex)) != previous.end())
    {
        vertex = prev->second;
        path.push_front(vertex);
    }
    return path;
}

void dijkstra::printShortestPath()
{
	for (adjacency_map_t::iterator vertex_iter = adjacency_map.begin();
         vertex_iter != adjacency_map.end();
         vertex_iter++)
    {
        vertex_t v = vertex_iter->first;
        std::cout << "Distance to " << vertex_names[v] << ": " << min_distance[v] << std::endl;
        std::list<vertex_t> path =
            getShortestPathTo(v);
        std::list<vertex_t>::iterator path_iter = path.begin();
        std::cout << "Path: ";
        for( ; path_iter != path.end(); path_iter++)
        {
            std::cout << vertex_names[*path_iter] << " ";
        }
        std::cout << std::endl;
    }
}

void dijkstra::printRouteTable()
{
	for (adjacency_map_t::iterator vertex_iter = adjacency_map.begin();
         vertex_iter != adjacency_map.end();
         vertex_iter++)
    {
        vertex_t v = vertex_iter->first;
        std::list<vertex_t> path =
            getShortestPathTo(v);
        std::list<vertex_t>::iterator path_iter = path.begin();
        if (path.size() > 1)
            path_iter++;
        std::cout << "Distance to " << vertex_names[v] << ": " <<
                     min_distance[v] << ", next hop is: "<< 
                     vertex_names[*path_iter] <<std::endl;
    }
}

void dijkstra::addVertex(vertex_t aVertice, std::string aName)
{
	vertex_names[aVertice].assign(aName);
}

void dijkstra::addEdge(int start, int end, int weight)
{
	adjacency_map[start].push_back(edge(end, weight));
}
