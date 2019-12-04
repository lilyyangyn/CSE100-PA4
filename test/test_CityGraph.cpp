/**
 * This file performs unit tests for CityGraph.
 *
 * Author: Yuening YANG, Shenlang Zhou
 * Email: y3yang@ucse.edu
 */
#include <gtest/gtest.h>
#include <iostream>
#include "CityGraph.hpp"
#include "HelpUtil.hpp"

using namespace std;
using namespace testing;

/**
 * A simple test fixture of unweighted graph from which multiple tests can be
 * written. Rebuit after every test.
 */
class SmallCityGraphFixture : public ::testing::Test {
  protected:
    CityGraph graph;

  public:
    SmallCityGraphFixture() {
        graph.insertCity("A", 0, 3);
        graph.insertCity("B", 2, 3);
        graph.insertCity("C", 6, 3);
        graph.insertCity("D", 1, 5);
        graph.insertCity("E", 5, 0);
        graph.insertCity("F", 10, 5);

        graph.insertRoad("A", "D");
        graph.insertRoad("A", "B");
        graph.insertRoad("A", "E");
        graph.insertRoad("B", "E");
        graph.insertRoad("D", "E");
        graph.insertRoad("C", "D");
        graph.insertRoad("C", "E");
    }
};

/* check if all actors and movies read successfully */
TEST_F(SmallCityGraphFixture, CITY_TEST) {
    vector<string> cities = {"A", "B", "C", "D", "E", "F"};

    // expect the unordered_map contains and only contains all cities above
    ASSERT_EQ(graph.getCities().size(), cities.size());
    for (string city : cities) {
        EXPECT_TRUE(graph.getCities().count(city));
    }
}

/* check the shortest path in unweighted mode */
TEST_F(SmallCityGraphFixture, A_START_SEARCH_TEST) {
    ostringstream os;
    // connected graph
    graph.find_path("A", "C", os);
    EXPECT_EQ(os.str(), "(A)-->(D)-->(C)\n");
    os.str("");

    // disconnected graph (no path)
    graph.find_path("A", "F", os);
    EXPECT_EQ(os.str(), "\n");
    os.str("");

    // test if start city not exist
    graph.find_path("Nowhere", "C", os);
    EXPECT_EQ(os.str(), "\n");
}

/* test navigatesystem helper method */
TEST_F(SmallCityGraphFixture, HELP_UTIL_FIND_ASTAR_TEST) {
    istringstream is;
    is.str("A C\nA F\n");

    ostringstream os;
    HelpUtil::find_AStar_paths(&graph, is, os);

    EXPECT_EQ(os.str(), "(A)-->(D)-->(C)\n\n");
}

/* test load function */
TEST(CityGraphTests, LOAD_TEST) {
    string cityxyFile = "/Code/cse100_pa4/data/cityxy.txt";
    string citypairFile = "/Code/cse100_pa4/data/citypairs.txt";
    CityGraph cityGraph;
    cityGraph.loadFromFile(cityxyFile.c_str(), citypairFile.c_str());
    EXPECT_TRUE(1);
}
