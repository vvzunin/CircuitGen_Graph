/**
 * @file example_export_dot.cpp
 * @brief Экспорт графа в DOT (через dotReturnToString).
 */
#include <CircuitGenGraph/DefaultAuxiliaryMethods.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <iostream>

auto main() -> int {
  using namespace CG_Graph;
  GraphPtr g = std::make_shared<OrientedGraph>("demo_dot");
  VertexPtr i = g->addInput("in");
  VertexPtr b = g->addGate(Gates::GateBuf, "buf");
  VertexPtr o = g->addOutput("out");
  g->addEdge(i, b);
  g->addEdge(b, o);
  g->updateLevels();
  std::cout << AuxMethodsGraph::dotReturnToString(g->toDOT());
  return 0;
}
