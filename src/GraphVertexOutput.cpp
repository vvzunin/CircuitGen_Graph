#include <CircuitGenGraph/GraphVertex.hpp>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CG_Graph {

GraphVertexOutput::GraphVertexOutput(GraphPtr i_baseGraph) :
    GraphVertexBase(VertexTypes::output, i_baseGraph) {
}

GraphVertexOutput::GraphVertexOutput(std::string_view i_name,
                                     GraphPtr i_baseGraph) :
    GraphVertexBase(VertexTypes::output, i_name, i_baseGraph) {
}

char GraphVertexOutput::updateValue() {
  if (d_inConnections.empty()) {
    return (d_value = 'x');
  }
  d_value = d_inConnections.front()->updateValue();

  for (size_t i = 1; i < d_inConnections.size(); i++) {
    if (d_inConnections.at(i)->updateValue() != d_value) {
      d_value = 'x';
    }
  }
  return d_value;
}

void GraphVertexOutput::updateLevel(std::string tab) {
  int counter = 0;
  if (d_needUpdate) {
    return;
  }
  for (VertexPtr ptr: d_inConnections) {
#ifdef LOGFLAG
    LOG(INFO) << tab << counter++ << ". " << ptr->getName() << " ("
              << ptr->getTypeName() << ")";
#endif
    ptr->updateLevel(tab + "  ");
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
  os << "Vertex Hash: "
     << "NuN"
     << "\n";
}
#endif

} // namespace CG_Graph
