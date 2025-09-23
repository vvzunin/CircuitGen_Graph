#include "CircuitGenGraph/GraphUtils.hpp"
#include "CircuitGenGraph/GraphVertexBus.hpp"
#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>

#include <gtest/gtest.h>
#include <memory>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

using namespace CG_Graph;
inline void testFile(const std::string &fileName, std::string_view text) {
  std::ifstream file(fileName);

  ASSERT_TRUE(file.is_open()) << "Unable to open file: " << fileName;

  std::string line;
  // skip first two lines
  for (int i = 0; i < 2 && std::getline(file, line); ++i)
    ;

  std::stringstream buffer;
  buffer << file.rdbuf();

  EXPECT_EQ(buffer.str(), text) << "Содержимое файла не совпадает с ожидаемым.";
  file.close();
  ASSERT_EQ(std::remove(fileName.c_str()), 0)
      << "Не удалось удалить файл: " << fileName;
}

TEST(BusTest, SimpleBusPrintedSeparate) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr inputBus = graph->addInputBus("inputBus", 5);
  VertexPtr anotherInputBus = graph->addInputBus("anotherInputBus", 3);
  VertexPtr inputVertex = graph->addInput("inputVertex");
  VertexPtr andBus = graph->addGateBus(GateAnd, "andGate", 3);
  VertexPtr notBus = graph->addGateBus(GateNot, "notGate", 5);
  VertexPtr buf = graph->addGate(GateBuf, "buf");
  VertexPtr outputBusSecond = graph->addOutputBus("resAnd", 4);
  VertexPtr outputBus = graph->addOutputBus("resNot", 5);
  VertexPtr outputVertex = graph->addOutput("outputVertex");
  graph->addEdge(inputBus, notBus);
  graph->addEdge(notBus, outputBus);
  graph->addEdge(inputVertex, buf);
  graph->addEdge(buf, outputVertex);
  graph->addEdge(anotherInputBus, andBus);
  graph->addEdge(buf, andBus);
  graph->addEdge(andBus, outputBusSecond);
  EXPECT_NO_THROW(graph->toVerilogBusEnabledAsOneBit("./", "oneBitVerilog.v"));
  graph->toVerilogBusEnabledAsOneBit("./", "lalala4.v");
  testFile(
      "./oneBitVerilog.v",
      "module graph_5(\n\tinputBus_0, inputBus_1, inputBus_2, inputBus_3, "
      "inputBus_4, anotherInputBus_0, anotherInputBus_1, anotherInputBus_2, "
      "inputVertex, \n\tresAnd_0, resAnd_1, resAnd_2, resAnd_3, resNot_0, "
      "resNot_1, resNot_2, resNot_3, resNot_4, outputVertex\n);\n\t// Writing "
      "inputs\n\tinput inputVertex;\n\tinput anotherInputBus_0, "
      "anotherInputBus_1, anotherInputBus_2;\n\tinput inputBus_0, inputBus_1, "
      "inputBus_2, inputBus_3, inputBus_4;\n\t\n\t// Writing outputs\n\toutput "
      "outputVertex;\n\toutput resAnd_0, resAnd_1, resAnd_2, "
      "resAnd_3;\n\toutput resNot_0, resNot_1, resNot_2, resNot_3, "
      "resNot_4;\n\t\n\t// Writing gates for main graph\n\twire buf;\n\twire "
      "andGate_0, andGate_1, andGate_2;\n\twire notGate_0, notGate_1, "
      "notGate_2, notGate_3, notGate_4;\n\t\n\t\n\tassign andGate_0 = "
      "anotherInputBus_0 & buf;\n\tassign andGate_1 = "
      "anotherInputBus_1;\n\tassign andGate_2 = "
      "anotherInputBus_2;\n\t\n\tassign notGate_0 = ~inputBus_0;\n\tassign "
      "notGate_1 = ~inputBus_1;\n\tassign notGate_2 = ~inputBus_2;\n\tassign "
      "notGate_3 = ~inputBus_3;\n\tassign notGate_4 = "
      "~inputBus_4;\n\t\n\tassign buf = inputVertex;\n\n\n\tassign resAnd_0 = "
      "andGate_0;\n\tassign resAnd_1 = andGate_1;\n\tassign resAnd_2 = "
      "andGate_2;\n\t\n\tassign resNot_0 = notGate_0;\n\tassign resNot_1 = "
      "notGate_1;\n\tassign resNot_2 = notGate_2;\n\tassign resNot_3 = "
      "notGate_3;\n\tassign resNot_4 = notGate_4;\n\t\n\tassign outputVertex = "
      "buf;\n\nendmodule\n");
}
TEST(BusTest, oneBitVerilogForGates) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr inputBus = graph->addInputBus("inputBus", 5);
  VertexPtr anotherInputBus = graph->addInputBus("anotherInputBus", 3);
  VertexPtr inputVertex = graph->addInput("inputVertex");
  VertexPtr constBus = graph->addConstBus("", 7);
  VertexPtr andBus = graph->addGateBus(GateAnd, "andGate", 3);
  VertexPtr notBus = graph->addGateBus(GateNot, "notGate", 5);
  VertexPtr buf = graph->addGate(GateBuf, "buf");
  graph->addEdge(inputBus, notBus);
  graph->addEdge(inputVertex, buf);
  graph->addEdge(anotherInputBus, andBus);
  graph->addEdge(buf, andBus);
  graph->addEdge(constBus, andBus);
  EXPECT_EQ(GraphVertexBus::getBusPointer(andBus)->toOneBitVerilog(),
            "assign andGate_0 = anotherInputBus_0 & buf & const_0_0;\n"
            "\tassign andGate_1 = anotherInputBus_1 & const_0_1;\n"
            "\tassign andGate_2 = anotherInputBus_2 & const_0_2;\n\t");
}
TEST(BusTest, severalBusModulePrintedToVerilog) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr inputBus = graph->addInputBus("inputBus", 5);
  VertexPtr inputVertex = graph->addInput("inputVertex");
  VertexPtr notBus = graph->addGateBus(GateNot, "not", 5);
  VertexPtr buf = graph->addGate(GateBuf, "buf");
  VertexPtr outputBus = graph->addOutputBus("res", 5);
  VertexPtr outputVertex = graph->addOutput("outputVertex");
  graph->addEdge(inputBus, notBus);
  graph->addEdge(notBus, outputBus);
  graph->addEdge(inputVertex, buf);
  graph->addEdge(buf, outputVertex);
  EXPECT_NO_THROW(graph->toVerilogBusEnabled("./", "lalala.v"));
  graph->toVerilogBusEnabled("./", "lalala3.v");
  testFile(
      "./lalala.v",
      "module graph_5(\n\tinputBus, inputVertex, \n\tres, "
      "outputVertex\n);\n\t// Writing inputs\n\tinput inputVertex;\n\tinput "
      "[4:0] inputBus;\n\n\t// Writing outputs\n\toutput "
      "outputVertex;\n\toutput [4:0] res;\n\n\t// Writing gates for main "
      "graph\n\twire buf;\n\twire [4:0] not;\n\n\t\n\tassign not = "
      "~inputBus;\n\tassign buf = inputVertex;\n\n\tassign res = "
      "not;\n\tassign outputVertex = buf;\nendmodule\n");
}
TEST(BusTest, SimpleBusAdded) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr v = graph->addInputBus("lalala", 5);
  VertexPtr v2 = graph->addGateBus(GateNot, "lalala2", 5);
  graph->addEdge(v, v2);
  EXPECT_NO_THROW(graph->toVerilog("./"));
}
TEST(BusTest, SequentialPrintTest) {
}