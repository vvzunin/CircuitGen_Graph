#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

/// @file DefaultSettings.hpp

/// @brief VertexTypes
/// Enumeration of vertex types

#ifndef DotReturn
#define DotReturn \
  std::vector<std::pair<DotTypes, std::map<std::string, std::string>>>
#endif

enum VertexTypes : uint8_t {
  input = 0,    ///  input vertex
  output = 4,   ///  output vertex
  constant = 1, /// constant vertex
  gate = 2,     /// vertex representing a logical element
  subGraph = 3  /// subgraph that makes up the vertex
};

/// @brief Gates
/// Enumeration of types of logical elements
/// This enumeration defines the various types of logic elements that can be
/// used in digital circuits.

enum Gates : uint8_t {
  GateAnd,    /// logical element - "AND" (AND)
  GateNand,   /// logical element "AND-NOT" (NAND)
  GateOr,     /// logical element - "OR" (OR)
  GateNor,    /// logical element - "OR-NOT" (NOR)
  GateXor,    /// lofical element - "Exclusive OR" (XOR)
  GateXnor,   /// logical element - XNOR
  GateNot,    /// logical element - NOT
  GateBuf,    /// logical element - Buffer
  GateDefault /// default logical element
};

/// @brief DotTypes
/// Enumeration for DOT generation
enum DotTypes : uint8_t {
  DotGraph = 0,
  DotInput = 1,
  DotConstant = 2,
  DotOutput = 3,
  DotGate = 4,
  DotEdge = 5,
  DotSubGraph = 6,
  DotExit = 7
};

/// @todo: To add Description some fields
/// class DefaultSettings
///
/// This is the detailed one. More details. Private...
/// @param d_name This is a string field for storing the name of the settings
/// @param d_singleton Singleton ensures that only one instance of the class
/// exists in the application
/// @param d_logicOperations This is an associative std::map container that maps
/// strings (keys) into pairs of strings and integers. It is used to store
/// logical operations and their associated parameters, such as the symbolic
/// representation of the operation and its priority
/// @param d_operationsToHierarchy This is an associative std::map container
/// that maps integers to string vectors. It is used to store a hierarchy of
/// logical operations
/// @param d_operationsToName It is used to match symbolic representations of
/// logical operations and their names.
///

class DefaultSettings {
public:
  DefaultSettings(const std::string &i_path) : d_path(i_path) {}

  DefaultSettings(DefaultSettings &other) = delete;
  void operator=(const DefaultSettings &) = delete;

  /// @brief getDefaultInstance Gets a single instance of the DefaultSettings
  /// class The method provides creation and receipt of a single instance of the
  /// DefaultSettings class with the specified value of the settings path. If an
  /// instance already exists, the method returns a pointer to it,
  /// otherwise it creates a new instance, loads the settings from the
  /// file and returns a pointer to it.
  /// @param i_value The value of the path to the settings
  /// @return std::shared_ptr<DefaultSettings> A pointer to a single instance of
  /// the DefaultSettings class
  /// @code
  /// // Creating a single instance of the DefaultSettings class, if it hasn't
  /// been
  /// // created yet, and we get a pointer to it
  /// std::shared_ptr<DefaultSettings>        settingsInstance =
  /// DefaultSettings::getDefaultInstance("/path/to/settings");
  /// // Now we can use the settings Instance object to access the methods
  /// // and members of the DefaultSettings class
  /// std::string datasetPath = settingsInstance->getDatasetPath();
  /// std::cout << "Dataset path: " << datasetPath << std::endl;
  /// @endcode

  static std::shared_ptr<DefaultSettings>
  getDefaultInstance(const std::string &i_value);

  /// @brief loadSettings Downloads settings from a file
  /// The method loads the settings from a file with the name specified in
  /// the data member in_filename. If the file exists, the method reads the
  /// settings values from it and updates the corresponding data members of
  /// the class

  void loadSettings();

  /// @brief getDefaultInstanceName Gets the name of the current instance of
  /// settings
  /// @return std::string Name of the current instance of settings

  std::string getDefaultInstanceName() const;

  /// @brief getLogicOperation Gets information about a logical operation by
  /// its name
  /// @param i_op A string containing the name of the logical operation
  /// @return std::pair<std::string, int32_t> A pair containing the name and ID
  /// of the logical operation
  /// @code
  /// DefaultSettings settingsInstance;
  /// try {
  /// // Get information about the logical operation "and"
  /// std::pair<std::string, int32_t> operationInfo =
  /// settingsInstance.getLogicOperation("and");
  /// // Output information about the logical operation
  /// std::cout << "Operation name: " << operationInfo.first << std::endl;
  /// std::cout << "Operation ID: " << operationInfo.second << std::endl;
  /// } catch (const std::out_of_range& e) {
  /// // Handle an exception if the operation is not found
  /// LOG(ERROR) << "Error: " << e.what() << std::endl;
  /// }
  /// @endcode
  /// @throws std::out_of_range If the passed operation name does not exist
  /// in the list of logical operations

  std::pair<std::string, int32_t> getLogicOperation(const std::string &i_op);

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
  /// // Creating an instance of the DefaultSettings class or getting it from an
  /// // existing object
  /// std::shared_ptr<DefaultSettings>        settingsInstance =
  /// DefaultSettings::getDefaultInstance("/path/to/settings");
  /// // Get logical operations together with information about the presence
  /// // of a single input
  /// std::pair<std::vector<bool>, std::vector<Gates>> logicOperationsInfo =
  /// settingsInstance->getLogicOperationsWithGates();
  /// // Output information about each logical operation
  /// for (size_t i = 0; i < logicOperationsInfo.second.size(); ++i)
  /// {
  /// std::string operationName =
  /// settingsInstance->parseGateToString(logicOperationsInfo.second[i]);
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

  std::pair<std::vector<bool>, std::vector<Gates>>
  getLogicOperationsWithGates();

  /// @brief fromOperationsToName Converts the operation to its name
  /// @param i_op a string representing the operation
  /// @return std::string Operation name
  /// @code
  /// // Creating an instance of the DefaultSettings class or getting it from
  /// an existing object
  /// std::shared_ptr<DefaultSettings> settingsInstance =
  /// DefaultSettings::getDefaultInstance("/path/to/settings");
  /// Convert the operation to its name
  /// std::string operationName;
  /// try
  /// {
  /// operationName = settingsInstance->fromOperationsToName("and");
  /// std::cout << "Operation name: " << operationName << std::endl;
  /// } catch (const std::out_of_range& e) {
  /// LOG(ERROR) << "Error: " << e.what() << std::endl;
  /// }
  /// @endcode
  /// @throw std::out_of_range If the passed operation does not exist in the
  /// list of operations

  std::string fromOperationsToName(std::string_view i_op) const;

  /// @brief getLogicOperations all logical operations
  /// The method returns a dictionary containing all logical operations
  /// in the form of a pair, where the key is a string representing the
  /// name of the operation, and the value is a pair in which the first
  /// element is a string representing the name of the operation, and the
  /// second element is an integer value representing the identifier of
  /// the operation
  /// @return std::map<std::string, std::pair<std::string, int32_t>> Dictionary
  /// with logical operations

  std::map<std::string, std::pair<std::string, int32_t>>
  getLogicOperations() const;

  /// @brief fromOperationsToHierarchy Converts an operation key to its
  /// corresponding hierarchy
  /// @param key The key representing the operation
  /// @return std::vector<std::string> The hierarchy associated with the
  /// operation key
  /// @code
  /// // Creating an instance of the DefaultSettings class or getting it from an
  /// existing object std::shared_ptr<DefaultSettings> settingsInstance =
  /// DefaultSettings::getDefaultInstance("/path/to/settings");
  /// // Get the hierarchy associated with the operation key 5
  /// std::vector<std::string> operationHierarchy =
  /// settingsInstance->fromOperationsToHierarchy(5);
  /// // Output the hierarchy
  /// for(const auto& element : operationHierarchy)
  /// {
  ///     std::cout << element << " ";
  /// }
  /// std::cout << std::endl;
  /// @endcode
  /// @throws std::out_of_range If the provided key does not exist in the
  /// internal map of operation keys to hierarchies

  std::string_view fromOperationsToHierarchy(int32_t key) const;

  /// @brief parseStringToGate Converts a string representation of a gate to
  /// its corresponding enum value
  /// @param i_gate The string representation of the gate
  /// @return Gates The enum value corresponding to the provided string
  /// representation of the gate
  /// @code
  /// // Creating an instance of the DefaultSettings class or getting it from an
  /// existing object std::shared_ptr<DefaultSettings> settingsInstance =
  /// DefaultSettings::getDefaultInstance("/path/to/settings");
  /// // Convert the string representation "and" to its corresponding enum value
  /// Gates gate = settingsInstance->parseStringToGate("and");
  /// std::cout << "Enum value of 'and': " << gate << std::endl;
  /// @endcode

  Gates parseStringToGate(std::string i_gate) const;

  /// @brief parseVertexToString Converts an enum value of a vertex type to its
  /// corresponding string representation
  /// @param vertex The enum value representing the vertex type
  /// @return std::string The string representation of the provided vertex type
  /// enum value
  /// @code
  /// // Creating an instance of the DefaultSettings class or getting it from an
  /// existing object std::shared_ptr<DefaultSettings> settingsInstance =
  /// DefaultSettings::getDefaultInstance("/path/to/settings");
  /// // Convert the enum value VertexTypes::input to its corresponding string
  /// representation std::string vertexString =
  /// settingsInstance->parseVertexToString(VertexTypes::input); std::cout <<
  /// "String representation of VertexTypes::input: " << vertexString <<
  /// std::endl;
  /// @endcode

  static std::string parseVertexToString(VertexTypes vertex);

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
  /// // Creating an instance of the DefaultSettings class or getting it from an
  /// existing object std::shared_ptr<DefaultSettings> settingsInstance =
  /// DefaultSettings::getDefaultInstance("/path/to/settings");
  /// // Convert the enum value Gates::GateAnd to its corresponding string
  /// representation std::string gateString =
  /// settingsInstance->parseGateToString(Gates::GateAnd); std::cout << "String
  /// representation of Gates::GateAnd: " << gateString << std::endl;
  /// @endcode

  static std::string parseGateToString(Gates gate);

  static void resetSingletone() { d_singleton = nullptr; }

  template<typename T, typename M, size_t N>
  static std::pair<T, M> *findPairByKey(std::pair<T, M> (&iterable)[N],
                                        const T &key) {
    auto *iter = std::find_if(std::begin(iterable), std::end(iterable),
                              [key](const auto &x) { return x.first == key; });
    return iter;
  }

protected:
  static std::mutex singletoneProtection;
  std::string d_name;
  static std::shared_ptr<DefaultSettings> d_singleton;
  std::string d_path;
  const std::map<std::string, std::pair<std::string, int32_t>>
      d_logicOperations = {
          {"input", {"", 10}}, {"output", {"=", 0}},  {"const", {"1'b0", 9}},
          {"and", {"and", 4}}, {"nand", {"nand", 3}}, {"or", {"or", 6}},
          {"nor", {"nor", 5}}, {"not", {"not", 7}},   {"buf", {"buf", 8}},
          {"xor", {"xor", 2}}, {"xnor", {"xnor", 1}}

  };

  static std::vector<Gates> d_logicElements;

  static std::pair<std::string, Gates> stringToGate[8];

  static std::pair<VertexTypes, std::string_view> vertexToString[5];

  static std::pair<Gates, std::string_view> gateToString[9];

  std::vector<std::string_view> d_operationsToHierarchy;
  std::map<std::string_view, std::string> d_operationsToName;
};
