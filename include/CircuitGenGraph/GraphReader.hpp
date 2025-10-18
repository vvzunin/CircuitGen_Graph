#pragma once

#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <cstddef>
#include <lorina/lorina.hpp>

#include <map>

#define GraphPtr std::shared_ptr<CG_Graph::OrientedGraph>
#define GraphPtrWeak std::weak_ptr<CG_Graph::OrientedGraph>
#define VertexPtr CG_Graph::GraphVertexBase *
namespace CG_Graph {

class OrientedGraph;
class GraphVertexBase;

class Context {
  /// class Context contains information used for parsing each graph
  /// and provides possibility to parse module, if it's submodules
  /// locates in several files (not ready yet)
  /// @param d_currentTopName name of top module in operation of
  /// reading module with instantiation of other modules
  /// @param d_graphs contains all already parsed modules
  /// @param d_currentGraph storage for graph while it is creating
  /// @param d_currentGraphNamesList using for fast search vertices by name
  /// @param d_numberOfVertices using to reserve memory at the beginning of
public:
  Context() = default;
  std::string d_currentTopName;
  std::map<std::string, GraphPtr> d_graphs;
  GraphPtr d_currentGraph;
  std::unordered_map<std::string, VertexPtr> d_currentGraphNamesList;
  size_t d_numberOfVertices;
};

class GraphReader : public lorina::verilog_reader {
  /// @file GraphReader.hpp
  /// class GraphReader using for parsing a object of class OrientedGraph from
  /// verilog file by module "lorina". Library calls methods from that class,
  /// which overrides virtual methods of verilog_reader.
  /// @param d_context @see Context
  /// */
public:
  GraphReader(Context &i_context);
  /*! \brief Callback method for parsed module.
   *
   * \param module_name Name of the module
   * \param inouts Container for input and output names
   */
  void on_module_header(const std::string &module_name,
                        const std::vector<std::string> &inouts) const override;

  /*! \brief Callback method for parsed inputs.
   *
   * \param inputs Input names
   * \param size Size modifier
   */
  void on_inputs(const std::vector<std::string> &inputs,
                 std::string const &size) const override;

  /*! \brief Callback method for parsed outputs.
   *
   * \param outputs Output names
   * \param size Size modifier
   */
  void on_outputs(const std::vector<std::string> &outputs,
                  std::string const &size = "") const override;

  /*! \brief Callback method for parsed wires.
   *
   * \param wires Wire names
   * \param size Size modifier
   */
  virtual void on_wires(const std::vector<std::string> &wires,
                        std::string const &size = "") const override;

  /*! \brief Callback method for parsed parameter definition of form ` parameter
   * M = 10;`.
   *
   * \param name Name of the parameter
   * \param value Value of the parameter
   */
  void on_parameter(const std::string &name,
                    const std::string &value) const override;

  /// @brief This method is used to find or create valid vertex by
  /// name and inversion parameter provided by lorina
  /// @param i_name name of verilog variable
  /// @param i_isInverted method returns (creates if it is not exist yet)
  /// an inversion of given variable if the
  /// parameter = true and original vertex otherwise
  /// @return pointer for requested vertex
  VertexPtr get_operand(const std::string &i_name,
                        bool i_isInverted = false) const;
  VertexPtr get_or_create_inversion(const std::string &i_name,
                                    VertexPtr i_vertex) const;
  /*! \brief Callback method for parsed immediate assignment of form `LHS = RHS
   * ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param rhs Right-hand side of assignment
   */
  virtual void
  on_assign(const std::string &lhs,
            const std::pair<std::string, bool> &rhs) const override;

  /// @brief This method is used to create all types of gates by provided
  /// Gates gateType and connect it with its input signals
  /// @param i_lhs name of gate that will be defined
  /// @param i_op1, i_op2 input signals of the gate
  void on_elem(const std::string &i_lhs,
               const std::pair<std::string, bool> &i_op1,
               const std::pair<std::string, bool> &i_op2,
               Gates i_gateType) const;

  /// @brief @see on_elem
  /// @param i_op3 also input signals of the gate
  void on_elem3(const std::string &i_lhs,
                const std::pair<std::string, bool> &i_op1,
                const std::pair<std::string, bool> &i_op2,
                const std::pair<std::string, bool> &i_op3,
                Gates i_gateType) const;

  /*! \brief Callback method for parsed AND-gate with 2 operands `LHS = OP1 &
   * OP2 ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   */
  void on_and(const std::string &lhs, const std::pair<std::string, bool> &op1,
              const std::pair<std::string, bool> &op2) const override;

  /*! \brief Callback method for parsed NAND-gate with 2 operands `LHS = ~(OP1 &
   * OP2) ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   */
  void on_nand(const std::string &lhs, const std::pair<std::string, bool> &op1,
               const std::pair<std::string, bool> &op2) const override;

  /*! \brief Callback method for parsed OR-gate with 2 operands `LHS = OP1 | OP2
   * ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   */
  void on_or(const std::string &lhs, const std::pair<std::string, bool> &op1,
             const std::pair<std::string, bool> &op2) const override;
  /*! \brief Callback method for parsed NOR-gate with 2 operands `LHS = ~(OP1 |
   * OP2) ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   */
  void on_nor(const std::string &lhs, const std::pair<std::string, bool> &op1,
              const std::pair<std::string, bool> &op2) const override;

  /*! \brief Callback method for parsed XOR-gate with 2 operands `LHS = OP1 ^
   * OP2 ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   */
  void on_xor(const std::string &lhs, const std::pair<std::string, bool> &op1,
              const std::pair<std::string, bool> &op2) const override;

  /*! \brief Callback method for parsed XOR-gate with 2 operands `LHS = ~(OP1 ^
   * OP2) ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param op1 operand1 of assignment
   * \param op2 operand2 of assignment
   */
  void on_xnor(const std::string &lhs, const std::pair<std::string, bool> &op1,
               const std::pair<std::string, bool> &op2) const override;

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
               const std::pair<std::string, bool> &op3) const override;

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
              const std::pair<std::string, bool> &op3) const override;
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
               const std::pair<std::string, bool> &op3) const override;
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
               const std::pair<std::string, bool> &op3) const override;

  void on_endmodule() const override;
  /* verilog_reader */
private:
  Context &d_context;
};
} // namespace CG_Graph