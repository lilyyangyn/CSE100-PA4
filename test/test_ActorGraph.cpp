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
    vector<string> movies = {
        "X-Men: First Class", "X-Men: Apocalypse", "Glass",
        "Alien: Covenant",    "Avengers: Endgame", "Spider-Man: Homecoming",
        "The Current War"};

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
    graph.find_unweighted_path("Kevin Bacon", "James McAvoy", os);
    EXPECT_EQ(os.str(),
              "(Kevin Bacon)--[X-Men: First Class#@2011]-->(James McAvoy)\n");
    os.str("");

    // test indirect long path
    graph.find_unweighted_path("Kevin Bacon", "Tom Holland", os);
    EXPECT_EQ(os.str(),
              "(Kevin Bacon)--[X-Men: First Class#@2011]-->(Michael "
              "Fassbender)--[Alien: Covenant#@2017]-->(Katherine "
              "Waterston)--[The Current War#@2017]-->(Tom Holland)\n");
    os.str("");

    // test if start actor not exist
    graph.find_unweighted_path("Nobody", "Tom Holland", os);
    EXPECT_EQ(os.str(), "\n");
}

TEST(ActorGraphTest, SIMPLE_TEST) {}