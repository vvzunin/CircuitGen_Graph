#include <iostream>
#include <CircuitGenGraph/GraphVertex.hpp>
#include "easyloggingpp/easylogging++.h"
#include "span.hpp"
#include <cassert>

GraphVertexDataBus::GraphVertexDataBus(
    tcb::span<VertexPtr> i_vertices, GraphPtr i_baseGraph) :
    GraphVertexBase(VertexTypes::dataBus, i_baseGraph) {
  assert(!i_vertices.empty() && "Data bus cannot be empty");

  VertexTypes firstType = i_vertices.front()->getType();
  assert(firstType != VertexTypes::subGraph && "Subgraph cannot be in the data bus");
  for (const auto& vertex : i_vertices) {
    assert(vertex->getType() == firstType && "All vertices in the bus must be of the same type");
  }
  tcb::span<VertexPtr> d_vertices = i_vertices;
}

GraphVertexDataBus::GraphVertexDataBus(
    tcb::span<VertexPtr> i_vertices, std::string_view i_name,
    GraphPtr i_baseGraph) :
    GraphVertexBase(VertexTypes::dataBus, i_name, i_baseGraph) {
  assert(!i_vertices.empty() && "Data bus cannot be empty");

  VertexTypes firstType = i_vertices.front()->getType();
  assert(firstType != VertexTypes::subGraph && "Subgraph cannot be in the data bus");
  for (const auto& vertex : i_vertices) {
    assert(vertex->getType() == firstType && "All vertices in the bus must be of the same type");
  }
  tcb::span<VertexPtr> d_vertices = i_vertices;
}

GraphVertexDataBus::GraphVertexDataBus(
    tcb::span<VertexPtr> i_vertices,
    const GraphVertexDataBus &i_vertexDataBus) :
    GraphVertexBase(VertexTypes::dataBus, 
      i_vertexDataBus.d_baseGraph.lock()) {
  tcb::span<VertexPtr> d_vertices = i_vertices;
}

GraphVertexDataBus GraphVertexDataBus::slice(size_t startBit,
  size_t endBit) const {
  if (startBit >= endBit || endBit > d_vertices.size()) {
    throw std::out_of_range("Invalid slice range");
  }

  return GraphVertexDataBus(tcb::span<VertexPtr>(d_vertices.begin() + startBit,
        d_vertices.begin() + endBit), *this);
}

GraphVertexDataBus::VertexPtr
GraphVertexDataBus::operator[](size_t index) const {
  if (index >= d_vertices.size()) {
    throw std::out_of_range("Index out of range");
  }
  return d_vertices[index];
}

std::string GraphVertexDataBus::toVerilog(bool flag) const {
  std::ostringstream verilogCode;  // Поток для формирования итогового Verilog кода

  if (flag) {
    // Отдельная строка для каждой вершины
    for (const auto& vertex : d_vertices) {
        verilogCode << vertex->toVerilog() << "\n";  // Вызов метода toVerilog для каждой вершины
    }
  } else {
    // Определяем тип всех вершин (все они одного типа, см. конструктор)
    VertexTypes type = d_vertices.front()->getType();
    std::string busName = getName();
    size_t busWidth = d_vertices.size() - 1;

    switch (type) {
      case VertexTypes::constant: {
        verilogCode << "wire [" << busWidth << ":0] " << busName << ";\n";
        verilogCode << "assign " << busName << " = " << busWidth + 1 << "'b";
        for (const auto& vertex : d_vertices) {
          // Приведение типа для константных вершин
          auto* constantVertex = dynamic_cast<GraphVertexConstant*>(vertex);
          if (constantVertex) {
            verilogCode << constantVertex->getValue();
          } else {
            std::cerr << "Error: vertex is not a GraphVertexConstant" << std::endl;
          }
        }
        verilogCode << ";\n";
        break;
      }

      case VertexTypes::input:
      case VertexTypes::output: {
        verilogCode << VertexUtils::vertexTypeToVerilog(type) << " [" 
                    << busWidth << ":0] " << busName << ";\n";

        // Добавляем assign для output, если есть соединения
        if (type == VertexTypes::output) {
          for (size_t i = 0; i < d_vertices.size(); ++i) {
            if (!d_vertices[i]->getInConnections().empty()) {
              verilogCode << "assign " << busName << "[" << i << "] = " 
                          << d_vertices[i]->getInConnections().back()->getName() << ";\n";
            }
          }
        }
        break;
      }
    }
  }

  return verilogCode.str();  // Возвращаем сформированный Verilog код
}


DotReturn GraphVertexDataBus::toDOT() {
  DotReturn dot;
  for (size_t i = 0; i < d_vertices.size(); ++i) {
    DotReturn vertexDot = d_vertices[i]->toDOT();  // Получаем DOT-данные для вершины
    for (auto& [type, data] : vertexDot) {  
      data["index"] = std::to_string(i);  // Добавляем индекс в map
      dot.emplace_back(type, std::move(data));  // Перемещаем обновленные данные в результат
    }
  }
  return dot;
}

size_t GraphVertexDataBus::getWidth() const {
  return d_vertices.size(); // Пример реализации
}


// char GraphVertexDataBus::updateValue() {
//   // if (!d_vertices.empty()) {
//   //     char value = d_vertices.front()->updateValue();
//   //     for (size_t i = 1; i < d_vertices.size(); i++) {
//   //         if (d_vertices[i]->updateValue() != value) {
//   //             return 'x'; // неопределенное состояние
//   //         }
//   //     }
//   //     return value;
//   // }
//   // return '0'; // Значение по умолчанию
// }

// void GraphVertexDataBus::updateLevel(bool i_recalculate, std::string tab) {
//   // if (d_needUpdate && !i_recalculate) {
//   //     return;
//   // }
//   // d_level = 0;
//   // for (auto& vertex : d_vertices) {
//   //     vertex->updateLevel(i_recalculate, tab + "  ");
//   //     d_level = std::max(d_level, vertex->getLevel());
//   // }
//   // d_needUpdate = true;
// }
