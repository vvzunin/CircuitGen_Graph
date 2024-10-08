#include <iostream>

#include <CircuitGenGraph/GraphVertex.hpp>
#include "easyloggingpp/easylogging++.h"

GraphVertexGates::GraphVertexGates(Gates i_gate, GraphPtr i_baseGraph) :
  GraphVertexBase(VertexTypes::gate, i_baseGraph) {
  d_gate = i_gate;
}

GraphVertexGates::GraphVertexGates(
    Gates             i_gate,
    const std::string i_name,
    GraphPtr          i_baseGraph
) :
  GraphVertexBase(VertexTypes::gate, i_name, i_baseGraph) {
  d_gate = i_gate;
}

Gates GraphVertexGates::getGate() const {
  return d_gate;
}

char GraphVertexGates::updateValue() {
  std::map<char, char> table;
  if (d_inConnections.size() > 0) {
    VertexPtr ptr;
    // check if ptr is alive
    if (!(ptr = d_inConnections[0].lock())) {
      return 'x';
    }

    d_value = ptr->getValue();
    if (d_gate == Gates::GateBuf)
      d_value = tableBuf.at(ptr->getValue());
    if (d_gate == Gates::GateNot)
      d_value = tableNot.at(ptr->getValue());
    for (size_t i = 1; i < d_inConnections.size(); i++) {
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
          LOG(ERROR) << "Error" << std::endl;
      }

      if (!(ptr = d_inConnections[i].lock())) {
        return 'x';
      }

      d_value = table.at(ptr->getValue());
    }
  }
  return d_value;
}

std::string GraphVertexGates::calculateHash(bool recalculate) {
  if (hashed != "" && !recalculate)
    return hashed;

  hashed = std::to_string(d_outConnections.size()) + std::to_string(d_gate);

  // futuire sorted struct
  std::vector<std::string> hashed_data;

  for (auto& child : d_outConnections) {
    hashed_data.push_back(child->calculateHash(recalculate));
  }
  std::sort(hashed_data.begin(), hashed_data.end());

  for (const auto& sub : hashed_data) {
    hashed += sub;
  }

  hashed = std::to_string(std::hash<std::string> {}(hashed));

  return hashed;
}

std::string GraphVertexGates::getVerilogString() const {
  std::string s = "";

  if (d_inConnections.size() > 0) {
    VertexPtr ptr;

    // check if ptr is alive
    if (!(ptr = d_inConnections[0].lock())) {
      throw std::invalid_argument("Dead pointer!");
      return "";
    }

    if (this->d_baseGraph.lock() == ptr->getBaseGraph().lock())
      s = ptr->getName();
    else
      s = ptr->getBaseGraph().lock()->getName() + "_" + ptr->getName();

    if (d_gate == Gates::GateNot)
      s = "~" + s;
    if ((d_gate == Gates::GateNand) || (d_gate == Gates::GateNor)
        || (d_gate == Gates::GateXnor))
      s = "~(" + s;

    for (size_t i = 1; i < d_inConnections.size(); i++) {
      // check if ptr is alive
      if (!(ptr = d_inConnections[i].lock())) {
        throw std::invalid_argument("Dead pointer!");
        return "";
      }

      std::string name;
      if (this->d_baseGraph.lock() == ptr->getBaseGraph().lock())
        name = ptr->getName();
      else
        name = ptr->getBaseGraph().lock()->getName() + "_" + ptr->getName();

      s += " " + VertexUtils::gateToString(d_gate) + " " + name;
      if (d_gate == GateDefault)
        LOG(ERROR) << "Error" << std::endl;
    }

    if ((d_gate == Gates::GateNand) || (d_gate == Gates::GateNor)
        || (d_gate == Gates::GateXnor))
      s += ")";
  }

  return s;
}

std::string GraphVertexGates::toVerilog() {
  if (!d_inConnections.size()) {
    LOG(ERROR) << "TODO: delete empty vertices: " << d_name << std::endl;
    return "";
  }
  std::string basic = "assign " + d_name + " = ";

  std::string oper  = VertexUtils::gateToString(d_gate);
  
  if (VertexPtr ptr = d_inConnections.back().lock()) {
    if (d_gate == Gates::GateNot || d_gate == Gates::GateBuf) {
      basic += oper + ptr->getName() + ";";

      return basic;
    }
  } else {
    throw std::invalid_argument("Dead pointer!");
  }

  std::string end = "";

  if (d_gate == Gates::GateNand || d_gate == Gates::GateNor
      || d_gate == Gates::GateXnor) {
    basic += "~ ( ";

    end   = " )";
  }
  VertexPtr ptr;
  for (size_t i = 0; i < d_inConnections.size() - 1; ++i) {
    if (ptr = d_inConnections[i].lock()) {
      basic += ptr->getName() + " " + oper + " ";
    } else {
    }
  }

  if (ptr = d_inConnections.back().lock()) {
    basic += ptr->getName() + end + ";";
  } else {
    throw std::invalid_argument("Dead pointer!");
  }

  return basic;
}

DotReturn GraphVertexGates::toDOT() {
  if (!d_inConnections.size()) {
    LOG(ERROR) << "TODO: delete empty vertices: " << d_name << std::endl;
    return {};
  }

  DotReturn dot;

  dot.push_back({DotTypes::DotGate,{
    {"name", d_name},
    {"label", d_name},
    {"level", std::to_string(d_level)}
  }});

  for (VertexPtrWeak ptrWeak : d_inConnections) {
    if (VertexPtr ptr = ptrWeak.lock()) 
      dot.push_back({DotTypes::DotEdge, {
        {"from", ptr->getName()},
        {"to", d_name}
      }});
    else {
      LOG(ERROR) << "Dead pointer!" << d_name << std::endl;
      throw std::invalid_argument("Dead pointer!");
    }
  }
  return dot;
}

bool GraphVertexGates::isSubgraphBuffer() const {
  if (d_gate != Gates::GateBuf || d_inConnections.empty()) {
    return false;
  }
  return d_inConnections[0].lock()->getType() == VertexTypes::subGraph;
}

void GraphVertexGates::log(el::base::type::ostream_t& os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "") << ")\n";
  os << "Vertex Type: " << d_settings->parseVertexToString(VertexTypes::gate) << "(" + d_settings->parseGateToString(d_gate) + ")"<< "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << d_level << "\n";
  os << "Vertex Hash: " << hashed << "\n";
}