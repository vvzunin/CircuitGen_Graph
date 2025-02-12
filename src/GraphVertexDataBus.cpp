#include <iostream>

#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/span.hpp>
#include "easyloggingpp/easylogging++.h"
#include <CircuitGenGraph/span.hpp>

GraphVertexDataBus::GraphVertexDataBus(span<VertexPtr> i_vertices, GraphPtr i_baseGraph,
    const VertexTypes i_type) :
GraphVertexBase(i_type, i_baseGraph) {
    span<VertexPtr> d_vertices = span<VertexPtr> i_vertices;
    GraphMemory& d_memory();
}
GraphVertexInput::GraphVertexInput(span<VertexPtr> i_vertices, std::string_view i_name,
    GraphPtr i_baseGraph,
    const VertexTypes i_type) :
GraphVertexBase(i_type, i_name, i_baseGraph) {
    span<VertexPtr> d_vertices = span<VertexPtr> i_vertices;
    GraphMemory& d_memory();
}
char GraphVertexDataBus::updateValue() {
    // if (!d_vertices.empty()) {
    //     char value = d_vertices.front()->updateValue();
    //     for (size_t i = 1; i < d_vertices.size(); i++) {
    //         if (d_vertices[i]->updateValue() != value) {
    //             return 'x'; // неопределенное состояние
    //         }
    //     }
    //     return value;
    // }
    // return '0'; // Значение по умолчанию
}

void GraphVertexDataBus::updateLevel(bool i_recalculate, std::string tab) {
    // if (d_needUpdate && !i_recalculate) {
    //     return;
    // }
    // d_level = 0;
    // for (auto& vertex : d_vertices) {
    //     vertex->updateLevel(i_recalculate, tab + "  ");
    //     d_level = std::max(d_level, vertex->getLevel());
    // }
    // d_needUpdate = true;
}

GraphVertexDataBus GraphVertexDataBus::slice(size_t startVertex, size_t endVertex) const {
    if (startVertex >= d_vertices.size() || endVertex >= d_vertices.size() || startVertex > endVertex) {
        throw std::out_of_range("Slice indices out of range in GraphVertexDataBus");
    }
    GraphVertexDataBus slicedBus(baseGraph(), VertexTypes::dataBus);
    size_t newSize = endVertex - startVertex + 1;
    slicedBus.d_vertices = d_memory.allocateSpan<VertexPtr>(newSize);
    std::copy(d_vertices.begin() + startVertex, d_vertices.begin() + endVertex + 1, slicedBus.d_vertices.begin());
    return slicedBus;
}

GraphVertexDataBus::VertexPtr GraphVertexDataBus::operator[](size_t index) const {
    if (index >= d_vertices.size()) {
        throw std::out_of_range("Index out of range in GraphVertexDataBus");
    }
    return d_vertices[index];
}

DotReturn GraphVertexDataBus::toDOT() {
    // DotReturn dot;
    // for (size_t i = 0; i < d_vertices.size(); ++i) {
    //     dot.push_back({DotTypes::DotBus,
    //                    {"index", std::to_string(i)},
    //                     {"value", std::to_string(d_vertices[i]->updateValue())}});
    // }
    // return dot;
}
