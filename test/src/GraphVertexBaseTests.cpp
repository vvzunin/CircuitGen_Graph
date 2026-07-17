#include <CircuitGenGraph/GraphUtils.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>

#include <gtest/gtest.h>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

using namespace CG_Graph;

TEST(GraphVertexBaseTests, MinWidthVertexPicksNarrowestInput) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *wide = graph->addInputBus("wide", 8);
  auto *narrow = graph->addInputBus("narrow", 2);
  auto *scalar = graph->addInput("bit");
  auto *gate = graph->addGateBus(Gates::GateOr, "orBus", 8);
  graph->addEdge(wide, gate);
  graph->addEdge(narrow, gate);
  graph->addEdge(scalar, gate);

  // Scalar is treated as width 1 — narrower than any bus here.
  EXPECT_EQ(gate->minWidthVertex(), scalar);

  GraphPtr onlyBuses = std::make_shared<OrientedGraph>();
  auto *b8 = onlyBuses->addInputBus("b8", 8);
  auto *b3 = onlyBuses->addInputBus("b3", 3);
  auto *b5 = onlyBuses->addInputBus("b5", 5);
  auto *andBus = onlyBuses->addGateBus(Gates::GateAnd, "andBus", 8);
  onlyBuses->addEdge(b8, andBus);
  onlyBuses->addEdge(b3, andBus);
  onlyBuses->addEdge(b5, andBus);
  EXPECT_EQ(andBus->minWidthVertex(), b3);

  auto *lonely = graph->addGate(Gates::GateNot);
  EXPECT_EQ(lonely->minWidthVertex(), nullptr);
}

TEST(GraphVertexBaseTests, simpleGetByLevel) {
  GraphPtr graph = std::make_shared<OrientedGraph>();

  // level 0
  VertexPtr vert = graph->addInput();
  std::vector<VertexPtr> stack;
  stack.reserve(5);
  for (int i = 0; i < 5; ++i) {
    VertexPtr another = graph->addGate(GateNot);
    // last level it level 5
    graph->addEdge(vert, another);
    stack.push_back(vert);
    vert = another;
  }
  // level 6
  VertexPtr last = graph->addOutput();
  graph->addEdge(vert, last);
  std::vector<VertexPtr> found;

  EXPECT_FALSE(stack.front()->getVerticesByLevel(0u, found, false));
  graph->updateLevels();

  EXPECT_TRUE(stack.front()->getVerticesByLevel(0u, found, false));
  ASSERT_EQ(found.size(), 1ul);
  EXPECT_EQ(found.back(), stack.front());

  EXPECT_TRUE(stack.front()->getVerticesByLevel(0u, found, false));
  EXPECT_EQ(found.size(), 1ul);

  EXPECT_TRUE(last->getVerticesByLevel(1u, found, true));
  EXPECT_EQ(found.back(), stack[1u]);

  stack.front()->resetUsedLevelState();
  EXPECT_TRUE(stack.front()->getVerticesByLevel(0u, found, false));
  EXPECT_EQ(found.back(), stack.front());

  graph->clearUsedLevelStates();
  EXPECT_TRUE(last->getVerticesByLevel(5u, found, true));
  EXPECT_EQ(found.back(), vert);
}