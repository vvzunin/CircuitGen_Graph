#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>

#include <gtest/gtest.h>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

using namespace CG_Graph;

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