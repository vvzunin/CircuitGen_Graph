#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include <CircuitGenGraph/DefaultSettings.hpp>

/* start of static variable values declaration */

std::shared_ptr<DefaultSettings> DefaultSettings::d_singleton = nullptr;
std::mutex DefaultSettings::singletoneProtection = {};

std::pair<VertexTypes, std::string_view> DefaultSettings::vertexToString[] = {
    {VertexTypes::input, "input"},
    {VertexTypes::output, "output"},
    {VertexTypes::constant, "const"},
    {VertexTypes::subGraph, "subGraph"},
    {VertexTypes::gate, "gate"}};

std::pair<Gates, std::string_view> DefaultSettings::gateToString[] = {
    {Gates::GateAnd, "and"},      {Gates::GateNand, "nand"},
    {Gates::GateOr, "or"},        {Gates::GateNor, "nor"},
    {Gates::GateNot, "not"},      {Gates::GateBuf, "buf"},
    {Gates::GateXor, "xor"},      {Gates::GateXnor, "xnor"},
    {Gates::GateDefault, "ERROR"}};

std::pair<std::string, Gates> DefaultSettings::stringToGate[] = {
    {"and", Gates::GateAnd}, {"nand", Gates::GateNand}, {"or", Gates::GateOr},
    {"nor", Gates::GateNor}, {"not", Gates::GateNot},   {"buf", Gates::GateBuf},
    {"xor", Gates::GateXor}, {"xnor", Gates::GateXnor}};

/* end of static variable values declaration */

std::shared_ptr<DefaultSettings>
DefaultSettings::getDefaultInstance(const std::string &i_value) {
  /**
   * This is a safer way to create an instance. instance = new Singleton is
   * dangeruous in case two instance threads wants to access at the same time
   */
  singletoneProtection.lock();
  if (d_singleton == nullptr) {
    d_singleton = std::make_shared<DefaultSettings>(i_value);
    d_singleton->loadSettings();
  }
  singletoneProtection.unlock();
  return d_singleton;
}

void DefaultSettings::loadSettings() {
  int32_t maxSize = 0;
  for (const auto &[key, value]: d_logicOperations) {
    maxSize = std::max(maxSize, value.second);
  }
  d_operationsToHierarchy.resize(maxSize + 1);
  for (const auto &[key, value]: d_logicOperations) {
    d_operationsToHierarchy[value.second] = value.first;
  }

  for (const auto &[key, value]: d_logicOperations)
    d_operationsToName[value.first] = key;
}

std::string DefaultSettings::getDefaultInstanceName() const {
  return d_name;
}

std::pair<std::string, int32_t>
DefaultSettings::getLogicOperation(const std::string &i_op) {
  return d_logicOperations.at(i_op);
}

std::vector<Gates> DefaultSettings::getLogicOperationsKeys() {
  static std::vector<Gates> logicElements = {
      Gates::GateAnd, Gates::GateNand, Gates::GateOr,  Gates::GateNor,
      Gates::GateXor, Gates::GateXnor, Gates::GateNot, Gates::GateBuf};
  return logicElements;
}

std::pair<std::vector<bool>, std::vector<Gates>>
DefaultSettings::getLogicOperationsWithGates() {
  std::vector<bool> oneGate;

  auto &&res = getLogicOperationsKeys();
  for (const auto &key: res) {
    oneGate.push_back(key == Gates::GateBuf || key == Gates::GateNot);
  }

  return std::make_pair(oneGate, res);
}

std::string_view DefaultSettings::fromOperationsToHierarchy(int32_t key) const {
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
  return findPairByKey(stringToGate, i_gate)->second;
}

std::string DefaultSettings::parseGateToString(Gates gate) {
  return std::string(findPairByKey(gateToString, gate)->second);
}

std::string DefaultSettings::parseVertexToString(VertexTypes vertex) {
  return std::string(findPairByKey(vertexToString, vertex)->second);
};
