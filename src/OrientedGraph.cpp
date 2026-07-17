/**
 * @file OrientedGraph.cpp
 * @brief Реализация ориентированного графа схемы.
 */
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "CircuitGenGraph/GraphUtils.hpp"
#include "CircuitGenGraph/GraphVertexBus.hpp"
#include <CircuitGenGraph/DefaultAuxiliaryMethods.hpp>
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>

#include <CircuitGenGraph/Logging.hpp>

#include "CircuitGenGraph/GraphUtils.hpp"
#include "GraphMLTemplates.hpp"

#include <lorina/lorina.hpp>
#ifdef LOGFLAG
INITIALIZE_EASYLOGGINGPP
#endif

namespace CG_Graph {

std::atomic_size_t OrientedGraph::d_countGraph = 0;
std::atomic_size_t OrientedGraph::d_countNewGraphInstance = 0;

bool CompareLevels(const VertexPtr left, const VertexPtr right) {
  return left->getLevel() < right->getLevel();
}

void OrientedGraph::initLogging(const std::string &i_configPath) {
#ifdef LOGFLAG
  el::Configurations conf(i_configPath);
  el::Loggers::reconfigureAllLoggers(conf);
  el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
  CG_LOG_INFO << "Logging initialized from " << i_configPath;
#endif
}

OrientedGraph::OrientedGraph(const std::string &i_name, size_t buffer_size,
                             size_t chunk_size) :
    GraphMemory(buffer_size, chunk_size) {
  d_graphID = d_countNewGraphInstance++;

  if (i_name == "")
    d_name = "graph_" + std::to_string(d_countGraph++);
  else
    d_name = i_name;

  CG_LOG_INFO << "Creating OrientedGraph '" << d_name << "' (ID: " << d_graphID
              << ")";

  // filling edges
  for (auto cur_gate: GraphUtils::getLogicOperationsKeys()) {
    d_edgesGatesCount[cur_gate] = d_gatesCount;
  }

  d_vertices = {std::vector<VertexPtr>(), std::vector<VertexPtr>(),
                std::vector<VertexPtr>(), std::vector<VertexPtr>(),
                std::vector<VertexPtr>()};
}

OrientedGraph::~OrientedGraph() {
  CG_LOG_INFO << "Destroying OrientedGraph '" << d_name
              << "' (ID: " << d_graphID << ")";
  for (auto sub: d_subGraphs) {
    sub->d_currentParentGraph.lock() = nullptr;
  }

  for (auto &verticesVector: d_vertices) {
    for (auto *vertex: verticesVector) {
      vertex->~GraphVertexBase();
    }
  }

  for (auto *vertex: d_allSubGraphsOutputs) {
    vertex->~GraphVertexBase();
  }
}

size_t OrientedGraph::baseSize() const {
  return d_vertices.at(VertexTypes::gate).size();
}

size_t OrientedGraph::fullSize() const {
  size_t size = this->baseSize();
  for (GraphPtr vert: d_subGraphs)
    size += vert->fullSize();
  return size;
}

bool OrientedGraph::isEmpty() const {
  return this->fullSize() == 0;
}

bool OrientedGraph::isEmptyFull() const {
  bool f = true;
  for (const auto &value: d_vertices)
    f &= value.size() == 0;
  if (!f)
    return f;

  for (GraphPtr vert: d_subGraphs)
    f &= vert->isEmptyFull();
  return f;
}

void OrientedGraph::setName(const std::string &i_name) {
  d_name = i_name;
}

std::string OrientedGraph::getName() const {
  return d_name;
}

std::string_view OrientedGraph::getRawName() const {
  return d_name;
}

bool OrientedGraph::needToUpdateLevel() const {
  return d_needLevelUpdate;
}

void OrientedGraph::updateLevels() {
  CG_VLOG(1) << "Starting level update for graph '" << d_name << "'.";
  CG_VLOG(1) << "Outputs for update: "
             << d_vertices.at(VertexTypes::output).size();
  int counter = 0;
  for (VertexPtr vert: d_vertices.at(VertexTypes::output)) {
    CG_VLOG(2) << counter++ << ". " << vert->getRawName() << " ("
               << vert->getTypeName() << ")";
    vert->updateLevel();
  }
}

uint32_t OrientedGraph::getMaxLevel() {
  updateLevels();
  uint32_t mx = 0;
  for (VertexPtr vert: d_vertices.at(VertexTypes::output)) {
    mx = std::max(mx, vert->getLevel());
  }
  return mx;
}

#define SIMPLE_VERT_ITER(vertices, methodName) \
  for (auto &vec: vertices) \
    for (auto *vertex: vec) \
  vertex->methodName()

void OrientedGraph::clearHashStates() {
  SIMPLE_VERT_ITER(d_vertices, resetHashState);
  d_hashState = HC_NOT_CALC;
}

void OrientedGraph::clearNeedUpdateStates() {
  SIMPLE_VERT_ITER(d_vertices, resetNeedUpdateState);
}

void OrientedGraph::clearUsedLevelStates() {
  SIMPLE_VERT_ITER(d_vertices, resetUsedLevelState);
}

void OrientedGraph::clearAllStates() {
  SIMPLE_VERT_ITER(d_vertices, resetAllStates);
  d_hashState = HC_NOT_CALC;
}

#undef SIMPLE_VERT_ITER

VertexPtr OrientedGraph::addInput(const std::string &i_name) {
  VertexPtr newVertex = create<GraphVertexInput>(
      i_name.empty() ? "" : internalize(i_name), shared_from_this());
  d_vertices[VertexTypes::input].push_back(newVertex);

  return newVertex;
}

std::vector<VertexPtr> OrientedGraph::addInputs(size_t i_num) {
  std::vector<VertexPtr> inputs;
  inputs.reserve(i_num);
  reserve(VertexTypes::input, i_num);
  for (size_t i = 0; i < i_num; ++i) {
    VertexPtr ptr = create<GraphVertexInput>(shared_from_this());
    inputs.push_back(ptr);
    d_vertices[VertexTypes::input].push_back(ptr);
  }
  return inputs;
}

std::vector<VertexPtr> OrientedGraph::addInputs(size_t i_num,
                                                NameGenerator generator) {
  std::vector<VertexPtr> inputs;
  inputs.reserve(i_num);
  reserve(VertexTypes::input, i_num);
  for (size_t i = 0; i < i_num; ++i) {
    VertexPtr ptr =
        create<GraphVertexInput>(internalize(generator(i)), shared_from_this());
    inputs.push_back(ptr);
    d_vertices[VertexTypes::input].push_back(ptr);
  }
  return inputs;
}

VertexPtr OrientedGraph::addInputBus(const std::string &i_name, size_t width) {
  VertexPtr newVertex = create<GraphVertexBusInput>(
      i_name.empty() ? "" : internalize(i_name), shared_from_this(), width);
  d_vertices[VertexTypes::input].push_back(newVertex);

  return newVertex;
}

VertexPtr OrientedGraph::addOutput(const std::string &i_name) {
  VertexPtr newVertex = create<GraphVertexOutput>(
      i_name.empty() ? "" : internalize(i_name), shared_from_this());
  d_vertices[VertexTypes::output].push_back(newVertex);

  return newVertex;
}

std::vector<VertexPtr> OrientedGraph::addOutputs(size_t i_num) {
  std::vector<VertexPtr> outputs;
  outputs.reserve(i_num);
  reserve(VertexTypes::output, i_num);
  for (size_t i = 0; i < i_num; ++i) {
    VertexPtr ptr = create<GraphVertexOutput>(shared_from_this());
    outputs.push_back(ptr);
    d_vertices[VertexTypes::output].push_back(ptr);
  }
  return outputs;
}

std::vector<VertexPtr>
OrientedGraph::addOutputs(const std::vector<VertexPtr> &gates) {
  std::vector<VertexPtr> outputs;
  outputs.reserve(gates.size());
  reserve(VertexTypes::output, gates.size());
  for (size_t i = 0; i < gates.size(); ++i) {
    VertexPtr ptr = create<GraphVertexOutput>(shared_from_this());
    outputs.push_back(ptr);
    d_vertices[VertexTypes::output].push_back(ptr);
    addEdge(gates[i], ptr);
  }
  return outputs;
}

VertexPtr OrientedGraph::addOutputBus(const std::string &i_name, size_t width) {
  VertexPtr newVertex = create<GraphVertexBusOutput>(
      i_name.empty() ? "" : internalize(i_name), shared_from_this(), width);
  d_vertices[VertexTypes::output].push_back(newVertex);

  return newVertex;
}

VertexPtr OrientedGraph::addConst(const char &i_value,
                                  const std::string &i_name) {
  auto name = i_name.empty() ? "" : internalize(i_name);
  VertexPtr newVertex =
      create<GraphVertexConstant>(i_value, name, shared_from_this());
  d_vertices[VertexTypes::constant].push_back(newVertex);

  return newVertex;
}

VertexPtr OrientedGraph::addConstBus(const std::string &i_name, size_t width) {
  VertexPtr newVertex = create<GraphVertexBusConstant>(
      i_name.empty() ? "" : internalize(i_name), shared_from_this(), width);
  d_vertices[VertexTypes::constant].push_back(newVertex);

  return newVertex;
}
VertexPtr OrientedGraph::addGate(const Gates &i_gate,
                                 const std::string &i_name) {
  VertexPtr newVertex = create<GraphVertexGates>(
      i_gate, i_name.empty() ? "" : internalize(i_name), shared_from_this());
  d_vertices[VertexTypes::gate].push_back(newVertex);

  ++d_gatesCount[i_gate];

  return newVertex;
}

VertexPtr OrientedGraph::addGateBus(const Gates &i_gate,
                                    const std::string &i_name, size_t i_width) {
  VertexPtr newVertex = create<GraphVertexBusGate>(
      i_gate, i_name.empty() ? "" : internalize(i_name), shared_from_this(),
      i_width);
  d_vertices[VertexTypes::gate].push_back(newVertex);
  ++d_gatesCount[i_gate];
  return newVertex;
}
VertexPtr OrientedGraph::addSliceBus(VertexPtr i_bus, size_t i_begin,
                                     size_t i_width,
                                     const std::string &i_name) {
  VertexPtr newVertex;
  size_t correctWidth = i_width;
  size_t correctBegin = i_begin;
  if (!i_bus->isBus()) {
    std::cerr << "Created slice with name " +
                     (i_name.empty() ? "(name is not defined)" : i_name)
              << " is connected with vertex, which is not a bus\n";
    correctWidth = 1;
    correctBegin = 0;
  } else if (i_width == 0) {
    std::cerr << "Width of bus must be an positive value\n";
    correctWidth = 1;
  } else if (i_bus->isBus() &&
             i_begin + i_width >
                 GraphVertexBus::getBusPointer(i_bus)->getWidth()) {
    std::cerr << "Width of slice is out of range of bus\n";
    correctWidth = GraphVertexBus::getBusPointer(i_bus)->getWidth() - i_begin;
  }
  newVertex = create<GraphVertexBusSlice>(
      i_name.empty() ? "" : internalize(i_name), shared_from_this(),
      correctBegin, correctWidth);
  d_vertices[gate].push_back(newVertex);
  ++d_gatesCount[GateSlice];

  addEdge(i_bus, newVertex);
  return newVertex;
}
VertexPtr OrientedGraph::addSequential(const SequentialTypes &i_type,
                                       VertexPtr i_clk, VertexPtr i_data,
                                       const std::string &i_name) {
  auto name = i_name.empty() ? "" : internalize(i_name);
  VertexPtr newVertex = create<GraphVertexSequential>(i_type, i_clk, i_data,
                                                      shared_from_this(), name);
  d_vertices[VertexTypes::sequential].push_back(newVertex);
  return newVertex;
}

VertexPtr OrientedGraph::addSequentialBus(const SequentialTypes &i_type,
                                          VertexPtr i_clk, VertexPtr i_data,
                                          const std::string &i_name,
                                          size_t i_width) {
  auto name = i_name.empty() ? "" : internalize(i_name);
  VertexPtr newVertex = create<GraphVertexBusSequential>(
      i_type, i_clk, i_data, shared_from_this(), name, i_width);
  d_vertices[VertexTypes::sequential].push_back(newVertex);
  return newVertex;
}
VertexPtr OrientedGraph::addSequential(const SequentialTypes &i_type,
                                       VertexPtr i_clk, VertexPtr i_data,
                                       VertexPtr i_wire,
                                       const std::string &i_name) {
  auto name = i_name.empty() ? "" : internalize(i_name);
  VertexPtr newVertex = create<GraphVertexSequential>(
      i_type, i_clk, i_data, i_wire, shared_from_this(), name);
  d_vertices[VertexTypes::sequential].push_back(newVertex);
  return newVertex;
}
VertexPtr OrientedGraph::addSequentialBus(const SequentialTypes &i_type,
                                          VertexPtr i_clk, VertexPtr i_data,
                                          VertexPtr i_wire,
                                          const std::string &i_name,
                                          size_t i_width) {
  auto name = i_name.empty() ? "" : internalize(i_name);
  VertexPtr newVertex = create<GraphVertexBusSequential>(
      i_type, i_clk, i_data, i_wire, shared_from_this(), name, i_width);
  d_vertices[VertexTypes::sequential].push_back(newVertex);

  newVertex->reserveInConnections(3ul);
  return newVertex;
}
VertexPtr OrientedGraph::addSequential(const SequentialTypes &i_type,
                                       VertexPtr i_clk, VertexPtr i_data,
                                       VertexPtr i_wire1, VertexPtr i_wire2,
                                       const std::string &i_name) {
  auto name = i_name.empty() ? "" : internalize(i_name);
  VertexPtr newVertex = create<GraphVertexSequential>(
      i_type, i_clk, i_data, i_wire1, i_wire2, shared_from_this(), name);
  d_vertices[VertexTypes::sequential].push_back(newVertex);
  return newVertex;
}

VertexPtr OrientedGraph::addSequentialBus(const SequentialTypes &i_type,
                                          VertexPtr i_clk, VertexPtr i_data,
                                          VertexPtr i_wire1, VertexPtr i_wire2,
                                          const std::string &i_name,
                                          size_t i_width) {
  auto name = i_name.empty() ? "" : internalize(i_name);
  VertexPtr newVertex =
      create<GraphVertexBusSequential>(i_type, i_clk, i_data, i_wire1, i_wire2,
                                       shared_from_this(), name, i_width);
  d_vertices[VertexTypes::sequential].push_back(newVertex);
  return newVertex;
}
VertexPtr OrientedGraph::addSequential(const SequentialTypes &i_type,
                                       VertexPtr i_clk, VertexPtr i_data,
                                       VertexPtr i_rst, VertexPtr i_set,
                                       VertexPtr i_en,
                                       const std::string &i_name) {
  auto name = i_name.empty() ? "" : internalize(i_name);
  VertexPtr newVertex = create<GraphVertexSequential>(
      i_type, i_clk, i_data, i_rst, i_set, i_en, shared_from_this(), name);
  d_vertices[VertexTypes::sequential].push_back(newVertex);
  return newVertex;
}

VertexPtr OrientedGraph::addSequentialBus(const SequentialTypes &i_type,
                                          VertexPtr i_clk, VertexPtr i_data,
                                          VertexPtr i_rst, VertexPtr i_set,
                                          VertexPtr i_en,
                                          const std::string &i_name,
                                          size_t i_width) {
  auto name = i_name.empty() ? "" : internalize(i_name);
  VertexPtr newVertex =
      create<GraphVertexBusSequential>(i_type, i_clk, i_data, i_rst, i_set,
                                       i_en, shared_from_this(), name, i_width);
  d_vertices[VertexTypes::sequential].push_back(newVertex);
  return newVertex;
}
std::vector<VertexPtr>
OrientedGraph::addSubGraph(GraphPtr i_subGraph,
                           std::vector<VertexPtr> i_inputs) {
  std::vector<VertexPtr> iGraph =
      i_subGraph->getVerticesByType(VertexTypes::input);

  i_subGraph->setCurrentParent(shared_from_this());
  i_subGraph->d_isSubGraph = true;

  if (i_inputs.size() != iGraph.size()) {
    throw std::invalid_argument(
        "Number of inputs should be same, as subgraph inputs number");
  }

  std::vector<VertexPtr> outputs;

  VertexPtr newGraph =
      create<GraphVertexSubGraph>(i_subGraph, shared_from_this());
  d_vertices[VertexTypes::subGraph].push_back(newGraph);

  // adding edges for subGraphs
  addEdges(i_inputs, newGraph);

  size_t outSize = i_subGraph->getVerticesByType(VertexTypes::output).size();

  if (outSize > 0) {
    newGraph->reserveOutConnections(outSize);
    for (size_t i = 0; i < outSize; ++i) {
      VertexPtr newVertex =
          create<GraphVertexGates>(Gates::GateBuf, shared_from_this());

      outputs.push_back(newVertex);
      d_allSubGraphsOutputs.push_back(newVertex);

      addEdge(newGraph, newVertex);
    }
  } else {
    CG_LOG_ERROR << "Error, SubGraph without outputs" << std::endl;
  }

  // here we use i_subGraph like an instance of BasicType,
  // and we call it's toVerilog, having in multiple instance
  // of one i_subGraph, so we can have many times "moduleName name (inp, out);"
  // having different names of module, inputs and outputs
  d_subGraphs.insert(i_subGraph);

  return outputs;
}

std::vector<char>
OrientedGraph::graphSimulation(std::vector<char> inputsValues) {
  std::vector<char> outputsValues;
  // Force recompute of multi-level cones. Vertices are constructed with
  // NoSignal ('x'), while updateValue only recurses on UndefinedState ('n').
  for (VertexPtr v: d_vertices[VertexTypes::gate])
    v->invalidateValue();
  for (VertexPtr v: d_vertices[VertexTypes::output])
    v->invalidateValue();
  for (VertexPtr v: d_vertices[VertexTypes::subGraph])
    v->invalidateValue();
  // Do not invalidate sequential cells: their d_value / d_prevClk are state
  // that must persist across graphSimulation vectors until simulationRemove.

  for (size_t i = 0; i < d_vertices[VertexTypes::input].size(); ++i) {
    GraphVertexInput *inputVert =
        static_cast<GraphVertexInput *>(d_vertices[VertexTypes::input].at(i));
    inputVert->setValue(inputsValues.at(i));
  }
  // Gates only recurse into inputs that are UndefinedState ('n'). Sequential
  // cells keep a defined Q between vectors, so a path like FF → GateBuf → out
  // would otherwise skip updateValue and freeze the flop. Tick sequentials
  // explicitly after inputs are applied (pulling any combinational D cones).
  for (VertexPtr seqVert: d_vertices[VertexTypes::sequential]) {
    seqVert->updateValue();
  }
  for (VertexPtr outputVert: d_vertices[VertexTypes::output]) {
    outputsValues.push_back(outputVert->updateValue());
  }
  return outputsValues;
}

void OrientedGraph::simulationRemove() {
  for (VertexPtr ptr: d_vertices[VertexTypes::output]) {
    ptr->removeValue();
  }
}

void OrientedGraph::updateEdgesGatesCount(VertexPtr vertex, Gates type) {
  if (type == GateDefault) {
    for (auto *i: vertex->getOutConnections())
      if (i->getType() == gate)
        --d_edgesGatesCount[type][i->getGate()];
    --d_gatesCount[GateDefault];
  } else {
    assert(vertex->getGate() == GateDefault);
    --d_gatesCount[GateDefault];
    ++d_gatesCount[type];
    for (auto *i: vertex->getInConnections())
      if (i->getType() == gate)
        ++d_edgesGatesCount[i->getGate()][type];
    for (auto *i: vertex->getOutConnections())
      if (i->getType() == gate)
        ++d_edgesGatesCount[type][i->getGate()];
  }
}

size_t OrientedGraph::removeEmptyLogicVertices() {
  size_t removed = 0;
  auto scrub = [this, &removed](VertexTypes type) {
    auto &bucket = d_vertices[type];
    size_t write = 0;
    for (size_t i = 0; i < bucket.size(); ++i) {
      VertexPtr vert = bucket[i];
      if (!vert->getInConnections().empty()) {
        bucket[write++] = vert;
        continue;
      }

      // Copy: removeEdge mutates the adjacency vectors.
      const std::vector<VertexPtr> outs = vert->getOutConnections();
      for (VertexPtr consumer: outs)
        removeEdge(vert, consumer);

      if (type == gate)
        d_gatesCount[vert->getGate()] -= 1;

      CG_LOG_WARNING << "Removing empty "
                     << GraphUtils::parseVertexToString(type) << " vertex '"
                     << vert->getRawName() << "'";
      vert->~GraphVertexBase();
      ++removed;
    }
    bucket.resize(write);
  };

  scrub(gate);
  scrub(sequential);
  return removed;
}

void OrientedGraph::removeWasteVertices() {
  removeEmptyLogicVertices();
  updateLevels();
  auto removingForType = [this](VertexTypes type) {
    uint8_t counterForResize = 0;
    for (auto iter = d_vertices[type].begin();
         iter != d_vertices[type].end() - counterForResize;) {
      VertexPtr vert = *iter;
      if (!vert->getLevel()) {
        if (vert->getType() != input && vert->getType() != constant) {
          // Copy: removeEdge mutates adjacency; cannot iterate the live vector.
          const std::vector<VertexPtr> inConns = vert->getInConnections();
          for (auto *inConnVert: inConns) {
            if (inConnVert->getLevel() != 0 ||
                inConnVert->getType() == constant ||
                inConnVert->getType() == input) {
              removeEdge(inConnVert, vert);
            } else if (inConnVert->getLevel() == 0) {
              this->d_edgesCount -= 1;
              if (type == gate && inConnVert->getType() == gate)
                this->d_edgesGatesCount[inConnVert->getGate()]
                                       [vert->getGate()] -= 1;
            }
          }
        }
        if (vert->getOutConnections().empty() ||
            !(vert->getType() == constant || vert->getType() == input)) {
          if (type == gate) {
            this->d_gatesCount[vert->getGate()] -= 1;
          }
          // IMPORTANT. do not use rbegin
          std::swap(*iter, *(d_vertices[type].end() - 1 - counterForResize));
          vert->~GraphVertexBase();
          ++counterForResize;
        } else
          ++iter;
      } else
        ++iter;
    }
    if (counterForResize == d_vertices[type].size())
      d_vertices[type].clear();
    else if (counterForResize)
      d_vertices[type].resize(d_vertices[type].size() - counterForResize);
  };
  removingForType(gate);
  removingForType(sequential);
  removingForType(subGraph);
  removingForType(input);
  removingForType(constant);
}

GraphPtr OrientedGraph::createMajoritySubgraph() {
  auto majority = std::make_shared<CG_Graph::OrientedGraph>(
      "Majority3", 9 * sizeof(GraphVertexBase));

  VertexPtr in1 = majority->addInput("a");
  VertexPtr in2 = majority->addInput("b");
  VertexPtr in3 = majority->addInput("c");

  VertexPtr and_ab = majority->addGate(Gates::GateAnd, "and_ab");
  majority->addEdges({in1, in2}, and_ab);

  VertexPtr and_ac = majority->addGate(Gates::GateAnd, "and_ac");
  majority->addEdges({in1, in3}, and_ac);

  VertexPtr and_bc = majority->addGate(Gates::GateAnd, "and_bc");
  majority->addEdges({in2, in3}, and_bc);

  VertexPtr or1 = majority->addGate(Gates::GateOr, "or1");
  majority->addEdges({and_ab, and_ac}, or1);

  VertexPtr or2 = majority->addGate(Gates::GateOr, "or2");
  majority->addEdges({or1, and_bc}, or2);

  VertexPtr out = majority->addOutput("result");
  majority->addEdge(or2, out);

  return majority;
}

VertexPtr OrientedGraph::generateMajority(VertexPtr a, VertexPtr b,
                                          VertexPtr c) {
  static GraphPtr majoritySubgraph =
      createMajoritySubgraph(); // создается один раз

  std::vector<VertexPtr> outputs =
      this->addSubGraph(majoritySubgraph, {a, b, c});
  return outputs.back();
}
VertexPtr OrientedGraph::majorityAsLogic(VertexPtr a, VertexPtr b, VertexPtr c,
                                         VertexPtr output = nullptr) {
  VertexPtr and_ab = addGate(Gates::GateAnd);
  addEdges({a, b}, and_ab);

  VertexPtr and_ac = addGate(Gates::GateAnd);
  addEdges({a, c}, and_ac);

  VertexPtr and_bc = addGate(Gates::GateAnd);
  addEdges({b, c}, and_bc);

  VertexPtr or1 = addGate(Gates::GateOr);
  addEdges({and_ab, and_ac}, or1);

  VertexPtr or2;
  if (output != nullptr && output->getGate() == GateDefault) {
    static_cast<GraphVertexGates *>(output)->setGateIfDefault(GateOr);
    or2 = output;
  } else
    or2 = addGate(Gates::GateOr);
  addEdges({or1, and_bc}, or2);
  return or2;
}
bool OrientedGraph::addEdge(VertexPtr from, VertexPtr to) {
  CG_VLOG(2) << "Adding edge from " << (from ? from->getName() : "nullptr")
             << " to " << (to ? to->getName() : "nullptr");
  bool f;
  uint32_t n = 0;
  if (from && to && from->getBaseGraph().lock() == to->getBaseGraph().lock()) {
    // Out-side rejects duplicates; only then append the matching in-edge.
    // Otherwise a repeated addEdge would leave an orphan in-connection.
    f = from->addVertexToOutConnections(to);
    if (f)
      n = to->addVertexToInConnections(from);
  } else {
    CG_LOG_ERROR << "Attempted to add edge between different graphs/subgraphs "
                    "(or null pointers): "
                 << (from ? from->getName() : "nullptr") << " and "
                 << (to ? to->getName() : "nullptr");
    throw std::invalid_argument("Not allowed to add edge from one subgraph to "
                                "another (or null pointers)");
  }
  d_edgesCount += f && (n > 0);

  if (f && n > 0 && from->getType() == VertexTypes::gate &&
      to->getType() == VertexTypes::gate)
    ++d_edgesGatesCount[from->getGate()][to->getGate()];

  return f && (n > 0);
}

bool OrientedGraph::addEdges(std::vector<VertexPtr> from1, VertexPtr to) {
  bool f = true;
  to->reserveInConnections(from1.size());
  for (VertexPtr vert: from1)
    f &= this->addEdge(vert, to);
  return f;
}

bool OrientedGraph::removeEdge(VertexPtr from1, VertexPtr to) {
  bool deleted = false;
  deleted = from1->removeVertexToOutConnections(to);
  deleted = deleted && to->removeVertexToInConnections(from1);
  if (deleted) {
    d_edgesCount -= 1;
    if (from1->getType() == gate && to->getType() == gate) {
      d_edgesGatesCount[from1->getGate()][to->getGate()] -= 1;
    }
  }
  return deleted;
}

void OrientedGraph::readVerilog(std::string i_path, Context &context) {
  GraphReader *reader = new GraphReader(context);
  std::ifstream in(i_path.c_str(), std::ifstream::in);
  if (!in.is_open())
    throw std::runtime_error("File do not exist. Current path:" + i_path +
                             "\n");
  std::string word;
  in >> word;
  while (word != "module") {
    in.ignore(256, '\n');
    in >> word;
  }
  in.seekg(-6, std::ios::cur);
  // Redirect lorina diagnostics into CG_LOG_*.
  LogDiagnosticConsumer diagnosticConsumer;
  lorina::diagnostic_engine diag(&diagnosticConsumer);
  lorina::return_code returnCode = lorina::read_verilog(in, *reader, &diag);
  if (returnCode == lorina::return_code::parse_error)
    throw std::runtime_error("File is incorrect\n");
  in.close();
  delete reader;
}

CG_Graph::Context OrientedGraph::readVerilog(std::string i_path) {
  Context context;
  readVerilog(i_path, context);
  return context;
}

std::set<GraphPtr> OrientedGraph::getSubGraphs() const {
  return d_subGraphs;
}

std::array<std::vector<VertexPtr>, VertexTypes::output + 1>
OrientedGraph::getBaseVertexes() const {
  return d_vertices;
}

VertexPtr OrientedGraph::getVerticeByIndex(size_t idx) const {
  if (sumFullSize() <= idx)
    throw std::out_of_range("OrientedGraph getVerticeByIndex: invalid index");

  static auto types = {input, constant, gate, sequential, subGraph};
  for (const auto &type: types) {
    if (d_vertices.at(type).size() > idx)
      return d_vertices.at(type).at(idx);
    idx -= d_vertices.at(type).size();
  }
  // here output
  return d_vertices.at(VertexTypes::output).at(idx);
}

void OrientedGraph::clearVerilogParameters() {
  d_verilogParameters.clear();
}

bool OrientedGraph::addVerilogParameter(const std::string &i_name,
                                        const std::string &i_value) {
  if (i_name.empty()) {
    return false;
  }

  for (auto &parameter: d_verilogParameters) {
    if (parameter.first == i_name) {
      parameter.second = i_value;
      return true;
    }
  }

  d_verilogParameters.push_back({i_name, i_value});
  return true;
}

const std::vector<std::pair<std::string, std::string>> &
OrientedGraph::getVerilogParameters() const {
  return d_verilogParameters;
}

std::vector<VertexPtr> OrientedGraph::getVerticesByLevel(uint32_t i_level) {
  updateLevels();
  std::vector<VertexPtr> a;
  if (!i_level) {
    a.reserve(d_vertices[input].size() + d_vertices[constant].size());
    a.insert(a.end(), d_vertices[input].begin(), d_vertices[input].end());
    a.insert(a.end(), d_vertices[constant].begin(), d_vertices[constant].end());
  } else {
    // Reuse levels from updateLevels() above — do not call getMaxLevel(),
    // which would run updateLevels() a second time.
    uint32_t maxLevel = 0;
    for (VertexPtr vert: d_vertices[output]) {
      maxLevel = std::max(maxLevel, vert->getLevel());
    }
    // maxLevel / 2 <= i_level -> start from outputs (closer to target)
    // ( maxLevel / 2 <= i_level ) * 2 = maxLevel <= i_level << 1
    if (maxLevel <= (i_level << 1u)) {
      for (VertexPtr vertex: d_vertices[output]) {
        vertex->getVerticesByLevel(i_level, a);
      }
    } else {
      for (VertexPtr vertex: d_vertices[input]) {
        vertex->getVerticesByLevel(i_level, a, false);
      }
      for (VertexPtr vertex: d_vertices[constant]) {
        vertex->getVerticesByLevel(i_level, a, false);
      }
    }
    clearUsedLevelStates();
  }
  return a;
}

std::vector<VertexPtr>
OrientedGraph::getVerticesByType(const VertexTypes &i_type,
                                 std::string_view i_name,
                                 const bool &i_addSubGraphs) const {
  std::vector<VertexPtr> resVert;
  if (i_name.empty()) {
    resVert = d_vertices.at(i_type);
  } else {
    for (VertexPtr vert: d_vertices.at(i_type)) {
      if (vert->getRawName() == i_name)
        resVert.push_back(vert);
    }
  }

  if (i_addSubGraphs) {
    for (GraphPtr sub: d_subGraphs) {
      std::vector<VertexPtr> subResVert =
          sub->getVerticesByType(i_type, i_name, i_addSubGraphs);
      resVert.insert(resVert.end(), subResVert.begin(), subResVert.end());
    }
  }
  return resVert;
}

std::vector<VertexPtr>
OrientedGraph::getVerticesByName(std::string_view i_name,
                                 const bool i_addSubGraphs) const {
  std::vector<VertexPtr> resVert;
  for (const auto &value: d_vertices) {
    for (VertexPtr vert: value)
      if (vert->getRawName() == i_name)
        resVert.push_back(vert);
  }
  if (i_addSubGraphs)
    for (GraphPtr vert: d_subGraphs) {
      std::vector<VertexPtr> subResVert =
          vert->getVerticesByName(i_name, i_addSubGraphs);
      resVert.insert(resVert.end(), subResVert.begin(), subResVert.end());
    }
  return resVert;
}

size_t OrientedGraph::sumFullSize() const {
  return d_vertices.at(VertexTypes::input).size() +
         d_vertices.at(VertexTypes::constant).size() +
         d_vertices.at(VertexTypes::gate).size() +
         d_vertices.at(VertexTypes::output).size() +
         d_vertices.at(VertexTypes::sequential).size() +
         d_vertices.at(VertexTypes::subGraph).size();
}

std::map<Gates, size_t> OrientedGraph::getGatesCount() const {
  return d_gatesCount;
}

std::map<Gates, std::map<Gates, size_t>>
OrientedGraph::getEdgesGatesCount() const {
  return d_edgesGatesCount;
}

void OrientedGraph::reserve(VertexTypes i_type, size_t i_capacity) {
  d_vertices[i_type].reserve(d_vertices[i_type].size() + i_capacity);
}

std::string OrientedGraph::calculateHash() {
  if (d_hashState)
    return std::to_string(d_hashed);

  d_hashState = HC_IN_PROGRESS;
  std::vector<size_t> hashed_data;
  hashed_data.reserve(d_vertices[VertexTypes::output].size());

  for (auto *vertex: d_vertices[VertexTypes::output]) {
    hashed_data.push_back(vertex->calculateHash());
  }
  std::sort(hashed_data.begin(), hashed_data.end());

  size_t h = 0;
  for (const auto &sub: hashed_data) {
    hashCombine(h, sub);
  }
  d_hashed = h;
  d_hashState = HC_CALC;

  return std::to_string(d_hashed);
}

bool OrientedGraph::operator==(const OrientedGraph &rhs) {
  return d_hashed == rhs.d_hashed && d_hashed;
}

void OrientedGraph::setCurrentParent(GraphPtr i_parent) {
  d_currentParentGraph = i_parent;
}

void OrientedGraph::resetCounters(GraphPtr i_where) {
  d_graphInstanceToVerilogCount[i_where->d_graphID] = 0;
  d_graphInstanceToDotCount[i_where->d_graphID] = 0;
}
bool OrientedGraph::verilogFileCreating(GraphPtr i_graph, std::string i_path,
                                        std::string i_filename,
                                        std::ofstream &i_fileStream) {
  if (i_graph->d_alreadyParsedVerilog && i_graph->d_isSubGraph) {
    return true;
  }
  // В данном методе происходит только генерация одного графа. Без подграфов.
  std::string res;
  std::string verilogTab = "\t";

  if (!i_filename.size()) {
    i_filename = fmt::format("{}.v", i_graph->d_name);
  }
  if (!i_fileStream) {
    CG_LOG_ERROR << "cannot write file to " << i_path;
    return false;
  }

  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  i_fileStream
      << "//This file was generated automatically using CircuitGen_Graph at "
      << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << ".\n\n";
  return true;
}

void OrientedGraph::verilogInoutsWriting(
    GraphPtr i_graph, std::ofstream &i_fileStream,
    std::function<void(VertexPtr i_usedType)> i_printPin) {
  std::string verilogTab = "\t";
  i_fileStream << "module " << i_graph->d_name << "(\n" << verilogTab;

  // here we are parsing inputs by their wire size
  for (auto *inp: i_graph->d_vertices[VertexTypes::input]) {
    i_printPin(inp);
    i_fileStream << ", ";
  }
  i_fileStream << '\n' << verilogTab;

  // and outputs
  for (auto *outVert: i_graph->d_vertices[VertexTypes::output]) {
    i_printPin(outVert);
    i_fileStream << ((outVert ==
                      i_graph->d_vertices[VertexTypes::output].back())
                         ? "\n"
                         : ", ");
  }
  i_fileStream << ");\n";

  for (const auto &parameter: i_graph->d_verilogParameters) {
    i_fileStream << verilogTab << "parameter " << parameter.first << " = "
                 << parameter.second << ";\n";
  }

  i_fileStream << verilogTab;
}

void OrientedGraph::verilogVerticesDeclaration(
    GraphPtr i_graph, std::ofstream &i_fileStream,
    std::function<void(std::vector<GraphVertexBase *>, VertexTypes i_usedType)>
        i_printFunction) {
  std::string verilogTab = "\t";
  uint8_t count = 0;
  for (auto eachVertex:
       {i_graph->d_vertices[VertexTypes::input],
        i_graph->d_vertices[VertexTypes::output],
        i_graph->d_allSubGraphsOutputs, i_graph->d_vertices[VertexTypes::gate],
        i_graph->d_vertices[VertexTypes::sequential]}) {
    if (eachVertex.size()) {
      auto i_usedType = eachVertex.back()->getType();
      i_fileStream << VertexUtils::vertexTypeToComment(i_usedType);
      switch (count) {
        case 2:
          i_fileStream << " for subGraphs outputs";
          break;
        case 3:
          i_fileStream << " for main graph";
          break;
      }
      i_fileStream << std::endl << verilogTab;
      i_printFunction(eachVertex, i_usedType);
    }
    if (eachVertex.size())
      i_fileStream << verilogTab;
    ++count;
  }
}

void OrientedGraph::verilogConstantWriting(
    GraphPtr i_graph, std::ofstream &i_fileStream,
    std::function<void(VertexPtr)> i_getInstance,
    std::function<void(VertexPtr)> i_getDefinition) {
  std::string verilogTab = "\t";

  if (i_graph->d_vertices[VertexTypes::constant].size()) {
    i_fileStream << VertexUtils::vertexTypeToComment(constant);
    i_fileStream << "\n";
  }
  // writing consts
  for (auto *oper: i_graph->d_vertices[VertexTypes::constant]) {
    i_fileStream << verilogTab;
    i_getInstance(oper);
  }
  if (!i_graph->d_vertices[constant].empty())
    i_fileStream << "\n";
  for (auto *oper: i_graph->d_vertices[VertexTypes::constant]) {
    i_getDefinition(oper);
    i_fileStream << "\n";
  }
}

bool OrientedGraph::verilogSubgraphWriting(GraphPtr i_graph,
                                           std::ofstream &i_fileStream,
                                           std::string i_path) {
  if (i_graph->d_subGraphs.size()) {
    i_fileStream << "\n";
  }
  // and all modules
  for (auto *subPtr: i_graph->d_vertices[VertexTypes::subGraph]) {
    auto *sub = static_cast<GraphVertexSubGraph *>(subPtr);

    if (!sub->toVerilog(i_path)) {
      return false;
    }
    i_fileStream << sub->toVerilog();
  }
  return true;
}

void OrientedGraph::verilogVerticesDefining(
    GraphPtr i_graph, std::ofstream &i_fileStream,
    std::function<void(VertexPtr)> i_printDefinitionSequential,
    std::function<void(const VertexPtr)> i_printDefinitionGates) {

  if (i_graph->d_vertices[VertexTypes::sequential].size())
    i_fileStream << "\n";

  for (auto *oper: i_graph->d_vertices[VertexTypes::sequential]) {
    i_printDefinitionSequential(oper);
  }

  if (i_graph->d_vertices[VertexTypes::gate].size()) {
    i_fileStream << "\n";
  }
  // and all operations
  for (auto *oper: i_graph->d_vertices[VertexTypes::gate]) {
    i_printDefinitionGates(oper);
    i_fileStream << "\n";
  }

  i_fileStream << "\n";
  // and all outputs
  for (auto *oper: i_graph->d_vertices[VertexTypes::output]) {
    i_printDefinitionGates(oper);
    i_fileStream << "\n";
  }
}

bool OrientedGraph::verilogFinalOperations(GraphPtr i_graph,
                                           std::ofstream &i_fileStream) {
  i_fileStream << "endmodule\n";

  i_graph->d_alreadyParsedVerilog = true;

  if (i_graph->d_isSubGraph) {
    return true;
  }

  i_fileStream.close();
  return true;
}
bool OrientedGraph::printSequentialModules(GraphPtr i_graph,
                                           std::ofstream &i_fileStream) {
  std::string_view module =
      "module {}({}, q{})\n\tinput {};\n\toutput q{};\n\t{}\n\tendmodule\n\n";
  std::string inoutsList, inputList;
  std::unordered_set<SequentialTypes> typesToPrint;

  for (auto *v: i_graph->getVerticesByType(sequential)) {
    typesToPrint.insert(static_cast<GraphVertexSequential *>(v)->getSeqType());
  }

  for (SequentialTypes type: typesToPrint) {
    i_fileStream << fmt::format(
        module, GraphUtils::parseSequentialToString(type),
        fmt::join(GraphUtils::parseSequentialToInputs(type), ", "), "",
        fmt::join(GraphUtils::parseSequentialToInputs(type), ", "), "",
        GraphVertexSequential::getSequentialString(
            type, GraphUtils::parseSequentialToString(type),
            GraphUtils::parseSequentialToInputs(type)));
  }
  return true;
}

bool OrientedGraph::toVerilog(std::string i_path, std::string i_filename,
                              bool i_sequentialByInstance) {
  removeEmptyLogicVertices();
  std::string path = i_path + (d_isSubGraph ? "/submodules" : "");
  auto correctPath = path + "/" + i_filename;
  std::ofstream i_fileStream(correctPath);

  auto lambdaConstant = [&](VertexPtr oper) {
    i_fileStream
        << static_cast<GraphVertexConstant *>(oper)->getVerilogInstance();
    i_fileStream << "\n";
  };
  auto lambdaDeclaration = [&](std::vector<GraphVertexBase *> eachVertex,
                               VertexTypes usedType) {
    i_fileStream << ((usedType == sequential) && i_sequentialByInstance
                         ? "wire"
                         : VertexUtils::vertexTypeToVerilog(usedType))
                 << " ";
    for (auto *value: eachVertex) {
      i_fileStream << value->getRawName()
                   << (value != eachVertex.back() ? ", " : ";\n");
    }
  };
  auto lambdaInouts = [&](VertexPtr vertex) {
    i_fileStream << vertex->getRawName();
  };
  auto lambdaDefining = [&](const VertexPtr vertex) {
    i_fileStream << "\t" << *vertex;
  };
  auto lambdaSequentialDefining = [&](VertexPtr oper) {
    i_fileStream
        << static_cast<GraphVertexSequential *>(oper)->getVerilogInstance();
    i_fileStream << "\n";
  };

  verilogFileCreating(shared_from_this(), correctPath, i_filename,
                      i_fileStream);
  if (i_sequentialByInstance)
    printSequentialModules(shared_from_this(), i_fileStream);

  verilogInoutsWriting(shared_from_this(), i_fileStream, lambdaInouts);
  verilogVerticesDeclaration(shared_from_this(), i_fileStream,
                             lambdaDeclaration);
  verilogConstantWriting(shared_from_this(), i_fileStream, lambdaConstant,
                         lambdaDefining);
  verilogSubgraphWriting(shared_from_this(), i_fileStream, i_path);

  if (i_sequentialByInstance)
    verilogVerticesDefining(shared_from_this(), i_fileStream,
                            lambdaSequentialDefining, lambdaDefining);

  else
    verilogVerticesDefining(shared_from_this(), i_fileStream, lambdaDefining,
                            lambdaDefining);
  return verilogFinalOperations(shared_from_this(), i_fileStream);
}

bool OrientedGraph::toVerilogBusEnabled(std::string i_path,
                                        std::string i_filename,
                                        bool i_sequentialByInstance) {
  removeEmptyLogicVertices();
  std::string path = i_path + (d_isSubGraph ? "/submodules" : "");
  auto correctPath = path + "/" + i_filename;
  std::ofstream i_fileStream(correctPath);
  auto lambdaConstant = [&](VertexPtr ptr) {
    if (ptr->isBus())
      i_fileStream
          << static_cast<GraphVertexBusConstant *>(ptr)->getVerilogInstance();
    else
      i_fileStream
          << static_cast<GraphVertexConstant *>(ptr)->getVerilogInstance();
  };
  auto lambdaDeclaration = [&](std::vector<GraphVertexBase *> eachVertex,
                               VertexTypes usedType) {
    size_t length = -1;
    std::multimap<size_t, VertexPtr> busSet;
    for (auto *value: eachVertex) {
      if (value->isBus())
        busSet.emplace(std::make_pair(
            GraphVertexBus::getBusPointer(value)->getWidth(), value));
    }

    if (busSet.size() < eachVertex.size()) {
      i_fileStream << ((usedType == sequential) && i_sequentialByInstance
                           ? "wire"
                           : VertexUtils::vertexTypeToVerilog(usedType))
                   << " ";
      for (auto *value: eachVertex) {
        if (!value->isBus())
          i_fileStream << value->getRawName()
                       << (value != eachVertex.back() ? ", " : ";\n\t");
      }
    }
    for (auto value: busSet) {
      if (GraphVertexBus::getBusPointer(value.second)->getWidth() == length)
        i_fileStream << value.second->getRawName()
                     << (value != *busSet.rbegin() ? ", " : ";\n");
      else
        i_fileStream
            << VertexUtils::vertexTypeToVerilog(usedType) << " "
            << GraphVertexBus::getBusPointer(value.second)->getBusNameSuffix()
            << " " << value.second->getRawName()
            << (value != *busSet.rbegin() ? ", " : ";\n");
      length = value.first;
    }
    i_fileStream << "\n";
  };
  auto lambdaInouts = [&](VertexPtr vertex) {
    i_fileStream << vertex->getRawName();
  };
  auto lambdaDefining = [&](const VertexPtr vertex) {
    i_fileStream << "\t" << *vertex;
  };
  auto lambdaSequentialDefining = [&](VertexPtr vertex) {
    i_fileStream
        << "\t"
        << static_cast<GraphVertexSequential *>(vertex)->getVerilogInstance();
  };

  verilogFileCreating(shared_from_this(), i_path, i_filename, i_fileStream);
  if (i_sequentialByInstance)
    printSequentialModules(shared_from_this(), i_fileStream);

  verilogInoutsWriting(shared_from_this(), i_fileStream, lambdaInouts);
  verilogVerticesDeclaration(shared_from_this(), i_fileStream,
                             lambdaDeclaration);
  verilogConstantWriting(shared_from_this(), i_fileStream, lambdaConstant,
                         lambdaDefining);
  verilogSubgraphWriting(shared_from_this(), i_fileStream, i_path);
  if (i_sequentialByInstance)
    verilogVerticesDefining(shared_from_this(), i_fileStream,
                            lambdaSequentialDefining, lambdaDefining);
  else
    verilogVerticesDefining(shared_from_this(), i_fileStream, lambdaDefining,
                            lambdaDefining);
  return verilogFinalOperations(shared_from_this(), i_fileStream);
}

bool OrientedGraph::toVerilogBusEnabledAsOneBit(std::string i_path,
                                                std::string i_filename) {
  removeEmptyLogicVertices();
  std::string path = i_path + (d_isSubGraph ? "/submodules" : "");
  auto correctPath = path + "/" + i_filename;
  std::ofstream i_fileStream(correctPath);

  auto lambdaConstantInstance = [&](VertexPtr ptr) {
    if (ptr->isBus())
      i_fileStream << static_cast<GraphVertexBusConstant *>(ptr)
                          ->getVerilogInstanceSeparate();
    else
      i_fileStream
          << static_cast<GraphVertexConstant *>(ptr)->getVerilogInstance();
  };

  auto lambdaDeclaration = [&](std::vector<GraphVertexBase *> eachVertex,
                               VertexTypes usedType) {
    std::multimap<size_t, VertexPtr> busSet;
    std::vector<std::string> singleVertices;
    for (auto *value: eachVertex) {
      if (!value->isBus())
        singleVertices.emplace_back(value->getRawName());
      else
        busSet.emplace(std::make_pair(
            GraphVertexBus::getBusPointer(value)->getWidth(), value));
    }
    if (!singleVertices.empty()) {
      i_fileStream << fmt::format(
          "{} {};\n\t",
          ((usedType == sequential)
               ? "wire"
               : VertexUtils::vertexTypeToVerilog(usedType)),
          fmt::join(singleVertices, ", "));
    }
    singleVertices.clear();
    for (auto value = busSet.begin(); value != busSet.end(); ++value) {
      i_fileStream << ((usedType == sequential)
                           ? "wire"
                           : VertexUtils::vertexTypeToVerilog(usedType))
                   << " ";
      for (size_t i = 0;
           i < GraphVertexBus::getBusPointer(value->second)->getWidth(); ++i)
        i_fileStream
            << value->second->getRawName() << "_" << i
            << (i != GraphVertexBus::getBusPointer(value->second)->getWidth() -
                            1
                    ? ", "
                    : ";\n\t");
    }
    i_fileStream << "\n";
  };

  auto lambdaInouts = [&](VertexPtr vertex) {
    if (vertex->isBus()) {
      const size_t width = GraphVertexBus::getBusPointer(vertex)->getWidth();
      for (size_t i = 0; i + 1 < width; ++i) {
        i_fileStream << vertex->getRawName() << "_" << i << ", ";
      }
      i_fileStream << vertex->getRawName() << "_" << (width - 1);
    } else
      i_fileStream << vertex->getRawName();
  };
  auto lambdaDefining = [&](const VertexPtr vertex) {
    if (!vertex->isBus())
      i_fileStream << "\t" << *vertex << "\n";
    else
      i_fileStream << "\t"
                   << GraphVertexBus::getBusPointer(vertex)->toOneBitVerilog();
  };
  verilogFileCreating(shared_from_this(), i_path, i_filename, i_fileStream);
  printSequentialModules(shared_from_this(), i_fileStream);
  verilogInoutsWriting(shared_from_this(), i_fileStream, lambdaInouts);
  verilogVerticesDeclaration(shared_from_this(), i_fileStream,
                             lambdaDeclaration);
  verilogConstantWriting(shared_from_this(), i_fileStream,
                         lambdaConstantInstance, lambdaDefining);
  verilogSubgraphWriting(shared_from_this(), i_fileStream, i_path);
  verilogVerticesDefining(shared_from_this(), i_fileStream, lambdaDefining,
                          lambdaDefining);
  return verilogFinalOperations(shared_from_this(), i_fileStream);
}
DotReturn OrientedGraph::toDOT() {
  removeEmptyLogicVertices();
  DotReturn dot = {{DotTypes::DotGraph, {{"name", d_name}}}};

  CG_LOG_INFO << "      DotGraph(" << d_name << ") added to DOT";
  CG_LOG_INFO << "      Start adding vertices to DOT";
  CG_LOG_INFO << "      inputs          : "
              << d_vertices[VertexTypes::input].size();
  CG_LOG_INFO << "      outputs         : "
              << d_vertices[VertexTypes::output].size();
  CG_LOG_INFO << "      subGraphOutputs : " << d_allSubGraphsOutputs.size();
  CG_LOG_INFO << "      gates           : "
              << d_vertices[VertexTypes::gate].size();
  CG_LOG_INFO << "      constants       : "
              << d_vertices[VertexTypes::constant].size();

  size_t sizeAll = d_allSubGraphsOutputs.size();
  for (const auto &vec: d_vertices)
    sizeAll += vec.size();
  dot.reserve(sizeAll);
  for (const auto &eachVertex:
       {d_vertices[VertexTypes::input], d_vertices[VertexTypes::output],
        d_vertices[VertexTypes::gate], d_vertices[VertexTypes::sequential],
        d_vertices[VertexTypes::constant]}) {
    for (auto *value: eachVertex) {
      DotReturn dotVertex = value->toDOT();
      dot.insert(std::end(dot), std::begin(dotVertex), std::end(dotVertex));
    }
  }
  for (auto *value: d_allSubGraphsOutputs) {
    dot.push_back(value->toDOT()[0]);
  }
  std::vector<std::pair<GraphVertexSubGraph *, DotReturn>> subDotResults;
  for (auto *subPtr: d_vertices[VertexTypes::subGraph]) {
    auto *sub = static_cast<GraphVertexSubGraph *>(subPtr);

    DotReturn val = sub->toDOT();
    subDotResults.push_back({sub, val});

    auto subGraph = sub->getSubGraph();
    for (size_t i = 0;
         i < subGraph->getBaseVertexes()[VertexTypes::input].size(); ++i) {
      auto *inp = subPtr->getInConnections()[i];
      std::string inp_name =
          subGraph->getBaseVertexes()[VertexTypes::input][i]->getName();

      dot.push_back({DotTypes::DotEdge,
                     {{"to", val[0].second["instName"] + "_" + inp->getName()},
                      {"from", inp_name}}});
    }
  }

  for (const auto &subGraphPair: subDotResults) {
    auto buffers = subGraphPair.first->getOutConnections();
    auto outs = subGraphPair.first->getSubGraph()->getVerticesByType(
        VertexTypes::output);
    for (uint32_t i = 0; i < outs.size(); i++) {
      dot.push_back({DotTypes::DotEdge,
                     {{"from", subGraphPair.second[0].second.at("instName") +
                                   "_" + outs[i]->getName()},
                      {"to", buffers[i]->getName()}}});
    }
  }
  for (auto &i: subDotResults) {
    dot.insert(std::end(dot), std::begin(i.second), std::end(i.second));
  }

  dot.push_back({DotTypes::DotExit, {{"name", d_name}}});

  d_alreadyParsedDot = true;
  return dot;
}

bool OrientedGraph::toDOT(std::string i_path, std::string i_filename) {
  using namespace AuxMethodsGraph;
  if (d_alreadyParsedDot && d_isSubGraph) {
    CG_LOG_INFO << "getGraphDotInstance()";
    return true;
  }
  updateLevels();
  DotReturn dot = toDOT();

  std::string dotTab = "  ";

  if (!i_filename.size()) {
    i_filename = d_name + ".dot";
  }
  std::string path = i_path + (d_isSubGraph ? "/submodulesDOT" : "");

  auto correctPath = path + "/" + i_filename;
  std::ofstream fileStream(correctPath);

  if (!fileStream) {
    CG_LOG_ERROR << "cannot write file to " << path;
    return false;
  }

  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  fileStream
      << "// This file was generated automatically using CircuitGen_Graph at ";
  fileStream << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << ".\n\n";

  fileStream << dotReturnToString(dot);

  d_alreadyParsedDot = true;

  if (d_isSubGraph) {
    return true;
  }

  fileStream.close();
  return true;
}

void OrientedGraph::parseVertexToGraphML(
    const VertexTypes &vertexType, const std::vector<VertexPtr> &vertexVector,
    const std::string &nodeTemplate, const std::string &edgeTemplate,
    const std::string &i_prefix, std::string &nodes, std::string &edges) {
  using namespace AuxMethodsGraph; // format() and replacer()
  std::string vertexKindName;

  switch (vertexType) {
    // skipping subgraphs, will be parsed later
    case VertexTypes::subGraph:
      return;
    case VertexTypes::input:
    case VertexTypes::output:
    case VertexTypes::sequential:
      vertexKindName = GraphUtils::parseVertexToString(vertexType);
      break;
    default:
      break;
  }

  for (const auto &v: vertexVector) {
    // every "gate" and "const" vertex has subtypes
    switch (vertexType) {
      case VertexTypes::constant:
        vertexKindName = std::string(1, v->getValue());
        break;
      case VertexTypes::gate:
        vertexKindName = GraphUtils::parseGateToString(v->getGate());
        break;
      case VertexTypes::sequential: {
        const auto *seq = static_cast<const GraphVertexSequential *>(v);
        vertexKindName =
            GraphUtils::parseVertexToString(VertexTypes::sequential);
        vertexKindName += seq->isFF() ? "/ff" : "/latch";
        break;
      }
      default:
        break;
    }

    nodes +=
        fmt::format(nodeTemplate, v->getName(i_prefix), vertexKindName, "", "");

    for (const auto &sink: v->getOutConnections()) {
      // parsing edges not related to subGraphs
      if (sink->getType() != VertexTypes::subGraph) {
        edges += fmt::format(edgeTemplate, v->getName(i_prefix),
                             sink->getName(i_prefix));
      }
    }
  }
}

std::string OrientedGraph::toGraphMLClassic(uint16_t i_indent,
                                            const std::string &i_prefix) {
  using namespace AuxMethodsGraph; // format() and replacer()
  using namespace ClassicGraphML;  // templates

  const std::string spaces(i_indent, ' ');

  const std::string graphTemplate = fmt::format(
      rawGraphTemplate, spaces, i_indent ? "{}:" : "{}", "{}", spaces);
  const std::string nodeTemplate =
      fmt::format(rawNodeTemplate, spaces, "{}", spaces, "{}", "{}{}", spaces);
  const std::string edgeTemplate =
      fmt::format(rawEdgeTemplate, spaces, "{}", "{}");

  std::string nodes, edges, vertexKindName;
  // Nested graphs use "prefix::" for hierarchical node/edge names.
  const std::string actI_prefix = i_indent ? i_prefix + "::" : i_prefix;

  uint8_t counter = 0;
  for (const auto &vertexVector: d_vertices) {
    parseVertexToGraphML((VertexTypes)counter++, vertexVector, nodeTemplate,
                         edgeTemplate, actI_prefix, nodes, edges);
  }
  if (!d_allSubGraphsOutputs.empty()) {
    parseVertexToGraphML(VertexTypes::gate, d_allSubGraphsOutputs, nodeTemplate,
                         edgeTemplate, actI_prefix, nodes, edges);
  }

  // Emit each subgraph instance as a nested <graph> under a subGraph node,
  // plus edges that bind parent ports to the instance (ported from
  // graphMLrework; edge directions match GraphVertexSubGraph::toVerilog).
  for (const auto &sgvBase: d_vertices.at(VertexTypes::subGraph)) {
    const auto *sgv = static_cast<const GraphVertexSubGraph *>(sgvBase);
    const std::string sgI_prefix = sgvBase->getName(actI_prefix);
    const GraphPtr &sg = sgv->getSubGraph();

    nodes += fmt::format(
        nodeTemplate, sgI_prefix, "subGraph", "\n",
        sgv->toGraphML(static_cast<uint16_t>(i_indent + 6), sgI_prefix));

    const auto &gInputs = sg->d_vertices.at(VertexTypes::input);
    const auto &gOutputs = sg->d_vertices.at(VertexTypes::output);
    const auto &vInputs = sgv->d_inConnections;
    const auto &vOutputs = sgv->d_outConnections;

    const size_t inputCount = std::min(gInputs.size(), vInputs.size());
    for (size_t i = 0; i < inputCount; ++i) {
      edges += fmt::format(edgeTemplate, vInputs[i]->getName(actI_prefix),
                           gInputs[i]->getName(sgI_prefix + "::"));
    }

    const size_t outputCount = std::min(gOutputs.size(), vOutputs.size());
    for (size_t i = 0; i < outputCount; ++i) {
      edges +=
          fmt::format(edgeTemplate, gOutputs[i]->getName(sgI_prefix + "::"),
                      vOutputs[i]->getName(actI_prefix));
    }
  }

  if (i_indent != 0) {
    return fmt::format(graphTemplate, i_prefix, nodes + edges);
  }
  return fmt::format(mainTemplate,
                     fmt::format(graphTemplate, d_name, nodes + edges));
}

bool OrientedGraph::toGraphMLClassic(std::ofstream &fileStream) {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  fileStream << "<!-- This file was generated automatically using "
                "CircuitGen_Graph at ";
  fileStream << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << ". -->" << std::endl
             << std::endl;

  fileStream << this->toGraphMLClassic();
  return true;
}

bool OrientedGraph::toGraphMLPseudoABCD(std::ofstream &fileStream) {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  fileStream << "<!-- This file was generated automatically using "
                "CircuitGen_Graph at ";
  fileStream << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << ". -->" << std::endl
             << std::endl;

  fileStream << this->toGraphMLPseudoABCD();
  return true;
}

bool OrientedGraph::toGraphMLOpenABCD(std::ofstream &fileStream) {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  fileStream << "<!-- This file was generated automatically using "
                "CircuitGen_Graph at ";
  fileStream << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << ". -->" << std::endl
             << std::endl;

  fileStream << this->toGraphMLOpenABCD();
  return true;
}

std::string OrientedGraph::toGraphMLPseudoABCD() {
  using namespace AuxMethodsGraph; // format()
  using namespace PseudoABCD;      // templates

  GraphPtr graphPtr = shared_from_this();
  if (!d_vertices.at(VertexTypes::subGraph).empty()) {
    graphPtr = this->unrollGraph();
  }

  graphPtr->updateLevels();
  std::sort(graphPtr->d_vertices.at(VertexTypes::gate).begin(),
            graphPtr->d_vertices.at(VertexTypes::gate).end(), CompareLevels);

  std::string nodes, edges, nodeType, actualName, sinkName;
  std::map<std::string, uint32_t> nodeNames;
  uint32_t nodeCounter = 0;

  char vertexType = 0;
  for (const auto &vertexVector: graphPtr->d_vertices) {
    switch (vertexType) {
      case VertexTypes::input:
        nodeType = "0";
        break;
      case VertexTypes::output:
        nodeType = "1";
        break;
    }

    for (const auto &v: vertexVector) {
      // every "gate" and "const" vertex has subtypes
      switch (vertexType) {
        case VertexTypes::constant:
          nodeType = "100" + std::string(1, v->getValue());
          break;
        case VertexTypes::gate:
          nodeType = gateToABCDType.at(v->getGate());
          break;
        case VertexTypes::sequential: {
          const auto *seq = static_cast<const GraphVertexSequential *>(v);
          // Dedicated codes (not used by gateToABCDType): flip-flop vs latch.
          nodeType = seq->isFF() ? "17" : "18";
          break;
        }
      }
      actualName = v->getName();
      if (nodeNames.find(actualName) == nodeNames.end()) {
        nodeNames[actualName] = nodeCounter++;
      }
      nodes += fmt::format(nodeTemplate, nodeNames.at(actualName), actualName,
                           nodeType);

      for (const auto &sink: v->getOutConnections()) {
        sinkName = sink->getName();
        if (nodeNames.find(sinkName) == nodeNames.end()) {
          nodeNames[sinkName] = nodeCounter++;
        }

        edges += fmt::format(edgeTemplate, nodeNames.at(actualName),
                             nodeNames.at(sinkName));
      }
    }
    ++vertexType;
  }

  return fmt::format(mainTemplate, nodes + edges);
}

std::string OrientedGraph::toGraphMLOpenABCD() {
  using namespace AuxMethodsGraph; // format()
  using namespace OpenABCD;        // templates

  GraphPtr graphPtr = shared_from_this();
  if (!d_vertices.at(VertexTypes::subGraph).empty()) {
    graphPtr = this->unrollGraph();
  }

  graphPtr->updateLevels();
  std::sort(graphPtr->d_vertices.at(VertexTypes::gate).begin(),
            graphPtr->d_vertices.at(VertexTypes::gate).end(), CompareLevels);

  std::string nodes, edges, nodeType, actualName, currentName;
  Gates currentGate, vGate, sGate;
  std::map<std::string, uint32_t> nodeNames;
  uint32_t nodeCounter = 0, inverted;

  char vertexType = 0;
  for (const auto &vertexVector: graphPtr->d_vertices) {
    switch (vertexType) {
      case VertexTypes::input:
        nodeType = "0";
        break;
      case VertexTypes::output:
        nodeType = "1";
        break;
    }

    for (const auto &v: vertexVector) {
      // every "gate" and "const" vertex has subtypes
      switch (vertexType) {
        case VertexTypes::constant:
          nodeType = "100" + std::string(1, v->getValue());
          break;
        case VertexTypes::gate:
          vGate = v->getGate();
          if (vGate == Gates::GateBuf || vGate == Gates::GateNot) {
            continue;
          }
          nodeType = gateToABCDType.at(vGate);
          break;
        case VertexTypes::sequential: {
          const auto *seq = static_cast<const GraphVertexSequential *>(v);
          nodeType = seq->isFF() ? "17" : "18";
          break;
        }
      }

      actualName = v->getName();
      if (nodeNames.find(actualName) == nodeNames.end()) {
        nodeNames[actualName] = nodeCounter++;
      }

      inverted = 0;
      for (const auto &sink: v->getOutConnections()) {
        std::stack<std::pair<VertexPtr, bool>> stck;
        stck.push({sink, sink->getGate() == Gates::GateNot ? 1 : 0});

        while (!stck.empty()) {
          auto current = stck.top();
          stck.pop();
          currentGate = current.first->getGate();
          if (currentGate == Gates::GateBuf || currentGate == Gates::GateNot) {
            for (const auto &s: current.first->getOutConnections()) {
              sGate = s->getGate();
              bool state = current.second;
              stck.push({s, sGate == Gates::GateNot ? !state : state});
            }
          } else {
            currentName = current.first->getName();
            if (nodeNames.find(currentName) == nodeNames.end()) {
              nodeNames[currentName] = nodeCounter++;
            }
            edges += fmt::format(edgeTemplate, nodeNames.at(currentName),
                                 nodeNames.at(actualName), current.second);
            inverted += current.second;
          }
        }
      }
      nodes += fmt::format(nodeTemplate, nodeNames.at(actualName), actualName,
                           nodeType, inverted);
    }
    ++vertexType;
  }
  return fmt::format(mainTemplate, nodes + edges);
}

GraphPtr OrientedGraph::unrollGraph() {
  GraphPtr newGraph = std::make_shared<OrientedGraph>(d_name + "_unrolled");
  std::map<VertexPtr, VertexPtr> vPairs;

  for (const auto &v: d_vertices.at(VertexTypes::input)) {
    vPairs.insert({v, newGraph->addInput(v->getName())});
  }
  for (const auto &v: d_vertices.at(VertexTypes::output)) {
    vPairs.insert({v, newGraph->addOutput(v->getName())});
  }

  auto unroller = [&](GraphPtr graph, std::string prefix,
                      auto &&unroller) -> void {
    char vertexType = 0;
    for (const auto &vertices: graph->getBaseVertexes()) {
      for (const auto &v: vertices) {
        VertexPtr newVertex;

        switch (vertexType) {
          case VertexTypes::constant:
            newVertex = newGraph->addConst(v->getValue(), v->getName(prefix));
            vPairs[v] = newVertex;
            break;
          case VertexTypes::gate:
            newVertex = newGraph->addGate(v->getGate(), v->getName(prefix));
            vPairs[v] = newVertex;
            break;

          case VertexTypes::subGraph: {
            const auto *sgv = static_cast<GraphVertexSubGraph *>(v);
            const GraphPtr sg = sgv->getSubGraph();
            const auto &gInputs = sg->d_vertices.at(VertexTypes::input);
            const auto &gOutputs = sg->d_vertices.at(VertexTypes::output);
            const auto &vInputs = v->getInConnections();
            const auto &vOutputs = v->getOutConnections();
            VertexPtr ptr;

            unroller(sg, v->getName() + "__", unroller);

            for (size_t i = 0; i < vInputs.size(); ++i) {
              ptr = vInputs.at(i);
              for (const auto &innerVertex: gInputs[i]->getOutConnections()) {
                if (vPairs.find(innerVertex) != vPairs.end() &&
                    vPairs.find(ptr) != vPairs.end())
                  newGraph->addEdge(vPairs.at(ptr), vPairs.at(innerVertex));
              }
            }

            for (size_t i = 0; i < vOutputs.size(); ++i) {
              for (const auto &ptr: gOutputs[i]->getInConnections()) {
                for (const auto &nonBuffer: vOutputs[i]->getOutConnections()) {
                  if (vPairs.find(nonBuffer) != vPairs.end() &&
                      vPairs.find(ptr) != vPairs.end())
                    newGraph->addEdge(vPairs.at(ptr), vPairs.at(nonBuffer));
                }
              }
            }

            break;
          }
          default:
            continue;
        }
      }
      ++vertexType;
    }
  };

  unroller(shared_from_this(), "", unroller);

  for (const auto &pair: vPairs) {
    size_t size = pair.first->getOutConnections().size();
    pair.second->reserveOutConnections(size);
    for (const auto &v: pair.first->getOutConnections()) {
      // if v is not subGraph and if v is not output from subGraph
      if (v->getType() != VertexTypes::subGraph &&
          (v->getType() != VertexTypes::output ||
           v->getBaseGraph().lock().get() == this)) {
        newGraph->addEdge(pair.second, vPairs.at(v));
      }
    }
  }
  return newGraph;
}

bool OrientedGraph::isConnected(bool i_recalculate) {
  if (d_connected && !i_recalculate) {
    return d_connected + 1;
  }

  size_t size = sumFullSize();
  if (size <= 1) {
    return (d_connected = 1);
  }

  size_t subGraphsBuffersCount = 0;
  std::unordered_set<VertexPtr> disconnectedSubGraphs;
  for (auto *subGraph: d_vertices[VertexTypes::subGraph]) {
    subGraphsBuffersCount += subGraph->getOutConnections().size();
    auto *subGraphPtr = static_cast<GraphVertexSubGraph *>(subGraph);
    if (!subGraphPtr->getSubGraph()->isConnected()) {
      disconnectedSubGraphs.insert(subGraph);
    }
  }

  std::unordered_set<VertexPtr> visited;
  VertexPtr startVertex = nullptr;

  char type = 0;
  for (auto &vertices: d_vertices) {
    if (type == VertexTypes::subGraph) {
      continue;
    }
    if (!vertices.empty()) {
      startVertex = vertices[0];
      break;
    }
    ++type;
  }

  dfs(startVertex, visited, disconnectedSubGraphs);

  if (visited.size() ==
      size + subGraphsBuffersCount - disconnectedSubGraphs.size()) {
    return (d_connected = 1);
  }
  return (d_connected = -1) + 1;
}

// void OrientedGraph::dfs(
//     VertexPtr                      i_v,
//     std::unordered_set<VertexPtr>& i_visited
// ) {
//   i_visited.insert(i_v);
//   for (auto v : i_v->getOutConnections()) {
//     if (i_visited.find(v) == i_visited.end()) {
//       dfs(v, i_visited);
//     }
//   }
//   for (auto v : i_v->getInConnections()) {
//     auto ptr = v.lock();
//     if (!ptr) {
//       throw std::invalid_argument("Dead pointer!");
//     }
//     if (i_visited.find(ptr) == i_visited.end()) {
//       dfs(ptr, i_visited);
//     }
//   }
// }

void OrientedGraph::dfs(VertexPtr i_startVertex,
                        std::unordered_set<VertexPtr> &i_visited,
                        std::unordered_set<VertexPtr> &i_dsg) {
  std::stack<VertexPtr> stck;
  stck.push(i_startVertex);

  while (!stck.empty()) {
    VertexPtr current = stck.top();
    stck.pop();

    if (i_visited.find(current) == i_visited.end()) {
      i_visited.insert(current);

      for (auto *vert: current->getOutConnections()) {
        if (vert->getType() != VertexTypes::subGraph ||
            i_dsg.find(vert) == i_dsg.end()) {
          stck.push(vert);
        } else {
          auto *subGraphPtr = static_cast<GraphVertexSubGraph *>(vert);
          for (auto *buf: subGraphPtr->getOutputBuffersByOuterInput(current)) {
            stck.push(buf);
          }
        }
      }
      for (auto *ptr: current->getInConnections()) {
        if (ptr->getType() != VertexTypes::subGraph ||
            i_dsg.find(ptr) == i_dsg.end()) {
          stck.push(ptr);
        } else {
          auto *subGraphPtr = static_cast<GraphVertexSubGraph *>(ptr);
          for (auto *input:
               subGraphPtr->getOuterInputsByOutputBuffer(current)) {
            stck.push(input);
          }
        }
      }
    }
  }
}

#ifdef LOGFLAG
void OrientedGraph::log(el::base::type::ostream_t &osStream) const {
  //   osStream << "\n";
  //   osStream << "Graph Name: " << d_name << "\n";
  //   osStream << "Graph ID: " << d_graphID << "\n";
  //   osStream << "Number of Edges: " << d_edgesCount << "\n";
  //   osStream << "Need Level Update: " << (d_needLevelUpdate ? "Yes" : "No")
  //   <<
  //   "\n"; osStream << "Already Parsed Verilog: " << (d_alreadyParsedVerilog ?
  //   "Yes" : "No")
  //      << "\n";
  //   osStream << "Already Parsed DOT: " << (d_alreadyParsedDot ? "Yes" : "No")
  //   <<
  //   "\n"; osStream << "Graph hash: " << d_hashed << "\n";

  //   bool flag = true;
  //   osStream << "Number of Subgraphs: " << d_subGraphs.size() << "\n";
  //   osStream << "baseSize: " << this->baseSize() << std::endl;
  //   osStream << "fullSize: " << this->fullSize() << std::endl;
  //   osStream << "Stored Vertex Types and Counts:";
  //   for (const auto& pair : d_vertices) {
  //     if (pair.second.size() != 0) {
  //       flag = false;
  //       osStream << "\n\t" <<
  //       GraphUtils::parseVertexToString(pair.first); if (pair.first ==
  //       VertexTypes::subGraph)
  //         osStream << "\t:\t" << pair.second.size();
  //       else
  //         osStream << "\t\t:\t" << pair.second.size();
  //     }
  //   }
  //   if (flag)
  //     osStream << " None";
  //   osStream << std::endl;

  //   flag = true;
  //   osStream << "Gate Types and Counts:";
  //   for (const auto& pair : d_gatesCount) {
  //     if (pair.second != 0) {
  //       flag = false;
  //       osStream << "\n\t" << GraphUtils::parseGateToString(pair.first)
  //       <<
  //       "\t:\t"
  //          << pair.second;
  //     }
  //   }
  //   if (flag)
  //     osStream << " None";
  //   osStream << std::endl;

  //   flag = true;
  //   osStream << "Edges Between Gates Counts:";
  //   for (const auto& outer_pair : d_edgesGatesCount) {
  //     for (const auto& inner_pair : outer_pair.second) {
  //       if (inner_pair.second != 0) {
  //         flag = false;
  //         osStream << "\n\t" <<
  //         GraphUtils::parseGateToString(outer_pair.first)
  //            << "\t-> " <<
  //            GraphUtils::parseGateToString(inner_pair.first)
  //            << "\t:\t" << inner_pair.second;
  //       }
  //     }
  //   }
  //   if (flag)
  //     osStream << " None";
  //   osStream << "\n\n";
  //   for (const auto& subGraph : d_vertices.at(VertexTypes::subGraph)) {
  //     osStream << *subGraph;
  //   }
}
#endif

} // namespace CG_Graph
