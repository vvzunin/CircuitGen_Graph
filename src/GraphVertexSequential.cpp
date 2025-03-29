#include <CircuitGenGraph/GraphVertex.hpp>

#include <iostream>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif
#include "../lib/fmt/core.h"

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

inline void GraphVertexSequential::setSignalByType(VertexPtr i_wire,
                                                   SequentialTypes i_type,
                                                   unsigned &factType) {
  if ((i_type & RST) && !d_rst) {
    factType |= RST;
    d_rst = i_wire;
  } else if ((i_type & CLR) && !d_rst) {
    factType |= CLR;
    d_rst = i_wire;
  } else if ((i_type & SET) && !d_set) {
    factType |= SET;
    d_set = i_wire;
  } else {
    factType |= EN;
    d_en = i_wire;
  }
}

// clang-format off

/// @brief Constructor for default types
/// @param i_type type of sequential vertex (can be only (n)ff or latch = EN)
/// @param i_clk is clock signal for a ff and enable signal for a latch
/// @param i_data 
/// @param i_baseGraph 
GraphVertexSequential::GraphVertexSequential(
    SequentialTypes i_type,
    VertexPtr i_clk,
    VertexPtr i_data,
    GraphPtr i_baseGraph,
    std::string_view i_name)
    : GraphVertexBase(VertexTypes::seuqential, i_name, i_baseGraph)
    , d_data(i_data) {
  // NOT ALLOWED TO USE BOTH RST AND CLR
  assert(!((i_type & RST) && (i_type & CLR)));

  d_clk = i_clk;
  if (i_type & ff) {
    d_seqType = static_cast<SequentialTypes>(i_type & nff);
  } else {
    d_en = d_clk;
    // without ff flag it is just a latch
    d_seqType = latch;
  }
}

GraphVertexSequential::GraphVertexSequential(
    SequentialTypes i_type,
    VertexPtr i_clk,
    VertexPtr i_data,
    VertexPtr i_wire,
    GraphPtr i_baseGraph,
    std::string_view i_name)
    : GraphVertexSequential(i_type, i_clk, i_data, i_baseGraph, i_name) {
  unsigned factType = 0u;

  setSignalByType(i_wire, i_type, factType);

  d_seqType = static_cast<SequentialTypes>(d_seqType | factType);
}

/// @brief 
/// @param i_type 
/// @param i_clk EN for latch and CLK for ff
/// @param i_data 
/// @param i_wire1 RST or CLR or SET
/// @param i_wire2 SET or EN
/// @param i_baseGraph 
GraphVertexSequential::GraphVertexSequential(
    SequentialTypes i_type,
    VertexPtr i_clk,
    VertexPtr i_data,
    VertexPtr i_wire1,
    VertexPtr i_wire2,
    GraphPtr i_baseGraph,
    std::string_view i_name)
    : GraphVertexSequential(i_type, i_clk, i_data, i_baseGraph, i_name) {
  unsigned factType = 0u;

  setSignalByType(i_wire1, i_type, factType);
  setSignalByType(i_wire2, i_type, factType);

  d_seqType = static_cast<SequentialTypes>(d_seqType | factType);
}

GraphVertexSequential::GraphVertexSequential(
    SequentialTypes i_type,
    VertexPtr i_clk,
    VertexPtr i_data,
    VertexPtr i_rst,
    VertexPtr i_set,
    VertexPtr i_en,
    GraphPtr i_baseGraph,
    std::string_view i_name)
    : GraphVertexSequential(i_type, i_clk, i_data, i_baseGraph, i_name) {
  // cannot have 3 input wires and be a latch - latch has only 3 signals at all
  assert(isFF());
  unsigned factType = SET | EN;
  d_rst = i_rst;
  d_set = i_set;
  d_en = i_en;
  
  factType |= i_type & (RST | CLR);

  d_seqType = static_cast<SequentialTypes>(d_seqType | factType);
}

// clang-format on

SequentialTypes GraphVertexSequential::getSeqType() const {
  return d_seqType;
}

VertexPtr GraphVertexSequential::getClk() const {
  return d_clk;
}

VertexPtr GraphVertexSequential::getData() const {
  return d_data;
}

VertexPtr GraphVertexSequential::getEn() const {
  return d_en;
}

VertexPtr GraphVertexSequential::getRst() const {
  return d_rst;
}

VertexPtr GraphVertexSequential::getSet() const {
  return d_set;
}

size_t GraphVertexSequential::calculateHash(bool i_recalculate) {
  if (d_hasHash && (!i_recalculate || d_hasHash == HC_IN_PROGRESS)) {
    return d_hashed;
  }
  // same as in gate, but adds type of a sequential vertex
  std::string hashedStr = std::to_string(d_outConnections.size()) +
                          std::to_string(getType()) + std::to_string(d_seqType);

  d_hasHash = HC_IN_PROGRESS;
  std::vector<size_t> hashed_data;
  hashed_data.reserve(d_inConnections.size());

  for (auto &child: d_inConnections) {
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

inline void
GraphVertexSequential::formatAlwaysBegin(std::string &verilog) const {
  if (isFF() && !isAsync()) {
    verilog =
        fmt::format("always @({} {}) begin\n",
                    isNegedge() ? "negedge" : "posedge", d_clk->getRawName());
  } else if (isFF()) {
    verilog = fmt::format("always @({} {} or negedge {}) begin\n",
                          isNegedge() ? "negedge" : "posedge",
                          d_clk->getRawName(), d_rst->getRawName());
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
  std::string verilog;
  formatAlwaysBegin(verilog);
  std::string_view toFormat;
  std::string_view tab = "\t\t";
  bool flag = false;
  if (unsigned val = (d_seqType & RST) | (d_seqType & CLR)) {
    simpleCheckFormat(verilog, d_rst->getRawName(), d_name, val, tab);
    verilog += "\t\telse";
    flag = true;
  }
  if (d_seqType & SET) {
    simpleCheckFormat(verilog, d_set->getRawName(), d_name, SET,
                      flag ? " " : tab);
    verilog += "\t\telse";
    flag = true;
  }
  verilog += flag ? " " : tab;
  if (d_seqType & EN) {
    toFormat = "if ({}) ";
    verilog += fmt::format(toFormat, d_en->getRawName());
  }
  toFormat = "{} <= {};\n\tend\n";
  verilog += fmt::format(toFormat, d_name, d_data->getRawName());

  return verilog;
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

} // namespace CG_Graph
