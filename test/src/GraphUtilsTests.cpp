#include <CircuitGenGraph/GraphUtils.hpp>
#include <gtest/gtest.h>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

using namespace CG_Graph;

TEST(GraphUtTests, GetLogicOperTest) {
  std::pair<std::string_view, int32_t> funcName =
      GraphUtils::getLogicOperation("and");
  std::pair<std::string_view, int32_t> actualName = {"and", 4};
  ASSERT_EQ(funcName, actualName);
}

// TEST(GraphUtTests, GetLogicDeathTest) {
//   ASSERT_DEATH(GraphUtils::getLogicOperation("nonexistentName"),
//                "value not found in operations' container");
// }

TEST(GraphUtTests, GetOperationsGatesTest) {
  std::pair<std::vector<bool>, std::vector<Gates>> funcOperations =
      GraphUtils::getLogicOperationsWithGates();
  static std::vector<Gates> logicElements = {
      Gates::GateAnd, Gates::GateNand, Gates::GateOr,  Gates::GateNor,
      Gates::GateXor, Gates::GateXnor, Gates::GateNot, Gates::GateBuf};
  static std::vector<bool> logicValues = {false, false, false, false,
                                          false, false, true,  true};
  std::pair<std::vector<bool>, std::vector<Gates>> actualOperations = {
      logicValues, logicElements};
  EXPECT_EQ(funcOperations, actualOperations);
}

TEST(GraphUtTests, HierarchyTest) {
  std::string_view funcSign = GraphUtils::fromOperationsToHierarchy(0);
  std::string_view actualSign = "=";
  EXPECT_EQ(funcSign, actualSign);
}

TEST(GraphUtTests, FromOperToNameTest) {
  std::string funcName = GraphUtils::fromOperationsToName("1'b0");
  std::string actualName = "const";
  ASSERT_EQ(funcName, actualName);
}

// TEST(GraphUtTests, fromOperToNameDeathTest) {
//   ASSERT_DEATH(GraphUtils::fromOperationsToName("nonexistentName"),
//                "name not found in operations' container");
// }

TEST(GraphUtTests, ConvertsAllGatesCorrectly) {
  // Проверяем все возможные значения Gates
  EXPECT_EQ(GraphUtils::parseStringToGate("and"), Gates::GateAnd);
  EXPECT_EQ(GraphUtils::parseStringToGate("nand"), Gates::GateNand);
  EXPECT_EQ(GraphUtils::parseStringToGate("or"), Gates::GateOr);
  EXPECT_EQ(GraphUtils::parseStringToGate("nor"), Gates::GateNor);
  EXPECT_EQ(GraphUtils::parseStringToGate("xor"), Gates::GateXor);
  EXPECT_EQ(GraphUtils::parseStringToGate("xnor"), Gates::GateXnor);
  EXPECT_EQ(GraphUtils::parseStringToGate("not"), Gates::GateNot);
  EXPECT_EQ(GraphUtils::parseStringToGate("buf"), Gates::GateBuf);
}

TEST(GraphUtTests, ConvertsAllGatesCorrectly) {
  // Проверяем все возможные значения Gates
  EXPECT_EQ(GraphUtils::parseStringToGate("and"), Gates::GateAnd);
  EXPECT_EQ(GraphUtils::parseStringToGate("nand"), Gates::GateNand);
  EXPECT_EQ(GraphUtils::parseStringToGate("or"), Gates::GateOr);
  EXPECT_EQ(GraphUtils::parseStringToGate("nor"), Gates::GateNor);
  EXPECT_EQ(GraphUtils::parseStringToGate("xor"), Gates::GateXor);
  EXPECT_EQ(GraphUtils::parseStringToGate("xnor"), Gates::GateXnor);
  EXPECT_EQ(GraphUtils::parseStringToGate("not"), Gates::GateNot);
  EXPECT_EQ(GraphUtils::parseStringToGate("buf"), Gates::GateBuf);
}
