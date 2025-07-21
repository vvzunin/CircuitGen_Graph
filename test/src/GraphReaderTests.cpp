#include "CircuitGenGraph/GraphUtils.hpp"
#include <gtest/gtest.h>
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <lorina/lorina.hpp>
#include <memory>
using namespace CG_Graph;
TEST(VerilogReadingTest, SimplestGraphsIsReadCorrect) {
  Context context = Context();
  EXPECT_NO_THROW(OrientedGraph::readVerilog(
      "/home/rainbowkittensss/circuitGen/CircuitGen_Graph/test/"
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

TEST(VerilogReadingTest, CorrectSequenceCreated) {
  Context context = Context();
  EXPECT_NO_THROW(OrientedGraph::readVerilog(
      "/home/rainbowkittensss/circuitGen/CircuitGen_Graph/test/"
      "testModulesForReading/third.v",
      context));
  GraphPtr graph = context.d_graphs["simpleStructure"];
  EXPECT_EQ(graph->getGatesCount()[GateNot], 1);
  EXPECT_EQ(graph->getGatesCount()[GateOr], 1);
  EXPECT_EQ(graph->getGatesCount()[GateXnor], 1);
  GraphPtr graphCreated = std::make_shared<OrientedGraph>();
  VertexPtr inputA = graphCreated->addInput();
  VertexPtr inputC = graphCreated->addInput();
  VertexPtr gateXnor = graphCreated->addGate(GateXnor);
  VertexPtr gateOr = graphCreated->addGate(GateOr);
  VertexPtr gateAnd = graphCrea  VertexPtr inputB = graphCreated->addInput();
ted->addGate(GateAnd);
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
TEST(VerilogReadingTest, AllGateTypesCreatingCorrectly) {
  Context context = Context();
  // EXPECT_NO_THROW(OrientedGraph::readVerilog(
  //     "/home/rainbowkittensss/circuitGen/CircuitGen_Graph/test/"
  //     "testModulesForReading/third.v",
  //     context));
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
  graph->addEdge(vAnd, outputQ1);
  graph->addEdge(vXor, outputQ2);
  graph->setName("test");
  graph->toVerilog("/home/rainbowkittensss/circuitGen/CircuitGen_Graph/test/"
                   "testModulesForReading",
                   "generatedFile.v");
  EXPECT_NO_THROW(OrientedGraph::readVerilog(
      "/home/rainbowkittensss/circuitGen/CircuitGen_Graph/test/"
      "testModulesForReading/generatedFile.v",
      context));
  GraphPtr graphCreated = context.d_graphs["test"];
  EXPECT_EQ(graph->calculateHash(), graphCreated->calculateHash());
}