/**
 * @file GraphVertexSubGraph.cpp
 * @brief Реализация вершины-подграфа.
 */

#include "CircuitGenGraph/GraphVertexBase.hpp"
#include <CircuitGenGraph/GraphVertex.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include <fmt/core.h>

#include <CircuitGenGraph/Logging.hpp>

namespace CG_Graph {

namespace {
void parseAndStoreVerilogParameters(const GraphPtr &graph,
                                    const std::string &filepath);
}

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

char GraphVertexSubGraph::updateValue() {
  if (d_inConnections.size() > 0) {
    std::vector<char> inputsValues, outputsValues;
    if (d_inConnections.front()->getValue() == ValueStates::UndefinedState) {
      inputsValues.push_back(d_inConnections.front()->updateValue());
    }
    for (size_t i = 1; i < d_inConnections.size(); ++i) {
      if (d_inConnections.at(i)->getValue() == ValueStates::UndefinedState) {
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
  CG_LOG_ERROR << "Error, SubGraph without inputs" << std::endl;
  return ValueStates::NoSignal;
}

void GraphVertexSubGraph::removeValue() {
  if (d_inConnections.size() > 0) {
    d_subGraph->simulationRemove();
    for (VertexPtr ptr: d_inConnections) {
      if (ptr->getValue() != ValueStates::UndefinedState) {
        ptr->removeValue();
      }
    }
  } else {
    CG_LOG_ERROR << "Error, SubGraph without inputs" << std::endl;
  }
}

void GraphVertexSubGraph::updateLevel() {
  size_t max_inLevel = 0;
  if (d_needUpdate != VS_NOT_CALC) {
    return;
  }
  d_needUpdate = VS_IN_PROGRESS;
  d_subGraph->updateLevels();
  std::vector<VertexPtr> output_verts =
      d_subGraph->getVerticesByType(VertexTypes::output);
  for (VertexPtr vert: d_inConnections) {
    max_inLevel =
        (vert->getLevel() > max_inLevel) ? vert->getLevel() : max_inLevel;
  }
  for (size_t i = 0; i < d_outConnections.size(); ++i) {
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
    throw std::invalid_argument("Dead pointer!");
  }

  if (!d_verilogPath.empty()) {
    parseAndStoreVerilogParameters(d_subGraph, d_verilogPath);
  }

  return d_subGraph->toVerilog(i_path, i_filename);
}
bool GraphVertexSubGraph::toVerilogBusEnabled(std::string i_path,
                                              std::string i_filename) {
  if (auto parentPtr = d_baseGraph.lock()) {
    d_subGraph->setCurrentParent(parentPtr);
  } else {
    throw std::invalid_argument("Dead pointer!");
  }

  return d_subGraph->toVerilogBusEnabled(i_path, i_filename);
}
bool GraphVertexSubGraph::toVerilogBusEnabledAsOneBit(std::string i_path,
                                                      std::string i_filename) {
  if (auto parentPtr = d_baseGraph.lock()) {
    d_subGraph->setCurrentParent(parentPtr);
  } else {
    throw std::invalid_argument("Dead pointer!");
  }

  return d_subGraph->toVerilogBusEnabledAsOneBit(i_path, i_filename);
}
DotReturn GraphVertexSubGraph::toDOT() {
  auto parentPtr = d_baseGraph.lock();
  if (parentPtr) {
    d_subGraph->setCurrentParent(parentPtr);
  } else {
    throw std::invalid_argument("Dead pointer!");
  }
  uint64_t dotCount = parentPtr->getGraphInstDOT(d_subGraph->getID());
  std::string instName =
      parentPtr->getName() + "_inst_" + std::to_string(dotCount);
  DotReturn dot = d_subGraph->toDOT();

  dot[0].first = DotTypes::DotSubGraph;
  dot[0].second["instName"] = instName;
  for (size_t i = 0; i < dot.size(); i++) {
    dot[i].second["name"] = instName + "_" + dot[i].second["name"];
    if (dot[i].second.find("from") != dot[i].second.end()) {
      dot[i].second["from"] = instName + "_" + dot[i].second["from"];
      dot[i].second["to"] = instName + "_" + dot[i].second["to"];
    }
  }

  return dot;
}

bool GraphVertexSubGraph::toDOT(std::string i_path, std::string i_filename) {
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

std::string GraphVertexSubGraph::toVerilogCommon(
    std::function<void(VertexPtr, VertexPtr, std::string &)> printPin,
    std::string &module_ver) const {
  auto base = d_baseGraph.lock();
  uint64_t verilogCount = base->getGraphInstVerilog(d_subGraph->getID());

  std::string verilogTab = "  ";
  std::string nameSub = base->getName();
  // module_name module_name_inst_1 (
  module_ver =
      fmt::format("  {} {}_inst_{} (\n", nameSub, nameSub, verilogCount);

  auto &&inputs = d_subGraph->getVerticesByType(VertexTypes::input);
  auto &&outputs = d_subGraph->getVerticesByType(VertexTypes::output);
  for (size_t i = 0; i < inputs.size(); ++i) {
    std::string inp_name = inputs[i]->getName();

    printPin(inputs[i], d_inConnections[i], module_ver);
  }

  for (size_t i = 0; i < outputs.size() - 1; ++i) {
    std::string out_name = outputs[i]->getName();

    printPin(outputs[i], d_outConnections[i], module_ver);
  }

  std::string out_name = outputs.back()->getName();
  module_ver += fmt::format("    .{}( {} )\n  );\n", out_name,
                            d_outConnections.back()->getName());
  return module_ver;
};
std::string GraphVertexSubGraph::toVerilog() const {
  // auto base = d_baseGraph.lock();
  // uint64_t verilogCount = base->getGraphInstVerilog(d_subGraph->getID());

  // std::string verilogTab = "  ";
  // std::string nameSub = base->getName();
  // // module_name module_name_inst_1 (
  // std::string module_ver = verilogTab + nameSub + " " + nameSub + "_inst_" +
  //                          std::to_string(verilogCount) + " (\n";

  // auto &&inputs = d_subGraph->getVerticesByType(VertexTypes::input);
  // auto &&outputs = d_subGraph->getVerticesByType(VertexTypes::output);
  // for (size_t i = 0; i < inputs.size(); ++i) {
  //   VertexPtr inp = d_inConnections[i];
  //   std::string inp_name = inputs[i]->getName();

  //   module_ver += verilogTab + verilogTab + "." + inp_name + "( ";
  //   module_ver += inp->getName() + " ),\n";
  // }

  // for (size_t i = 0; i < outputs.size() - 1; ++i) {
  //   VertexPtr out = d_outConnections[i];
  //   std::string out_name = outputs[i]->getName();

  //   module_ver += verilogTab + verilogTab + "." + out_name + "( ";
  //   module_ver += out->getName() + " ),\n";
  // }

  // std::string out_name = outputs.back()->getName();

  // module_ver += verilogTab + verilogTab + "." + out_name + "( ";
  // module_ver += d_outConnections.back()->getName() + " )\n";
  // module_ver += verilogTab + "); \n";

  // return module_ver;
  std::string module_ver;
  auto printPin = [&](VertexPtr innerPinI, VertexPtr connectionI,
                      std::string &module_ver) {
    module_ver += fmt::format("    .{}( {} ),\n", innerPinI->getName(),
                              connectionI->getName());
  };
  return toVerilogCommon(printPin, module_ver);
}

std::string GraphVertexSubGraph::toVerilogBusEnabledAsOneBit() const {
  std::string module_ver;
  auto printPin = [&](VertexPtr innerPinI, VertexPtr connectionI,
                      std::string &module_ver) {
    if (innerPinI->isBus()) {
      for (size_t j = 0;
           j < GraphVertexBus::getBusPointer(innerPinI)->getWidth(); ++j) {
        module_ver +=
            fmt::format("    .{}_{}( {}_{} ),\n", innerPinI->getName(), j,
                        connectionI->getName(), j);
      }
    } else
      module_ver += fmt::format("    .{}( {} ),\n", innerPinI->getName(),
                                connectionI->getName());
  };
  return toVerilogCommon(printPin, module_ver);
}

void GraphVertexSubGraph::setSubGraph(GraphPtr i_subGraph) {
  d_subGraph = i_subGraph;
}

GraphPtr GraphVertexSubGraph::getSubGraph() const {
  return d_subGraph;
}

size_t GraphVertexSubGraph::calculateHash() {
  if (d_hasHash) {
    return d_hashed;
  }
  // calc hash from subgraph
  std::string hashedStr =
      d_subGraph->calculateHash() + std::to_string(d_outConnections.size());

  d_hasHash = HC_IN_PROGRESS;
  std::vector<size_t> hashed_data;
  hashed_data.reserve(d_inConnections.size());

  for (auto *child: d_inConnections) {
    hashed_data.push_back(child->calculateHash());
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

namespace {
const std::set<std::string> ignoredPortKeywords = {
    "wire",    "tri",     "tri0",   "tri1",    "wand",     "wor",
    "triand",  "trior",   "trireg", "supply0", "supply1",  "uwire",
    "reg",     "logic",   "bit",    "byte",    "shortint", "int",
    "longint", "integer", "time",   "signed",  "unsigned"};

const std::set<std::string> ignoredParameterKeywords = {
    "wire",       "tri",     "tri0",    "tri1",    "wand",     "wor",
    "triand",     "trior",   "trireg",  "supply0", "supply1",  "uwire",
    "reg",        "logic",   "bit",     "byte",    "shortint", "int",
    "longint",    "integer", "time",    "signed",  "unsigned", "parameter",
    "localparam", "real",    "realtime"};

std::string trimWhitespace(const std::string &s) {
  const auto begin = s.find_first_not_of(" \t\n\r");
  if (begin == std::string::npos) {
    return "";
  }
  const auto end = s.find_last_not_of(" \t\n\r");
  return s.substr(begin, end - begin + 1);
}

std::string readVerilogFileWithoutLineComments(const std::string &filepath) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open Verilog file: " + filepath);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content = buffer.str();

  std::string clean;
  std::istringstream iss(content);
  std::string line;
  while (std::getline(iss, line)) {
    auto comment_pos = line.find("//");
    if (comment_pos != std::string::npos) {
      line = line.substr(0, comment_pos);
    }
    clean += line + "\n";
  }
  return clean;
}

void appendPortNames(const std::string &text, std::vector<std::string> &ports) {
  const std::regex nameRegex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
  for (auto it = std::sregex_iterator(text.begin(), text.end(), nameRegex);
       it != std::sregex_iterator(); ++it) {
    const std::string name = it->str();
    if (ignoredPortKeywords.find(name) == ignoredPortKeywords.end()) {
      ports.push_back(name);
    }
  }
}

void collectPortsFromANSIDeclaration(const std::string &decl,
                                     VerilogPorts &ports) {
  const std::regex inputWord(R"(\binput\b)");
  const std::regex outputWord(R"(\boutput\b)");
  std::vector<std::string> *currentPortList = nullptr;
  std::stringstream ss(decl);
  std::string segment;

  while (std::getline(ss, segment, ',')) {
    std::string token = trimWhitespace(segment);
    if (token.empty()) {
      continue;
    }

    std::smatch inputMatch;
    std::smatch outputMatch;
    const bool hasInput = std::regex_search(token, inputMatch, inputWord);
    const bool hasOutput = std::regex_search(token, outputMatch, outputWord);

    if (hasInput &&
        (!hasOutput || inputMatch.position() < outputMatch.position())) {
      currentPortList = &ports.inputs;
      token = token.substr(inputMatch.position() + inputMatch.length());
    } else if (hasOutput) {
      currentPortList = &ports.outputs;
      token = token.substr(outputMatch.position() + outputMatch.length());
    }

    if (currentPortList != nullptr) {
      appendPortNames(token, *currentPortList);
    }
  }
}

void collectPortNamesByType(const GraphPtr &graph, const VertexTypes i_type,
                            std::set<std::string> &ports) {
  const auto vertices = graph->getVerticesByType(i_type);
  for (const auto *vertex: vertices) {
    ports.insert(std::string(vertex->getRawName()));
  }
}

std::vector<std::string> splitTopLevelByComma(const std::string &text) {
  std::vector<std::string> chunks;
  size_t chunkBegin = 0;
  int roundDepth = 0;
  int squareDepth = 0;
  int braceDepth = 0;
  bool insideDoubleQuote = false;

  for (size_t i = 0; i < text.size(); ++i) {
    const char symbol = text[i];

    if (symbol == '\"' && (i == 0 || text[i - 1] != '\\')) {
      insideDoubleQuote = !insideDoubleQuote;
      continue;
    }

    if (insideDoubleQuote) {
      continue;
    }

    switch (symbol) {
      case '(':
        ++roundDepth;
        break;
      case ')':
        roundDepth = std::max(0, roundDepth - 1);
        break;
      case '[':
        ++squareDepth;
        break;
      case ']':
        squareDepth = std::max(0, squareDepth - 1);
        break;
      case '{':
        ++braceDepth;
        break;
      case '}':
        braceDepth = std::max(0, braceDepth - 1);
        break;
      case ',':
        if (roundDepth == 0 && squareDepth == 0 && braceDepth == 0) {
          chunks.push_back(text.substr(chunkBegin, i - chunkBegin));
          chunkBegin = i + 1;
        }
        break;
      default:
        break;
    }
  }

  chunks.push_back(text.substr(chunkBegin));
  return chunks;
}

std::string extractParameterName(const std::string &text) {
  const std::regex nameRegex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
  std::string parameterName;

  for (auto it = std::sregex_iterator(text.begin(), text.end(), nameRegex);
       it != std::sregex_iterator(); ++it) {
    const std::string token = it->str();
    if (ignoredParameterKeywords.find(token) ==
        ignoredParameterKeywords.end()) {
      parameterName = token;
    }
  }

  return parameterName;
}

void collectParameterAssignments(
    const std::string &declaration,
    std::vector<std::pair<std::string, std::string>> &parameters) {
  for (const auto &segment: splitTopLevelByComma(declaration)) {
    const std::string token = trimWhitespace(segment);
    if (token.empty()) {
      continue;
    }

    const auto eqPos = token.find('=');
    if (eqPos == std::string::npos) {
      continue;
    }

    const std::string lhs = trimWhitespace(token.substr(0, eqPos));
    const std::string rhs = trimWhitespace(token.substr(eqPos + 1));
    if (lhs.empty() || rhs.empty()) {
      continue;
    }

    const std::string name = extractParameterName(lhs);
    if (name.empty()) {
      continue;
    }

    parameters.push_back({name, rhs});
  }
}

std::vector<std::pair<std::string, std::string>>
parseVerilogParametersFromText(const std::string &clean) {
  std::vector<std::pair<std::string, std::string>> parameters;
  std::string bodyClean = clean;

  const std::regex moduleParamRegex(
      R"(module\s+\w+\s*#\s*\(([\s\S]*?)\)\s*\()");
  std::smatch moduleParamMatch;
  if (std::regex_search(clean, moduleParamMatch, moduleParamRegex)) {
    collectParameterAssignments(moduleParamMatch[1], parameters);

    const size_t headerParamPos =
        static_cast<size_t>(moduleParamMatch.position(1));
    const size_t headerParamLen =
        static_cast<size_t>(moduleParamMatch.length(1));
    bodyClean.erase(headerParamPos, headerParamLen);
  }

  const std::regex bodyParamRegex(R"(\b(?:parameter|localparam)\b([^;]*);)");
  for (std::sregex_iterator
           it(bodyClean.begin(), bodyClean.end(), bodyParamRegex),
       end;
       it != end; ++it) {
    collectParameterAssignments((*it)[1], parameters);
  }

  return parameters;
}

void parseAndStoreVerilogParameters(const GraphPtr &graph,
                                    const std::string &filepath) {
  if (!graph || filepath.empty()) {
    return;
  }

  const std::string clean = readVerilogFileWithoutLineComments(filepath);
  const auto parsedParameters = parseVerilogParametersFromText(clean);

  graph->clearVerilogParameters();
  for (const auto &parameter: parsedParameters) {
    graph->addVerilogParameter(parameter.first, parameter.second);
  }
}

} // namespace

VerilogPorts parseVerilogPorts(const std::string &filepath) {
  VerilogPorts ports;
  std::string clean = readVerilogFileWithoutLineComments(filepath);

  std::regex moduleRegex(R"(module\s+\w+\s*\(([^)]*)\)\s*;)");
  std::smatch moduleMatch;
  if (std::regex_search(clean, moduleMatch, moduleRegex)) {
    std::string ansiPorts = moduleMatch[1];
    ansiPorts.erase(std::remove(ansiPorts.begin(), ansiPorts.end(), '\n'),
                    ansiPorts.end());
    ansiPorts.erase(std::remove(ansiPorts.begin(), ansiPorts.end(), '\r'),
                    ansiPorts.end());
    collectPortsFromANSIDeclaration(ansiPorts, ports);
    clean = std::regex_replace(clean, moduleRegex, ";");
  }

  const std::regex inputStmtRegex(R"(\binput\b([^;]*);)");
  const std::regex outputStmtRegex(R"(\boutput\b([^;]*);)");
  const std::regex nameRegex(R"([a-zA-Z_][a-zA-Z0-9_]*)");

  auto appendPortsFromMatch = [&](const std::smatch &match,
                                  std::vector<std::string> &portList) {
    const std::string portsStr = match[1];
    for (auto it =
             std::sregex_iterator(portsStr.begin(), portsStr.end(), nameRegex);
         it != std::sregex_iterator(); ++it) {
      const std::string name = it->str();
      if (ignoredPortKeywords.find(name) == ignoredPortKeywords.end()) {
        portList.push_back(name);
      }
    }
  };

  for (std::sregex_iterator it(clean.begin(), clean.end(), inputStmtRegex), end;
       it != end; ++it) {
    appendPortsFromMatch(*it, ports.inputs);
  }
  for (std::sregex_iterator it(clean.begin(), clean.end(), outputStmtRegex),
       end;
       it != end; ++it) {
    appendPortsFromMatch(*it, ports.outputs);
  }

  return ports;
}

bool checkPortsMatch(const GraphPtr &graph, const VerilogPorts &verilogPorts,
                     std::string &errorMsg) {
  std::set<std::string> gIn;
  std::set<std::string> gOut;
  collectPortNamesByType(graph, VertexTypes::input, gIn);
  collectPortNamesByType(graph, VertexTypes::output, gOut);

  std::set<std::string> vIn(verilogPorts.inputs.begin(),
                            verilogPorts.inputs.end());
  std::set<std::string> vOut(verilogPorts.outputs.begin(),
                             verilogPorts.outputs.end());

  if (gIn != vIn || gOut != vOut) {
    errorMsg = "Graph ports do not match Verilog ports.";
    return false;
  }
  errorMsg.clear();
  return true;
}

} // namespace CG_Graph
