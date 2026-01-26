#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

/// @file GraphUtils.hpp
/// @brief Utility functions, enumerations and templates for graph operations
/// @ingroup GraphCore
/// 
/// This header contains:
/// - Graph-related enumerations (VertexTypes, Gates, SequentialTypes, etc.)
/// - String conversion utilities
/// - Template functions for pair lookup
/// - Logic operation mappings

namespace CG_Graph {

#ifndef DotReturn
/// @def DotReturn
/// @brief Type alias for DOT format return structure
/// @details Vector of pairs where each pair contains a DotType and associated properties
#define DotReturn \
  std::vector<std::pair<DotTypes, std::map<std::string, std::string>>>
#endif

/// @brief Enumeration of all possible vertex types in a graph
/// @enum VertexTypes
enum VertexTypes : uint8_t {
  input = 0,     ///< Input vertex
  output = 6,    ///< Output vertex
  constant = 1,  ///< Constant vertex
  gate = 2,      ///< Vertex representing a logical element
  subGraph = 3,  ///< Subgraph that makes up the vertex
  dataBus = 4,   ///< Bus vertex type. Not supported yet
  sequential = 5 ///< Sequential vertex type (d-latch or d-flip-flop)
};

/// @def CGG_FF_TYPE
/// @brief Macro for defining flip-flop type pairs (normal and negedge versions)
/// @param S Suffix for the flip-flop type name
/// @param V Base value combining control signals
/// @details Generates both standard and negedge-clocked versions of a flip-flop type
/// @note CGG stands for CircuitGenGraph
#define CGG_FF_TYPE(S, V) S = V, n##S = NEGEDGE | V

/// @brief Types of all sequential cells being supported
/// @enum SequentialTypes
enum SequentialTypes : uint8_t {
  /// Enable signal, writes data to output if is equal to 1'b1
  EN = 1 << 0,
  /// Set signal, writes 1'b1 to output if is equal to 1'b1
  SET = 1 << 1,
  /// Clear signal, writes 1'b0 to output if is equal to 1'b1
  CLR = 1 << 2,
  /// Reset signal, writes 1'b0 to output if is equal to 1'b0
  RST = 1 << 3,
  /// Use with reset only, makes ff async (adds negedge rst to signals list)
  ASYNC = 1 << 5,
  /// If used, activates always on negedge of clk signal (ff-only)
  NEGEDGE = 1 << 6,

  /// DEFAULT TYPES
  latch = EN,
  CGG_FF_TYPE(ff, 1 << 4),

  /// ASYNC FLIP-FLOPS
  CGG_FF_TYPE(affr, ASYNC | ff | RST),
  CGG_FF_TYPE(affre, ASYNC | ff | EN | RST),
  CGG_FF_TYPE(affrs, ASYNC | ff | SET | RST),
  CGG_FF_TYPE(affrse, ASYNC | ff | EN | SET | RST),

  /// LATCHES
  latchr = latch | RST,
  latchc = latch | CLR,
  latchs = latch | SET,
  /// COMBINED LATCHES
  latchrs = latch | RST | SET,
  latchcs = latch | CLR | SET,

  /// FLIP-FLOPS
  CGG_FF_TYPE(ffe, ff | EN),
  CGG_FF_TYPE(ffr, ff | RST),
  CGG_FF_TYPE(ffc, ff | CLR),
  CGG_FF_TYPE(ffs, ff | SET),
  /// COMBINED FLIP-FLOPS
  CGG_FF_TYPE(ffre, ff | EN | RST),
  CGG_FF_TYPE(ffce, ff | EN | CLR),
  CGG_FF_TYPE(ffse, ff | EN | SET),

  CGG_FF_TYPE(ffrs, ff | RST | SET),
  CGG_FF_TYPE(ffcs, ff | CLR | SET),

  CGG_FF_TYPE(ffrse, ff | EN | RST | SET),
  CGG_FF_TYPE(ffcse, ff | EN | CLR | SET)
};

/// @brief Enumeration of logic gate types
/// @enum Gates
/// @details Defines the various types of logic elements that can be used in digital circuits.
enum Gates : uint8_t {
  GateAnd,    ///< Logical element - AND
  GateNand,   ///< Logical element - "AND-NOT" (NAND)
  GateOr,     ///< Logical element - OR
  GateNor,    ///< Logical element - "OR-NOT" (NOR)
  GateXor,    ///< Logical element - XOR (Exclusive OR)
  GateXnor,   ///< Logical element - XNOR
  GateNot,    ///< Logical element - NOT
  GateBuf,    ///< Logical element - Buffer
  GateDefault ///< Default logical element (error state)
};

/// @brief Enumeration for DOT graph generation components
/// @enum DotTypes
enum DotTypes : uint8_t {
  DotGraph = 0,    ///< DOT type representing graph
  DotInput = 1,    ///< DOT type representing input of a graph
  DotConstant = 2, ///< DOT type representing constant value
  DotOutput = 3,   ///< DOT type representing output of a graph
  DotGate = 4,     ///< DOT type representing logic gate in DOT format
  DotEdge = 5,     ///< DOT type representing an edge between vertices
  DotSubGraph = 6, ///< DOT type representing subgraph inside a graph
  DotExit = 7      ///< DOT type representing end of graph
};

/// @brief Enumeration of possible signal values in circuit simulation
/// @enum ValueStates
enum ValueStates : char {
  UndefinedState = 'n',  ///< Undefined/unknown state
  TrueValue = '1',       ///< Logical true (high)
  FalseValue = '0',      ///< Logical false (low)
  HighImpedance = 'z',   ///< High impedance (tri-state)
  NoSignal = 'x'         ///< No signal/invalid
};

/// \namespace GraphUtils
/// \brief Utility functions and data for graph operations
/// 
/// Contains static data and helper functions for:
/// - Converting between string and enum representations
/// - Logic operation mappings
/// - Graph hierarchy utilities
namespace GraphUtils {

/// @brief Gets information about a logical operation by its name
/// @param i_op A string containing the name of the logical operation
/// @return std::pair<std::string_view, int32_t> A pair containing the name and
/// ID of the logical operation
/// @code
/// std::pair<std::string_view, int32_t> operationInfo =
/// GraphUtils::getLogicOperation("and");
/// // Output information about the logical operation
/// std::cout << "Operation name: " << operationInfo.first << std::endl;
/// std::cout << "Operation ID: " << operationInfo.second << std::endl;
/// @endcode
/// @throws std::runtime_error if operation name is not found
std::pair<std::string_view, int32_t> getLogicOperation(const std::string &i_op);

/// @brief Returns the keys of logical operations
/// @return std::vector<Gates> A vector containing the keys of logical operations
std::vector<Gates> getLogicOperationsKeys();

/// @brief Returns logical operations along with information about single input gates
/// @return std::pair<std::vector<bool>, std::vector<Gates>> A pair of
/// vectors: information about single-input gates and the keys of logical operations
/// @code
/// auto logicOperationsInfo = GraphUtils::getLogicOperationsWithGates();
/// for (size_t i = 0; i < logicOperationsInfo.second.size(); ++i)
/// {
///   std::string operationName =
///     GraphUtils::parseGateToString(logicOperationsInfo.second[i]);
///   bool hasOneInput = logicOperationsInfo.first[i];
///   std::cout << "Operation: " << operationName;
///   if (hasOneInput)
///   {
///     std::cout << " (Has one input)";
///   }
///   else
///   {
///     std::cout << " (Does not have one input)";
///   }
///   std::cout << std::endl;
/// }
/// @endcode
std::pair<std::vector<bool>, std::vector<Gates>> getLogicOperationsWithGates();

/// @brief Converts operation string to its formal name
/// @param i_op a string representing the operation
/// @return std::string Operation name
/// @throw std::runtime_error if the passed operation does not exist
std::string fromOperationsToName(std::string_view i_op);

/// @brief Converts hierarchy key to its corresponding operation value
/// @param key Required hierarchy key
/// @return std::string_view The value representing the operation
/// @throws std::out_of_range if the provided key does not exist
std::string_view fromHierarchyToOperation(int32_t key);

/// @brief Converts a string representation of a gate to its corresponding enum value
/// @param i_gate The string representation of the gate
/// @return Gates The enum value corresponding to the provided string representation
Gates parseStringToGate(std::string i_gate);

/// @brief Converts an enum value of a vertex type to its string representation
/// @param vertex The enum value representing the vertex type
/// @return std::string The string representation of the vertex type
std::string parseVertexToString(VertexTypes vertex);

/// @brief Converts an enum value of a gate to its string representation
/// @param gate The enum value representing the gate
/// @return std::string The string representation of the gate
std::string parseGateToString(Gates gate);

/// @brief Looks for a std::pair with a given key in an array of pairs
/// @tparam T Key type in pairs array
/// @tparam M Value type in pairs array
/// @tparam N Number of elements in the array
/// @param iterable Array of pairs to search in
/// @param key The key to search for (pair.first)
/// @return Pointer to found pair or nullptr if not found
template<typename T, typename M, size_t N>
static std::pair<T, M> *findPairByKey(std::pair<T, M> (&iterable)[N],
                                      const T &key) {
  auto *iter = std::find_if(std::begin(iterable), std::end(iterable),
                            [key](const auto &x) { return x.first == key; });
  return iter;
}

/// @var d_hierarchySize
/// @brief Size of the logic operations hierarchy (max value + 1 from d_logicOperations)
static constexpr size_t d_hierarchySize = 11;

/// @var d_logicOperations
/// @brief Static array mapping operation names to their string views and IDs
static constexpr std::array<
    std::pair<std::string_view, std::pair<std::string_view, int32_t>>,
    d_hierarchySize>
    d_logicOperations = {{{"input", {"", 10}},
                          {"output", {"=", 0}},
                          {"const", {"1'b0", 9}},
                          {"and", {"and", 4}},
                          {"nand", {"nand", 3}},
                          {"or", {"or", 6}},
                          {"nor", {"nor", 5}},
                          {"not", {"not", 7}},
                          {"buf", {"buf", 8}},
                          {"xor", {"xor", 2}},
                          {"xnor", {"xnor", 1}}}};

/// @var stringToGate
/// @brief Static array for converting strings to Gates enum values
static std::pair<std::string, Gates> stringToGate[] = {
    {"and", Gates::GateAnd}, {"nand", Gates::GateNand}, {"or", Gates::GateOr},
    {"nor", Gates::GateNor}, {"not", Gates::GateNot},   {"buf", Gates::GateBuf},
    {"xor", Gates::GateXor}, {"xnor", Gates::GateXnor}};

/// @var vertexToString
/// @brief Static array for converting VertexTypes enum to string representations
static std::pair<VertexTypes, std::string_view> vertexToString[] = {
    {VertexTypes::input, "input"},
    {VertexTypes::output, "output"},
    {VertexTypes::constant, "const"},
    {VertexTypes::subGraph, "subGraph"},
    {VertexTypes::sequential, "sequential"},
    {VertexTypes::gate, "gate"}};

/// @var gateToString
/// @brief Static array for converting Gates enum to string representations
/// @todo Optimize gateToString lookup by using Gates enum as direct array index
/// @note Current implementation requires linear search via findPairByKey
static std::pair<Gates, std::string_view> gateToString[] = {
    {Gates::GateAnd, "and"},      {Gates::GateNand, "nand"},
    {Gates::GateOr, "or"},        {Gates::GateNor, "nor"},
    {Gates::GateXor, "xor"},      {Gates::GateXnor, "xnor"},
    {Gates::GateNot, "not"},      {Gates::GateBuf, "buf"},
    {Gates::GateDefault, "ERROR"}};

} // namespace GraphUtils

} // namespace CG_Graph