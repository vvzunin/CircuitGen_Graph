#include "CircuitGenGraph/GraphVertexBus.hpp"
#include <CircuitGenGraph/GraphUtils.hpp>
#include <CircuitGenGraph/GraphVertex.hpp>

#include <cassert>
#include <iostream>
#include <string>
#include <string_view>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif
#include "fmt/core.h"

namespace CG_Graph {

inline bool GraphVertexSequential::isFF() const {
  return d_seqType & ff;
}

inline bool GraphVertexSequential::isAsync() const {
  return d_seqType & ASYNC;
}

inline bool GraphVertexSequential::isNegedge() const {
  return d_seqType & NEGEDGE;
}

inline std::string convertSequentialFlag(SequentialTypes i_type) {
  static std::pair<SequentialTypes, std::string_view> types_seq[] = {
      {EN, "EN"},   {RST, "RST"},     {RST, "CLR"},
      {SET, "SET"}, {ASYNC, "ASYNC"}, {NEGEDGE, "NEGEDGE"}};
  return std::string(GraphUtils::findPairByKey(types_seq, i_type)->second);
}

inline bool validateSignal(SequentialTypes current, SequentialTypes found) {
  unsigned delta = current ^ found;
  if (!delta) {
    return true;
  }
  for (const auto &flag: {EN, RST, CLR, SET, NEGEDGE, ASYNC}) {
    if (delta & flag) {
      SequentialTypes foundFlag = static_cast<SequentialTypes>(delta & flag);
#ifdef LOGFLAG
      LOG(ERROR) << "Invalid flag found in used type: "
                 << convertSequentialFlag(foundFlag) << '\n';
#else
      std::cerr << "Invalid flag found in used type: "
                << convertSequentialFlag(foundFlag) << '\n';
#endif
    }
  }
  return false;
}

#define DEFAULT_CHECK_TYPE \
  do { \
    /* NOT ALLOWED TO USE BOTH RST AND CLR */ \
    assert(!((i_type & RST) && (i_type & CLR))); \
    if (i_type & ff) { \
      d_seqType = static_cast<SequentialTypes>(i_type & nff); \
    } else { /* without ff flag it is just a latch */ \
      d_seqType = latch; \
    } \
  } while (0)

unsigned short countSignalsInType(SequentialTypes i_type) {
  return bool(i_type & RST) + bool(i_type & CLR) + bool(i_type & EN) +
         bool(i_type & SET);
}
// BRAND NEW SIGNALS SEQUENCE 0 -- data, 1 -- clk, 2 -- en, 3 -- RST/CLR, 4 --
// SET
GraphVertexSequential::GraphVertexSequential(SequentialTypes i_type,
                                             VertexPtr i_clk, VertexPtr i_data,
                                             GraphPtr i_baseGraph,
                                             std::string_view i_name,
                                             bool i_isBus) :
    GraphVertexBase(VertexTypes::sequential, i_name, i_baseGraph) {
  reserveInConnections(2);
  i_baseGraph->addEdge(i_data, this);
  i_baseGraph->addEdge(i_clk, this);
  DEFAULT_CHECK_TYPE;
  validateSignal(i_type, d_seqType);
}

GraphVertexSequential::GraphVertexSequential(
    SequentialTypes i_type, VertexPtr i_clk, VertexPtr i_data, VertexPtr i_wire,
    GraphPtr i_baseGraph, std::string_view i_name, bool i_isBus) :
    GraphVertexBase(VertexTypes::sequential, i_name, i_baseGraph) {
  reserveInConnections(3);
  i_baseGraph->addEdge(i_data, this);
  i_baseGraph->addEdge(i_clk, this);
  i_baseGraph->addEdge(i_wire, this);
  DEFAULT_CHECK_TYPE;
  short signals = countSignalsInType(i_type);
  if (signals < 1)
    d_seqType = static_cast<SequentialTypes>(i_type | EN);
  else if (signals + bool(i_type & ff) > 2) {
    if (signals + bool(i_type & ff) > 3)
      d_seqType = static_cast<SequentialTypes>(i_type & ~SET & ~RST & ~CLR);
    d_seqType = static_cast<SequentialTypes>(i_type & ~SET);
  } else
    d_seqType = i_type;
  validateSignal(i_type, d_seqType);
}

GraphVertexSequential::GraphVertexSequential(
    SequentialTypes i_type, VertexPtr i_clk, VertexPtr i_data,
    VertexPtr i_wire1, VertexPtr i_wire2, GraphPtr i_baseGraph,
    std::string_view i_name, bool i_isBus) :
    GraphVertexBase(VertexTypes::sequential, i_name, i_baseGraph) {
  reserveInConnections(4);
  i_baseGraph->addEdge(i_data, this);
  i_baseGraph->addEdge(i_clk, this);
  i_baseGraph->addEdge(i_wire1, this);
  i_baseGraph->addEdge(i_wire2, this);
  DEFAULT_CHECK_TYPE;
  short signals = countSignalsInType(i_type);
  if (signals == 0) {
    d_seqType = static_cast<SequentialTypes>(i_type | EN | RST);
  } else if (signals == 1)
    d_seqType = static_cast<SequentialTypes>(i_type | EN);
  else if (signals + bool(i_type & ff) > 3)
    d_seqType = static_cast<SequentialTypes>(i_type & ~SET);
  else
    d_seqType = i_type;
  validateSignal(i_type, d_seqType);
}

GraphVertexSequential::GraphVertexSequential(
    SequentialTypes i_type, VertexPtr i_clk, VertexPtr i_data, VertexPtr i_rst,
    VertexPtr i_set, VertexPtr i_en, GraphPtr i_baseGraph,
    std::string_view i_name, bool i_isBus) :
    GraphVertexBase(VertexTypes::sequential, i_name, i_baseGraph) {
  reserveInConnections(5);
  i_baseGraph->addEdge(i_data, this);
  i_baseGraph->addEdge(i_clk, this);
  i_baseGraph->addEdge(i_en, this);
  i_baseGraph->addEdge(i_rst, this);
  i_baseGraph->addEdge(i_set, this);
  DEFAULT_CHECK_TYPE;

  // cannot have 3 input wires and be a latch - latch has only 3 signals at all
  assert(isFF());
  unsigned factType = SET | EN;
  factType |= i_type & (RST | CLR | NEGEDGE | ASYNC);

  d_seqType = static_cast<SequentialTypes>(d_seqType | factType);
  validateSignal(i_type, d_seqType);
}

#undef DEFAULT_CHECK_TYPE

// clang-format on

SequentialTypes GraphVertexSequential::getSeqType() const {
  return d_seqType;
}

VertexPtr GraphVertexSequential::getClk() const {
  if (getSeqType() & ff)
    return d_inConnections[1];
  return nullptr;
}

VertexPtr GraphVertexSequential::getData() const {
  return d_inConnections[0];
}

VertexPtr GraphVertexSequential::getEn() const {
  if (getSeqType() &
      EN) // for flip-flops EN stored in d_inConnections[2], after clk
          // and for latches it is in d finConnections[1]
    return d_inConnections[bool(getSeqType() & ff) + 1];
  return nullptr;
}

VertexPtr GraphVertexSequential::getRst() const {
  if (getSeqType() & RST | getSeqType() & CLR)
    return d_inConnections[bool(getSeqType() & ff) + bool(getSeqType() & EN) +
                           1];
  return nullptr;
}

VertexPtr GraphVertexSequential::getSet() const {
  if (getSeqType() & SET)
    return d_inConnections[bool(getSeqType() & ff) + bool(getSeqType() & EN) +
                           bool(getSeqType() & RST | getSeqType() & CLR) + 1];
  return nullptr;
}

size_t GraphVertexSequential::calculateHash() {
  if (d_hasHash) {
    return d_hashed;
  }
  // same as in gate, but adds type of a sequential vertex
  std::string hashedStr = std::to_string(d_outConnections.size()) +
                          std::to_string(getType()) + std::to_string(d_seqType);

  d_hasHash = HC_IN_PROGRESS;
  std::vector<size_t> hashed_data;
  hashed_data.reserve(d_inConnections.size());

  for (auto &child: d_inConnections) {
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

inline void
GraphVertexSequential::formatAlwaysBegin(std::string &verilog) const {
  if (isFF() && !isAsync()) {
    verilog = fmt::format("always @({} {}) begin\n",
                          isNegedge() ? "negedge" : "posedge",
                          getClk()->getRawName());
  } else if (isFF()) {
    verilog = fmt::format("always @({} {} or negedge {}) begin\n",
                          isNegedge() ? "negedge" : "posedge",
                          getClk()->getRawName(), getRst()->getRawName());
  } else {
    verilog = "always @(*) begin\n";
  }
}

inline void simpleCheckFormat(std::string &verilog,
                              std::string_view nameToCheck,
                              std::string_view data, unsigned type,
                              std::string_view tab) {
  std::string_view toFormat = "{}if ({}{}) {} <= 1'b{};\n";
  verilog += fmt::format(toFormat, tab, type & RST ? "!" : "", nameToCheck,
                         data, type & SET ? "1" : "0");
}

std::string GraphVertexSequential::toVerilog() const {
  // switch (countSignalsInType(d_seqType) + isFF()) {
  //   case 1:

  //   break;
  //   case 2:
  //   break;
  //   case 3:
  //   break;
  //   case 4:
  switch(d_seqType)
  case affr: 
  return fmt::format(const S &format_str, Args &&args...)
  break;

  // case affre: ;
  // case affrs: ;
  // case affrse: ;
  // case latchr: ;
  // case latchc: ;
  // case latchs: ;
  // case latchrs: ;
  // case latchcs: ;
  // case ffe: ;
  // case ffr: ;
  // case ffc: ;
  // case SequentialTypes::ffs: ;
  // case ffre: ;
  // case ffce: ;
  // case ffse: ;
  // case ffrs: ;
  // case ffcs: ;
  // case ffrse: ;
  // case ffcse: ;
  // case nff: ;
  // case nffe: ;
  // case nffr: ;
  // case nffc: ;
  // case SequentialTypes::nffs: ; 
  // case nffre: ;
  // case nffce: ;
  // case nffse: ;
  // case nffrs: ;
  // case nffcs: ;
  // case nffrse: ;
  // case nffcse: ;
  // case naffrs: ;
  // case naffrse: ;
  // case naffr: ;
  // case naffre: ;
  }
  // std::string verilog;
  // formatAlwaysBegin(verilog);
  // std::string_view toFormat;
  // std::string_view tab = "\t\t";
  // bool flag = false;
  // if (unsigned val = (d_seqType & RST) | (d_seqType & CLR)) {
  //   simpleCheckFormat(verilog, getRst()->getRawName(), d_name, val, tab);
  //   verilog += "\t\telse";
  //   flag = true;
  // }
  // if (d_seqType & SET) {
  //   simpleCheckFormat(verilog, getSet()->getRawName(), d_name, SET,
  //                     flag ? " " : tab);
  //   verilog += "\t\telse";
  //   flag = true;
  // }
  // verilog += flag ? " " : tab;
  // if (d_seqType & EN) {
  //   toFormat = "if ({}) ";
  //   verilog += fmt::format(toFormat, getEn()->getRawName());
  // }
  // toFormat = "{} <= {};\n\tend\n";
  // verilog += fmt::format(toFormat, d_name, getData()->getRawName());

  // return verilog;
}

DotReturn GraphVertexSequential::toDOT() {
  if (!d_inConnections.size()) {
#ifdef LOGFLAG
    LOG(ERROR) << "TODO: delete empty vertices: " << d_name << std::endl;
#else
    std::cerr << "TODO: delete empty vertices: " << d_name << std::endl;
#endif
    return {};
  }

  DotReturn dot;

  dot.push_back({DotTypes::DotGate,
                 {{"name", getName()},
                  {"label", getName()},
                  {"level", std::to_string(d_level)}}});

  for (VertexPtr ptr: d_inConnections) {
    dot.push_back(
        {DotTypes::DotEdge, {{"from", ptr->getName()}, {"to", getName()}}});
  }
  return dot;
}
/// @brief GraphVertexSequential Constructor for default types
/// @param i_type type of sequential vertex (can be only (n)ff or latch = EN)
/// @param i_clk is clock signal for a ff and enable signal for a latch
/// @param i_data
/// @param i_baseGraph
/// @param i_name
GraphVertexBusSequential::GraphVertexBusSequential(
    SequentialTypes i_type, VertexPtr i_clk, VertexPtr i_data,
    GraphPtr i_baseGraph, std::string_view i_name, size_t i_width) :
    GraphVertexSequential(i_type, i_clk, i_data, i_baseGraph, i_name),
    GraphVertexBus(i_width) {
}

/// @brief
/// @param i_type
/// @param i_clk is clock signal for a ff and enable signal for a latch
/// @param i_data
/// @param i_wire RST or CLR or SET or EN
/// @param i_baseGraph
/// @param i_name
GraphVertexBusSequential::GraphVertexBusSequential(
    SequentialTypes i_type, VertexPtr i_clk, VertexPtr i_data, VertexPtr i_wire,
    GraphPtr i_baseGraph, std::string_view i_name, size_t i_width) :
    GraphVertexSequential(i_type, i_clk, i_data, i_wire, i_baseGraph, i_name),
    GraphVertexBus(i_width) {
}

/// @brief GraphVertexSequential
/// @param i_type
/// @param i_clk EN for latch and CLK for ff
/// @param i_data
/// @param i_wire1 RST or CLR or SET
/// @param i_wire2 SET or EN
/// @param i_baseGraph
GraphVertexBusSequential::GraphVertexBusSequential(
    SequentialTypes i_type, VertexPtr i_clk, VertexPtr i_data,
    VertexPtr i_wire1, VertexPtr i_wire2, GraphPtr i_baseGraph,
    std::string_view i_name, size_t i_width) :
    GraphVertexSequential(i_type, i_clk, i_data, i_wire1, i_wire2, i_baseGraph,
                          i_name, true),
    GraphVertexBus(i_width) {
}

/// @brief GraphVertexSequential
/// @param i_type type of Sequential - (a/n/an)ff(r/c)se,
/// @param i_clk clock for flip=flop
/// @param i_data data value
/// @param i_rst clear (or reset signal)
/// @param i_set set signal
/// @param i_en enable
/// @param i_baseGraph
GraphVertexBusSequential::GraphVertexBusSequential(
    SequentialTypes i_type, VertexPtr i_clk, VertexPtr i_data, VertexPtr i_rst,
    VertexPtr i_set, VertexPtr i_en, GraphPtr i_baseGraph,
    std::string_view i_name, size_t i_width) :
    GraphVertexSequential(i_type, i_clk, i_data, i_rst, i_set, i_en,
                          i_baseGraph, i_name),
    GraphVertexBus(i_width) {
}

std::string GraphVertexBusSequential::toVerilog() const {
  return "";
}
std::string GraphVertexBusSequential::toOneBitVerilog() const {
  return "";
}
} // namespace CG_Graph
