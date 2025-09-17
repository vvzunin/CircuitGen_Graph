#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

/// @file GraphUtils.hpp

namespace CG_Graph {

#ifndef DotReturn
#define DotReturn \
  std::vector<std::pair<DotTypes, std::map<std::string, std::string>>>
#endif

/// @brief VertexTypes
/// Enumeration of vertex types
static constexpr size_t d_busInType = 8;
enum VertexTypes : uint8_t {
  input = 0,      ///  input vertex
  output = 5,     ///  output vertex
  constant = 1,   /// constant vertex
  gate = 2,       /// vertex representing a logical element
  subGraph = 3,   /// subgraph that makes up the vertex
  sequential = 4, /// sequential vertex type (d-latch or d-flip-flop)
  inputBus = input | d_busInType,
  constantBus = constant | d_busInType,
  gateBus = gate | d_busInType,
  sequentialBus = sequential | d_busInType,
  outputBus = output | d_busInType
};

// CGG - CiruitGenGraph
#define CGG_FF_TYPE(S, V) S = V, n##S = NEGEDGE | V

/// @brief Types of all sequential cells being supported
enum SequentialTypes : uint8_t {
  /// enable signal, writes data to output if is equal to 1'b1
  EN = 1 << 0,
  /// set signal, writes 1'b1 to output if is equal to 1'b1
  SET = 1 << 1,
  /// clear signal, writes 1'b0 to output if is equal to 1'b1
  CLR = 1 << 2,
  /// reset signal, writes 1'b0 to output if is equal to 1'b0
  RST = 1 << 3,
  /// use with reset only, makes ff async (adds negedge rst to signals list)
  ASYNC = 1 << 5,
  /// is is used, activates always on negedge of clk signal (ff-only)
  NEGEDGE = 1 << 6,

  /// DEFAULT TYPES
  latch = EN,
  CGG_FF_TYPE(ff, 1 << 4),

  /// ASYNC
  CGG_FF_TYPE(affr, ASYNC | ff | RST),
  CGG_FF_TYPE(affre, ASYNC | ff | EN | RST),
  CGG_FF_TYPE(affrs, ASYNC | ff | SET | RST),
  CGG_FF_TYPE(affrse, ASYNC | ff | EN | SET | RST),

  /// LATCHES
  latchr = latch | RST,
  latchc = latch | CLR,
  latchs = latch | SET,
  /// COMBINED
  latchrs = latch | RST | SET,
  latchcs = latch | CLR | SET,

  /// FF
  CGG_FF_TYPE(ffe, ff | EN),
  CGG_FF_TYPE(ffr, ff | RST),
  CGG_FF_TYPE(ffc, ff | CLR),
  CGG_FF_TYPE(ffs, ff | SET),
  /// COMBINED
  CGG_FF_TYPE(ffre, ff | EN | RST),
  CGG_FF_TYPE(ffce, ff | EN | CLR),
  CGG_FF_TYPE(ffse, ff | EN | SET),

  CGG_FF_TYPE(ffrs, ff | RST | SET),
  CGG_FF_TYPE(ffcs, ff | CLR | SET),

  CGG_FF_TYPE(ffrse, ff | EN | RST | SET),
  CGG_FF_TYPE(ffcse, ff | EN | CLR | SET)

};

/// @brief Gates
/// Enumeration of types of logical elements
/// This enumeration defines the various types of logic elements that can be
/// used in digital circuits.

enum Gates : uint8_t {
  GateAnd,           /// logical element - AND
  GateNand,          /// logical element - "AND-NOT" (NAND)
  GateOr,            /// logical element - OR
  GateNor,           /// logical element - "OR-NOT" (NOR)
  GateXor,           /// lofical element - XOR (Exclusive OR)
  GateXnor,          /// logical element - XNOR
  GateNot,           /// logical element - NOT
  GateBuf,           /// logical element - Buffer
  GateConcatenation, /// logical element - concatenation of several vertices or
                     /// buses
  GateSlice,         /// logical element - slice of bus
  GateDefault        /// default logical element
};

/// @brief DotTypes
/// Enumeration for DOT generation
enum DotTypes : uint8_t {
  DotGraph = 0,    /// DOT type, representing graph
  DotInput = 1,    /// DOT type, representing input of a graph
  DotConstant = 2, /// DOT type, representing constant value
  DotOutput = 3,   /// DOT type, representing output of a graph
  DotGate = 4,     /// DOT type, representing logic value in DOT format
  DotEdge = 5,     /// DOT type, representing an edge between vertices
  DotSubGraph = 6, /// DOT type, representing subraph inside a graph
  DotExit = 7      /// DOT type, representing end of graph
};

enum ValueStates : char {
  UndefindedState = 'n',
  TrueValue = '1',
  FalseValue = '0',
  HighImpedance = 'z',
  NoSignal = 'x'
};

/// \namespace GraphUtils stores all useful methods, used for some operations
/// with graph and its vertices.
/// @param d_logicOperations This is an associative std::map container that
/// maps strings (keys) into pairs of strings and integers. It is used to store;
/// @param d_hierarchySize Number of all possible operations being used.
/// @param stringToGate A static arrays of pairs with gates and strings.
/// key is a std::string, value - value from enum `Gates`. Use function
/// findPairByKey to iterate through it, if necessary.
/// @param gateToString A static array, which is quite similar to
/// `stringToGate`, but now key is a value from `Gates` enum, not string.
/// Contains std::string_view, not std::string. String values all
/// are hardcoded, so you do not need to care about memory
/// @param vertexToString A static array of pairs, vertex from enum
/// to its string representation. Use function
/// findPairByKey to iterate through it, if necessary.

namespace GraphUtils {

/// @brief getLogicOperation Gets information about a logical operation by
/// its name
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
/// @throws makes an assert, if values is out of range
/// (it is a non-excepted behavior)

std::pair<std::string_view, int32_t> getLogicOperation(const std::string &i_op);

/// @brief getLogicOperationsKeys Returns the keys of logical operations
/// @return std::vector<Gates> A vector containing the keys of logical
/// operations

std::vector<Gates> getLogicOperationsKeys();

/// @brief getLogicOperationsWithGates Returns logical operations along with
/// information about the presence of a single input
/// The method returns a pair of vectors: the first vector contains
/// information about whether each a logical operation has only one
/// input(true if this is the case, false otherwise),
/// and the second vector contains the keys(enumerated values) of all
/// available logical operations
/// @return std::pair<std::vector<bool>, std::vector<Gates>> A pair of
/// vectors: information about the presence of a single input and the keys
/// of logical operations
/// @code
/// // Get logical operations together with information about the presence
/// // of a single input
/// std::pair<std::vector<bool>, std::vector<Gates>> logicOperationsInfo =
/// GraphUtils::getLogicOperationsWithGates();
/// // Output information about each logical operation
/// for (size_t i = 0; i < logicOperationsInfo.second.size(); ++i)
/// {
/// std::string operationName =
/// GraphUtils::parseGateToString(logicOperationsInfo.second[i]);
/// bool hasOneInput = logicOperationsInfo.first[i];
/// std::cout << "Operation: " << operationName;
/// if (hasOneInput)
/// {
///   std::cout << " (Has one input)";
/// }
/// else
/// {
///   std::cout << " (Does not have one input)";
/// }
/// std::cout << std::endl;
/// }
/// @endcode

std::pair<std::vector<bool>, std::vector<Gates>> getLogicOperationsWithGates();

/// @brief fromOperationsToName Converts the operation to its name
/// @param i_op a string representing the operation
/// @return std::string Operation name
/// @code
/// // Convert the operation to its name
/// std::string operationName;
/// operationName = GraphUtils::fromOperationsToName("and");
/// std::cout << "Operation name: " << operationName << std::endl;
/// @endcode
/// @throw has an assert if the passed operation does not exist in the
/// list of operations

std::string fromOperationsToName(std::string_view i_op);

/// @brief fromHierarchyToOperation Converts hierarchy key to its
/// corresponding operation value
/// @param key Required hierarchy key
/// @return std::string_view The value representing the operation. As
/// string_view refers to hardcoded strings, you do not need to care about
/// lifetime
/// @code
/// // Get the hierarchy associated with the operation key 5
/// std::string_view element =
/// GraphUtils::fromHierarchyToOperation(5);
/// // Output the element
/// std::cout << element << " ";
/// @endcode
/// @throws assert if the provided key does not exist in the
/// internal array of operation keys to hierarchies

std::string_view fromHierarchyToOperation(int32_t key);

/// @brief parseStringToGate Converts a string representation of a gate to
/// its corresponding enum value
/// @param i_gate The string representation of the gate
/// @return Gates The enum value corresponding to the provided string
/// representation of the gate
/// @code
/// // Convert the string representation "and" to its corresponding enum value
/// Gates gate = GraphUtils::parseStringToGate("and");
/// std::cout << "Enum value of 'and': " << gate << std::endl;
/// @endcode

Gates parseStringToGate(std::string i_gate);

/// @brief parseVertexToString Converts an enum value of a vertex type to its
/// corresponding string representation
/// @param vertex The enum value representing the vertex type
/// @return std::string The string representation of the provided vertex type
/// enum value
/// @code
/// // Creating an instance of the GraphUtils class or getting it from an
/// // Convert the enum value VertexTypes::input to its corresponding string
/// representation std::string vertexString =
/// GraphUtils::parseVertexToString(VertexTypes::input); std::cout <<
/// "String representation of VertexTypes::input: " << vertexString <<
/// std::endl;
/// @endcode

std::string parseVertexToString(VertexTypes vertex);

/// @brief parseGateToString Converts an enum value of a gate to its
/// corresponding string representation
/// @param gate The enum value representing the gate
/// @return std::string The string representation of the provided gate enum
/// value This method converts an enum value representing a gate to its
/// corresponding string representation.
/// It retrieves the string representation from the internal map date
/// ToString, which maps enum values of gates to their string
/// representations.
/// @code
/// // Creating an instance of the GraphUtils class or getting it from an
/// // Convert the enum value Gates::GateAnd to its corresponding string
/// representation std::string gateString =
/// GraphUtils::parseGateToString(Gates::GateAnd); std::cout << "String
/// representation of Gates::GateAnd: " << gateString << std::endl;
/// @endcode

std::string parseGateToString(Gates gate);

/// @brief Looks for a std::pair with a given key (pair - <key, value>).
/// Most of all functions in GraphUtils use arrays of pairs, and looking
/// for a value with key is one of the most frequently used operations.
/// @tparam T key type in pairs array
/// @tparam M value type in pairs array
/// @tparam N number elements in an array
/// @param iterable array of pairs, in which value should be found
/// @param key the key that must contain the required pair (pair.first)
/// @return found pair or std::end(iterable)
template<typename T, typename M, size_t N>
static std::pair<T, M> *findPairByKey(std::pair<T, M> (&iterable)[N],
                                      const T &key) {
  auto *iter = std::find_if(std::begin(iterable), std::end(iterable),
                            [key](const auto &x) { return x.first == key; });
  return iter;
}

// Here is located max value + 1 from the array d_logicOperations
static constexpr size_t d_hierarchySize = 11;

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

static std::pair<std::string, Gates> stringToGate[] = {
    {"and", Gates::GateAnd},       {"nand", Gates::GateNand},
    {"or", Gates::GateOr},         {"nor", Gates::GateNor},
    {"not", Gates::GateNot},       {"buf", Gates::GateBuf},
    {"xor", Gates::GateXor},       {"xnor", Gates::GateXnor},
    {"busSlice", Gates::GateSlice}};

static std::pair<VertexTypes, std::string_view> vertexToString[] = {
    {VertexTypes::input, "input"},
    {VertexTypes::output, "output"},
    {VertexTypes::constant, "const"},
    {VertexTypes::subGraph, "subGraph"},
    {VertexTypes::sequential, "sequential"},
    {VertexTypes::gate, "gate"}};

/// TODO: we can use Gate type as an index
static std::pair<Gates, std::string_view> gateToString[] = {
    {Gates::GateAnd, "and"},        {Gates::GateNand, "nand"},
    {Gates::GateOr, "or"},          {Gates::GateNor, "nor"},
    {Gates::GateXor, "xor"},        {Gates::GateXnor, "xnor"},
    {Gates::GateNot, "not"},        {Gates::GateBuf, "buf"},
    {Gates::GateSlice, "busSlice"}, {Gates::GateDefault, "ERROR"}};

} // namespace GraphUtils

} // namespace CG_Graph
