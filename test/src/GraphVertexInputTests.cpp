#include <sstream>

#include <CircuitGenGraph/GraphVertex.hpp>
#include <gtest/gtest.h>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

using namespace CG_Graph;

/*
  Need to remake RemoveVertexToOutConnections
*/

GraphPtr memoryOwnerInputGr = std::make_shared<OrientedGraph>();

TEST(TestConstructorWithoutIName, WithoutDefaultInputParametrs) {
  GraphVertexBase::resetCounter();
  GraphVertexInput input(memoryOwnerInputGr);
  std::string graphNum = std::to_string(0);
  EXPECT_EQ(input.getType(), VertexTypes::input);
  EXPECT_EQ(input.getTypeName(), "input");
  EXPECT_EQ(input.getRawName(), "input_" + graphNum);
  EXPECT_EQ(input.getLevel(), 0);
  EXPECT_EQ(input.getValue(), 'x');
  EXPECT_EQ(input.getBaseGraph().lock(), memoryOwnerInputGr);
  EXPECT_EQ(input.getOutConnections().size(), 0);
}

TEST(TestConstructorWithoutIName, InputWithDefaultInputParametrs) {
  GraphVertexBase::resetCounter();
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  std::string graphNum = std::to_string(0);
  GraphVertexInput input(graphPtr, VertexTypes::input);
  EXPECT_EQ(input.getType(), VertexTypes::input);
  EXPECT_EQ(input.getTypeName(), "input");
  EXPECT_EQ(input.getRawName(), "input_" + graphNum);
  EXPECT_EQ(input.getLevel(), 0);
  EXPECT_EQ(input.getValue(), 'x');
  EXPECT_EQ(input.getBaseGraph().lock(), graphPtr);
  EXPECT_EQ(input.getOutConnections().size(), 0);
}

TEST(TestConstructorWithIName_Input, InputWithoutDefaultInputParametrs) {
  GraphVertexInput input("Anything", memoryOwnerInputGr);
  EXPECT_EQ(input.getBaseGraph().lock(), memoryOwnerInputGr);
  EXPECT_EQ(input.getType(), VertexTypes::input);
  EXPECT_EQ(input.getTypeName(), "input");
  EXPECT_EQ(input.getRawName(), "Anything");
  EXPECT_EQ(input.getLevel(), 0);
  EXPECT_EQ(input.getValue(), 'x');
  EXPECT_EQ(input.getOutConnections().size(), 0);
}

TEST(TestConstructorWithIName_Input, InputWithDefaultInputParametrs) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  GraphVertexInput input("Anything", graphPtr, VertexTypes::input);
  EXPECT_EQ(input.getType(), VertexTypes::input);
  EXPECT_EQ(input.getTypeName(), "input");
  EXPECT_EQ(input.getRawName(), "Anything");
  EXPECT_EQ(input.getLevel(), 0);
  EXPECT_EQ(input.getValue(), 'x');
  EXPECT_EQ(input.getBaseGraph().lock(), graphPtr);
  EXPECT_EQ(input.getOutConnections().size(), 0);
}
// ------------------------OverrideMethodsTests

// TEST(TestUpdateValue, UpdateValueCorrect) {
//   GraphVertexInput input1(memoryOwnerInputGr);
//   VertexPtr constantPtr1 = memoryOwnerInputGr->addConst('z');
//   input1.addVertexToInConnections(constantPtr1);
//   EXPECT_EQ(input1.getValue(), 'x');
//   EXPECT_EQ(input1.updateValue(), 'z');
//   EXPECT_EQ(input1.getValue(), 'z');

//   VertexPtr constantPtr2 = memoryOwnerInputGr->addConst('z');
//   input1.addVertexToInConnections(constantPtr2);
//   EXPECT_EQ(input1.updateValue(), 'z');

//   VertexPtr constantPtr3 = memoryOwnerInputGr->addConst('1');
//   input1.addVertexToInConnections(constantPtr3);
//   EXPECT_EQ(input1.updateValue(), 'x');
// }

TEST(TestUpdateValue, InputReturnDValueIfDInConnectionsSizeZero) {
  GraphVertexInput input(memoryOwnerInputGr);
  const char c = input.getValue();
  EXPECT_EQ(input.updateValue(), c);
}

// TODO: Update with easylogging
// TEST(TestUpdateValue, CerrErrorIfThereIsBaseGraph) {
//   GraphPtr         graphPtr1 = std::make_shared<OrientedGraph>();
//   GraphVertexInput input1(graphPtr1);

//   input1.addVertexToInConnections(memoryOwnerInputGr->create<GraphVertexInput>());

//   std::stringstream capturedOutput;
//   std::streambuf*   originalStderr = std::cerr.rdbuf(capturedOutput.rdbuf());

//   input1.updateValue();

//   std::cerr.rdbuf(originalStderr);
//   std::string output = capturedOutput.str();
//   EXPECT_EQ(output, "Error\n");
// }

// TEST(TestUpdateValue, ThrowInvalidArgumentIfDInConnectionsZeroIsNullptr) {
//   GraphVertexInput input1(memoryOwnerInputGr);
//   input1.addVertexToInConnections(nullptr);
//   EXPECT_THROW(input1.updateValue(), std::invalid_argument);
// }

// TEST(TestUpdateValue, ThrowInvalidArgumentIfDInconnectionsNIsNullptr) {
//   GraphVertexInput input1(memoryOwnerInputGr);
//   VertexPtr inputPtr1 = memoryOwnerInputGr->addInput();
//   VertexPtr inputPtr2 = memoryOwnerInputGr->addInput();
//   input1.addVertexToInConnections(inputPtr1);
//   input1.addVertexToInConnections(inputPtr2);
//   EXPECT_NO_THROW(input1.updateValue());

//   // input1.addVertexToInConnections(nullptr);
//   // EXPECT_THROW(input1.updateValue(), std::invalid_argument);
// }

TEST(TestUpdateLevel, InputThrowInvalidArgumentIfDInconnectionsNIsNullptr) {
  // Странный тест с подключение input to input
  // GraphVertexInput input1;

  // VertexPtr        inputPtr1 =
  // memoryOwnerInputGr->create<GraphVertexInput>(memoryOwnerInputGr); VertexPtr
  // inputPtr2 =
  // memoryOwnerInputGr->create<GraphVertexInput>(memoryOwnerInputGr);
  // input1.addVertexToInConnections(inputPtr1);
  // input1.addVertexToInConnections(inputPtr2);
  // EXPECT_NO_THROW(input1.updateLevel());

  // input1.addVertexToInConnections(nullptr);
  // EXPECT_THROW(input1.updateLevel(), std::invalid_argument);

  // VertexPtr inputPtr3 =
  // memoryOwnerInputGr->create<GraphVertexInput>(memoryOwnerInputGr);
  // input1.addVertexToInConnections(inputPtr3);
  // EXPECT_THROW(input1.updateLevel(), std::invalid_argument);
}

// -------------------------------------

TEST(TestSetName_Input, InputCorrectName) {
  GraphVertexInput input(memoryOwnerInputGr);
  input.setName("Anything");
  EXPECT_EQ(input.getRawName(), "Anything");
}

// TEST(TestAddInConnections, AddConnections) {
//   GraphVertexInput input1(memoryOwnerInputGr);
//   EXPECT_EQ(input1.getInConnections().size(), 0);

//   VertexPtr inputPtr1 = memoryOwnerInputGr->addInput();
//   EXPECT_EQ(input1.addVertexToInConnections(inputPtr1), 1);
//   EXPECT_EQ(input1.addVertexToInConnections(inputPtr1), 2);
//   EXPECT_EQ(input1.getInConnections()[0], inputPtr1);
//   EXPECT_EQ(input1.getInConnections()[1], inputPtr1);

//   VertexPtr inputPtr2 = memoryOwnerInputGr->addInput();
//   input1.addVertexToInConnections(inputPtr2);
//   EXPECT_EQ(input1.getInConnections()[2], inputPtr2);
// }

TEST(TestAddOutConnections, InputAddConnections) {
  GraphVertexInput input1(memoryOwnerInputGr);
  EXPECT_EQ(input1.getOutConnections().size(), 0);

  VertexPtr inputPtr1 = memoryOwnerInputGr->addInput();
  EXPECT_EQ(input1.addVertexToOutConnections(inputPtr1), true);
  EXPECT_EQ(input1.addVertexToOutConnections(inputPtr1), false);
  EXPECT_EQ(input1.getOutConnections()[0], inputPtr1);

  VertexPtr inputPtr2 = memoryOwnerInputGr->addInput();
  input1.addVertexToOutConnections(inputPtr2);
  EXPECT_EQ(input1.getOutConnections()[1], inputPtr2);
}

TEST(TestCalculateHash_Input, SameHashWhenEqualInputs) {
  GraphVertexInput input1(memoryOwnerInputGr);
  GraphVertexInput input2(memoryOwnerInputGr);
  EXPECT_EQ(input1.calculateHash(), input2.calculateHash());

  input1.addVertexToOutConnections(memoryOwnerInputGr->addOutput());
  input1.resetHashState();
  input2.resetHashState();
  EXPECT_NE(input1.calculateHash(), input2.calculateHash());

  input2.addVertexToOutConnections(memoryOwnerInputGr->addOutput());
  input1.resetHashState();
  input2.resetHashState();
  EXPECT_EQ(input1.calculateHash(), input2.calculateHash());
}

// TEST(TestRemoveVertexToInConnections, RemoveConnections) {
//   VertexPtr inputPtr1 = memoryOwnerInputGr->addInput();
//   EXPECT_EQ(inputPtr1->removeVertexToInConnections(nullptr), false);

//   inputPtr1->addVertexToInConnections(memoryOwnerInputGr->addInput());
//   inputPtr1->addVertexToInConnections(memoryOwnerInputGr->addInput());
//   EXPECT_EQ(inputPtr1->getInConnections().size(), 2);
//   EXPECT_EQ(inputPtr1->removeVertexToInConnections(nullptr), true);
//   EXPECT_EQ(inputPtr1->getInConnections().size(), 1);
// }

// // need to remake realisition of method
// // TEST(TestRemoveVertexToOutConnections, RemoveConnecttions){}