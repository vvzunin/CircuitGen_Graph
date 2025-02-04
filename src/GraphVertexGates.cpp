#include <iostream>

#include <CircuitGenGraph/GraphVertex.hpp>

#include "easyloggingpp/easylogging++.h"

GraphVertexGates::GraphVertexGates(Gates i_gate, GraphPtr i_baseGraph) :
    GraphVertexBase(VertexTypes::gate, i_baseGraph) {
  d_gate = i_gate;
}

GraphVertexGates::GraphVertexGates(Gates i_gate, std::string_view i_name,
                                   GraphPtr i_baseGraph) :
    GraphVertexBase(VertexTypes::gate, i_name, i_baseGraph) {
  d_gate = i_gate;
}

Gates GraphVertexGates::getGate() const {
  return d_gate;
}

char GraphVertexGates::updateValue() {
  std::map<char, char> table;
  if (d_inConnections.size() > 0) {
    VertexPtr ptr = d_inConnections.at(0);

    d_value = ptr->getValue();
    if (d_gate == Gates::GateBuf)
      d_value = tableBuf.at(ptr->getValue());
    if (d_gate == Gates::GateNot)
      d_value = tableNot.at(ptr->getValue());
    for (size_t i = 1; i < d_inConnections.size(); i++) {
      switch (d_gate) {
        case (Gates::GateAnd):
          table = tableAnd.at(d_value);
          break;
        case (Gates::GateNand):
          table = tableNand.at(d_value);
          break;
        case (Gates::GateOr):
          table = tableOr.at(d_value);
          break;
        case (Gates::GateNor):
          table = tableNor.at(d_value);
          break;
        case (Gates::GateXor):
          table = tableXor.at(d_value);
          break;
        case (Gates::GateXnor):
          table = tableXnor.at(d_value);
          break;
        default:
          LOG(ERROR) << "Error" << std::endl;
      }
      ptr = d_inConnections.at(i);
      d_value = table.at(ptr->getValue());
    }
  }
  return d_value;
}

size_t GraphVertexGates::calculateHash(bool i_recalculate) {
  if (d_hasHash && (!i_recalculate || d_hasHash == 2)) {
    return d_hashed;
  }
  std::string hashedStr =
      std::to_string(d_inConnections.size()) + std::to_string(d_gate);

  d_hasHash = 2;
  // future sorted struct
  std::vector<size_t> hashed_data;

  for (auto &child: d_outConnections) {
    hashed_data.push_back(child->calculateHash(i_recalculate));
  }
  std::sort(hashed_data.begin(), hashed_data.end());

  for (const auto &sub: hashed_data) {
    hashedStr += sub;
  }

  d_hashed = std::hash<std::string>{}(hashedStr);
  d_hasHash = 1;

  return d_hashed;
}

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

    if (d_gate == Gates::GateNot)
      s = "~" + s;
    if ((d_gate == Gates::GateNand) || (d_gate == Gates::GateNor) ||
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

      s += " " + VertexUtils::gateToString(d_gate) + " " + name;
      if (d_gate == GateDefault)
        LOG(ERROR) << "Error" << std::endl;
    }

    if ((d_gate == Gates::GateNand) || (d_gate == Gates::GateNor) ||
        (d_gate == Gates::GateXnor))
      s += ")";
  }

  return s;
}

std::string GraphVertexGates::toVerilog() {
  if (!d_inConnections.size()) {
    LOG(ERROR) << "TODO: delete empty vertices: " << d_name << std::endl;
    return "";
  }
  std::string basic = "assign " + getName() + " = ";

  std::string oper = VertexUtils::gateToString(d_gate);
  VertexPtr ptr = d_inConnections.back();
  if (d_gate == Gates::GateNot || d_gate == Gates::GateBuf) {
    basic += oper + ptr->getName() + ";";

    return basic;
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

DotReturn GraphVertexGates::toDOT() {
  if (!d_inConnections.size()) {
    LOG(ERROR) << "TODO: delete empty vertices: " << d_name << std::endl;
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

void GraphVertexGates::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: "
     << DefaultSettings::parseVertexToString(VertexTypes::gate)
     << "(" + DefaultSettings::parseGateToString(d_gate) + ")"
     << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << d_level << "\n";
  os << "Vertex Hash: " << d_hashed << "\n";
}