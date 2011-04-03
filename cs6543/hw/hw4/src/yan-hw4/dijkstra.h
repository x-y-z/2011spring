#ifndef __DIJSTRA_H__
#define __DIJSTRA_H__

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <list>

#include <set>


//edge structure and initializing function
struct edge {
    int target;
    int weight;
    edge(int aTarget, int aWeight)
        : target(aTarget), weight(aWeight) { }
};

//adjacency list represents the graph
typedef std::map<int, std::list<edge> > adj_list;


//pair comparing function for the set
struct pair_cmp
{
    bool operator()(std::pair<int, int> p1, std::pair<int, int> p2) const 
    {
        //if first parts are equal
        if(p1.first == p2.first) {
            return p1.second < p2.second;
        }
        return p1.first < p2.first;
    }
};

//a dijkstra algorithm implementation
class dijkstra
{
private:
    adj_list adjacency_list;
    std::map<int, std::string> vertex_names;
    std::map<int, int> min_dist;
    std::map<int, int> pre;
public:
	void addVertex(int vertice, std::string aName);
	void addEdge(int start, int end, int weight);
	
public:
	void computePaths(int source);
	std::list<int> getShortestPathTo(int target);
	void printShortestPath();
    void printRouteTable();
};


#endif
