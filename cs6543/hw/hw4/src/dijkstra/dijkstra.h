#ifndef __DIJSTRA_H__
#define __DIJSTRA_H__

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <list>

#include <set>

typedef int vertex_t;
typedef int weight_t;

struct edge {
    vertex_t target;
    weight_t weight;
    edge(vertex_t arg_target, weight_t arg_weight)
        : target(arg_target), weight(arg_weight) { }
};

typedef std::map<vertex_t, std::list<edge> > adjacency_map_t;


template <typename T1, typename T2>
struct pair_first_less
{
    bool operator()(std::pair<T1,T2> p1, std::pair<T1,T2> p2) const 
    {
       if(p1.first == p2.first) {
          //Otherwise the initial vertex_queue will have the size 2 { 0,source ; inf;n }
          return p1.second < p2.second;
       }
       return p1.first < p2.first;
    }
};


class dijkstra
{
private:
    adjacency_map_t adjacency_map;
    std::map<vertex_t, std::string> vertex_names;
    std::map<vertex_t, weight_t> min_distance;
    std::map<vertex_t, vertex_t> previous;
public:
	void addVertex(vertex_t vertice, std::string aName);
	void addEdge(int start, int end, int weight);
	
public:
	void computePaths(vertex_t source);
	std::list<vertex_t> getShortestPathTo(vertex_t target);
	void printShortestPath();
};


#endif
