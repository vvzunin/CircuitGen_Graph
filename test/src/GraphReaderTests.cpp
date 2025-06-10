#include <gtest/gtest.h>
#include<CircuitGenGraph/GraphReader.hpp>
#include<CircuitGenGraph/OrientedGraph.hpp>
#include<lorina/lorina.hpp>
using namespace CG_Graph;
TEST(SimpleReadingTest, SomeStringPrints){
EXPECT_NO_THROW(OrientedGraph::readVerilog("temp.v"));
GraphPtr graph = OrientedGraph::readVerilog("temp.v","lilmodule");
EXPECT_EQ(graph->getGatesCount()[GateAnd],1);
}