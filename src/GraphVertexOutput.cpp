/**
 * @file GraphVertexOutput.cpp
 * @brief Реализация вершины-выхода графа.
 */
#include "CircuitGenGraph/GraphUtils.hpp"
#include "CircuitGenGraph/GraphVertexBase.hpp"
#include <CircuitGenGraph/GraphVertex.hpp>
#include <algorithm>
#include <cstddef>
#include <sstream>
#include <string>

#include <CircuitGenGraph/Logging.hpp>

namespace CG_Graph {

GraphVertexOutput::GraphVertexOutput(GraphPtr i_baseGraph, bool i_isBus) :
    GraphVertexBase(i_isBus ? VertexTypes::outputBus : output, i_baseGraph) {
}

GraphVertexOutput::GraphVertexOutput(std::string_view i_name,
                                     GraphPtr i_baseGraph, bool i_isBus) :
    GraphVertexBase(i_isBus ? VertexTypes::outputBus : output, i_name,
                    i_baseGraph) {
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
    CG_LOG_INFO << counter++ << ". " << ptr->getName() << " ("
                << ptr->getTypeName() << ")";
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
GraphVertexBusOutput::GraphVertexBusOutput(std::string_view i_name,
                                           GraphPtr i_baseGraph,
                                           size_t i_width) :
    GraphVertexOutput(i_name, i_baseGraph, true), GraphVertexBus(i_width) {
}

std::string GraphVertexBusOutput::toOneBitVerilog() const {
  std::stringstream res;
  size_t minWidth = -1;
  if (!d_inConnections.empty())
    minWidth = getBusPointer(
                   (*std::min_element(d_inConnections.begin(),
                                      d_inConnections.end(), hasSmallerWidth)))
                   ->getWidth();
  // Match scalar output / updateValue: first driver is the source.
  const VertexPtr driver = d_inConnections.front();
  if (d_inConnections.size() > 1) {
    CG_LOG_WARNING << "Bus output '" << d_name << "' has "
                   << d_inConnections.size()
                   << " drivers; one-bit Verilog uses the first connection";
  }
  for (size_t i = 0; i < std::min(minWidth, getWidth()); ++i)
    res << "assign " << getRawName() << "_" << i << " = " << driver->getRawName()
        << "_" << i << ";\n\t";
  return res.str();
}

} // namespace CG_Graph
