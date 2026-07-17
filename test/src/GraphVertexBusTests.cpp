#include <CircuitGenGraph/GraphUtils.hpp>
#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <CircuitGenGraph/GraphVertexBus.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>

#include "TestSeqData.hpp"

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
  OrientedGraph::resetCounter();
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
  testFile(
      "./oneBitVerilog.v",
      "module graph_0(\n\tinputBus_0, inputBus_1, inputBus_2, inputBus_3, "
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
      "anotherInputBus_1 & 1'bx;\n\tassign andGate_2 = "
      "anotherInputBus_2 & 1'bx;\n\t\n\tassign notGate_0 = "
      "~inputBus_0;\n\tassign "
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
  VertexPtr constBus = graph->addConstBus("const_0", 7);
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
            "\tassign andGate_1 = anotherInputBus_1 & 1'bx & const_0_1;\n"
            "\tassign andGate_2 = anotherInputBus_2 & 1'bx & const_0_2;\n\t");
}
TEST(BusTest, oneBitVerilogForSequentials) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr inputBus = graph->addInputBus("inputBus", 5);
  VertexPtr anotherInputBus = graph->addInputBus("anotherInputBus", 3);
  VertexPtr inputVertex = graph->addInput("inputVertex");
  VertexPtr constBus = graph->addConstBus("const_0", 7);
  VertexPtr firstFF = graph->addSequentialBus(SequentialTypes::ff, inputVertex,
                                              inputBus, "firstFF", 5);
  VertexPtr latch = graph->addSequentialBus(
      latchcs, inputVertex, inputBus, firstFF, anotherInputBus, "Latch", 6);
  VertexPtr secondFF =
      graph->addSequentialBus(ff, anotherInputBus, inputBus, "secondFF", 3);
  // VertexPtr emptyOutputLatch =
  graph->addSequentialBus(latchrs, inputVertex, inputBus, constBus,
                          anotherInputBus, "emptyLatch", 8);
  VertexPtr gateBus = graph->addGateBus(GateOr, "or", 4);
  graph->addEdge(latch, gateBus);
  graph->addEdge(secondFF, gateBus);
  EXPECT_EQ(GraphVertexBus::getBusPointer(firstFF)->toOneBitVerilog(),
            "ff firstFF_0_ins (.data(inputBus_0), .clk(inputVertex), "
            ".q(firstFF_0))\n"
            "\tff firstFF_1_ins (.data(inputBus_1), .clk(inputVertex), "
            ".q(firstFF_1))\n"
            "\tff firstFF_2_ins (.data(inputBus_2), .clk(inputVertex), "
            ".q(firstFF_2))\n"
            "\tff firstFF_3_ins (.data(inputBus_3), .clk(inputVertex), "
            ".q(firstFF_3))\n"
            "\tff firstFF_4_ins (.data(inputBus_4), .clk(inputVertex), "
            ".q(firstFF_4))\n\n");
  EXPECT_NO_THROW(graph->toVerilogBusEnabledAsOneBit("./", "sequentialTest.v"));
  testFile("./sequentialTest.v", TestData::SEQ_8_TEST);
}
TEST(BusTest, severalBusModulePrintedToVerilog) {
  OrientedGraph::resetCounter();
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
  testFile(
      "./lalala.v",
      "module graph_0(\n\tinputBus, inputVertex, \n\tres, "
      "outputVertex\n);\n\t// Writing inputs\n\tinput inputVertex;\n\tinput "
      "[4:0] inputBus;\n\n\t// Writing outputs\n\toutput "
      "outputVertex;\n\toutput [4:0] res;\n\n\t// Writing gates for main "
      "graph\n\twire buf;\n\twire [4:0] not;\n\n\t\n\tassign not = "
      "~inputBus;\n\tassign buf = inputVertex;\n\n\tassign res = "
      "not;\n\tassign outputVertex = buf;\nendmodule\n");
}

TEST(BusTest, MixedWidthBusDeclarationsAreSeparateStatements) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>("mixedWidths");
  graph->addInputBus("wide", 5);
  graph->addInputBus("narrow", 3);
  graph->addInput("bit");
  EXPECT_NO_THROW(graph->toVerilogBusEnabled("./", "mixed_widths.v"));
  testFile("./mixed_widths.v",
           "module mixedWidths(\n\twide, narrow, bit\n\t);\n\t// Writing "
           "inputs\n\tinput bit;\n\tinput [2:0] narrow;\n\tinput [4:0] "
           "wide;\n\n\t\nendmodule\n");
}
TEST(BusTest, SliceToVerilog) {
  GraphPtr graph = std::make_shared<OrientedGraph>("sliceTestGraph");
  VertexPtr v = graph->addInputBus("inputBus", 5);
  VertexPtr v1 = graph->addSliceBus(v, 0, 2, "sliceBus");
  EXPECT_NO_THROW(graph->toVerilog("./", "micro_to_verilog_test.v"));
  EXPECT_EQ(v1->toVerilog(), "assign sliceBus = inputBus[1:0];\n");
  testFile("micro_to_verilog_test.v", "module sliceTestGraph(\n"
                                      "\tinputBus\n"
                                      "\t);\n"
                                      "\t// Writing inputs\n"
                                      "\tinput inputBus;\n"
                                      "\t// Writing gates for main graph\n"
                                      "\twire sliceBus;\n\t\n"
                                      "\tassign sliceBus = inputBus[1:0];\n\n\n"
                                      "endmodule\n");
}

TEST(BusTest, SliceOneBitVerilogUsesLocalIndices) {
  GraphPtr graph = std::make_shared<OrientedGraph>("sliceOneBit");
  VertexPtr bus = graph->addInputBus("bus", 5);
  VertexPtr slice = graph->addSliceBus(bus, 2, 3, "sl");
  EXPECT_EQ(GraphVertexBus::getBusPointer(slice)->toOneBitVerilog(),
            "assign sl_0 = bus_2;\n\t"
            "assign sl_1 = bus_3;\n\t"
            "assign sl_2 = bus_4;\n\t");
}
TEST(BusTest, SliceErrorsWhenIncorrect) {
  std::stringstream buffer;
  // перенаправляем cerr
  std::streambuf *old = std::cerr.rdbuf(buffer.rdbuf());
  GraphPtr graph = std::make_shared<OrientedGraph>();

  VertexPtr input = graph->addInputBus("input_", 5);
  VertexPtr s1 = graph->addSliceBus(input, 0, 0);
  EXPECT_EQ(GraphVertexBus::getBusPointer(s1)->getWidth(), 1);
  VertexPtr s2 = graph->addSliceBus(input, 2, 6);
  EXPECT_EQ(GraphVertexBus::getBusPointer(s2)->getWidth(), 3);
  VertexPtr sBeginPastEnd = graph->addSliceBus(input, 6, 1);
  EXPECT_EQ(GraphVertexBus::getBusPointer(sBeginPastEnd)->getWidth(), 1);
  EXPECT_EQ(static_cast<GraphVertexBusSlice *>(sBeginPastEnd)->getSliceSuffix(),
            "[4];\n");
  VertexPtr gate = graph->addGate(GateNor);
  VertexPtr s3 = graph->addSliceBus(gate, 2, 6);
  EXPECT_EQ(GraphVertexBus::getBusPointer(s3)->getWidth(), 1);
  // возвращаем старый буфер
  std::cerr.rdbuf(old);
  std::string output = buffer.str();
  EXPECT_EQ(
      output,
      "Width of bus must be an positive value\n"
      "Width of slice is out of range of bus\n"
      "Width of slice is out of range of bus\n"
      "Created slice with name "
      "(name is not defined) is connected with vertex, which is not a bus\n");
}

TEST(BusTest, BusConstantVerilogInstanceAndOneBitLiterals) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr constBus = graph->addConstBus("const_0", 3);
  auto *busConst = static_cast<GraphVertexBusConstant *>(
      GraphVertexBus::getBusPointer(constBus));
  EXPECT_EQ(busConst->getVerilogInstance(), "wire [2:0] const_0;");
  EXPECT_EQ(busConst->toOneBitVerilog(), "assign const_0_0 = 1'bx;\n\t"
                                         "assign const_0_1 = 1'bx;\n\t"
                                         "assign const_0_2 = 1'bx;\n\t\n");

  busConst->setValue("10");
  EXPECT_EQ(busConst->toVerilog(), "assign const_0 = 3'bx10;");
  EXPECT_EQ(busConst->toOneBitVerilog(), "assign const_0_0 = 1'b0;\n\t"
                                         "assign const_0_1 = 1'b1;\n\t"
                                         "assign const_0_2 = 1'bx;\n\t\n");
}

TEST(BusTest, UnaryBusGateOneBitKeepsScalarInputName) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr bit = graph->addInput("bit");
  VertexPtr notBus = graph->addGateBus(GateNot, "notGate", 1);
  graph->addEdge(bit, notBus);
  EXPECT_EQ(GraphVertexBus::getBusPointer(notBus)->toOneBitVerilog(),
            "assign notGate_0 = ~bit;\n\t");
}

TEST(BusTest, UnaryBusGateOneBitPadsMissingInputBits) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr narrow = graph->addInputBus("narrow", 2);
  VertexPtr notBus = graph->addGateBus(GateNot, "notGate", 4);
  graph->addEdge(narrow, notBus);
  EXPECT_EQ(GraphVertexBus::getBusPointer(notBus)->toOneBitVerilog(),
            "assign notGate_0 = ~narrow_0;\n\t"
            "assign notGate_1 = ~narrow_1;\n\t"
            "assign notGate_2 = ~1'bx;\n\t"
            "assign notGate_3 = ~1'bx;\n\t");
}

TEST(BusTest, BusOutputOneBitKeepsScalarDriverName) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr bit = graph->addInput("bit");
  VertexPtr buf = graph->addGate(GateBuf, "buf");
  VertexPtr out = graph->addOutputBus("out", 2);
  graph->addEdge(bit, buf);
  graph->addEdge(buf, out);
  EXPECT_EQ(GraphVertexBus::getBusPointer(out)->toOneBitVerilog(),
            "assign out_0 = buf;\n\t");
}
TEST(BusTest, ConcatenationToVerilog) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  VertexPtr v = graph->addInputBus("input_", 5);
  VertexPtr v2 = graph->addGateBus(GateNot, "not", 5);
  VertexPtr v3 = graph->addGateBus(GateConcatenation, "concat", 5);
  VertexPtr v4 = graph->addConst('1', "const_");
  graph->addEdge(v, v3);
  graph->addEdge(v2, v3);
  graph->addEdge(v4, v3);
  EXPECT_EQ(v3->toVerilog(), "assign concat = { input_ , not , const_ };");
}