#include <CircuitGenGraph/GraphVertex.hpp>

#include <cassert>
#include <iostream>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

#include "span.hpp"

namespace CG_Graph {

GraphVertexDataBus::GraphVertexDataBus(tcb::span<VertexPtr> i_vertices,
                                       GraphPtr i_baseGraph)
    : GraphVertexBase(VertexTypes::dataBus, i_baseGraph),
      d_vertices(i_vertices.begin(), i_vertices.end()) {
  assert(!i_vertices.empty() && "Data bus cannot be empty");

  VertexTypes firstType = i_vertices.front()->getType();
  assert(firstType != VertexTypes::subGraph && "Subgraph cannot be in the data bus");
  for (const auto &vertex: i_vertices) {
    assert(vertex->getType() == firstType && "All vertices in the bus must be of the same type");
  }
}

GraphVertexDataBus::GraphVertexDataBus(tcb::span<VertexPtr> i_vertices,
                                       std::string_view i_name,
                                       GraphPtr i_baseGraph)
    : GraphVertexBase(VertexTypes::dataBus, i_name, i_baseGraph),
      d_vertices(i_vertices.begin(), i_vertices.end()) {
  assert(!i_vertices.empty() && "Data bus cannot be empty");

  VertexTypes firstType = i_vertices.front()->getType();
  assert(firstType != VertexTypes::subGraph && "Subgraph cannot be in the data bus");
  for (const auto &vertex: i_vertices) {
    assert(vertex->getType() == firstType && "All vertices in the bus must be of the same type");
  }
}

GraphVertexDataBus::GraphVertexDataBus(tcb::span<VertexPtr> i_vertices,
                                       const GraphVertexDataBus &i_vertexDataBus)
    : GraphVertexBase(VertexTypes::dataBus, i_vertexDataBus.d_baseGraph.lock()),
      d_vertices(i_vertices.begin(), i_vertices.end()) {
}

GraphVertexDataBus GraphVertexDataBus::slice(size_t startBit,
                                             size_t endBit) const {
  if (startBit >= endBit || endBit > d_vertices.size()) {
    throw std::out_of_range("Invalid slice range");
  }

  return GraphVertexDataBus(tcb::span<VertexPtr>(d_vertices.begin() + startBit,
                                                 d_vertices.begin() + endBit),
                            *this);
}

VertexPtr GraphVertexDataBus::operator[](size_t index) const {
  if (index >= d_vertices.size()) {
    throw std::out_of_range("Index out of range");
  }
  return d_vertices[index];
}

std::string GraphVertexDataBus::toVerilog() const {
  return toVerilog(true);
}

std::string GraphVertexDataBus::toVerilog(bool flag) const {
  std::ostringstream
      verilogCode; // Поток для формирования итогового Verilog кода

  if (flag) {
    // Отдельная строка для каждой вершины
    for (const auto &vertex: d_vertices) {
      verilogCode << vertex->toVerilog()
                  << "\n"; // Вызов метода toVerilog для каждой вершины
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
        for (const auto &vertex: d_vertices) {
          // Приведение типа для константных вершин
          auto *constantVertex = dynamic_cast<GraphVertexConstant *>(vertex);
          if (constantVertex) {
            verilogCode << constantVertex->getValue();
          } else {
            std::cerr << "Error: vertex is not a GraphVertexConstant"
                      << std::endl;
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
                          << d_vertices[i]->getInConnections().back()->getName()
                          << ";\n";
            }
          }
        }
        break;
      }

      case VertexTypes::sequential: {
        const std::string busName = getName();
        const size_t width = d_vertices.size();
        // const VertexTypes type = d_vertices.front()->getType();
        std::ostringstream verilog;
        // Получаем первый элемент с правильным приведением типа
        const GraphVertexSequential *firstSeq =
            static_cast<const GraphVertexSequential *>(d_vertices.front());
        SequentialTypes seqType = firstSeq->getSeqType();

        verilog << "reg [" << (width - 1) << ":0] " << busName << ";\n";

        // Always block
        if ((seqType & ff) && !(seqType & ASYNC)) {
          verilog << "always @("
                  << ((seqType & NEGEDGE) ? "negedge" : "posedge") << " "
                  << firstSeq->getClk()->getRawName() << ") begin\n";
        } else if (seqType & ff) {
          verilog << "always @("
                  << ((seqType & NEGEDGE) ? "negedge" : "posedge") << " "
                  << firstSeq->getClk()->getRawName() << " or negedge "
                  << firstSeq->getRst()->getRawName() << ") begin\n";
        } else {
          verilog << "always @(*) begin\n";
        }

        std::string_view tab = "\t\t";
        bool hasControl = false;

        // RST/CLR
        if (unsigned val = (seqType & RST) | (seqType & CLR)) {
          verilog << tab << "if (" << (val & RST ? "!" : "")
                  << firstSeq->getRst()->getRawName() << ") " << busName
                  << " <= " << width << "'b0;\n";
          verilog << tab << "else";
          hasControl = true;
        }

        // SET
        if (seqType & SET) {
          verilog << (hasControl ? " " : tab) << "if ("
                  << firstSeq->getSet()->getRawName() << ") " << busName
                  << " <= " << width << "'b" << std::string(width, '1') << ";\n"
                  << tab << "else";
          hasControl = true;
        }

        // EN
        verilog << (hasControl ? " " : tab);
        if (seqType & EN) {
          verilog << "if (" << firstSeq->getEn()->getRawName() << ") ";
        }

        // Data assignment
        verilog << busName << " <= {";
        for (int i = width - 1; i >= 0; --i) {
          const GraphVertexSequential *seq =
              static_cast<const GraphVertexSequential *>(d_vertices[i]);
          verilog << seq->getData()->getRawName();
          if (i > 0)
            verilog << ", ";
        }
        verilog << "};\nend\n";
        break;
      }
    }
  }

  return verilogCode.str(); // Возвращаем сформированный Verilog код
}

DotReturn GraphVertexDataBus::toDOT() {
  DotReturn dot;
  for (size_t i = 0; i < d_vertices.size(); ++i) {
    DotReturn vertexDot =
        d_vertices[i]->toDOT(); // Получаем DOT-данные для вершины
    for (auto &[type, data]: vertexDot) {
      data["index"] = std::to_string(i); // Добавляем индекс в map
      dot.emplace_back(
          type, std::move(data)); // Перемещаем обновленные данные в результат
    }
  }
  return dot;
}

size_t GraphVertexDataBus::getWidth() const {
  return d_vertices.size(); // Пример реализации
}

} // namespace CG_Graph
