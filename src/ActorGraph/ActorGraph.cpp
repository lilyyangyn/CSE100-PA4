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

const int WEIGHT_HELPER = 2020;     // current year + 1
const int LINK_PREDICTOR_SIZE = 4;  // number of actors wanted in linkpredictor

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
void ActorGraph::find_path(string startActorName, string endActorName,
                           ostream& outFile, bool use_weighted_edges) {
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

    // reset the graph
    for (auto itr = actors.begin(); itr != actors.end(); itr++) {
        itr->second->dist = INT32_MAX;
        itr->second->prevEdge = 0;
        itr->second->prevNode = 0;
    }
    startActor->dist = 0;

    if (use_weighted_edges) {
        // use Dijkstra's Algorithm to find the shortest path in a weighted
        // graph.
        priority_queue<ActorNode*, vector<ActorNode*>, ActorNode::DistComp>
            toExplore;
        toExplore.push(startActor);

        while (!toExplore.empty()) {
            ActorNode* current = toExplore.top();
            // if get target end actor, break
            if (current == endActor) break;
            toExplore.pop();
            for (MovieEdge* nextEdge : current->movies) {
                int newDist = current->dist + nextEdge->weight;
                for (string nextActorName : nextEdge->actors) {
                    ActorNode* next = actors.at(nextActorName);
                    if (newDist < next->dist) {
                        next->dist = newDist;
                        next->prevNode = current;
                        next->prevEdge = nextEdge;
                        // push it to the priority queue
                        toExplore.push(next);
                    }
                }
            }
        }
    } else {
        // use BFS to find the shortest path in an unweighted graph
        queue<ActorNode*> toExplore;
        toExplore.push(startActor);
        // build path
        while (!toExplore.empty()) {
            ActorNode* current = toExplore.front();
            // if get target end actor, break
            if (current == endActor) break;
            toExplore.pop();
            for (MovieEdge* nextEdge : current->movies) {
                for (string nextActorName : nextEdge->actors) {
                    ActorNode* next = actors.at(nextActorName);
                    if (next->dist == INT32_MAX) {
                        next->dist = current->dist + 1;
                        next->prevNode = current;
                        next->prevEdge = nextEdge;
                        // push it to the queue
                        toExplore.push(next);
                    }
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
        path = "(" + actor->name + ")--[" + movie->key + "]-->" + path;
    }
    outFile << path << endl;
}

/* predict possible future collaberation of other actors and the given actor
 */
void ActorGraph::predictlink(string targetActorName, ostream& outFile1,
                             ostream& outFile2) {
    // if actor does not exist in the graph, output an empty line
    if (actors.count(targetActorName) == 0) {
        outFile1 << endl;
        outFile2 << endl;
        return;
    }

    ActorNode* targetActor = actors.at(targetActorName);

    // reset the graph
    for (auto itr = actors.begin(); itr != actors.end(); itr++) {
        itr->second->priority = 0;
    }

    // get direct neighbor
    unordered_set<ActorNode*> collaberated;
    unordered_set<ActorNode*> not_collaberated;
    for (auto movie : targetActor->movies) {
        for (string actorName : movie->actors) {
            // get actors who have collaberated with our target actor
            collaberated.insert(actors.at(actorName));
        }
    }

    // get the priority of actors and the not directly connected actors
    for (auto movie1 : targetActor->movies) {
        for (string firstActorName : movie1->actors) {
            if (firstActorName == targetActorName) {
                // cannot edge to itself
                continue;
            }
            // get actors who have collaberated with our target actor
            ActorNode* firstActor = actors.at(firstActorName);
            for (auto movie2 : firstActor->movies) {
                for (string secondActorName : movie2->actors) {
                    if (secondActorName == targetActorName ||
                        secondActorName == firstActorName) {
                        // cannot edge to itself
                        continue;
                    }
                    // get actors who have collaberated with the first-level
                    // actor
                    ActorNode* secondActor = actors.at(secondActorName);
                    // increase priority (as there is a new path
                    // found)
                    secondActor->priority++;
                    if (collaberated.count(secondActor) == 0 &&
                        not_collaberated.count(secondActor) == 0) {
                        not_collaberated.insert(secondActor);
                    }
                }
            }
        }
    }

    // get the 4 of highest priority of actors who have collaberated with target
    priority_queue<ActorNode*, vector<ActorNode*>, ActorNode::PriorityComp>
        collaberated_pq;
    for (auto itr = collaberated.begin(); itr != collaberated.end(); itr++) {
        if ((*itr)->name == targetActorName) {
            continue;
        }
        if (collaberated_pq.size() < LINK_PREDICTOR_SIZE) {
            collaberated_pq.push(*itr);
        } else {
            if (((*itr)->priority > collaberated_pq.top()->priority) ||
                ((*itr)->priority == collaberated_pq.top()->priority &&
                 (*itr)->name < collaberated_pq.top()->name)) {
                collaberated_pq.pop();
                collaberated_pq.push(*itr);
            }
        }
    }
    string output1 = "";
    while (collaberated_pq.size() > 0) {
        output1 = collaberated_pq.top()->name + "\t" + output1;
        collaberated_pq.pop();
    }
    outFile1 << output1 << endl;

    // get the 4 of highest priority of actors who have not collaberated with
    // target
    priority_queue<ActorNode*, vector<ActorNode*>, ActorNode::PriorityComp>
        not_collaberated_pq;
    for (auto itr = not_collaberated.begin(); itr != not_collaberated.end();
         itr++) {
        if (not_collaberated_pq.size() < LINK_PREDICTOR_SIZE) {
            not_collaberated_pq.push(*itr);
        } else {
            if (((*itr)->priority > not_collaberated_pq.top()->priority) ||
                ((*itr)->priority == not_collaberated_pq.top()->priority &&
                 (*itr)->name < not_collaberated_pq.top()->name)) {
                not_collaberated_pq.pop();
                not_collaberated_pq.push(*itr);
            }
        }
    }
    string output2 = "";
    while (not_collaberated_pq.size() > 0) {
        output2 = not_collaberated_pq.top()->name + "\t" + output2;
        not_collaberated_pq.pop();
    }
    outFile2 << output2 << endl;
}

/* find the minimal spanning tree of the connected graph */
void ActorGraph::findMST(ostream& outFile) {}

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

            // get the next string before hitting a tab character and put it
            // in 'str'
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
        string movie_key = movie_title + "#@" + to_string(year);

        // create new node and edge if not exists
        if (!actors.count(actor)) {
            // key not exists, create new actor node
            actors.emplace(actor, new ActorNode(actor));
        }
        if (!movies.count(movie_key)) {
            // key not exists, create new movie edge
            movies.emplace(movie_key, new MovieEdge(movie_key, movie_title,
                                                    year, use_weighted_edges));
        }

        // update the list. there's impossible for repitition of (actor,
        // movie)
        movies[movie_key]->actors.insert(actor);
        actors[actor]->movies.insert(movies[movie_key]);
    }
    if (!infile.eof()) {
        cerr << "Failed to read " << in_filename << "!\n";
        return false;
    }
    infile.close();

    return true;
}

/* Constructo that initialize a MovieEdge */
ActorGraph::MovieEdge::MovieEdge(string key, string name, int year,
                                 bool use_weighted_edges)
    : key(key), title(name), year(year) {
    if (use_weighted_edges) {
        weight = WEIGHT_HELPER - year;
    } else {
        weight = 1;
    }
}

/* a comparator of MovieEdge pointer.
 * The edge with lower weight value will have appear first */
bool ActorGraph::MovieEdge::WeightComp::operator()(MovieEdge* left,
                                                   MovieEdge* right) const {
    return left->weight < right->weight;
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

/* a comparator of ActorNode pointer.
 * The node with lower priority value will have higher priority
 * If 2 nodes are of the same priority value the node with name in
 * higher alphebetic order will have higher priority */
bool ActorGraph::ActorNode::PriorityComp::operator()(ActorNode* left,
                                                     ActorNode* right) const {
    if (left->priority == right->priority) {
        return left->name < right->name;
    } else {
        return left->priority > right->priority;
    }
}

/* a comparator of ActorNode pointer.
 * The node with lower dist value will have higher priority */
bool ActorGraph::ActorNode::DistComp::operator()(ActorNode* left,
                                                 ActorNode* right) const {
    return left->dist > right->dist;
}