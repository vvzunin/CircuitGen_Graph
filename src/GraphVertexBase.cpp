/**
 * @file GraphVertexBase.cpp
 * @brief Реализация базовой вершины графа и утилит VertexUtils.
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 * @author Theossr <feolab05@gmail.com>
 * @author rainbowkittensss <viktorrrrry20@gmail.com>
 * @author NonDif <shapkin.andrey123@gmail.com>
 */
#include <algorithm>
#include <iostream>
#include <string>

#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif
#include "../lib/fmt/core.h"

namespace CG_Graph {

std::atomic_uint64_t GraphVertexBase::d_count = 0ul;

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
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

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
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
    case VertexTypes::sequential:
      return "reg";
    default:
      return "Not callable";
  }
  return "";
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
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
    case VertexTypes::sequential:
      return "// Writing registers";
    default:
      return "// Not writable vertex type";
  }
  return "// Unknown vertex";
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
inline void format_comment(std::string &res, std::string_view to_format,
                           std::string_view name) {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  std::string copied = fmt::format(to_format, name);
  res.reserve(copied.size() + res.size());
  res += copied;
}

std::string
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
VertexUtils::getSequentialComment(const GraphVertexSequential *i_seq) {
  constexpr std::string_view en_comment =
      "\t// EN signal \"{}\" - when it is in a logical one "
      "state, trigger writes data to the output\n";
  constexpr std::string_view rst_comment =
      "\t// RST signal \"{}\" - when it is in a logical zero "
      "state, trigger writes logical zero to the output\n";
  constexpr std::string_view async_comment =
      "\t// RST signal \"{}\" is async - always "
      "block enables on negedge of RST\n";
  constexpr std::string_view clr_comment =
      "\t// CLR signal \"{}\" - when it is in a logical one "
      "state, trigger writes logical zero to the output\n";
  constexpr std::string_view set_comment =
      "\t// SET signal \"{}\" - when it is in a logical one "
      "state, trigger writes logical one to the output\n";

  std::string res;
  auto type = i_seq->getSeqType();

  if (type & RST) {
    format_comment(res, rst_comment, i_seq->getRst()->getRawName());
  } else if (type & CLR) {
    format_comment(res, clr_comment, i_seq->getRst()->getRawName());
  }
  if (type & ASYNC) {
    format_comment(res, async_comment, i_seq->getRst()->getRawName());
  }
  if (type & SET) {
    format_comment(res, set_comment, i_seq->getSet()->getRawName());
  }
  if (type & EN) {
    format_comment(res, en_comment, i_seq->getEn()->getRawName());
  }
  return res;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
GraphVertexBase::GraphVertexBase(const VertexTypes i_type, GraphPtr i_graph) {
  d_baseGraph = i_graph;
  d_type = i_type;
  d_name = i_graph->internalize(this->getTypeName() + "_" +
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
                                std::to_string(d_count++));
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
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
                                  std::to_string(d_count++));
  }
  d_value = 'x';
  d_level = 0;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
GraphVertexBase::~GraphVertexBase() {
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
VertexTypes GraphVertexBase::getType() const {
  return d_type;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
std::string GraphVertexBase::getTypeName() const {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  return GraphUtils::parseVertexToString(d_type);
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
void GraphVertexBase::setName(const std::string_view i_name) {
  d_name = i_name;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
std::string GraphVertexBase::getName() const {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  return std::string(d_name);
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
std::string_view GraphVertexBase::getRawName() const {
  return d_name;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
std::string GraphVertexBase::getName(const std::string &i_prefix) const {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  return i_prefix + std::string(d_name);
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
uint32_t GraphVertexBase::getLevel() const {
  return d_level;
}

/** @author Theossr <feolab05@gmail.com> */
void GraphVertexBase::removeValue() {
  d_value = ValueStates::UndefindedState;
  if (d_inConnections.empty()) {
    return;
  }
  for (VertexPtr ptr: d_inConnections) {
    ptr->removeValue();
  }
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
void GraphVertexBase::updateLevel() {
  // 2 - IN PROGRESS, 1 - HC_CALC
  // 2 == 010
  // 1 == 001
  // d_needUpdate = static_cast<MY_ENUM>(HC_CALC | ADDED); // ADDED = 4 // 100
  // 101
  int counter = 0;
  if (d_needUpdate != VS_NOT_CALC) {
    return;
  }
  d_needUpdate = VS_IN_PROGRESS;
  for (VertexPtr vert: d_inConnections) {
#ifdef LOGLFLAG
    LOG(INFO) << counter++ << ". " << vert->getName() << " ("
              << vert->getTypeName() << ")";
#endif
    vert->updateLevel();
    d_level = (vert->getLevel() >= d_level) ? vert->getLevel() + 1 : d_level;
  }
  d_needUpdate = VS_CALC;
}

bool GraphVertexBase::getVerticesByLevel(uint32_t i_level,
                                         std::vector<VertexPtr> &i_result,
                                         bool i_fromOut) {
  if (d_needUpdate & VS_USED_LEVEL)
    return true;
  if (!(d_needUpdate & VS_CALC)) {
    return false;
  }
  d_needUpdate = VS_USED_CALC;

  if (d_level == i_level) {
    i_result.push_back(this);
    return true;
  }
  bool flag = true;
  if (i_fromOut) {
    for (auto *vert: d_inConnections) {
      flag &= vert->getVerticesByLevel(i_level, i_result, i_fromOut);
    }
  } else {
    for (auto *vert: d_outConnections) {
      flag &= vert->getVerticesByLevel(i_level, i_result, i_fromOut);
    }
  }
  return flag;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
char GraphVertexBase::getValue() const {
  return d_value;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
GraphPtrWeak GraphVertexBase::getBaseGraph() const {
  return d_baseGraph;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
size_t GraphVertexBase::calculateHash() {
  if (d_hasHash) {
    return d_hashed;
  }
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  if (d_type == VertexTypes::input) {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    d_hashed = std::hash<size_t>{}(d_outConnections.size());
    d_hasHash = HC_CALC;
    return d_hashed;
  }
  d_hasHash = HC_IN_PROGRESS;
  std::vector<size_t> hashed_data;
  hashed_data.reserve(d_inConnections.size());
  std::string hashedStr;

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

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
void GraphVertexBase::reserveInConnections(size_t i_size) {
  d_inConnections.reserve(d_inConnections.size() + i_size);
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
void GraphVertexBase::reserveOutConnections(size_t i_size) {
  d_outConnections.reserve(d_outConnections.size() + i_size);
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
std::vector<VertexPtr> GraphVertexBase::getInConnections() const {
  return d_inConnections;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
uint32_t GraphVertexBase::addVertexToInConnections(VertexPtr i_vert) {
  assert(i_vert != this);
  assert(d_type != input && d_type != constant);
  uint32_t n = 0;
  d_inConnections.push_back(i_vert);
  // TODO is rly needed?
  for (VertexPtr vert: d_inConnections)
    n += (vert == i_vert);
  return n;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
std::vector<VertexPtr> GraphVertexBase::getOutConnections() const {
  return d_outConnections;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
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

// TODO: what if some (more than 1) connected to output?
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
std::string GraphVertexBase::toVerilog() const {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  if (d_type == VertexTypes::output) {
    if (!d_inConnections.empty()) {
      return "assign " + getName() + " = " + d_inConnections.back()->getName() +
             ";";
    }
  }
  return "";
}

std::vector<std::pair<DotTypes, std::map<std::string, std::string>>>
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
GraphVertexBase::toDOT() {
  return {};
}

#ifdef LOGFLAG
/** @author Vladimir Zunin <vzunin@hse.ru> */
void GraphVertexBase::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  os << "Vertex Type: " << GraphUtils::parseVertexToString(d_type) << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << d_level << "\n";
  os << "Vertex Hash: " << d_hashed << "\n";
}
#endif

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
std::ostream &operator<<(std::ostream &stream, const GraphVertexBase &vertex) {
  stream << vertex.toVerilog();
  return stream;
}

/** @author rainbowkittensss <viktorrrrry20@gmail.com> */
bool GraphVertexBase::removeVertexToInConnections(VertexPtr i_vert) {
  auto vertToRemove =
/** @author rainbowkittensss <viktorrrrry20@gmail.com> */
      std::find(d_inConnections.begin(), d_inConnections.end(), i_vert);
  if (vertToRemove == d_inConnections.end())
    return false;
/** @author rainbowkittensss <viktorrrrry20@gmail.com> */
  std::swap(*vertToRemove, *d_inConnections.rbegin());
  d_inConnections.resize(d_inConnections.size() - 1);
  return true;
}

/** @author rainbowkittensss <viktorrrrry20@gmail.com> */
bool GraphVertexBase::removeVertexToOutConnections(VertexPtr i_vert) {
  auto vertToRemove =
/** @author rainbowkittensss <viktorrrrry20@gmail.com> */
      std::find(d_outConnections.begin(), d_outConnections.end(), i_vert);
  if (vertToRemove == d_outConnections.end())
    return false;
/** @author rainbowkittensss <viktorrrrry20@gmail.com> */
  std::swap(*vertToRemove, *d_outConnections.rbegin());
  d_outConnections.resize(d_outConnections.size() - 1);
  return true;
}

} // namespace CG_Graph
