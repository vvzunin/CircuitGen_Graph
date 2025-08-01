#include "CircuitGenGraph/GraphUtils.hpp"
#include <gtest/gtest.h>
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <lorina/lorina.hpp>
#include <memory>
using namespace CG_Graph;
TEST(VerilogReadingTest, SimplestGraphsIsRead) {
  Context context = Context();
  EXPECT_NO_THROW(
      OrientedGraph::readVerilog("../../../test/"
                                 "testModulesForReading/simpleReading.v",
                                 context));
  GraphPtr graph = context.d_graphs["simpleReading"];
  EXPECT_EQ(graph->getGatesCount()[GateAnd], 1);
  GraphPtr graphCreated = std::make_shared<OrientedGraph>();
  VertexPtr clk = graphCreated->addInput();
  VertexPtr someInput = graphCreated->addInput();
  VertexPtr gate = graphCreated->addGate(GateAnd);
  VertexPtr output = graphCreated->addOutput();
  graphCreated->addEdge(clk, gate);
  graphCreated->addEdge(someInput, gate);
  graphCreated->addEdge(gate, output);
  EXPECT_EQ(graph->calculateHash(), graphCreated->calculateHash());
}

TEST(VerilogReadingTest, VerticesSequenceCreated) {
  Context context = Context();
  EXPECT_NO_THROW(
      OrientedGraph::readVerilog("../../../test/"
                                 "testModulesForReading/simpleStructure.v",
                                 context));
  GraphPtr graph = context.d_graphs["simpleStructure"];
  EXPECT_EQ(graph->getGatesCount()[GateNot], 1);
  EXPECT_EQ(graph->getGatesCount()[GateOr], 1);
  EXPECT_EQ(graph->getGatesCount()[GateXnor], 1);
  GraphPtr graphCreated = std::make_shared<OrientedGraph>();
  VertexPtr inputA = graphCreated->addInput();
  VertexPtr inputB = graphCreated->addInput();
  VertexPtr inputC = graphCreated->addInput();
  VertexPtr gateXnor = graphCreated->addGate(GateXnor);
  VertexPtr gateOr = graphCreated->addGate(GateOr);
  VertexPtr gateAnd = graphCreated->addGate(GateAnd);
  VertexPtr gateNot = graphCreated->addGate(GateNot);
  VertexPtr outputQ = graphCreated->addOutput();
  VertexPtr outputQ2 = graphCreated->addOutput();
  graphCreated->addEdge(inputB, gateNot);
  graphCreated->addEdge(inputA, gateOr);
  graphCreated->addEdge(gateNot, gateOr);
  graphCreated->addEdge(inputC, gateOr);
  graphCreated->addEdge(gateOr, gateXnor);
  graphCreated->addEdge(inputA, gateXnor);
  graphCreated->addEdge(gateXnor, gateAnd);
  graphCreated->addEdge(gateOr, gateAnd);
  graphCreated->addEdge(gateAnd, outputQ);
  graphCreated->addEdge(gateNot, outputQ2);
  EXPECT_EQ(graph->calculateHash(), graphCreated->calculateHash());
}
TEST(VerilogReadingTest, ConstantCreating) {
  GraphPtr graphCreated = std::make_shared<OrientedGraph>();
  VertexPtr constOne = graphCreated->addConst('1');
  VertexPtr constZero = graphCreated->addConst('0');
  VertexPtr input = graphCreated->addInput();
  VertexPtr nand = graphCreated->addGate(GateNand);
  VertexPtr output = graphCreated->addOutput();
  VertexPtr buf = graphCreated->addGate(GateBuf);
  VertexPtr xnor = graphCreated->addGate(GateXnor);
  graphCreated->addEdge(constOne, nand);
  graphCreated->addEdge(input, nand);
  graphCreated->addEdge(constZero, buf);
  graphCreated->addEdge(buf, xnor);
  graphCreated->addEdge(nand, xnor);
  graphCreated->addEdge(xnor, output);
  graphCreated->setName("constTest");
  graphCreated->toVerilog("../../../test/"
                          "testModulesForReading",
                          "constTest.v");
  Context context = Context();
  EXPECT_NO_THROW(
      OrientedGraph::readVerilog("../../../test/"
                                 "testModulesForReading/constTest.v",
                                 context));
  GraphPtr graph = context.d_graphs["constTest"];
  EXPECT_EQ(graph->calculateHash(), graphCreated->calculateHash());
}

TEST(VerilogReadingTest, OneContextForSeveralGraphs) {
  Context context = Context();
  EXPECT_NO_THROW(
      OrientedGraph::readVerilog("../../../test/"
                                 "testModulesForReading/simpleStructure.v",
                                 context));
  EXPECT_NO_THROW(
      OrientedGraph::readVerilog("../../../test/"
                                 "testModulesForReading/simpleReading.v",
                                 context));
  EXPECT_NO_THROW(context.d_graphs["simpleStructure"]);
  EXPECT_NO_THROW(context.d_graphs["simpleReading"]);
}
TEST(VerilogReadingTest, UseOutputAsGate) {
  Context context = Context();
  EXPECT_NO_THROW(
      OrientedGraph::readVerilog("../../../test/"
                                 "testModulesForReading/outputAsGate.v",
                                 context));
  GraphPtr graph = context.d_graphs["outputAsGate"];
  EXPECT_EQ(graph->getGatesCount()[GateAnd], 1);
  GraphPtr graphCreated = std::make_shared<OrientedGraph>();
  VertexPtr clk = graphCreated->addInput("clk");
  VertexPtr someInput = graphCreated->addInput("someInput");
  VertexPtr gateQ1 = graphCreated->addGate(GateAnd, "Q1and");
  VertexPtr outputQ1 = graphCreated->addOutput("q1");
  VertexPtr gateQ2 = graphCreated->addGate(GateNot, "Q2not");
  VertexPtr outputQ2 = graphCreated->addOutput("q2");
  VertexPtr gateQ3 = graphCreated->addGate(GateOr, "Q3or");
  VertexPtr outputQ3 = graphCreated->addOutput("q3");
  VertexPtr nand = graphCreated->addGate(GateNand, "nand");
  VertexPtr gateQ4 = graphCreated->addGate(GateBuf, "Q4buf");
  VertexPtr outputQ4 = graphCreated->addOutput("q4");
  graphCreated->addEdge(clk, gateQ1);
  graphCreated->addEdge(someInput, gateQ1);
  graphCreated->addEdge(gateQ1, outputQ1);
  graphCreated->addEdge(gateQ1, gateQ2);
  graphCreated->addEdge(gateQ2, outputQ2);
  graphCreated->addEdge(gateQ1, gateQ3);
  graphCreated->addEdge(gateQ2, gateQ3);
  graphCreated->addEdge(someInput, gateQ3);
  graphCreated->addEdge(gateQ3, outputQ3);
  graphCreated->addEdge(gateQ3, nand);
  graphCreated->addEdge(clk, nand);
  graphCreated->addEdge(nand, gateQ4);
  graphCreated->addEdge(gateQ4, outputQ4);
  EXPECT_EQ(graph->calculateHash(), graphCreated->calculateHash());
}
TEST(VerilogReadingTest, AllGateTypesCreating) {
  Context context = Context();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr inputA = graph->addInput();
  VertexPtr inputB = graph->addInput();
  VertexPtr inputC = graph->addInput();
  VertexPtr outputQ1 = graph->addOutput();
  VertexPtr outputQ2 = graph->addOutput();
  VertexPtr vAnd = graph->addGate(GateAnd);
  VertexPtr vNand = graph->addGate(GateNand);
  VertexPtr vOr = graph->addGate(GateOr);
  VertexPtr vNor = graph->addGate(GateNor);
  VertexPtr vXor = graph->addGate(GateXor);
  VertexPtr vXnor = graph->addGate(GateXnor);
  VertexPtr vNotA = graph->addGate(GateNot);
  VertexPtr vNotB = graph->addGate(GateNot);
  VertexPtr vBufQ1 = graph->addGate(GateBuf);
  VertexPtr vBufQ2 = graph->addGate(GateBuf);
  graph->addEdge(inputA, vXor);
  graph->addEdge(inputC, vXor);
  graph->addEdge(inputA, vNotA);
  graph->addEdge(vNotA, vNand);
  graph->addEdge(inputB, vNand);
  graph->addEdge(vNand, vOr);
  graph->addEdge(vXor, vOr);
  graph->addEdge(vOr, vXnor);
  graph->addEdge(vNotA, vXnor);
  graph->addEdge(vXnor, vNor);
  graph->addEdge(inputB, vNor);
  graph->addEdge(inputB, vNotB);
  graph->addEdge(inputA, vXor);
  graph->addEdge(vNotA, vAnd);
  graph->addEdge(vNotB, vAnd);
  graph->addEdge(vAnd, vBufQ1);
  graph->addEdge(vXor, vBufQ2);
  graph->addEdge(vBufQ1, outputQ1);
  graph->addEdge(vBufQ2, outputQ2);
  graph->setName("allGatesTest");
  graph->toVerilog("../../../test/"
                   "testModulesForReading",
                   "allGatesTest.v");
  EXPECT_NO_THROW(
      OrientedGraph::readVerilog("../../../test/"
                                 "testModulesForReading/allGatesTest.v",
                                 context));
  GraphPtr graphCreated = context.d_graphs["allGatesTest"];
  EXPECT_EQ(graph->calculateHash(), graphCreated->calculateHash());
}