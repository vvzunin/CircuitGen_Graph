#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <gtest/gtest.h>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

using namespace CG_Graph;

/*
  Need to remake RemoveVertexToOutConnections
*/

GraphPtr memoryOwnerOutputGr = std::make_shared<OrientedGraph>();

TEST(TestConstructorWithoutIName, OutputWithoutDefaultInputParametrs) {
  GraphVertexOutput output(memoryOwnerOutputGr);
  std::string graphNum = std::to_string(0);
  EXPECT_EQ(output.getType(), VertexTypes::output);
  EXPECT_EQ(output.getTypeName(), "output");
  EXPECT_EQ(output.getRawName(), "output_" + graphNum);
  EXPECT_EQ(output.getLevel(), 0);
  EXPECT_EQ(output.getValue(), 'x');
  EXPECT_EQ(output.getBaseGraph().lock(), memoryOwnerOutputGr);
  EXPECT_EQ(output.getOutConnections().size(), 0);
}

TEST(TestConstructorWithoutIName, OutputWithDefaultInputParametrs) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  std::string graphNum = std::to_string(0);
  GraphVertexOutput output(graphPtr);
  EXPECT_EQ(output.getType(), VertexTypes::output);
  EXPECT_EQ(output.getTypeName(), "output");
  EXPECT_EQ(output.getRawName(), "output_" + graphNum);
  EXPECT_EQ(output.getLevel(), 0);
  EXPECT_EQ(output.getValue(), 'x');
  EXPECT_EQ(output.getBaseGraph().lock(), graphPtr);
  EXPECT_EQ(output.getOutConnections().size(), 0);
}

TEST(TestConstructorWithIName_Output, OutputWithoutDefaultInputParametrs) {
  GraphVertexOutput output("Anything", memoryOwnerOutputGr);
  EXPECT_EQ(output.getBaseGraph().lock(), memoryOwnerOutputGr);
  EXPECT_EQ(output.getType(), VertexTypes::output);
  EXPECT_EQ(output.getTypeName(), "output");
  EXPECT_EQ(output.getRawName(), "Anything");
  EXPECT_EQ(output.getLevel(), 0);
  EXPECT_EQ(output.getValue(), 'x');
  EXPECT_EQ(output.getOutConnections().size(), 0);
}

TEST(TestConstructorWithIName_Output, OutputWithDefaultInputParametrs) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  GraphVertexOutput output("Anything", graphPtr);
  EXPECT_EQ(output.getType(), VertexTypes::output);
  EXPECT_EQ(output.getTypeName(), "output");
  EXPECT_EQ(output.getRawName(), "Anything");
  EXPECT_EQ(output.getLevel(), 0);
  EXPECT_EQ(output.getValue(), 'x');
  EXPECT_EQ(output.getBaseGraph().lock(), graphPtr);
  EXPECT_EQ(output.getOutConnections().size(), 0);
}
// ------------------------OverrideMethodsTests

TEST(TestUpdateValue, UpdateValueCorrect) {
  GraphVertexOutput output1(memoryOwnerOutputGr);
  VertexPtr constantPtr1 = memoryOwnerOutputGr->addConst('z');
  output1.addVertexToInConnections(constantPtr1);
  EXPECT_EQ(output1.getValue(), 'x');
  EXPECT_EQ(output1.updateValue(), 'z');
  EXPECT_EQ(output1.getValue(), 'z');

  VertexPtr constantPtr2 = memoryOwnerOutputGr->addConst('z');
  output1.addVertexToInConnections(constantPtr2);
  EXPECT_EQ(output1.updateValue(), 'z');

  VertexPtr constantPtr3 = memoryOwnerOutputGr->addConst('1');
  output1.addVertexToInConnections(constantPtr3);
  EXPECT_EQ(output1.updateValue(), 'x');
}

TEST(TestUpdateValue, OutputReturnDValueIfDInConnectionsSizeZero) {
  GraphVertexOutput output(memoryOwnerOutputGr);
  const char c = output.getValue();
  EXPECT_EQ(output.updateValue(), c);
}

// TEST(TestUpdateValue, ThrowInvalidArgumentIfDInConnectionsZeroIsNullptr) {
//   GraphVertexOutput output1(memoryOwnerOutputGr);
//   output1.addVertexToInConnections(nullptr);
//   EXPECT_THROW(output1.updateValue(), std::invalid_argument);
// }

// TEST(TestUpdateValue, ThrowInvalidArgumentIfDInconnectionsNIzNullptr) {
//   GraphVertexInput output1(memoryOwnerOutputGr);
//   VertexPtr outputPtr1 = memoryOwnerOutputGr->addOutput();
//   VertexPtr outputPtr2 = memoryOwnerOutputGr->addOutput();
//   output1.addVertexToInConnections(outputPtr1);
//   output1.addVertexToInConnections(outputPtr2);
//   EXPECT_NO_THROW(output1.updateValue());

//   // output1.addVertexToInConnections(nullptr);
//   // EXPECT_THROW(output1.updateValue(), std::invalid_argument);
// }

TEST(TestUpdateLevel, OutputCorrectUpdate) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr inputVert = graph->addInput();
  VertexPtr gateVert = graph->addGate(GateAnd);
  VertexPtr outputVert = graph->addOutput();
  graph->addEdge(inputVert, gateVert);
  graph->addEdge(gateVert, outputVert);
  graph->updateLevels();
  EXPECT_EQ(outputVert->getLevel(), 2);
}

TEST(TestUpdateLevel, ThrowInvalidArgumentIfDInconnectionsNIzNullptr) {
  GraphVertexOutput output1(memoryOwnerOutputGr);

  VertexPtr outputPtr1 = memoryOwnerOutputGr->addOutput();
  VertexPtr outputPtr2 = memoryOwnerOutputGr->addOutput();
  output1.addVertexToInConnections(outputPtr1);
  output1.addVertexToInConnections(outputPtr2);
  EXPECT_NO_THROW(output1.updateLevel());

  // output1.addVertexToInConnections(nullptr);
  // EXPECT_THROW(output1.updateLevel(true), std::invalid_argument);

  VertexPtr outputPtr3 = memoryOwnerOutputGr->addOutput();
  output1.addVertexToInConnections(outputPtr3);
  // EXPECT_THROW(output1.updateLevel(true), std::invalid_argument);
}

// -------------------------------------

TEST(TestSetName_Output, InputCorrectName) {
  GraphVertexOutput output(memoryOwnerOutputGr);
  output.setName("Anything");
  EXPECT_EQ(output.getRawName(), "Anything");
}

TEST(TestAddInConnections, OutputAddConnections) {
  GraphVertexOutput output1(memoryOwnerOutputGr);
  EXPECT_EQ(output1.getInConnections().size(), 0);

  VertexPtr outputPtr1 = memoryOwnerOutputGr->addOutput();
  EXPECT_EQ(output1.addVertexToInConnections(outputPtr1), 1);
  EXPECT_EQ(output1.addVertexToInConnections(outputPtr1), 2);
  EXPECT_EQ(output1.getInConnections()[0], outputPtr1);
  EXPECT_EQ(output1.getInConnections()[1], outputPtr1);

  VertexPtr outputPtr2 = memoryOwnerOutputGr->addOutput();
  output1.addVertexToInConnections(outputPtr2);
  EXPECT_EQ(output1.getInConnections()[2], outputPtr2);
}

// TEST(TestAddOutConnections, AddConnections) {
//   GraphVertexOutput output1(memoryOwnerOutputGr);
//   EXPECT_EQ(output1.getOutConnections().size(), 0);

//   VertexPtr outputPtr1 = memoryOwnerOutputGr->addOutput();
//   EXPECT_EQ(output1.addVertexToOutConnections(outputPtr1), true);
//   EXPECT_EQ(output1.addVertexToOutConnections(outputPtr1), false);
//   EXPECT_EQ(output1.getOutConnections()[0], outputPtr1);

//   VertexPtr outputPtr2 = memoryOwnerOutputGr->addOutput();
//   output1.addVertexToOutConnections(outputPtr2);
//   EXPECT_EQ(output1.getOutConnections()[1], outputPtr2);
// }

TEST(TestCalculateHash_Output, SameHashWhenEqualInputs) {
  GraphVertexOutput output1(memoryOwnerOutputGr);
  EXPECT_NE(output1.calculateHash(), 0);

  output1.addVertexToInConnections(memoryOwnerOutputGr->addInput());
  output1.resetHashState();
  EXPECT_NE(output1.calculateHash(), 0);

  GraphPtr graphPtr1 = std::make_shared<OrientedGraph>();
  GraphVertexInput output2(graphPtr1);
  GraphVertexInput output3(graphPtr1);

  output2.addVertexToOutConnections(memoryOwnerOutputGr->addOutput());
  output2.resetHashState();
  output3.resetHashState();
  EXPECT_NE(output2.calculateHash(), output3.calculateHash());

  output3.addVertexToOutConnections(memoryOwnerOutputGr->addOutput());
  output2.resetHashState();
  output3.resetHashState();
  EXPECT_EQ(output2.calculateHash(), output3.calculateHash());
}

// need to remake realisition of method
// TEST(TestRemoveVertexToOutConnections, RemoveConnecttions){}

TEST(TestIsConnected, SizeZeroAndOneIsConnected) {
  GraphPtr graphPtr1 = std::make_shared<OrientedGraph>();
  GraphPtr graphPtr2 = std::make_shared<OrientedGraph>();
  graphPtr2->addGate(Gates::GateAnd, "and");

  EXPECT_EQ(graphPtr1->isConnected(), true);
  EXPECT_EQ(graphPtr2->isConnected(), true);
}

TEST(TestIsConnected, SizeTwoWithoutEdgesIsNotConnected) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  graphPtr->addInput("input");
  graphPtr->addOutput("output");

  EXPECT_EQ(graphPtr->isConnected(), false);
}

TEST(TestIsConnected, SizeTwoWithEdgeIsConnected) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  VertexPtr input = graphPtr->addInput("input");
  VertexPtr output = graphPtr->addOutput("output");
  graphPtr->addEdge(input, output);

  EXPECT_EQ(graphPtr->isConnected(), true);
}

TEST(TestIsConnectedWithSubGraphsTrivial, ConnectedSubGraphIsConnected) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();

  std::vector<VertexPtr> inputs;
  VertexPtr input = graphPtr->addInput("input");
  VertexPtr output = graphPtr->addOutput("output");
  inputs.push_back(input);

  GraphPtr subGraphPtr = std::make_shared<OrientedGraph>();

  VertexPtr subGraphInput = subGraphPtr->addInput("subGraphInput");
  VertexPtr testBuf = subGraphPtr->addGate(Gates::GateBuf);
  VertexPtr subGraphOutput = subGraphPtr->addOutput("subGraphOutput");

  subGraphPtr->addEdge(subGraphInput, testBuf);
  subGraphPtr->addEdge(testBuf, subGraphOutput);

  auto subGraphOutput2 = graphPtr->addSubGraph(subGraphPtr, inputs).back();
  auto inv = graphPtr->addGate(GateNot);

  graphPtr->addEdge(subGraphOutput2, inv);
  graphPtr->addEdge(inv, output);

  EXPECT_EQ(graphPtr->isConnected(), true);
}

TEST(TestIsConnectedWithSubGraphsTrivial, DisconnectedSubGraphIsNotConnected) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  graphPtr->addInput("input");

  GraphPtr subGraphPtr = std::make_shared<OrientedGraph>();
  subGraphPtr->addInput("subGraphInput");
  subGraphPtr->addOutput("subGraphOutput");

  graphPtr->addSubGraph(subGraphPtr,
                        graphPtr->getVerticesByType(VertexTypes::input));

  EXPECT_EQ(graphPtr->isConnected(), false);
}

TEST(TestIsConnectedWithSubGraphsNontrivial,
     DisconnectedSubGraphLvl2IsNotConnected) {
  GraphPtr lvl0 = std::make_shared<OrientedGraph>();
  GraphPtr lvl1 = std::make_shared<OrientedGraph>();
  GraphPtr lvl2 = std::make_shared<OrientedGraph>();

  VertexPtr lvl2Input1 = lvl2->addInput("lvl2Input1");
  VertexPtr lvl2Output1 = lvl2->addOutput("lvl2Output1");
  VertexPtr lvl2Input2 = lvl2->addInput("lvl2Input2");
  VertexPtr lvl2Output2 = lvl2->addOutput("lvl2Output2");
  lvl2->addEdge(lvl2Input1, lvl2Output2);
  lvl2->addEdge(lvl2Input2, lvl2Output1);

  lvl1->addInput("lvl1Input1");
  VertexPtr lvl1Output1 = lvl1->addOutput("lvl1Output1");
  lvl1->addInput("lvl1Input2");
  VertexPtr lvl1Output2 = lvl1->addOutput("lvl1Output2");
  auto bufsLvl1 =
      lvl1->addSubGraph(lvl2, lvl1->getVerticesByType(VertexTypes::input));
  lvl1->addEdge(bufsLvl1[0], lvl1Output1);
  lvl1->addEdge(bufsLvl1[1], lvl1Output2);

  lvl0->addInput("lvl0Input1");
  VertexPtr lvl0Output1 = lvl0->addOutput("lvl0Output1");
  lvl0->addInput("lvl0Input2");
  VertexPtr lvl0Output2 = lvl0->addOutput("lvl0Output2");
  auto bufsLvl0 =
      lvl0->addSubGraph(lvl1, lvl0->getVerticesByType(VertexTypes::input));
  lvl0->addEdge(bufsLvl0[0], lvl0Output1);
  lvl0->addEdge(bufsLvl0[1], lvl0Output2);

  EXPECT_EQ(lvl2->isConnected(), false);
  EXPECT_EQ(lvl1->isConnected(), false);
  EXPECT_EQ(lvl0->isConnected(), false);
}

TEST(TestIsConnectedWithSubGraphsNontrivial, ConnectedSubGraphLvl2IsConnected) {
  GraphPtr lvl0 = std::make_shared<OrientedGraph>();
  GraphPtr lvl1 = std::make_shared<OrientedGraph>();
  GraphPtr lvl2 = std::make_shared<OrientedGraph>();

  VertexPtr lvl2Input1 = lvl2->addInput("lvl2Input1");
  VertexPtr lvl2Output1 = lvl2->addOutput("lvl2Output1");
  VertexPtr lvl2Input2 = lvl2->addInput("lvl2Input2");
  VertexPtr lvl2Output2 = lvl2->addOutput("lvl2Output2");
  lvl2->addEdge(lvl2Input1, lvl2Output2);
  lvl2->addEdge(lvl2Input2, lvl2Output2); // <-- different from upper test
  lvl2->addEdge(lvl2Input2, lvl2Output1);

  lvl1->addInput("lvl1Input1");
  VertexPtr lvl1Output1 = lvl1->addOutput("lvl1Output1");
  lvl1->addInput("lvl1Input2");
  VertexPtr lvl1Output2 = lvl1->addOutput("lvl1Output2");
  auto bufsLvl1 =
      lvl1->addSubGraph(lvl2, lvl1->getVerticesByType(VertexTypes::input));
  lvl1->addEdge(bufsLvl1[0], lvl1Output1);
  lvl1->addEdge(bufsLvl1[1], lvl1Output2);

  lvl0->addInput("lvl0Input1");
  VertexPtr lvl0Output1 = lvl0->addOutput("lvl0Output1");
  lvl0->addInput("lvl0Input2");
  VertexPtr lvl0Output2 = lvl0->addOutput("lvl0Output2");
  auto bufsLvl0 =
      lvl0->addSubGraph(lvl1, lvl0->getVerticesByType(VertexTypes::input));
  lvl0->addEdge(bufsLvl0[0], lvl0Output1);
  lvl0->addEdge(bufsLvl0[1], lvl0Output2);

  EXPECT_EQ(lvl2->isConnected(), true);
  EXPECT_EQ(lvl1->isConnected(), true);
  EXPECT_EQ(lvl0->isConnected(), true);
}

TEST(TestIsConnectedWithSubGraphsNontrivial,
     ConnectedSubGraphLvl1IsConnectedStraight) {
  GraphPtr lvl0 = std::make_shared<OrientedGraph>();
  GraphPtr lvl1 = std::make_shared<OrientedGraph>();
  GraphPtr lvl2 = std::make_shared<OrientedGraph>();

  VertexPtr lvl2Input1 = lvl2->addInput("lvl2Input1");
  VertexPtr lvl2Output1 = lvl2->addOutput("lvl2Output1");
  VertexPtr lvl2Input2 = lvl2->addInput("lvl2Input2");
  VertexPtr lvl2Output2 = lvl2->addOutput("lvl2Output2");
  lvl2->addEdge(lvl2Input1, lvl2Output2);
  lvl2->addEdge(lvl2Input2, lvl2Output1);

  lvl1->addInput("lvl1Input1");
  VertexPtr lvl1Output1 = lvl1->addOutput("lvl1Output1");
  VertexPtr lvl1Input2 = lvl1->addInput("lvl1Input2");
  VertexPtr lvl1Output2 = lvl1->addOutput("lvl1Output2");
  VertexPtr lvl1And = lvl1->addGate(Gates::GateAnd, "lvl1And");
  auto bufsLvl1 =
      lvl1->addSubGraph(lvl2, lvl1->getVerticesByType(VertexTypes::input));
  lvl1->addEdge(bufsLvl1[0], lvl1Output1);
  lvl1->addEdge(bufsLvl1[1], lvl1And);
  lvl1->addEdge(lvl1Input2, lvl1And);
  lvl1->addEdge(lvl1And, lvl1Output2);

  lvl0->addInput("lvl0Input1");
  VertexPtr lvl0Output1 = lvl0->addOutput("lvl0Output1");
  lvl0->addInput("lvl0Input2");
  VertexPtr lvl0Output2 = lvl0->addOutput("lvl0Output2");
  auto bufsLvl0 =
      lvl0->addSubGraph(lvl1, lvl0->getVerticesByType(VertexTypes::input));
  lvl0->addEdge(bufsLvl0[0], lvl0Output1);
  lvl0->addEdge(bufsLvl0[1], lvl0Output2);

  EXPECT_EQ(lvl2->isConnected(), false);
  EXPECT_EQ(lvl1->isConnected(), true);
  EXPECT_EQ(lvl0->isConnected(), true);
}

TEST(TestIsConnectedWithSubGraphsNontrivial,
     ConnectedSubGraphLvl1IsConnectedReverse) {
  GraphPtr lvl0 = std::make_shared<OrientedGraph>();
  GraphPtr lvl1 = std::make_shared<OrientedGraph>();
  GraphPtr lvl2 = std::make_shared<OrientedGraph>();

  VertexPtr lvl2Input1 = lvl2->addInput("lvl2Input1");
  VertexPtr lvl2Output1 = lvl2->addOutput("lvl2Output1");
  VertexPtr lvl2Input2 = lvl2->addInput("lvl2Input2");
  VertexPtr lvl2Output2 = lvl2->addOutput("lvl2Output2");
  lvl2->addEdge(lvl2Input1, lvl2Output2);
  lvl2->addEdge(lvl2Input2, lvl2Output1);

  lvl1->addInput("lvl1Input2"); // <-- different from upper test
  VertexPtr lvl1Input1 = lvl1->addInput("lvl1Input1");
  VertexPtr lvl1Output1 = lvl1->addOutput("lvl1Output1");
  VertexPtr lvl1Output2 = lvl1->addOutput("lvl1Output2");
  VertexPtr lvl1And = lvl1->addGate(Gates::GateAnd, "lvl1And");
  auto bufsLvl1 =
      lvl1->addSubGraph(lvl2, lvl1->getVerticesByType(VertexTypes::input));
  lvl1->addEdge(bufsLvl1[0], lvl1Output1);
  lvl1->addEdge(bufsLvl1[1], lvl1And);
  lvl1->addEdge(lvl1Input1, lvl1And); // <-- different from upper test
  lvl1->addEdge(lvl1And, lvl1Output2);

  lvl0->addInput("lvl0Input1");
  VertexPtr lvl0Output1 = lvl0->addOutput("lvl0Output1");
  lvl0->addInput("lvl0Input2");
  VertexPtr lvl0Output2 = lvl0->addOutput("lvl0Output2");
  auto bufsLvl0 =
      lvl0->addSubGraph(lvl1, lvl0->getVerticesByType(VertexTypes::input));
  lvl0->addEdge(bufsLvl0[0], lvl0Output1);
  lvl0->addEdge(bufsLvl0[1], lvl0Output2);

  EXPECT_EQ(lvl2->isConnected(), false);
  EXPECT_EQ(lvl1->isConnected(), true);
  EXPECT_EQ(lvl0->isConnected(), true);
}

TEST(TestIsConnectedWithSubGraphsNontrivial,
     ConnectedSubGraphLvl0IsConnectedStraight) {
  GraphPtr lvl0 = std::make_shared<OrientedGraph>();
  GraphPtr lvl1 = std::make_shared<OrientedGraph>();
  GraphPtr lvl2 = std::make_shared<OrientedGraph>();

  VertexPtr lvl2Input1 = lvl2->addInput("lvl2Input1");
  VertexPtr lvl2Output1 = lvl2->addOutput("lvl2Output1");
  VertexPtr lvl2Input2 = lvl2->addInput("lvl2Input2");
  VertexPtr lvl2Output2 = lvl2->addOutput("lvl2Output2");
  lvl2->addEdge(lvl2Input1, lvl2Output2);
  lvl2->addEdge(lvl2Input2, lvl2Output1);

  lvl1->addInput("lvl1Input1");
  VertexPtr lvl1Output1 = lvl1->addOutput("lvl1Output1");
  lvl1->addInput("lvl1Input2");
  VertexPtr lvl1Output2 = lvl1->addOutput("lvl1Output2");
  auto bufsLvl1 =
      lvl1->addSubGraph(lvl2, lvl1->getVerticesByType(VertexTypes::input));
  lvl1->addEdge(bufsLvl1[0], lvl1Output1);
  lvl1->addEdge(bufsLvl1[1], lvl1Output2);

  lvl0->addInput("lvl0Input1");
  VertexPtr lvl0Output1 = lvl0->addOutput("lvl0Output1");
  VertexPtr lvl0Input2 = lvl0->addInput("lvl0Input2");
  VertexPtr lvl0Output2 = lvl0->addOutput("lvl0Output2");
  VertexPtr lvl0And = lvl0->addGate(Gates::GateAnd, "lvl0And");
  auto bufsLvl0 =
      lvl0->addSubGraph(lvl1, lvl0->getVerticesByType(VertexTypes::input));
  lvl0->addEdge(bufsLvl0[0], lvl0Output1);
  lvl0->addEdge(bufsLvl0[1], lvl0And);
  lvl0->addEdge(lvl0Input2, lvl0And);
  lvl0->addEdge(lvl0And, lvl0Output2);

  EXPECT_EQ(lvl2->isConnected(), false);
  EXPECT_EQ(lvl1->isConnected(), false);
  EXPECT_EQ(lvl0->isConnected(), true);
}

TEST(TestIsConnectedWithSubGraphsNontrivial,
     ConnectedSubGraphLvl0IsConnectedReverse) {
  GraphPtr lvl0 = std::make_shared<OrientedGraph>();
  GraphPtr lvl1 = std::make_shared<OrientedGraph>();
  GraphPtr lvl2 = std::make_shared<OrientedGraph>();

  VertexPtr lvl2Input1 = lvl2->addInput("lvl2Input1");
  VertexPtr lvl2Output1 = lvl2->addOutput("lvl2Output1");
  VertexPtr lvl2Input2 = lvl2->addInput("lvl2Input2");
  VertexPtr lvl2Output2 = lvl2->addOutput("lvl2Output2");
  lvl2->addEdge(lvl2Input1, lvl2Output2);
  lvl2->addEdge(lvl2Input2, lvl2Output1);

  lvl1->addInput("lvl1Input1");
  VertexPtr lvl1Output1 = lvl1->addOutput("lvl1Output1");
  lvl1->addInput("lvl1Input2");
  VertexPtr lvl1Output2 = lvl1->addOutput("lvl1Output2");
  auto bufsLvl1 =
      lvl1->addSubGraph(lvl2, lvl1->getVerticesByType(VertexTypes::input));
  lvl1->addEdge(bufsLvl1[0], lvl1Output1);
  lvl1->addEdge(bufsLvl1[1], lvl1Output2);

  lvl0->addInput("lvl0Input2"); // <-- different from upper test
  VertexPtr lvl0Input1 = lvl0->addInput("lvl0Input1");
  VertexPtr lvl0Output1 = lvl0->addOutput("lvl0Output1");
  VertexPtr lvl0Output2 = lvl0->addOutput("lvl0Output2");
  VertexPtr lvl0And = lvl0->addGate(Gates::GateAnd, "lvl0And");
  auto bufsLvl0 =
      lvl0->addSubGraph(lvl1, lvl0->getVerticesByType(VertexTypes::input));
  lvl0->addEdge(bufsLvl0[0], lvl0Output1);
  lvl0->addEdge(bufsLvl0[1], lvl0And);
  lvl0->addEdge(lvl0Input1, lvl0And); // <-- different from upper test
  lvl0->addEdge(lvl0And, lvl0Output2);

  EXPECT_EQ(lvl2->isConnected(), false);
  EXPECT_EQ(lvl1->isConnected(), false);
  EXPECT_EQ(lvl0->isConnected(), true);
}

TEST(TestIsConnectedRecalculation, SimpleRecalculation) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  VertexPtr input = graphPtr->addInput("input");
  VertexPtr output = graphPtr->addOutput("output");
  EXPECT_EQ(graphPtr->isConnected(), false);

  graphPtr->addEdge(input, output);
  EXPECT_EQ(graphPtr->isConnected(), false);
  EXPECT_EQ(graphPtr->isConnected(true), true);

  graphPtr->addGate(Gates::GateAnd, "and");
  EXPECT_EQ(graphPtr->isConnected(), true);
  EXPECT_EQ(graphPtr->isConnected(true), false);
}