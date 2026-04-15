#include "CircuitGenGraph/GraphUtils.hpp"
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <cstddef>
#include <lorina/lorina.hpp>
#include <regex>
#include <string>
#include <utility>
#include <cassert>

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
  d_context.d_numberOfVertices += inputs.size();
  d_context.d_currentGraphNamesList.reserve(d_context.d_numberOfVertices);

  for (auto i: inputs)
    d_context.d_currentGraphNamesList[i] =
        d_context.d_currentGraph->addInput(i);
}

void GraphReader::on_outputs(const std::vector<std::string> &outputs,
                             std::string const &size) const {
  d_context.d_currentGraph->reserve(output, outputs.size());
  d_context.d_currentGraph->reserve(gate, outputs.size());
  d_context.d_numberOfVertices += outputs.size() * 2;
  d_context.d_currentGraphNamesList.reserve(d_context.d_numberOfVertices);

  for (auto i: outputs) {
    d_context.d_currentGraphNamesList[i] =
        d_context.d_currentGraph->addOutput(i);
  }
}

void GraphReader::on_wires(const std::vector<std::string> &wires,
                           std::string const &size) const {
  d_context.d_currentGraph->reserve(gate, wires.size());
  d_context.d_numberOfVertices += wires.size();
  d_context.d_currentGraphNamesList.reserve(d_context.d_numberOfVertices);
  for (auto i: wires)
    d_context.d_currentGraphNamesList[i] =
        d_context.d_currentGraph->addGate(GateDefault, i);
}

std::pair<size_t, std::string> parseConstValue(const std::string &input) {
  // It is a mock, will be reworked

  size_t length;
  std::string value;

  if (input.find("\'") < input.length()) {
    length = std::stoi(input.substr(0, input.find("\'")));
    value = input.substr(input.find("\'") + 2,
                         input.length() - input.find("\'") - 2);
  }

  else {
    length = input.length();
    value = input;
  }

  return std::make_pair(length, value);
}

void GraphReader::on_parameter(const std::string &name,
                               const std::string &value) const {
  VertexPtr vertex;
  std::pair<size_t, std::string> data = parseConstValue(value);
  vertex = d_context.d_currentGraph->addConst(data.second[0], name);
  d_context.d_currentGraphNamesList[name] = vertex;
}

void GraphReader::on_assign(const std::string &lhs,
                            const std::pair<std::string, bool> &rhs) const {
  std::regex isConstant("^([0-9]+)'[bhdo]([0-9A-Fa-f]+)$"),
      isSimpleConstant("^[0-9]+$");
  if (std::regex_match(rhs.first, isConstant) ||
      std::regex_match(rhs.first, isSimpleConstant)) {
    on_parameter(lhs, rhs.first);
  } else {
    if (d_context.d_currentGraphNamesList[lhs]->getType() != output) {
      VertexPtr leftVertex = d_context.d_currentGraphNamesList[lhs];
      if (!rhs.second) {
        static_cast<GraphVertexGates *>(leftVertex)->setGateIfDefault(GateBuf);
        d_context.d_currentGraph->addEdge(getRightHS(rhs.first), leftVertex);
      } else {
        static_cast<GraphVertexGates *>(leftVertex)->setGateIfDefault(GateNot);
        if (d_context.d_currentGraphNamesList.find(rhs.first +
                                                   SUFFIX_INVERSION) ==
            d_context.d_currentGraphNamesList.end())
          d_context.d_currentGraphNamesList[rhs.first + SUFFIX_INVERSION] =
              leftVertex;
        d_context.d_currentGraphNamesList[lhs] = leftVertex;
        d_context.d_currentGraph->addEdge(getRightHS(rhs.first), leftVertex);
      }
    } else {
      d_context.d_currentGraph->addEdge(getRightHS(rhs.first, rhs.second),
                                        get_vertex_by_name(lhs));
    }
  }
};

VertexPtr GraphReader::getRightHS(const std::string &i_name,
                                  bool i_isInverted) const {
  return get_vertex_by_name(i_name, i_isInverted);
}

VertexPtr GraphReader::getLeftHS(const std::string &i_name,
                                 bool i_isInverted) const {
  auto temp = d_context.d_currentGraphNamesList.find(i_name);
  // create assert for calling for each vertex ONCE incorrect verilog otherwise
  if (temp->second->getType() == output) {
    if (d_context.d_currentGraphNamesList.find(i_name + SUFFIX_OUTPUT_GATE) ==
        d_context.d_currentGraphNamesList.end()) {
      VertexPtr outputGate = d_context.d_currentGraph->addGate(
          GateDefault, i_name + SUFFIX_OUTPUT_GATE);
      d_context.d_currentGraph->addEdge(outputGate, temp->second);
      d_context.d_currentGraphNamesList[i_name + SUFFIX_OUTPUT_GATE] =
          outputGate;
    }
    return get_vertex_by_name(i_name + SUFFIX_OUTPUT_GATE, i_isInverted);
  }
  return get_vertex_by_name(i_name, i_isInverted);
}

VertexPtr GraphReader::get_vertex_by_name(const std::string &i_name,
                                          bool i_isInverted) const {
  VertexPtr res = d_context.d_currentGraphNamesList[i_name];
  if (i_isInverted) {
    return get_or_create_inversion(i_name, res);
  }
  return res;
}

VertexPtr GraphReader::get_or_create_inversion(const std::string &i_name,
                                               VertexPtr i_vertex) const {
  auto searchForNot =
      d_context.d_currentGraphNamesList.find(i_name + SUFFIX_INVERSION);
  VertexPtr notAns;
  if (searchForNot == d_context.d_currentGraphNamesList.end()) {
    notAns =
        d_context.d_currentGraph->addGate(GateNot, i_name + SUFFIX_INVERSION);
    d_context.d_currentGraphNamesList[i_name + SUFFIX_INVERSION] = notAns;
    d_context.d_currentGraph->addEdge(i_vertex, notAns);
    return notAns;
  }
  return searchForNot->second;
}

void GraphReader::on_elem(const std::string &i_lhs,
                          const std::pair<std::string, bool> &i_op1,
                          const std::pair<std::string, bool> &i_op2,
                          Gates i_gateType) const {
  VertexPtr vertexGate = getLeftHS(i_lhs);
  if (vertexGate->getType() == gate && vertexGate->getGate() == GateDefault) {
    static_cast<GraphVertexGates *>(vertexGate)->setGateIfDefault(i_gateType);
  }
  d_context.d_currentGraph->addEdge(getRightHS(i_op1.first, i_op1.second),
                                    vertexGate);
  d_context.d_currentGraph->addEdge(getRightHS(i_op2.first, i_op2.second),
                                    vertexGate);
}

void GraphReader::on_elem3(const std::string &i_lhs,
                           const std::pair<std::string, bool> &i_op1,
                           const std::pair<std::string, bool> &i_op2,
                           const std::pair<std::string, bool> &i_op3,
                           Gates i_gateType) const {
  on_elem(i_lhs, i_op1, i_op2, i_gateType);
  VertexPtr vertexGate = getLeftHS(i_lhs);
  d_context.d_currentGraph->addEdge(getRightHS(i_op3.first, i_op3.second),
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
    d_context.d_currentGraph->majorityAsLogic(
        getRightHS(op1.first, op1.second), getRightHS(op2.first, op2.second),
        getRightHS(op3.first, op3.second), temp->second);
  } else {
    d_context.d_currentGraphNamesList[lhs] =
        d_context.d_currentGraph->generateMajority(
            getRightHS(op1.first, op1.second),
            getRightHS(op2.first, op2.second),
            getRightHS(op3.first, op3.second));
    d_context.d_currentGraphNamesList[lhs]->setName(lhs);
  }
}

void GraphReader::on_endmodule() const {
  d_context.d_graphs[d_context.d_currentGraph.get()->getName()] =
      d_context.d_currentGraph;
  d_context.d_currentGraphNamesList.clear();
}
} // namespace CG_Graph