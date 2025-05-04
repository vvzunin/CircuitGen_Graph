#include <CircuitGenGraph/GraphVertex.hpp>
#include <gtest/gtest.h>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

using namespace CG_Graph;

/*
  Need to add another UpdateValueTest
  Need to remake RemoveVertexToOutConnections
*/

GraphPtr memoryOwnerGateGr = std::make_shared<OrientedGraph>();

TEST(TestConstructorWithoutIName, WithoutDefaultGatesParametrs) {
  VertexPtr gate = memoryOwnerGateGr->addGate(Gates::GateAnd);
  std::string graphNum = std::to_string(0);
  EXPECT_EQ(gate->getType(), VertexTypes::gate);
  EXPECT_EQ(gate->getTypeName(), "gate");
  EXPECT_EQ(gate->getName(), "gate_" + graphNum);
  EXPECT_EQ(gate->getLevel(), 0);
  EXPECT_EQ(gate->getValue(), 'x');
  EXPECT_EQ(gate->getBaseGraph().lock(), memoryOwnerGateGr);
  EXPECT_EQ(gate->getOutConnections().size(), 0);
}

/*NeedToExplain
Ask for EXPECT_EQ(gate.getRawName(), "gate_" + graphNum); (it was '1' as
graphNum)
*/

TEST(TestConstructorWithoutIName, WithDefaultGatesParametrs) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  std::string graphNum = std::to_string(0);
  GraphVertexGates gate(Gates::GateAnd, graphPtr);
  EXPECT_EQ(gate.getType(), VertexTypes::gate);
  EXPECT_EQ(gate.getTypeName(), "gate");
  EXPECT_EQ(gate.getRawName(), "gate_" + graphNum);
  EXPECT_EQ(gate.getLevel(), 0);
  EXPECT_EQ(gate.getValue(), 'x');
  EXPECT_EQ(gate.getBaseGraph().lock(), graphPtr);
  EXPECT_EQ(gate.getOutConnections().size(), 0);
}

TEST(TestConstructorWithIName_Gate, WithoutDefaultGatesParametrs) {
  VertexPtr gate = memoryOwnerGateGr->addGate(Gates::GateAnd, "Anything");
  EXPECT_EQ(gate->getBaseGraph().lock(), memoryOwnerGateGr);
  EXPECT_EQ(gate->getType(), VertexTypes::gate);
  EXPECT_EQ(gate->getTypeName(), "gate");
  EXPECT_EQ(gate->getName(), "Anything");
  EXPECT_EQ(gate->getLevel(), 0);
  EXPECT_EQ(gate->getValue(), 'x');
  EXPECT_EQ(gate->getOutConnections().size(), 0);
}

TEST(TestConstructorWithIName_Gate, WithDefaultGatesParametrs) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  std::string graphNum = std::to_string(1);
  GraphVertexGates gate(Gates::GateAnd, "Anything", graphPtr);
  EXPECT_EQ(gate.getType(), VertexTypes::gate);
  EXPECT_EQ(gate.getTypeName(), "gate");
  EXPECT_EQ(gate.getRawName(), "Anything");
  EXPECT_EQ(gate.getLevel(), 0);
  EXPECT_EQ(gate.getValue(), 'x');
  EXPECT_EQ(gate.getBaseGraph().lock(), graphPtr);
  EXPECT_EQ(gate.getOutConnections().size(), 0);
}

// -----OverrideMethodsTests

TEST(TestUpdateValue, GatesReturnDValueIfDInConnectionsSizeZero) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  const char c = gate1.getValue();
  EXPECT_EQ(gate1.updateValue(), c);
}

// TEST(TestUpdateValue, ReturnXIfDInConnectionsZeroIsDead) {
//   GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
//   gate1.addVertexToInConnections(nullptr);
//   EXPECT_EQ(gate1.updateValue(), 'x');
// }

TEST(TestUpdateLevel, GatesCorrectUpdate) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr inputVert = graph->addInput();
  VertexPtr gateVert = graph->addGate(GateAnd);
  VertexPtr outputVert = graph->addOutput();
  graph->addEdge(inputVert, gateVert);
  graph->addEdge(gateVert, outputVert);
  graph->updateLevels(true);
  EXPECT_EQ(gateVert->getLevel(), 1);
}

TEST(TestUpdateLevel, GatesThrowInvalidArgumentIfDInconnectionsNIsNullptr) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);

  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd);
  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd);
  gate1.addVertexToInConnections(gatePtr1);
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_NO_THROW(gate1.updateLevel());

  // gate1.addVertexToInConnections(nullptr);
  // EXPECT_THROW(gate1.updateLevel(true), std::invalid_argument);

  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateAnd);
  gate1.addVertexToInConnections(gatePtr3);
  // EXPECT_THROW(gate1.updateLevel(true), std::invalid_argument);
}

TEST(TestGetVerilogString, ReturnEmptyStringIfDInConnectionsSizeIsZero) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  EXPECT_EQ(gate1.getVerilogString(), "");
}

TEST(TestGetVerilogString, ThrowInvalidArgumentIfDInConnectionsZeroIsDead) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  gate1.addVertexToInConnections(nullptr);
  EXPECT_THROW(gate1.getVerilogString(), std::invalid_argument);
}

TEST(TestGetVerilogString, ThrowInvalidArgumentIfDInConnectionsNIsDead) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd);
  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd);
  gate1.addVertexToInConnections(gatePtr1);
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_NO_THROW(gate1.getVerilogString());

  gate1.addVertexToInConnections(nullptr);
  EXPECT_THROW(gate1.getVerilogString(), std::invalid_argument);

  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateAnd);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_THROW(gate1.getVerilogString(), std::invalid_argument);
}

TEST(TestGetVerilogString, ReturnStringWithAndExpressionWhenUseGateAnd) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var1");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "Var1");

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_EQ(gate1.getVerilogString(), "Var1 & Var2 & Var3");
}

// TODO: Update with easylogging
// TEST(TestGetVerilogString, CerrErrorStringWhenUseGateDefault) {
//   GraphVertexGates gate1(Gates::GateDefault);
//   VertexPtr        gatePtr1 =
//       memoryOwnerGateGr->addGate(Gates::GateAnd, "Var1");
//   VertexPtr gatePtr2 =
//       memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
//   gate1.addVertexToInConnections(gatePtr1);
//   gate1.addVertexToInConnections(gatePtr2);

//   std::stringstream capturedOutput;
//   std::streambuf*   originalStderr = std::cerr.rdbuf(capturedOutput.rdbuf());

//   gate1.getVerilogString();

//   std::cerr.rdbuf(originalStderr);
//   std::string output = capturedOutput.str();
//   EXPECT_EQ(output, "Error\n");
// }

TEST(TestGetVerilogString, ReturnCorrectStringExpressionWhenUseGateNand) {
  GraphVertexGates gate1(Gates::GateNand, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var1");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "~(Var1)");

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_EQ(gate1.getVerilogString(), "~(Var1 & Var2 & Var3)");
}

TEST(TestGetVerilogString, ReturnCorrectStringExpressionWhenUsedGateNor) {
  GraphVertexGates gate1(Gates::GateNor, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var1");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "~(Var1)");

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_EQ(gate1.getVerilogString(), "~(Var1 | Var2 | Var3)");
}

// TEST(TestGetVerilogString, ReturnCorrectStrinExpressionWhenUseGateNot) {
//   GraphVertexGates gate1(Gates::GateNot, memoryOwnerGateGr);
//   VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var1");
//   gate1.addVertexToInConnections(gatePtr1);
//   EXPECT_EQ(gate1.getVerilogString(), "~Var1");

//   VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
//   VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
//   gate1.addVertexToInConnections(gatePtr2);
//   gate1.addVertexToInConnections(gatePtr3);
//   EXPECT_EQ(gate1.getVerilogString(), "~Var1 ~ Var2 ~ Var3");
// }

TEST(TestGetVerilogString, ReturnCorrectStringExpressionWhenUsedGateOr) {
  GraphVertexGates gate1(Gates::GateOr, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var1");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "Var1");

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_EQ(gate1.getVerilogString(), "Var1 | Var2 | Var3");
}

TEST(TestGetVerilogString, ReturnCorrectStringExpressionWhenUseGateXnor) {
  GraphVertexGates gate1(Gates::GateXnor, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var1");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "~(Var1)");

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_EQ(gate1.getVerilogString(), "~(Var1 ^ Var2 ^ Var3)");
}

TEST(TestGetVerilogString, ReturnCorrectStringExpressionWhenUseGateXor) {
  GraphVertexGates gate1(Gates::GateXor, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var1");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "Var1");

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_EQ(gate1.getVerilogString(), "Var1 ^ Var2 ^ Var3");
}

TEST(TestGetGate, ReturnCorrectGate) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  EXPECT_EQ(gate1.getGate(), Gates::GateAnd);

  GraphVertexGates gate2(Gates::GateBuf, memoryOwnerGateGr);
  EXPECT_EQ(gate2.getGate(), Gates::GateBuf);

  GraphVertexGates gate3(Gates::GateDefault, memoryOwnerGateGr);
  EXPECT_EQ(gate3.getGate(), Gates::GateDefault);

  GraphVertexGates gate4(Gates::GateNand, memoryOwnerGateGr);
  EXPECT_EQ(gate4.getGate(), Gates::GateNand);

  GraphVertexGates gate5(Gates::GateNor, memoryOwnerGateGr);
  EXPECT_EQ(gate5.getGate(), Gates::GateNor);

  GraphVertexGates gate6(Gates::GateNot, memoryOwnerGateGr);
  EXPECT_EQ(gate6.getGate(), Gates::GateNot);

  GraphVertexGates gate7(Gates::GateOr, memoryOwnerGateGr);
  EXPECT_EQ(gate7.getGate(), Gates::GateOr);

  GraphVertexGates gate8(Gates::GateXnor, memoryOwnerGateGr);
  EXPECT_EQ(gate8.getGate(), Gates::GateXnor);

  GraphVertexGates gate9(Gates::GateXor, memoryOwnerGateGr);
  EXPECT_EQ(gate9.getGate(), Gates::GateXor);
}

// TODO: Update with easylogging
// TEST(TestToVerilog, CerrErrorStringIfDInConnectionsSizeIzZero) {
//   GraphVertexGates  gate1(Gates::GateAnd);
//   std::stringstream capturedOutput;
//   std::streambuf*   originalStderr = std::cerr.rdbuf(capturedOutput.rdbuf());

//   gate1.toVerilog();

//   std::cerr.rdbuf(originalStderr);
//   std::string output = capturedOutput.str();
//   EXPECT_EQ(output, "TODO: delete empty vertices: " + gate1.getRawName() +
//   "\n");
// }

TEST(TestToDOT, TryingTomakeDotWithSeveralPtr) {
  GraphVertexGates gate(Gates::GateAnd, memoryOwnerGateGr);
  std::vector<std::pair<CG_Graph::DotTypes, std::map<std::string, std::string>>>
      emptyDot;
  EXPECT_EQ(gate.toDOT(), emptyDot);
}

TEST(TestToVerilog, ReturnEmptyStringIfDInConnectionsSizeIsZero) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  EXPECT_EQ(gate1.toVerilog(), "");
}

TEST(TestToVerilog, ThrowInvalidArgumentIfDInConnectionsLastIsDead) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  // gate1.addVertexToInConnections(nullptr);
  // EXPECT_THROW(gate1.toVerilog(), std::invalid_argument);

  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_NO_THROW(gate1.toVerilog());

  // gate1.addVertexToInConnections(nullptr);
  // EXPECT_THROW(gate1.toVerilog(), std::invalid_argument);
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateAnd) {
  GraphVertexGates gate1(Gates::GateAnd, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2;");

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2 & Var3;");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseBuf) {
  GraphVertexGates gate1(Gates::GateBuf, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2;");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseDefault) {
  GraphVertexGates gate1(Gates::GateDefault, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
  gate1.addVertexToInConnections(gatePtr1);
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2 Error Var3;");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateNand) {
  GraphVertexGates gate1(Gates::GateNand, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 );");

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 & Var3 );");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateNor) {
  GraphVertexGates gate1(Gates::GateNor, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 );");

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 | Var3 );");
}

// TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateNot) {
//   GraphVertexGates gate1(Gates::GateNot, "Var1", memoryOwnerGateGr);
//   VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
//   gate1.addVertexToInConnections(gatePtr1);
//   EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~Var2;");

//   // Does it correct eq?
//   VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
//   gate1.addVertexToInConnections(gatePtr2);
//   EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~Var3;");
// }

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateOr) {
  GraphVertexGates gate1(Gates::GateOr, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2;");

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2 | Var3;");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateXnor) {
  GraphVertexGates gate1(Gates::GateXnor, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 );");

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 ^ Var3 );");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateXor) {
  GraphVertexGates gate1(Gates::GateXor, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2;");

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2 ^ Var3;");
}

// -------------------------------------

TEST(TestSetName_Gate, GatesCorrectName) {
  GraphVertexGates gate(Gates::GateAnd, memoryOwnerGateGr);
  gate.setName("Anything");
  EXPECT_EQ(gate.getRawName(), "Anything");
}

TEST(TestAddInConnections, GatesAddConnections) {
  VertexPtr gate1 = memoryOwnerGateGr->addGate(Gates::GateAnd);
  EXPECT_EQ(gate1->getInConnections().size(), 0);

  memoryOwnerGateGr->addGate(Gates::GateAnd);
  GraphVertexGates *ptr1 = static_cast<GraphVertexGates *>(
      memoryOwnerGateGr->addGate(Gates::GateOr));
  EXPECT_EQ(gate1->addVertexToInConnections(ptr1), 1);
  EXPECT_EQ(gate1->addVertexToInConnections(ptr1), 2);
  EXPECT_EQ(gate1->getInConnections()[0], ptr1);
  EXPECT_EQ(gate1->getInConnections()[1], ptr1);

  memoryOwnerGateGr->addGate(Gates::GateAnd);
  GraphVertexGates *ptr2 = static_cast<GraphVertexGates *>(
      memoryOwnerGateGr->addGate(Gates::GateAnd));
  gate1->addVertexToInConnections(ptr2);
  EXPECT_EQ(gate1->getInConnections()[2], ptr2);
}
TEST(TestAddInConnectionsEdgeNumber, ThrowExceptionWhenManyInputsForNotOrBuf) {
  GraphVertexGates gate1(Gates::GateNot, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var1");
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "~Var1");

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  EXPECT_THROW(gate1.addVertexToInConnections(gatePtr2), std::overflow_error);
  GraphVertexGates gate2(Gates::GateAnd, memoryOwnerGateGr);
  VertexPtr gatePtr3 = memoryOwnerGateGr->addInput("Var3");
  VertexPtr gatePtr4 = memoryOwnerGateGr->addInput("Var4");
  gate2.addVertexToInConnections(gatePtr3);
  EXPECT_NO_THROW(gate2.addVertexToInConnections(gatePtr4));
}

TEST(TestAddOutConnections, GatesAddConnections) {
  VertexPtr gate1 = memoryOwnerGateGr->addGate(Gates::GateAnd);
  EXPECT_EQ(gate1->getOutConnections().size(), 0);

  GraphVertexGates gate2(Gates::GateAnd, memoryOwnerGateGr);
  VertexPtr ptr1 = memoryOwnerGateGr->addGate(Gates::GateOr);
  EXPECT_EQ(gate1->addVertexToOutConnections(ptr1), true);
  EXPECT_EQ(gate1->addVertexToOutConnections(ptr1), false);
  EXPECT_EQ(gate1->getOutConnections()[0], ptr1);

  memoryOwnerGateGr->addGate(Gates::GateAnd);
  auto ptr2 = memoryOwnerGateGr->addGate(Gates::GateAnd);
  gate1->addVertexToOutConnections(ptr2);
  EXPECT_EQ(gate1->getOutConnections()[1], ptr2);
}

TEST(TestCalculateHash_Gate, SameHashWhenEqualInputs) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  GraphVertexGates gate2(Gates::GateAnd, memoryOwnerGateGr);
  EXPECT_EQ(gate1.calculateHash(), gate2.calculateHash());

  gate1.addVertexToOutConnections(memoryOwnerGateGr->addGate(Gates::GateAnd));
  EXPECT_NE(gate1.calculateHash(true), gate2.calculateHash(true));
  gate2.addVertexToOutConnections(memoryOwnerGateGr->addGate(Gates::GateAnd));
  EXPECT_EQ(gate1.calculateHash(true), gate2.calculateHash(true));
}

TEST(TestCalulateHash_Gate, BistableCell) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  // level = 0
  VertexPtr inputVertA = graph->addInput();
  graph->addInput();
  VertexPtr gateVertA = graph->addGate(GateNor);
  VertexPtr gateVertB = graph->addGate(GateNor);
  VertexPtr outputVertA = graph->addOutput();
  VertexPtr outputVertB = graph->addOutput();
  graph->addEdge(inputVertA, gateVertA);
  graph->addEdge(gateVertB, outputVertB);
  graph->addEdge(gateVertA, outputVertA);
  graph->addEdge(gateVertA, gateVertB);
  graph->addEdge(gateVertB, outputVertB);
  graph->addEdge(gateVertB, gateVertA);

  GraphPtr graph1 = std::make_shared<OrientedGraph>();
  // level = 0
  VertexPtr inputVertA1 = graph1->addInput();
  graph1->addInput();
  VertexPtr gateVertA1 = graph1->addGate(GateNor);
  VertexPtr gateVertB1 = graph1->addGate(GateNor);
  VertexPtr outputVertA1 = graph1->addOutput();
  VertexPtr outputVertB1 = graph1->addOutput();
  graph1->addEdge(inputVertA1, gateVertA1);
  graph1->addEdge(gateVertB1, outputVertB1);
  graph1->addEdge(gateVertA1, outputVertA1);
  graph1->addEdge(gateVertB1, gateVertA1);
  graph1->addEdge(gateVertB1, outputVertB1);
  graph1->addEdge(gateVertA1, gateVertB1);
  graph1->calculateHash(true);

  EXPECT_NE(graph->calculateHash(true), graph1->calculateHash(true));
}

// TEST(TestRemoveVertexToInConnections, GatesRemoveConnections) {
//   VertexPtr gatesPtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd);
//   EXPECT_EQ(gatesPtr1->removeVertexToInConnections(nullptr), false);

//   gatesPtr1->addVertexToInConnections(
//       memoryOwnerGateGr->addGate(Gates::GateAnd));
//   gatesPtr1->addVertexToInConnections(
//       memoryOwnerGateGr->addGate(Gates::GateAnd));
//   EXPECT_EQ(gatesPtr1->getInConnections().size(), 2);
//   EXPECT_EQ(gatesPtr1->removeVertexToInConnections(nullptr), true);
//   EXPECT_EQ(gatesPtr1->getInConnections().size(), 1);
// }

// TEST(TestRemoveVertexToOutConnections, GatesRemoveConnections) {
// #ifdef LOGFLAG
//   initLogging("TestRemoveVertexToOutConnections", "RemoveConnections");
// #endif
//   // Создаем вершину графа
//   VertexPtr vertex1 = memoryOwnerGateGr->addGate((Gates::GateAnd));

//   // Добавляем несколько соединений к выходным соединениям вершины
//   VertexPtr vertex2 = memoryOwnerGateGr->addGate((Gates::GateAnd));
//   VertexPtr vertex3 = memoryOwnerGateGr->addGate((Gates::GateAnd));
//   vertex1->addVertexToOutConnections(vertex2);
//   vertex1->addVertexToOutConnections(vertex3);

//   // Проверяем, что соединения добавлены
//   EXPECT_EQ(vertex1->getOutConnections().size(), 2);

//   // Удаляем последнее оставшееся соединение
//   EXPECT_EQ(vertex1->removeVertexToOutConnections(vertex3), true);
//   EXPECT_EQ(vertex3->removeVertexToOutConnections(vertex3), false);
// }

TEST(GraphVertexGatesTest, UpdateValue_NoConnections) {
  GraphVertexGates gate(Gates::GateBuf, memoryOwnerGateGr);
  EXPECT_TRUE(gate.updateValue());
}

// TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateXnor) {
//   GraphVertexGates gate1(Gates::GateXnor, "Var1", memoryOwnerGateGr);
//   VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(
//       Gates::GateAnd, "Var2");
//   gate1.addVertexToInConnections(gatePtr1);
//   EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 );");

//   VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(
//       Gates::GateAnd, "Var3");
//   gate1.addVertexToInConnections(gatePtr2);
//   EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 ^ Var3 );");
// }

TEST(GraphVerexGatesTest, IsSubGraph_Buffer) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  GraphVertexGates gate2(Gates::GateBuf, memoryOwnerGateGr);
  EXPECT_EQ(gate1.isSubgraphBuffer(), false);
  EXPECT_EQ(gate2.isSubgraphBuffer(), false);
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  graphPtr->addInput("input");

  GraphPtr subGraphPtr = std::make_shared<OrientedGraph>();
  subGraphPtr->addInput("subGraphInput");
  subGraphPtr->addOutput("subGraphOutput");

  GraphVertexGates *gate3 =
      static_cast<GraphVertexGates *>(graphPtr->addSubGraph(
          subGraphPtr, graphPtr->getVerticesByType(VertexTypes::input))[0]);
  EXPECT_EQ(gate3->isSubgraphBuffer(), true);
}

TEST(GraphVertexGatesTest, UpdateValue_GateAnd) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  gate1.addVertexToInConnections(gatePtr1);

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateOr, "Var2");
  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateXor, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_TRUE(gate1.updateValue());
}
TEST(GraphVertexGatesTest, UpdateValue_GateOr) {
  GraphVertexGates gate1(Gates::GateOr, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  gate1.addVertexToInConnections(gatePtr1);

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateOr, "Var2");
  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateXor, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_TRUE(gate1.updateValue());
}

TEST(GraphVertexGatesTest, UpdateValue_GateXor) {
  GraphVertexGates gate1(Gates::GateXor, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  gate1.addVertexToInConnections(gatePtr1);

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateOr, "Var2");
  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateXor, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_TRUE(gate1.updateValue());
}

TEST(GraphVertexGatesTest, UpdateValue_GateNand) {
  GraphVertexGates gate1(Gates::GateNand, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  gate1.addVertexToInConnections(gatePtr1);

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateOr, "Var2");
  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateXor, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_TRUE(gate1.updateValue());
}

TEST(GraphVertexGatesTest, UpdateValue_GateNor) {
  GraphVertexGates gate1(Gates::GateNor, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  gate1.addVertexToInConnections(gatePtr1);

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateOr, "Var2");
  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateXor, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_TRUE(gate1.updateValue());
}

TEST(GraphVertexGatesTest, UpdateValue_GateXnor) {
  GraphVertexGates gate1(Gates::GateXnor, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->addGate(Gates::GateAnd, "Var2");
  gate1.addVertexToInConnections(gatePtr1);

  VertexPtr gatePtr2 = memoryOwnerGateGr->addGate(Gates::GateOr, "Var2");
  VertexPtr gatePtr3 = memoryOwnerGateGr->addGate(Gates::GateXor, "Var3");
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_TRUE(gate1.updateValue());
}

// TEST(GraphVertexGatesTest, UpdateValue_GateNot) {
//     GraphVertexGates inputGate(GateType::BUF);
//     GraphVertexGates notGate(GateType::NOT);

//     notGate.addInput(&inputGate);

//     inputGate.updateValue();
//     EXPECT_TRUE(notGate.updateValue());
// }

// TEST(GraphVertexGatesTest, UpdateValue_GateAnd) {
//     GraphVertexGates inputGate1(GateType::BUF);
//     GraphVertexGates inputGate2(GateType::BUF);
//     GraphVertexGates andGate(GateType::AND);

//     andGate.addInput(&inputGate1);
//     andGate.addInput(&inputGate2);

//     inputGate1.updateValue();
//     inputGate2.updateValue();
//     EXPECT_TRUE(andGate.updateValue());
// }

// TEST(GraphVertexGatesTest, UpdateValue_NoConnections) {
//   GraphVertexGates vertex(Gates::GateAnd);
//   ASSERT_EQ(vertex.updateValue(), vertex.getValue());
// }

// TEST(GraphVertexGatesTest, UpdateValue_SingleConnection) {
//   GraphVertexGates vertex(Gates::GateBuf);
//   GraphVertexGates inputVertex(Gates::GateAnd);
//   inputVertex.setValue('1');
//   vertex.addConnection(&inputVertex);

//   ASSERT_EQ(vertex.updateValue(), tableBuf.at('1'));
// }

// TEST(GraphVertexGatesTest, UpdateValue_MultipleConnections_AND) {
//   GraphVertexGates vertex(Gates::GateAnd);
//   GraphVertexGates input1(Gates::GateBuf);
//   GraphVertexGates input2(Gates::GateBuf);

//   input1.setValue('1');
//   input2.setValue('0');

//   vertex.addConnection(&input1);
//   vertex.addConnection(&input2);

//   char expected = tableAnd.at(input1.getValue()).at(input2.getValue());
//   ASSERT_EQ(vertex.updateValue(), expected);
// }
