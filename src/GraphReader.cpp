#include "CircuitGenGraph/GraphUtils.hpp"
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <lorina/lorina.hpp>
#include <utility>
#define SUFFIX_INVERSION "_not"
#define SUFFIX_OUTPUT_GATE "_gate"
namespace CG_Graph {
GraphReader::GraphReader(Context &i_context) : d_context(i_context){};
void GraphReader::on_module_header(
    const std::string &module_name,
    const std::vector<std::string> &inouts) const {
  d_context.d_currentGraph = std::make_shared<OrientedGraph>(module_name);
  if (d_context.d_currentGraphNamesList.max_load_factor() != 1)
    d_context.d_currentGraphNamesList.max_load_factor(1);
}
void GraphReader::on_inputs(const std::vector<std::string> &inputs,
                            std::string const &size) const {
  d_context.d_currentGraph->reserve(input, inputs.size());
  if (d_context.d_numberOfVertices > 7 &&
      inputs.size() > d_context.d_numberOfVertices) {
    d_context.d_numberOfVertices += inputs.size();
    d_context.d_currentGraphNamesList.reserve(d_context.d_numberOfVertices);
  }
  for (auto i: inputs)
    d_context.d_currentGraphNamesList[i] =
        d_context.d_currentGraph->addInput(i);
}
void GraphReader::on_outputs(const std::vector<std::string> &outputs,
                             std::string const &size) const {
  d_context.d_currentGraph->reserve(output, outputs.size());
  d_context.d_currentGraph->reserve(gate, outputs.size());
  if (d_context.d_numberOfVertices > 7 &&
      outputs.size() * 2 > d_context.d_numberOfVertices) {
    d_context.d_numberOfVertices += outputs.size() * 2;
    d_context.d_currentGraphNamesList.reserve(d_context.d_numberOfVertices);
  }
  for (auto i: outputs) {
    VertexPtr outputVertex = d_context.d_currentGraphNamesList[i] =
        d_context.d_currentGraph->addOutput(i);
  }
}
void GraphReader::on_wires(const std::vector<std::string> &wires,
                           std::string const &size) const {
  d_context.d_currentGraph->reserve(gate, wires.size());
  d_context.d_numberOfVertices += wires.size();
  if (d_context.d_numberOfVertices > 7 &&
      wires.size() > d_context.d_numberOfVertices) {
    d_context.d_numberOfVertices += wires.size();
    d_context.d_currentGraphNamesList.reserve(d_context.d_numberOfVertices);
  }
  for (auto i: wires)
    d_context.d_currentGraphNamesList[i] =
        d_context.d_currentGraph->addGate(GateDefault, i);
}
void GraphReader::on_parameter(const std::string &name,
                               const std::string &value) const {
  VertexPtr vertex;
  if (value == "1'b1" || value == "1")
    vertex = d_context.d_currentGraph->addConst('1', name);
  else
    vertex = d_context.d_currentGraph->addConst('0', name);
  d_context.d_currentGraphNamesList[name] = vertex;
}
void GraphReader::on_assign(const std::string &lhs,
                            const std::pair<std::string, bool> &rhs) const {
  if (rhs.first == "1'b1" || rhs.first == "1'b0") {
    VertexPtr vertex;
    if (rhs.first == "1'b1" || rhs.first == "1")
      vertex = d_context.d_currentGraph->addConst('1', lhs);
    else
      vertex = d_context.d_currentGraph->addConst('0', lhs);
    d_context.d_currentGraphNamesList[lhs] = vertex;
  } else {
    if (d_context.d_currentGraphNamesList[lhs]->getType() != output) {
      VertexPtr leftVertex = d_context.d_currentGraphNamesList[lhs];
      if (!rhs.second) {
        static_cast<GraphVertexGates *>(leftVertex)->setGateType(GateBuf);
        d_context.d_currentGraph->addEdge(get_operand(rhs.first, rhs.second),
                                          leftVertex);
      } else {
        static_cast<GraphVertexGates *>(leftVertex)->setGateType(GateNot);
        d_context.d_currentGraphNamesList[rhs.first + SUFFIX_INVERSION] =
            leftVertex;
        d_context.d_currentGraph->addEdge(get_operand(rhs.first), leftVertex);
      }
    } else {
      VertexPtr outputGate = get_operand(rhs.first, rhs.second);
      if (d_context.d_currentGraphNamesList.find(lhs + SUFFIX_OUTPUT_GATE) ==
          d_context.d_currentGraphNamesList.end()) {
        d_context.d_currentGraph->addEdge(
            outputGate, d_context.d_currentGraphNamesList[lhs]);
        d_context.d_currentGraphNamesList[lhs + SUFFIX_OUTPUT_GATE] =
            outputGate;
      } else {
        for (VertexPtr vertex:
             d_context.d_currentGraphNamesList[lhs + SUFFIX_OUTPUT_GATE]
                 ->getOutConnections()) {
          d_context.d_currentGraph->addEdge(outputGate, vertex);
        }
        d_context.d_currentGraph->updateEdgesGatesCount(
            d_context.d_currentGraphNamesList[lhs + SUFFIX_OUTPUT_GATE],
            GateDefault);
        d_context.d_currentGraphNamesList[lhs + SUFFIX_OUTPUT_GATE]
            ->~GraphVertexBase();
        d_context.d_currentGraphNamesList[lhs + SUFFIX_OUTPUT_GATE] =
            outputGate;
      }
    }
  }
};

VertexPtr GraphReader::get_operand(const std::string &i_name,
                                   bool i_isInverted) const {
  auto temp = d_context.d_currentGraphNamesList.find(i_name);
  VertexPtr res;
  if (temp->second->getType() == output) {
    if (d_context.d_currentGraphNamesList.find(i_name + SUFFIX_OUTPUT_GATE) ==
        d_context.d_currentGraphNamesList.end()) {
      res = d_context.d_currentGraph->addGate(GateDefault,
                                              i_name + SUFFIX_OUTPUT_GATE);
      d_context.d_currentGraph->addEdge(res, temp->second);
      d_context.d_currentGraphNamesList[i_name + SUFFIX_OUTPUT_GATE] = res;
    } else
      res = d_context.d_currentGraphNamesList[i_name + SUFFIX_OUTPUT_GATE];
  } else
    res = d_context.d_currentGraphNamesList[i_name];
  if (i_isInverted) {
    auto searchForNot =
        d_context.d_currentGraphNamesList.find(i_name + SUFFIX_INVERSION);
    VertexPtr notAns;
    if (searchForNot == d_context.d_currentGraphNamesList.end()) {
      notAns =
          d_context.d_currentGraph->addGate(GateNot, i_name + SUFFIX_INVERSION);
      d_context.d_currentGraphNamesList[i_name + SUFFIX_INVERSION] = notAns;
      d_context.d_currentGraph->addEdge(res, notAns);
    } else
      notAns = searchForNot->second;
    return notAns;
  }
  return res;
}

void GraphReader::on_elem(const std::string &i_lhs,
                          const std::pair<std::string, bool> &i_op1,
                          const std::pair<std::string, bool> &i_op2,
                          Gates i_gateType) const {
  VertexPtr vertexGate = get_operand(i_lhs);
  if (vertexGate->getType() == gate && vertexGate->getGate() == GateDefault)
    static_cast<GraphVertexGates *>(vertexGate)->setGateType(i_gateType);
  d_context.d_currentGraph->addEdge(get_operand(i_op1.first, i_op1.second),
                                    vertexGate);
  d_context.d_currentGraph->addEdge(get_operand(i_op2.first, i_op2.second),
                                    vertexGate);
}

void GraphReader::on_elem3(const std::string &i_lhs,
                           const std::pair<std::string, bool> &i_op1,
                           const std::pair<std::string, bool> &i_op2,
                           const std::pair<std::string, bool> &i_op3,
                           Gates i_gateType) const {
  on_elem(i_lhs, i_op1, i_op2, i_gateType);
  VertexPtr vertexGate = get_operand(i_lhs);
  d_context.d_currentGraph->addEdge(get_operand(i_op3.first, i_op3.second),
                                    vertexGate);
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
  on_elem(lhs, op1, op2, GateXor);
}

void GraphReader::on_xnor(const std::string &lhs,
                          const std::pair<std::string, bool> &op1,
                          const std::pair<std::string, bool> &op2) const {
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
      d_context.d_currentGraph->generateMajority(
          get_operand(op1.first, op1.second),
          get_operand(op2.first, op2.second),
          get_operand(op3.first, op3.second));
  d_context.d_currentGraphNamesList[lhs]->setName(lhs);
}

void GraphReader::on_endmodule() const {
  d_context.d_graphs[d_context.d_currentGraph.get()->getName()] =
      d_context.d_currentGraph;
  d_context.d_currentGraphNamesList.clear();
}
} // namespace CG_Graph