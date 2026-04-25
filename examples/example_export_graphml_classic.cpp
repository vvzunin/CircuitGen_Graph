/**
 * @file example_export_graphml_classic.cpp
 * @brief Экспорт графа в классический GraphML (строка в stdout).
 */
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <iostream>

auto main() -> int {
  using namespace CG_Graph;
  GraphPtr g = std::make_shared<OrientedGraph>("demo_gml");
  VertexPtr x = g->addInput("x");
  VertexPtr n = g->addGate(Gates::GateNot, "nx");
  VertexPtr z = g->addOutput("z");
  g->addEdge(x, n);
  g->addEdge(n, z);
  g->updateLevels();
  std::cout << g->toGraphMLClassic(0);
  return 0;
}
