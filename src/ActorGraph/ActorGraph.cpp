/*
 * ActorGraph.cpp
 * Author: Yuening YANG, Shenlang ZHOU
 * Date:   11/18/2019
 *
 * This file shows the implementation details of ActorGraph class (and all its
 * inner classes) methods, which are declared in 'ActorGraph.hpp' file.
 */

#include "ActorGraph.hpp"
#include <algorithm>
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
void ActorGraph::findMST(ostream& outFile, bool show_abstract_only) {
    vector<MovieEdge*> edges;
    // push all movie edges into vector. each movie edge appears only ONCE
    for (auto itr = movies.begin(); itr != movies.end(); itr++) {
        edges.push_back(itr->second);
    }
    // sort edges according to their weight in ascending order
    sort(edges.begin(), edges.end(), MovieEdge::WeightComp());

    // initial V and E of the MST
    vector<string> movie_traveling;

    // construct the MST
    int edgeWeights = 0;
    // create disjoint set
    DisjointSet ds(actors);
    for (int m = 0; m < edges.size(); m++) {
        unordered_set<string>& actorsInMovie = edges[m]->actors;
        int weight = edges[m]->weight;
        // pair each two actors who played in this movie
        for (auto itr1 = actorsInMovie.begin(); itr1 != actorsInMovie.end();
             itr1++) {
            for (auto itr2 = actorsInMovie.begin(); itr2 != actorsInMovie.end();
                 itr2++) {
                string sentinel1 = ds.find_sentinel((*itr1));
                string sentinel2 = ds.find_sentinel((*itr2));
                // if not in the same set
                if (sentinel1 != sentinel2) {
                    // add this edge to MST
                    string path = "(" + (*itr1) + ")<--[" + edges[m]->key +
                                  "]-->(" + (*itr2) + ")\n";
                    movie_traveling.push_back(path);
                    // union two disjoint set
                    ds.union_set(sentinel1, sentinel2);
                    // increasing edge weights
                    edgeWeights += weight;

                    // the total num of edges in MST should be |V| - 1. If
                    // reach, then write output stream and return
                    if (movie_traveling.size() == actors.size() - 1) {
                        // write output file
                        if (!show_abstract_only) {
                            outFile << "(actor)<--[movie#@year]-->(actor)\n";
                            for (int i = 0; i < movie_traveling.size(); i++) {
                                outFile << movie_traveling[i];
                            }
                        }
                        outFile << "#NODE CONNECTED: " << actors.size() << endl;
                        outFile << "#EDGE CHOSEN: " << movie_traveling.size()
                                << endl;
                        outFile << "TOTAL EDGE WEIGHTS: " << edgeWeights
                                << endl;
                        return;
                    }

                    // switch to another actor to let him introduce you to new
                    // actors
                    break;
                }
            }
        }
    }
}

/* helper method to insert (actor, movie) pair into the tree */
void ActorGraph::insert(string actor, string movie_title, int year,
                        bool use_weighted_edges) {
    string movie_key = movie_title + "#@" + to_string(year);

    // create new node and edge if not exists
    if (!actors.count(actor)) {
        // key not exists, create new actor node
        actors.emplace(actor, new ActorNode(actor));
    }
    if (!movies.count(movie_key)) {
        // key not exists, create new movie edge
        movies.emplace(movie_key, new MovieEdge(movie_key, movie_title, year,
                                                use_weighted_edges));
    }

    // update the list. there's impossible for repitition of (actor,
    // movie)
    movies[movie_key]->actors.insert(actor);
    actors[actor]->movies.insert(movies[movie_key]);
}

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

        insert(actor, movie_title, year, use_weighted_edges);
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

/* Constructor of DisjointSet */
ActorGraph::DisjointSet::DisjointSet(
    unordered_map<string, ActorNode*>& actors) {
    for (auto itr = actors.begin(); itr != actors.end(); itr++) {
        // create a disjoint set for each node by setting the parent equal to
        // the node itself
        parents.emplace(itr->first, itr->first);
        weights.emplace(itr->first, 1);
    }
}

/* union 2 sets, which contain actor1 and actor2 respectively */
void ActorGraph::DisjointSet::union_set(string actor1, string actor2) {
    // get parents of both nodes respectively
    string sentinel1 = find_sentinel(actor1);
    string sentinel2 = find_sentinel(actor2);
    if (sentinel1 != sentinel2) {
        // not in the same set, use the sentinel with larger weight as parent
        if (weights.at(sentinel1) > weights.at(sentinel2)) {
            parents.at(sentinel2) = sentinel1;
            weights.at(sentinel1) += weights.at(sentinel2);
        } else {
            parents.at(sentinel1) = sentinel2;
            weights.at(sentinel2) += weights.at(sentinel1);
        }
    }
}

/* find the sentinel node of the set actor is in. Compress path at the
 * same time */
string ActorGraph::DisjointSet::find_sentinel(string actor) {
    // get parent node
    string parent = parents.at(actor);
    // actor is not the sentinel node
    if (parent != actor) {
        parents.at(actor) = find_sentinel(parent);
    }
    return parents.at(actor);
}