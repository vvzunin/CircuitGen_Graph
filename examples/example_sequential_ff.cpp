/**
 * @file example_sequential_ff.cpp
 * @brief D-триггер: clk, data, выход q.
 */
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <iostream>

auto main() -> int {
  using namespace CG_Graph;
  GraphPtr g = std::make_shared<OrientedGraph>("demo_ff");
  VertexPtr clk = g->addInput("clk");
  VertexPtr data = g->addInput("data");
  VertexPtr q = g->addSequential(ff, clk, data, "q");
  VertexPtr out = g->addOutput("out");
  g->addEdge(q, out);
  g->updateLevels();
  std::cout << "Sequential graph levels_ok hash=" << g->calculateHash() << '\n';
  return 0;
}
