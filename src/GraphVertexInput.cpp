#include <iostream>

#include <CircuitGenGraph/GraphVertex.hpp>

#include "easyloggingpp/easylogging++.h"

GraphVertexInput::GraphVertexInput(GraphPtr i_baseGraph,
                                   const VertexTypes i_type) :
    GraphVertexBase(i_type, i_baseGraph) {
}

GraphVertexInput::GraphVertexInput(std::string_view i_name,
                                   GraphPtr i_baseGraph,
                                   const VertexTypes i_type) :
    GraphVertexBase(i_type, i_name, i_baseGraph) {
}

// TODO: Он здесь нужен?
char GraphVertexInput::updateValue() {
  if (d_inConnections.size() > 0) {
    d_value = d_inConnections.front()->getValue();

    for (size_t i = 1; i < d_inConnections.size(); i++) {
      if (d_inConnections.at(i)->getValue() != d_value) {
        d_value = 'x';
      }
    }
  }
  return d_value;
}

void GraphVertexInput::updateLevel(bool i_recalculate, std::string tab) {
  if (d_needUpdate && !i_recalculate) {
    return;
  }
  // LOG(INFO) << tab << "0. " << d_name << " (" << getTypeName() << ")";
  d_level = 0;
  d_needUpdate = 1;
}

DotReturn GraphVertexInput::toDOT() {
  DotReturn dot;

  dot.push_back({DotTypes::DotInput,
                 {{"name", getName()},
                  {"label", getName()},
                  {"level", std::to_string(d_level)}}});
  return dot;
}

void GraphVertexInput::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: "
     << DefaultSettings::parseVertexToString(VertexTypes::input) << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << 0 << "\n";
  os << "Vertex Hash: "
     << "NuN"
     << "\n";
}