/**
 * This file implements a program that find the shortest path
 * using A star algorithm
 *
 * Author Yuening Yang
 */
#include <fstream>
#include <iostream>

#include "CityGraph.hpp"
#include "cxxopts.hpp"

using namespace std;

/* find the shortest path of the graph */
void find_graph_paths(CityGraph* graph, istream& inFile, ostream& outFile) {
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

/* Main program that runs the findpath */
int main(int argc, char* argv[]) {
    cxxopts::Options options("./navigationsystem",
                             "Find the shortest path on the city graph.");
    options.positional_help(
        "./path_to_cityxy_file ./path_to_citypair_file"
        "./path_to_query_file ./path_to_output_file");

    string cityxyFile, citypairFile, inFileName, outFileName;
    options.allow_unrecognised_options().add_options()(
        "cityxy", "", cxxopts::value<string>(cityxyFile))(
        "citypair", "", cxxopts::value<string>(citypairFile))(
        "input", "", cxxopts::value<string>(inFileName))(
        "output", "", cxxopts::value<string>(outFileName))(
        "h, help", "Print help and exit");

    options.parse_positional({"cityxy", "citypair", "input", "output"});
    auto userOptions = options.parse(argc, argv);

    // if invalid format or using help flag, then print help and exit
    // assume the input dataset and query file is formatted well
    if (userOptions.count("help") || outFileName.empty()) {
        cout << options.help({""}) << std::endl;
        exit(0);
    }

    // get graph info and construct graph
    CityGraph* graph = new CityGraph();
    if (graph->loadFromFile(cityxyFile.c_str(), citypairFile.c_str())) {
        // prepare the query and output file
        ifstream inFile;
        inFile.open(inFileName);
        ofstream outFile;
        outFile.open(outFileName);

        // find the shortest path
        find_graph_paths(graph, inFile, outFile);
        if (!inFile.eof()) {
            cerr << "Failed to read the query file!\n";
        }

        // close file
        inFile.close();
        outFile.close();

        // delete graph and release memory
        delete graph;
    }
}