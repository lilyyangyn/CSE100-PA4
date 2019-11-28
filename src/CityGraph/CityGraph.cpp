/*
 * CityGraph.cpp
 * Author: Yuening YANG, Shenlang ZHOU
 * Date:   11/18/2019
 *
 * This file shows the implementation details of CityGraph class (and all its
 * inner classes) methods, which are declared in 'CityGraph.hpp' file.
 */

#include "CityGraph.hpp"
#include <math.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <vector>

using namespace std;

const int c = 1;  // coefficient related to heuristic function

/* Constructor of CityGraph */
CityGraph::CityGraph(void) {}

/* Destructor of the City Graph */
CityGraph::~CityGraph() {
    for (auto itr = cities.begin(); itr != cities.end(); itr++) {
        delete itr->second;
    }
}

/* find the shortest path from start city to target city using A* Algorithm
 */
void CityGraph::find_path(string startCityName, string endCityName,
                          ostream& out) {
    // if the start and end are the same city, then output an empty line
    if (startCityName == endCityName) {
        out << endl;
        return;
    }

    // if city not exists, the output an empty line
    if (cities.count(startCityName) == 0 || cities.count(endCityName) == 0) {
        out << endl;
        return;
    }
    // get start and end nodes
    CityNode* start = cities.at(startCityName);
    CityNode* end = cities.at(endCityName);

    // reset graph
    for (auto itr = cities.begin(); itr != cities.end(); itr++) {
        itr->second->dist = INT32_MAX;
        itr->second->heuristic = sqrt(pow(itr->second->x - end->x, 2) +
                                      pow(itr->second->y - end->y, 2));
        itr->second->prev = 0;
    }
    start->dist = 0;

    // perform A-Start Algorithm to find the shortest path
    priority_queue<CityNode*, vector<CityNode*>, CityNode::FValueComp>
        toExplore;
    toExplore.push(start);

    while (!toExplore.empty()) {
        CityNode* current = toExplore.top();
        // if get target city, break
        if (current == end) break;
        toExplore.pop();
        for (string nextCityName : current->neighbors) {
            CityNode* next = cities.at(nextCityName);
            double distance = sqrt(pow(next->x - current->x, 2) +
                                   pow(next->y - current->y, 2));
            if (distance + current->dist < next->dist) {
                next->dist = current->dist + distance;
                next->prev = current;
                // push it to the priority queue
                toExplore.push(next);
            }
        }
    }

    // if no path (not connected), then output an empty line
    if (end->prev == 0) {
        out << endl;
        return;
    }

    CityNode* city = end;
    string path = "(" + city->name + ")";
    while (city != start) {
        city = city->prev;
        path = "(" + city->name + ")-->" + path;
    }
    out << path << endl;
}

/* helper method to insert city into the graph */
void CityGraph::insertCity(string city, int x, int y) {
    if (!cities.count(city)) {
        cities.emplace(city, new CityNode(city, x, y));
    }
}

/* helper method to insert road (edge) into the graph */
void CityGraph::insertRoad(string city1, string city2) {
    // insert road only if 2 cities both exist
    if (cities.count(city1) && cities.count(city2)) {
        cities.at(city1)->neighbors.insert(city2);
        cities.at(city2)->neighbors.insert(city1);
    }
}

/**
 * Load the graph from a tab-delimited file of city->position(x, y)
 * relationships.
 *
 * in_filename - input filename
 * return true if file was loaded sucessfully, false otherwise
 */
bool CityGraph::loadFromFile(const char* cityxy, const char* citypair) {
    // Initialize the file stream for cityxy
    ifstream infile1(cityxy);

    // keep reading lines until the end of file is reached
    while (infile1) {
        string s;

        // get the next line of cityxy
        if (!getline(infile1, s)) break;

        istringstream ss(s);
        vector<string> record;

        while (ss) {
            string str;

            // get the nest string before hitting a space character and put it
            // in 'str
            if (!getline(ss, str, ' ')) break;
            record.push_back(str);
        }

        if (record.size() != 3) {
            // we should have exactly 3 columns
            continue;
        }

        string city(record[0]);
        int x = stoi(record[1]);
        int y = stoi(record[2]);

        insertCity(city, x, y);
    }
    if (!infile1.eof()) {
        cerr << "Failed to read " << cityxy << "!\n";
        return false;
    }
    infile1.close();

    // Initialize the file stream for citypair
    ifstream infile2(citypair);

    // keep reading lines until the end of file is reached
    while (infile2) {
        string s;

        // get the next line of cityxy
        if (!getline(infile2, s)) break;

        istringstream ss(s);
        vector<string> record;

        while (ss) {
            string str;

            // get the nest string before hitting a space character and put it
            // in 'str
            if (!getline(ss, str, ' ')) break;
            record.push_back(str);
        }

        if (record.size() != 2) {
            // we should have exactly 3 columns
            continue;
        }

        string city1(record[0]);
        string city2(record[1]);

        insertRoad(city1, city2);
    }
    if (!infile2.eof()) {
        cerr << "Failed to read " << citypair << "!\n";
        return false;
    }
    infile2.close();

    return true;
}

/* Constructor for CityNode */
CityGraph::CityNode::CityNode(string name, int x, int y)
    : name(name), x(x), y(y) {}

/* Comparator of CityNode pointer. */
bool CityGraph::CityNode::FValueComp::operator()(CityNode* left,
                                                 CityNode* right) const {
    if (left->dist + c * left->heuristic ==
        right->dist + c * right->heuristic) {
        return left->name > right->name;
    } else {
        return left->dist + c * left->heuristic >
               right->dist + c * right->heuristic;
    }
}