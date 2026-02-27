/**
 * @file GraphVertexSubGraph.cpp
 * @brief Реализация вершины-подграфа.
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 * @author Fuuulkrum7 <fuuulkrum7@gmail.com>
 * @author Theossr <feolab05@gmail.com>
 * @author NonDif <shapkin.andrey123@gmail.com>
 */
#include "CircuitGenGraph/GraphVertexBase.hpp"
#include <CircuitGenGraph/GraphVertex.hpp>

#include <iostream>
#include <memory>
#include <stack>
#include <stdexcept>
#include <unordered_set>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CG_Graph {

GraphVertexSubGraph::GraphVertexSubGraph(GraphPtr i_subGraph,
                                         GraphPtr i_baseGraph) :
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    GraphVertexBase(VertexTypes::subGraph, i_baseGraph) {
  d_subGraph = i_subGraph;
}

GraphVertexSubGraph::GraphVertexSubGraph(GraphPtr i_subGraph,
                                         std::string_view i_name,
                                         GraphPtr i_baseGraph) :
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    GraphVertexBase(VertexTypes::subGraph, i_name, i_baseGraph) {
  d_subGraph = i_subGraph;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
char GraphVertexSubGraph::updateValue() {
  if (d_inConnections.size() > 0) {
    std::vector<char> inputsValues, outputsValues;
/** @author Theossr <feolab05@gmail.com> */
    if (d_inConnections.front()->getValue() == ValueStates::UndefindedState) {
      inputsValues.push_back(d_inConnections.front()->updateValue());
    }
    for (size_t i = 1; i < d_inConnections.size(); ++i) {
/** @author Theossr <feolab05@gmail.com> */
      if (d_inConnections.at(i)->getValue() == ValueStates::UndefindedState) {
        inputsValues.push_back(d_inConnections.at(i)->updateValue());
      }
    }
    outputsValues = d_subGraph->graphSimulation(inputsValues);
    for (size_t i = 1; i < d_outConnections.size(); ++i) {
      GraphVertexGates *out_connectionVert =
          static_cast<GraphVertexGates *>(d_outConnections.at(i));
      out_connectionVert->d_value = outputsValues.at(i);
    }
    return outputsValues.at(0);
  }
#ifdef LOGFLAG
/** @author Theossr <feolab05@gmail.com> */
  LOG(ERROR) << "Error, SubGraph without inputs" << std::endl;
#else
  std::cerr << "Error, SubGraph without inputs" << std::endl;
#endif
  return ValueStates::NoSignal;
}

/** @author Theossr <feolab05@gmail.com> */
void GraphVertexSubGraph::removeValue() {
  if (d_inConnections.size() > 0) {
    d_subGraph->simulationRemove();
    for (VertexPtr ptr: d_inConnections) {
/** @author Theossr <feolab05@gmail.com> */
      if (ptr->getValue() != ValueStates::UndefindedState) {
        ptr->removeValue();
      }
    }
  } else {
#ifdef LOGFLAG
/** @author Theossr <feolab05@gmail.com> */
    LOG(ERROR) << "Error, SubGraph without inputs" << std::endl;
#else
    std::cerr << "Error, SubGraph without inputs" << std::endl;
#endif
  }
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
void GraphVertexSubGraph::updateLevel() {
  int counter = 0, max_inLevel = 0;
  if (d_needUpdate != VS_NOT_CALC) {
    return;
  }
  d_needUpdate = VS_IN_PROGRESS;
  d_subGraph->updateLevels();
  std::vector<VertexPtr> output_verts =
/** @author Чернявских Илья Игоревич <fuuulkrum7@gmail.com> */
      d_subGraph->getVerticesByType(VertexTypes::output);
  for (VertexPtr vert: d_inConnections) {
    max_inLevel =
        (vert->getLevel() > max_inLevel) ? vert->getLevel() : max_inLevel;
  }
  for (int i = 0; i < d_outConnections.size(); ++i) {
    assert(d_outConnections.at(i)->getType() == gate);
    GraphVertexGates *out_connectionVert =
        static_cast<GraphVertexGates *>(d_outConnections.at(i));
    VertexPtr output_vert = output_verts.at(i);
    out_connectionVert->d_level = output_vert->getLevel() + max_inLevel - 2;
  }
  d_needUpdate = VS_CALC;
}

bool GraphVertexSubGraph::toVerilog(std::string i_path,
                                    std::string i_filename) {
  if (auto parentPtr = d_baseGraph.lock()) {
    d_subGraph->setCurrentParent(parentPtr);
  } else {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    throw std::invalid_argument("Dead pointer!");
  }

  return d_subGraph->toVerilog(i_path, i_filename);
}

/** @author Vladimir Zunin <vzunin@hse.ru> */
DotReturn GraphVertexSubGraph::toDOT() {
  auto parentPtr = d_baseGraph.lock();
  if (parentPtr) {
    d_subGraph->setCurrentParent(parentPtr);
  } else {
/** @author Vladimir Zunin <vzunin@hse.ru> */
    throw std::invalid_argument("Dead pointer!");
  }
  uint64_t dotCount = parentPtr->getGraphInstDOT(d_subGraph->getID());
  std::string instName =
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
      parentPtr->getName() + "_inst_" + std::to_string(dotCount);
  DotReturn dot = d_subGraph->toDOT();

  dot[0].first = DotTypes::DotSubGraph;
  dot[0].second["instName"] = instName;
  for (int i = 0; i < dot.size(); i++) {
    dot[i].second["name"] = instName + "_" + dot[i].second["name"];
    if (dot[i].second.find("from") != dot[i].second.end()) {
      dot[i].second["from"] = instName + "_" + dot[i].second["from"];
      dot[i].second["to"] = instName + "_" + dot[i].second["to"];
    }
  }

  return dot;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
bool GraphVertexSubGraph::toDOT(std::string i_path, std::string i_filename) {
  if (auto parentPtr = d_baseGraph.lock()) {
    d_subGraph->setCurrentParent(parentPtr);
  } else {
/** @author Vladimir Zunin <vzunin@hse.ru> */
    throw std::invalid_argument("Dead pointer!");
  }

  return d_subGraph->toDOT(i_path, i_filename);
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
bool GraphVertexSubGraph::toGraphML(std::ofstream &i_fileStream) const {
  return d_subGraph->toGraphMLClassic(i_fileStream);
}

std::string GraphVertexSubGraph::toGraphML(uint16_t i_indent,
                                           std::string i_prefix) const {
  return d_subGraph->toGraphMLClassic(i_indent, i_prefix);
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
std::string GraphVertexSubGraph::toVerilog() const {
  auto base = d_baseGraph.lock();
  uint64_t verilogCount = base->getGraphInstVerilog(d_subGraph->getID());

  std::string verilogTab = "  ";
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  std::string nameSub = base->getName();
  // module_name module_name_inst_1 (
  std::string module_ver = verilogTab + nameSub + " " + nameSub + "_inst_" +
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
                           std::to_string(verilogCount) + " (\n";

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  auto &&inputs = d_subGraph->getVerticesByType(VertexTypes::input);
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  auto &&outputs = d_subGraph->getVerticesByType(VertexTypes::output);
  for (size_t i = 0; i < inputs.size(); ++i) {
    VertexPtr inp = d_inConnections[i];
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    std::string inp_name = inputs[i]->getName();

    module_ver += verilogTab + verilogTab + "." + inp_name + "( ";
    module_ver += inp->getName() + " ),\n";
  }

  for (size_t i = 0; i < outputs.size() - 1; ++i) {
    VertexPtr out = d_outConnections[i];
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    std::string out_name = outputs[i]->getName();

    module_ver += verilogTab + verilogTab + "." + out_name + "( ";
    module_ver += out->getName() + " ),\n";
  }

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  std::string out_name = outputs.back()->getName();

  module_ver += verilogTab + verilogTab + "." + out_name + "( ";
  module_ver += d_outConnections.back()->getName() + " )\n";
  module_ver += verilogTab + "); \n";

  return module_ver;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
void GraphVertexSubGraph::setSubGraph(GraphPtr i_subGraph) {
  d_subGraph = i_subGraph;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
GraphPtr GraphVertexSubGraph::getSubGraph() const {
  return d_subGraph;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
size_t GraphVertexSubGraph::calculateHash() {
  if (d_hasHash) {
    return d_hashed;
  }
  // calc hash from subgraph
  std::string hashedStr =
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
      d_subGraph->calculateHash() + std::to_string(d_outConnections.size());

  d_hasHash = HC_IN_PROGRESS;
  std::vector<size_t> hashed_data;
  hashed_data.reserve(d_inConnections.size());

  for (auto *child: d_inConnections) {
    hashed_data.push_back(child->calculateHash());
  }
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  std::sort(hashed_data.begin(), hashed_data.end());

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  hashedStr.reserve(sizeof(decltype(hashed_data)::value_type) *
                    hashed_data.size());
  for (const auto &sub: hashed_data) {
    hashedStr += sub;
  }
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  d_hashed = std::hash<std::string>{}(hashedStr);
  d_hasHash = HC_CALC;

  return d_hashed;
}

std::vector<VertexPtr> GraphVertexSubGraph::getOutputBuffersByOuterInput(
    VertexPtr i_outerInput) const {
  size_t inputIndex = SIZE_MAX;
  for (size_t i = 0; i < d_inConnections.size(); ++i) {
    if (d_inConnections.at(i) == i_outerInput) {
      inputIndex = i;
      break;
    }
  }
  if (inputIndex == SIZE_MAX) {
    throw std::invalid_argument(
        "Provided outer input does not correspond to this subgraph.");
  }
  VertexPtr sgInput =
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
      d_subGraph->getBaseVertexes().at(VertexTypes::input).at(inputIndex);
  std::vector<VertexPtr> sgAllOutputs =
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
      d_subGraph->getBaseVertexes().at(VertexTypes::output);
  std::vector<VertexPtr> outputs;

  std::unordered_set<VertexPtr> visited;
  std::stack<VertexPtr> stck;
  stck.push(sgInput);

  while (!stck.empty()) {
    VertexPtr current = stck.top();
    stck.pop();

    if (visited.find(current) == visited.end()) {
      visited.insert(current);

      for (auto *v: current->getOutConnections()) {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
        if (v->getType() == VertexTypes::output) {
          for (size_t i = 0; i < sgAllOutputs.size(); ++i) {
            if (sgAllOutputs[i] == v) {
              outputs.push_back(d_outConnections.at(i));
            }
          }
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
        } else if (v->getType() != VertexTypes::subGraph) {
          stck.push(v);
        } else {
          auto *subGraphPtr = static_cast<GraphVertexSubGraph *>(v);
          for (auto *buf: subGraphPtr->getOutputBuffersByOuterInput(current)) {
            stck.push(buf);
          }
        }
      }
    }
  }
  return outputs;
}

std::vector<VertexPtr> GraphVertexSubGraph::getOuterInputsByOutputBuffer(
    VertexPtr i_outputBuffer) const {
  size_t bufferIndex = SIZE_MAX;
  for (size_t i = 0; i < d_outConnections.size(); ++i) {
    if (d_outConnections.at(i) == i_outputBuffer) {
      bufferIndex = i;
      break;
    }
  }
  if (bufferIndex == SIZE_MAX) {
    throw std::invalid_argument(
        "Provided output buffer does not correspond to this subgraph.");
  }
  VertexPtr sgOutput =
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
      d_subGraph->getBaseVertexes().at(VertexTypes::output).at(bufferIndex);
  std::vector<VertexPtr> sgAllInputs =
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
      d_subGraph->getBaseVertexes().at(VertexTypes::input);
  std::vector<VertexPtr> inputs;

  std::unordered_set<VertexPtr> visited;
  std::stack<VertexPtr> stck;
  stck.push(sgOutput);

  while (!stck.empty()) {
    VertexPtr current = stck.top();
    stck.pop();

    if (visited.find(current) == visited.end()) {
      visited.insert(current);

      for (auto *ptr: current->getInConnections()) {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
        if (ptr->getType() == VertexTypes::input) {
          for (size_t i = 0; i < sgAllInputs.size(); ++i) {
            if (sgAllInputs[i] == ptr) {
              inputs.push_back(d_inConnections.at(i));
            }
          }
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
        } else if (ptr->getType() != VertexTypes::subGraph) {
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
  return inputs;
}

#ifdef LOGFLAG
/** @author Vladimir Zunin <vzunin@hse.ru> */
void GraphVertexSubGraph::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: "
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
     << GraphUtils::parseVertexToString(VertexTypes::subGraph) << "\n";
  os << "Vertex Hash: " << d_hashed;
  os << *d_subGraph;
}
#endif

} // namespace CG_Graph
