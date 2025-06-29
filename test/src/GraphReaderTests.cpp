#include <gtest/gtest.h>
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <lorina/lorina.hpp>
using namespace CG_Graph;
TEST(SimpleReadingTest, SomeStringPrints) {
  Context context = Context();
  EXPECT_NO_THROW(
      OrientedGraph::readVerilog("../../../test/src/temp.v", context));
  GraphPtr graph = context.d_graphs["temp"];
  EXPECT_EQ(graph->getGatesCount()[GateAnd], 1);
}