/**
 * @file GraphVertexOutput.cpp
 * @brief Реализация вершины-выхода графа.
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 * @author Theossr <feolab05@gmail.com>
 * @author NonDif <shapkin.andrey123@gmail.com>
 */
#include "CircuitGenGraph/GraphVertexBase.hpp"
#include <CircuitGenGraph/GraphVertex.hpp>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CG_Graph {

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
GraphVertexOutput::GraphVertexOutput(GraphPtr i_baseGraph) :
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    GraphVertexBase(VertexTypes::output, i_baseGraph) {
}

GraphVertexOutput::GraphVertexOutput(std::string_view i_name,
                                     GraphPtr i_baseGraph) :
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    GraphVertexBase(VertexTypes::output, i_name, i_baseGraph) {
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
char GraphVertexOutput::updateValue() {
  if (d_inConnections.empty()) {
/** @author Theossr <feolab05@gmail.com> */
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

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
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

/** @author Vladimir Zunin <vzunin@hse.ru> */
DotReturn GraphVertexOutput::toDOT() {
  DotReturn dot;

  dot.push_back({DotTypes::DotOutput,
                 {{"name", getName()},
                  {"label", getName()},
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
                  {"level", std::to_string(d_level)}}});

  for (VertexPtr ptr: d_inConnections) {
    dot.push_back({DotTypes::DotEdge,
                   {{"from", ptr->getName()},
                    {"to", getName()},
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
                    {"level", std::to_string(d_level)}}});
  }
  return dot;
}

#ifdef LOGFLAG
/** @author Vladimir Zunin <vzunin@hse.ru> */
void GraphVertexOutput::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  os << "Vertex Type: " << GraphUtils::parseVertexToString(VertexTypes::output)
     << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << d_level << "\n";
  os << "Vertex Hash: "
     << "NuN"
     << "\n";
}
#endif

} // namespace CG_Graph
