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

//GraphVertexBusSlice GraphVertexBus::getSlice(size_t start, size_t end) {
//
//}

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
    } // преобразует вершину к ее дочернему классу и возвращает
                   // ссылку на полученный объект-родителя класса GraphVertexBus
                   // вероятно стоит оформить это как то иначе, но пока так

  std::string GraphVertexBus::getBusNameSuffix() {
    return "[" + std::to_string(d_width-1) +":0]";
  } // возвращает что то в духе [7:0], вероятно это будет
                      // удобно в
                      //  OrientedGraph::toVerilog() и вспомогательных методах
                      // для вывода графа в верилог, по крайней мере более
                      // аккуратного решения пока не придумывается :(
  /// @brief toVerilog
  /// Generates Verilog code for the vertex
  /// @return A string containing Verilog code for the vertex, or an empty
  /// string if the vertex type is not "output" or if the incoming connection
  /// is invalid
  // template<class T>
  // T* GraphVertexBus::as_if() {
  //       if (T::isKind(kind()))
  //           return static_cast<T*>(this);
  //       return static_cast<T*>(nullptr);
  //   }
  
  // вообще все ребра будут создаваться как у обычных вершин, но если
  // пользователь хочет в качестве входа подключить вершину которая имеет
  // большую или меньшую ширину чем у целевой и задать какую именно часть то
  // можно воспользоваться перегрузкой addEdge со срезом и тогда данные о нем
  // будут скопированы в вектор InConnectionSlices. все еще неясно, стоит ли так
  // делать... но это при определенном соотношении срезов к шинам и количестве
  // таких странных ребер экономит память!!!
    /// @brief toVerilog
  /// Generates Verilog code for the vertex
  /// @return A string containing Verilog code for the vertex, or an empty
  /// string if the vertex type is not "output" or if the incoming connection
  /// is invalid

  std::string GraphVertexBus::updateValueBus() {

  }
bool GraphVertexBus::compareBusWidth(const VertexPtr left, const VertexPtr right) 
{ if (!left->isBus()) return false;
  if (!right->isBus()) return true;
  return GraphVertexBus::getBusPointer(left)->getWidth() < GraphVertexBus::getBusPointer(right)->getWidth(); 
}
}; // namespace CG_Graph