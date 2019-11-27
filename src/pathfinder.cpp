/**
 * This file implements a program that takes the actor-pair input
 * and find the shortest length path between them
 *
 * Author Yuening Yang
 */
#include <fstream>
#include <iostream>

#include "ActorGraph.hpp"
#include "cxxopts.hpp"

using namespace std;

/* find the shortest path of the graph */
void find_graph_paths(ActorGraph* graph, istream& inFile, ostream& outFile,
                      bool use_weighted_edges) {
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
        graph->find_path(targets[0], targets[1], outFile, use_weighted_edges);
    }
    if (!inFile.eof()) {
        cerr << "Failed to read the query file!\n";
    }
}

/* Main program that runs the findpath */
int main(int argc, char* argv[]) {
    cxxopts::Options options("./findpath",
                             "Find the shortest path from one actor to another "
                             "actor through shared movies.");
    options.positional_help(
        "./path_to_graph_info_file mode[u/w] "
        "./path_to_input_file ./path_to_output_file");

    char findMode;
    string infoFileName, inFileName, outFileName;
    options.allow_unrecognised_options().add_options()(
        "graphinfo", "", cxxopts::value<string>(infoFileName))(
        "mode", "Find the shortest path of weighted(w) or unweighted(u) graph",
        cxxopts::value<char>(findMode))("input", "",
                                        cxxopts::value<string>(inFileName))(
        "output", "", cxxopts::value<string>(outFileName))(
        "h, help", "Print help and exit");

    options.parse_positional({"graphinfo", "mode", "input", "output"});
    auto userOptions = options.parse(argc, argv);

    // if invalid format or using help flag, then print help and exit
    // assume the input dataset and query file is formatted well
    if (userOptions.count("help") || outFileName.empty()) {
        cout << options.help({""}) << std::endl;
        exit(0);
    }

    // get option type
    bool use_weighted_edges;
    if (findMode == 'u') {
        use_weighted_edges = false;
    } else if (findMode == 'w') {
        use_weighted_edges = true;
    } else {
        // print help and exit if mode is invalid
        cout << options.help({""}) << std::endl;
        exit(0);
    }

    // get graph info and construct graph
    ActorGraph* graph = new ActorGraph;
    if (graph->loadFromFile(infoFileName.c_str(), use_weighted_edges)) {
        // prepare the query and output file
        ifstream inFile;
        inFile.open(inFileName);
        ofstream outFile;
        outFile.open(outFileName);

        // find the shortest path
        find_graph_paths(graph, inFile, outFile, use_weighted_edges);

        // close file
        inFile.close();
        outFile.close();

        // delete graph to release memory
        delete graph;
    }
}
