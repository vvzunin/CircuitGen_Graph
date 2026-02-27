/**
 * @file GraphVertexGates.cpp
 * @brief Реализация вершины-логического элемента (гейт) графа.
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 * @author Fuuulkrum7 <fuuulkrum7@gmail.com>
 * @author Theossr <feolab05@gmail.com>
 * @author rainbowkittensss <viktorrrrry20@gmail.com>
 */
#include <iostream>

#include <CircuitGenGraph/GraphVertex.hpp>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CG_Graph {

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
GraphVertexGates::GraphVertexGates(Gates i_gate, GraphPtr i_baseGraph) :
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    GraphVertexBase(VertexTypes::gate, i_baseGraph) {
  d_gate = i_gate;
}

GraphVertexGates::GraphVertexGates(Gates i_gate, std::string_view i_name,
                                   GraphPtr i_baseGraph) :
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    GraphVertexBase(VertexTypes::gate, i_name, i_baseGraph) {
  d_gate = i_gate;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
Gates GraphVertexGates::getGate() const {
  return d_gate;
}
/** @author rainbowkittensss <viktorrrrry20@gmail.com> */
uint32_t GraphVertexGates::addVertexToInConnections(VertexPtr i_vert) {
/** @author rainbowkittensss <viktorrrrry20@gmail.com> */
  if ((getGate() == Gates::GateBuf || getGate() == Gates::GateNot) &&
      !d_inConnections.empty()) {
    throw std::length_error("Buf and Not gate types can not obtain more than "
                            "one input connection.");
  }
/** @author rainbowkittensss <viktorrrrry20@gmail.com> */
  return GraphVertexBase::addVertexToInConnections(i_vert);
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
char GraphVertexGates::updateValue() {
  std::map<char, char> table;
  d_value = ValueStates::NoSignal;
  if (d_inConnections.size() > 0) {
/** @author Theossr <feolab05@gmail.com> */
    if (d_inConnections.front()->getValue() == ValueStates::UndefindedState) {
      d_inConnections.front()->updateValue();
    }
    d_value = d_inConnections.front()->getValue();
/** @author Theossr <feolab05@gmail.com> */
    if (d_gate == Gates::GateNot || d_gate == Gates::GateBuf) {
/** @author Theossr <feolab05@gmail.com> */
      if (d_gate == Gates::GateNot)
        table = tableNot;
      else
        table = tableBuf;
      d_value = table.at(d_value);
    }
    for (size_t i = 1; i < d_inConnections.size(); i++) {
/** @author Theossr <feolab05@gmail.com> */
      if (d_inConnections.at(i)->getValue() == ValueStates::UndefindedState) {
        d_inConnections.at(i)->updateValue();
      }
      switch (d_gate) {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
        case (Gates::GateAnd):
          table = tableAnd.at(d_value);
          break;
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
        case (Gates::GateNand):
          table = tableNand.at(d_value);
          break;
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
        case (Gates::GateOr):
          table = tableOr.at(d_value);
          break;
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
        case (Gates::GateNor):
          table = tableNor.at(d_value);
          break;
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
        case (Gates::GateXor):
          table = tableXor.at(d_value);
          break;
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
        case (Gates::GateXnor):
          table = tableXnor.at(d_value);
          break;
        default:
#ifdef LOGFLAG
/** @author Vladimir Zunin <vzunin@hse.ru> */
          LOG(ERROR) << "Error" << std::endl;
#else
          std::cerr << "Error" << std::endl;
#endif
      }
      d_value = table.at(d_inConnections.at(i)->getValue());
    }
  }
  return d_value;
}

/** @author Theossr <feolab05@gmail.com> */
void GraphVertexGates::removeValue() {
  d_value = ValueStates::UndefindedState;
  if (d_inConnections.size() > 0) {
    for (VertexPtr ptr: d_inConnections) {
/** @author Theossr <feolab05@gmail.com> */
      if (ptr->getValue() != ValueStates::UndefindedState) {
        ptr->removeValue();
      }
    }
  }
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
size_t GraphVertexGates::calculateHash() {
  if (d_hasHash) {
    return d_hashed;
  }
  std::string hashedStr =
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
      std::to_string(d_outConnections.size()) + std::to_string(d_gate);

  d_hasHash = HC_IN_PROGRESS;
  std::vector<size_t> hashed_data;
  hashed_data.reserve(d_inConnections.size());

  for (auto &child: d_inConnections) {
    hashed_data.push_back(child->calculateHash());
  }
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  std::sort(hashed_data.begin(), hashed_data.end());

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  hashedStr.reserve(sizeof(decltype(hashed_data)::value_type) *
                    hashed_data.size());
  for (const auto &sub: hashed_data) {
    hashedStr += sub;
  }
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  d_hashed = std::hash<std::string>{}(hashedStr);
  d_hasHash = HC_CALC;

  return d_hashed;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
std::string GraphVertexGates::getVerilogString() const {
  std::string s = "";

  if (d_inConnections.size() > 0) {
    VertexPtr ptr = d_inConnections.at(0);
    if (!ptr) {
      throw std::invalid_argument(
          "Cannot use nullptr for printing it to verilog");
    }

    if (this->d_baseGraph.lock() == ptr->getBaseGraph().lock())
      s = ptr->getName();
    else
      s = ptr->getBaseGraph().lock()->getName() + "_" + ptr->getName();

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    if (d_gate == Gates::GateNot)
      s = "~" + s;
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    if ((d_gate == Gates::GateNand) || (d_gate == Gates::GateNor) ||
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
        (d_gate == Gates::GateXnor))
      s = "~(" + s;

    for (size_t i = 1; i < d_inConnections.size(); i++) {
      ptr = d_inConnections.at(i);
      if (!ptr) {
        throw std::invalid_argument(
            "Cannot use nullptr for printing it to verilog");
      }

      std::string name;
      if (this->d_baseGraph.lock() == ptr->getBaseGraph().lock())
        name = ptr->getName();
      else
        name = ptr->getBaseGraph().lock()->getName() + "_" + ptr->getName();

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
      s += " " + VertexUtils::gateToString(d_gate) + " " + name;
      if (d_gate == GateDefault)
#ifdef LOGFLAG
/** @author Vladimir Zunin <vzunin@hse.ru> */
        LOG(ERROR) << "Error" << std::endl;
#else
        std::cerr << "Error" << std::endl;
#endif
    }

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    if ((d_gate == Gates::GateNand) || (d_gate == Gates::GateNor) ||
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
        (d_gate == Gates::GateXnor))
      s += ")";
  }

  return s;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
std::string GraphVertexGates::toVerilog() const {
  if (!(d_inConnections.size())) {
#ifdef LOGFLAG
/** @author Vladimir Zunin <vzunin@hse.ru> */
    LOG(ERROR) << "TODO: delete empty vertices: " << d_name << std::endl;
#else
    std::cerr << "TODO: delete empty vertices: " << d_name << std::endl;
#endif
    return "";
  }
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  std::string basic = "assign " + getName() + " = ";

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  std::string oper = VertexUtils::gateToString(d_gate);
  VertexPtr ptr = d_inConnections.back();
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  if (d_gate == Gates::GateNot || d_gate == Gates::GateBuf) {
    if (d_inConnections.size() > 1) {
      std::cerr << "Invalid: one-input vertex \"" << d_name
                << "\" has inputs: " << d_inConnections.size() << '\n';
    }
    basic += oper + ptr->getName() + ";";

    return basic;
  }
  if (d_inConnections.size() == 1) {
    std::cerr << "Invalid: multiple-input vertex \"" << d_name
              << "\" has one input\n";
  }

  std::string end = "";

  if (d_gate == Gates::GateNand || d_gate == Gates::GateNor ||
      d_gate == Gates::GateXnor) {
    basic += "~ ( ";

    end = " )";
  }
  for (size_t i = 0; i < d_inConnections.size() - 1; ++i) {
    basic += d_inConnections.at(i)->getName() + " " + oper + " ";
  }
  basic += d_inConnections.back()->getName() + end + ";";

  return basic;
}

/** @author Vladimir Zunin <vzunin@hse.ru> */
DotReturn GraphVertexGates::toDOT() {
  if (!d_inConnections.size()) {
#ifdef LOGFLAG
/** @author Vladimir Zunin <vzunin@hse.ru> */
    LOG(ERROR) << "TODO: delete empty vertices: " << d_name << std::endl;
#else
    std::cerr << "TODO: delete empty vertices: " << d_name << std::endl;
#endif
    return {};
  }

  DotReturn dot;

  dot.push_back({DotTypes::DotGate,
                 {{"name", getName()},
                  {"label", getName()},
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
                  {"level", std::to_string(d_level)}}});

  for (VertexPtr ptr: d_inConnections) {
    dot.push_back(
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
        {DotTypes::DotEdge, {{"from", ptr->getName()}, {"to", getName()}}});
  }
  return dot;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
bool GraphVertexGates::isSubgraphBuffer() const {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  if (d_gate != Gates::GateBuf || d_inConnections.empty()) {
    return false;
  }
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  return d_inConnections.front()->getType() == VertexTypes::subGraph;
}

#ifdef LOGFLAG
/** @author Vladimir Zunin <vzunin@hse.ru> */
void GraphVertexGates::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  os << "Vertex Type: " << GraphUtils::parseVertexToString(VertexTypes::gate)
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
     << "(" + GraphUtils::parseGateToString(d_gate) + ")" << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << d_level << "\n";
  os << "Vertex Hash: " << d_hashed << "\n";
}
#endif

} // namespace CG_Graph
