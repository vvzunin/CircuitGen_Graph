#pragma once

#include <string>
#include <vector>

#include <CircuitGenGraph/GraphVertexBase.hpp>

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
  /// @param i_baseGraph Pointer to the base graph.
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

  void setValue(const char value);

  virtual char updateValue() override;

  virtual void removeValue() override;

  /// @brief updateLevel
  /// This method updates the level of the vertex based on the levels of its
  /// input connections. It iterates through each input connection and sets
  /// the vertex level to the maximum level of its input connections plus one.
  /// If you are going to call this method for a second time, please, set
  /// all flags, used in updateLevel to their default state.

  virtual void updateLevel() override;

  /// @brief writes vertex to dot
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

/// class GraphVertexConstant
/// A simple class, making available to use constant values in circuits
class GraphVertexConstant : public GraphVertexInput {
public:
  /// @brief Initializes the GraphVertexConstant object with
  /// the provided base graph pointer and char symbol
  /// @param i_baseGraph Pointer to the base graph.
  /// @param i_const char symbol for constant (1, 0, z, x)
  GraphVertexConstant(char i_const, GraphPtr i_baseGraph);

  /// @brief Initializes the GraphVertexConstant object with
  /// the provided base graph pointer and char symbol
  /// @param i_name Name of the vertex.
  /// @param i_baseGraph Pointer to the base graph.
  /// @param i_const char symbol for constant (1, 0, z, x)
  GraphVertexConstant(char i_const, std::string_view i_name,
                      GraphPtr i_baseGraph);

  ~GraphVertexConstant() override{};

  /// @brief calculates hash for constant.
  /// Calculates the hash value for the vertex based on its outgoing
  /// connections.
  ///  When running for a second time, set hash flags to default state
  /// @return The hash value of the vertex based on its outgoing connections.
  /// @code
  /// // Creating an instance of the GraphVertexBase class
  /// GraphVertexBase vertex(VertexTypes::output, "vertex1");
  /// // Creating two more vertices
  /// VertexPtr vertex2 = std::make_shared<GraphVertexBase>(VertexTypes::input,
  /// "vertex2"); VertexPtr vertex3 =
  /// std::make_shared<GraphVertexBase>(VertexTypes::input, "vertex3");
  /// // Adding the second and third vertices to the output connections of the
  /// first vertex vertex.addVertexToOutConnections(vertex2);
  /// vertex.addVertexToOutConnections(vertex3);
  /// // Calculating the hash for the first vertex
  /// std::string hashValue = vertex.calculateHash();
  /// // Output of the result
  /// std::cout << "Hash for the first vertex: " << hashValue << std::endl;
  /// @endcode
  size_t calculateHash() override;

  /// @brief toVerilog
  /// Generates Verilog code for the constant vertex
  /// @return A string containing Verilog code for the vertex, or an empty
  /// string if the vertex type is not "output" or if the incoming connection
  /// is invalid
  /// @code
  /// // Creating an instance of the GraphVertexBase class with the type
  /// "output" and the name "output_vertex"
  /// GraphPtr graph = std::make_shared<OrientedGraph>();
  /// VertexPtr outputVertex = graph->addOutput("output");
  /// // Creating another vertex with the type "input" and the name
  /// VertexPtr inputVertex = graph->addInput("input_vertex");
  /// // Setting the input connection for the vertex "output_vertex"
  /// graph->addEdge(inputVertex, outputVertex);
  /// // Generating the Verilog code for the vertex "output_vertex"
  /// std::string verilogCode = outputVertex->toVerilog();
  /// // Display the generated Verilog code on the screen
  /// std::cout << "Generated Verilog code:\n" << verilogCode << std::endl;
  /// @endcode
  std::string toVerilog() const override;
  DotReturn toDOT() override;

  /// @brief getDefaultInstance
  /// Creates simple verilog const instance (as a wire)

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
  /// @brief Initializes the GraphVertexSubGraph object with
  /// the provided base graph pointer and subGraph pointer
  /// @param i_baseGraph Pointer to the base graph.
  /// @param i_subGraph Pointer to the subGraph.
  GraphVertexSubGraph(GraphPtr i_subGraph, GraphPtr i_baseGraph);

  GraphVertexSubGraph(GraphPtr i_subGraph, std::string_view i_name,
                      GraphPtr i_baseGraph);

  ~GraphVertexSubGraph() override{};

  /// @brief updateValue A virtual function for updating the vertex value.
  /// The implementation is provided in derived classes
  /// @return the value of the vertex after its update
  char updateValue() override;

  void removeValue() override;

  /// @brief updateLevel
  /// This method updates the level of the vertex based on the levels of its
  /// input connections. It iterates through each input connection and sets
  /// the vertex level to the maximum level of its input connections plus one.
  /// If you are going to call this method for a second time, please, set
  /// all flags, used in updateLevel to their default state.
  void updateLevel() override;

  /// @brief toVerilog
  /// Generates Verilog code for the vertex
  /// @return A string containing Verilog code for the vertex, or an empty
  /// string if the vertex type is not "output" or if the incoming connection
  /// is invalid
  /// @code
  /// // Creating an instance of the GraphVertexBase class with the type
  /// "output" and the name "output_vertex"
  /// GraphPtr graph = std::make_shared<OrientedGraph>();
  /// GraphPtr subgraph = std::make_shared<OrientedGraph>();
  /// subgraph->addEdge(
  ///   subgraph->addInput("in"),
  ///   subgraph->addOutput("out"));
  /// VertexPtr outputVertex = graph->addOutput("output");
  /// // Creating another vertex with the type "input" and the name
  /// VertexPtr inputVertex = graph->addInput("input_vertex");
  /// // Creating another vertex with the type "subGraph" and the name
  /// VertexPtr subgraphOut = graph->addSubGraph(subgraph, inputVertex).back();
  /// // Setting the input connection for the vertex "output_vertex"
  /// graph->addEdge(inputVertex, subgraphOut);
  /// graph->addEdge(subgraphOut, outputVertex);
  /// // Generating the Verilog code for the vertex subgraph
  /// VertexPtr subgaphItself = getVerticesByLevel(1u).back();
  /// std::cout << subgaphItself << '\n';
  /// @endcode
  std::string toVerilog() const override;
  DotReturn toDOT() override;

  /// @brief This method is used as a substructure for
  /// OrientedGraph methods
  /// @param i_path path to future file storing. Do not add submodule here,
  /// folder would be created there
  /// @param i_filename name of file to be created (default is same as graph
  /// name)
  /// @return bool, meaning was file writing successful or not
  bool toVerilog(std::string i_path, std::string i_filename = "");

  bool toDOT(std::string i_path, std::string i_filename = "");

  /// @brief This method is used as a substructure for
  /// OrientedGraph methods
  bool toGraphML(std::ofstream &i_fileStream) const;

  /// @brief This method is used as a substructureS for
  /// OrientedGraph methods
  /// @param i_indent
  /// @return
  std::string toGraphML(uint16_t i_indent = 0, std::string i_prefix = "") const;

  /// @brief This method is used as a substructure for
  /// OrientedGraph methods. When running it for a second time, clear hash flags
  /// @return

  size_t calculateHash() override;

  /// @brief sets new subgraph to the vertex
  void setSubGraph(GraphPtr i_subGraph);
  /// @return pointer to subgraph, being stored in vertex
  GraphPtr getSubGraph() const;
  /// @brief returns all vertices of outputs, which are
  /// influenced by given input to vertex
  std::vector<VertexPtr>
  getOutputBuffersByOuterInput(VertexPtr i_outerInput) const;
  /// @brief returns all vertices of inputs, which are
  /// influenced by given output to vertex
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

  char updateValue() override;

  /// @brief updateLevel updates the level of the current vertex in the graph
  /// based on the levels of its incoming connections. If a vertex has incoming
  /// connections, the method determines the highest level among all the
  /// vertices to which it is connected, and sets the level of the current
  /// vertex to one higher than the highest level

  void updateLevel() override;

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

  void removeValue() override;

  /// @brief calculateHash
  /// Calculates the hash value of the vertex. When running for a second time,
  /// set hash flags to default state
  /// @return The calculated hash value as a string

  size_t calculateHash() override;

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

  Gates getGate() const override;

  /// @brief setGateIfDefault
  /// If type of current gate is gateDefault, method change it
  /// on provided one. In other cases nothing happens.
  /// @code
  /// GraphVertexGates vertex(Gates::GateDefault);
  /// vertex.setGateIfDefault(GateAnd);
  /// Gates gateType = vertex.getGate();
  /// std:: cout << "Gate type : " << gateType << std::endl;
  /// @endcode

  void setGateIfDefault(Gates type);

  /// @brief  addVertexToInConnections
  /// Buffer and Not types of gates must have only one element in
  /// d_inConnections, so realization for GraphVertexGates has a
  /// check before adding
  /// @param i_vert Vertex that will be added to d_inConnections of this
  /// @return The count of occurrences of the given vertex in the input
  /// connections after adding it
  /// @throws std::overflow_error in case of connecting more than one
  /// vertex in d_inConnections

  uint32_t addVertexToInConnections(VertexPtr i_vert) override;

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

  friend class GraphVertexSubGraph;
};

class GraphVertexSequential : public GraphVertexBase {
public:
  // clang-format off

  /// @brief GraphVertexSequential Constructor for default types
  /// @param i_type type of sequential vertex (can be only (n)ff or latch = EN)
  /// @param i_clk is clock signal for a ff and enable signal for a latch
  /// @param i_data 
  /// @param i_baseGraph 
  /// @param i_name 
  GraphVertexSequential(SequentialTypes i_type,
                        VertexPtr i_clk,
                        VertexPtr i_data,
                        GraphPtr i_baseGraph,
                        std::string_view i_name);

  /// @brief 
  /// @param i_type 
  /// @param i_clk is clock signal for a ff and enable signal for a latch
  /// @param i_data 
  /// @param i_wire RST or CLR or SET or EN
  /// @param i_baseGraph 
  /// @param i_name 
  GraphVertexSequential(SequentialTypes i_type,
                        VertexPtr i_clk,
                        VertexPtr i_data,
                        VertexPtr i_wire,
                        GraphPtr i_baseGraph,
                        std::string_view i_name);

  /// @brief GraphVertexSequential
  /// @param i_type
  /// @param i_clk EN for latch and CLK for ff
  /// @param i_data
  /// @param i_wire1 RST or CLR or SET
  /// @param i_wire2 SET or EN
  /// @param i_baseGraph
  GraphVertexSequential(SequentialTypes i_type,
                        VertexPtr i_clk,
                        VertexPtr i_data,
                        VertexPtr i_wire1,
                        VertexPtr i_wire2,
                        GraphPtr i_baseGraph,
                        std::string_view i_name);

  /// @brief GraphVertexSequential
  /// @param i_type type of Sequential - (a/n/an)ff(r/c)se, 
  /// @param i_clk clock for flip=flop
  /// @param i_data data value
  /// @param i_rst clear (or reset signal)
  /// @param i_set set signal
  /// @param i_en enable
  /// @param i_baseGraph
  GraphVertexSequential(SequentialTypes i_type,
                        VertexPtr i_clk,
                        VertexPtr i_data,
                        VertexPtr i_rst,
                        VertexPtr i_set,
                        VertexPtr i_en,
                        GraphPtr i_baseGraph,
                        std::string_view i_name);

  // clang-format on

  ~GraphVertexSequential() override{};

  /// @brief calculateHash
  /// Calculates the hash value of the vertex. When running for a second time,
  /// set hash flags to default state
  /// @throws None.
  /// @code
  /// TO DO:
  /// @endcode
  /// @return The calculated hash value as a string

  size_t calculateHash() override;

  /// @brief toVerilog
  /// generates a string in Verilog format for the current vertex,
  /// representing the valve according to its type and input connections.
  /// If a vertex has no input connections, an empty string is returned.
  /// If any input connection is invalid, an exception is thrown.
  /// @return A Verilog format string for the current vertex
  /// @throws std::invalid_argument if any input connection is invalid

  std::string toVerilog() const override;
  DotReturn toDOT() override;

  /// @brief updateValue A virtual function for updating the vertex value.
  /// The implementation is provided in derived classes
  /// @return the value of the vertex after its update
  char updateValue() override { return ValueStates::FalseValue; };

  /// @brief return true if sequential cell is
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

  VertexPtr d_clk = nullptr;
  VertexPtr d_data = nullptr;
  VertexPtr d_en = nullptr;
  VertexPtr d_rst = nullptr;
  VertexPtr d_set = nullptr;
};

} // namespace CG_Graph
