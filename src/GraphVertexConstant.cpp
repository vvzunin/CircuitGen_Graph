#include <memory>

#include <CircuitGenGraph/GraphVertex.hpp>

#include "easyloggingpp/easylogging++.h"

GraphVertexConstant::GraphVertexConstant(char i_const, GraphPtr i_baseGraph) :
  GraphVertexInput(i_baseGraph, VertexTypes::constant) {
  d_value = i_const;
}

GraphVertexConstant::GraphVertexConstant(
    char         i_const,
    GraphMemory& memory,
    GraphPtr     i_baseGraph
) :
  GraphVertexInput(memory, VertexTypes::constant, i_baseGraph) {
  d_value = i_const;
}

GraphVertexConstant::GraphVertexConstant(
    char             i_const,
    std::string_view i_name,
    GraphPtr         i_baseGraph
) :
  GraphVertexInput(i_name, i_baseGraph, VertexTypes::constant) {
  d_value = i_const;
}

void GraphVertexConstant::updateLevel(std::string tab) {
  LOG(INFO) << tab << "0. " << d_name << " (" << getTypeName() << ")";
  d_level = 0;
}

std::string GraphVertexConstant::getVerilogInstance() {
  return "wire " + getChangableName() + ";";
}

std::string GraphVertexConstant::toVerilog() {
  return "assign " + getChangableName() + " = 1'b" + d_value + ";";
}

DotReturn GraphVertexConstant::toDOT() {
  DotReturn   dot;
  std::string str(1, d_value);

  dot.push_back(
      {DotTypes::DotConstant,
       {{"name", getChangableName()},
        {"label", getChangableName()},
        {"value", "1'b" + str},
        {"level", std::to_string(d_level)}}}
  );
  return dot;
}

void GraphVertexConstant::log(el::base::type::ostream_t& os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: "
     << SettingsUtils::parseVertexToString(VertexTypes::constant) << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << 0 << "\n";
  os << "Vertex Hash: " << "NuN" << "\n";
}