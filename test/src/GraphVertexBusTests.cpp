#include "CircuitGenGraph/GraphUtils.hpp"
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
TEST(BusTest, SimpleBusPrinted) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr v = graph->addInputBus("input_", 5);
  VertexPtr v2 = graph->addGateBus(GateNot, "not", 5);
  VertexPtr v3 = graph->addOutputBus("res", 5);
  graph->addEdge(v, v2);
  graph->addEdge(v2, v3);
  EXPECT_NO_THROW(graph->toVerilogBusEnabled("./", "lalala.v"));
  graph->toVerilogBusEnabled("./", "lalala2.v");
  testFile("./lalala.v", "module graph_5(\n"
                         "\tinput,\n"
                         "\tres\n"
                         ");\n"
                         "\t// Writing inputs\n"
                         "\tinput [4:0] input_;\n"
                         "\t// Writing outputs\n"
                         "\toutput [4:0] res;\n"
                         "\t// Writing gates for main graph\n"
                         "wire [4:0] not;\n\n"
                         "\tassign not = ~input_;\n\n\n"
                         "\tassign res = not;\n"
                         "\t\n"
                         "\tendmodule");
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
  graph->addEdge(andBus, outputBusSecond);
  EXPECT_NO_THROW(graph->toVerilogBusEnabled("./", "lalala.v"));
  graph->toVerilogBusEnabledAsOneBit("./", "lalala4.v");
  testFile("./lalala.v",
           "module graph_5(\n"
           "\tinput,\n"
           "\tres\n"
           ");\n"
           "\t// Writing inputs\n"
           "\tinput input_0, input__1, input__2, input__3, input__4;\n"
           "\t// Writing outputs\n"
           "\toutput res_0, res_1, res_2, res_3, res_4;\n"
           "\t// Writing gates for main graph\n"
           "wire not_0, not_1, not_2, not_3, not_4;\n\n"
           "\tassign not_0 = ~input__0;\n"
           "\tassign not_1 = ~input__1;\n"
           "\tassign not_2 = ~input__2;\n"
           "\tassign not_3 = ~input__3;\n"
           "\tassign not_4 = ~input__4;\n"
           "\tassign res_0 = not_0;\n"
           "\tassign res_1 = not_1;\n"
           "\tassign res_2 = not_2;\n"
           "\tassign res_3 = not_3;\n"
           "\tassign res_4 = not_4;\n"
           "\t\n"
           "\tendmodule");
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
  testFile("./lalala.v", "module graph_5(\n"
                         "\tinput,\n"
                         "\tres\n"
                         ");\n"
                         "\t// Writing inputs\n"
                         "\tinput [4:0] input_;\n"
                         "\t// Writing outputs\n"
                         "\toutput [4:0] res;\n"
                         "\t// Writing gates for main graph\n"
                         "wire [4:0] not;\n\n"
                         "\tassign not = ~input_;\n\n\n"
                         "\tassign res = not;\n"
                         "\t\n"
                         "\tendmodule");
}
TEST(BusTest, SimpleBusAdded) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr v = graph->addInputBus("lalala", 5);
  VertexPtr v2 = graph->addGateBus(GateNot, "lalala2", 5);
  graph->addEdge(v, v2);
  EXPECT_NO_THROW(graph->toVerilog("./"));
}