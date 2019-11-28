/**
 * CityGraoh.hpp
 * Author: Yuening YANG, Shenlang ZHOU
 * Date:   11/28/2019
 *
 * This file declares the structure of CityGraph class,
 * including its private variables and public methods
 */
#ifndef CITYGRAPH_HPP
#define CITYGRAPH_HPP

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

/**
 * This class defines the structure and methods of a graph,
 * which shows the connectivity of citis
 */
class CityGraph {
  public:
    class CityNode {
      public:
        string name;  // name of the city
        int x;        // x-pos of the city
        int y;        // y-pos of the city

        unordered_set<string> neighbors;  // adjacency list of neighbor cities

        double dist;       // dist from the start city
        double heuristic;  // heuristic function value to the target city
        CityNode* prev;    // previous city in the path

        /* Constructor for CityNode */
        CityNode(string name, int x, int y);

        /* Comparator of CityNode pointer. */
        struct FValueComp {
            /* a comparator of CityNode pointer.
             * The node with lower (f = dist+ heuristic) value will have higher
             * priority If 2 nodes are of the same priority value the node with
             * name in lower alphebetic order will have higher priority */
            bool operator()(CityNode* left, CityNode* right) const;
        };
    };

  private:
    unordered_map<string, CityNode*> cities;

  public:
    /* Constructor of CityGraph */
    CityGraph(void);

    /* find the shortest path from start city to target city using A* Algorithm
     */
    void find_path(string startCityName, string endCityName, ostream& out);

    /* helper method to insert city into the graph */
    void insertCity(string city, int x, int y);

    /* helper method to insert road (edge) into the graph */
    void insertRoad(string city1, string city2);

    /**
     * Load the graph from a tab-delimited file of city->position(x, y)
     * relationships.
     *
     * in_filename - input filename
     * return true if file was loaded sucessfully, false otherwise
     */
    bool loadFromFile(const char* cityxy, const char* citypair);

    /* return the copy of unordered_map cities for debugging */
    unordered_map<string, CityNode*> getCities() { return cities; }

    /* Destructor of the City Graph */
    ~CityGraph();
};
#endif