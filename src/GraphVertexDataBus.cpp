#include <iostream>
#include <CircuitGenGraph/GraphVertex.hpp>
#include "easyloggingpp/easylogging++.h"
#include "span.hpp"
#include <cassert>

GraphVertexDataBus::GraphVertexDataBus(
    tcb::span<VertexPtr> i_vertices, GraphPtr i_baseGraph,
    const VertexTypes i_type = VertexTypes::dataBus) :
    GraphVertexBase(i_type, i_baseGraph) {
  assert(!i_vertices.empty() && "Data bus cannot be empty");

  VertexTypes firstType = i_vertices.front()->getType();
  for (const auto& vertex : i_vertices) {
    assert(vertex->getType() == firstType && "All vertices in the bus must be of the same type");
    assert(vertex->getType() != VertexTypes::subGraph && "Subgraph cannot be in the data bus");
  }
  tcb::span<VertexPtr> d_vertices = i_vertices;
}

GraphVertexDataBus::GraphVertexDataBus(
    tcb::span<VertexPtr> i_vertices, std::string_view i_name,
    GraphPtr i_baseGraph, const VertexTypes i_type = VertexTypes::dataBus) :
    GraphVertexBase(i_type, i_name, i_baseGraph) {
  assert(!i_vertices.empty() && "Data bus cannot be empty");

  VertexTypes firstType = i_vertices.front()->getType();
  for (const auto& vertex : i_vertices) {
    assert(vertex->getType() == firstType && "All vertices in the bus must be of the same type");
    assert(vertex->getType() != VertexTypes::subGraph && "Subgraph cannot be in the data bus");
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
    // Если flag == false, генерируем одну строку для всех вершин
    verilogCode << "assign ";  // Начинаем с присваивания

    bool first = true;  // Флаг, чтобы не добавлять запятую перед первым элементом
    for (size_t i = 0; i < d_vertices.size(); ++i) {
      if (!first) verilogCode << ", ";  // Ставим запятую, если это не первая вершина
      verilogCode << d_vertices[i]->toVerilog();  // Используем метод toVerilog для вершины
      first = false;
    }
    verilogCode << ";\n";  // Закрываем строку присваивания
  }

  return verilogCode.str();  // Возвращаем сформированный Verilog код
}

DotReturn GraphVertexDataBus::toDOT() {
  DotReturn dot;  // Коллекция для хранения всех элементов графа в формате DOT

  // Для каждой вершины вызываем её метод toDOT
  for (size_t i = 0; i < d_vertices.size(); ++i) {
    // Вызов метода toDOT для каждой вершины
    DotReturn vertexDot = d_vertices[i]->toDOT();  // Получаем DOT-данные для вершины

    // Добавляем информацию о вершине в общий результат (если требуется дополнительная информация)
    for (auto& v : vertexDot) {
      v["index"] = std::to_string(i);  // Добавляем индекс вершины в данные (если нужно)
      dot.push_back(v);  // Добавляем данные вершины в результат
    }
  }

  return dot;  // Возвращаем собранные данные в формате DOT
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
