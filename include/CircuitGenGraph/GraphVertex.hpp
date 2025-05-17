#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <CircuitGenGraph/GraphVertexBase.hpp>
#include "span.hpp"

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

/// @file GraphVertex.hpp
/// TODO: Maybe Description some virtual methods for Graph's classes,
/// such as GraphVertexGates

/// class GraphVertexInput Represents a vertex in a directed graph that
/// serves as an input vertex. It inherits from the GraphVertexBase class
/// and extends its functionality to work with input vertices
/// @param i_baseGraph A pointer to the oriented graph to which this vertex
/// belongs. It can be null ptr, which indicates the absence of a base graph
/// @param i_type Vertex type (VertexTypes enumeration). By default, it is
/// set to VertexTypes::input, indicating that this is the input vertex
/// @param i_name The name of the vertex provided during creation. This is
/// a string of type std::string that identifies a vertex
/// */

namespace CG_Graph {

class GraphVertexInput : public GraphVertexBase {
public:
  /// @brief GraphVertexInput
  /// Initializes the GraphVertexInput object with the provided base graph
  /// pointer and type
  /// @param i_baseGraph Pointer to the base graph. Default is nullptr.
  /// @param i_type i_type Type of the vertex. Default is VertexTypes::input.

  GraphVertexInput(GraphPtr i_baseGraph,
                   const VertexTypes i_type = VertexTypes::input);

  /// @brief GraphVertexInput
  /// Initializes the GraphVertexInput object with the provided name, base
  /// graph pointer, and type
  /// @param i_name Name of the vertex.
  /// @param i_baseGraph Pointer to the base graph.
  /// @param i_type Type of the vertex.
  GraphVertexInput(std::string_view i_name, GraphPtr i_baseGraph,
                   const VertexTypes i_type = VertexTypes::input);

  /// @brief updateValue A virtual function for updating the vertex value.
  /// The implementation is provided in derived classes
  /// @return the value of the vertex after its update
  /// @code
  /// TO DO:
  /// @endcode
  /// @throws std::invalid_argument if any input connection is invalid

  virtual char updateValue() override;

  /// @brief updateLevel It is designed to update the level of the current
  /// vertex in a directed graph based on the levels of its input connections

  virtual void updateLevel(bool i_recalculate = false,
                           std::string tab = "") override;

  DotReturn toDOT() override;

  /// @brief log Used for easylogging++
  /// @param os Stream for easylogging
#ifdef LOGFLAG
  virtual void log(el::base::type::ostream_t &os) const override;
#endif

protected:
  GraphVertexInput(VertexTypes i_type);

private:
};

/// class GraphVertexConstant TO DO: Description
///
///
/// </summary>

class GraphVertexConstant : public GraphVertexInput {
public:
  GraphVertexConstant(char i_const, GraphPtr i_baseGraph);

  GraphVertexConstant(char i_const, std::string_view i_name,
                      GraphPtr i_baseGraph);

  ~GraphVertexConstant() override{};

  size_t calculateHash(bool i_recalculate = false) override;

  /// @brief updateLevel updates the level of the current vertex in the graph
  /// based on the levels of its incoming connections

  virtual void updateLevel(bool i_recalculate = false,
                           std::string tab = "") override;
  std::string toVerilog() const override;
  DotReturn toDOT() override;

  /// @brief getDefaultInstance
  /// TO DO:

  std::string getVerilogInstance();

  /// @brief log Used for easylogging++
  /// @param os Stream for easylogging
#ifdef LOGFLAG
  virtual void log(el::base::type::ostream_t &os) const override;
#endif

private:
};

/// class GraphVertexSubGraph It is a class, having a subGruph pointer
/// inside. Is used for storing this pointer for providing graph
/// connectivity
/// @param d_subGraph Pointer to the subgraph associated with this vertex
/// @param d_hashed Cached hash value of the vertex
class GraphVertexSubGraph : public GraphVertexBase {
public:
  GraphVertexSubGraph(GraphPtr i_subGraph, GraphPtr i_baseGraph);

  GraphVertexSubGraph(GraphPtr i_subGraph, std::string_view i_name,
                      GraphPtr i_baseGraph);

  ~GraphVertexSubGraph() override{};

  char updateValue() override;
  void updateLevel(bool i_recalculate = false, std::string tab = "") override;

  std::string toVerilog() const override;
  DotReturn toDOT() override;

  /// @brief This method is used as a substructure for
  /// OrientedGraph methods
  /// @param i_path path to future file storing. Do not add submodule here,
  /// folder would be created there
  /// @param i_filename name of file to be created (default is same as graph
  /// name)
  /// @return pair, first is bool, meaning was file writing successful or not
  /// and second is string, for graph is empty, for subgraph is module instance
  std::pair<bool, std::string> toVerilog(std::string i_path,
                                         std::string i_filename = "");

  std::pair<bool, std::string> toDOT(std::string i_path,
                                     std::string i_filename = "");

  /// @brief This method is used as a substructure for
  /// OrientedGraph methods
  /// @param i_fileStream TO DO:
  /// @return TO DO:
  bool toGraphML(std::ofstream &i_fileStream) const;

  /// @brief This method is used as a substructure for
  /// OrientedGraph methods
  /// @param i_indent
  /// @return
  std::string toGraphML(uint16_t i_indent = 0, std::string i_prefix = "") const;

  /// @brief This method is used as a substructure for
  /// OrientedGraph methods
  /// @return
  std::string getVerilogInstance() override;

  size_t calculateHash(bool i_recalculate = false) override;

  void setSubGraph(GraphPtr i_subGraph);
  GraphPtr getSubGraph() const;
  std::vector<VertexPtr>
  getOutputBuffersByOuterInput(VertexPtr i_outerInput) const;
  std::vector<VertexPtr>
  getOuterInputsByOutputBuffer(VertexPtr i_outputBuffer) const;

  /// @brief log Used for easylogging++
  /// @param os Stream for easylogging
#ifdef LOGFLAG
  virtual void log(el::base::type::ostream_t &os) const override;
#endif

private:
  GraphPtr d_subGraph;
};

/// class GraphVertexOutput It is a vertex of the graph, specially designed for
/// data output. It inherits from the GraphVertexBase class and adds additional
/// functionality related to data output

class GraphVertexOutput : public GraphVertexBase {
public:
  GraphVertexOutput(GraphPtr i_baseGraph);

  GraphVertexOutput(std::string_view i_name, GraphPtr i_baseGraph);

  /// @brief updateValue updates the value of the current vertex of the graph
  /// based on the values of its incoming connections and the type of logical
  /// element (or "gate"). Depending on the type of gate and the values of the
  /// incoming links, the method uses the truth tables to perform the
  /// corresponding logical operation and sets a new value for the current
  /// vertex
  /// @return A char that represents the new value of the current vertex after
  /// updating based on the values of its incoming connections and the type of
  /// logical element (or "gate").

  virtual char updateValue() override;

  /// @brief updateLevel updates the level of the current vertex in the graph
  /// based on the levels of its incoming connections. If a vertex has incoming
  /// connections, the method determines the highest level among all the
  /// vertices to which it is connected, and sets the level of the current
  /// vertex to one higher than the highest level

  virtual void updateLevel(bool i_recalculate = false,
                           std::string tab = "") override;

  DotReturn toDOT() override;

  /// @brief log Used for easylogging++
  /// @param os Stream for easylogging
#ifdef LOGFLAG
  virtual void log(el::base::type::ostream_t &os) const override;
#endif

private:
};

/// @brief GraphVertexGates
/// Class representing a vertex in a graph with gates

class GraphVertexGates : public GraphVertexBase {
public:
  GraphVertexGates(Gates i_gate, GraphPtr i_baseGraph);

  GraphVertexGates(Gates i_gate, std::string_view i_name, GraphPtr i_baseGraph);

  ~GraphVertexGates() override{};

  /// @brief updateValue
  /// Updates the value of the vertex
  /// @return The updated value of the vertex
  /// @code
  /// TO DO:
  /// @endcode
  /// @throws std::invalid_argument if any of the input connections point
  /// to a nullptr

  virtual char updateValue() override;

  /// @brief calculateHash
  /// Calculates the hash value of the vertex
  /// @param i_recalculate Flag indicating whether to i_recalculate the hash
  /// value (default false)
  /// @throws None.
  /// @code
  /// TO DO:
  /// @endcode
  /// @return The calculated hash value as a string

  size_t calculateHash(bool i_recalculate = false) override;

  /// @brief getVerilogString
  /// Gets a string in Verilog format representing the current vertex
  /// @return A string in Verilog format representing the current vertex
  /// @throws std::invalid_argument if any input connection is invalid

  std::string getVerilogString() const;

  /// @brief getGate
  /// Returns the type of valve associated with the current vertex
  /// @return The type of valve associated with the current vertex
  /// @code
  /// GraphVertexGates vertex(Gates::GateAnd);
  /// Gates gateType = vertex.getGate();
  /// std:: cout << "Gate type : " << gateType << std::endl;
  /// @endcode

  Gates getGate() const;

  /// @brief toVerilog
  /// generates a string in Verilog format for the current vertex,
  /// representing the valve according to its type and input connections.
  /// If a vertex has no input connections, an empty string is returned.
  /// If any input connection is invalid, an exception is thrown.
  /// @return A Verilog format string for the current vertex
  /// @throws std::invalid_argument if any input connection is invalid

  std::string toVerilog() const override;
  DotReturn toDOT() override;

  bool isSubgraphBuffer() const override;

  /// @brief log Used for easylogging++
  /// @param os Stream for easylogging
#ifdef LOGFLAG
  virtual void log(el::base::type::ostream_t &os) const override;
#endif

private:
  Gates d_gate;
  // Определяем тип вершины: подграф, вход, выход, константа или одна из базовых
  // логических операций.
};

/// class GraphVertexDataBus Represents a data bus
class GraphVertexDataBus : public GraphVertexBase {
public:
  GraphVertexDataBus(tcb::span<VertexPtr> i_vertices, GraphPtr i_baseGraph);
  GraphVertexDataBus(tcb::span<VertexPtr> i_vertices, std::string_view i_name,
                     GraphPtr i_baseGraph);
  GraphVertexDataBus(tcb::span<VertexPtr> i_vertices,
                     const GraphVertexDataBus &i_vertexDataBus);
  char updateValue() override;
  void updateLevel(bool i_recalculate = false,
                           std::string tab = "") override;
  GraphVertexDataBus slice(size_t startBit, size_t endBit) const;
  VertexPtr operator[](size_t index) const;
  std::string toVerilog(bool flag) const;
  DotReturn toDOT() override;
  size_t getWidth() const;
  std::string toVerilog() const override;
  ~GraphVertexDataBus() override{};
  size_t calculateHash(bool i_recalculate = false) override {return 0;};

  #ifdef LOGFLAG
  virtual void log(el::base::type::ostream_t &os) const override;
  #endif

private:
  tcb::span<VertexPtr> d_vertices;
};

class GraphVertexSequential : public GraphVertexBase {
public:
  // clang-format off
    GraphVertexSequential(SequentialTypes i_type,
                          VertexPtr i_clk,
                          VertexPtr i_data,
                          GraphPtr i_baseGraph,
                          std::string_view i_name);
  
    GraphVertexSequential(SequentialTypes i_type,
                          VertexPtr i_clk,
                          VertexPtr i_data,
                          VertexPtr wire,
                          GraphPtr i_baseGraph,
                          std::string_view i_name);
  
    GraphVertexSequential(SequentialTypes i_type,
                          VertexPtr i_clk,
                          VertexPtr i_data,
                          VertexPtr wire1,
                          VertexPtr wire2,
                          GraphPtr i_baseGraph,
                          std::string_view i_name);
  
    GraphVertexSequential(SequentialTypes i_type,
                          VertexPtr i_clk,
                          VertexPtr i_data,
                          VertexPtr wire1,
                          VertexPtr wire2,
                          VertexPtr wire3,
                          GraphPtr i_baseGraph,
                          std::string_view i_name);

  // clang-format on

  ~GraphVertexSequential() override{};

  /// @brief calculateHash
  /// Calculates the hash value of the vertex
  /// @param i_recalculate Flag indicating whether to i_recalculate the hash
  /// value (default false)
  /// @throws None.
  /// @code
  /// TO DO:
  /// @endcode
  /// @return The calculated hash value as a string

  size_t calculateHash(bool i_recalculate = false) override;

  /// @brief toVerilog
  /// generates a string in Verilog format for the current vertex,
  /// representing the valve according to its type and input connections.
  /// If a vertex has no input connections, an empty string is returned.
  /// If any input connection is invalid, an exception is thrown.
  /// @return A Verilog format string for the current vertex
  /// @throws std::invalid_argument if any input connection is invalid

  std::string toVerilog() const override;
  DotReturn toDOT() override;
  char updateValue() override { return '0'; };

  bool isFF() const;
  bool isAsync() const;
  bool isNegedge() const;
  SequentialTypes getSeqType() const;

  VertexPtr getClk() const;
  VertexPtr getData() const;
  VertexPtr getEn() const;
  VertexPtr getRst() const;
  VertexPtr getSet() const;

private:
  void setSignalByType(VertexPtr i_wire, SequentialTypes i_type,
                       unsigned &factType);
  void formatAlwaysBegin(std::string &verilog) const;

private:
  SequentialTypes d_seqType;

  VertexPtr d_clk;
  VertexPtr d_data;
  VertexPtr d_en;
  VertexPtr d_rst;
  VertexPtr d_set;
};

} // namespace CG_Graph