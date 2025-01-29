#include <cassert>
#include <memory>

#include <CircuitGenGraph/GraphVertex.hpp>

#include "easyloggingpp/easylogging++.h"

GraphVertexConstant::GraphVertexConstant(char i_const, GraphPtr i_baseGraph) :
    GraphVertexInput(i_baseGraph, VertexTypes::constant) {
  d_value = i_const;
}

GraphVertexConstant::GraphVertexConstant(char i_const, std::string_view i_name,
                                         GraphPtr i_baseGraph) :
    GraphVertexInput(i_name, i_baseGraph, VertexTypes::constant) {
  d_value = i_const;
}

size_t GraphVertexConstant::calculateHash(bool i_recalculate) {
  if (d_hasHash && (!i_recalculate || d_hasHash == IN_PROGRESS)) {
    return d_hashed;
  }
  d_hashed = std::hash<std::string>{}(std::to_string(d_value) +
                                      std::to_string(d_outConnections.size()));
  d_hasHash = CALC;

  return d_hashed;
}

void GraphVertexConstant::updateLevel(bool i_recalculate, std::string tab) {
  if (d_needUpdate && !i_recalculate) {
    return;
  }
  // LOG(INFO) << tab << "0. " << d_name << " (" << getTypeName() << ")";
  d_level = 0;
  d_needUpdate = 1;
}

std::string GraphVertexConstant::getVerilogInstance() {
  return "wire " + getName() + ";";
}

std::string GraphVertexConstant::toVerilog() const {
  return "assign " + getName() + " = 1'b" + d_value + ";";
}

DotReturn GraphVertexConstant::toDOT() {
  DotReturn dot;
  std::string str(1, d_value);

  dot.push_back({DotTypes::DotConstant,
                 {{"name", getName()},
                  {"label", getName()},
                  {"value", "1'b" + str},
                  {"level", std::to_string(d_level)}}});
  return dot;
}

void GraphVertexConstant::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: "
     << DefaultSettings::parseVertexToString(VertexTypes::constant) << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << 0 << "\n";
  os << "Vertex Hash: "
     << "NuN"
     << "\n";
}