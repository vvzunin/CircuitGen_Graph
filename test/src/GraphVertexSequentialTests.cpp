#include <CircuitGenGraph/GraphVertex.hpp>
#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <sstream>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

#include "TestSeqData.hpp"

using namespace CG_Graph;

/*
need to realize updateLevel
*/

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

TEST(SequentialTests, TestSimpleTrigger) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *seq = graph->addSequential(ff, clk, data, "q");

  EXPECT_EQ(seq->toVerilog(), "always @(posedge clk) begin\n"
                              "\t\tq <= data;\n"
                              "\tend\n");

  auto *out = graph->addOutput("res");
  graph->addEdge(seq, out);

  const std::string fileName = "testSeq1.v";
  graph->toVerilog("./", fileName);
  testFile(fileName, TestData::SEQ_1_TEST);
}

TEST(SequentialTests, TestSimpleLatch) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *en = graph->addInput("en");
  auto *data = graph->addInput("data");
  auto *seq = graph->addSequential(latch, en, data, "q");

  EXPECT_EQ(seq->toVerilog(), "always @(*) begin\n"
                              "\t\tif (en) q <= data;\n"
                              "\tend\n");

  auto *out = graph->addOutput("res");
  graph->addEdge(seq, out);

  const std::string fileName = "testSeq2.v";
  graph->toVerilog("./", fileName);
  testFile(fileName, TestData::SEQ_2_TEST);
}

TEST(SequentialTests, TestLatchRstN) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *en = graph->addInput("en");
  auto *data = graph->addInput("data");
  auto *rst = graph->addInput("rst");
  auto *seq = graph->addSequential(latchr, en, data, rst, "q");

  EXPECT_EQ(seq->toVerilog(), "always @(*) begin\n"
                              "\t\tif (!rst) q <= 1'b0;\n"
                              "\t\telse if (en) q <= data;\n"
                              "\tend\n");

  auto *out = graph->addOutput("res");
  graph->addEdge(seq, out);

  GraphVertexSequential *seqChild = static_cast<GraphVertexSequential *>(seq);
  EXPECT_EQ(en, seqChild->getEn());
  EXPECT_EQ(data, seqChild->getData());
  EXPECT_EQ(rst, seqChild->getRst());
  EXPECT_FALSE(seqChild->getClk());

  const std::string fileName = "testSeq3.v";
  graph->toVerilog("./", fileName);
  testFile(fileName, TestData::SEQ_3_TEST);
}

TEST(SequentialTests, TestLatchClrSet) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *en = graph->addInput("en");
  auto *data = graph->addInput("data");
  auto *clr = graph->addInput("clr");
  auto *set = graph->addInput("set");
  auto *seq = graph->addSequential(latchcs, en, data, clr, set, "q");

  EXPECT_EQ(seq->toVerilog(), "always @(*) begin\n"
                              "\t\tif (clr) q <= 1'b0;\n"
                              "\t\telse if (set) q <= 1'b1;\n"
                              "\t\telse if (en) q <= data;\n"
                              "\tend\n");

  auto *out = graph->addOutput("res");
  graph->addEdge(seq, out);

  const std::string fileName = "testSeq4.v";
  graph->toVerilog("./", fileName);
  testFile(fileName, TestData::SEQ_4_TEST);
}

TEST(SequentialTests, TestFullTrigger) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *rst = graph->addInput("rst");
  auto *set = graph->addInput("set");
  auto *en = graph->addInput("en");
  auto *seq = graph->addSequential(ffrse, clk, data, rst, set, en, "q");

  EXPECT_EQ(seq->toVerilog(), "always @(posedge clk) begin\n"
                              "\t\tif (!rst) q <= 1'b0;\n"
                              "\t\telse if (set) q <= 1'b1;\n"
                              "\t\telse if (en) q <= data;\n"
                              "\tend\n");

  auto *out = graph->addOutput("res");
  graph->addEdge(seq, out);

  // graph->toDOT("../../", "testSeq3.dot");
  const std::string fileName = "testSeq5.v";
  graph->toVerilog("./", fileName);
  testFile(fileName, TestData::SEQ_5_TEST);
}

TEST(SequentialTests, TestNegedgeTriggerAsyncRstN_Set) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *rst_n = graph->addInput("rst_n");
  auto *set = graph->addInput("set");
  auto *seq = graph->addSequential(naffrs, clk, data, rst_n, set, "q");

  EXPECT_EQ(seq->toVerilog(), "always @(negedge clk or negedge rst_n) begin\n"
                              "\t\tif (!rst_n) q <= 1'b0;\n"
                              "\t\telse if (set) q <= 1'b1;\n"
                              "\t\telse q <= data;\n"
                              "\tend\n");

  auto *out = graph->addOutput("res");
  graph->addEdge(seq, out);

  // graph->toDOT("../../", "testSeq3.dot");
  const std::string fileName = "testSeq6.v";
  graph->toVerilog("./", fileName);
  testFile(fileName, TestData::SEQ_6_TEST);
}

TEST(SequentialTests, TestTriggerHashCycle) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *rst_n = graph->addInput("rst_n");
  auto *en = graph->addInput("en");
  auto *seq = graph->addSequential(affre, clk, data, rst_n, en, "q");

  EXPECT_EQ(seq->toVerilog(), "always @(posedge clk or negedge rst_n) begin\n"
                              "\t\tif (!rst_n) q <= 1'b0;\n"
                              "\t\telse if (en) q <= data;\n"
                              "\tend\n");

  auto *out = graph->addOutput("res");
  graph->addEdge(seq, out);

  // graph->toDOT("../../", "testSeq3.dot");
  const std::string fileName = "testSeq7.v";
  graph->toVerilog("./", fileName);
  testFile(fileName, TestData::SEQ_7_TEST);
}

TEST(SequentialTests, TestTriggerHash) {
  OrientedGraph::resetCounter();
  GraphPtr graph1 = std::make_shared<OrientedGraph>();
  auto *clk1 = graph1->addInput("clk");
  auto *data1 = graph1->addInput("data");
  auto *rst_n1 = graph1->addInput("rst_n");
  auto *en1 = graph1->addInput("en");
  auto *seq1 = graph1->addSequential(affre, clk1, data1, rst_n1, en1, "q");

  auto *out1 = graph1->addOutput("res");
  graph1->addEdge(seq1, out1);

  GraphPtr graph2 = std::make_shared<OrientedGraph>();
  auto *clk = graph2->addInput("clk");
  auto *data = graph2->addInput("data");
  auto *rst_n = graph2->addInput("rst_n");
  auto *set = graph2->addInput("set");
  auto *seq = graph2->addSequential(naffrs, clk, data, rst_n, set, "q");

  auto *out = graph2->addOutput("res");
  graph2->addEdge(seq, out);

  EXPECT_NE(graph1->calculateHash(), graph2->calculateHash());
}

TEST(SequentialTests, TestTriggerAsyncRstN_En) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *rst_n = graph->addInput("rst_n");
  auto *en = graph->addInput("en");

  auto *en_or = graph->addGate(GateOr);
  auto *en_and = graph->addGate(GateOr);
  graph->addEdge(en, en_or);
  graph->addEdge(en, en_and);

  auto *seq1 = graph->addSequential(affre, clk, data, rst_n, en_or, "q2");
  auto *seq2 = graph->addSequential(affre, clk, data, rst_n, en_and, "q2");

  graph->addEdge(seq2, en_or);
  graph->addEdge(seq1, en_and);

  auto *out1 = graph->addOutput("res1");
  auto *out2 = graph->addOutput("res2");
  graph->addEdge(seq1, out1);
  graph->addEdge(seq2, out2);

  EXPECT_TRUE(graph->calculateHash().size());
}

TEST(ErrorOutputTest, CapturesCerr) {
  std::stringstream buffer;
  // перенаправляем cerr
  std::streambuf *old = std::cerr.rdbuf(buffer.rdbuf());

  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *en = graph->addInput("en");
  auto *data = graph->addInput("data");
  auto *rst = graph->addInput("rst");
  graph->addSequential(latchrs, en, data, rst, "q");

  // возвращаем старый буфер
  std::cerr.rdbuf(old);

  std::string output = buffer.str();
  EXPECT_EQ(output, "Invalid flag found in used type: SET\n");
}
