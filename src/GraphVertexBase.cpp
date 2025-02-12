#include <algorithm>
#include <iostream>
#include <string>

#include <CircuitGenGraph/GraphVertexBase.hpp>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

std::string VertexUtils::gateToString(Gates i_type) {
  switch (i_type) {
    case Gates::GateNot:
      return "~";
    case Gates::GateAnd:
    case Gates::GateNand:
      return "&";
    case Gates::GateOr:
    case Gates::GateNor:
      return "|";
    case Gates::GateXor:
    case Gates::GateXnor:
      return "^";
    case Gates::GateBuf:
      return "";
      // Default
    default:
      return "Error";
  }
}

std::string VertexUtils::vertexTypeToVerilog(VertexTypes i_type) {
  switch (i_type) {
    case VertexTypes::input:
      return "input";
    case VertexTypes::output:
      return "output";
    case VertexTypes::constant:
      return "localparam";
    case VertexTypes::gate:
      return "wire";
    default:
      return "Not callable";
  }
  return "";
}

std::string VertexUtils::vertexTypeToComment(VertexTypes i_type) {
  switch (i_type) {
    case VertexTypes::input:
      return "// Writing inputs";
    case VertexTypes::output:
      return "// Writing outputs";
    case VertexTypes::constant:
      return "// Writing consts";
    case VertexTypes::gate:
      return "// Writing gates";
    default:
      return "// Not writable vertex type";
  }
  return "// Unknown vertex";
}

GraphVertexBase::GraphVertexBase(const VertexTypes i_type, GraphPtr i_graph) {
  d_baseGraph = i_graph;
  d_type = i_type;
  d_name = i_graph->internalize(this->getTypeName() + "_" +
                                std::to_string(VertexUtils::d_count++));
  d_value = 'x';
  d_level = 0;
}

GraphVertexBase::GraphVertexBase(const VertexTypes i_type,
                                 std::string_view i_name, GraphPtr i_graph) {
  d_baseGraph = i_graph;
  d_type = i_type;
  if (i_name.size()) {
    d_name = i_name;
  } else {
    if (!i_graph) {
      throw std::invalid_argument(
          "Graph name is empty, and pointer on graph is empty, so string_view "
          "name cannot be created");
    }
    d_name = i_graph->internalize(this->getTypeName() + "_" +
                                  std::to_string(VertexUtils::d_count++));
  }
  d_value = 'x';
  d_level = 0;
}

GraphVertexBase::~GraphVertexBase() {
}

VertexTypes GraphVertexBase::getType() const {
  return d_type;
}

std::string GraphVertexBase::getTypeName() const {
  return GraphUtils::parseVertexToString(d_type);
}

void GraphVertexBase::setName(const std::string_view i_name) {
  d_name = i_name;
}

std::string GraphVertexBase::getName() const {
  return std::string(d_name);
}

std::string_view GraphVertexBase::getRawName() const {
  return d_name;
}

std::string GraphVertexBase::getName(const std::string &i_prefix) const {
  return i_prefix + std::string(d_name);
}

void GraphVertexBase::setLevel(const uint32_t i_level) {
  d_level = i_level;
}

uint32_t GraphVertexBase::getLevel() const {
  return d_level;
}

void GraphVertexBase::updateLevel(bool i_recalculate, std::string tab) {
  int counter = 0;
  if (d_needUpdate && (!i_recalculate || d_needUpdate == 2)) {
    return;
  }
  d_needUpdate = 2;
  for (VertexPtr vert: d_inConnections) {
#ifdef LOGLFLAG
    LOG(INFO) << tab << counter++ << ". " << vert->getName() << " ("
              << vert->getTypeName() << ")";
#endif
    vert->updateLevel(i_recalculate, tab + "  ");
    d_level = (vert->getLevel() >= d_level) ? vert->getLevel() + 1 : d_level;
  }
  d_needUpdate = 1;
}

char GraphVertexBase::getValue() const {
  return d_value;
}

GraphPtrWeak GraphVertexBase::getBaseGraph() const {
  return d_baseGraph;
}

size_t GraphVertexBase::calculateHash(bool i_recalculate) {
  if (d_hasHash && (!i_recalculate || d_hasHash == IN_PROGRESS)) {
    return d_hashed;
  }
  if (d_type == VertexTypes::input) {
    d_hashed = std::hash<size_t>{}(d_outConnections.size());
    d_hasHash = CALC;
    return d_hashed;
  }
  d_hasHash = IN_PROGRESS;
  std::vector<size_t> hashed_data;
  hashed_data.reserve(d_inConnections.size());
  std::string hashedStr;

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
  d_hasHash = CALC;

  return d_hashed;
}

std::vector<VertexPtr> GraphVertexBase::getInConnections() const {
  return d_inConnections;
}

uint32_t GraphVertexBase::addVertexToInConnections(VertexPtr i_vert) {
  assert(i_vert != this);
  assert(d_type != input && d_type != constant);

  d_inConnections.push_back(i_vert);
  uint32_t n = 0;
  // TODO is rly needed?
  for (VertexPtr vert: d_inConnections)
    n += (vert == i_vert);
  return n;
}

bool GraphVertexBase::removeVertexToInConnections(VertexPtr i_vert,
                                                  bool i_full) {
  if (i_full) {
    bool f = false;
    for (int64_t i = d_inConnections.size() - 1; i >= 0; i--) {
      d_inConnections.erase(d_inConnections.begin() + i);
      f = true;
    }
    return f;
  }
  for (size_t i = 0; i < d_inConnections.size(); i++) {
    d_inConnections.erase(d_inConnections.begin() + i);
    return true;
  }
  return false;
}

std::vector<VertexPtr> GraphVertexBase::getOutConnections() const {
  return d_outConnections;
}

bool GraphVertexBase::addVertexToOutConnections(VertexPtr i_vert) {
  assert(i_vert != this);
  assert(d_type != output);

  size_t n = 0;
  for (VertexPtr vert: d_outConnections)
    n += (vert == i_vert);
  if (n == 0) {
    d_outConnections.push_back(i_vert);
    return true;
  }
  return false;
}

bool GraphVertexBase::removeVertexToOutConnections(VertexPtr i_vert) {
  for (size_t i = 0; i < d_outConnections.size(); i++) {
    d_outConnections.erase(d_outConnections.begin() + i);
    return true;
  }
  return false;
}

std::string GraphVertexBase::getVerilogInstance() {
  if (!d_inConnections.size()) {
#ifdef LOGFLAG
    LOG(ERROR) << "TODO: delete empty vertex instance: " << d_name << std::endl;
#else
    std::cerr << "TODO: delete empty vertex instance: " << std::endl;
#endif
    return "";
  }

  return VertexUtils::vertexTypeToVerilog(d_type) + " " + getName() + ";";
}

// TODO: what if some (more than 1) connected to output?
std::string GraphVertexBase::toVerilog() const {
  if (d_type == VertexTypes::output) {
    if (!d_inConnections.empty()) {
      return "assign " + getName() + " = " + d_inConnections.back()->getName() +
             ";";
    }
  }
  return "";
}

std::vector<std::pair<DotTypes, std::map<std::string, std::string>>>
GraphVertexBase::toDOT() {
  return {};
}

#ifdef LOGFLAG
void GraphVertexBase::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: " << GraphUtils::parseVertexToString(d_type) << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << d_level << "\n";
  os << "Vertex Hash: " << d_hashed << "\n";
}
#endif

std::ostream &operator<<(std::ostream &stream, const GraphVertexBase &vertex) {
  stream << vertex.toVerilog();
  return stream;
}
