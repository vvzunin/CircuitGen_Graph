#include <cassert>

#include <CircuitGenGraph/GraphUtils.hpp>

/* start of static variable values declaration */

namespace CG_Graph::GraphUtils {

/* end of static variable values declaration */

std::pair<std::string_view, int32_t>
getLogicOperation(const std::string &i_op) {
  const auto *iter =
      std::find_if(d_logicOperations.begin(), d_logicOperations.end(),
                   [&](const auto &x) { return x.first == i_op; });
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
  assert(iter != std::end(operationsToName) &&
         "name not found in operations' container");
  return std::string(iter->second);
}

Gates parseStringToGate(std::string i_gate) {
  return findPairByKey(stringToGate, i_gate)->second;
}

std::string parseGateToString(Gates gate) {
  return std::string(findPairByKey(gateToString, gate)->second);
}

std::string parseVertexToString(VertexTypes vertex) {
  return std::string(findPairByKey(vertexToString, vertex)->second);
};

} // namespace CG_Graph::GraphUtils
