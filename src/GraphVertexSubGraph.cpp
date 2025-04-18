#include <iostream>
#include <memory>
#include <stack>
#include <unordered_set>

#include <CircuitGenGraph/GraphVertex.hpp>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CG_Graph {

GraphVertexSubGraph::GraphVertexSubGraph(GraphPtr i_subGraph,
                                         GraphPtr i_baseGraph) :
    GraphVertexBase(VertexTypes::subGraph, i_baseGraph) {
  d_subGraph = i_subGraph;
}

GraphVertexSubGraph::GraphVertexSubGraph(GraphPtr i_subGraph,
                                         std::string_view i_name,
                                         GraphPtr i_baseGraph) :
    GraphVertexBase(VertexTypes::subGraph, i_name, i_baseGraph) {
  d_subGraph = i_subGraph;
}

// Simulation rework

char GraphVertexSubGraph::updateValue() {
  return 'x';
}

// char GraphVertexSubGraph::updateValue() {
//   std::vector<VertexPtr> output_verts =
//   d_subGraph->getVerticesByType(VertexTypes::output); for (int i = 0; i <
//   d_outConnections.size(); ++i) {
//     assert(d_outConnections[i]->getType() == gate);
//     GraphVertexGates *out_connectionVert = static_cast<GraphVertexGates
//     *>(d_outConnections[i]); VertexPtr output_vert = output_verts[i];
//     out_connectionVert->d_value = output_vert->getValue();
//   }
// }

void GraphVertexSubGraph::updateLevel(bool i_recalculate, std::string tab) {
  int counter = 0, max_inLevel = 0;
  if (d_needUpdate && !i_recalculate) {
    return;
  }
  d_needUpdate = VS_IN_PROGRESS;
  d_subGraph->updateLevels(i_recalculate);
  std::vector<VertexPtr> output_verts =
      d_subGraph->getVerticesByType(VertexTypes::output);
  for (VertexPtr vert: d_inConnections) {
    max_inLevel =
        (vert->getLevel() > max_inLevel) ? vert->getLevel() : max_inLevel;
  }
  for (int i = 0; i < d_outConnections.size(); ++i) {
    assert(d_outConnections[i]->getType() == gate);
    GraphVertexGates *out_connectionVert =
        static_cast<GraphVertexGates *>(d_outConnections[i]);
    VertexPtr output_vert = output_verts[i];
    out_connectionVert->d_level = output_vert->getLevel() + max_inLevel - 2;
  }
  d_needUpdate = VS_CALC;
}

std::pair<bool, std::string>
GraphVertexSubGraph::toVerilog(std::string i_path, std::string i_filename) {
  if (auto parentPtr = d_baseGraph.lock()) {
    d_subGraph->setCurrentParent(parentPtr);
  } else {
    throw std::invalid_argument("Dead pointer!");
  }

  return d_subGraph->toVerilog(i_path, i_filename);
}

DotReturn GraphVertexSubGraph::toDOT() {
  if (auto parentPtr = d_baseGraph.lock()) {
    d_subGraph->setCurrentParent(parentPtr);
  } else {
    throw std::invalid_argument("Dead pointer!");
  }

  return d_subGraph->getGraphDotInstance();
}

std::pair<bool, std::string>
GraphVertexSubGraph::toDOT(std::string i_path, std::string i_filename) {
  if (auto parentPtr = d_baseGraph.lock()) {
    d_subGraph->setCurrentParent(parentPtr);
  } else {
    throw std::invalid_argument("Dead pointer!");
  }

  return d_subGraph->toDOT(i_path, i_filename);
}

bool GraphVertexSubGraph::toGraphML(std::ofstream &i_fileStream) const {
  return d_subGraph->toGraphMLClassic(i_fileStream);
}

std::string GraphVertexSubGraph::toGraphML(uint16_t i_indent,
                                           std::string i_prefix) const {
  return d_subGraph->toGraphMLClassic(i_indent, i_prefix);
}

std::string GraphVertexSubGraph::toVerilog() const {
  return "DO NOT CALL IT";
}

void GraphVertexSubGraph::setSubGraph(GraphPtr i_subGraph) {
  d_subGraph = i_subGraph;
}

GraphPtr GraphVertexSubGraph::getSubGraph() const {
  return d_subGraph;
}

size_t GraphVertexSubGraph::calculateHash(bool i_recalculate) {
  if (d_hasHash && (!i_recalculate || d_hasHash == HC_IN_PROGRESS)) {
    return d_hashed;
  }
  // calc hash from subgraph
  std::string hashedStr =
      d_subGraph->calculateHash() + std::to_string(d_outConnections.size());

  d_hasHash = HC_IN_PROGRESS;
  std::vector<size_t> hashed_data;
  hashed_data.reserve(d_inConnections.size());

  for (auto *child: d_inConnections) {
    hashed_data.push_back(child->calculateHash(i_recalculate));
  }
  std::sort(hashed_data.begin(), hashed_data.end());

  hashedStr.reserve(sizeof(decltype(hashed_data)::value_type) *
                    hashed_data.size());
  for (const auto &sub: hashed_data) {
    hashedStr += sub;
  }
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
      d_subGraph->getBaseVertexes().at(VertexTypes::input).at(inputIndex);
  std::vector<VertexPtr> sgAllOutputs =
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
        if (v->getType() == VertexTypes::output) {
          for (size_t i = 0; i < sgAllOutputs.size(); ++i) {
            if (sgAllOutputs[i] == v) {
              outputs.push_back(d_outConnections.at(i));
            }
          }
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
      d_subGraph->getBaseVertexes().at(VertexTypes::output).at(bufferIndex);
  std::vector<VertexPtr> sgAllInputs =
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
        if (ptr->getType() == VertexTypes::input) {
          for (size_t i = 0; i < sgAllInputs.size(); ++i) {
            if (sgAllInputs[i] == ptr) {
              inputs.push_back(d_inConnections.at(i));
            }
          }
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
void GraphVertexSubGraph::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: "
     << GraphUtils::parseVertexToString(VertexTypes::subGraph) << "\n";
  os << "Vertex Hash: " << d_hashed;
  os << *d_subGraph;
}
#endif

} // namespace CG_Graph
