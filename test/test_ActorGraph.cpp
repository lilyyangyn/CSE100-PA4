#include <gtest/gtest.h>
#include <iostream>
#include "ActorGraph.hpp"

using namespace std;
using namespace testing;

class SmallUnweightedGraphFixture : public ::testing::Test {
  protected:
    ActorGraph graph;

  public:
    SmallUnweightedGraphFixture() {
        string infoFileName = "/Code/cse100_pa4/data/imdb_small_sample.tsv";
        graph.loadFromFile(infoFileName.c_str(), false);
    }
};

/* check if all actors and movies read successfully */
TEST_F(SmallUnweightedGraphFixture, ACTOR_MOVIE_TEST) {
    vector<string> actors = {"Kevin Bacon",        "James McAvoy",
                             "Michael Fassbender", "Samuel L. Jackson",
                             "Robert Downey Jr.",  "Tom Holland",
                             "Katherine Waterston"};
    vector<string> movies = {"X-Men: First Class#@2011",
                             "X-Men: Apocalypse#@2016",
                             "Glass#@2019",
                             "Alien: Covenant#@2017",
                             "Avengers: Endgame#@2019",
                             "Spider-Man: Homecoming#@2017",
                             "The Current War#@2017"};

    // expect the unordered_map contains and only contains all actors above
    ASSERT_EQ(graph.getActors().size(), actors.size());
    for (string actor : actors) {
        EXPECT_TRUE(graph.getActors().count(actor));
    }

    // expect the unordered_map contains and only contains all actors above
    ASSERT_EQ(graph.getMovies().size(), movies.size());
    for (string movie : movies) {
        EXPECT_TRUE(graph.getMovies().count(movie));
    }
}

/* check the shortest path in unweighted mode */
TEST_F(SmallUnweightedGraphFixture, UNWEIGHTED_SHORTEST_PATH_TEST) {
    ostringstream os;
    // test direct neighbor
    graph.find_path("Kevin Bacon", "James McAvoy", os, false);
    EXPECT_EQ(os.str(),
              "(Kevin Bacon)--[X-Men: First Class#@2011]-->(James McAvoy)\n");
    os.str("");

    // test indirect long path
    graph.find_path("Kevin Bacon", "Tom Holland", os, false);
    EXPECT_EQ(os.str(),
              "(Kevin Bacon)--[X-Men: First Class#@2011]-->(Michael "
              "Fassbender)--[Alien: Covenant#@2017]-->(Katherine "
              "Waterston)--[The Current War#@2017]-->(Tom Holland)\n");
    os.str("");

    // test if start actor not exist
    graph.find_path("Nobody", "Tom Holland", os, false);
    EXPECT_EQ(os.str(), "\n");
}

/* check the shortest path in weighted mode */
TEST_F(SmallUnweightedGraphFixture, WEIGHTED_SHORTEST_PATH_TEST) {
    // to be implemented
    ostringstream os;
    graph.find_path("Kevin Bacon", "James McAvoy", os, true);
}

/* check whether predictlink works well */
TEST_F(SmallUnweightedGraphFixture, PREDICT_LINK_TEST) {
    ostringstream os1;
    ostringstream os2;

    // normal prediction 1 (diff priority)
    graph.predictlink("James McAvoy", os1, os2);
    EXPECT_EQ(os1.str(),
              "Kevin Bacon\tMichael Fassbender\tSamuel L. Jackson\t\n");
    EXPECT_EQ(os2.str(), "Katherine Waterston\tRobert Downey Jr.\t\n");

    os1.str("");
    os2.str("");
    // normal prediction 2 (same priority)
    graph.predictlink("Robert Downey Jr.", os1, os2);
    EXPECT_EQ(os1.str(), "Samuel L. Jackson\tTom Holland\t\n");
    EXPECT_EQ(os2.str(), "James McAvoy\tKatherine Waterston\t\n");

    os1.str("");
    os2.str("");
    // test when target actor does not show up in graph
    graph.predictlink("Nobody", os1, os2);
    EXPECT_EQ(os1.str(), "\n");
    EXPECT_EQ(os2.str(), "\n");
}

/* find number of edges between two actor nodes */
TEST_F(SmallUnweightedGraphFixture, GET_EDGE_NUM_TEST) {
    ActorGraph::ActorNode* actor = graph.getActors().at("James McAvoy");
    EXPECT_EQ(actor->getEdgeNum("Michael Fassbender"), 2);
    EXPECT_EQ(actor->getEdgeNum("Tom Holland"), 0);
}
