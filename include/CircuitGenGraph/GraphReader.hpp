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

/**
 * \~english
 * @brief Parsing context shared across Verilog modules.
 * @param d_currentTopName Name of the current top module.
 * @param d_graphs Already parsed module graphs.
 * @param d_currentGraph Graph currently being built.
 * @param d_currentGraphNamesList Fast lookup by vertex name.
 * @param d_numberOfVertices Hint for upfront memory reservation.
 *
 * \~russian
 * @brief Контекст разбора, общий для Verilog-модулей.
 * @param d_currentTopName Имя текущего верхнего модуля.
 * @param d_graphs Уже распарсенные графы модулей.
 * @param d_currentGraph Граф, который сейчас строится.
 * @param d_currentGraphNamesList Быстрый поиск по имени вершины.
 * @param d_numberOfVertices Подсказка для предварительного резерва памяти.
 */
class Context {
public:
  Context() = default;
  std::string d_currentTopName;
  std::map<std::string, GraphPtr> d_graphs;
  GraphPtr d_currentGraph;
  std::unordered_map<std::string, VertexPtr> d_currentGraphNamesList;
  size_t d_numberOfVertices;
};

/**
 * \~english
 * @brief Verilog reader adapter that builds `OrientedGraph` via lorina
 * callbacks.
 * @param d_context Shared parse context (@see Context).
 *
 * \~russian
 * @brief Адаптер чтения Verilog, строящий `OrientedGraph` через callbacks
 * lorina.
 * @param d_context Общий контекст разбора (@see Context).
 */
class GraphReader final : public lorina::verilog_reader {
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
   * An output vertex can be used as a signal source for gates.
   * Each output is connected to exactly one logic gate,
   * which acts as its signal source.
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
  void on_wires(const std::vector<std::string> &wires,
                std::string const &size = "") const override;

  /*! \brief Callback method for parsed parameter definition of form ` parameter
   * M = 10;`.
   *
   * \param name Name of the parameter
   * \param value Value of the parameter
   */
  void on_parameter(const std::string &name,
                    const std::string &value) const override;

  /**
   * \~english
   * @brief Finds or creates a vertex by name and optional inversion flag.
   * @param i_name Verilog signal name.
   * @param i_isInverted If true, returns/creates inverted signal vertex.
   * @return Pointer to requested vertex.
   *
   * \~russian
   * @brief Находит или создает вершину по имени и флагу инверсии.
   * @param i_name Имя Verilog-сигнала.
   * @param i_isInverted Если true, возвращает/создает инверсную вершину.
   * @return Указатель на требуемую вершину.
   */
  VertexPtr get_vertex_by_name(const std::string &i_name,
                               bool i_isInverted = false) const;
  VertexPtr get_or_create_inversion(const std::string &i_name,
                                    VertexPtr i_vertex) const;
  /*! \brief Callback method for parsed immediate assignment of form `LHS = RHS
   * ;`.
   *
   * \param lhs Left-hand side of assignment
   * \param rhs Right-hand side of assignment
   */
  void on_assign(const std::string &lhs,
                 const std::pair<std::string, bool> &rhs) const override;

  /**
   * \~english
   * @brief Creates a gate of requested type and connects its inputs.
   * @param i_lhs Name of gate output signal.
   * @param i_op1 First input signal.
   * @param i_op2 Second input signal.
   *
   * \~russian
   * @brief Создает вентиль заданного типа и подключает его входы.
   * @param i_lhs Имя выходного сигнала вентиля.
   * @param i_op1 Первый входной сигнал.
   * @param i_op2 Второй входной сигнал.
   */
  void on_elem(const std::string &i_lhs,
               const std::pair<std::string, bool> &i_op1,
               const std::pair<std::string, bool> &i_op2,
               Gates i_gateType) const;

  /**
   * \~english
   * @brief Variant of @ref on_elem for three-input gates.
   * @param i_op3 Third input signal.
   *
   * \~russian
   * @brief Вариант @ref on_elem для вентилей с тремя входами.
   * @param i_op3 Третий входной сигнал.
   */
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

  /**
   * \~english
   * @brief Callback invoked at end of current module.
   *
   * \~russian
   * @brief Callback, вызываемый при завершении текущего модуля.
   */
  void on_endmodule() const override;

  VertexPtr getRightHS(const std::string &i_name,
                       bool i_isInverted = false) const;

  VertexPtr getLeftHS(const std::string &i_name,
                      bool i_isInverted = false) const;
  /* verilog_reader */
private:
  Context &d_context;
};

/**
 * @brief Custom diagnostic consumer for lorina that redirects diagnostics to CG_LOG.
 */
class LogDiagnosticConsumer : public lorina::diagnostic_consumer {
public:
  void handle_diagnostic(lorina::diagnostic_level level,
                         const std::string &message) const override;
};

} // namespace CG_Graph