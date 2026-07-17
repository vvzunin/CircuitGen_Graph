/**
 * @file GraphVertexGates.cpp
 * @brief Реализация вершины-логического элемента (гейт) графа.
 */
#include "CircuitGenGraph/GraphUtils.hpp"
#include "CircuitGenGraph/GraphVertexBus.hpp"
#include <cstddef>
#include <functional>
#include <iostream>

#include <CircuitGenGraph/GraphVertex.hpp>

#include <sstream>
#include <string>
#include <string_view>

#include <CircuitGenGraph/Logging.hpp>

#include "fmt/core.h"

namespace CG_Graph {

GraphVertexGates::GraphVertexGates(Gates i_gate, GraphPtr i_baseGraph,
                                   bool i_isBus) :
    GraphVertexBase(i_isBus ? VertexTypes::gateBus : gate, i_baseGraph) {
  d_gate = i_gate;
}
GraphVertexGates::GraphVertexGates(Gates i_gate, std::string_view i_name,
                                   GraphPtr i_baseGraph, bool i_isBus) :
    GraphVertexBase(i_isBus ? VertexTypes::gateBus : gate, i_name,
                    i_baseGraph) {
  d_gate = i_gate;
}

Gates GraphVertexGates::getGate() const {
  return d_gate;
}
void GraphVertexGates::setGateIfDefault(Gates type) {
  assert(d_gate == GateDefault);
  getBaseGraph().lock()->updateEdgesGatesCount(this, type);
  d_gate = type;
}
uint32_t GraphVertexGates::addVertexToInConnections(VertexPtr i_vert) {
  if ((getGate() == Gates::GateBuf || getGate() == Gates::GateNot) &&
      !d_inConnections.empty()) {
    throw std::length_error("Buf and Not gate types can not obtain more than "
                            "one input connection.");
  }
  return GraphVertexBase::addVertexToInConnections(i_vert);
}

char GraphVertexGates::updateValue() {
  d_value = ValueStates::NoSignal;
  if (d_inConnections.empty())
    return d_value;

  VertexPtr src = d_inConnections.front();
  // Multi-output subgraph: each instance GateBuf must read its own nested
  // output. SubGraph::getValue() is only the first output.
  if (d_gate == Gates::GateBuf && src->getType() == VertexTypes::subGraph) {
    if (src->getValue() == ValueStates::UndefinedState)
      src->updateValue();
    auto *sub = static_cast<GraphVertexSubGraph *>(src);
    d_value = sub->bufferedOutputValue(this);
    return d_value;
  }

  if (src->getValue() == ValueStates::UndefinedState)
    src->updateValue();
  d_value = src->getValue();

  // Unary gates: lookup without copying the truth-table map.
  if (d_gate == Gates::GateNot) {
    d_value = tableNot.at(d_value);
    return d_value;
  }
  if (d_gate == Gates::GateBuf) {
    d_value = tableBuf.at(d_value);
    return d_value;
  }

  for (size_t i = 1; i < d_inConnections.size(); ++i) {
    if (d_inConnections.at(i)->getValue() == ValueStates::UndefinedState)
      d_inConnections.at(i)->updateValue();
    const char rhs = d_inConnections.at(i)->getValue();
    // Nested .at() avoids copying the inner std::map row each step.
    switch (d_gate) {
      case Gates::GateAnd:
        d_value = tableAnd.at(d_value).at(rhs);
        break;
      case Gates::GateNand:
        d_value = tableNand.at(d_value).at(rhs);
        break;
      case Gates::GateOr:
        d_value = tableOr.at(d_value).at(rhs);
        break;
      case Gates::GateNor:
        d_value = tableNor.at(d_value).at(rhs);
        break;
      case Gates::GateXor:
        d_value = tableXor.at(d_value).at(rhs);
        break;
      case Gates::GateXnor:
        d_value = tableXnor.at(d_value).at(rhs);
        break;
      default:
        CG_LOG_ERROR << "GraphVertexGates: Unknown gate type in updateValue "
                        "for vertex '"
                     << d_name << "'";
        break;
    }
  }
  return d_value;
}

void GraphVertexGates::removeValue() {
  d_value = ValueStates::UndefinedState;
  if (d_inConnections.size() > 0) {
    for (VertexPtr ptr: d_inConnections) {
      if (ptr->getValue() != ValueStates::UndefinedState) {
        ptr->removeValue();
      }
    }
  }
}

size_t GraphVertexGates::calculateHash() {
  if (d_hasHash) {
    return d_hashed;
  }
  d_hasHash = HC_IN_PROGRESS;
  std::vector<size_t> hashed_data;
  hashed_data.reserve(d_inConnections.size());

  for (auto &child: d_inConnections) {
    hashed_data.push_back(child->calculateHash());
  }
  std::sort(hashed_data.begin(), hashed_data.end());

  size_t h = 0;
  hashCombine(h, static_cast<size_t>(getType()));
  hashCombine(h, static_cast<size_t>(d_gate));
  hashCombine(h, d_outConnections.size());
  for (const auto &sub: hashed_data) {
    hashCombine(h, sub);
  }
  d_hashed = h;
  d_hasHash = HC_CALC;

  return d_hashed;
}

std::string GraphVertexGates::getVerilogString() const {
  std::string s;

  auto appendDriverName = [this](std::string &out, VertexPtr ptr) {
    if (this->d_baseGraph.lock() == ptr->getBaseGraph().lock()) {
      out.append(ptr->getRawName());
    } else {
      out.append(ptr->getBaseGraph().lock()->getRawName());
      out.push_back('_');
      out.append(ptr->getRawName());
    }
  };

  if (d_inConnections.size() > 0) {
    VertexPtr ptr = d_inConnections.at(0);
    if (!ptr) {
      throw std::invalid_argument(
          "Cannot use nullptr for printing it to verilog");
    }

    appendDriverName(s, ptr);

    if (d_gate == Gates::GateNot)
      s.insert(s.begin(), '~');
    if ((d_gate == Gates::GateNand) || (d_gate == Gates::GateNor) ||
        (d_gate == Gates::GateXnor))
      s = "~(" + s;

    for (size_t i = 1; i < d_inConnections.size(); i++) {
      ptr = d_inConnections.at(i);
      if (!ptr) {
        throw std::invalid_argument(
            "Cannot use nullptr for printing it to verilog");
      }

      s.push_back(' ');
      s += VertexUtils::gateToString(d_gate);
      s.push_back(' ');
      appendDriverName(s, ptr);
      if (d_gate == GateDefault)
        CG_LOG_ERROR << "GraphVertexGates: Default gate used in "
                        "getVerilogString for vertex '"
                     << d_name << "'";
    }

    if ((d_gate == Gates::GateNand) || (d_gate == Gates::GateNor) ||
        (d_gate == Gates::GateXnor))
      s += ")";
  }

  return s;
}

std::string GraphVertexGates::toVerilog() const {
  if (d_inConnections.empty()) {
    CG_LOG_WARNING << "GraphVertexGates: empty vertex '" << d_name
                   << "' skipped in Verilog";
    return "";
  }
  std::string end;
  std::string oper = VertexUtils::gateToString(d_gate);
  auto printUnaryOperators = [&]() {
    return fmt::format("assign {} = {}{};", getRawName(), oper,
                       d_inConnections.back()->getRawName());
  };
  auto printBinaryOperators = [&]() {
    std::string basic = fmt::format("assign {} = ", getRawName());
    if (d_gate == Gates::GateNand || d_gate == Gates::GateNor ||
        d_gate == Gates::GateXnor) {
      basic += "~ ( ";
      end = " )";
    } else if (d_gate == Gates::GateConcatenation) {
      basic += "{ ";
      end = " }";
    }
    for (size_t i = 0; i < d_inConnections.size() - 1; ++i) {
      basic.append(d_inConnections.at(i)->getRawName());
      basic.push_back(' ');
      basic += oper;
      basic.push_back(' ');
    }
    basic.append(d_inConnections.back()->getRawName());
    basic += end;
    basic.push_back(';');

    return basic;
  };
  return toVerilogCommon(printBinaryOperators, printUnaryOperators);
}

DotReturn GraphVertexGates::toDOT() {
  if (d_inConnections.empty()) {
    CG_LOG_WARNING << "GraphVertexGates: empty vertex '" << d_name
                   << "' skipped in DOT";
    return {};
  }

  DotReturn dot;

  dot.push_back({DotTypes::DotGate,
                 {{"name", getName()},
                  {"label", getName()},
                  {"level", std::to_string(d_level)}}});

  for (VertexPtr ptr: d_inConnections) {
    dot.push_back(
        {DotTypes::DotEdge, {{"from", ptr->getName()}, {"to", getName()}}});
  }
  return dot;
}

bool GraphVertexGates::isSubgraphBuffer() const {
  if (d_gate != Gates::GateBuf || d_inConnections.empty()) {
    return false;
  }
  return d_inConnections.front()->getType() == VertexTypes::subGraph;
}
std::string GraphVertexGates::toVerilogCommon(
    std::function<std::string()> printBinaryOperators,
    std::function<std::string()> printUnaryOperators) const {
  if (d_inConnections.empty()) {
    CG_LOG_WARNING << "GraphVertexGates: empty vertex '" << d_name
                   << "' skipped in Verilog (call removeEmptyLogicVertices)";
    return "";
  }
  if (d_gate == Gates::GateNot || d_gate == Gates::GateBuf) {
    if (d_inConnections.size() > 1) {
      std::cerr << "Invalid: one-input vertex \"" << d_name
                << "\" has inputs: " << d_inConnections.size() << '\n';
    }
    return printUnaryOperators();
  }
  if (d_inConnections.size() == 1) {

    std::cerr << "Invalid: multiple-input vertex \"" << d_name
              << "\" has one input\n";
  }
  return printBinaryOperators();
}

#ifdef LOGFLAG
void GraphVertexGates::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: " << GraphUtils::parseVertexToString(VertexTypes::gate)
     << "(" + GraphUtils::parseGateToString(d_gate) + ")" << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << d_level << "\n";
  os << "Vertex Hash: " << d_hashed << "\n";
}
#endif
GraphVertexBusGate::GraphVertexBusGate(Gates i_gate, std::string_view i_name,
                                       GraphPtr i_baseGraph, size_t i_width) :
    GraphVertexGates(i_gate, i_name, i_baseGraph, true),
    GraphVertexBus(i_width) {
}

GraphVertexBusSlice::GraphVertexBusSlice(std::string_view i_name,
                                         GraphPtr i_baseGraph, size_t i_begin,
                                         size_t i_width) :
    GraphVertexBusGate(GateSlice, i_name, i_baseGraph, i_width),
    d_begin(i_begin) {
}

std::string GraphVertexBusSlice::getSliceSuffix() const {
  // Width W starting at begin maps to [begin+W-1 : begin] (W bits).
  if (d_width <= 1)
    return fmt::format("[{}];\n", d_begin);
  return fmt::format("[{}:{}];\n", d_begin + d_width - 1, d_begin);
}

std::string GraphVertexBusSlice::toVerilog() const {
  if (d_inConnections.size() > 1) {
    std::cerr << "Gate of type 'GateSlice' can not have"
              << "more than one vertex in d_inConnections " << d_name
              << std::endl;
    return "";
  }
  return fmt::format("assign {} = {}{}", getRawName(),
                     getInConnections()[0]->getRawName(), getSliceSuffix());
}
std::string GraphVertexBusSlice::toOneBitVerilog() const {
  if (d_inConnections.size() > 1) {
    std::cerr << "Gate of type 'GateSlice' can not have"
              << "more than one vertex in d_inConnections " << d_name
              << std::endl;
#ifdef LOGFLAG
    LOG(INFO) << "Gate of type 'GateSlice' can not have"
              << "more than one vertex in d_inConnections " << d_name
              << std::endl;
#endif
    return "";
  }
  // Slice wires are declared as name_0 .. name_{W-1}; source bits are absolute.
  std::stringstream stream;
  for (size_t j = 0; j < d_width; ++j) {
    stream << "assign " << getRawName() << "_" << j << " = "
           << getInConnections()[0]->getRawName() << "_" << (d_begin + j)
           << ";\n\t";
  }
  return stream.str();
}
std::string GraphVertexBusGate::toVerilog() const {
  return GraphVertexGates::toVerilog();
}
void getNamesVectorFirst(std::vector<std::string> &names,
                         const std::vector<VertexPtr> &inConnections) {
  for (auto *vertex: inConnections)
    names.push_back(fmt::format("{}{}", vertex->getRawName(),
                                (vertex->isBus() ? "_0" : "")));
}

void getNamesVector(std::vector<std::string> &names,
                    const std::vector<VertexPtr> &inConnections, size_t number,
                    std::string filler = "1'bx") {
  for (auto *vertex: inConnections) {
    if (vertex->isBus() &&
        CG_Graph::GraphVertexBus::getBusPointer(vertex)->getWidth() > number)
      names.push_back(fmt::format("{}_{}", vertex->getRawName(), number));
    else
      names.push_back(filler);
  }
}

std::string GraphVertexBusGate::toOneBitVerilog() const {
  std::string oper = VertexUtils::gateToString(d_gate);
  std::stringstream stream;
  auto printUnaryOperators = [&]() {
    if (d_inConnections.empty())
      return stream.str();
    const VertexPtr in = getInConnections().back();
    for (size_t j = 0; j < getWidth(); ++j) {
      std::string temporaryName;
      if (in->isBus() && getBusPointer(in)->getWidth() > j)
        temporaryName = fmt::format("{}_{}", in->getRawName(), j);
      else if (!in->isBus() && j == 0)
        temporaryName = std::string(in->getRawName());
      else
        temporaryName = "1'bx";
      stream << fmt::format("assign {}_{} = {}{}{};\n\t", getRawName(), j, oper,
                            temporaryName, "");
    }
    return stream.str();
  };
  auto printBinaryOperators = [&]() {
    std::string begin, end;
    std::vector<std::string> names;
    if (d_gate == Gates::GateNand || d_gate == Gates::GateNor ||
        d_gate == Gates::GateXnor) {
      begin = "~ ( ";
      end = " )";
    }
    if (d_gate == Gates::GateConcatenation) {
      begin = "{ ";
      end = " }";
    }
    names.clear();
    getNamesVectorFirst(names, d_inConnections);
    stream << fmt::format("assign {}_0 = {}{}{};\n\t", getRawName(), begin,
                          fmt::join(names, " " + oper + " "), end);

    for (size_t j = 1; j < getWidth(); ++j) {
      names.clear();
      getNamesVector(names, d_inConnections, j);
      stream << fmt::format("assign {}_{} = {}{}{};\n\t", getRawName(), j,
                            begin, fmt::join(names, " " + oper + " "), end);
    }
    return stream.str();
  };
  return toVerilogCommon(printBinaryOperators, printUnaryOperators);
}
} // namespace CG_Graph
