/**
 * @file GraphVertexSequential.cpp
 * @brief Реализация последовательностной вершины графа (триггеры, регистры).
 */
#include <CircuitGenGraph/GraphUtils.hpp>
#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/GraphVertexBus.hpp>
#include <CircuitGenGraph/Logging.hpp>
#include <CircuitGenGraph/SequentialVerilogStorage.hpp>

#include <algorithm>
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
      {EN, "EN"},   {RST, "RST"},     {CLR, "CLR"},
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

void GraphVertexSequential::stageValue() {
  // Sample other sequentials via getValue (NBA / same-cycle safety). Combo
  // cones still recurse with updateValue.
  auto eval = [](VertexPtr vert) -> char {
    if (!vert)
      return ValueStates::NoSignal;
    if (vert->getType() == VertexTypes::sequential)
      return vert->getValue();
    return vert->updateValue();
  };

  const char data = eval(getData());

  // RST: active-low. CLR: active-high (matches SequentialVerilogStorage).
  // Returns 1 = force Q=0, -1 = X, 0 = inactive.
  auto clearEffect = [this](char v) -> int {
    if (d_seqType & CLR) {
      if (v == '1')
        return 1;
      if (v != '0')
        return -1;
      return 0;
    }
    if (d_seqType & RST) {
      if (v == '0')
        return 1;
      if (v != '1')
        return -1;
      return 0;
    }
    return 0;
  };
  auto setEffect = [](char v) -> int {
    if (v == '1')
      return 1;
    if (v != '0')
      return -1;
    return 0;
  };

  char nextVal = d_value;
  char nextPrevClk = d_prevClk;

  const auto captureData = [&]() {
    nextVal = (data == '0' || data == '1') ? data : ValueStates::NoSignal;
  };

  // Apply rst/clr then set into nextVal. Returns true if Q was forced.
  const auto applyResetSet = [&]() -> bool {
    if (VertexPtr rst = getRst()) {
      const int c = clearEffect(eval(rst));
      if (c == 1) {
        nextVal = ValueStates::FalseValue;
        return true;
      }
      if (c < 0) {
        nextVal = ValueStates::NoSignal;
        return true;
      }
    }
    if (VertexPtr set = getSet()) {
      const int s = setEffect(eval(set));
      if (s == 1) {
        nextVal = ValueStates::TrueValue;
        return true;
      }
      if (s < 0) {
        nextVal = ValueStates::NoSignal;
        return true;
      }
    }
    return false;
  };

  if (isFF()) {
    const char clk = eval(getClk());
    // Verilog `if (en)`: only '1' enables capture; X/Z/0 hold Q.
    bool enabled = true;
    if (VertexPtr en = getEn())
      enabled = (eval(en) == '1');

    // Async RST/CLR apply immediately; SET follows the clock edge.
    if (isAsync()) {
      if (VertexPtr rst = getRst()) {
        const int c = clearEffect(eval(rst));
        if (c == 1) {
          nextVal = ValueStates::FalseValue;
          nextPrevClk = clk;
          d_stagedValue = nextVal;
          d_stagedPrevClk = nextPrevClk;
          d_hasStaged = true;
          return;
        }
        if (c < 0) {
          nextVal = ValueStates::NoSignal;
          nextPrevClk = clk;
          d_stagedValue = nextVal;
          d_stagedPrevClk = nextPrevClk;
          d_hasStaged = true;
          return;
        }
      }
    }

    const bool rising = (d_prevClk == '0' && clk == '1');
    const bool falling = (d_prevClk == '1' && clk == '0');
    const bool tick = isNegedge() ? falling : rising;
    nextPrevClk = clk;

    if (tick) {
      if (!isAsync() && applyResetSet()) {
        d_stagedValue = nextVal;
        d_stagedPrevClk = nextPrevClk;
        d_hasStaged = true;
        return;
      }
      if (isAsync()) {
        if (VertexPtr set = getSet()) {
          const int s = setEffect(eval(set));
          if (s == 1) {
            nextVal = ValueStates::TrueValue;
            d_stagedValue = nextVal;
            d_stagedPrevClk = nextPrevClk;
            d_hasStaged = true;
            return;
          }
          if (s < 0) {
            nextVal = ValueStates::NoSignal;
            d_stagedValue = nextVal;
            d_stagedPrevClk = nextPrevClk;
            d_hasStaged = true;
            return;
          }
        }
      }
      if (enabled)
        captureData();
    } else if (nextVal == ValueStates::UndefinedState) {
      nextVal = ValueStates::NoSignal;
    }

    d_stagedValue = nextVal;
    d_stagedPrevClk = nextPrevClk;
    d_hasStaged = true;
    return;
  }

  // Latch: all controls are level-sensitive (always @*).
  if (applyResetSet()) {
    d_stagedValue = nextVal;
    d_stagedPrevClk = nextPrevClk;
    d_hasStaged = true;
    return;
  }
  const char enVal = eval(getEn());
  if (enVal == '1')
    captureData();
  else if (nextVal == ValueStates::UndefinedState)
    nextVal = ValueStates::NoSignal;
  // EN=X/Z: hold (Verilog `if (en)` does not take the true branch).

  d_stagedValue = nextVal;
  d_stagedPrevClk = nextPrevClk;
  d_hasStaged = true;
}

void GraphVertexSequential::commitStagedValue() {
  if (!d_hasStaged)
    return;
  d_value = d_stagedValue;
  d_prevClk = d_stagedPrevClk;
  d_hasStaged = false;
}

char GraphVertexSequential::updateValue() {
  stageValue();
  commitStagedValue();
  return d_value;
}

void GraphVertexSequential::removeValue() {
  d_prevClk = ValueStates::NoSignal;
  d_hasStaged = false;
  d_stagedValue = ValueStates::UndefinedState;
  d_stagedPrevClk = ValueStates::NoSignal;
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
/// @param i_wire1 EN for *re/*ce, else RST/CLR/SET
/// @param i_wire2 RST/CLR for *re/*ce, else SET or EN
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
  // Same always-block templates as scalar; bus nets use vector widths.
  // `1'b0`/`1'b1` on a multi-bit LHS zero-/sign-extends in Verilog.
  return GraphVertexSequential::toVerilog();
}

void GraphVertexBusSequential::commitStagedValue() {
  GraphVertexSequential::commitStagedValue();
  // Keep bus string in sync with the scalar Q (broadcast). Distinct per-bit
  // stimulus needs a multi-bit graphSimulation API; until then all bits match
  // Q.
  const char v = getValue();
  if (v == ValueStates::UndefinedState)
    updateValueBus(std::string(getWidth(), ValueStates::NoSignal));
  else
    updateValueBus(std::string(getWidth(), v));
}

char GraphVertexBusSequential::updateValue() {
  stageValue();
  commitStagedValue();
  return getValue();
}

void GraphVertexBusSequential::removeValue() {
  GraphVertexSequential::removeValue();
  updateValueBus(std::string(getWidth(), ValueStates::NoSignal));
}

std::string GraphVertexBusSequential::toOneBitVerilog() const {
  const SequentialTypes type = getSeqType();
  const auto ports = GraphUtils::parseSequentialToInputs(type);
  const std::string typeName = GraphUtils::parseSequentialToString(type);
  if (ports.empty() || d_inConnections.size() < ports.size())
    return "";

  size_t bits = getWidth();
  if (d_inConnections[0]->isBus()) {
    const GraphVertexBus *dataBus =
        GraphVertexBus::getBusPointer(d_inConnections[0]);
    if (dataBus)
      bits = std::min(bits, dataBus->getWidth());
  }

  auto netFor = [&](size_t portIdx, size_t bit) -> std::string {
    VertexPtr v = d_inConnections.at(portIdx);
    if (v->isBus()) {
      const GraphVertexBus *bus = GraphVertexBus::getBusPointer(v);
      if (bus && bit < bus->getWidth())
        return fmt::format("{}_{}", v->getRawName(), bit);
      return "1'bx";
    }
    return std::string(v->getRawName());
  };

  std::vector<std::string> instances;
  instances.reserve(bits);
  for (size_t i = 0; i < bits; ++i) {
    std::vector<std::string> binds;
    binds.reserve(ports.size() + 1);
    for (size_t p = 0; p < ports.size(); ++p)
      binds.push_back(fmt::format(".{}({})", ports[p], netFor(p, i)));
    binds.push_back(fmt::format(".q({}_{})", getRawName(), i));
    instances.push_back(fmt::format("{} {}_{}_ins ({})", typeName, getRawName(),
                                    i, fmt::join(binds, ", ")));
  }
  return fmt::format("{}\n\n", fmt::join(instances, "\n\t"));
}
} // namespace CG_Graph
