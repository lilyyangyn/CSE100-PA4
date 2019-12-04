/**
 * This file implements all helper methods for the executable programs
 * split main and help functions to help unit test
 *
 * Author Yuening Yang
 */
#include <fstream>
#include <iostream>
#include <sstream>

#include "ActorGraph.hpp"
#include "CityGraph.hpp"

using namespace std;

class HelpUtil {
  public:
    /* find the shortest path of the graph. used in pathfinder.cpp */
    void static find_graph_paths(ActorGraph* graph, istream& inFile,
                                 ostream& outFile, bool use_weighted_edges) {
        // write header
        outFile << "(actor)--[movie#@year]-->(actor)--..." << endl;

        bool have_header = false;

        while (inFile) {
            string s;

            // get the next line
            if (!getline(inFile, s)) break;

            if (!have_header) {
                // skip the header
                have_header = true;
                continue;
            }

            istringstream ss(s);
            vector<string> targets;

            while (ss) {
                string str;

                if (!getline(ss, str, '\t')) break;
                targets.push_back(str);
            }

            if (targets.size() != 2) {
                // we should have exactly 2 columns
                continue;
            }

            // write path
            graph->find_path(targets[0], targets[1], outFile,
                             use_weighted_edges);
        }
    }

    /* find the 4 actors with the highest priority who have collaberated and not
     * collaberated with the the given actor, and output to outFile1 and
     * outFile2 respectively */
    void static predictFutureCollaboration(ActorGraph* graph, istream& inFile,
                                           ostream& outFile1,
                                           ostream& outFile2) {
        // write header
        outFile1 << "Actor1,Actor2,Actor3,Actor4" << endl;
        outFile2 << "Actor1,Actor2,Actor3,Actor4" << endl;

        bool have_header = false;
        while (inFile) {
            string s;

            // get the next line
            if (!getline(inFile, s)) break;

            if (!have_header) {
                // skip the header
                have_header = true;
                continue;
            }

            // predict link and write output
            graph->predictlink(s, outFile1, outFile2);
        }
    }

    /* find the shortest path of the graph */
    void static find_AStar_paths(CityGraph* graph, istream& inFile,
                                 ostream& outFile) {
        while (inFile) {
            string s;

            // get the next line
            if (!getline(inFile, s)) break;

            istringstream ss(s);
            vector<string> targets;

            while (ss) {
                string str;

                if (!getline(ss, str, ' ')) break;
                targets.push_back(str);
            }

            if (targets.size() != 2) {
                // we should have exactly 2 columns
                continue;
            }

            // write path
            graph->find_path(targets[0], targets[1], outFile);
        }
    }
};