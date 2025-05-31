#pragma once

#include <CircuitGenGraph/OrientedGraph.hpp>

#include <lorina/lorina.hpp>

#include <map>

#define GraphPtr std::shared_ptr<CG_Graph::OrientedGraph>
#define GraphPtrWeak std::weak_ptr<CG_Graph::OrientedGraph>

namespace CG_Graph {

class OrientedGraph;

class GraphReader : public lorina::verilog_reader {
public:
  void on_module_header(const std::string &module_name,
                        const std::vector<std::string> &inouts);
  /*! \brief Callback method for parsed module.
   *
   * \param module_name Name of the module
   * \param inouts Container for input and output names
   */
  void on_module_header(const std::string &module_name,
                        const std::vector<std::string> &inouts) const;

  /*! \brief Callback method for parsed inputs.
   *
   * \param inputs Input names
   * \param size Size modifier
   */
  void on_inputs(const std::vector<std::string> &inputs,
                 std::string const &size) const;

  /*! \brief Callback method for parsed outputs.
   *
   * \param outputs Output names
   * \param size Size modifier
   */
  void on_outputs(const std::vector<std::string> &outputs,
                  std::string const &size = "") const;

  /*! \brief Callback method for parsed wires.
   *
   * \param wires Wire names
   * \param size Size modifier
   */
  void on_wires(const std::vector<std::string> &wires,
                std::string const &size = "") const;

  /*! \brief Callback method for parsed parameter definition of form ` parameter
   * M = 10;`.
   *
   * \param name Name of the parameter
   * \param value Value of the parameter
   */
  void on_parameter(const std::string &name, const std::string &value) const;

  /*! \brief Callback method for parsed immediate assignment of form `LHS = RHS
   * ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param rhs Right-hand side of assignment
   */
  void on_assign(const std::string &lhs,
                 const std::pair<std::string, bool> &rhs) const;

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
    const std::pair<std::string, bool> &op2, Gates gateType) const;
  void on_and(const std::string &lhs, const std::pair<std::string, bool> &op1,
              const std::pair<std::string, bool> &op2) const;
void GraphReader::on_elem3(const std::string &lhs, const std::pair<std::string, bool> &op1,
    const std::pair<std::string, bool> &op2, const std::pair<std::string, bool> &op3,
     Gates gateType) const;
  /*! \brief Callback method for parsed NAND-gate with 2 operands `LHS = ~(OP1 &
   * OP2) ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   */
  void on_nand(const std::string &lhs, const std::pair<std::string, bool> &op1,
               const std::pair<std::string, bool> &op2) const;

  /*! \brief Callback method for parsed OR-gate with 2 operands `LHS = OP1 | OP2
   * ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   */
  void on_or(const std::string &lhs, const std::pair<std::string, bool> &op1,
             const std::pair<std::string, bool> &op2) const;
  /*! \brief Callback method for parsed NOR-gate with 2 operands `LHS = ~(OP1 |
   * OP2) ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   */
  void on_nor(const std::string &lhs, const std::pair<std::string, bool> &op1,
              const std::pair<std::string, bool> &op2) const;

  /*! \brief Callback method for parsed XOR-gate with 2 operands `LHS = OP1 ^
   * OP2 ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   */
  void on_xor(const std::string &lhs, const std::pair<std::string, bool> &op1,
              const std::pair<std::string, bool> &op2) const;

  /*! \brief Callback method for parsed XOR-gate with 2 operands `LHS = ~(OP1 ^
   * OP2) ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   */
  void on_xnor(const std::string &lhs, const std::pair<std::string, bool> &op1,
               const std::pair<std::string, bool> &op2) const;

  /*! \brief Callback method for parsed AND-gate with 3 operands `LHS = OP1 &
   * OP2 & OP3 ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   * \param op3 operand3 of assignment
   */
  void on_and3(const std::string &lhs, const std::pair<std::string, bool> &op1,
               const std::pair<std::string, bool> &op2,
               const std::pair<std::string, bool> &op3) const;

  /*! \brief Callback method for parsed OR-gate with 3 operands `LHS = OP1 | OP2
   * | OP3 ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   * \param op3 operand3 of assignment
   */
  void on_or3(const std::string &lhs, const std::pair<std::string, bool> &op1,
              const std::pair<std::string, bool> &op2,
              const std::pair<std::string, bool> &op3) const;
  /*! \brief Callback method for parsed XOR-gate with 3 operands `LHS = OP1 ^
   * OP2 ^ OP3 ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   * \param op3 operand3 of assignment
   */
  void on_xor3(const std::string &lhs, const std::pair<std::string, bool> &op1,
               const std::pair<std::string, bool> &op2,
               const std::pair<std::string, bool> &op3) const;
  /*! \brief Callback method for parsed majority-of-3 gate `LHS = ( OP1 & OP2 )
   * | ( OP1 & OP3 ) | ( OP2 & OP3 ) ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   * \param op3 operand3 of assignment
   */
  void on_maj3(const std::string &lhs, const std::pair<std::string, bool> &op1,
               const std::pair<std::string, bool> &op2,
               const std::pair<std::string, bool> &op3) const;

  /*! \brief Callback method for parsed 2-to-1 multiplexer gate `LHS = OP1 ? OP2
   * : OP3 ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   * \param op3 operand3 of assignment
   */
  void on_mux21(const std::string &lhs, const std::pair<std::string, bool> &op1,
                const std::pair<std::string, bool> &op2,
                const std::pair<std::string, bool> &op3) const;

  /*! \brief Callback method for parsed comments `// comment string`.
   *
   * \param comment Comment string
   */
  void on_comment(std::string const &comment);

  /*! \brief Callback method for parsed endmodule.
   *
   */
  void on_endmodule() const;
  /* verilog_reader */
  GraphPtr getGraphByName(std::string_view name);

private:
  mutable std::map<std::string, GraphPtr> graphs;
  mutable GraphPtr currentGraph;
};

} // namespace CG_Graph