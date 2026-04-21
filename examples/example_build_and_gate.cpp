/**
 * @file example_build_and_gate.cpp
 * @brief Минимальный граф: два входа, вентиль AND, выход.
 */
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <iostream>

auto main() -> int {
  using namespace CG_Graph;
  GraphPtr g = std::make_shared<OrientedGraph>("demo_and");
  VertexPtr a = g->addInput("a");
  VertexPtr b = g->addInput("b");
  VertexPtr y = g->addGate(Gates::GateAnd, "y");
  VertexPtr o = g->addOutput("o");
  g->addEdges({a, b}, y);
  g->addEdge(y, o);
  g->updateLevels();
  std::cout << "Graph '" << g->getName() << "' hash=" << g->calculateHash()
            << " maxLevel=" << g->getMaxLevel() << '\n';
  return 0;
}
