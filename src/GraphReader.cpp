#include <CircuitGenGraph/OrientedGraph.hpp>
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <lorina/lorina.hpp>
namespace CG_Graph {
GraphReader::GraphReader(Context &i_context) : d_context(i_context){};
void GraphReader::on_module_header(
    const std::string &module_name,
    const std::vector<std::string> &inouts) const {
  d_context.d_currentGraph = std::make_shared<OrientedGraph>(module_name);
}
void GraphReader::on_inputs(const std::vector<std::string> &inputs,
                            std::string const &size) const {
  for (auto i: inputs)
    d_context.d_currentGraphNamesList[i] =
        d_context.d_currentGraph->addInput(i);
}
void GraphReader::on_outputs(const std::vector<std::string> &outputs,
                             std::string const &size) const {
  for (auto i: outputs)
    d_context.d_currentGraphNamesList[i] =
        d_context.d_currentGraph->addOutput(i);
}

void GraphReader::on_parameter(const std::string &name,
                               const std::string &value) const {
}

void GraphReader::on_assign(const std::string &lhs,
                            const std::pair<std::string, bool> &rhs) const {
  std::cout << lhs << "\n" << rhs.first << rhs.second << "\n";
}

VertexPtr GraphReader::find_operand(const std::string &name) const {
  auto temp = d_context.d_currentGraphNamesList.find(name);
  VertexPtr ans;
  if (temp == d_context.d_currentGraphNamesList.end()) {
    ans = d_context.d_currentGraph->addGate(GateDefault, name);
    d_context.d_currentGraphNamesList[name] = ans;
  } else
    ans = d_context.d_currentGraphNamesList[name];
  return ans;
}

void GraphReader::on_elem(const std::string &lhs,
                          const std::pair<std::string, bool> &op1,
                          const std::pair<std::string, bool> &op2,
                          Gates gateType) const {
  VertexPtr vertexGate = find_operand(lhs);
  if (vertexGate->getType() == gate && vertexGate->getGate() == GateDefault)
    static_cast<GraphVertexGates *>(vertexGate)->setGateType(gateType);
  if (vertexGate->getType() == output) {
    VertexPtr logicVertex = d_context.d_currentGraph->addGate(gateType);
    d_context.d_currentGraph->addEdge(find_operand(op1.first), logicVertex);
    d_context.d_currentGraph->addEdge(find_operand(op2.first), logicVertex);
    d_context.d_currentGraph->addEdge(logicVertex, vertexGate);
  } else {
    d_context.d_currentGraph->addEdge(find_operand(op1.first), vertexGate);
    d_context.d_currentGraph->addEdge(find_operand(op2.first), vertexGate);
  }
}

void GraphReader::on_elem3(const std::string &lhs,
                           const std::pair<std::string, bool> &op1,
                           const std::pair<std::string, bool> &op2,
                           const std::pair<std::string, bool> &op3,
                           Gates gateType) const {
  on_elem(lhs, op1, op2, gateType);
  VertexPtr vertexGate = d_context.d_currentGraphNamesList[lhs];
  d_context.d_currentGraph->addEdge(find_operand(op3.first), vertexGate);
}

void GraphReader::on_and(const std::string &lhs,
                         const std::pair<std::string, bool> &op1,
                         const std::pair<std::string, bool> &op2) const {
  on_elem(lhs, op1, op2, GateAnd);
}
void GraphReader::on_nand(const std::string &lhs,
                          const std::pair<std::string, bool> &op1,
                          const std::pair<std::string, bool> &op2) const {
  on_elem(lhs, op1, op2, GateNand);
}

void GraphReader::on_or(const std::string &lhs,
                        const std::pair<std::string, bool> &op1,
                        const std::pair<std::string, bool> &op2) const {
  on_elem(lhs, op1, op2, GateOr);
}

void GraphReader::on_nor(const std::string &lhs,
                         const std::pair<std::string, bool> &op1,
                         const std::pair<std::string, bool> &op2) const {
  on_elem(lhs, op1, op2, GateNor);
}

void GraphReader::on_xor(const std::string &lhs,
                         const std::pair<std::string, bool> &op1,
                         const std::pair<std::string, bool> &op2) const {
  VertexPtr vGate = d_context.d_currentGraph->addGate(Gates::GateXor, lhs);
  on_elem(lhs, op1, op2, GateXor);
}

void GraphReader::on_xnor(const std::string &lhs,
                          const std::pair<std::string, bool> &op1,
                          const std::pair<std::string, bool> &op2) const {
  VertexPtr vGate = d_context.d_currentGraph->addGate(Gates::GateXnor, lhs);
  on_elem(lhs, op1, op2, GateXnor);
}

void GraphReader::on_and3(const std::string &lhs,
                          const std::pair<std::string, bool> &op1,
                          const std::pair<std::string, bool> &op2,
                          const std::pair<std::string, bool> &op3) const {
  on_elem3(lhs, op1, op2, op3, GateAnd);
}

void GraphReader::on_or3(const std::string &lhs,
                         const std::pair<std::string, bool> &op1,
                         const std::pair<std::string, bool> &op2,
                         const std::pair<std::string, bool> &op3) const {
  on_elem3(lhs, op1, op2, op3, GateOr);
}

void GraphReader::on_xor3(const std::string &lhs,
                          const std::pair<std::string, bool> &op1,
                          const std::pair<std::string, bool> &op2,
                          const std::pair<std::string, bool> &op3) const {
  on_elem3(lhs, op1, op2, op3, GateXor);
}

void GraphReader::on_maj3(const std::string &lhs,
                          const std::pair<std::string, bool> &op1,
                          const std::pair<std::string, bool> &op2,
                          const std::pair<std::string, bool> &op3) const {
  auto temp = d_context.d_currentGraphNamesList.find(lhs);
  if (temp != d_context.d_currentGraphNamesList.end()) {
    d_context.d_currentGraphNamesList[lhs]->~GraphVertexBase();
  }
  d_context.d_currentGraphNamesList[lhs] =
      d_context.d_currentGraph->generateMajority(find_operand(op1.first),
                                                 find_operand(op2.first),
                                                 find_operand(op3.first));
  d_context.d_currentGraphNamesList[lhs]->setName(lhs);
}

void GraphReader::on_endmodule() const {
  d_context.d_graphs[d_context.d_currentGraph.get()->getName()] =
      d_context.d_currentGraph;
  d_context.d_currentGraphNamesList.clear();
}
} // namespace CG_Graph