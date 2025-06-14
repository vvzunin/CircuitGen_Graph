#include <CircuitGenGraph/GraphVertex.hpp>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CG_Graph {

GraphVertexInput::GraphVertexInput(GraphPtr i_baseGraph,
                                   const VertexTypes i_type) :
    GraphVertexBase(i_type, i_baseGraph) {
}

GraphVertexInput::GraphVertexInput(std::string_view i_name,
                                   GraphPtr i_baseGraph,
                                   const VertexTypes i_type) :
    GraphVertexBase(i_type, i_name, i_baseGraph) {
}

void GraphVertexInput::setValue(const char value) {
  d_value = value;
}

char GraphVertexInput::updateValue() {
  return d_value;
}

void GraphVertexInput::removeValue() {
  d_value = ValueStates::UndefindedState;
}

void GraphVertexInput::updateLevel(std::string tab) {
  if (d_needUpdate) {
    return;
  }
#ifdef LOGFLAG
  LOG(INFO) << tab << "0. " << d_name << " (" << getTypeName() << ")";
#endif
  d_level = 0;
  d_needUpdate = VS_CALC;
}

DotReturn GraphVertexInput::toDOT() {
  DotReturn dot;

  dot.push_back({DotTypes::DotInput,
                 {{"name", getName()},
                  {"label", getName()},
                  {"level", std::to_string(d_level)}}});
  return dot;
}

#ifdef LOGFLAG
void GraphVertexInput::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: " << GraphUtils::parseVertexToString(VertexTypes::input)
     << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << 0 << "\n";
  os << "Vertex Hash: "
     << "NuN"
     << "\n";
}
#endif

} // namespace CG_Graph
