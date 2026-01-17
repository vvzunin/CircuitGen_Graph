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
#include <type_traits>
#define GraphPtr std::shared_ptr<CG_Graph::OrientedGraph>
#define GraphPtrWeak std::weak_ptr<CG_Graph::OrientedGraph>
#define VertexPtr CG_Graph::GraphVertexBase *

namespace CG_Graph {
class GraphVertexBus;
GraphVertexBus::GraphVertexBus(size_t i_width) : d_width(i_width) {
}

void GraphVertexBus::setWidth(size_t i_width) {
  if (d_width == 1)
    d_width = i_width;
}
size_t GraphVertexBus::getWidth() const {
  return d_width;
}


const GraphVertexBus *GraphVertexBus::getBusPointer(const VertexPtr i_vertex) {
  switch (i_vertex->getFullType()) {
    case inputBus:
      return static_cast<const GraphVertexBusInput *>(i_vertex);
      break;
    case outputBus:
      return static_cast<const GraphVertexBusOutput *>(i_vertex);
      break;
    case gateBus:
      return static_cast<const GraphVertexBusGate *>(i_vertex);
      break;
    case constantBus:
      return static_cast<const GraphVertexBusConstant *>(i_vertex);
      break;
    case sequentialBus:
      return static_cast<const GraphVertexBusSequential *>(i_vertex);
      break;
    default: {
      std::cerr << "This vertex is not a bus\n";
      return nullptr;
    }
  }
}

GraphVertexBus *GraphVertexBus::getBusPointer(VertexPtr i_vertex) {
  switch (i_vertex->getFullType()) {
    case inputBus:
      return static_cast<GraphVertexBusInput *>(i_vertex);
      break;
    case outputBus:
      return static_cast<GraphVertexBusOutput *>(i_vertex);
      break;
    case gateBus:
      return static_cast<GraphVertexBusGate *>(i_vertex);
      break;
    case constantBus:
      return static_cast<GraphVertexBusConstant *>(i_vertex);
      break;
    case sequentialBus:
      return static_cast<GraphVertexBusSequential *>(i_vertex);
      break;
    default: {
      std::cerr << "This vertex is not a bus\n";
      return nullptr;
    }
  }
}

std::string GraphVertexBus::getBusNameSuffix() {
  return "[" + std::to_string(d_width - 1) + ":0]";
}

void GraphVertexBus::updateValueBus(std::string i_value) {
  if (i_value != "")
    d_value = i_value;
}

bool GraphVertexBus::compareBusWidth(const VertexPtr i_left,
                                     const VertexPtr i_right) {
  if (!i_left->isBus())
    return false;
  if (!i_right->isBus())
    return true;
  return GraphVertexBus::getBusPointer(i_left)->getWidth() <
         GraphVertexBus::getBusPointer(i_right)->getWidth();
}
}; // namespace CG_Graph