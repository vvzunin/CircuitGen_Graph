/**
 * @file GraphVertexInput.cpp
 * @brief Реализация вершины-входа графа.
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 * @author Theossr <feolab05@gmail.com>
 * @author NonDif <shapkin.andrey123@gmail.com>
 */
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

/** @author Theossr <feolab05@gmail.com> */
void GraphVertexInput::setValue(const char value) {
  d_value = value;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
char GraphVertexInput::updateValue() {
  return d_value;
}

/** @author Theossr <feolab05@gmail.com> */
void GraphVertexInput::removeValue() {
  d_value = ValueStates::UndefindedState;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
void GraphVertexInput::updateLevel() {
  if (d_needUpdate != VS_NOT_CALC) {
    return;
  }
#ifdef LOGFLAG
  LOG(INFO) << "0. " << d_name << " (" << getTypeName() << ")";
#endif
  d_level = 0;
  d_needUpdate = VS_CALC;
}

/** @author Vladimir Zunin <vzunin@hse.ru> */
DotReturn GraphVertexInput::toDOT() {
  DotReturn dot;

  dot.push_back({DotTypes::DotInput,
                 {{"name", getName()},
                  {"label", getName()},
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
                  {"level", std::to_string(d_level)}}});
  return dot;
}

#ifdef LOGFLAG
/** @author Vladimir Zunin <vzunin@hse.ru> */
void GraphVertexInput::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
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
