/**
 * @file GraphVertexSubGraph.cpp
 * @brief Реализация вершины-подграфа.
 */

#include "CircuitGenGraph/GraphVertexBase.hpp"
#include <CircuitGenGraph/GraphUtils.hpp>
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
  if (d_inConnections.empty()) {
    CG_LOG_ERROR << "Error, SubGraph without inputs" << std::endl;
    d_simOutputs.clear();
    return (d_value = ValueStates::NoSignal);
  }

  // Already evaluated this vector (parent invalidateValue sets Undefined).
  if (d_value != ValueStates::UndefinedState)
    return d_value;

  // Always collect every driver. Skipping non-Undefined values left
  // inputsValues shorter than the nested graph's inputs (parent
  // graphSimulation already set 0/1 on wires), so nested simulation broke.
  std::vector<char> inputsValues;
  inputsValues.reserve(d_inConnections.size());
  for (VertexPtr in: d_inConnections)
    inputsValues.push_back(in->updateValue());

  d_simOutputs = d_subGraph->graphSimulation(std::move(inputsValues));
  if (d_simOutputs.empty()) {
    CG_LOG_ERROR << "Error, SubGraph without outputs" << std::endl;
    return (d_value = ValueStates::NoSignal);
  }

  // GateBufs read per-buffer values via bufferedOutputValue (not getValue()).
  return (d_value = d_simOutputs.front());
}

char GraphVertexSubGraph::bufferedOutputValue(VertexPtr i_buffer) const {
  for (size_t i = 0; i < d_outConnections.size(); ++i) {
    if (d_outConnections[i] != i_buffer)
      continue;
    if (i < d_simOutputs.size())
      return d_simOutputs[i];
    return ValueStates::NoSignal;
  }
  return ValueStates::NoSignal;
}

void GraphVertexSubGraph::removeValue() {
  d_value = ValueStates::UndefinedState;
  d_simOutputs.clear();
  if (!d_subGraph) {
    CG_LOG_ERROR << "Error, SubGraph without nested graph" << std::endl;
    return;
  }
  // Only clear the nested simulation. Parent drivers in d_inConnections are
  // shared wires of the outer graph and must not be cleared here — that
  // orphaned values for other fanouts of the same inputs.
  d_subGraph->simulationRemove();
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
  // Instance level must be > 0 so removeWasteVertices keeps live subgraphs.
  d_level = static_cast<uint32_t>(max_inLevel + 1);
  for (size_t i = 0; i < d_outConnections.size(); ++i) {
    assert(d_outConnections.at(i)->getType() == gate);
    GraphVertexGates *out_connectionVert =
        static_cast<GraphVertexGates *>(d_outConnections.at(i));
    VertexPtr output_vert = output_verts.at(i);
    // Nested output level is relative to nested inputs at 0; avoid unsigned
    // underflow from the former `+ max_inLevel - 2` formula.
    const size_t nested = output_vert->getLevel();
    const size_t rel = nested > 0 ? nested : 1;
    out_connectionVert->d_level =
        static_cast<uint32_t>(max_inLevel + rel);
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

std::string GraphVertexSubGraph::toVerilog() const {
  auto base = d_baseGraph.lock();
  uint64_t verilogCount = base->getGraphInstVerilog(d_subGraph->getID());

  std::string verilogTab = "  ";
  // Instantiate the nested module, not the parent graph name.
  const std::string moduleName = d_subGraph->getName();
  const std::string instName =
      moduleName + "_inst_" + std::to_string(verilogCount);
  std::string module_ver =
      verilogTab + moduleName + " " + instName + " (\n";

  auto &&inputs = d_subGraph->getVerticesByType(VertexTypes::input);
  auto &&outputs = d_subGraph->getVerticesByType(VertexTypes::output);
  if (outputs.empty()) {
    CG_LOG_ERROR << "Error, SubGraph without outputs in toVerilog" << std::endl;
    return "";
  }
  for (size_t i = 0; i < inputs.size(); ++i) {
    VertexPtr inp = d_inConnections[i];
    const std::string inp_name(inputs[i]->getRawName());

    module_ver += verilogTab + verilogTab + "." + inp_name + "( ";
    module_ver += std::string(inp->getRawName()) + " ),\n";
  }

  for (size_t i = 0; i + 1 < outputs.size(); ++i) {
    VertexPtr out = d_outConnections[i];
    const std::string out_name(outputs[i]->getRawName());

    module_ver += verilogTab + verilogTab + "." + out_name + "( ";
    module_ver += std::string(out->getRawName()) + " ),\n";
  }

  const std::string out_name(outputs.back()->getRawName());

  module_ver += verilogTab + verilogTab + "." + out_name + "( ";
  module_ver += std::string(d_outConnections.back()->getRawName()) + " )\n";
  module_ver += verilogTab + "); \n";

  return module_ver;
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
  d_hasHash = HC_IN_PROGRESS;
  std::vector<size_t> hashed_data;
  hashed_data.reserve(d_inConnections.size());

  for (auto *child: d_inConnections) {
    hashed_data.push_back(child->calculateHash());
  }
  std::sort(hashed_data.begin(), hashed_data.end());

  // OrientedGraph::calculateHash returns a decimal string of the size_t hash.
  const std::string subGraphHash = d_subGraph->calculateHash();
  size_t nested = 0;
  try {
    nested = static_cast<size_t>(std::stoull(subGraphHash));
  } catch (const std::exception &) {
    nested = std::hash<std::string>{}(subGraphHash);
  }

  size_t h = 0;
  hashCombine(h, static_cast<size_t>(getType()));
  hashCombine(h, nested);
  hashCombine(h, d_outConnections.size());
  for (const auto &sub: hashed_data) {
    hashCombine(h, sub);
  }
  d_hashed = h;
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
