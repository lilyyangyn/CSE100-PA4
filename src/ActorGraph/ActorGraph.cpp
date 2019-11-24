/*
 * ActorGraph.cpp
 * Author: Yuening YANG, Shenlang ZHOU
 * Date:   11/18/2019
 *
 * This file is meant to exist as a container for starter code that you can use
 * to read the input file format defined in imdb_2019.tsv. Feel free to modify
 * any/all aspects as you wish.
 */

#include "ActorGraph.hpp"
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const int WEIGHT_HELPER = 2020;

/**
 * Constructor of the Actor graph
 */
ActorGraph::ActorGraph(void) {}

/**
 * Destructor of the Actor graph
 */
ActorGraph::~ActorGraph() {
    for (auto itr = actors.begin(); itr != actors.end(); itr++) {
        delete itr->second;
    }
    for (auto itr = movies.begin(); itr != movies.end(); itr++) {
        delete itr->second;
    }
}

/* find the shortest path of the graph */
void ActorGraph::find_unweighted_path(string startActorName,
                                      string endActorName, ostream& outFile) {
    // if the start and end are the same actor, then output an empty line
    if (startActorName == endActorName) {
        outFile << endl;
        return;
    }

    // if not exists, then output an empty line
    if (actors.count(startActorName) == 0 || actors.count(endActorName) == 0) {
        outFile << endl;
        return;
    }
    // get start actor node and end actor node
    ActorNode* startActor = actors.at(startActorName);
    ActorNode* endActor = actors.at(endActorName);

    // use BFS to find the shortest path in an unweighted graph
    queue<ActorNode*> toExplore;
    // reset the graph
    for (auto itr = actors.begin(); itr != actors.end(); itr++) {
        itr->second->dist = INT32_MAX;
        itr->second->prevEdge = 0;
        itr->second->prevNode = 0;
    }
    startActor->dist = 0;
    toExplore.push(startActor);
    // build path
    while (!toExplore.empty()) {
        ActorNode* current = toExplore.front();
        toExplore.pop();
        for (MovieEdge* nextEdge : current->movies) {
            for (string nextActorName : nextEdge->actors) {
                ActorNode* next = actors.at(nextActorName);
                if (next->dist == INT32_MAX) {
                    next->dist = current->dist + 1;
                    next->prevNode = current;
                    next->prevEdge = nextEdge;
                    // if get target end actor, break
                    if (next == endActor) break;
                    // otherwise, push it to the queue
                    toExplore.push(next);
                }
            }
        }
    }

    // if no path between startActor and endActor, then output an empty line
    if (endActor->dist == INT32_MAX) {
        outFile << endl;
        return;
    }

    ActorNode* actor = endActor;
    MovieEdge* movie;
    string path = "(" + actor->name + ")";
    while (actor != startActor) {
        movie = actor->prevEdge;
        actor = actor->prevNode;
        path = "(" + actor->name + ")--[" + movie->title + "#@" +
               to_string(movie->year) + "]-->" + path;
    }
    outFile << path << endl;
}

/* find the shortest path of the weighted graph */
void ActorGraph::find_weighted_path(string startActorName, string endActorName,
                                    ostream& outFile) {}

/** You can modify this method definition as you wish
 *
 * Load the graph from a tab-delimited file of actor->movie relationships.
 *
 * in_filename - input filename
 * use_weighted_edges - if true, compute edge weights as 1 + (2019 -
 * movie_year), otherwise all edge weights will be 1
 *
 * return true if file was loaded sucessfully, false otherwise
 */
bool ActorGraph::loadFromFile(const char* in_filename,
                              bool use_weighted_edges) {
    // Initialize the file stream
    ifstream infile(in_filename);

    bool have_header = false;

    // keep reading lines until the end of file is reached
    while (infile) {
        string s;

        // get the next line
        if (!getline(infile, s)) break;

        if (!have_header) {
            // skip the header
            have_header = true;
            continue;
        }

        istringstream ss(s);
        vector<string> record;

        while (ss) {
            string str;

            // get the next string before hitting a tab character and put it in
            // 'str'
            if (!getline(ss, str, '\t')) break;
            record.push_back(str);
        }

        if (record.size() != 3) {
            // we should have exactly 3 columns
            continue;
        }

        string actor(record[0]);
        string movie_title(record[1]);
        int year = stoi(record[2]);

        // create new node and edge if not exists
        if (!actors.count(actor)) {
            // key not exists, create new actor node
            actors.emplace(actor, new ActorNode(actor));
        }
        if (!movies.count(movie_title)) {
            // key not exists, create new movie edge
            movies.emplace(movie_title, new MovieEdge(movie_title, year,
                                                      use_weighted_edges));
        }

        // update the list. there's impossible for repitition of (actor, movie)
        movies[movie_title]->actors.insert(actor);
        actors[actor]->movies.insert(movies[movie_title]);
    }
    if (!infile.eof()) {
        cerr << "Failed to read " << in_filename << "!\n";
        return false;
    }
    infile.close();

    return true;
}

/* Constructo that initialize a MovieEdge */
ActorGraph::MovieEdge::MovieEdge(string name, int year, bool use_weighted_edges)
    : title(name), year(year) {
    if (use_weighted_edges) {
        weight = WEIGHT_HELPER - year;
    } else {
        weight = 1;
    }
}

/* find if there's at least one edge between the given actor and current actor
 */
bool ActorGraph::ActorNode::hasEdge(string actorName) {
    for (MovieEdge* me : movies) {
        if (me->actors.count(actorName)) {
            return true;
        }
    }
    return false;
}

/* get weight of the edge between the given actor and current actor */
int ActorGraph::ActorNode::getEdgeNum(string actorName) {
    int weight = 0;
    for (MovieEdge* me : movies) {
        if (me->actors.count(actorName)) {
            weight++;
        }
    }
    return weight;
}

/* Constructo that initialize an ActorNode */
ActorGraph::ActorNode::ActorNode(string name) : name(name), priority(0) {}

/* set the priority according to the target ActorNode */
void ActorGraph::ActorNode::setPriority(ActorNode* target) {}

bool ActorGraph::ActorNode::ActorNodeComp::operator()(ActorNode* left,
                                                      ActorNode* right) const {
    if (left->priority == right->priority) {
        return left->name < right->name;
    } else {
        return left->priority > right->priority;
    }
}