#include <CircuitGenGraph/GraphVertex.hpp>
#include <gtest/gtest.h>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

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

TEST(TestConstructorWithoutIName, WithDefaultGatesParametrs) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  std::string graphNum = std::to_string(1);
  GraphVertexGates gate(Gates::GateAnd, graphPtr);
  EXPECT_EQ(gate.getType(), VertexTypes::gate);
  EXPECT_EQ(gate.getTypeName(), "gate");
  EXPECT_EQ(gate.getRawName(), "gate_" + graphNum);
  EXPECT_EQ(gate.getLevel(), 0);
  EXPECT_EQ(gate.getValue(), 'x');
  EXPECT_EQ(gate.getBaseGraph().lock(), graphPtr);
  EXPECT_EQ(gate.getOutConnections().size(), 0);
}

TEST(TestConstructorWithIName, WithoutDefaultGatesParametrs) {
  VertexPtr gate = memoryOwnerGateGr->addGate(
      Gates::GateAnd, "Anything");
  EXPECT_EQ(gate->getBaseGraph().lock(), memoryOwnerGateGr);
  EXPECT_EQ(gate->getType(), VertexTypes::gate);
  EXPECT_EQ(gate->getTypeName(), "gate");
  EXPECT_EQ(gate->getName(), "Anything");
  EXPECT_EQ(gate->getLevel(), 0);
  EXPECT_EQ(gate->getValue(), 'x');
  EXPECT_EQ(gate->getOutConnections().size(), 0);
}

TEST(TestConstructorWithIName, WithDefaultGatesParametrs) {
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

TEST(TestUpdateValue, ReturnDValueIfDInConnectionsSizeZero) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  const char c = gate1.getValue();
  EXPECT_EQ(gate1.updateValue(), c);
}

// TEST(TestUpdateValue, ReturnXIfDInConnectionsZeroIsDead) {
//   GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
//   gate1.addVertexToInConnections(nullptr);
//   EXPECT_EQ(gate1.updateValue(), 'x');
// }

TEST(TestUpdateLevel, CorrectUpdate) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  gatePtr1->setLevel(1);
  gate1.addVertexToInConnections(gatePtr1);
  gate1.updateLevel(true);
  EXPECT_EQ(gate1.getLevel(), 2);

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  VertexPtr gatePtr3 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  gatePtr2->setLevel(3);
  gatePtr3->setLevel(2);
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  gate1.updateLevel(true);
  EXPECT_EQ(gate1.getLevel(), 4);

  VertexPtr gatePtr4 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr4);
  gate1.updateLevel(true);
  EXPECT_EQ(gate1.getLevel(), 4);
}

TEST(TestUpdateLevel, ThrowInvalidArgumentIfDInconnectionsNIsNullptr) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);

  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_NO_THROW(gate1.updateLevel());

  // gate1.addVertexToInConnections(nullptr);
  // EXPECT_THROW(gate1.updateLevel(true), std::invalid_argument);

  VertexPtr gatePtr3 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
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
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_NO_THROW(gate1.getVerilogString());

  gate1.addVertexToInConnections(nullptr);
  EXPECT_THROW(gate1.getVerilogString(), std::invalid_argument);

  VertexPtr gatePtr3 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_THROW(gate1.getVerilogString(), std::invalid_argument);
}

TEST(TestGetVerilogString, ReturnStringWithAndExpressionWhenUseGateAnd) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var1", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "Var1");

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  VertexPtr gatePtr3 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_EQ(gate1.getVerilogString(), "Var1 & Var2 & Var3");
}

// TODO: Update with easylogging
// TEST(TestGetVerilogString, CerrErrorStringWhenUseGateDefault) {
//   GraphVertexGates gate1(Gates::GateDefault);
//   VertexPtr        gatePtr1 =
//       memoryOwnerGateGr->create<GraphVertexGates>(Gates::GateAnd, "Var1");
//   VertexPtr gatePtr2 =
//       memoryOwnerGateGr->create<GraphVertexGates>(Gates::GateAnd, "Var2");
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
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var1", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "~(Var1)");

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  VertexPtr gatePtr3 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_EQ(gate1.getVerilogString(), "~(Var1 & Var2 & Var3)");
}

TEST(TestGetVerilogString, ReturnCorrectStringExpressionWhenUsedGateNor) {
  GraphVertexGates gate1(Gates::GateNor, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var1", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "~(Var1)");

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  VertexPtr gatePtr3 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_EQ(gate1.getVerilogString(), "~(Var1 | Var2 | Var3)");
}

TEST(TestGetVerilogString, ReturnCorrectStrinExpressionWhenUseGateNot) {
  GraphVertexGates gate1(Gates::GateNot, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var1", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "~Var1");

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  VertexPtr gatePtr3 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_EQ(gate1.getVerilogString(), "~Var1 ~ Var2 ~ Var3");
}

TEST(TestGetVerilogString, ReturnCorrectStringExpressionWhenUsedGateOr) {
  GraphVertexGates gate1(Gates::GateOr, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var1", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "Var1");

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  VertexPtr gatePtr3 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_EQ(gate1.getVerilogString(), "Var1 | Var2 | Var3");
}

TEST(TestGetVerilogString, ReturnCorrectStringExpressionWhenUseGateXnor) {
  GraphVertexGates gate1(Gates::GateXnor, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var1", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "~(Var1)");

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  VertexPtr gatePtr3 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr2);
  gate1.addVertexToInConnections(gatePtr3);
  EXPECT_EQ(gate1.getVerilogString(), "~(Var1 ^ Var2 ^ Var3)");
}

TEST(TestGetVerilogString, ReturnCorrectStringExpressionWhenUseGateXor) {
  GraphVertexGates gate1(Gates::GateXor, memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var1", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.getVerilogString(), "Var1");

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  VertexPtr gatePtr3 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
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

TEST(TestToVerilog, ReturnEmptyStringIfDInConnectionsSizeIsZero) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  EXPECT_EQ(gate1.toVerilog(), "");
}

TEST(TestToVerilog, ThrowInvalidArgumentIfDInConnectionsLastIsDead) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  // gate1.addVertexToInConnections(nullptr);
  // EXPECT_THROW(gate1.toVerilog(), std::invalid_argument);

  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_NO_THROW(gate1.toVerilog());

  // gate1.addVertexToInConnections(nullptr);
  // EXPECT_THROW(gate1.toVerilog(), std::invalid_argument);
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateAnd) {
  GraphVertexGates gate1(Gates::GateAnd, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2;");

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2 & Var3;");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseBuf) {
  GraphVertexGates gate1(Gates::GateBuf, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2;");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseDefault) {
  GraphVertexGates gate1(Gates::GateDefault, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2 Error Var3;");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateNand) {
  GraphVertexGates gate1(Gates::GateNand, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 );");

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 & Var3 );");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateNor) {
  GraphVertexGates gate1(Gates::GateNor, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 );");

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 | Var3 );");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateNot) {
  GraphVertexGates gate1(Gates::GateNot, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~Var2;");

  // Does it correct eq?
  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~Var3;");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateOr) {
  GraphVertexGates gate1(Gates::GateOr, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2;");

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2 | Var3;");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateXnor) {
  GraphVertexGates gate1(Gates::GateXnor, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 );");

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = ~ ( Var2 ^ Var3 );");
}

TEST(TestToVerilog, ReturnCorrectVerilogStringWhenUseGateXor) {
  GraphVertexGates gate1(Gates::GateXor, "Var1", memoryOwnerGateGr);
  VertexPtr gatePtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var2", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr1);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2;");

  VertexPtr gatePtr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, "Var3", memoryOwnerGateGr);
  gate1.addVertexToInConnections(gatePtr2);
  EXPECT_EQ(gate1.toVerilog(), "assign Var1 = Var2 ^ Var3;");
}

// -------------------------------------

TEST(TestSetName, GatesCorrectName) {
  GraphVertexGates gate(Gates::GateAnd, memoryOwnerGateGr);
  gate.setName("Anything");
  EXPECT_EQ(gate.getRawName(), "Anything");
}

TEST(TestAddInConnections, AddConnections) {
  VertexPtr gate1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  EXPECT_EQ(gate1->getInConnections().size(), 0);

  VertexPtr gate2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  GraphVertexGates *ptr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateOr, memoryOwnerGateGr);
  EXPECT_EQ(gate1->addVertexToInConnections(ptr1), 1);
  EXPECT_EQ(gate1->addVertexToInConnections(ptr1), 2);
  EXPECT_EQ(gate1->getInConnections()[0], ptr1);
  EXPECT_EQ(gate1->getInConnections()[1], ptr1);

  VertexPtr gate3 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  GraphVertexGates *ptr2 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  gate1->addVertexToInConnections(ptr2);
  EXPECT_EQ(gate1->getInConnections()[2], ptr2);
}

TEST(TestAddOutConnections, AddConnections) {
  VertexPtr gate1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  EXPECT_EQ(gate1->getOutConnections().size(), 0);

  GraphVertexGates gate2(Gates::GateAnd, memoryOwnerGateGr);
  VertexPtr ptr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateOr, memoryOwnerGateGr);
  EXPECT_EQ(gate1->addVertexToOutConnections(ptr1), true);
  EXPECT_EQ(gate1->addVertexToOutConnections(ptr1), false);
  EXPECT_EQ(gate1->getOutConnections()[0], ptr1);

  VertexPtr gate3 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  auto ptr2 = memoryOwnerGateGr->create<GraphVertexGates>(Gates::GateAnd,
                                                          memoryOwnerGateGr);
  gate1->addVertexToOutConnections(ptr2);
  EXPECT_EQ(gate1->getOutConnections()[1], ptr2);
}

TEST(TestCalculateHash, SameHashWhenEqualInputs) {
  GraphVertexGates gate1(Gates::GateAnd, memoryOwnerGateGr);
  GraphVertexGates gate2(Gates::GateAnd, memoryOwnerGateGr);
  EXPECT_EQ(gate1.calculateHash(), gate2.calculateHash());

  gate1.addVertexToOutConnections(memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr));
  EXPECT_NE(gate1.calculateHash(true), gate2.calculateHash(true));
  gate2.addVertexToOutConnections(memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr));
  EXPECT_EQ(gate1.calculateHash(true), gate2.calculateHash(true));
}

TEST(TestRemoveVertexToInConnections, RemoveConnections) {
  VertexPtr gatesPtr1 = memoryOwnerGateGr->create<GraphVertexGates>(
      Gates::GateAnd, memoryOwnerGateGr);
  EXPECT_EQ(gatesPtr1->removeVertexToInConnections(nullptr), false);

  gatesPtr1->addVertexToInConnections(
      memoryOwnerGateGr->create<GraphVertexGates>(Gates::GateAnd,
                                                  memoryOwnerGateGr));
  gatesPtr1->addVertexToInConnections(
      memoryOwnerGateGr->create<GraphVertexGates>(Gates::GateAnd,
                                                  memoryOwnerGateGr));
  EXPECT_EQ(gatesPtr1->getInConnections().size(), 2);
  EXPECT_EQ(gatesPtr1->removeVertexToInConnections(nullptr), true);
  EXPECT_EQ(gatesPtr1->getInConnections().size(), 1);
}

// // need to remake realisition of method
// // TEST(TestRemoveVertexToOutConnections, RemoveConnecttions){}
