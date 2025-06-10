#include<CircuitGenGraph/OrientedGraph.hpp>
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <lorina/lorina.hpp>
namespace CG_Graph {

void GraphReader::on_module_header(const std::string& module_name, const std::vector<std::string> & inouts) const {
  currentGraph = std::make_shared<OrientedGraph>(module_name);
  for (auto i : inouts)
    std::cout << i;
}
void GraphReader::on_inputs( const std::vector<std::string>& inputs, std::string const& size) const {
  for (auto i : inputs) 
    currentGraphNamesList[i] = currentGraph->addInput(i);

}
void GraphReader::on_outputs( const std::vector<std::string>& inputs, std::string const& size) const {
  for (auto i : inputs) 
    currentGraphNamesList[i] = currentGraph->addOutput(i);
}

void GraphReader::on_parameter(const std::string &name, const std::string &value) const {}

void GraphReader::on_assign(const std::string &lhs,
   const std::pair<std::string, bool> &rhs) const {
    std::cout << lhs <<"\n" << rhs.first << rhs.second<<"\n";
}

VertexPtr GraphReader::find_operand(const std::string& name) const {
  auto temp = currentGraphNamesList.find(name);
  VertexPtr ans;
  if (temp == currentGraphNamesList.end()) {
    ans = currentGraph->addGate(GateDefault,name);
    currentGraphNamesList[name] = ans;
  } else ans = currentGraphNamesList[name];  
  return ans;
}

void GraphReader::on_elem(const std::string &lhs, const std::pair<std::string, bool> &op1,
  const std::pair<std::string, bool> &op2, Gates gateType) const{
    VertexPtr vertexGate = find_operand(lhs);
    static_cast<GraphVertexGates*>(vertexGate)->setGateType(gateType);
    currentGraph->addEdge(find_operand(op1.first),vertexGate);
    currentGraph->addEdge(find_operand(op2.first),vertexGate);
  }

  void GraphReader::on_elem3(const std::string &lhs, const std::pair<std::string, bool> &op1,
    const std::pair<std::string, bool> &op2, const std::pair<std::string, bool> &op3,
     Gates gateType) const{
      on_elem(lhs,op1,op2,gateType);
      VertexPtr vertexGate = currentGraphNamesList[lhs];      
      currentGraph->addEdge(find_operand(op3.first),vertexGate);
    }
  
void GraphReader::on_and(const std::string &lhs, const std::pair<std::string, bool> &op1,
const std::pair<std::string, bool> &op2) const{
on_elem(lhs,op1,op2,GateAnd);
}
void GraphReader::on_nand(const std::string &lhs, const std::pair<std::string, bool> &op1,
 const std::pair<std::string, bool> &op2) const{
  on_elem(lhs,op1,op2,GateNand);
 }

void GraphReader::on_or(const std::string &lhs, const std::pair<std::string, bool> &op1,
const std::pair<std::string, bool> &op2) const{
  on_elem(lhs,op1,op2,GateOr);
}

void GraphReader::on_nor(const std::string &lhs, const std::pair<std::string, bool> &op1,
const std::pair<std::string, bool> &op2) const{
  on_elem(lhs,op1,op2,GateNor);
}

void GraphReader::on_xor(const std::string &lhs, const std::pair<std::string, bool> &op1,
const std::pair<std::string, bool> &op2) const{ VertexPtr vGate = currentGraph->addGate(Gates::GateXor, lhs);
  on_elem(lhs,op1,op2,GateXor);  
}

void GraphReader::on_xnor(const std::string &lhs, const std::pair<std::string, bool> &op1,
 const std::pair<std::string, bool> &op2) const { VertexPtr vGate = currentGraph->addGate(Gates::GateXnor, lhs);
  on_elem(lhs,op1,op2,GateXnor);
}

void GraphReader::on_and3(const std::string &lhs, const std::pair<std::string, bool> &op1,
 const std::pair<std::string, bool> &op2,
 const std::pair<std::string, bool> &op3) const{
  on_elem3(lhs,op1,op2,op3,GateAnd);
 }

void GraphReader::on_or3(const std::string &lhs, const std::pair<std::string, bool> &op1,
const std::pair<std::string, bool> &op2,
const std::pair<std::string, bool> &op3) const{
  on_elem3(lhs,op1,op2,op3,GateOr);
}

void GraphReader::on_xor3(const std::string &lhs, const std::pair<std::string, bool> &op1,
 const std::pair<std::string, bool> &op2,
 const std::pair<std::string, bool> &op3) const{
  on_elem3(lhs,op1,op2,op3,GateXor);
 }

void GraphReader::on_maj3(const std::string &lhs, const std::pair<std::string, bool> &op1,
 const std::pair<std::string, bool> &op2,
 const std::pair<std::string, bool> &op3) const{
  auto temp = currentGraphNamesList.find(lhs);
  if(temp != currentGraphNamesList.end()) {
    currentGraphNamesList[lhs]->~GraphVertexBase();
  } 
  currentGraphNamesList[lhs] = currentGraph->generateMajority(find_operand(op1.first), find_operand(op2.first), find_operand(op3.first));
  currentGraphNamesList[lhs]->setName(lhs);
}

void GraphReader::on_endmodule() const {
  graphs[currentGraph.get()->getName()] = currentGraph;
  currentGraphNamesList.clear();
}
GraphPtr GraphReader::getGraphByName(std::string_view name) {
    return this->graphs[std::string(name)];
}

} // namespace CG_Graph