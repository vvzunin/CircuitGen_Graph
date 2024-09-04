#include <CircuitGenGraph/DefaultAuxiliaryMethods.hpp>
#include <CircuitGenGraph/GraphVertex.hpp>
#include <gtest/gtest.h>

#include "easylogging++Init.hpp"

using namespace AuxMethods;

GraphMemory memoryOwnerConst;

TEST(TestConstructorWithoutIName, WithoutDefaultInputParametrsDefName) {
  initLogging(
      "TestConstructorWithoutIName", "WithoutDefaultInputParametrsDefName"
  );
  GraphVertexConstant constant('z', memoryOwnerConst);
  LOG(INFO) << constant;
  std::string graphNum = std::to_string(0);
  EXPECT_EQ(constant.getType(), VertexTypes::constant);
  EXPECT_EQ(constant.getTypeName(), "const");
  EXPECT_EQ(constant.getRawName(), "const_" + graphNum);
  EXPECT_EQ(constant.getLevel(), 0);
  EXPECT_EQ(constant.getValue(), 'z');
  EXPECT_EQ(constant.getBaseGraph().lock(), nullptr);
  EXPECT_EQ(constant.getOutConnections().size(), 0);
}

TEST(TestConstructorWithoutIName, WithDefaultInputParametrsDefName) {
  initLogging(
      "TestConstructorWithoutIName", "WithDefaultInputParametrsDefName"
  );
  GraphPtr            graphPtr = std::make_shared<OrientedGraph>();

  std::string         graphNum = std::to_string(1);
  GraphVertexConstant constant('z', graphPtr);
  EXPECT_EQ(constant.getType(), VertexTypes::constant);
  EXPECT_EQ(constant.getTypeName(), "const");
  EXPECT_EQ(constant.getRawName(), "const_" + graphNum);
  EXPECT_EQ(constant.getLevel(), 0);
  EXPECT_EQ(constant.getValue(), 'z');
  EXPECT_EQ(constant.getBaseGraph().lock(), graphPtr);
  EXPECT_EQ(constant.getOutConnections().size(), 0);
}

TEST(TestConstructorWithIName, WithoutDefaultInputParametrs) {
  initLogging("TestConstructorWithIName", "WithoutDefaultInputParametrs");
  GraphVertexConstant constant('z', "Anything", nullptr);
  EXPECT_EQ(constant.getType(), VertexTypes::constant);
  EXPECT_EQ(constant.getTypeName(), "const");
  EXPECT_EQ(constant.getRawName(), "Anything");
  EXPECT_EQ(constant.getLevel(), 0);
  EXPECT_EQ(constant.getValue(), 'z');
  EXPECT_EQ(constant.getBaseGraph().lock(), nullptr);
  EXPECT_EQ(constant.getOutConnections().size(), 0);
}

TEST(TestConstructorWithIName, WithDefaultInputParametrs) {
  initLogging("TestConstructorWithIName", "WithDefaultInputParametrs");
  GraphPtr            graphPtr = std::make_shared<OrientedGraph>();

  GraphVertexConstant constant('z', "Anything", graphPtr);
  EXPECT_EQ(constant.getType(), VertexTypes::constant);
  EXPECT_EQ(constant.getTypeName(), "const");
  EXPECT_EQ(constant.getRawName(), "Anything");
  EXPECT_EQ(constant.getLevel(), 0);
  EXPECT_EQ(constant.getValue(), 'z');
  EXPECT_EQ(constant.getBaseGraph().lock(), graphPtr);
  EXPECT_EQ(constant.getOutConnections().size(), 0);
}

// ------------------OverrideMethodsTests

// it returns 0 does it correct?
TEST(TestUpdateLevel, CorrectUpdate) {
  initLogging("TestUpdateLevel", "CorrectUpdate");
  GraphVertexConstant constant1('z', memoryOwnerConst);
  constant1.updateLevel();
  EXPECT_EQ(constant1.getLevel(), 0);
}

TEST(TestGetVerilogInstance, ReturnCorrectInstance) {
  initLogging("TestGetVerilogInstance", "ReturnCorrectInstance");
  GraphVertexConstant constant1('z', "Anything", nullptr);
  EXPECT_EQ(constant1.getVerilogInstance(), "wire Anything;");
}

// -------------------------------------

TEST(TestSetName, InputCorrectName) {
  initLogging("TestSetName", "InputCorrectName");
  GraphVertexConstant constant('z', memoryOwnerConst);
  constant.setName("Anything");
  EXPECT_EQ(constant.getRawName(), "Anything");
}

TEST(TestAddVertexToInConnections, AddConnections1) {
  initLogging("TestAddVertexToInConnections", "AddConnections1");
  VertexPtr constant1 =
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst);
  EXPECT_EQ(constant1->getInConnections().size(), 0);

  VertexPtr constant2 =
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst);
  VertexPtr ptr1 =
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst);
  ptr1->addVertexToInConnections(constant2);
  EXPECT_EQ(constant1->addVertexToInConnections(ptr1), 1);
  EXPECT_EQ(constant1->addVertexToInConnections(ptr1), 2);
  EXPECT_EQ(constant1->getInConnections()[0].lock(), ptr1);
  EXPECT_EQ(constant1->getInConnections()[1].lock(), ptr1);

  VertexPtr constant3 =
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst);
  VertexPtr ptr2 =
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst);
  constant1->addVertexToInConnections(ptr2);
  EXPECT_EQ(constant1->getInConnections()[2].lock(), ptr2);
}

TEST(TestAddVertexToOutConnections, AddConnections2) {
  initLogging("TestAddVertexToOutConnections", "AddConnections2");
  VertexPtr constant1 =
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst);
  EXPECT_EQ(constant1->getOutConnections().size(), 0);

  VertexPtr constant2 =
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst);
  VertexPtr ptr1 =
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst);
  EXPECT_EQ(constant1->addVertexToOutConnections(ptr1), true);
  EXPECT_EQ(constant1->addVertexToOutConnections(ptr1), false);
  EXPECT_EQ(constant1->getOutConnections()[0], ptr1);

  VertexPtr constant3 =
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst);
  VertexPtr ptr2 =
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst);
  constant1->addVertexToOutConnections(ptr2);
  EXPECT_EQ(constant1->getOutConnections()[1], ptr2);
}

TEST(TestCalculateHash, SameHashWhenEqualInputs) {
  initLogging("TestCalculateHash", "SameHashWhenEqualInputs");
  GraphVertexConstant constant1('z', memoryOwnerConst);
  GraphVertexConstant constant2('z', memoryOwnerConst);
  EXPECT_EQ(constant1.calculateHash(), constant2.calculateHash());

  constant1.addVertexToOutConnections(
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst)
  );
  EXPECT_NE(constant1.calculateHash(true), constant2.calculateHash(true));
  constant2.addVertexToOutConnections(
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst)
  );
  EXPECT_EQ(constant1.calculateHash(true), constant2.calculateHash(true));
}
TEST(TestCalculateHash, SameGraphDifferentValues) {
  initLogging("TestCalculateHash", "SameHashWhenEqualInputs");
  GraphVertexConstant constant1('z', memoryOwnerConst);
  GraphVertexConstant constant2('x', memoryOwnerConst);
  EXPECT_NE(constant1.calculateHash(), constant2.calculateHash());

  constant1.addVertexToOutConnections(
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst)
  );
  EXPECT_NE(constant1.calculateHash(true), constant2.calculateHash(true));
  constant2.addVertexToOutConnections(
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst)
  );
  EXPECT_NE(constant1.calculateHash(true), constant2.calculateHash(true));
}

TEST(TestRemoveVertexToInConnections, RemoveConnections) {
  initLogging("TestRemoveVertexToInConnections", "RemoveConnections");
  VertexPtr vertexPtr1 =
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst);
  EXPECT_EQ(vertexPtr1->removeVertexToInConnections(vertexPtr1), false);
  vertexPtr1->addVertexToInConnections(
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst)
  );
  vertexPtr1->addVertexToInConnections(
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst)
  );
  EXPECT_EQ(vertexPtr1->getInConnections().size(), 2);
  EXPECT_EQ(vertexPtr1->removeVertexToInConnections(nullptr), true);
  EXPECT_EQ(vertexPtr1->getInConnections().size(), 1);
}

TEST(TestToDOT, CheckName) {
  initLogging("TestToDOT", "CheckName");
  VertexPtr vertexPtr1 =
      std::make_shared<GraphVertexConstant>('0', memoryOwnerConst);
  VertexPtr vertexPtr2 =
      std::make_shared<GraphVertexConstant>('1', memoryOwnerConst);
  VertexPtr vertexPtr3 =
      std::make_shared<GraphVertexConstant>('x', memoryOwnerConst);
  VertexPtr vertexPtr4 =
      std::make_shared<GraphVertexConstant>('z', memoryOwnerConst);
  EXPECT_EQ(
      dotReturnToString(vertexPtr1->toDOT()),
      vertexPtr1->getName() + " [shape=cds, label=\""
          + vertexPtr1->getName() + "\\n1'b0\"];\n"
  );
  EXPECT_EQ(
      dotReturnToString(vertexPtr2->toDOT()),
      vertexPtr2->getName() + " [shape=cds, label=\""
          + vertexPtr2->getName() + "\\n1'b1\"];\n"
  );
  EXPECT_EQ(
      dotReturnToString(vertexPtr3->toDOT()),
      vertexPtr3->getName() + " [shape=cds, label=\""
          + vertexPtr3->getName() + "\\n1'bx\"];\n"
  );
  EXPECT_EQ(
      dotReturnToString(vertexPtr4->toDOT()),
      vertexPtr4->getName() + " [shape=cds, label=\""
          + vertexPtr4->getName() + "\\n1'bz\"];\n"
  );
}

// need to remake realisition of method
// TEST(TestRemoveVertexToOutConnections, RemoveConnecttions){}
