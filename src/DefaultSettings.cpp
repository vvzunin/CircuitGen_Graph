#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include <CircuitGenGraph/DefaultSettings.hpp>

std::shared_ptr<DefaultSettings> DefaultSettings::d_singleton = nullptr;

std::shared_ptr<DefaultSettings> DefaultSettings::getInstance(const std::string& i_value) {
  /**
   * This is a safer way to create an instance. instance = new Singleton is
   * dangeruous in case two instance threads wants to access at the same time
   */
  if (d_singleton == nullptr) {
    d_singleton.reset(new DefaultSettings(i_value));
    d_singleton->loadSettings();
  }
  return d_singleton;
}

void DefaultSettings::loadSettings() {
  int32_t maxSize = 0;
  for (const auto& [key, value] : d_logicOperations) {
    maxSize = std::max(maxSize, value.second);
  }
  d_operationsToHierarchy.resize(maxSize);
  for (const auto& [key, value] : d_logicOperations) {
    d_operationsToHierarchy[value.second] = value.first;
  }

  for (const auto& [key, value] : d_logicOperations)
    d_operationsToName[value.first] = key;
}

std::string DefaultSettings::getInstanceName() const {
  return d_name;
}

std::pair<std::string, int32_t> DefaultSettings::getLogicOperation(
    const std::string& i_op
) {
  return d_logicOperations.at(i_op);
}

std::vector<Gates> DefaultSettings::getLogicOperationsKeys() {
  return d_logicElements;
}

std::pair<std::vector<bool>, std::vector<Gates>>
    DefaultSettings::getLogicOperationsWithGates() {
  std::vector<bool> oneGate;

  for (const auto& key : d_logicElements) {
    oneGate.push_back(key == Gates::GateBuf || key == Gates::GateNot);
  }

  return std::make_pair(oneGate, d_logicElements);
}

std::string_view DefaultSettings::fromOperationsToHierarchy(int32_t key
) const {
  return d_operationsToHierarchy.at(key);
}

std::string DefaultSettings::fromOperationsToName(std::string_view i_op) const {
  return d_operationsToName.at(i_op);
}

std::map<std::string, std::pair<std::string, int32_t>>
    DefaultSettings::getLogicOperations() const {
  return d_logicOperations;
}

Gates DefaultSettings::parseStringToGate(std::string i_gate) const {
  return stringToGate.at(i_gate);
}

std::string DefaultSettings::parseGateToString(Gates gate) const {
  return gateToString.at(gate);
}

std::string DefaultSettings::parseVertexToString(VertexTypes vertex) const {
  return vertexToString.at(vertex);
};
