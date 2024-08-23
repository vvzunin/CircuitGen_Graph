#include <iostream>

#include <CircuitGenGraph/GraphVertex.hpp>
#include "easyloggingpp/easylogging++.h"

GraphVertexInput::GraphVertexInput(
    GraphPtr          i_baseGraph,
    const VertexTypes i_type
) :
  GraphVertexBase(i_type, i_baseGraph) {}

GraphVertexInput::GraphVertexInput(
    const std::string i_name,
    GraphPtr          i_baseGraph,
    const VertexTypes i_type
) :
  GraphVertexBase(i_type, i_name, i_baseGraph) {}


// TODO: Он здесь нужен?
char GraphVertexInput::updateValue() {
  if (d_inConnections.size() > 0) {
    if (!d_baseGraph.lock()) {
      if (auto ptr = d_inConnections[0].lock()) {
        d_value = ptr->getValue();
      } else {
        throw std::invalid_argument("Dead pointer!");
        return 'x';
      }

      for (size_t i = 1; i < d_inConnections.size(); i++) {
        if (auto ptr = d_inConnections[i].lock()) {
          if (ptr->getValue() != d_value) {
            d_value = 'x';
          }
        } else {
          throw std::invalid_argument("Dead pointer!");
        }
      }

    } else {
      LOG(ERROR) << "Error" << std::endl;
    }
  }
  return d_value;
}

void GraphVertexInput::updateLevel(std::string tab) {
  LOG(INFO) << tab << "0. " << d_name << " (" << getTypeName() << ")";
  d_level = 0;
}

DotReturn GraphVertexInput::toDOT() {
  DotReturn dot;

  dot.push_back({DotTypes::DotInput, {
    {"name", d_name},
    {"label", d_name},
    {"level", std::to_string(d_level)}
  }});
  return dot;
}

void GraphVertexInput::log(el::base::type::ostream_t& os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "") << ")\n";
  os << "Vertex Type: " << d_settings->parseVertexToString(VertexTypes::input) << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << 0 << "\n";
  os << "Vertex Hash: " << "NuN" << "\n";
}