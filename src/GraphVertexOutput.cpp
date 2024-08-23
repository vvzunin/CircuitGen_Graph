#include <CircuitGenGraph/GraphVertex.hpp>
#include "easyloggingpp/easylogging++.h"

GraphVertexOutput::GraphVertexOutput(GraphPtr i_baseGraph) :
  GraphVertexBase(VertexTypes::output, i_baseGraph) {}

GraphVertexOutput::GraphVertexOutput(
    const std::string i_name,
    GraphPtr          i_baseGraph
) :
  GraphVertexBase(VertexTypes::output, i_name, i_baseGraph) {}

char GraphVertexOutput::updateValue() {
  if (d_inConnections.size() > 0) {
    if (auto ptr = d_inConnections[0].lock()) {
      d_value = ptr->getValue();
    } else {
      throw std::invalid_argument("Dead pointer!");
    }
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
  return d_value;
}

void GraphVertexOutput::updateLevel(std::string tab) {
  int counter = 0;
  for (VertexPtrWeak vert : d_inConnections) {
    if (auto ptr = vert.lock()) {
      LOG(INFO) << counter++ << ". " << ptr->getName() << " (" << ptr->getTypeName() << ")";
      ptr->updateLevel(tab + "  ");
      d_level = (ptr->getLevel() > d_level) ? ptr->getLevel() : d_level;
    } else {
      throw std::invalid_argument("Dead pointer!");
    }
  }
}

DotReturn GraphVertexOutput::toDOT() {
  DotReturn dot;

  dot.push_back({DotTypes::DotOutput, {
    {"name", d_name},
    {"label", d_name},
    {"level", std::to_string(d_level)}
  }});

  for (VertexPtrWeak ptrWeak : d_inConnections) {
    VertexPtr ptr = ptrWeak.lock();
    dot.push_back({DotTypes::DotEdge, {
      {"from", ptr->getName()},
      {"to", d_name},
      {"level", std::to_string(d_level)}
    }});
  }
  return dot;
}

void GraphVertexOutput::log(el::base::type::ostream_t& os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "") << ")\n";
  os << "Vertex Type: " << d_settings->parseVertexToString(VertexTypes::output) << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << d_level << "\n";
  os << "Vertex Hash: " << "NuN" << "\n";
}