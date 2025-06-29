#pragma once

#include <array>
#include <atomic>
#include <ctime>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include <CircuitGenGraph/enums.hpp>

#include <CircuitGenGraph/GraphMemory.hpp>
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/GraphUtils.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

/// TODO: Добавить проверку на имена файлов при доблении новых вершин

#define GraphPtr std::shared_ptr<CG_Graph::OrientedGraph>
#define GraphPtrWeak std::weak_ptr<CG_Graph::OrientedGraph>

#define VertexPtr CG_Graph::GraphVertexBase *

namespace CG_Graph {

class GraphVertexBase;
class GraphReader;
class Context;

/// \class OrientedGraph
/// @param d_countNewGraphInstance Static variable to count new graph
/// instances
/// @param d_countGraph Current instance counter. Is used for setting a
/// graphID
/// @param d_currentInstance Current instance count
/// @param d_currentParentGraph Weak pointer to the current parent graph
/// @param d_edgesCount The number of edges in the graph
/// @param d_hashed Hashed value of the graph
/// @param d_parentGraphs Vector of weak pointers to parent graphs
/// @param d_name The name of the graph
/// @param d_needLevelUpdate A flag indicating whether the vertex levels in
/// the graph need to be updated
/// @param d_graphInstanceToVerilogCount Map to count instances to Verilog
/// This map is used to count how many times each subgraph instance has been
/// converted to Verilog. The key represents the instance number of the
/// subgraph, and the value represents the count of how many times it has been
/// converted to Verilog
/// @param d_graphInstanceToDotCount Map to count instances to DOT
/// This map is used to count how many times each subgraph instance has been
/// converted to DOT. The key represents the instance number of the
/// subgraph, and the value represents the count of how many times it has been
/// converted to DOT
/// @param d_allSubGraphsOutputs Vector storing all outputs of subgraphs
/// @param d_subGraphs Set of subgraphs
/// This set stores the subgraphs present in the graph. It is used to keep
/// track of all the subgraphs associated with the current graph instance
/// @param d_vertices Map of vertex types to vectors of vertex pointers
/// @param d_countGraph Static counter for the total number of graphs
/// @param d_gatesCount Map for quick gates count
/// This map is used for quick counting of gates in the graph. It maps each
/// gate type (Gates) to the count of gates of that type present in the graph.
/// @param d_edgesGatesCount Map for quick count of edges of gate type
/// This map is used for quick counting of edges between different types of
/// gates in the graph. It maps each gate type (Gates) to another map, where
/// the inner map maps gate types to the count of edges between them.
// clang-format off
class OrientedGraph
    : public GraphMemory
    , public std::enable_shared_from_this<OrientedGraph>
#ifdef LOGFLAG
    , public el::Loggable
#endif
{
  // clang-format on
public:
  OrientedGraph(const std::string &i_name = "",
                size_t buffer_size = DEFAULT_BUF,
                size_t chunk_size = CHUNK_SIZE);

  using GraphID = std::size_t;

  // TODO: Добавить использование gates_inputs_info.

  ~OrientedGraph();

  OrientedGraph &operator=(const OrientedGraph &other) = delete;
  OrientedGraph &operator=(OrientedGraph &&other) = delete;
  OrientedGraph(const OrientedGraph &other) = delete;
  OrientedGraph(OrientedGraph &&other) = delete;

  // Количество gate в графе, за исключением подграфов
  /// @brief baseSize returns the number of "gate" type vertices in the graph
  /// @return An integer value representing the number of "gate" type vertices
  /// in the graph

  size_t baseSize() const;
  // Количество gate в графе, включая подграфы
  /// @brief fullSize returns the total number of vertices in the graph,
  /// including vertices from all subgraphs. It recursively traverses all
  /// the subgraphs and sums up the number of vertices in each of them
  /// @return An integer value representing the total number of vertices
  /// in the graph, including vertices from all subgraphs

  size_t fullSize() const;
  // sum of gates, inputs, outputs and consts sizes
  /// @brief sumFullSize returns the total number of vertices of all types
  /// in the graph, including input vertices, constants, gates, and output
  /// vertices
  /// @return the size of all vertices in the graph, summing the number of
  /// vertices of each type
  size_t sumFullSize() const;

  // Имеются ли gate в схеме, включая подграфы
  /// @brief Checks, if there are any gates in graph (including subrapgs).
  /// In fact checks, if fullSize is equal to zero
  bool isEmpty() const;

  /// @brief Resets all internal states for all types of vertices
  void clearAllStates();
  /// @brief Resets hash states for all vertices of current graph
  /// (including subgraphs). Should be called before each hash recalculation
  void clearHashStates();
  /// @brief Resets update for all vertices of current graph
  /// (including subgraphs). Should be called before each level recalculation
  void clearNeedUpdateStates();
  /// @brief Sets for all vertices state, that shows, that all vertices
  /// were not used for for `getVerticesByLevel`. Is called by
  /// `getVerticesByLevel`.
  void clearUsedLevelStates();

  /// @brief isEmptyFull It is used to check the emptiness of a graph,
  /// including all its subgraphs. It recursively traverses all the subgraphs
  /// and checks if they are empty
  /// @return bool - true if the graph and all its subgraphs are empty, and
  /// false if at least one of them contains vertice
  bool isEmptyFull() const;

  /// @brief setName Used to set the name of the graph. It takes a string as
  /// an argument and sets the d_name field to this string
  /// @param i_name the new name of the graph

  void setName(const std::string &i_name);

  /// @brief getName Used to get the name of the graph
  /// @return the name of the graph

  std::string getName() const;

  /// @brief needToUpdateLevel it is used to check whether the vertex levels
  /// in the graph need to be updated
  /// @return bool - true if updating the vertex levels in the graph is
  /// required, and false if not required

  bool needToUpdateLevel() const;

  /// @brief updateLevels Calculates level for all vertices inside graph.
  /// Level show, how far is vertex from an input, Inputs has level 0,
  /// vertices, which inConnections contains inputs only - 1 level, etc.
  /// Outputs (1 or more) always has max possible level, but
  /// not **all** outputs have max possible level.

  void updateLevels();

  /// @brief getMaxLevel
  /// Calculates and returns the maximum level of the output vertices in the
  /// graph
  /// @return maximum level of the output vertices
  /// @code
  ///  OrientedGraph graph("ExampleGraph");
  /// // Add vertices and edges to the graph (omitted for brevity)
  /// // Calculate the maximum level of output vertices
  /// uint32_t maxLevel = graph.getMaxLevel();
  /// // Output the result
  /// std::cout << "Maximum level of output vertices: " << maxLevel << '\n';
  /// @endcode

  uint32_t getMaxLevel();

  /// @brief setCurrentParent
  /// Sets the current parent graph of the current graph
  /// @param parent A shared pointer to the parent graph to be set as the
  /// current parent
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Create another instance of OrientedGraph to be set as the current
  /// parent auto parentGraph = std::make_shared<OrientedGraph>("ParentGraph");
  /// // Set parentGraph as the current parent of graph
  /// graph->setCurrentParent(parentGraph);
  /// // Now parentGraph is set as the current parent of graph
  /// @endcode
  void setCurrentParent(GraphPtr parent);

  /// @brief resetCounters
  /// Resets counters associated with a specific graph instance
  /// @param where A shared pointer to the subgraph whose counters need to be
  /// reset
  void resetCounters(GraphPtr where);

  /// @brief addInput
  /// Adds an input vertex to the current graph
  /// @param i_name The name of the input vertex to be added
  /// @return A shared pointer to the newly created input vertex
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Add an input vertex to the graph
  /// auto inputVertex = graph->addInput("Input1");
  /// // Now inputVertex is a shared pointer to the newly added input vertex
  /// @endcode

  VertexPtr addInput(const std::string &i_name = "");

  /// @brief addOutput
  /// Adds an output vertex to the current graph
  /// @param i_name The name of the output vertex to be added
  /// @return A shared pointer to the newly created output vertex
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Add an output vertex to the graph
  /// auto outputVertex = graph->addOutput("Output1");
  /// // Now outputVertex is a shared pointer to the newly added output vertex
  /// @endcode

  VertexPtr addOutput(const std::string &i_name = "");

  /// @brief addConst
  /// Adds a constant vertex to the current graph
  /// @param i_value The value of the constant vertex to be added
  /// @param i_name The name of the constant vertex to be added
  /// @return A shared pointer to the newly created constant vertex
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Add a constant vertex to the graph with value 'A' and name "Constant1"
  /// auto constantVertex = graph->addConst('A', "Constant1");
  /// // Now constantVertex is a shared pointer to the newly added constant
  /// vertex
  /// @endcode

  VertexPtr addConst(const char &i_value, const std::string &i_name = "");

  /// @brief addGate
  /// Adds a gate vertex to the current graph
  /// @param i_gate The type of the gate vertex to be added
  /// @param i_name The name of the gate vertex to be added
  /// @return A shared pointer to the newly created gate vertex
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Add a gate vertex to the graph with type GateAnd and name "Gate1"
  /// auto gateVertex = graph->addGate(Gates::GateAnd, "Gate1");
  /// // Now gateVertex is a shared pointer to the newly added gate vertex
  /// @endcode

  VertexPtr addGate(const Gates &i_gate, const std::string &i_name = "");

  /// @brief addSequential Adds a sequential vertex to the current graph.
  /// @param i_type The type of the gsequential to be added;
  /// can be flip-flop (ff) or latch only
  /// @param i_clk Vertex, that is used as clock (or enable for latch)
  /// @param i_data Data vertex, should be written to a reg
  /// @param i_name The name of the gate vertex to be added
  /// @return A shared pointer to the newly created gate vertex
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// auto *clk = graph->addInput("clk");
  /// auto *data = graph->addInput("data");
  /// // Creates a simple d flip-flop
  /// auto *seq = graph->addSequential(ff, clk, data, "q");
  /// @endcode
  VertexPtr addSequential(const SequentialTypes &i_type, VertexPtr i_clk,
                          VertexPtr i_data, const std::string &i_name = "");

  /// @brief addSequential Adds a sequential vertex to the current graph.
  /// @param i_type The type of the gsequential to be added;
  /// can be any type, that need one additional signal.
  /// @param i_clk Vertex, that is used as clock (or enable for latch)
  /// EN for latch and CLK for FF
  /// @param i_data Data vertex, should be written to a reg
  /// @param i_wire RST or CLR or SET or EN (enable only if flip-flop)
  /// @param i_name The name of the gate vertex to be added
  /// @return A shared pointer to the newly created gate vertex
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// auto *clk = graph->addInput("clk");
  /// auto *data = graph->addInput("data");
  /// auto *rst_n = graph->addInput("rst_n");
  /// // Creates a simple d flip-flop with async reset
  /// auto *seq = graph->addSequential(affr, clk, data, rst_n, "q");
  /// @endcode
  VertexPtr addSequential(const SequentialTypes &i_type, VertexPtr i_clk,
                          VertexPtr i_data, VertexPtr i_wire,
                          const std::string &i_name = "");

  /// @brief addSequential Adds a sequential vertex to the current graph.
  /// @param i_type The type of the gsequential to be added;
  /// can be flip-flop (ff) or latch only
  /// @param i_clk Vertex, that is used as clock (or enable for latch)
  /// @param i_data Data vertex, should be written to a reg
  /// @param i_wire1 RST or CLR or SET
  /// @param i_wire2 SET (double set is not allowed, for sure)
  /// or EN (en for flip-flop only)
  /// @param i_name The name of the gate vertex to be added
  /// @return A shared pointer to the newly created gate vertex
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// auto *clk = graph->addInput("clk");
  /// auto *data = graph->addInput("data");
  /// auto *clr = graph->addInput("clr");
  /// auto *set = graph->addInput("set");
  /// // Creates a latch with clr and set signals
  /// auto *seq = graph->addSequential(latchcs, clk, data, clr, set, "q");
  /// @endcode
  VertexPtr addSequential(const SequentialTypes &i_type, VertexPtr i_clk,
                          VertexPtr i_data, VertexPtr i_wire1,
                          VertexPtr i_wire2, const std::string &i_name = "");

  /// @brief addSequential Adds a sequential vertex to the current graph.
  /// Use with FF only!
  /// @param i_clk CLK signal
  /// @param i_data what to write, D
  /// @param i_rst RST (or CLR) signal
  /// @param i_set SET signal
  /// @param i_en EN signal
  /// @param i_name The name of the gate vertex to be added
  /// @return A shared pointer to the newly created gate vertex
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// auto *clk = graph->addInput("clk");
  /// auto *data = graph->addInput("data");
  /// auto *rst = graph->addInput("rst");
  /// auto *set = graph->addInput("set");
  /// auto *en = graph->addInput("en");
  /// // Flip-flop with negedge clk, sync reset signal, set and enable signals
  /// auto *seq = graph->addSequential(nffrse, clk, data, rst, set, en, "q");
  /// @endcode
  VertexPtr addSequential(const SequentialTypes &i_type, VertexPtr i_clk,
                          VertexPtr i_data, VertexPtr i_rst, VertexPtr i_set,
                          VertexPtr i_en, const std::string &i_name = "");

  /// @brief addSubGraph
  /// Adds a subgraph to the current graph
  /// @param i_subGraph A shared pointer to the subgraph to be added
  /// @param i_inputs A vector containing the input vertices to be connected
  /// to the subgraph
  /// @return A vector containing the output vertices of the subgraph
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Create a subgraph
  /// auto subGraph = std::make_shared<OrientedGraph>("SubGraph");
  /// // Create input vertices for the subgraph
  /// std::vector<VertexPtr> subGraphInputs;
  /// for (size_t i = 0; i < 3; ++i)
  /// {
  ///   auto inputVertex = graph->addInput("SubGraphInput" +
  ///   std::to_string(i+1)); subGraphInputs.push_back(inputVertex);
  /// }
  /// // Add the subgraph to the graph with its inputs
  /// auto subGraphOutputs = graph->addSubGraph(subGraph, subGraphInputs);
  /// // Now subGraphOutputs contains the output vertices of the subgraph
  /// @endcode
  /// @throws std::invalid_argument if the number of inputs does not match
  /// the number of input vertices in the subgraph

  std::vector<VertexPtr> addSubGraph(GraphPtr i_subGraph,
                                     std::vector<VertexPtr> i_inputs);

  /// @brief removeWasteVertices()
  /// Some generated graphs have vertices which have not any path
  /// to outputs of scheme. The method deletes these ones.
  /// Firstly, it is removing all inner (gates, sequental, subgraph)
  /// vertices which d_level == 0 (good criterion because of
  /// OrientedGraph::updateLevel() algorithm specificity)
  /// and all edges from normal vertices to current wrong.
  /// After removing of inner ones, some inputs or constants
  /// can have not any element in d_outConnections(), these
  /// also will be removed.
  std::vector<char> graphSimulation(std::vector<char> values);

  void simulationRemove();

  void removeWasteVertices();

  void updateEdgesGatesCount(VertexPtr vertex, Gates type);
  /// @brief addEdge
  /// Adds an edge between two vertices in the current graph
  /// @param from A shared pointer to the vertex where the edge originates
  /// @param to A shared pointer to the vertex where the edge terminates
  /// @return true if the edge was successfully added, false otherwise
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Create two vertices
  /// auto vertex1 = graph->addInput("Vertex1");
  /// auto vertex2 = graph->addOutput("Vertex2");
  /// // Add an edge from vertex1 to vertex2
  /// bool edgeAdded = graph->addEdge(vertex1, vertex2);
  /// if (edgeAdded)
  /// {
  ///   std::cout << "Edge successfully added between Vertex1 and Vertex2\n";
  /// }
  /// else
  /// {
  ///   LOG(ERROR) << "Failed to add edge between Vertex1 and Vertex2\n";
  /// }
  /// @endcode
  /// @throws std::invalid_argument if attempting to add an edge from one
  /// subgraph to another when the 'from' vertex is not an output, or when
  /// attempting to add an edge to another subgraph when the 'to' vertex is
  /// not an input

  bool addEdge(VertexPtr from, VertexPtr to);

  /// @brief addEdges
  /// Adds multiple edges from a vector of source vertices to a target vertex
  /// @param from1 A vector containing the source vertices from which edges
  /// will originate
  /// @param to A shared pointer to the target vertex where the edges will
  /// terminate
  /// @return true if all edges were successfully added, false if at least one
  /// edge failed to be added
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Create multiple source vertices
  /// std::vector<VertexPtr> sources;
  /// for (size_t i = 0; i < 3; ++i)
  /// {
  ///   auto vertex = graph->addInput("Source" + std::to_string(i+1));
  ///   sources.push_back(vertex);
  /// }
  /// // Create a target vertex
  /// auto target = graph->addOutput("Target");
  /// // Add edges from all source vertices to the target vertex
  /// bool edgesAdded = graph->addEdges(sources, target);
  /// if (edgesAdded)
  /// {
  ///   std::cout << "Edges successfully added from all sources to the
  ///   target\n";
  /// }
  /// else
  /// {
  ///   LOG(ERROR) << "Failed to add edges from one or more sources to the
  ///   target\n";
  /// }
  /// @endcode

  bool addEdges(std::vector<VertexPtr> from1, VertexPtr to);

  /// @brief removeEdge
  /// Remove an edge from graph if it exists.
  /// @param from1 The vertex to be deleted to the input connections of "to"
  /// @param to The vertex to be deleted to the output connections of "from"
  /// @return True if edge was found and deleted, false otherwise
  /// @code
  /// // Creating an instance of the OrientedGraph and two vertices
  /// GraphPtr graphPtr = std::make_shared<OrientedGraph>("Graph");
  /// VertexPtr v1 = graphPtr->addInput();
  /// VertexPtr v2 = graphPtr->addGate(GateBuf);
  /// // Creating an edge
  /// graphPtr->addEdge(v1, v2);
  /// // remove the edge and check of result
  /// bool deleted = graphPtr->removeEdge(v1, v2);
  /// if (deleted) cout << "Edge removed successfully";
  /// else cout << "Edge from v1 to v2 is not exist";
  /// @endcode

  bool removeEdge(VertexPtr from1, VertexPtr to);

  /// @brief getEdgesCount
  /// Retrieves the total number of edges in the graph
  /// @return The total number of edges in the graph
  size_t getEdgesCount() { return d_edgesCount; }

  /// @brief Returns set af all subGraphs (graphs, which instances has current
  /// graph)
  /// @return set of subGrpahs
  std::set<GraphPtr> getSubGraphs() const;

  /// @brief returns all vertices (as an array of vectors of pointers to the
  /// base class)
  /// @return d_vertices field
  std::array<std::vector<VertexPtr>, VertexTypes::output + 1>
  getBaseVertexes() const;

  /// @brief getVerticeByIndex returns a vertex from graph. Index should be
  /// smaller, that number of all vertices inside current graph. Index firstly
  /// is used for inputs, than - constants, than - gates, sequential, subGraphs
  /// and than - outputs.
  /// @throw out_of_range if idx is bigger than number of all vertices in graph
  VertexPtr getVerticeByIndex(size_t idx) const;

  static void readVerilog(std::string i_path, Context &context);
  static CG_Graph::Context readVerilog(std::string i_path);
  /// @brief method used for translating graph to verilog
  /// @param i_path folder, in which file should be created
  /// @param i_filename name of a file, which should be created
  /// @return flag, if file was correctly vreated or not
  bool toVerilog(std::string i_path, std::string i_filename = "");

  /// @brief
  /// @return
  DotReturn toDOT();

  /// @brief
  /// @param i_path
  /// @param i_filename
  /// @return
  bool toDOT(std::string i_path, std::string i_filename = "");

  /// @brief toGraphML Writes the graph structure in GraphML format to the
  /// specified output stream
  /// @param i_fileStream A reference to the std::ofstream object, which
  /// represents the file in which the graph structure will be written in
  /// GraphML format
  /// @return bool: Returns true if the graph structure has been successfully
  /// written in GraphML format, and false otherwise. In this case, it always
  /// returns true.
  bool toGraphMLClassic(std::ofstream &i_fileStream);

  /// @brief toGraphMLPseudoABCD Writes the graph structure in GraphML format to
  /// the specified output stream
  /// @param i_fileStream A reference to the std::ofstream object, which
  /// represents the file in which the graph structure will be written in
  /// GraphML format
  /// @return bool: Returns true if the graph structure has been successfully
  /// written in GraphML format, and false otherwise. In this case, it always
  /// returns true.
  bool toGraphMLPseudoABCD(std::ofstream &i_fileStream);

  /// @brief toGraphMLOpenABCD Writes the graph structure in GraphML format to
  /// the specified output stream
  /// @param i_fileStream A reference to the std::ofstream object, which
  /// represents the file in which the graph structure will be written in
  /// GraphML format
  /// @return bool: Returns true if the graph structure has been successfully
  /// written in GraphML format, and false otherwise. In this case, it always
  /// returns true.
  bool toGraphMLOpenABCD(std::ofstream &i_fileStream);

  /// TODO: add description
  void parseVertexToGraphML(const VertexTypes &vertexType,
                            const std::vector<VertexPtr> &vertexVector,
                            const std::string &nodeTemplate,
                            const std::string &edgeTemplate,
                            const std::string &i_prefix, std::string &nodes,
                            std::string &edges);

  /// @brief Is called by toGraphMLClassic. TODO: add description
  std::string toGraphMLClassic(uint16_t i_indent = 0,
                               const std::string &i_prefix = "");

  /// @brief Is called by toGraphMLPseudoABCD. TODO: add description
  std::string toGraphMLPseudoABCD();

  /// @brief Is called by toGraphMLOpenABCD. TODO: add description
  std::string toGraphMLOpenABCD();

  /// @brief used for looking for a vector of all vertices with required type
  /// @param i_type
  /// @param i_name
  /// @param i_addSubGraphs
  /// @return
  std::vector<VertexPtr>
  getVerticesByType(const VertexTypes &i_type, std::string_view i_name = "",
                    const bool &i_addSubGraphs = false) const;

  /// @brief gets all vertices with requires level
  std::vector<VertexPtr> getVerticesByLevel(uint32_t i_level);

  /// @brief Looks for all vertices with given name in graph
  /// and subGrpahs (if required)
  /// @param i_name name, which should have vertices
  /// @param i_addSubGraphs if true, looks inside subGraphs for vertices.
  /// Is false by default.
  /// @return vector with all found vertices.
  std::vector<VertexPtr>
  getVerticesByName(std::string_view i_name,
                    const bool i_addSubGraphs = false) const;

  /// @brief Call calculateHash before this check!!!!
  /// @param rhs another value to be compared
  /// @return returns true, if hashes are equal
  bool operator==(const OrientedGraph &rhs);

  /// @brief calculateHash calculates hash values for a graph based on the hash
  /// values of its vertices
  /// When running for a second time, set hash flags to default state
  /// @return A string representing the hash value of the graph

  std::string calculateHash();

  // @brief getGatesCount Returns a display containing the number of each gate
  /// type in the graph
  /// @return A display where each key is a type of gate (Gates), and the
  /// corresponding value is the number of gates of this type in the graph

  std::map<Gates, size_t> getGatesCount() const;

  /// @brief getEdgesGatesCount Returns a mapping containing the number of
  /// edges between different types of gates in the graph
  /// @return A mapping where each external key is a gate type, and the
  /// corresponding value is an internal mapping containing the number of
  /// edges between different types of gates in the graph

  std::map<Gates, std::map<Gates, size_t>> getEdgesGatesCount() const;

  /// @brief reserve additional place in vector for given number of VertexPtr,
  /// where are located vertices of given type.
  /// @param i_type Type, for which place should be reserved
  /// @param i_capacity Number of vertices, which would be added later
  void reserve(VertexTypes i_type, size_t i_capacity);

  /// @brief resets counter for graph IDs
  static void resetCounter() { d_countGraph = 0ul; }

  /// @brief Checks graph connectivity
  /// @return bool, true if the graph is connected, and false if not.
  bool isConnected(bool i_recalculate = false);

  /// @brief A simple counter for subGrpah instances to give
  /// them unique names in Verilog
  /// @param i_id GraphID of subGraph, instance of which is being created.
  /// @return new index for new unique name.
  std::uint64_t getGraphInstVerilog(GraphID i_id) {
    return d_graphInstanceToVerilogCount[i_id]++;
  }

  /// @brief A simple counter for subGrpah instances to give
  /// them unique names in DOT format
  /// @param i_id GraphID of subGraph, instance of which is being created.
  /// @return new index for new unique name.
  std::uint64_t getGraphInstDOT(GraphID i_id) {
    return d_graphInstanceToDotCount[i_id]++;
  }

  /// @brief getter for unique graph ID
  /// @return id of graph (size_t)
  GraphID getID() { return d_graphID; }

  /// @brief Unrolls graph. TODO: add normal description
  GraphPtr unrollGraph();

  /// @brief creates a majority element, represented by a graph.
  /// @return GraphPtr to created graph
  static GraphPtr createMajoritySubgraph();
  /// @brief creates majority element inside current graph
  VertexPtr generateMajority(VertexPtr a, VertexPtr b, VertexPtr c);

  /// @brief log Used for easylogging++
  /// @param os Stream for easylogging
#ifdef LOGFLAG
  virtual void log(el::base::type::ostream_t &os) const;
#endif

protected:
  /* memory management block */

  /// @author Fuuulkrum7
  /// @brief Allocates memory and creates an instance of required type. Is used
  /// for creating ALL vertices of any type.
  /// @tparam T
  /// @tparam ...Args
  /// @param ...args
  /// @return
  template<typename T, typename... Args>
  T *create(Args &&...args) {
    return new (allocate<T>()) T(std::forward<Args>(args)...);
  }

  void dfs(VertexPtr i_startVertex, std::unordered_set<VertexPtr> &i_visited,
           std::unordered_set<VertexPtr> &i_dsg);

private:
  static std::atomic_size_t d_countNewGraphInstance;
  static std::atomic_size_t d_countGraph;

private:
  enum HASH_STATE : uint8_t {
    HC_NOT_CALC = 0,
    HC_IN_PROGRESS = 1,
    HC_CALC = 2
  };

  // used for quick gates count
  std::map<Gates, size_t> d_gatesCount = {
      {Gates::GateAnd, 0}, {Gates::GateNand, 0}, {Gates::GateOr, 0},
      {Gates::GateNor, 0}, {Gates::GateNot, 0},  {Gates::GateBuf, 0},
      {Gates::GateXor, 0}, {Gates::GateXnor, 0}};
  // used for quick edges of gate type count;
  std::map<Gates, std::map<Gates, size_t>> d_edgesGatesCount;

  // We can add a subgraph multiple times
  // so we need to count instances to verilog.
  // We are counting to know, which inputs and outputs should we use now
  std::map<GraphID, uint64_t> d_graphInstanceToVerilogCount;
  std::map<GraphID, uint64_t> d_graphInstanceToDotCount;

  // each subgraph has one or more outputs. We save them,
  // depending on subgraph instance number
  std::vector<VertexPtr> d_allSubGraphsOutputs;

  std::set<GraphPtr> d_subGraphs;
  std::array<std::vector<VertexPtr>, VertexTypes::output + 1> d_vertices;

  // as we can have multiple parents, we save
  // for toVerilog current parent graph
  GraphPtrWeak d_currentParentGraph;

  std::string d_name;

  GraphID d_graphID;
  size_t d_edgesCount = 0;

  size_t d_hashed = 0;
  HASH_STATE d_hashState = HC_NOT_CALC;

  bool d_isSubGraph = false;
  // Пока не реализован функционал.
  bool d_needLevelUpdate = true;

  // also we need to now, was .v file for subgraph created, or not
  bool d_alreadyParsedVerilog = false;
  bool d_alreadyParsedDot = false;

  // -1 if false, 0 if undefined, 1 if true
  int8_t d_connected = 0;
  static GraphReader *graphReader;
};

} // namespace CG_Graph
