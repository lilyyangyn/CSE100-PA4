/**
 * ActorGraph.hpp
 * Author: Yuening YANG, Shenlang ZHOU
 * Date:   11/18/2019
 *
 * This file is meant to exist as a container for starter code that you can use
 * to read the input file format defined imdb_2019.tsv. Feel free to modify
 * any/all aspects as you wish.
 */

#ifndef ACTORGRAPH_HPP
#define ACTORGRAPH_HPP

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
// Maybe include some data structures here

using namespace std;

/**
 * This class defines the structure and methods of a graph,
 * which shows the corperate relation between actors
 */
class ActorGraph {
  protected:
    /** An inner class, instances of which are edges in an ActorGraph */
    class MovieEdge {
      public:
        string title;  // movie name
        int year;      // movie year
        int weight;    // weight = age of the movie = 1 + (2019 - Y)
        unordered_set<string>
            actors;  // list of actors who have played in this movie

        /* Constructo that initialize a MovieEdge */
        MovieEdge(string name, int year, bool use_weighted_edges);
    };

    /** An inner class, instances of which are vertices in an ActorGraph */
    class ActorNode {
      public:
        string name;  // name of the actor
        unordered_set<MovieEdge*>
            movies;  // list of movies the actor has played

        unsigned int dist;
        ActorNode* prevNode;  // prev actor node
        MovieEdge* prevEdge;  // prev movie edge

        unsigned int priority;  // priority used in link prediction

        /* Constructo that initialize an ActorNode */
        ActorNode(string name);

        /* find if there's at least one edge between the given actor and current
         * actor */
        bool hasEdge(string actorName);

        /* get num of the edges between the given actor and current actor */
        int getEdgeNum(string actorName);

        /* set the priority according to the target ActorNode */
        void setPriority(ActorNode* target);

        /* Comparator of ActorNode pointer. */
        struct ActorNodeComp {
            /* a comparator of ActorNode pointer.
             * The node with higher priority will come first
             * If 2 nodes are of the same priority, the node with name in
             * lower alphebetic order will come first */
            bool operator()(ActorNode* left, ActorNode* right) const;
        };
    };

    unordered_map<string, ActorNode*> actors;
    unordered_map<string, MovieEdge*> movies;

  public:
    /**
     * Constuctor of the Actor graph
     */
    ActorGraph(void);

    /* find the shortest path of the unweighted graph */
    void find_unweighted_path(string startActorName, string endActorName,
                              ostream& outFile);

    /* find the shortest path of the weighted graph */
    void find_weighted_path(string startActorName, string endActorName,
                            ostream& outFile);

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
    bool loadFromFile(const char* in_filename, bool use_weighted_edges);

    /* return the copy of unordered_map actors for debugging */
    unordered_map<string, ActorNode*> getActors() { return actors; }
    /* return the copy of unordered_map movies for debugging */
    unordered_map<string, MovieEdge*> getMovies() { return movies; }

    /**
     * Destuctor of the Actor graph
     */
    ~ActorGraph();
};

#endif  // ACTORGRAPH_HPP
