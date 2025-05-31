#include<CircuitGenGraph/OrientedGraph.hpp>
#include <CircuitGenGraph/GraphReader.hpp>
#include <lorina/lorina.hpp>
namespace CG_Graph {

void GraphReader::on_module_header(const std::string& module_name, const std::vector<std::string> & inouts) {
  currentGraph = std::make_shared<OrientedGraph>(module_name);
  for (auto i : inouts)
    std::cout << i;
}
void GraphReader::on_inputs( const std::vector<std::string>& inputs, std::string const& size) const {
  for (auto i : inputs) 
    std::cout << i;
}
void GraphReader::on_outputs( const std::vector<std::string>& inputs, std::string const& size) const {
    for (auto i : inputs) 
      std::cout << i;
}
void GraphReader::on_wires(const std::vector<std::string> &wires,
  std::string const &size) const{}

/*! \brief Callback method for parsed parameter definition of form ` parameter
* M = 10;`.
*
* \param name Name of the parameter
* \param value Value of the parameter
*/
void GraphReader::on_parameter(const std::string &name, const std::string &value) const{}

/*! \brief Callback method for parsed immediate assignment of form `LHS = RHS
* ;`.
*
* \param lhs Left-hand side of assignment
* \param rhs Right-hand side of assignment
*/
void GraphReader::on_assign(const std::string &lhs,
   const std::pair<std::string, bool> &rhs) const{
    std::cout << lhs <<"\n" << rhs.first << rhs.second<<"\n";
   }

/*! \brief Callback method for parsed module instantiation of form `NAME
* #(P1,P2) NAME(.SIGNAL(SIGANL), ..., .SIGNAL(SIGNAL));`
*
* \param module_name Name of the module
* \param params List of parameters
* \param inst_name Name of the instantiation
* \param args List (a_1,b_1), ..., (a_n,b_n) of name pairs, where
*             a_i is a name of a signals in module_name and b_i is a name of
* a signal in inst_name.
*/
void GraphReader::on_elem(const std::string &lhs, const std::pair<std::string, bool> &op1,
  const std::pair<std::string, bool> &op2, Gates gateType) const{
    VertexPtr vGate = currentGraph->addGate(gateType, lhs);
    std::vector<VertexPtr> vert1 = currentGraph->getVerticesByName(std::string_view(op1.first));
    if(vert1.size()>1) throw std::length_error("");
    else
    currentGraph->addEdge(currentGraph->getVerticesByName(std::string_view(op1.first))[0],vGate);
   
    std::vector<VertexPtr> vert2 = currentGraph->getVerticesByName(std::string_view(op2.first));
    if(vert2.size()>1) throw std::length_error("");
    else
    currentGraph->addEdge(currentGraph->getVerticesByName(std::string_view(op2.first))[0],vGate);
  }

  void GraphReader::on_elem3(const std::string &lhs, const std::pair<std::string, bool> &op1,
    const std::pair<std::string, bool> &op2, const std::pair<std::string, bool> &op3,
     Gates gateType) const{
      on_elem(lhs,op1,op2,gateType);      
      VertexPtr vGate = currentGraph->getVerticesByName(std::string_view(lhs))[0];
      std::vector<VertexPtr> vert3 = currentGraph->getVerticesByName(std::string_view(op3.first));
    if(vert3.size()>1) throw std::length_error("");
    else
    currentGraph->addEdge(currentGraph->getVerticesByName(std::string_view(op3.first))[0],vGate);
    }
  
void GraphReader::on_and(const std::string &lhs, const std::pair<std::string, bool> &op1,
const std::pair<std::string, bool> &op2) const{
on_elem(lhs,op1,op2,GateAnd);
}

/*! \brief Callback method for parsed NAND-gate with 2 operands `LHS = ~(OP1 &
* OP2) ;`.
*
* \param lhs Left-hand side of assignment
* \param op1 operand1 of assignment
* \param op2 operand2 of assignment
*/
void GraphReader::on_nand(const std::string &lhs, const std::pair<std::string, bool> &op1,
 const std::pair<std::string, bool> &op2) const{
  on_elem(lhs,op1,op2,GateNand);
 }

/*! \brief Callback method for parsed OR-gate with 2 operands `LHS = OP1 | OP2
* ;`.
*
* \param lhs Left-hand side of assignment
* \param op1 operand1 of assignment
* \param op2 operand2 of assignment
*/
void GraphReader::on_or(const std::string &lhs, const std::pair<std::string, bool> &op1,
const std::pair<std::string, bool> &op2) const{
  on_elem(lhs,op1,op2,GateOr);
}
/*! \brief Callback method for parsed NOR-gate with 2 operands `LHS = ~(OP1 |
* OP2) ;`.
*
* \param lhs Left-hand side of assignment
* \param op1 operand1 of assignment
* \param op2 operand2 of assignment
*/
void GraphReader::on_nor(const std::string &lhs, const std::pair<std::string, bool> &op1,
const std::pair<std::string, bool> &op2) const{
  on_elem(lhs,op1,op2,GateNor);
}

/*! \brief Callback method for parsed XOR-gate with 2 operands `LHS = OP1 ^
* OP2 ;`.
*
* \param lhs Left-hand side of assignment
* \param op1 operand1 of assignment
* \param op2 operand2 of assignment
*/
void GraphReader::on_xor(const std::string &lhs, const std::pair<std::string, bool> &op1,
const std::pair<std::string, bool> &op2) const{ VertexPtr vGate = currentGraph->addGate(Gates::GateXor, lhs);
  on_elem(lhs,op1,op2,GateXor);  
}

/*! \brief Callback method for parsed XOR-gate with 2 operands `LHS = ~(OP1 ^
* OP2) ;`.
*
* \param lhs Left-hand side of assignment
* \param op1 operand1 of assignment
* \param op2 operand2 of assignment
*/
void GraphReader::on_xnor(const std::string &lhs, const std::pair<std::string, bool> &op1,
 const std::pair<std::string, bool> &op2) const { VertexPtr vGate = currentGraph->addGate(Gates::GateXnor, lhs);
  on_elem(lhs,op1,op2,GateXnor);
}

/*! \brief Callback method for parsed AND-gate with 3 operands `LHS = OP1 &
* OP2 & OP3 ;`.
*
* \param lhs Left-hand side of assignment
* \param op1 operand1 of assignment
* \param op2 operand2 of assignment
* \param op3 operand3 of assignment
*/
void GraphReader::on_and3(const std::string &lhs, const std::pair<std::string, bool> &op1,
 const std::pair<std::string, bool> &op2,
 const std::pair<std::string, bool> &op3) const{
  on_elem3(lhs,op1,op2,op3,GateAnd);
 }

/*! \brief Callback method for parsed OR-gate with 3 operands `LHS = OP1 | OP2
* | OP3 ;`.
*
* \param lhs Left-hand side of assignment
* \param op1 operand1 of assignment
* \param op2 operand2 of assignment
* \param op3 operand3 of assignment
*/
void GraphReader::on_or3(const std::string &lhs, const std::pair<std::string, bool> &op1,
const std::pair<std::string, bool> &op2,
const std::pair<std::string, bool> &op3) const{
  on_elem3(lhs,op1,op2,op3,GateOr);
}
/*! \brief Callback method for parsed XOR-gate with 3 operands `LHS = OP1 ^
* OP2 ^ OP3 ;`.
*
* \param lhs Left-hand side of assignment
* \param op1 operand1 of assignment
* \param op2 operand2 of assignment
* \param op3 operand3 of assignment
*/
void GraphReader::on_xor3(const std::string &lhs, const std::pair<std::string, bool> &op1,
 const std::pair<std::string, bool> &op2,
 const std::pair<std::string, bool> &op3) const{
  on_elem3(lhs,op1,op2,op3,GateXor);
 }
/*! \brief Callback method for parsed majority-of-3 gate `LHS = ( OP1 & OP2 )
* | ( OP1 & OP3 ) | ( OP2 & OP3 ) ;`.
*
* \param lhs Left-hand side of assignment
* \param op1 operand1 of assignment
* \param op2 operand2 of assignment
* \param op3 operand3 of assignment
*/
void GraphReader::on_maj3(const std::string &lhs, const std::pair<std::string, bool> &op1,
 const std::pair<std::string, bool> &op2,
 const std::pair<std::string, bool> &op3) const{
 }

/*! \brief Callback method for parsed 2-to-1 multiplexer gate `LHS = OP1 ? OP2
* : OP3 ;`.
*
* \param lhs Left-hand side of assignment
* \param op1 operand1 of assignment
* \param op2 operand2 of assignment
* \param op3 operand3 of assignment
*/
void GraphReader::on_mux21(const std::string &lhs, const std::pair<std::string, bool> &op1,
  const std::pair<std::string, bool> &op2,
  const std::pair<std::string, bool> &op3) const{}

/*! \brief Callback method for parsed comments `// comment string`.
*
* \param comment Comment string
*/
void GraphReader::on_comment(std::string const &comment){}

/*! \brief Callback method for parsed endmodule.
*
*/
void GraphReader::on_endmodule() const {
graphs[currentGraph.get()->getName()] = currentGraph;
}
GraphPtr GraphReader::getGraphByName(std::string_view name) {
    return this->graphs[std::string(name)];
}

}