#include "CircuitGenGraph/GraphVertexBase.hpp"
#include <CircuitGenGraph/GraphVertex.hpp>
#include <algorithm>
#include <cstddef>
#include <sstream>
#include <string>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CG_Graph {

GraphVertexOutput::GraphVertexOutput(GraphPtr i_baseGraph, bool i_isBus) :
    GraphVertexBase(VertexTypes::output, i_baseGraph, i_isBus) {
}

GraphVertexOutput::GraphVertexOutput(std::string_view i_name,
                                     GraphPtr i_baseGraph, bool i_isBus) :
    GraphVertexBase(VertexTypes::output, i_name, i_baseGraph, i_isBus) {
}

char GraphVertexOutput::updateValue() {
  if (d_inConnections.empty()) {
    return (d_value = ValueStates::NoSignal);
  }
  d_value = d_inConnections.front()->updateValue();

  for (size_t i = 1; i < d_inConnections.size(); i++) {
    if (d_inConnections.at(i)->updateValue() != d_value) {
      d_value = ValueStates::NoSignal;
    }
  }
  return d_value;
}

void GraphVertexOutput::updateLevel() {
  if (d_needUpdate != VS_NOT_CALC) {
    return;
  }
  int counter = 0;
  for (VertexPtr ptr: d_inConnections) {
#ifdef LOGFLAG
    LOG(INFO) << counter++ << ". " << ptr->getName() << " ("
              << ptr->getTypeName() << ")";
#endif
    ptr->updateLevel();
    uint32_t lvl = ptr->getLevel() + 1;
    d_level = (lvl > d_level) ? lvl : d_level;
  }
  d_needUpdate = VS_CALC;
}

DotReturn GraphVertexOutput::toDOT() {
  DotReturn dot;

  dot.push_back({DotTypes::DotOutput,
                 {{"name", getName()},
                  {"label", getName()},
                  {"level", std::to_string(d_level)}}});

  for (VertexPtr ptr: d_inConnections) {
    dot.push_back({DotTypes::DotEdge,
                   {{"from", ptr->getName()},
                    {"to", getName()},
                    {"level", std::to_string(d_level)}}});
  }
  return dot;
}

#ifdef LOGFLAG
void GraphVertexOutput::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: " << GraphUtils::parseVertexToString(VertexTypes::output)
     << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << d_level << "\n";
  os << "Vertex Hash: " << "NuN" << "\n";
}
#endif
GraphVertexBusOutput::GraphVertexBusOutput(std::string_view i_name, GraphPtr i_baseGraph, size_t i_width) 
: GraphVertexOutput(i_name,i_baseGraph, true),
GraphVertexBus(i_width){}

  std::string GraphVertexBusOutput::toOneBitVerilog() const {
    std::stringstream ans;
    size_t minWidth;
    if(!d_inConnections.empty())
    minWidth = getBusPointer( (*std::min_element(d_inConnections.begin(),d_inConnections.end(), compareBusWidth)))->getWidth();
    for (int i = 0; i<std::min(minWidth, getWidth());++i) 
      ans << "assign " << getName()<<"_"<<std::to_string(i) << " = " << 
      d_inConnections.back()->getName() << "_"<<std::to_string(i) <<";\n\t";
    return ans.str();
  }

} // namespace CG_Graph
