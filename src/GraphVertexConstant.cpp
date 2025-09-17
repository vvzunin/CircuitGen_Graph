#include "CircuitGenGraph/GraphUtils.hpp"
#include <cassert>
#include <memory>

#include <CircuitGenGraph/GraphVertex.hpp>
#include <sstream>
#include <string>
#include <string_view>

#ifdef lOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CG_Graph {

GraphVertexConstant::GraphVertexConstant(char i_const, GraphPtr i_baseGraph, bool i_isBus) :
    GraphVertexInput(i_baseGraph,i_isBus ? constantBus : VertexTypes::constant) {
  d_value = i_const;
}

GraphVertexConstant::GraphVertexConstant(char i_const, std::string_view i_name,
                                         GraphPtr i_baseGraph, bool i_isBus) :
    GraphVertexInput(i_name, i_baseGraph, i_isBus ? constantBus : VertexTypes::constant) {
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

#ifdef LOGFLAG
void GraphVertexConstant::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: "
     << GraphUtils::parseVertexToString(VertexTypes::constant) << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << 0 << "\n";
  os << "Vertex Hash: " << "NuN" << "\n";
}
#endif
GraphVertexBusConstant::GraphVertexBusConstant(std::string_view i_name, GraphPtr i_baseGraph, size_t i_width)
: GraphVertexConstant('x',i_name, i_baseGraph, true),
 GraphVertexBus(i_width) {
d_valueBus = std::string("x",i_width);
}
void GraphVertexBusConstant::setValue(std::string i_value) {
d_valueBus = i_value;
}

std::string GraphVertexBusConstant::toVerilog() const {
return "assign " + getName() + " =" + std::to_string(getWidth())+"'b" + d_valueBus + ";";
}
std::string GraphVertexBusConstant::getVerilogInstance(){
 return "wire " + getName()+getBusNameSuffix() + ";";  
}
std::string GraphVertexBusConstant::getVerilogInstanceSeparate() {
    std::stringstream ans;
    ans << "wire ";
 for (int i = 0; i < getWidth(); ++i) {
    ans  << getName() << "_" << i << (i != getWidth()-1 ? ", ": ";\n");
  }
  return ans.str();
}
std::string GraphVertexBusConstant::toOneBitVerilog() const {
  std::stringstream ans;
  for (int i = 0; i < getWidth(); ++i) {
    ans <<  "assign " << getName() << "_" << i << " = " << d_valueBus[i] << ";\n";
  }
  return ans.str();
}
} // namespace CG_Graph
