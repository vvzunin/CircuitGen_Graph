#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <CircuitGenGraph/OrientedGraph.hpp>
#include <gtest/gtest.h>

#include "easylogging++Init.hpp"

/*
  Need to add toVerilogTest, setCurrentParentTest,
    resetCountersTest, getGraphVerilogInstanceTest
  Need to realize getVerticesByLevel, needToUpdateLevel, updateLevels,
    getMaxLevel

  Questions in:
    TestSetNameAndGetName
    TestIsEmptyAndIsEmptyFull
    TestGetEdgesCount
    TestGetVerticesByName
    TestCalculateHash
*/

std::string loadStringFile(const std::filesystem::path& p) {
  std::string   str;
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  file.open(p, std::ios_base::binary);
  std::size_t sz = static_cast<std::size_t>(file_size(p));
  str.resize(sz, '\0');
  file.read(&str[0], sz);
  return str;
}

// Test is on top because it needs to contain the graph_0

TEST(TestSetNameAndGetName, ReturnCorrectName) {
  initLogging("TestSetNameAndGetName", "ReturnCorrectName");
  GraphPtr graphPtr1 = std::make_shared<OrientedGraph>();
  GraphPtr graphPtr2 = std::make_shared<OrientedGraph>();

  EXPECT_EQ(graphPtr1->getName(), "graph_0");
  EXPECT_EQ(graphPtr2->getName(), "graph_1");

  GraphPtr graphPtr3 = std::make_shared<OrientedGraph>();
  graphPtr3          = graphPtr1;
  EXPECT_EQ(graphPtr3->getName(), "graph_0");

  graphPtr1->setName("Changed");
  EXPECT_EQ(graphPtr3->getName(), "Changed");
}

TEST(TestBaseSizeAndFullSizeAndSumFullSize, ReturnCorrectSize) {
  initLogging("TestBaseSizeAndFullSizeAndSumFullSize", "ReturnCorrectSize");
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  EXPECT_EQ(graphPtr->baseSize(), 0);
  EXPECT_EQ(graphPtr->fullSize(), 0);
  EXPECT_EQ(graphPtr->sumFullSize(), 0);

  graphPtr->addGate(Gates::GateAnd, "Anything");
  graphPtr->addGate(Gates::GateNand, "Anything");
  graphPtr->addGate(Gates::GateOr, "Anything");
  graphPtr->addGate(Gates::GateNor, "Anything");
  graphPtr->addGate(Gates::GateXor, "Anything");
  graphPtr->addGate(Gates::GateXnor, "Anything");
  graphPtr->addGate(Gates::GateNot, "Anything");
  graphPtr->addGate(Gates::GateBuf, "Anything");
  graphPtr->addGate(Gates::GateDefault, "Anything");
  graphPtr->addInput("Anything");
  graphPtr->addOutput("Anything");
  graphPtr->addConst('x', "Anything");
  graphPtr->addGate(Gates::GateAnd, "Anything");
  graphPtr->addGate(Gates::GateNand, "Anything");
  graphPtr->addGate(Gates::GateOr, "Anything");
  graphPtr->addGate(Gates::GateNor, "Anything");
  graphPtr->addGate(Gates::GateXor, "Anything");
  graphPtr->addGate(Gates::GateXnor, "Anything");
  graphPtr->addGate(Gates::GateNot, "Anything");
  graphPtr->addGate(Gates::GateBuf, "Anything");
  graphPtr->addGate(Gates::GateDefault, "Anything");
  graphPtr->addInput("Anything");
  graphPtr->addOutput("Anything");
  graphPtr->addConst('x', "Anything");

  EXPECT_EQ(graphPtr->baseSize(), 18);
  EXPECT_EQ(graphPtr->fullSize(), 18);
  EXPECT_EQ(graphPtr->sumFullSize(), 24);

  GraphPtr subGraphPtr1 = std::make_shared<OrientedGraph>();
  subGraphPtr1->addGate(Gates::GateAnd, "Anything");
  subGraphPtr1->addOutput("Anything");
  subGraphPtr1->addInput("Anything");
  subGraphPtr1->addInput("Anything");
  subGraphPtr1->addConst('x', "Anything");

  graphPtr->addSubGraph(
      subGraphPtr1, graphPtr->getVerticesByType(VertexTypes::input)
  );

  EXPECT_EQ(graphPtr->baseSize(), 18);
  EXPECT_EQ(graphPtr->fullSize(), 19);
  // Does sumFullSize() return sum from subGraphs too
  // EXPECT_EQ(graphPtr->sumFullSize(), 28);

  GraphPtr subGraphPtr2 = std::make_shared<OrientedGraph>("");
  subGraphPtr2->addGate(Gates::GateAnd, "Anything");
  subGraphPtr2->addInput("Anything");
  subGraphPtr2->addInput("Anything");
  subGraphPtr2->addOutput("Anything");
  subGraphPtr2->addConst('x', "Anything");
  graphPtr->addSubGraph(
      subGraphPtr2, graphPtr->getVerticesByType(VertexTypes::input)
  );
  EXPECT_EQ(graphPtr->baseSize(), 18);
  EXPECT_EQ(graphPtr->fullSize(), 20);
  // Does sumFullSize() return sum from subGraphs too
  // EXPECT_EQ(graph.sumFullSize(), 32);
  // LOG(INFO) << *graphPtr << std::endl;
}

TEST(TestIsEmptyAndIsEmptyFull, ReturnCorrectSize) {
  GraphPtr graphPtr1 = std::make_shared<OrientedGraph>();
  GraphPtr graphPtr2 = std::make_shared<OrientedGraph>();
  GraphPtr graphPtr3 = std::make_shared<OrientedGraph>();
  GraphPtr graphPtr4 = std::make_shared<OrientedGraph>();
  GraphPtr graphPtr5 = std::make_shared<OrientedGraph>();

  EXPECT_EQ(graphPtr1->isEmpty(), true);
  EXPECT_EQ(graphPtr1->isEmptyFull(), true);
  EXPECT_EQ(graphPtr2->isEmpty(), true);
  EXPECT_EQ(graphPtr2->isEmptyFull(), true);

  graphPtr1->addGate(Gates::GateAnd, "Anything");
  EXPECT_EQ(graphPtr1->isEmpty(), false);
  EXPECT_EQ(graphPtr1->isEmptyFull(), false);

  graphPtr2->addInput("Anything");
  EXPECT_EQ(graphPtr2->isEmpty(), true);
  EXPECT_EQ(graphPtr2->isEmptyFull(), false);
  graphPtr2->addGate(Gates::GateAnd, "Anything");
  EXPECT_EQ(graphPtr2->isEmpty(), false);

  graphPtr3->addOutput("Anything");
  EXPECT_EQ(graphPtr3->isEmpty(), true);
  EXPECT_EQ(graphPtr3->isEmptyFull(), false);
  graphPtr3->addGate(Gates::GateAnd, "Anything");
  EXPECT_EQ(graphPtr3->isEmpty(), false);

  graphPtr4->addConst('x', "Anything");
  EXPECT_EQ(graphPtr4->isEmpty(), true);
  EXPECT_EQ(graphPtr4->isEmptyFull(), false);
  graphPtr4->addGate(Gates::GateAnd, "Anything");
  EXPECT_EQ(graphPtr4->isEmpty(), false);

  GraphPtr subGraphPtr = std::make_shared<OrientedGraph>();
  graphPtr5->addSubGraph(
      subGraphPtr, subGraphPtr->getVerticesByType(VertexTypes::input)
  );
  EXPECT_EQ(graphPtr5->isEmpty(), true);
  // Does graph with empty subGraph isEmptyFull?
  // EXPECT_EQ(graph5.isEmptyFull(), false);
  subGraphPtr->addInput("Anything");
  EXPECT_EQ(graphPtr5->isEmpty(), true);
  EXPECT_EQ(graphPtr5->isEmptyFull(), false);
  subGraphPtr->addGate(Gates::GateAnd, "Anything");
  EXPECT_EQ(graphPtr5->isEmpty(), false);
  EXPECT_EQ(graphPtr5->isEmptyFull(), false);
}

// needToUpdateLevel isn't realized
TEST(TestNeedToUpdateLevel, ReturnCorrectValue) {}

// updateLevels is commented
TEST(TestUpdateLevelsAndGetMaxLevel, ReturnCorrectMaxLevel) {}

TEST(TestGetEdgesCount, ReturnCorrectCount) {
  GraphPtr graphPtr1 = std::make_shared<OrientedGraph>();
  auto     input1    = graphPtr1->addInput("Input1");
  auto     input2    = graphPtr1->addInput("Input2");
  auto     gate1     = graphPtr1->addGate(Gates::GateAnd, "And");
  auto     output1   = graphPtr1->addOutput("Output");
  EXPECT_EQ(graphPtr1->getEdgesCount(), 0);

  graphPtr1->addEdge(gate1, output1);
  EXPECT_EQ(graphPtr1->getEdgesCount(), 1);

  graphPtr1->addEdges({input1, input2}, gate1);
  EXPECT_EQ(graphPtr1->getEdgesCount(), 3);

  // there is no check for edge being already exist
  // graph1.addEdge(gate, output);
  // EXPECT_EQ(graph1.getEdgesCount(), 3);
  // EXPECT_THROW()

  // there is no check for edges being already exist
  // graph1.addEdges({input1, input2}, gate);
  // EXPECT_EQ(graph1.getEdgesCount(), 3);
  // EXPECT_THROW()

  GraphPtr graphPtr2 = std::make_shared<OrientedGraph>();
  auto     gate2     = graphPtr2->addGate(Gates::GateAnd, "And");
  auto     gate3     = graphPtr2->addGate(Gates::GateAnd, "And");

  // There is no check to the same objects in Edge
  // graph2.addEdge(gate2, gate2);
  // EXPECT_EQ(graph2.getEdgesCount(), 0);
  // EXPECT_TRHOW();
  // graph2.addEdges({gate2, gate2, gate3, gate3}, gate2);
  // EXPECT_EQ(graph2.getEdgesCount(), 1);
  // EXPECT_TRHOW();
}

TEST(TestGetSubGraphs, ReturnCorrectSubGraphs) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  EXPECT_EQ(graphPtr->getSubGraphs().size(), 0);

  GraphPtr subGraphPtr1 = std::make_shared<OrientedGraph>();
  graphPtr->addSubGraph(
      subGraphPtr1, graphPtr->getVerticesByType(VertexTypes::input)
  );
  EXPECT_NE(
      graphPtr->getSubGraphs().find(subGraphPtr1),
      graphPtr->getSubGraphs().end()
  );

  GraphPtr subGraphPtr2 = std::make_shared<OrientedGraph>();
  GraphPtr subGraphPtr3 = std::make_shared<OrientedGraph>();
  graphPtr->addSubGraph(
      subGraphPtr2, subGraphPtr2->getVerticesByType(VertexTypes::input)
  );
  subGraphPtr3->addSubGraph(
      subGraphPtr1, subGraphPtr1->getVerticesByType(VertexTypes::input)
  );
  EXPECT_EQ(graphPtr->getSubGraphs().size(), 2);
  EXPECT_NE(
      graphPtr->getSubGraphs().find(subGraphPtr1),
      graphPtr->getSubGraphs().end()
  );
  EXPECT_NE(
      graphPtr->getSubGraphs().find(subGraphPtr2),
      graphPtr->getSubGraphs().end()
  );
  EXPECT_NE(
      graphPtr->getSubGraphs().find(subGraphPtr3),
      graphPtr->getSubGraphs().end()
  );
}

TEST(TestGetBaseVertexes, ReturnCorrectVertexes) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  EXPECT_EQ(graphPtr->getBaseVertexes()[VertexTypes::input].size(), 0);

  // without subGraph
  auto gate1 = graphPtr->addGate(Gates::GateAnd, "Anything");
  EXPECT_EQ(graphPtr->getBaseVertexes()[VertexTypes::gate].size(), 1);
  EXPECT_EQ(graphPtr->getBaseVertexes()[VertexTypes::gate][0], gate1);

  auto input1 = graphPtr->addInput("Anything");
  EXPECT_EQ(graphPtr->getBaseVertexes()[VertexTypes::input].size(), 1);
  EXPECT_EQ(graphPtr->getBaseVertexes()[VertexTypes::input][0], input1);

  auto output1 = graphPtr->addOutput("Anything");
  EXPECT_EQ(graphPtr->getBaseVertexes()[VertexTypes::output].size(), 1);
  EXPECT_EQ(graphPtr->getBaseVertexes()[VertexTypes::output][0], output1);

  auto const1 = graphPtr->addConst('x', "Anything");
  EXPECT_EQ(graphPtr->getBaseVertexes()[VertexTypes::constant].size(), 1);
  EXPECT_EQ(graphPtr->getBaseVertexes()[VertexTypes::constant][0], const1);

  // with subGraph
  GraphPtr subGraphPtr = std::make_shared<OrientedGraph>();
  graphPtr->addSubGraph(
      subGraphPtr, subGraphPtr->getVerticesByType(VertexTypes::input)
  );
  subGraphPtr->addGate(Gates::GateAnd, "Anything");
  EXPECT_EQ(graphPtr->getBaseVertexes()[VertexTypes::gate].size(), 1);

  subGraphPtr->addInput("Anything");
  EXPECT_EQ(graphPtr->getBaseVertexes()[VertexTypes::input].size(), 1);

  subGraphPtr->addOutput("Anything");
  EXPECT_EQ(graphPtr->getBaseVertexes()[VertexTypes::output].size(), 1);

  subGraphPtr->addConst('x', "Anything");
  EXPECT_EQ(graphPtr->getBaseVertexes()[VertexTypes::constant].size(), 1);
}

TEST(TestGetVerticeByIndex, ReturnCorrectType) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  graphPtr->addGate(Gates::GateAnd, "Anything");
  graphPtr->addGate(Gates::GateNand, "Anything");
  graphPtr->addGate(Gates::GateOr, "Anything");
  graphPtr->addGate(Gates::GateNor, "Anything");
  graphPtr->addGate(Gates::GateXor, "Anything");
  graphPtr->addGate(Gates::GateXnor, "Anything");
  graphPtr->addGate(Gates::GateNot, "Anything");
  graphPtr->addGate(Gates::GateBuf, "Anything");
  graphPtr->addGate(Gates::GateDefault, "Anything");
  graphPtr->addInput("Anything");
  graphPtr->addOutput("Anything");
  graphPtr->addConst('x', "Anything");

  EXPECT_EQ(graphPtr->getVerticeByIndex(0)->getType(), VertexTypes::input);
  EXPECT_EQ(graphPtr->getVerticeByIndex(1)->getType(), VertexTypes::constant);
  EXPECT_EQ(graphPtr->getVerticeByIndex(2)->getType(), VertexTypes::gate);
  EXPECT_EQ(graphPtr->getVerticeByIndex(3)->getType(), VertexTypes::gate);
  EXPECT_EQ(graphPtr->getVerticeByIndex(4)->getType(), VertexTypes::gate);
  EXPECT_EQ(graphPtr->getVerticeByIndex(5)->getType(), VertexTypes::gate);
  EXPECT_EQ(graphPtr->getVerticeByIndex(6)->getType(), VertexTypes::gate);
  EXPECT_EQ(graphPtr->getVerticeByIndex(7)->getType(), VertexTypes::gate);
  EXPECT_EQ(graphPtr->getVerticeByIndex(8)->getType(), VertexTypes::gate);
  EXPECT_EQ(graphPtr->getVerticeByIndex(9)->getType(), VertexTypes::gate);
  EXPECT_EQ(graphPtr->getVerticeByIndex(10)->getType(), VertexTypes::gate);
  EXPECT_EQ(graphPtr->getVerticeByIndex(11)->getType(), VertexTypes::output);
}

TEST(TestGetVerticeByIndex, ReturnCorrectGate) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  graphPtr->addGate(Gates::GateAnd, "Anything");
  graphPtr->addGate(Gates::GateNand, "Anything");
  graphPtr->addGate(Gates::GateOr, "Anything");
  graphPtr->addGate(Gates::GateNor, "Anything");
  graphPtr->addGate(Gates::GateXor, "Anything");
  graphPtr->addGate(Gates::GateXnor, "Anything");
  graphPtr->addGate(Gates::GateNot, "Anything");
  graphPtr->addGate(Gates::GateBuf, "Anything");
  graphPtr->addGate(Gates::GateDefault, "Anything");

  EXPECT_EQ(graphPtr->getVerticeByIndex(0)->getGate(), Gates::GateAnd);
  EXPECT_EQ(graphPtr->getVerticeByIndex(1)->getGate(), Gates::GateNand);
  EXPECT_EQ(graphPtr->getVerticeByIndex(2)->getGate(), Gates::GateOr);
  EXPECT_EQ(graphPtr->getVerticeByIndex(3)->getGate(), Gates::GateNor);
  EXPECT_EQ(graphPtr->getVerticeByIndex(4)->getGate(), Gates::GateXor);
  EXPECT_EQ(graphPtr->getVerticeByIndex(5)->getGate(), Gates::GateXnor);
  EXPECT_EQ(graphPtr->getVerticeByIndex(6)->getGate(), Gates::GateNot);
  EXPECT_EQ(graphPtr->getVerticeByIndex(7)->getGate(), Gates::GateBuf);
  EXPECT_EQ(graphPtr->getVerticeByIndex(8)->getGate(), Gates::GateDefault);
}

TEST(TestGetVerticeByIndex, ThrowExceptionWhenWrongIndex) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();

  EXPECT_THROW(graphPtr->getVerticeByIndex(0), std::invalid_argument);
  graphPtr->addGate(Gates::GateAnd, "Anything");
  EXPECT_THROW(graphPtr->getVerticeByIndex(1), std::invalid_argument);
}
// // TEST(TestGetVerticesByLevel, ReturnCorrectVertices) {
// //   // OrientedGraph graph;
// //   // EXPECT_EQ(graph.getVerticesByLevel(0).size(), 0);

// //   // Do not work
// //   // auto          gate = graph.addGate(Gates::GateAnd, "Anything");
// //   // EXPECT_EQ(gate->getLevel(), 0);
// //   // EXPECT_EQ(graph.getVerticesByLevel(0)[0]->getType(),
// VertexTypes::gate);
// // }

TEST(TestGetVerticesByName, ReturnCorrectVertices) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();

  // Without subGraph
  EXPECT_EQ(graphPtr->getVerticesByName("Anything").size(), 0);

  graphPtr->addGate(Gates::GateAnd, "Anything");
  EXPECT_EQ(graphPtr->getVerticesByName("Anything").size(), 1);
  EXPECT_EQ(
      graphPtr->getVerticesByName("Anything")[0]->getGate(), Gates::GateAnd
  );

  graphPtr->addInput("Anything");
  EXPECT_EQ(graphPtr->getVerticesByName("Anything").size(), 2);
  EXPECT_EQ(
      graphPtr->getVerticesByName("Anything")[0]->getType(), VertexTypes::input
  );
  EXPECT_EQ(
      graphPtr->getVerticesByName("Anything")[1]->getGate(), Gates::GateAnd
  );

  graphPtr->addOutput("Anything");
  EXPECT_EQ(graphPtr->getVerticesByName("Anything").size(), 3);
  // Should VertexTypes::output be in the end or be after VertexTypes::input?
  // EXPECT_EQ(graphPtr->getVerticesByName("Anything")[2]->getType(),
  // VertexTypes::output);

  graphPtr->addConst('x', "Anything");
  EXPECT_EQ(graphPtr->getVerticesByName("Anything").size(), 4);
  // What should return at first VertexTypes::constant or VertexTypes::output?
  // EXPECT_EQ(graphPtr->getVerticesByName("Anything")[1]->getType(),
  // VertexTypes::constant);

  // With subGraph
  GraphPtr subGraphPtr = std::make_shared<OrientedGraph>();
  subGraphPtr->addInput("Anything");

  graphPtr->addSubGraph(
      subGraphPtr, graphPtr->getVerticesByType(VertexTypes::input)
  );
  subGraphPtr->addGate(Gates::GateAnd, "Anything");
  EXPECT_EQ(graphPtr->getVerticesByName("Anything").size(), 4);
  EXPECT_EQ(graphPtr->getVerticesByName("Anything", true).size(), 6);
  // Why return GateDefault?
  EXPECT_EQ(
      graphPtr->getVerticesByName("Anything", true)[4]->getGate(),
      Gates::GateDefault
  );
}

TEST(TestToGraphMLStringReturn, ReturnCorrectStringWhenGrpahIsEmpty) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>("Graph1");
  EXPECT_EQ(graphPtr->isEmptyFull(), true);
  EXPECT_EQ(
      graphPtr->toGraphMLClassic(),
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" "
      "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
      "xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns "
      "http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n"
      "  <key id=\"t\" for=\"node\" attr.name=\"type\" attr.type=\"string\"/>\n"
      "  <graph id=\"Graph1\" edgedefault=\"directed\">\n"
      "  </graph>\n"
      "</graphml>\n"
  );
}
TEST(TestToGraphMLStringReturn, ReturnCorrectStringWhenThereAreNodes) {
  GraphPtr graphPtr1 = std::make_shared<OrientedGraph>("Graph1");
  graphPtr1->addGate(Gates::GateAnd, "gate1");
  EXPECT_EQ(
      graphPtr1->toGraphMLClassic(),
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<graphml "
      "xmlns=\"http://graphml.graphdrawing.org/xmlns\" "
      "xmlns:xsi=\"http://www.w3.org/2001/"
      "XMLSchema-instance\"\nxsi:schemaLocation=\"http://"
      "graphml.graphdrawing.org/xmlns "
      "http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n  <key "
      "id=\"t\" for=\"node\" attr.name=\"type\" attr.type=\"string\"/>\n  "
      "<graph id=\"Graph1\" edgedefault=\"directed\">\n    <node "
      "id=\"gate1\">\n      <data key=\"t\">and</data>\n    </node>\n  "
      "</graph>\n</graphml>\n"
  );

  graphPtr1->addGate(Gates::GateNot, "gate2");
  EXPECT_EQ(
      graphPtr1->toGraphMLClassic(0),
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<graphml "
      "xmlns=\"http://graphml.graphdrawing.org/xmlns\" "
      "xmlns:xsi=\"http://www.w3.org/2001/"
      "XMLSchema-instance\"\nxsi:schemaLocation=\"http://"
      "graphml.graphdrawing.org/xmlns "
      "http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n  <key "
      "id=\"t\" for=\"node\" attr.name=\"type\" attr.type=\"string\"/>\n  "
      "<graph id=\"Graph1\" edgedefault=\"directed\">\n    <node "
      "id=\"gate1\">\n      <data key=\"t\">and</data>\n    </node>\n    <node "
      "id=\"gate2\">\n      <data key=\"t\">not</data>\n    </node>\n  "
      "</graph>\n</graphml>\n"
  );
}
// Unknown error
// TEST(TestToGraphMLStringReturn, ReturnCorrectStringWhenThereAreSubGraphs) {
//   GraphPtr graphPtr1    = std::make_shared<OrientedGraph>("Graph1");
//   GraphPtr subGraphPtr1 = std::make_shared<OrientedGraph>("SubGraph1");
//   graphPtr1->addSubGraph(
//       subGraphPtr1, graphPtr1->getVerticesByType(VertexTypes::input)
//   );
//   EXPECT_EQ(graphPtr1->toGraphMLClassic(0), "");

//   subGraphPtr1->addGate(Gates::GateAnd, "gate1");
//   EXPECT_EQ(graphPtr1->toGraphMLClassic(0), "");
// }

TEST(TestToGraphMLStringReturn, ReturnCorrectStringWhenThereAreSubEdges) {
  GraphPtr graphPtr1 = std::make_shared<OrientedGraph>("Graph1");
  auto     gate1     = graphPtr1->addGate(Gates::GateAnd, "gate1");
  auto     input1    = graphPtr1->addInput("input1");
  auto     input2    = graphPtr1->addInput("input2");

  graphPtr1->addEdges({input1, input2}, gate1);
  EXPECT_EQ(
      graphPtr1->toGraphMLClassic(0),
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<graphml "
      "xmlns=\"http://graphml.graphdrawing.org/xmlns\" "
      "xmlns:xsi=\"http://www.w3.org/2001/"
      "XMLSchema-instance\"\nxsi:schemaLocation=\"http://"
      "graphml.graphdrawing.org/xmlns "
      "http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n  <key "
      "id=\"t\" for=\"node\" attr.name=\"type\" attr.type=\"string\"/>\n  "
      "<graph id=\"Graph1\" edgedefault=\"directed\">\n    <node "
      "id=\"input1\">\n      <data key=\"t\">input</data>\n    </node>\n    "
      "<node id=\"input2\">\n      <data key=\"t\">input</data>\n    </node>\n "
      "   <node id=\"gate1\">\n      <data key=\"t\">and</data>\n    </node>\n "
      "   <edge source=\"input1\" target=\"gate1\"/>\n    <edge "
      "source=\"input2\" target=\"gate1\"/>\n  </graph>\n</graphml>\n"
  );
}

// Unknown error
// TEST(TestToGraphMLBoolReturn, CorrectWriteToFile) {
//   std::string   filename = "ToGraphMLTest.txt";
//   std::ofstream outF(filename);
//   GraphPtr      graphPtr1    = std::make_shared<OrientedGraph>("Graph1");
//   auto          gate1        = graphPtr1->addGate(Gates::GateAnd, "gate1");
//   auto          input1       = graphPtr1->addInput("input1");
//   auto          input2       = graphPtr1->addInput("input2");
//   GraphPtr      subGraphPtr1 = std::make_shared<OrientedGraph>("SubGraph1");
//   graphPtr1->addSubGraph(
//       subGraphPtr1, subGraphPtr1->getVerticesByType(VertexTypes::input)
//   );
//   graphPtr1->addEdges({input1, input2}, gate1);
//   subGraphPtr1->addGate(Gates::GateAnd, "gate1");
//   EXPECT_EQ(graphPtr1->toGraphMLClassic(outF), true);

//   std::ifstream inF(filename);
//   outF.close();
//   std::ostringstream contentStream;
//   contentStream << inF.rdbuf();
//   std::string stringF = contentStream.str();
//   inF.close();
//   EXPECT_EQ(stringF, graphPtr1->toGraphMLClassic(0));
// }

TEST(TestCalculateHash, GraphsWithTheSameStructureHaveEqualHash) {
  GraphPtr graphPtr1 = std::make_shared<OrientedGraph>();
  GraphPtr graphPtr2 = std::make_shared<OrientedGraph>();

  EXPECT_EQ(graphPtr1->calculateHash(), graphPtr1->calculateHash(true));
  EXPECT_EQ(graphPtr1->calculateHash(), graphPtr2->calculateHash());

  graphPtr1->addGate(Gates::GateAnd, "Anything");
  graphPtr2->addGate(Gates::GateAnd, "Anything");
  EXPECT_EQ(graphPtr1->calculateHash(true), graphPtr2->calculateHash(true));

  graphPtr1->addGate(Gates::GateNand, "Anything");
  // Should hash be not the same if there are differences in Gates
  // EXPECT_NE(graphPtr1->calculateHash(true),
  // graphPtr2->calculateHash(true));

  graphPtr1->addInput("Anything");
  EXPECT_NE(graphPtr1->calculateHash(true), graphPtr2->calculateHash(true));
  graphPtr2->addInput("Anything");
  EXPECT_EQ(graphPtr1->calculateHash(true), graphPtr2->calculateHash(true));

  graphPtr1->addOutput("Anything");
  // Should hash be not the same if there are differences in Outputs
  // EXPECT_NE(graphPtr1->calculateHash(true),
  // graphPtr2->calculateHash(true)); graphPtr2->addOutput("Anything");
  EXPECT_EQ(graphPtr1->calculateHash(true), graphPtr2->calculateHash(true));
}

TEST(TestCalculateHash, GraphsWithTheSameStructureButDifferentConst) {
  GraphPtr graphPtr1 = std::make_shared<OrientedGraph>();
  GraphPtr graphPtr2 = std::make_shared<OrientedGraph>();

  EXPECT_EQ(graphPtr1->calculateHash(), graphPtr1->calculateHash());

  auto inp1  = graphPtr1->addInput();
  auto inp2  = graphPtr2->addInput();
  auto inp11 = graphPtr1->addInput();
  auto inp22 = graphPtr2->addInput();
  EXPECT_EQ(graphPtr1->calculateHash(true), graphPtr2->calculateHash(true));

  auto gate1 = graphPtr1->addGate(Gates::GateAnd);
  auto gate2 = graphPtr2->addGate(Gates::GateAnd);
  graphPtr1->addEdge(inp1, gate1);
  graphPtr1->addEdge(inp11, gate1);
  graphPtr2->addEdge(inp2, gate2);
  graphPtr2->addEdge(inp22, gate2);
  EXPECT_EQ(graphPtr1->calculateHash(true), graphPtr2->calculateHash(true));

  auto gate3 = graphPtr1->addGate(Gates::GateNot);
  auto gate4 = graphPtr2->addGate(Gates::GateNot);
  graphPtr1->addEdge(gate1, gate3);
  graphPtr2->addEdge(gate2, gate4);
  EXPECT_EQ(graphPtr1->calculateHash(true), graphPtr2->calculateHash(true));

  auto out1 = graphPtr1->addOutput();
  auto out2 = graphPtr2->addOutput();
  graphPtr1->addEdge(gate1, out1);
  graphPtr2->addEdge(gate2, out2);
  EXPECT_EQ(graphPtr1->calculateHash(true), graphPtr2->calculateHash(true));

  auto const1 = graphPtr1->addConst('0');
  auto const2 = graphPtr2->addConst('1');
  graphPtr1->addEdge(const1, gate1);
  graphPtr2->addEdge(const2, gate2);
  EXPECT_NE(graphPtr1->calculateHash(true), graphPtr2->calculateHash(true));
}

TEST(TestGetGatesCount, ReturnCorrectGates) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();
  EXPECT_EQ(graphPtr->getGatesCount().size(), 8);

  graphPtr->addGate(Gates::GateAnd, "Anything");
  graphPtr->addGate(Gates::GateNand, "Anything");
  graphPtr->addGate(Gates::GateOr, "Anything");
  graphPtr->addGate(Gates::GateNor, "Anything");
  graphPtr->addGate(Gates::GateXor, "Anything");
  graphPtr->addGate(Gates::GateXnor, "Anything");
  graphPtr->addGate(Gates::GateNot, "Anything");
  graphPtr->addGate(Gates::GateBuf, "Anything");
  graphPtr->addGate(Gates::GateDefault, "Anything");
  graphPtr->addInput("Anything");
  graphPtr->addOutput("Anything");
  graphPtr->addConst('x', "Anything");
  EXPECT_EQ(graphPtr->getGatesCount().size(), 9);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateAnd], 1);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateNand], 1);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateOr], 1);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateNor], 1);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateXor], 1);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateXnor], 1);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateNot], 1);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateBuf], 1);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateDefault], 1);

  graphPtr->addGate(Gates::GateAnd, "Anything");
  graphPtr->addGate(Gates::GateNand, "Anything");
  graphPtr->addGate(Gates::GateOr, "Anything");
  graphPtr->addGate(Gates::GateNor, "Anything");
  graphPtr->addGate(Gates::GateXor, "Anything");
  graphPtr->addInput("Anything");
  graphPtr->addOutput("Anything");
  graphPtr->addConst('x', "Anything");
  EXPECT_EQ(graphPtr->getGatesCount().size(), 9);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateAnd], 2);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateNand], 2);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateOr], 2);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateNor], 2);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateXor], 2);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateXnor], 1);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateNot], 1);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateBuf], 1);
  EXPECT_EQ(graphPtr->getGatesCount()[Gates::GateDefault], 1);
}

TEST(TestGetEdgesGatesCount, ReturnCorrectGates) {
  GraphPtr graphPtr = std::make_shared<OrientedGraph>();

  EXPECT_EQ(graphPtr->getEdgesGatesCount().size(), 8);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateAnd].size(), 8);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateNand].size(), 8);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateOr].size(), 8);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateNor].size(), 8);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateXor].size(), 8);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateXnor].size(), 8);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateNot].size(), 8);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateBuf].size(), 8);

  auto gate1 = graphPtr->addGate(Gates::GateAnd, "Anything");
  auto gate2 = graphPtr->addGate(Gates::GateNand, "Anything");
  graphPtr->addEdge(gate1, gate2);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateAnd][Gates::GateNand], 1);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateAnd][Gates::GateOr], 0);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateAnd][Gates::GateAnd], 0);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateAnd][Gates::GateNor], 0);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateAnd][Gates::GateXor], 0);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateAnd][Gates::GateXnor], 0);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateAnd][Gates::GateNot], 0);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateAnd][Gates::GateBuf], 0);

  auto gate3 = graphPtr->addGate(Gates::GateNand, "Anything");
  auto gate4 = graphPtr->addGate(Gates::GateOr, "Anything");
  graphPtr->addEdges({gate3, gate4}, gate1);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateNand][Gates::GateAnd], 1);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateOr][Gates::GateAnd], 1);
  EXPECT_EQ(graphPtr->getEdgesGatesCount()[Gates::GateAnd][Gates::GateNand], 1);
}

TEST(TestToVerilog, Simple) {
  initLogging("TestToVerilog", "Simple");
  GraphPtr graphPtr = std::make_shared<OrientedGraph>("testGraph");
  auto     inA      = graphPtr->addInput("a");
  auto     inB      = graphPtr->addInput("b");
  auto     out      = graphPtr->addOutput("c");
  auto     gateAnd1 = graphPtr->addGate(Gates::GateAnd, "andAB");
  auto     const1   = graphPtr->addConst('1', "const1");
  auto     gateOr1  = graphPtr->addGate(Gates::GateOr, "orAnd11");
  graphPtr->addEdges({inA, inB}, gateAnd1);
  graphPtr->addEdges({gateAnd1, const1}, gateOr1);
  graphPtr->addEdge(gateOr1, out);
  auto        strs     = graphPtr->toVerilog(".", "testSimple.v");
  std::string curPath  = std::filesystem::current_path();
  std::string loadFile = loadStringFile(curPath + "/testSimple.v");
  loadFile             = loadFile.substr(loadFile.find("\n") + 2);
  // LOG(INFO) << "Printing Verilog file: " << strs.first << "\n" << loadFile;
}

TEST(TestToVerilog, SubGraph) {
  initLogging("TestToVerilog", "SubGraph");
  GraphPtr subGraphPtr = std::make_shared<OrientedGraph>("testSubGraph");
  auto     inA         = subGraphPtr->addInput("a");
  auto     inB         = subGraphPtr->addInput("b");
  auto     out         = subGraphPtr->addOutput("c");
  auto     gateAnd1    = subGraphPtr->addGate(Gates::GateAnd, "andAB");
  auto     const1      = subGraphPtr->addConst('1', "const1");
  auto     gateOr1     = subGraphPtr->addGate(Gates::GateOr, "orAnd11");
  subGraphPtr->addEdges({inA, inB}, gateAnd1);
  subGraphPtr->addEdges({gateAnd1, const1}, gateOr1);
  subGraphPtr->addEdge(gateOr1, out);

  GraphPtr graphPtr = std::make_shared<OrientedGraph>("testGraph");
  inA               = graphPtr->addInput("a");
  inB               = graphPtr->addInput("b");
  out               = graphPtr->addOutput("c");

  auto outs         = graphPtr->addSubGraph(subGraphPtr, {inA, inB});

  graphPtr->addEdge(outs[0], out);

  std::filesystem::create_directories("./submodules");
  auto        strs     = graphPtr->toVerilog(".", "testSubGraph.v");
  std::string curPath  = std::filesystem::current_path();
  std::string loadFile = loadStringFile(curPath + "/testSubGraph.v");
  loadFile             = loadFile.substr(loadFile.find("\n") + 2);
  // LOG(INFO) << "Printing DOT file: " << strs.first << "\n" << loadFile;
}

TEST(TestToDOT, Simple) {
  initLogging("TestToDOT", "Simple");
  GraphPtr graphPtr = std::make_shared<OrientedGraph>("testGraph");
  auto     inA      = graphPtr->addInput("a");
  auto     inB      = graphPtr->addInput("b");
  auto     out      = graphPtr->addOutput("c");
  auto     gateAnd1 = graphPtr->addGate(Gates::GateAnd, "andAB");
  auto     const1   = graphPtr->addConst('1', "const1");
  auto     gateOr1  = graphPtr->addGate(Gates::GateOr, "orAnd11");
  graphPtr->addEdges({inA, inB}, gateAnd1);
  graphPtr->addEdges({gateAnd1, const1}, gateOr1);
  graphPtr->addEdge(gateOr1, out);
  auto        strs     = graphPtr->toDOT(".", "testSimple.dot");
  // LOG(INFO) << "toDot complete!";
  std::string curPath  = std::filesystem::current_path();
  // LOG(INFO) << "curPath: " << curPath;
  std::string loadFile = loadStringFile(curPath + "/testSimple.dot");
  // LOG(INFO) << "loadFile: " << loadFile;
  loadFile             = loadFile.substr(loadFile.find("\n") + 2);
  // LOG(INFO) << "Printing DOT file: " << strs.first << "\n" << loadFile;
}

TEST(TestToDOT, SubGraph) {
  initLogging("TestToDOT", "SubGraph");
  GraphPtr subGraphPtr = std::make_shared<OrientedGraph>("testSubGraph");
  auto     inA         = subGraphPtr->addInput("a");
  auto     inB         = subGraphPtr->addInput("b");
  auto     outC        = subGraphPtr->addOutput("c");
  auto     outD        = subGraphPtr->addOutput("d");
  auto     gateAnd1    = subGraphPtr->addGate(Gates::GateAnd, "andAB");
  auto     const1      = subGraphPtr->addConst('1', "const1");
  auto     gateOr1     = subGraphPtr->addGate(Gates::GateOr, "orAnd11");
  subGraphPtr->addEdges({inA, inB}, gateAnd1);
  subGraphPtr->addEdges({gateAnd1, const1}, gateOr1);
  subGraphPtr->addEdge(gateOr1, outC);
  subGraphPtr->addEdge(gateAnd1, outD);

  GraphPtr graphPtr = std::make_shared<OrientedGraph>("testGraph");
  inA               = graphPtr->addInput("a");
  inB               = graphPtr->addInput("b");
  outC              = graphPtr->addOutput("c");
  outD              = graphPtr->addOutput("d");
  auto outE         = graphPtr->addOutput("e");

  auto outs         = graphPtr->addSubGraph(subGraphPtr, {inA, inB});

  graphPtr->addEdge(outs[0], outC);
  graphPtr->addEdge(outs[1], outD);
  graphPtr->addEdge(inB, outE);
  std::filesystem::create_directories("./submodulesDOT");
  auto        strs     = graphPtr->toDOT(".", "testSubGraph.dot");
  std::string curPath  = std::filesystem::current_path();
  std::string loadFile = loadStringFile(curPath + "/testSubGraph.dot");
  loadFile             = loadFile.substr(loadFile.find("\n") + 2);
  // LOG(INFO) << "Printing DOT file: " << strs.first << "\n" << loadFile;
}

TEST(TestToDOT, SubGraphUnroll) {
  initLogging("TestToDOT", "SubGraphUnroll");
  GraphPtr subGraphPtr = std::make_shared<OrientedGraph>("testSubGraph");
  auto     inA         = subGraphPtr->addInput("a");
  auto     inB         = subGraphPtr->addInput("b");
  auto     outC        = subGraphPtr->addOutput("c");
  auto     outD        = subGraphPtr->addOutput("d");
  auto     gateAnd1    = subGraphPtr->addGate(Gates::GateAnd, "andAB");
  auto     const1      = subGraphPtr->addConst('1', "const1");
  auto     gateOr1     = subGraphPtr->addGate(Gates::GateOr, "orAnd11");
  subGraphPtr->addEdges({inA, inB}, gateAnd1);
  subGraphPtr->addEdges({gateAnd1, const1}, gateOr1);
  subGraphPtr->addEdge(gateOr1, outC);
  subGraphPtr->addEdge(gateAnd1, outD);

  GraphPtr graphPtr = std::make_shared<OrientedGraph>("testGraph");
  inA               = graphPtr->addInput("a");
  inB               = graphPtr->addInput("b");
  outC              = graphPtr->addOutput("c");
  outD              = graphPtr->addOutput("d");
  auto outE         = graphPtr->addOutput("e");

  auto outs         = graphPtr->addSubGraph(subGraphPtr, {inA, inB});

  graphPtr->addEdge(outs[0], outC);
  graphPtr->addEdge(outs[1], outD);
  graphPtr->addEdge(inB, outE);

  GraphPtr unrollGraphPtr = graphPtr->unrollGraph();

  std::filesystem::create_directories("./submodulesDOT");
  auto        strs     = unrollGraphPtr->toDOT(".", "testSubGraphUnroll.dot");
  std::string curPath  = std::filesystem::current_path();
  std::string loadFile = loadStringFile(curPath + "/testSubGraphUnroll.dot");
  loadFile             = loadFile.substr(loadFile.find("\n") + 2);
  // LOG(INFO) << "Printing DOT file: " << strs.first << "\n" << loadFile;
}

TEST(TestToDOT, SubGraphUnroll2) {
  initLogging("TestToDOT", "SubGraphUnroll2");
  GraphPtr subGraphPtr = std::make_shared<OrientedGraph>("testSubGraph");
  auto     inA         = subGraphPtr->addInput("a");
  auto     inB         = subGraphPtr->addInput("b");
  auto     outC        = subGraphPtr->addOutput("c");
  auto     outD        = subGraphPtr->addOutput("d");
  auto     gateAnd1    = subGraphPtr->addGate(Gates::GateAnd, "andAB");
  auto     const1      = subGraphPtr->addConst('1', "const1");
  auto     gateOr1     = subGraphPtr->addGate(Gates::GateOr, "orAnd11");
  subGraphPtr->addEdges({inA, inB}, gateAnd1);
  subGraphPtr->addEdges({gateAnd1, const1}, gateOr1);
  subGraphPtr->addEdge(gateOr1, outC);
  subGraphPtr->addEdge(gateAnd1, outD);
  // LOG(INFO) << "First subGraph added!";

  GraphPtr subGraphPtr2 = std::make_shared<OrientedGraph>("testSubGraph2");
  inA                   = subGraphPtr2->addInput("a");
  inB                   = subGraphPtr2->addInput("b");
  outC                  = subGraphPtr2->addOutput("c");
  outD                  = subGraphPtr2->addOutput("d");
  gateAnd1              = subGraphPtr2->addGate(Gates::GateAnd, "andAB");
  const1                = subGraphPtr2->addConst('1', "const1");
  gateOr1               = subGraphPtr2->addGate(Gates::GateOr, "orAnd11");
  subGraphPtr2->addEdges({inA, inB}, gateAnd1);
  subGraphPtr2->addEdges({gateAnd1, const1}, gateOr1);
  subGraphPtr2->addEdge(gateOr1, outC);
  subGraphPtr2->addEdge(gateAnd1, outD);
  // LOG(INFO) << "Second subGraph added!";

  GraphPtr graphPtr = std::make_shared<OrientedGraph>("testGraph");
  inA               = graphPtr->addInput("a");
  inB               = graphPtr->addInput("b");
  outC              = graphPtr->addOutput("c");
  outD              = graphPtr->addOutput("d");
  auto outE         = graphPtr->addOutput("e");
  // LOG(INFO) << "In/outs added to main graph";

  auto outs1        = graphPtr->addSubGraph(subGraphPtr, {inA, inB});
  auto outs2        = graphPtr->addSubGraph(subGraphPtr2, {inA, inB});
  // LOG(INFO) << "SubGraphs added!";

  auto gateAnd2     = graphPtr->addGate(Gates::GateAnd, "andAB2");
  auto gateAnd3     = graphPtr->addGate(Gates::GateAnd, "andAB3");
  // LOG(INFO) << "Two AND gate added";

  graphPtr->addEdge(outs1[0], gateAnd2);
  graphPtr->addEdge(outs1[1], gateAnd2);
  // LOG(INFO) << "outs1 to and";

  graphPtr->addEdge(outs2[0], gateAnd3);
  graphPtr->addEdge(outs2[1], gateAnd3);
  // LOG(INFO) << "outs2 to and";

  graphPtr->addEdge(gateAnd2, outC);
  graphPtr->addEdge(gateAnd3, outD);
  // LOG(INFO) << "ANDs to outs";
  graphPtr->addEdge(inB, outE);
  // LOG(INFO) << "inB to outE";

  GraphPtr unrollGraphPtr = graphPtr->unrollGraph();
  // LOG(INFO) << "Graph unrolled!";

  std::filesystem::create_directories("./submodulesDOT");
  auto        strs     = unrollGraphPtr->toDOT(".", "testSubGraphUnroll2.dot");
  std::string curPath  = std::filesystem::current_path();
  std::string loadFile = loadStringFile(curPath + "/testSubGraphUnroll2.dot");
  loadFile             = loadFile.substr(loadFile.find("\n") + 2);
  // LOG(INFO) << "Printing DOT file: " << strs.first << "\n" << loadFile;
}

TEST(TestToDOT, SubGraphUnroll3) {
  initLogging("TestToDOT", "SubGraphUnroll3");
  GraphPtr subGraphPtr = std::make_shared<OrientedGraph>("testSubGraph");
  auto     inA         = subGraphPtr->addInput("a");
  auto     inB         = subGraphPtr->addInput("b");
  auto     outC        = subGraphPtr->addOutput("c");
  auto     outD        = subGraphPtr->addOutput("d");
  auto     gateAnd1    = subGraphPtr->addGate(Gates::GateAnd, "andAB");
  auto     const1      = subGraphPtr->addConst('1', "const1");
  auto     gateOr1     = subGraphPtr->addGate(Gates::GateOr, "orAnd11");
  subGraphPtr->addEdges({inA, inB}, gateAnd1);
  subGraphPtr->addEdges({gateAnd1, const1}, gateOr1);
  subGraphPtr->addEdge(gateOr1, outC);
  subGraphPtr->addEdge(gateAnd1, outD);
  // LOG(INFO) << "First subGraph added!";

  GraphPtr subGraphPtr2 = std::make_shared<OrientedGraph>("testSubGraph2");
  inA                   = subGraphPtr2->addInput("a");
  inB                   = subGraphPtr2->addInput("b");
  outC                  = subGraphPtr2->addOutput("c");
  outD                  = subGraphPtr2->addOutput("d");
  gateAnd1              = subGraphPtr2->addGate(Gates::GateAnd, "andAB");
  const1                = subGraphPtr2->addConst('1', "const1");
  gateOr1               = subGraphPtr2->addGate(Gates::GateOr, "orAnd11");
  subGraphPtr2->addEdges({inA, inB}, gateAnd1);
  subGraphPtr2->addEdges({gateAnd1, const1}, gateOr1);
  subGraphPtr2->addEdge(gateOr1, outC);
  subGraphPtr2->addEdge(gateAnd1, outD);
  // LOG(INFO) << "Second subGraph added!";
  auto outs = subGraphPtr2->addSubGraph(subGraphPtr, {gateAnd1, gateOr1});
  subGraphPtr2->addEdge(outs[0], outC);
  subGraphPtr2->addEdge(outs[1], outD);

  GraphPtr graphPtr = std::make_shared<OrientedGraph>("testGraph");
  inA               = graphPtr->addInput("a");
  inB               = graphPtr->addInput("b");
  outC              = graphPtr->addOutput("c");
  outD              = graphPtr->addOutput("d");
  auto outE         = graphPtr->addOutput("e");
  // LOG(INFO) << "In/outs added to main graph";

  auto outs2        = graphPtr->addSubGraph(subGraphPtr2, {inA, inB});
  // LOG(INFO) << "SubGraphs added!";

  auto gateAnd2     = graphPtr->addGate(Gates::GateAnd, "andAB2");
  auto gateAnd3     = graphPtr->addGate(Gates::GateAnd, "andAB3");
  // LOG(INFO) << "Two AND gate added";

  graphPtr->addEdge(outs2[0], outC);
  graphPtr->addEdge(outs2[1], outD);

  graphPtr->addEdge(inB, outE);

  GraphPtr unrollGraphPtr = graphPtr->unrollGraph();
  // LOG(INFO) << "Graph unrolled!";

  std::filesystem::create_directories("./submodulesDOT");
  auto        strs     = unrollGraphPtr->toDOT(".", "testSubGraphUnroll3.dot");
  std::string curPath  = std::filesystem::current_path();
  std::string loadFile = loadStringFile(curPath + "/testSubGraphUnroll3.dot");
  loadFile             = loadFile.substr(loadFile.find("\n") + 2);
  // LOG(INFO) << "Printing DOT file: " << strs.first << "\n" << loadFile;
}

TEST(TestToDOT, SubGraph3) {
  initLogging("TestToDOT", "SubGraph3");
  GraphPtr subGraphPtr = std::make_shared<OrientedGraph>("testSubGraph");
  auto     inA         = subGraphPtr->addInput("a");
  auto     inB         = subGraphPtr->addInput("b");
  auto     outC        = subGraphPtr->addOutput("c");
  auto     outD        = subGraphPtr->addOutput("d");
  auto     gateAnd1    = subGraphPtr->addGate(Gates::GateAnd);
  auto     const1      = subGraphPtr->addConst('1');
  auto     gateOr1     = subGraphPtr->addGate(Gates::GateOr);
  subGraphPtr->addEdges({inA, inB}, gateAnd1);
  subGraphPtr->addEdges({gateAnd1, const1}, gateOr1);
  subGraphPtr->addEdge(gateOr1, outC);
  subGraphPtr->addEdge(gateAnd1, outD);
  // LOG(INFO) << "First subGraph added!";

  GraphPtr subGraphPtr2 = std::make_shared<OrientedGraph>("testSubGraph2");
  inA                   = subGraphPtr2->addInput("a");
  inB                   = subGraphPtr2->addInput("b");
  outC                  = subGraphPtr2->addOutput("c");
  outD                  = subGraphPtr2->addOutput("d");
  gateAnd1              = subGraphPtr2->addGate(Gates::GateAnd);
  const1                = subGraphPtr2->addConst('1');
  gateOr1               = subGraphPtr2->addGate(Gates::GateOr);
  subGraphPtr2->addEdges({inA, inB}, gateAnd1);
  subGraphPtr2->addEdges({gateAnd1, const1}, gateOr1);
  subGraphPtr2->addEdge(gateOr1, outC);
  subGraphPtr2->addEdge(gateAnd1, outD);
  // LOG(INFO) << "Second subGraph added!";
  auto outs = subGraphPtr2->addSubGraph(subGraphPtr, {gateAnd1, gateOr1});
  subGraphPtr2->addEdge(outs[0], outC);
  subGraphPtr2->addEdge(outs[1], outD);

  GraphPtr graphPtr = std::make_shared<OrientedGraph>("testGraph");
  inA               = graphPtr->addInput("a");
  inB               = graphPtr->addInput("b");
  outC              = graphPtr->addOutput("c");
  outD              = graphPtr->addOutput("d");
  auto outE         = graphPtr->addOutput("e");
  // LOG(INFO) << "In/outs added to main graph";

  auto outs2        = graphPtr->addSubGraph(subGraphPtr2, {inA, inB});
  // LOG(INFO) << "SubGraphs added!";

  graphPtr->addEdge(outs2[0], outC);
  graphPtr->addEdge(outs2[1], outD);

  graphPtr->addEdge(inB, outE);

  std::filesystem::create_directories("./submodulesDOT");
  auto        strs     = graphPtr->toDOT(".", "testSubGraph3.dot");
  std::string curPath  = std::filesystem::current_path();
  std::string loadFile = loadStringFile(curPath + "/testSubGraph3.dot");
  loadFile             = loadFile.substr(loadFile.find("\n") + 2);
  // LOG(INFO) << "Printing DOT file: " << strs.first << "\n" << loadFile;
}
