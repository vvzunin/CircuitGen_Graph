#include "CircuitGenGraph/GraphUtils.hpp"
#include "CircuitGenGraph/GraphVertexBus.hpp"
#include <functional>
#include <iostream>

#include <CircuitGenGraph/GraphVertex.hpp>
#include <sstream>
#include <string>
#include <string_view>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CG_Graph {

GraphVertexGates::GraphVertexGates(Gates i_gate, GraphPtr i_baseGraph, bool i_isBus) :
    GraphVertexBase(VertexTypes::gate, i_baseGraph, i_isBus) {
  d_gate = i_gate;
}
GraphVertexGates::GraphVertexGates(Gates i_gate, std::string_view i_name,
                                   GraphPtr i_baseGraph, bool i_isBus) :
    GraphVertexBase(VertexTypes::gate, i_name, i_baseGraph, i_isBus) {
  d_gate = i_gate;
}

Gates GraphVertexGates::getGate() const {
  return d_gate;
}
void GraphVertexGates::setGateIfDefault(Gates type) {
  assert(d_gate == GateDefault);
  getBaseGraph().lock()->updateEdgesGatesCount(this, type);
  d_gate = type;
}
uint32_t GraphVertexGates::addVertexToInConnections(VertexPtr i_vert) {
  if ((getGate() == Gates::GateBuf || getGate() == Gates::GateNot) &&
      !d_inConnections.empty()) {
    throw std::length_error("Buf and Not gate types can not obtain more than "
                            "one input connection.");
  }
  return GraphVertexBase::addVertexToInConnections(i_vert);
}

char GraphVertexGates::updateValue() {
  std::map<char, char> table;
  d_value = ValueStates::NoSignal;
  if (d_inConnections.size() > 0) {
    if (d_inConnections.front()->getValue() == ValueStates::UndefindedState) {
      d_inConnections.front()->updateValue();
    }
    d_value = d_inConnections.front()->getValue();
    if (d_gate == Gates::GateNot || d_gate == Gates::GateBuf) {
      if (d_gate == Gates::GateNot)
        table = tableNot;
      else
        table = tableBuf;
      d_value = table.at(d_value);
    }
    for (size_t i = 1; i < d_inConnections.size(); i++) {
      if (d_inConnections.at(i)->getValue() == ValueStates::UndefindedState) {
        d_inConnections.at(i)->updateValue();
      }
      switch (d_gate) {
        case (Gates::GateAnd):
          table = tableAnd.at(d_value);
          break;
        case (Gates::GateNand):
          table = tableNand.at(d_value);
          break;
        case (Gates::GateOr):
          table = tableOr.at(d_value);
          break;
        case (Gates::GateNor):
          table = tableNor.at(d_value);
          break;
        case (Gates::GateXor):
          table = tableXor.at(d_value);
          break;
        case (Gates::GateXnor):
          table = tableXnor.at(d_value);
          break;
        default:
#ifdef LOGFLAG
          LOG(ERROR) << "Error" << std::endl;
#else
          std::cerr << "Error" << std::endl;
#endif
      }
      d_value = table.at(d_inConnections.at(i)->getValue());
    }
  }
  return d_value;
}

void GraphVertexGates::removeValue() {
  d_value = ValueStates::UndefindedState;
  if (d_inConnections.size() > 0) {
    for (VertexPtr ptr: d_inConnections) {
      if (ptr->getValue() != ValueStates::UndefindedState) {
        ptr->removeValue();
      }
    }
  }
}

size_t GraphVertexGates::calculateHash() {
  if (d_hasHash) {
    return d_hashed;
  }
  std::string hashedStr =
      std::to_string(d_outConnections.size()) + std::to_string(d_gate);

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

std::string GraphVertexGates::getVerilogString() const {
  std::string s = "";

  if (d_inConnections.size() > 0) {
    VertexPtr ptr = d_inConnections.at(0);
    if (!ptr) {
      throw std::invalid_argument(
          "Cannot use nullptr for printing it to verilog");
    }

    if (this->d_baseGraph.lock() == ptr->getBaseGraph().lock())
      s = ptr->getName();
    else
      s = ptr->getBaseGraph().lock()->getName() + "_" + ptr->getName();

    if (d_gate == Gates::GateNot)
      s = "~" + s;
    if ((d_gate == Gates::GateNand) || (d_gate == Gates::GateNor) ||
        (d_gate == Gates::GateXnor))
      s = "~(" + s;

    for (size_t i = 1; i < d_inConnections.size(); i++) {
      ptr = d_inConnections.at(i);
      if (!ptr) {
        throw std::invalid_argument(
            "Cannot use nullptr for printing it to verilog");
      }

      std::string name;
      if (this->d_baseGraph.lock() == ptr->getBaseGraph().lock())
        name = ptr->getName();
      else
        name = ptr->getBaseGraph().lock()->getName() + "_" + ptr->getName();

      s += " " + VertexUtils::gateToString(d_gate) + " " + name;
      if (d_gate == GateDefault)
#ifdef LOGFLAG
        LOG(ERROR) << "Error" << std::endl;
#else
        std::cerr << "Error" << std::endl;
#endif
    }

    if ((d_gate == Gates::GateNand) || (d_gate == Gates::GateNor) ||
        (d_gate == Gates::GateXnor))
      s += ")";
  }

  return s;
}

std::string GraphVertexGates::toVerilog() const {
  std::string end;
  std::string oper = VertexUtils::gateToString(d_gate);
  auto printFunction = [&](std::string basic) {
  if (d_gate == Gates::GateNand || d_gate == Gates::GateNor ||
      d_gate == Gates::GateXnor) {
    basic += "~ ( ";

    end = " )";
  }
  for (size_t i = 0; i < d_inConnections.size() - 1; ++i) {
    basic += d_inConnections.at(i)->getName() + " " + oper + " ";
  }
  basic += d_inConnections.back()->getName() + end + ";";

  return basic;
  };
  return toVerilogCommon(printFunction);
}
/*if (!(d_inConnections.size())) {
#ifdef LOGFLAG
    LOG(ERROR) << "TODO: delete empty vertices: " << d_name << std::endl;
#else
    std::cerr << "TODO: delete empty vertices: " << d_name << std::endl;
#endif
    return "";
  }
  std::string basic = "assign " + getName() + " = ";

  std::string oper = VertexUtils::gateToString(d_gate);
  VertexPtr ptr = d_inConnections.back();
  if (d_gate == Gates::GateNot || d_gate == Gates::GateBuf) {
    if (d_inConnections.size() > 1) {
      std::cerr << "Invalid: one-input vertex \"" << d_name
                << "\" has inputs: " << d_inConnections.size() << '\n';
    }
    basic += oper + ptr->getName() + ";";

    return basic;
  }
  if (d_inConnections.size() == 1) {
    
    std::cerr << "Invalid: multiple-input vertex \"" << d_name
              << "\" has one input\n";
  }

  std::string end = "";

  if (d_gate == Gates::GateNand || d_gate == Gates::GateNor ||
      d_gate == Gates::GateXnor) {
    basic += "~ ( ";

    end = " )";
  }
  for (size_t i = 0; i < d_inConnections.size() - 1; ++i) {
    basic += d_inConnections.at(i)->getName() + " " + oper + " ";
  }
  basic += d_inConnections.back()->getName() + end + ";";

  return basic;
}*/

DotReturn GraphVertexGates::toDOT() {
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

bool GraphVertexGates::isSubgraphBuffer() const {
  if (d_gate != Gates::GateBuf || d_inConnections.empty()) {
    return false;
  }
  return d_inConnections.front()->getType() == VertexTypes::subGraph;
}
std::string GraphVertexGates::toVerilogCommon(std::function<std::string(std::string& basic)> printFunction) const {
if (!(d_inConnections.size())) {
#ifdef LOGFLAG
    LOG(ERROR) << "TODO: delete empty vertices: " << d_name << std::endl;
#else
    std::cerr << "TODO: delete empty vertices: " << d_name << std::endl;
#endif
    return "";
  }
  std::string basic = "assign " + getName() + " = ";

  std::string oper = VertexUtils::gateToString(d_gate);
  VertexPtr ptr = d_inConnections.back();
  if (d_gate == Gates::GateNot || d_gate == Gates::GateBuf) {
    if (d_inConnections.size() > 1) {
      std::cerr << "Invalid: one-input vertex \"" << d_name
                << "\" has inputs: " << d_inConnections.size() << '\n';
    }
    basic += oper + ptr->getName() + ";";

    return basic;
  }
  if (d_inConnections.size() == 1) {
    
    std::cerr << "Invalid: multiple-input vertex \"" << d_name
              << "\" has one input\n";
  }

  std::string end = "";
  return printFunction(basic);
  
}

#ifdef LOGFLAG
void GraphVertexGates::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: " << GraphUtils::parseVertexToString(VertexTypes::gate)
     << "(" + GraphUtils::parseGateToString(d_gate) + ")" << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << d_level << "\n";
  os << "Vertex Hash: " << d_hashed << "\n";
}
#endif
GraphVertexBusGate::GraphVertexBusGate(Gates i_gate, std::string_view i_name, GraphPtr i_baseGraph, size_t i_width) 
: GraphVertexGates( i_gate, i_name, i_baseGraph, true),
GraphVertexBus(i_width){
  
}

GraphVertexBusSlice::GraphVertexBusSlice( std::string_view i_name, GraphPtr i_baseGraph, size_t i_begin, size_t i_width) 
: GraphVertexBusGate(GateSlice, i_name, i_baseGraph, i_width),
d_begin(i_begin) {}

std::string GraphVertexBusSlice::getSliceSuffix() const {
return "[" + std::to_string(d_begin+d_width) +":"+std::to_string(d_begin) +"]";
}

std::string GraphVertexBusSlice::toVerilog() const {
if (d_inConnections.size()>1) {
  std::cerr << "Gate of type 'GateSlice' can not have" 
  << "more than one vertex in d_inConnections " << d_name << std::endl;
    return "";
}
  return "assign " + getName() + " = " + getInConnections()[0]->getName() + getSliceSuffix();
}
std::string GraphVertexBusSlice::toOneBitVerilog() const {
if (d_inConnections.size()>1) {
  std::cerr << "Gate of type 'GateSlice' can not have" 
  << "more than one vertex in d_inConnections " << d_name << std::endl;
    return "";
}
std::stringstream stream;
for (int i = d_begin;i<d_begin+d_width;++i) 
stream << "assign " << getName() << "_" << std::to_string(i) 
<< " = " << getInConnections()[0]->getName()<< "_" << std::to_string(i);

return stream.str();
}
std::string  GraphVertexBusGate::toVerilog() const {
  return GraphVertexGates::toVerilog();
}

std::string GraphVertexBusGate::toOneBitVerilog() const {
  std::string end;
  std::string oper = VertexUtils::gateToString(d_gate);
  auto printFunction = [&](std::string& basic) {
    std::stringstream stream;
    size_t minWidth = getBusPointer((*std::min_element(d_inConnections.begin(),d_inConnections.end(), compareBusWidth)))->getWidth();
    for(int j = 0;j<std::min(getWidth(), minWidth);++j) {
    stream << "assign " << getName()<<"_"<< j << " = ";
    if (d_gate == Gates::GateNand || d_gate == Gates::GateNor ||
      d_gate == Gates::GateXnor) {
      stream << "~ ( ";
      end = " )";
    }
    for (size_t i = 0; i < d_inConnections.size() - 1; ++i) 
    stream << d_inConnections.at(i)->getName() <<"_" <<j << " " << oper << " ";
    
    stream << d_inConnections.back()->getName() <<"_" <<j << end << ";\n\t";
  }
  return stream.str();
  };
  return toVerilogCommon(printFunction);
}
// std::string GraphVertexBusConcatenation::toVerilog() const {
// if (!d_inConnections.size()) {
//   std::cerr << "TODO: delete empty vertices: " << d_name << std::endl;
//     return "";
// }
// std::stringstream ansStream;
// ansStream << "{"; 
// for (VertexPtr v : d_inConnections) ansStream << v->getName() <<", ";
// std::string ans = ansStream.str();
// ans.pop_back();
// ans.pop_back();
// ans.push_back('}');
// return ans; 
//}

} // namespace CG_Graph
