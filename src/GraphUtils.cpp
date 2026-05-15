/**
 * @file GraphUtils.cpp
 * @brief Реализация утилит графа (логические операции, парсинг, константы).
 */
#include <cassert>
#include <CircuitGenGraph/GraphUtils.hpp>
#include <CircuitGenGraph/Logging.hpp>

/* start of static variable values declaration */

namespace CG_Graph::GraphUtils {

/* end of static variable values declaration */

std::pair<std::string_view, int32_t>
getLogicOperation(const std::string &i_op) {
  const auto *iter =
      std::find_if(d_logicOperations.begin(), d_logicOperations.end(),
                   [&](const auto &x) { return x.first == i_op; });

  if (iter == d_logicOperations.end()) {
    CG_LOG_ERROR << "Logic operation '" << i_op << "' not found in container";
  }

  assert(iter != d_logicOperations.end() &&
         "value not found in operations' container");
  return iter->second;
}

std::vector<Gates> getLogicOperationsKeys() {
  static std::vector<Gates> logicElements = {
      Gates::GateAnd, Gates::GateNand, Gates::GateOr,  Gates::GateNor,
      Gates::GateXor, Gates::GateXnor, Gates::GateNot, Gates::GateBuf};
  return logicElements;
}

std::pair<std::vector<bool>, std::vector<Gates>> getLogicOperationsWithGates() {
  std::vector<bool> oneGate;

  auto &&res = getLogicOperationsKeys();
  for (const auto &key: res) {
    oneGate.push_back(key == Gates::GateBuf || key == Gates::GateNot);
  }

  return std::make_pair(oneGate, res);
}

std::string_view fromHierarchyToOperation(int32_t i_key) {
  /// \var d_operationsToHierarchy This is an associative std::map container
  /// that maps integers to string vectors. It is used to store a hierarchy of
  /// logical operations
  static std::string_view operationByHierarchy[d_hierarchySize];
  static bool filled = false;
  if (!filled) {
    for (size_t i = 0; i < d_logicOperations.size(); ++i) {
      operationByHierarchy[d_logicOperations[i].second.second] =
          d_logicOperations[i].second.first;
    }
    filled = true;
  }

  if (i_key >= d_hierarchySize) {
    CG_LOG_ERROR << "Hierarchy key " << i_key
                 << " out of range (max: " << d_hierarchySize - 1 << ")";
  }

  assert(i_key < d_hierarchySize);
  return operationByHierarchy[i_key];
}

std::string fromOperationsToName(std::string_view i_op) {
  /// \var d_operationsToName It is used to match symbolic representations of
  /// logical operations and their names.
  static std::pair<std::string_view, std::string_view>
      operationsToName[d_hierarchySize];
  static bool filled = false;
  if (!filled) {
    for (size_t i = 0; i < d_hierarchySize; ++i) {
      operationsToName[i] = {d_logicOperations[i].second.first,
                             d_logicOperations[i].first};
    }
    filled = true;
  }
  auto *iter =
      std::find_if(std::begin(operationsToName), std::end(operationsToName),
                   [&](const auto &x) { return x.first == i_op; });

  if (iter == std::end(operationsToName)) {
    CG_LOG_ERROR << "Operation name for symbol '" << i_op << "' not found";
  }

  assert(iter != std::end(operationsToName) &&
         "name not found in operations' container");
  return std::string(iter->second);
}

Gates parseStringToGate(std::string i_gate) {
  auto *pair = findPairByKey(stringToGate, i_gate);
  if (!pair) {
    CG_LOG_ERROR << "Failed to parse string to Gate: " << i_gate;
    return Gates::GateDefault;
  }
  return pair->second;
}

std::string parseGateToString(Gates gate) {
  auto *pair = findPairByKey(gateToString, gate);
  if (!pair) {
    CG_LOG_ERROR << "Failed to parse Gate enum to string: "
                 << static_cast<int>(gate);
    return "ERROR";
  }
  return std::string(pair->second);
}

std::string parseVertexToString(VertexTypes vertex) {
  auto *pair = findPairByKey(vertexToString, vertex);
  if (!pair) {
    CG_LOG_ERROR << "Failed to parse VertexType enum to string: "
                 << static_cast<int>(vertex);
    return "ERROR";
  }
  return std::string(pair->second);
};

} // namespace CG_Graph::GraphUtils
