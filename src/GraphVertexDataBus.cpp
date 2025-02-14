#include <iostream>
#include <CircuitGenGraph/GraphVertex.hpp>
#include "easyloggingpp/easylogging++.h"
#include "span.hpp"

GraphVertexDataBus::GraphVertexDataBus(tcb::span<VertexPtr> i_vertices, GraphPtr i_baseGraph,
    const VertexTypes i_type = VertexTypes::dataBus) :
GraphVertexBase(i_type, i_baseGraph) {
    tcb::span<VertexPtr> d_vertices = i_vertices;
}
GraphVertexDataBus::GraphVertexDataBus(tcb::span<VertexPtr> i_vertices, std::string_view i_name,
    GraphPtr i_baseGraph,
    const VertexTypes i_type = VertexTypes::dataBus) :
GraphVertexBase(i_type, i_name, i_baseGraph) {
    tcb::span<VertexPtr> d_vertices = i_vertices;
}
GraphVertexDataBus::GraphVertexDataBus(tcb::span<VertexPtr> i_vertices, const GraphVertexDataBus& i_vertexDataBus) :
GraphVertexBase(VertexTypes::dataBus, i_vertexDataBus.d_name, i_vertexDataBus.d_baseGraph.lock()) {
    tcb::span<VertexPtr> d_vertices = i_vertices;
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

GraphVertexDataBus GraphVertexDataBus::slice(size_t startBit, size_t endBit) const {
    if (startBit >= endBit || endBit > d_vertices.size()) {
        throw std::out_of_range("Invalid slice range");
    }
    
    return GraphVertexDataBus(
        tcb::span<VertexPtr>(d_vertices.begin() + startBit, d_vertices.begin() + endBit),
        *this
    );
}

GraphVertexDataBus::VertexPtr GraphVertexDataBus::operator[](size_t index) const {
    if (index >= d_vertices.size()) {
        throw std::out_of_range("Index out of range");
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
