#include <algorithm>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#include <CircuitGenGraph/DefaultAuxiliaryMethods.hpp>
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>

#include "GraphMLTemplates.hpp"

#include <lorina/lorina.hpp>
#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CG_Graph {

std::atomic_size_t OrientedGraph::d_countGraph = 0;
std::atomic_size_t OrientedGraph::d_countNewGraphInstance = 0;

bool CompareLevels(const VertexPtr left, const VertexPtr right) {
  return left->getLevel() < right->getLevel();
}

OrientedGraph::OrientedGraph(const std::string &i_name, size_t buffer_size,
                             size_t chunk_size) :
    GraphMemory(buffer_size, chunk_size) {
  d_graphID = d_countNewGraphInstance++;

  if (i_name == "")
    d_name = "graph_" + std::to_string(d_countGraph++);
  else
    d_name = i_name;

  // filling edges
  for (auto cur_gate: GraphUtils::getLogicOperationsKeys()) {
    d_edgesGatesCount[cur_gate] = d_gatesCount;
  }

  d_vertices = {std::vector<VertexPtr>(), std::vector<VertexPtr>(),
                std::vector<VertexPtr>(), std::vector<VertexPtr>(),
                std::vector<VertexPtr>()};
}

OrientedGraph::~OrientedGraph() {
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

bool OrientedGraph::needToUpdateLevel() const {
  return d_needLevelUpdate;
}

void OrientedGraph::updateLevels() {
#ifdef LOGFLAG
  LOG(INFO) << "Starting level update. Wait.";
  LOG(INFO) << "Outputs for update: "
            << d_vertices.at(VertexTypes::output).size();
#endif
  int counter = 0;
  for (VertexPtr vert: d_vertices.at(VertexTypes::output)) {
#ifdef LOGFLAG
    LOG(INFO) << counter++ << ". " << vert->getRawName() << " ("
              << vert->getTypeName() << ")";
#endif
    vert->updateLevel();
  }
}

uint32_t OrientedGraph::getMaxLevel() {
  this->updateLevels();
  uint32_t mx = 0;
  for (VertexPtr vert: d_vertices.at(VertexTypes::output)) {
    mx = mx > vert->getLevel() ? mx : vert->getLevel();
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

VertexPtr OrientedGraph::addOutput(const std::string &i_name) {
  VertexPtr newVertex = create<GraphVertexOutput>(
      i_name.empty() ? "" : internalize(i_name), shared_from_this());
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

VertexPtr OrientedGraph::addGate(const Gates &i_gate,
                                 const std::string &i_name) {
  VertexPtr newVertex = create<GraphVertexGates>(
      i_gate, i_name.empty() ? "" : internalize(i_name), shared_from_this());
  d_vertices[VertexTypes::gate].push_back(newVertex);

  ++d_gatesCount[i_gate];

  return newVertex;
}

VertexPtr OrientedGraph::addSequential(const SequentialTypes &i_type,
                                       VertexPtr i_clk, VertexPtr i_data,
                                       const std::string &i_name) {
  auto name = i_name.empty() ? "" : internalize(i_name);
  VertexPtr newVertex = create<GraphVertexSequential>(i_type, i_clk, i_data,
                                                      shared_from_this(), name);
  d_vertices[VertexTypes::sequential].push_back(newVertex);
  newVertex->reserveInConnections(2ul);
  addEdge(i_clk, newVertex);
  addEdge(i_data, newVertex);

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

  newVertex->reserveInConnections(3ul);
  addEdge(i_clk, newVertex);
  addEdge(i_data, newVertex);
  addEdge(i_wire, newVertex);

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

  newVertex->reserveInConnections(4ul);
  addEdge(i_clk, newVertex);
  addEdge(i_data, newVertex);
  addEdge(i_wire1, newVertex);
  addEdge(i_wire2, newVertex);

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

  newVertex->reserveInConnections(5ul);
  addEdge(i_clk, newVertex);
  addEdge(i_data, newVertex);
  addEdge(i_rst, newVertex);
  addEdge(i_set, newVertex);
  addEdge(i_en, newVertex);

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
    for (int i = 0; i < outSize; ++i) {
      VertexPtr newVertex =
          create<GraphVertexGates>(Gates::GateBuf, shared_from_this());

      outputs.push_back(newVertex);
      d_allSubGraphsOutputs.push_back(newVertex);

      addEdge(newGraph, newVertex);
    }
  } else {
#ifdef LOGFLAG
    LOG(ERROR) << "Error, SubGraph without outputs" << std::endl;
#else
    std::cerr << "Error, SubGraph without outputs" << std::endl;
#endif
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
  for (size_t i = 0; i < d_vertices[VertexTypes::input].size(); ++i) {
    GraphVertexInput *inputVert =
        static_cast<GraphVertexInput *>(d_vertices[VertexTypes::input].at(i));
    inputVert->setValue(inputsValues.at(i));
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
  assert(vertex->getGate() == GateDefault);
  for (auto *i: vertex->getInConnections())
    if (i->getType() == gate)
      ++d_edgesGatesCount[i->getGate()][type];
  for (auto *i: vertex->getOutConnections())
    if (i->getType() == gate)
      ++d_edgesGatesCount[type][i->getGate()];
}

void OrientedGraph::removeWasteVertices() {
  updateLevels();
  auto removingForType = [this](VertexTypes type) {
    uint8_t counterForResize = 0;
    for (auto iter = d_vertices[type].begin();
         iter != d_vertices[type].end() - counterForResize;) {
      VertexPtr vert = *iter;
      if (!vert->getLevel()) {
        if (vert->getType() != input && vert->getType() != constant) {
          for (auto *inConnVert: vert->getInConnections()) {
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
      createMajoritySubgraph(); // создаётся один раз

  std::vector<VertexPtr> outputs =
      this->addSubGraph(majoritySubgraph, {a, b, c});
  return outputs.back();
}

bool OrientedGraph::addEdge(VertexPtr from, VertexPtr to) {
  bool f;
  uint32_t n;
  if (from->getBaseGraph().lock() == to->getBaseGraph().lock()) {
    f = from->addVertexToOutConnections(to);
    n = to->addVertexToInConnections(from);
  } else {
    throw std::invalid_argument(
        "Not allowed to add edge from one subgraph to another");
  }
  d_edgesCount += f && (n > 0);

  if (from->getType() == VertexTypes::gate &&
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
  lorina::return_code returnCode = lorina::read_verilog(i_path, *reader);
  if (returnCode == lorina::return_code::parse_error)
    throw std::runtime_error("File do not exist");
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

std::vector<VertexPtr> OrientedGraph::getVerticesByLevel(uint32_t i_level) {
  updateLevels();
  std::vector<VertexPtr> a;
  if (!i_level) {
    a.reserve(d_vertices[input].size() + d_vertices[constant].size());
    a.insert(a.end(), d_vertices[input].begin(), d_vertices[input].end());
    a.insert(a.end(), d_vertices[constant].begin(), d_vertices[constant].end());
  } else {
    // maxLevel / 2 <= i_level -> output is more close to level target
    // ( maxLevel / 2 <= i_level ) * 2 = maxLevel <= i_level << 1
    if (getMaxLevel() <= (i_level << 1u)) {
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
  if (i_name.size()) {
    return d_vertices.at(i_type);
  }

  std::vector<VertexPtr> resVert;
  for (VertexPtr vert: d_vertices.at(i_type))
    if ((i_name == "") || (vert->getRawName() == i_name))
      resVert.push_back(vert);

  if (i_addSubGraphs)
    for (GraphPtr vert: d_subGraphs) {
      std::vector<VertexPtr> subResVert =
          vert->getVerticesByType(i_type, i_name, i_addSubGraphs);
      resVert.insert(resVert.end(), subResVert.begin(), subResVert.end());
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
  std::string hashedStr = "";

  for (auto *vertex: d_vertices[VertexTypes::output]) {
    hashed_data.push_back(vertex->calculateHash());
  }
  std::sort(hashed_data.begin(), hashed_data.end());

  hashedStr.reserve(sizeof(decltype(hashed_data)::value_type) *
                    hashed_data.size());
  for (const auto &sub: hashed_data) {
    hashedStr += sub;
  }
  d_hashed = std::hash<std::string>{}(hashedStr);
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

bool OrientedGraph::toVerilog(std::string i_path, std::string i_filename) {
  if (d_alreadyParsedVerilog && d_isSubGraph) {
    return true;
  }
  // В данном методе происходит только генерация одного графа. Без подграфов.
  std::string verilogTab = "\t";

  if (!i_filename.size()) {
    i_filename = d_name + ".v";
  }
  std::string path = i_path + (d_isSubGraph ? "/submodules" : "");

  auto correctPath = path + "/" + i_filename;
  std::ofstream fileStream(correctPath);

  if (!fileStream) {
#ifdef LOGFLAG
    LOG(ERROR) << "cannot write file to " << correctPath;
#else
    std::cerr << "cannot write file to " << correctPath << std::endl;
#endif
    return false;
  }

  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  fileStream
      << "//This file was generated automatically using CircuitGen_Graph at ";
  fileStream << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "." << std::endl
             << std::endl;
  fileStream << "module " << d_name << "(\n" << verilogTab;

  // here we are parsing inputs by their wire size
  for (auto *inp: d_vertices[VertexTypes::input]) {
    fileStream << inp->getRawName() << ", ";
  }
  fileStream << '\n' << verilogTab;

  // and outputs
  for (auto *outVert: d_vertices[VertexTypes::output]) {
    fileStream << outVert->getRawName()
               << ((outVert == d_vertices[VertexTypes::output].back()) ? "\n"
                                                                       : ", ");
  }
  fileStream << ");\n" << verilogTab;

  // parsing inputs, outputs and wires for subgraphs. And wires for operations
  // too
  uint8_t count = 0;
  for (auto eachVertex:
       {d_vertices[VertexTypes::input], d_vertices[VertexTypes::output],
        d_allSubGraphsOutputs, d_vertices[VertexTypes::gate],
        d_vertices[VertexTypes::sequential]}) {
    if (eachVertex.size()) {
      auto usedType = eachVertex.back()->getType();

      fileStream << VertexUtils::vertexTypeToComment(usedType);

      switch (count) {
        case 2:
          fileStream << " for subGraphs outputs";
          break;
        case 3:
          fileStream << " for main graph";
          break;
      }
      fileStream << std::endl << verilogTab;

      fileStream << VertexUtils::vertexTypeToVerilog(usedType) << " ";
    }

    for (auto *value: eachVertex) {
      fileStream << value->getRawName()
                 << (value != eachVertex.back() ? ", " : ";\n");
    }
    if (eachVertex.size())
      fileStream << verilogTab;

    ++count;
  }

  if (d_vertices[VertexTypes::constant].size()) {
    fileStream << "\n";
  }
  // writing consts
  for (auto *oper: d_vertices[VertexTypes::constant]) {
    fileStream << verilogTab
               << static_cast<GraphVertexConstant *>(oper)->getVerilogInstance()
               << "\n";
    fileStream << verilogTab << (*oper) << "\n";
  }

  if (d_subGraphs.size()) {
    fileStream << "\n";
  }
  // and all modules
  for (auto *subPtr: d_vertices[VertexTypes::subGraph]) {
    auto *sub = static_cast<GraphVertexSubGraph *>(subPtr);

    if (!sub->toVerilog(i_path)) {
      return false;
    }
    fileStream << sub->toVerilog();
  }

  if (d_vertices[VertexTypes::sequential].size()) {
    fileStream << "\n";
  }
  // and all operations
  for (const auto *oper: d_vertices[VertexTypes::sequential]) {
    fileStream << VertexUtils::getSequentialComment(
        static_cast<const GraphVertexSequential *>(oper));
    fileStream << verilogTab << (*oper);
  }

  if (d_vertices[VertexTypes::gate].size()) {
    fileStream << "\n";
  }
  // and all operations
  for (auto *oper: d_vertices[VertexTypes::gate]) {
    fileStream << verilogTab << (*oper) << "\n";
  }

  fileStream << "\n";
  // and all outputs
  for (auto *oper: d_vertices[VertexTypes::output]) {
    fileStream << verilogTab << (*oper) << "\n";
  }

  fileStream << "endmodule\n";

  d_alreadyParsedVerilog = true;

  if (d_isSubGraph) {
    return true;
  }

  fileStream.close();
  return true;
}

DotReturn OrientedGraph::toDOT() {
  DotReturn dot = {{DotTypes::DotGraph, {{"name", d_name}}}};
#ifdef LOGFLAG
  LOG(INFO) << "      DotGraph(" << d_name << ") added to DOT";
  LOG(INFO) << "      Start adding vertices to DOT";
  LOG(INFO) << "      inputs          : "
            << d_vertices[VertexTypes::input].size();
  LOG(INFO) << "      outputs         : "
            << d_vertices[VertexTypes::output].size();
  LOG(INFO) << "      subGraphOutputs : " << d_allSubGraphsOutputs.size();
  LOG(INFO) << "      gates           : "
            << d_vertices[VertexTypes::gate].size();
  LOG(INFO) << "      constants       : "
            << d_vertices[VertexTypes::constant].size();
#endif
  size_t sizeAll = d_allSubGraphsOutputs.size();
  for (const auto &vec: d_vertices)
    sizeAll += vec.size();
  dot.reserve(sizeAll);
  for (const auto &eachVertex:
       {d_vertices[VertexTypes::input], d_vertices[VertexTypes::output],
        d_vertices[VertexTypes::gate], d_vertices[VertexTypes::sequential],
        d_vertices[VertexTypes::constant]}) {
    int counter = 0;
    for (auto *value: eachVertex) {
      DotReturn dotVertex = value->toDOT();
      dot.insert(std::end(dot), std::begin(dotVertex), std::end(dotVertex));
    }
  }
  int counter = 0;
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
    for (auto i = 0; i < outs.size(); i++) {
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
#ifdef LOGFLAG
    LOG(INFO) << "getGraphDotInstance()";
#endif
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
#ifdef LOGFLAG
    LOG(ERROR) << "cannot write file to " << path;
#else
    std::cerr << "cannot write file to " << path << std::endl;
#endif
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

  std::string nodes, edges, graphs, vertexKindName;

  uint8_t counter = 0;
  for (const auto &vertexVector: d_vertices) {
    parseVertexToGraphML((VertexTypes)counter++, vertexVector, nodeTemplate,
                         edgeTemplate, i_prefix, nodes, edges);
  }
  if (d_allSubGraphsOutputs.size()) {
    parseVertexToGraphML(VertexTypes::gate, d_allSubGraphsOutputs, nodeTemplate,
                         edgeTemplate, i_prefix, nodes, edges);
  }

  std::string currentSubGraphTemplate, sgName;

  for (const auto &sg: d_subGraphs) {
    // preparing template for subGraphs as vertices
    currentSubGraphTemplate =
        fmt::format(nodeTemplate, "{}", "subGraph", "\n",
                    sg->toGraphMLClassic(i_indent + 4, i_prefix + "{}::"));

    /// FIXME: Why inputs and outputs of graph are connected with vertices of
    /// the same graph....
    // graphInputs, graphOutputs, verticesInputs, verticesOutputs
    // const auto &gInputs = sg->d_vertices.at(VertexTypes::input);
    // const auto &gOutputs = sg->d_vertices.at(VertexTypes::output);
    // const auto &vInputs = sg->d_subGraphsInputsPtr.at(d_graphID);
    // const auto &vOutputs = sg->d_subGraphsOutputsPtr.at(d_graphID);

    // for (size_t i = 0; i < vOutputs.size(); ++i) {
    //   // getting name of current subGraph vertex
    //   sgName = vOutputs[i][0]->getInConnections()[0]->getName(i_prefix);

    //   // element->subGraph edges
    //   for (size_t j = 0; j < gInputs.size(); ++j) {
    //     edges += format(edgeTemplate, vInputs[i][j]->getName(i_prefix),
    //                     gInputs[j]->getName(sgName + "::"));
    //   }
    //   // subGraph->element edges (skipping buffer)
    //   for (size_t j = 0; j < gOutputs.size(); ++j) {
    //     if (vOutputs[i][j]->getOutConnections().empty()) {
    //       continue;
    //     }
    //     edges +=
    //         format(edgeTemplate, gOutputs[j]->getName(sgName + "::"),
    //                vOutputs[i][j]->getOutConnections()[0]->getName(i_prefix));
    //   }

    //   // parsing subGraphs as vertices
    //   graphs += replacer(currentSubGraphTemplate, sgName);
    // }
  }

  std::string finalGraph =
      fmt::format(graphTemplate, "{}", nodes + graphs + edges);
  if (i_indent != 0) {
    return finalGraph;
  }
  return fmt::format(mainTemplate, fmt::format(finalGraph, d_name));
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
