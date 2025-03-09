#include <CircuitGenGraph/GraphVertex.hpp>
#include <gtest/gtest.h>

#include <fstream>
#include <sstream>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

#include "TestSeqData.hpp"

using namespace CircuitGenGraph;

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

TEST(SequentialTests, TestFullTrigger) {
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
  const std::string fileName = "testSeq3.v";
  graph->toVerilog("./", fileName);
  testFile(fileName, TestData::SEQ_3_TEST);
}
