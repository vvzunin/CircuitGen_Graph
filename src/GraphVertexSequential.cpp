/**
 * @file GraphVertexSequential.cpp
 * @brief Реализация последовательностной вершины графа (триггеры, регистры).
 */
#include <CircuitGenGraph/GraphUtils.hpp>
#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/GraphVertexBus.hpp>
#include <CircuitGenGraph/Logging.hpp>
#include <CircuitGenGraph/SequentialVerilogStorage.hpp>

#include <cassert>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "fmt/core.h"

namespace CG_Graph {

bool GraphVertexSequential::isFF() const {
  return d_seqType & ff;
}

bool GraphVertexSequential::isAsync() const {
  return d_seqType & ASYNC;
}

bool GraphVertexSequential::isNegedge() const {
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
      CG_LOG_ERROR << "Invalid flag found in used type: "
                   << convertSequentialFlag(foundFlag) << '\n';
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
  return 1 + bool(i_type & ff) + bool(i_type & RST) + bool(i_type & CLR) +
         bool(i_type & EN) + bool(i_type & SET);
}
// BRAND NEW SIGNALS SEQUENCE 0 -- data, 1 -- clk, 2 -- en, 3 -- RST/CLR, 4 --
// SET
GraphVertexSequential::GraphVertexSequential(SequentialTypes i_type,
                                             VertexPtr i_clk, VertexPtr i_data,
                                             GraphPtr i_baseGraph,
                                             std::string_view i_name,
                                             bool i_isBus) :
    GraphVertexBase(i_isBus ? sequentialBus : sequential, i_name, i_baseGraph) {
  reserveInConnections(2);
  i_baseGraph->addEdges({i_data, i_clk}, this);
  DEFAULT_CHECK_TYPE;
  validateSignal(i_type, d_seqType);
}

GraphVertexSequential::GraphVertexSequential(
    SequentialTypes i_type, VertexPtr i_clk, VertexPtr i_data, VertexPtr i_wire,
    GraphPtr i_baseGraph, std::string_view i_name, bool i_isBus) :
    GraphVertexBase(i_isBus ? sequentialBus : sequential, i_name, i_baseGraph) {
  reserveInConnections(3);
  i_baseGraph->addEdges({i_data, i_clk, i_wire}, this);
  DEFAULT_CHECK_TYPE;
  short signals = countSignalsInType(i_type);
  if (signals < 2) {
    d_seqType = static_cast<SequentialTypes>(i_type | EN);
  } else if (signals > 3) {
    d_seqType = static_cast<SequentialTypes>(i_type & ~SET);
    if (signals > 4) {
      d_seqType = static_cast<SequentialTypes>(i_type & ~SET & ~RST & ~CLR);
    }
  } else {
    d_seqType = i_type;
  }
  validateSignal(i_type, d_seqType);
}

GraphVertexSequential::GraphVertexSequential(
    SequentialTypes i_type, VertexPtr i_clk, VertexPtr i_data,
    VertexPtr i_wire1, VertexPtr i_wire2, GraphPtr i_baseGraph,
    std::string_view i_name, bool i_isBus) :
    GraphVertexBase(i_isBus ? sequentialBus : sequential, i_name, i_baseGraph) {
  reserveInConnections(4);
  i_baseGraph->addEdges({i_data, i_clk, i_wire1, i_wire2}, this);
  DEFAULT_CHECK_TYPE;
  short signals = countSignalsInType(i_type);
  if (signals == 1) {
    d_seqType = static_cast<SequentialTypes>(i_type | EN | RST);
  } else if (signals == 2)
    d_seqType = static_cast<SequentialTypes>(i_type | EN);
  else if (signals > 4)
    d_seqType = static_cast<SequentialTypes>(i_type & ~SET);
  else
    d_seqType = i_type;
  validateSignal(i_type, d_seqType);
}

GraphVertexSequential::GraphVertexSequential(
    SequentialTypes i_type, VertexPtr i_clk, VertexPtr i_data, VertexPtr i_rst,
    VertexPtr i_set, VertexPtr i_en, GraphPtr i_baseGraph,
    std::string_view i_name, bool i_isBus) :
    GraphVertexBase(i_isBus ? sequentialBus : sequential, i_name, i_baseGraph) {
  reserveInConnections(5);
  i_baseGraph->addEdges({i_data, i_clk, i_en, i_rst, i_set}, this);
  DEFAULT_CHECK_TYPE;

  // cannot have 3 input wires and be a latch - latch has only 3 signals at all
  assert(isFF());
  unsigned factType = SET | EN;
  factType |= i_type & (RST | CLR | NEGEDGE | ASYNC);

  d_seqType = static_cast<SequentialTypes>(d_seqType | factType);
  validateSignal(i_type, d_seqType);
}

#undef DEFAULT_CHECK_TYPE

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
  if (((getSeqType()) & RST) | ((getSeqType()) & CLR))
    return d_inConnections[bool(getSeqType() & ff) + bool(getSeqType() & EN) +
                           1];
  return nullptr;
}

VertexPtr GraphVertexSequential::getSet() const {
  if (getSeqType() & SET)
    return d_inConnections[bool(getSeqType() & ff) + bool(getSeqType() & EN) +
                           bool((getSeqType() & RST) | (getSeqType() & CLR)) +
                           1];
  return nullptr;
}

char GraphVertexSequential::updateValue() {
  auto eval = [](VertexPtr vert) -> char {
    if (!vert)
      return ValueStates::NoSignal;
    return vert->updateValue();
  };

  const char data = eval(getData());

  // RST/CLR are active-low; force Q=0 when asserted.
  if (VertexPtr rst = getRst()) {
    const char rstVal = eval(rst);
    if (rstVal == '0')
      return (d_value = ValueStates::FalseValue);
    if (rstVal != '1')
      return (d_value = ValueStates::NoSignal);
  }

  // SET is active-high; force Q=1 when asserted.
  if (VertexPtr set = getSet()) {
    const char setVal = eval(set);
    if (setVal == '1')
      return (d_value = ValueStates::TrueValue);
    if (setVal != '0')
      return (d_value = ValueStates::NoSignal);
  }

  const auto captureData = [&]() {
    if (data == '0' || data == '1')
      d_value = data;
    else
      d_value = ValueStates::NoSignal;
  };

  if (isFF()) {
    const char clk = eval(getClk());
    bool enabled = true;
    if (VertexPtr en = getEn()) {
      const char enVal = eval(en);
      if (enVal == '0')
        enabled = false;
      else if (enVal != '1') {
        d_prevClk = clk;
        return (d_value = ValueStates::NoSignal);
      }
    }

    const bool rising = (d_prevClk == '0' && clk == '1');
    const bool falling = (d_prevClk == '1' && clk == '0');
    const bool tick = isNegedge() ? falling : rising;
    d_prevClk = clk;

    if (tick && enabled)
      captureData();
    else if (d_value == ValueStates::UndefinedState)
      d_value = ValueStates::NoSignal;
    return d_value;
  }

  // Latch: EN is level-sensitive (constructor stores it as the "clk" wire).
  const char enVal = eval(getEn());
  if (enVal == '1')
    captureData();
  else if (enVal != '0')
    d_value = ValueStates::NoSignal;
  else if (d_value == ValueStates::UndefinedState)
    d_value = ValueStates::NoSignal;
  return d_value;
}

void GraphVertexSequential::removeValue() {
  d_prevClk = ValueStates::NoSignal;
  GraphVertexBase::removeValue();
}

size_t GraphVertexSequential::calculateHash() {
  if (d_hasHash) {
    return d_hashed;
  }
  d_hasHash = HC_IN_PROGRESS;
  std::vector<size_t> hashed_data;
  hashed_data.reserve(d_inConnections.size());

  for (auto &child: d_inConnections) {
    hashed_data.push_back(child->calculateHash());
  }
  std::sort(hashed_data.begin(), hashed_data.end());

  size_t h = 0;
  hashCombine(h, static_cast<size_t>(getType()));
  hashCombine(h, static_cast<size_t>(d_seqType));
  hashCombine(h, d_outConnections.size());
  for (const auto &sub: hashed_data) {
    hashCombine(h, sub);
  }
  d_hashed = h;
  d_hasHash = HC_CALC;

  return d_hashed;
}
std::string GraphVertexSequential::getSequentialString(
    SequentialTypes i_type, std::string_view i_name,
    std::vector<std::string_view> i_inputs) {
  switch (countSignalsInType(i_type)) {
    case 2:
      return fmt::format(
          SequentialVerilogInstance::SequentialTypeToVerilog[i_type], i_name,
          i_inputs[0], i_inputs[1]);
    case 3:
      return fmt::format(
          SequentialVerilogInstance::SequentialTypeToVerilog[i_type], i_name,
          i_inputs[0], i_inputs[1], i_inputs[2]);
    case 4:
      return fmt::format(
          SequentialVerilogInstance::SequentialTypeToVerilog[i_type], i_name,
          i_inputs[0], i_inputs[1], i_inputs[2], i_inputs[3]);
    case 5:
      return fmt::format(
          SequentialVerilogInstance::SequentialTypeToVerilog[i_type], i_name,
          i_inputs[0], i_inputs[1], i_inputs[2], i_inputs[3], i_inputs[4]);
    default:
      return "ERROR";
  }
}
std::string GraphVertexSequential::toVerilog() const {
  if (d_inConnections.empty()) {
    CG_LOG_WARNING << "GraphVertexSequential: empty vertex '" << d_name
                   << "' skipped in Verilog";
    return "";
  }
  std::vector<std::string_view> names;
  for (auto *v: d_inConnections)
    names.push_back(v->getRawName());
  return getSequentialString(d_seqType, getRawName(), names);
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

std::string GraphVertexSequential::getVerilogInstance() {
  std::vector<std::string_view> inputsInstance =
      GraphUtils::parseSequentialToInputs(d_seqType);
  std::vector<std::string_view> inputNames;
  for (auto *v: d_inConnections)
    inputNames.push_back(v->getRawName());
  return getVerilogInstance(this, d_inConnections[0]->getRawName(),
                            getRawName());
}
std::string GraphVertexSequential::getVerilogInstance(
    const VertexPtr vertex, std::string_view i_inputDataName,
    std::string_view i_qOutputName, std::string_view i_dataName,
    std::string_view i_qName, std::string_view i_instanceName) {
  std::string inouts;
  std::vector<std::string_view> inputsInstance =
      GraphUtils::parseSequentialToInputs(
          static_cast<const GraphVertexSequential *>(vertex)->getSeqType());
  switch (countSignalsInType(
      static_cast<const GraphVertexSequential *>(vertex)->getSeqType())) {
    case 2:
      inouts = fmt::format(".{}({}), .{}({}), .{}({})", i_dataName,
                           i_inputDataName, inputsInstance[1],
                           vertex->getInConnections()[1]->getRawName(), i_qName,
                           i_qOutputName);
      break;
    case 3:
      inouts = fmt::format(
          ".{}({}), .{}({}), .{}({}), .{}({})", i_dataName, i_inputDataName,
          inputsInstance[1], vertex->getInConnections()[1]->getRawName(),
          inputsInstance[2], vertex->getInConnections()[2]->getRawName(),
          i_qName, i_qOutputName);
      break;
    case 4:
      inouts = fmt::format(
          ".{}({}), .{}({}), .{}({}), .{}({}), .{}({})", i_dataName,
          i_inputDataName, inputsInstance[1],
          vertex->getInConnections()[1]->getRawName(), inputsInstance[2],
          vertex->getInConnections()[2]->getRawName(), inputsInstance[3],
          vertex->getInConnections()[3]->getRawName(), i_qName, i_qOutputName);
      break;
    case 5:
      inouts = fmt::format(
          ".{}({}), .{}({}), .{}({}), .{}({}), .{}({}), .{}({})", i_dataName,
          i_inputDataName, inputsInstance[1],
          vertex->getInConnections()[1]->getRawName(), inputsInstance[2],
          vertex->getInConnections()[2]->getRawName(), inputsInstance[3],
          vertex->getInConnections()[3]->getRawName(), inputsInstance[4],
          vertex->getInConnections()[4]->getRawName(), i_qName, i_qOutputName);
      break;
  }
  return fmt::format(
      "{} {} ({});",
      GraphUtils::parseSequentialToString(
          static_cast<const GraphVertexSequential *>(vertex)->getSeqType()),
      i_instanceName == "" ? fmt::format("{}_ins", vertex->getRawName())
                           : i_instanceName,
      inouts);
}
DotReturn GraphVertexSequential::toDOT() {
  if (d_inConnections.empty()) {
    CG_LOG_WARNING << "GraphVertexSequential: empty vertex '" << d_name
                   << "' skipped in DOT";
    return {};
  }

  DotReturn dot;

  const std::string kind = isFF() ? "ff" : "latch";
  dot.push_back({DotTypes::DotGate,
                 {{"name", getName()},
                  {"label", fmt::format("{} ({})", getName(), kind)},
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
    GraphVertexSequential(i_type, i_clk, i_data, i_baseGraph, i_name, true),
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
    GraphVertexSequential(i_type, i_clk, i_data, i_wire, i_baseGraph, i_name,
                          true),
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
                          i_baseGraph, i_name, true),
    GraphVertexBus(i_width) {
}

std::string GraphVertexBusSequential::toVerilog() const {
  return "";
}
std::string GraphVertexBusSequential::toOneBitVerilog() const {
  std::string dataName, dataInputName, outputName, qName;
  std::vector<std::string> instances;
  for (size_t i = 0; i < getWidth(); ++i) {
    dataName = GraphUtils::sequentialToInputList[getSeqType()].second[0];
    dataInputName = fmt::format("{}_{}", d_inConnections[0]->getRawName(), i);
    outputName = fmt::format("{}_{}", getRawName(), i);
    qName = "q";
    instances.push_back(getVerilogInstance(
        static_cast<const GraphVertexSequential *>(this), dataInputName,
        outputName, dataName, qName,
        fmt::format("{}_{}_ins", getRawName(), i)));
  }
  return fmt::format("{}\n\n", fmt::join(instances, "\n\t"));
}
} // namespace CG_Graph
