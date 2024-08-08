#include <memory>

#include <CircuitGenGraph/GraphVertex.hpp>
#include "easyloggingpp/easylogging++.h"

GraphVertexConstant::GraphVertexConstant(char i_const, GraphPtr i_baseGraph) :
  GraphVertexInput(i_baseGraph, VertexTypes::constant) {
  d_value = i_const;
}
GraphVertexConstant::GraphVertexConstant(
    char              i_const,
    const std::string i_name,
    GraphPtr          i_baseGraph
) :
  GraphVertexInput(i_name, i_baseGraph, VertexTypes::constant) {
  d_value = i_const;
}

void GraphVertexConstant::updateLevel() {
  d_level = 0;
}

std::string GraphVertexConstant::getInstance() {
  return "wire " + d_name + ";";
}

std::string GraphVertexConstant::toVerilog() {
  return "assign " + d_name + " = 1'b" + d_value + ";";
}

std::string GraphVertexConstant::toDOT() {
  std::string str = "const";
  str += d_value + " -> " + d_name + ";";
  return str;
}

void GraphVertexConstant::log(el::base::type::ostream_t& os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "") << ")\n";
  os << "Vertex Type: " << d_settings->parseVertexToString(VertexTypes::constant) << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << 0 << "\n";
  os << "Vertex Hash: " << "NuN" << "\n";
}