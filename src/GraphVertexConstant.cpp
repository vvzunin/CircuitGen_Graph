#include <cassert>
#include <memory>

#include <CircuitGenGraph/GraphVertex.hpp>

#ifdef lOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CG_Graph {

GraphVertexConstant::GraphVertexConstant(char i_const, GraphPtr i_baseGraph) :
    GraphVertexInput(i_baseGraph, VertexTypes::constant) {
  d_value = i_const;
}

GraphVertexConstant::GraphVertexConstant(char i_const, std::string_view i_name,
                                         GraphPtr i_baseGraph) :
    GraphVertexInput(i_name, i_baseGraph, VertexTypes::constant) {
  d_value = i_const;
}

size_t GraphVertexConstant::calculateHash() {
  if (d_hasHash) {
    return d_hashed;
  }
  d_hashed = std::hash<std::string>{}(std::to_string(d_value) +
                                      std::to_string(d_outConnections.size()));
  d_hasHash = HC_CALC;

  return d_hashed;
}

std::string GraphVertexConstant::getVerilogInstance() {
  return "wire " + getName()+ ";";
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

#ifdef LOGFLAG
void GraphVertexConstant::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: "
     << GraphUtils::parseVertexToString(VertexTypes::constant) << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << 0 << "\n";
  os << "Vertex Hash: "
     << "NuN"
     << "\n";
}
#endif

} // namespace CG_Graph
