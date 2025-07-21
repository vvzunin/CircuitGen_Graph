#include "CircuitGenGraph/GraphUtils.hpp"
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <lorina/lorina.hpp>
#include <utility>
#define SUFFIX_INVERSION "_not"
#define SUFFIX_OUTPUT_GATE "_gate"
namespace CG_Graph {
GraphReader::GraphReader(Context &i_context) : d_context(i_context){};
void GraphReader::on_module_header(
    const std::string &module_name,
    const std::vector<std::string> &inouts) const {
  d_context.d_currentGraph = std::make_shared<OrientedGraph>(module_name);
}
void GraphReader::on_inputs(const std::vector<std::string> &inputs,
                            std::string const &size) const {
  d_context.d_currentGraph->reserve(input,inputs.size());
  for (auto i: inputs)
    d_context.d_currentGraphNamesList[i] =
        d_context.d_currentGraph->addInput(i);
}
void GraphReader::on_outputs(const std::vector<std::string> &outputs,
                             std::string const &size) const {
  d_context.d_currentGraph->reserve(output,outputs.size());
  d_context.d_currentGraph->reserve(gate,outputs.size());
  for (auto i: outputs) {
    VertexPtr outputVertex = d_context.d_currentGraphNamesList[i] = d_context.d_currentGraph->addOutput(i);
    //VertexPtr outputGateVertex = d_context.d_currentGraphNamesList[i+SUFFIX_OUTPUT_GATE] = d_context.d_currentGraph->addGate(GateDefault,i+SUFFIX_OUTPUT_GATE);
    //d_context.d_currentGraph->addEdge(outputGateVertex, outputVertex);
    }
}

void GraphReader::on_parameter(const std::string &name,
                               const std::string &value) const {
  VertexPtr vertex;
  if (value == "1'b1" || value == "1" )
  vertex = d_context.d_currentGraph->addConst('1',name);
else vertex = d_context.d_currentGraph->addConst('0',name);
d_context.d_currentGraphNamesList[name] = vertex;
}
void GraphReader::on_assign( const std::string& lhs, const std::pair<std::string, bool>& rhs ) const {
  VertexPtr leftSideVertex = find_operand(lhs);
  if (!rhs.second) 
    d_context.d_currentGraphNamesList[lhs] = d_context.d_currentGraphNamesList[rhs.first];
  else {
    if(d_context.d_currentGraphNamesList[lhs]->getType() == output) {
      leftSideVertex = d_context.d_currentGraphNamesList[lhs+SUFFIX_INVERSION];
      d_context.d_currentGraphNamesList[lhs+SUFFIX_OUTPUT_GATE] =d_context.d_currentGraphNamesList[lhs+SUFFIX_INVERSION];
      if(leftSideVertex->getGate()==GateDefault){
      static_cast<GraphVertexGates*>(leftSideVertex)->setGateType(GateNot);
      d_context.d_currentGraph->addEdge(leftSideVertex, d_context.d_currentGraphNamesList[lhs]);
      }
      d_context.d_currentGraph->addEdge(find_operand(rhs.first), leftSideVertex);
      }
      else {

      }
    }
  };  

VertexPtr GraphReader::find_operand(const std::string &i_name, bool i_isInverted) const {
  auto temp = d_context.d_currentGraphNamesList.find(i_name);
  VertexPtr ans;
  if (temp == d_context.d_currentGraphNamesList.end()) {
    ans = d_context.d_currentGraph->addGate(GateDefault, i_name);
    d_context.d_currentGraphNamesList[i_name] = ans;
  } else
    if (d_context.d_currentGraphNamesList[i_name]->getType() != output) 
    ans = d_context.d_currentGraphNamesList[i_name];
    else 
    ans = d_context.d_currentGraphNamesList[i_name+SUFFIX_OUTPUT_GATE];
  if (i_isInverted){
    VertexPtr storageForAnsNoInversion = find_operand(i_name+SUFFIX_INVERSION);
    std::swap(ans, storageForAnsNoInversion);
    if(ans->getGate()==GateDefault) {
    static_cast<GraphVertexGates*>(ans)->setGateType(GateNot);
    d_context.d_currentGraph->addEdge(storageForAnsNoInversion,ans);
    }
  }
  return ans;
}

void GraphReader::on_elem(const std::string &lhs,
                          const std::pair<std::string, bool> &op1,
                          const std::pair<std::string, bool> &op2,
                          Gates gateType) const {
  VertexPtr vertexGate = find_operand(lhs);
  if (vertexGate->getType() == gate && vertexGate->getGate() == GateDefault)
    static_cast<GraphVertexGates *>(vertexGate)->setGateType(gateType);
  d_context.d_currentGraph->addEdge(find_operand(op1.first,op1.second), vertexGate);
  d_context.d_currentGraph->addEdge(find_operand(op2.first,op2.second), vertexGate);
}

void GraphReader::on_elem3(const std::string &lhs,
                           const std::pair<std::string, bool> &op1,
                           const std::pair<std::string, bool> &op2,
                           const std::pair<std::string, bool> &op3,
                           Gates gateType) const {
  on_elem(lhs, op1, op2, gateType);
  VertexPtr vertexGate = d_context.d_currentGraphNamesList[lhs];
  d_context.d_currentGraph->addEdge(find_operand(op3.first,op3.second), vertexGate);
}

void GraphReader::on_and(const std::string &lhs,
                         const std::pair<std::string, bool> &op1,
                         const std::pair<std::string, bool> &op2) const {
  on_elem(lhs, op1, op2, GateAnd);
}
void GraphReader::on_nand(const std::string &lhs,
                          const std::pair<std::string, bool> &op1,
                          const std::pair<std::string, bool> &op2) const {
  on_elem(lhs, op1, op2, GateNand);
}

void GraphReader::on_or(const std::string &lhs,
                        const std::pair<std::string, bool> &op1,
                        const std::pair<std::string, bool> &op2) const {
  on_elem(lhs, op1, op2, GateOr);
}

void GraphReader::on_nor(const std::string &lhs,
                         const std::pair<std::string, bool> &op1,
                         const std::pair<std::string, bool> &op2) const {
  on_elem(lhs, op1, op2, GateNor);
}

void GraphReader::on_xor(const std::string &lhs,
                         const std::pair<std::string, bool> &op1,
                         const std::pair<std::string, bool> &op2) const {
  on_elem(lhs, op1, op2, GateXor);
}

void GraphReader::on_xnor(const std::string &lhs,
                          const std::pair<std::string, bool> &op1,
                          const std::pair<std::string, bool> &op2) const {
  on_elem(lhs, op1, op2, GateXnor);
}

void GraphReader::on_and3(const std::string &lhs,
                          const std::pair<std::string, bool> &op1,
                          const std::pair<std::string, bool> &op2,
                          const std::pair<std::string, bool> &op3) const {
  on_elem3(lhs, op1, op2, op3, GateAnd);
}

void GraphReader::on_or3(const std::string &lhs,
                         const std::pair<std::string, bool> &op1,
                         const std::pair<std::string, bool> &op2,
                         const std::pair<std::string, bool> &op3) const {
  on_elem3(lhs, op1, op2, op3, GateOr);
}

void GraphReader::on_xor3(const std::string &lhs,
                          const std::pair<std::string, bool> &op1,
                          const std::pair<std::string, bool> &op2,
                          const std::pair<std::string, bool> &op3) const {
  on_elem3(lhs, op1, op2, op3, GateXor);
}

void GraphReader::on_maj3(const std::string &lhs,
                          const std::pair<std::string, bool> &op1,
                          const std::pair<std::string, bool> &op2,
                          const std::pair<std::string, bool> &op3) const {
  auto temp = d_context.d_currentGraphNamesList.find(lhs);
  if (temp != d_context.d_currentGraphNamesList.end()) {
    d_context.d_currentGraphNamesList[lhs]->~GraphVertexBase();
  }
  d_context.d_currentGraphNamesList[lhs] =
      d_context.d_currentGraph->generateMajority(find_operand(op1.first, op1.second),
                                                 find_operand(op2.first, op2.second),
                                                 find_operand(op3.first, op3.second));
  d_context.d_currentGraphNamesList[lhs]->setName(lhs);
}

void GraphReader::on_endmodule() const {
  d_context.d_graphs[d_context.d_currentGraph.get()->getName()] =
      d_context.d_currentGraph;
  d_context.d_currentGraphNamesList.clear();
  
}
} // namespace CG_Graph