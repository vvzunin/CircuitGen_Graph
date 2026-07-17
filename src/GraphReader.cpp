#include <CircuitGenGraph/GraphReader.hpp>

#include "CircuitGenGraph/GraphUtils.hpp"
#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <CircuitGenGraph/Logging.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <regex>
#include <stdexcept>
#include <string>
#include <utility>

#include <lorina/lorina.hpp>

#define SUFFIX_INVERSION "_not"
#define SUFFIX_OUTPUT_GATE "_gate"

namespace CG_Graph {

// clang-format off

GraphReader::GraphReader(Context &i_context) 
    : d_context(i_context)
{};

// clang-format on

void LogDiagnosticConsumer::handle_diagnostic(
    lorina::diagnostic_level level, const std::string &message) const {
  switch (level) {
    case lorina::diagnostic_level::fatal:
      CG_LOG_FATAL << "Lorina: " << message;
      break;
    case lorina::diagnostic_level::error:
      CG_LOG_ERROR << "Lorina: " << message;
      break;
    case lorina::diagnostic_level::warning:
      CG_LOG_WARNING << "Lorina: " << message;
      break;
    case lorina::diagnostic_level::note:
    case lorina::diagnostic_level::remark:
    case lorina::diagnostic_level::ignore:
    default:
      CG_VLOG(1) << "Lorina: " << message;
      break;
  }
}

void GraphReader::on_module_header(
    const std::string &module_name,
    const std::vector<std::string> &inouts) const {
  CG_LOG_INFO << "Parsing module '" << module_name << "' with " << inouts.size()
              << " ports";
  d_context.d_currentGraph = std::make_shared<OrientedGraph>(module_name);
  if (d_context.d_currentGraphNamesList.max_load_factor() != 1)
    d_context.d_currentGraphNamesList.max_load_factor(1);
}

void GraphReader::on_inputs(const std::vector<std::string> &inputs,
                            std::string const &size) const {
  CG_VLOG(1) << "Adding " << inputs.size() << " inputs to graph";
  d_context.d_currentGraph->reserve(input, inputs.size());
  d_context.d_numberOfVertices += inputs.size();
  d_context.d_currentGraphNamesList.reserve(d_context.d_numberOfVertices);

  for (auto i: inputs)
    d_context.d_currentGraphNamesList[i] =
        d_context.d_currentGraph->addInput(i);
}

void GraphReader::on_outputs(const std::vector<std::string> &outputs,
                             std::string const &size) const {
  CG_VLOG(1) << "Adding " << outputs.size() << " outputs to graph";
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
  CG_VLOG(1) << "Adding " << wires.size() << " wires as internal gates";
  d_context.d_currentGraph->reserve(gate, wires.size());
  d_context.d_numberOfVertices += wires.size();
  d_context.d_currentGraphNamesList.reserve(d_context.d_numberOfVertices);
  for (auto i: wires)
    d_context.d_currentGraphNamesList[i] =
        d_context.d_currentGraph->addGate(GateDefault, i);
}

std::pair<size_t, std::string> parseConstValue(const std::string &input) {
  // Returns (bitwidth, canonical 0/1/x digit string for simulation).
  // Multi-bit literals are collapsed to a single scalar char for the current
  // GraphVertexConstant model (one char); prefer LSB of the numeric value.
  size_t length = 1;
  std::string digits;

  const auto q = input.find('\'');
  if (q != std::string::npos && q + 1 < input.size()) {
    try {
      length = static_cast<size_t>(std::stoul(input.substr(0, q)));
    } catch (...) {
      length = 1;
    }
    if (length == 0)
      length = 1;
    const char baseCh = input[q + 1];
    const std::string body = input.substr(q + 2);
    unsigned long long num = 0;
    int base = 2;
    switch (baseCh) {
      case 'h':
      case 'H':
        base = 16;
        break;
      case 'd':
      case 'D':
        base = 10;
        break;
      case 'o':
      case 'O':
        base = 8;
        break;
      case 'b':
      case 'B':
      default:
        base = 2;
        break;
    }
    try {
      if (!body.empty())
        num = std::stoull(body, nullptr, base);
    } catch (...) {
      num = 0;
    }
    digits.assign(1, (num & 1ull) ? '1' : '0');
  } else if (!input.empty() &&
             std::all_of(input.begin(), input.end(), ::isdigit)) {
    length = input.size();
    const char last = input.back();
    digits.assign(1, (last == '0' || last == '1') ? last
                     : ((last - '0') & 1)         ? '1'
                                                  : '0');
  } else if (!input.empty()) {
    length = 1;
    const char c = input.front();
    digits.assign(1, (c == '0' || c == '1') ? c : 'x');
  } else {
    digits.assign(1, 'x');
  }

  return std::make_pair(length, digits);
}

void GraphReader::on_parameter(const std::string &name,
                               const std::string &value) const {
  std::pair<size_t, std::string> data = parseConstValue(value);
  const char bit = data.second.empty() ? 'x' : data.second.front();
  VertexPtr vertex = d_context.d_currentGraph->addConst(bit, name);
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
    VertexPtr leftVertex = get_vertex_by_name(lhs);
    if (leftVertex->getType() != output) {
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
                                        leftVertex);
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
  if (temp == d_context.d_currentGraphNamesList.end() || !temp->second) {
    throw std::runtime_error("GraphReader: unknown net on LHS: " + i_name);
  }
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
  auto it = d_context.d_currentGraphNamesList.find(i_name);
  if (it == d_context.d_currentGraphNamesList.end() || !it->second) {
    throw std::runtime_error("GraphReader: unknown net: " + i_name);
  }
  VertexPtr res = it->second;
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