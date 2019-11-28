/**
 * This file implements a program that output the MST of the actor graph
 *
 * Author Yuening Yang
 */
#include <fstream>
#include <iostream>

#include "ActorGraph.hpp"
#include "cxxopts.hpp"

using namespace std;

/* Main program that runs the findpath */
int main(int argc, char* argv[]) {
    cxxopts::Options options(
        "./movietraveler",
        "Find the minimal spanning tree of the actor graph.");
    options.positional_help(
        "./path_to_graph_info_file"
        "./path_to_output_file [debugging]");

    bool debugging_mode = false;
    string infoFileName, outFileName;
    options.allow_unrecognised_options().add_options()(
        "graphinfo", "", cxxopts::value<string>(infoFileName))(
        "output", "", cxxopts::value<string>(outFileName))(
        "debugging", "", cxxopts::value<bool>(debugging_mode))(
        "h, help", "Print help and exit");

    options.parse_positional({"graphinfo", "output"});
    auto userOptions = options.parse(argc, argv);

    // if invalid format or using help flag, then print help and exit
    // assume the input dataset and query file is formatted well
    if (userOptions.count("help") || outFileName.empty()) {
        cout << options.help({""}) << std::endl;
        exit(0);
    }

    // get graph info and construct graph
    ActorGraph* graph = new ActorGraph;
    if (graph->loadFromFile(infoFileName.c_str(), true)) {
        // prepare the query and output file
        ofstream outFile;
        outFile.open(outFileName);

        // find the shortest path
        graph->findMST(outFile, debugging_mode);

        // close file
        outFile.close();

        // delete graph to release memory
        delete graph;
    }
}
