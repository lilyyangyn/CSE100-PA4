/**
 * This file implements a program that tries to predict whether
 * two actors would act together in the future in an unweighted graph
 *
 * Author Yuening Yang
 */
#include <fstream>
#include <iostream>

#include "ActorGraph.hpp"
#include "cxxopts.hpp"

using namespace std;

/* find the 4 actors with the highest priority who have collaberated and not
 * collaberated with the the given actor, and output to outFile1 and outFile2
 * respectively */
void predictFutureCollaboration(ActorGraph* graph, istream& inFile,
                                ostream& outFile1, ostream& outFile2);

/* Main program that runs the linkpredictor */
int main(int argc, char* argv[]) {
    cxxopts::Options options("./linkpredictor",
                             "predict whether two actors would act together "
                             "in the future.");
    options.positional_help(
        "./path_to_graph_info_file ./path_to_input_file ./path_to_output_file1 "
        "./path_to_output_file2");

    string infoFileName, inFileName, outFileName1, outFileName2;
    options.allow_unrecognised_options().add_options()(
        "graphinfo", "", cxxopts::value<string>(infoFileName))(
        "input", "", cxxopts::value<string>(inFileName))(
        "outputCollaborated", "", cxxopts::value<string>(outFileName1))(
        "outputNotCollaborated", "", cxxopts::value<string>(outFileName2))(
        "h, help", "Print help and exit");

    options.parse_positional(
        {"graphinfo", "input", "outputCollaborated", "outputNotCollaborated"});
    auto userOptions = options.parse(argc, argv);

    // if invalid format or using help flag, then print help and exit
    // assume the input dataset and query file is formatted well
    if (userOptions.count("help") || outFileName1.empty() ||
        outFileName2.empty()) {
        cout << options.help({""}) << std::endl;
        exit(0);
    }

    // get graph info and construct graph
    ActorGraph* graph = new ActorGraph;
    // no need for weighted graph here
    if (graph->loadFromFile(infoFileName.c_str(), false)) {
        // prepare the query and output files
        ifstream inFile;
        inFile.open(inFileName);
        ofstream outFile1;
        outFile1.open(outFileName1);
        ofstream outFile2;
        outFile2.open(outFileName2);

        predictFutureCollaboration(graph, inFile, outFile1, outFile2);

        // close file
        inFile.close();
        outFile1.close();
        outFile2.close();

        // delete graph to release memory
        delete graph;
    }
}

/* find the 4 actors with the highest priority who have collaberated and not
 * collaberated with the the given actor, and output to outFile1 and outFile2
 * respectively */
void predictFutureCollaboration(ActorGraph* graph, istream& inFile,
                                ostream& outFile1, ostream& outFile2) {
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
    if (!inFile.eof()) {
        cerr << "Failed to read the query file!\n";
    }
}