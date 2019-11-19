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
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/**
 * Constructor of the Actor graph
 */
ActorGraph::ActorGraph(void) {}

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
        if (actors.count(actor)) {
            // key not exists, create new actor node
            actors.emplace(actor, new ActorNode(actor));
        }
        if (movies.count(movie_title)) {
            // key not exists, create new movie edge
            movies.emplace(movie_title, new MovieEdge(movie_title, year));
        }

        // update the list. there's impossible for repitition of (actor, movie)
        actors[actor]->movies.push_back(movies[movie_title]);
        movies[movie_title]->actors.push_back(actor);
    }
    if (!infile.eof()) {
        cerr << "Failed to read " << in_filename << "!\n";
        return false;
    }
    infile.close();

    return true;
}

/* Constructo that initialize a MovieEdge */
ActorGraph::MovieEdge::MovieEdge(string name, int year)
    : title(name), weight(2020 - year) {}

/* Constructo that initialize an ActorNode */
ActorGraph::ActorNode::ActorNode(string name) : name(name) {}

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