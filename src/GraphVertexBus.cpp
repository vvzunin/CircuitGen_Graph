#include "CircuitGenGraph/GraphUtils.hpp"
#include "CircuitGenGraph/GraphVertex.hpp"
#include "CircuitGenGraph/GraphVertexBase.hpp"
#include "CircuitGenGraph/GraphVertexBus.hpp"
#include "CircuitGenGraph/OrientedGraph.hpp"
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <string>
#include <sys/stat.h>
#define GraphPtr std::shared_ptr<CG_Graph::OrientedGraph>
#define GraphPtrWeak std::weak_ptr<CG_Graph::OrientedGraph>
#define VertexPtr CG_Graph::GraphVertexBase *
namespace CG_Graph {
  class GraphVertexBus;
GraphVertexBus::GraphVertexBus(size_t i_width) : d_width(i_width){}


void GraphVertexBus::setWidth(size_t i_width) {
  if (d_width == 1) d_width = i_width;
}
  size_t GraphVertexBus::getWidth() const {
    return d_width;
   }
GraphVertexBus* GraphVertexBus::getBusPointer(const VertexPtr vertex) {
switch (vertex->getFullType()) {
        case inputBus:
        return static_cast<GraphVertexBusInput*>(const_cast< VertexPtr>(vertex));
        break;
        case outputBus:
        return static_cast<GraphVertexBusOutput*>(const_cast< VertexPtr>(vertex));
        break;
        case gateBus:
        return static_cast<GraphVertexBusGate*>(const_cast< VertexPtr>(vertex));
        break;
        case constantBus:
        return static_cast<GraphVertexBusConstant*>(const_cast< VertexPtr>(vertex));
        break;
        case sequentialBus:
        return static_cast<GraphVertexBusSequential*>(const_cast< VertexPtr>(vertex));
        break;
        default: {
        std::cerr << "This vertex is not a bus\n";
        return nullptr;
        }
}
}
GraphVertexBus * GraphVertexBus::getBusPointer(VertexPtr vertex) {
        switch (vertex->getFullType()) {
        case inputBus:
         return static_cast<GraphVertexBusInput*>(vertex);
        break;
        case outputBus:
        return static_cast<GraphVertexBusOutput*>(vertex);
        break;
        case gateBus:
        return static_cast<GraphVertexBusGate*>(vertex);
        break;
        case constantBus:
        return static_cast<GraphVertexBusConstant*>(vertex);
        break;
        case sequentialBus:
        return static_cast<GraphVertexBusSequential*>(vertex);
        break;
        default: {
        std::cerr << "This vertex is not a bus\n";
        return nullptr;
        }
        }
    } 
  std::string GraphVertexBus::getBusNameSuffix() {
    return "[" + std::to_string(d_width-1) +":0]";
  } 
  void GraphVertexBus::updateValueBus(std::string i_value) {
    if (i_value != "")
    d_value = i_value;
  }
bool GraphVertexBus::compareBusWidth(const VertexPtr left, const VertexPtr right) 
{ if (!left->isBus()) return false;
  if (!right->isBus()) return true;
  return GraphVertexBus::getBusPointer(left)->getWidth() < GraphVertexBus::getBusPointer(right)->getWidth(); 
}
}; // namespace CG_Graph