/**
 * @file GraphVertexConstant.cpp
 * @brief Реализация вершины-константы графа.
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 * @author Fuuulkrum7 <fuuulkrum7@gmail.com>
 * @author Theossr <feolab05@gmail.com>
 * @author NonDif <shapkin.andrey123@gmail.com>
 */
#include <cassert>
#include <memory>

#include <CircuitGenGraph/GraphVertex.hpp>

#ifdef lOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CG_Graph {

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
GraphVertexConstant::GraphVertexConstant(char i_const, GraphPtr i_baseGraph) :
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    GraphVertexInput(i_baseGraph, VertexTypes::constant) {
  d_value = i_const;
}

GraphVertexConstant::GraphVertexConstant(char i_const, std::string_view i_name,
                                         GraphPtr i_baseGraph) :
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    GraphVertexInput(i_name, i_baseGraph, VertexTypes::constant) {
  d_value = i_const;
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
size_t GraphVertexConstant::calculateHash() {
  if (d_hasHash) {
    return d_hashed;
  }
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  d_hashed = std::hash<std::string>{}(std::to_string(d_value) +
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
                                      std::to_string(d_outConnections.size()));
  d_hasHash = HC_CALC;

  return d_hashed;
}

/** @author Vladimir Zunin <vzunin@hse.ru> */
std::string GraphVertexConstant::getVerilogInstance() {
  return "wire " + getName() + ";";
}

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
std::string GraphVertexConstant::toVerilog() const {
  return "assign " + getName() + " = 1'b" + d_value + ";";
}

/** @author Vladimir Zunin <vzunin@hse.ru> */
DotReturn GraphVertexConstant::toDOT() {
  DotReturn dot;
/** @author Vladimir Zunin <vzunin@hse.ru> */
  std::string str(1, d_value);

  dot.push_back({DotTypes::DotConstant,
                 {{"name", getName()},
                  {"label", getName()},
                  {"value", "1'b" + str},
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
                  {"level", std::to_string(d_level)}}});
  return dot;
}

#ifdef LOGFLAG
/** @author Vladimir Zunin <vzunin@hse.ru> */
void GraphVertexConstant::log(el::base::type::ostream_t &os) const {
  GraphPtr gr = d_baseGraph.lock();
  os << "Vertex Name(BaseGraph): " << d_name << "(" << (gr ? gr->getName() : "")
     << ")\n";
  os << "Vertex Type: "
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
     << GraphUtils::parseVertexToString(VertexTypes::constant) << "\n";
  os << "Vertex Value: " << d_value << "\n";
  os << "Vertex Level: " << 0 << "\n";
  os << "Vertex Hash: "
     << "NuN"
     << "\n";
}
#endif

} // namespace CG_Graph
