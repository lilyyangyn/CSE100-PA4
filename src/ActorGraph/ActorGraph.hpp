/**
 * ActorGraph.hpp
 * Author: Yuening YANG, Shenlang ZHOU
 * Date:   11/18/2019
 *
 * This file declares the structure of ActorGraph class,
 * including its private variables and public methods
 */

#ifndef ACTORGRAPH_HPP
#define ACTORGRAPH_HPP

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

/**
 * This class defines the structure and methods of a graph,
 * which shows the corperate relation between actors
 */
class ActorGraph {
  public:
    /** An inner class, instances of which are edges in an ActorGraph */
    class MovieEdge {
      public:
        string key;
        string title;  // movie name
        int year;      // movie year
        int weight;    // weight = age of the movie = 1 + (2019 - Y)
        unordered_set<string>
            actors;  // list of actors who have played in this movie

        /* Constructo that initialize a MovieEdge */
        MovieEdge(string key, string name, int year, bool use_weighted_edges);

        /* Comparator of MovieEdge pointer. */
        struct WeightComp {
            /* a comparator of MovieEdge pointer.
             * The edge with lower weight value will have appear first */
            bool operator()(MovieEdge* left, MovieEdge* right) const;
        };
    };

    /** An inner class, instances of which are vertices in an ActorGraph */
    class ActorNode {
      public:
        string name;  // name of the actor
        unordered_set<MovieEdge*>
            movies;  // list of movies the actor has played

        unsigned int dist;    // dist from the start actor
        ActorNode* prevNode;  // prev actor node
        MovieEdge* prevEdge;  // prev movie edge

        unsigned int priority;  // priority used in link prediction

        ActorNode* disjointSetParent;  // parent node in the disjoint set

        /* Constructo that initialize an ActorNode */
        ActorNode(string name);

        /* get num of the edges between the given actor and current actor */
        int getEdgeNum(string actorName);

        /* Comparator of ActorNode pointer. */
        struct PriorityComp {
            /* a comparator of ActorNode pointer.
             * The node with lower priority value will have higher priority
             * If 2 nodes are of the same priority value the node with name in
             * higher alphebetic order will have higher priority */
            bool operator()(ActorNode* left, ActorNode* right) const;
        };

        /* Comparator of ActorNode pointer. */
        struct DistComp {
            /* a comparator of ActorNode pointer.
             * The node with lower dist value will have higher priority */
            bool operator()(ActorNode* left, ActorNode* right) const;
        };
    };

  protected:
    unordered_map<string, ActorNode*> actors;
    unordered_map<string, MovieEdge*> movies;

  public:
    /**
     * Constuctor of the Actor graph
     */
    ActorGraph(void);

    /* find the shortest path of the graph */
    void find_path(string startActorName, string endActorName, ostream& outFile,
                   bool use_weighted_edges);

    /* predict possible future collaberation of other actors and the given actor
     */
    void predictlink(string targetActorName, ostream& outFile1,
                     ostream& outFile2);

    /* find the minimal spanning tree of the connected graph */
    void findMST(ostream& outFile, bool show_abstract_only);

    /* helper method to insert (actor, movie) pair into the graph */
    void insert(string actor, string movie_title, int year,
                bool use_weighted_edges);

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
     * Destuctor of the Actor Graph
     */
    ~ActorGraph();

  public:
    /** inner set, which are used to construct disjoint set */
    struct DisjointSet {
        unordered_map<string, string> parents;
        unordered_map<string, int> weights;

        /* Constructor of DisjointSet */
        DisjointSet(unordered_map<string, ActorNode*>& actors);
        /* union 2 sets, which contain actor1 and actor2 respectively */
        void union_set(string actor1, string actor2);
        /* find the sentinel node of the set actor is in. Compress path at the
         * same time */
        string find_sentinel(string actor);
    };
};

#endif  // ACTORGRAPH_HPP
