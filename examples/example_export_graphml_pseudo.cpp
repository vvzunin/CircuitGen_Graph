/**
 * @file example_export_graphml_pseudo.cpp
 * @brief Экспорт в GraphML PseudoABCD (фрагмент в stdout).
 */
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <iostream>
#include <string>

auto main() -> int {
  using namespace CG_Graph;
  GraphPtr g = std::make_shared<OrientedGraph>("demo_abcd");
  VertexPtr clk = g->addInput("clk");
  VertexPtr d = g->addInput("d");
  VertexPtr q = g->addSequential(ff, clk, d, "q");
  VertexPtr z = g->addOutput("z");
  g->addEdge(q, z);
  g->updateLevels();
  const std::string xml = g->toGraphMLPseudoABCD();
  const std::size_t n = std::min<std::size_t>(600, xml.size());
  std::cout.write(xml.data(), static_cast<std::streamsize>(n));
  if (xml.size() > n) {
    std::cout << "\n... (truncated)\n";
  }
  return 0;
}
