#include <CircuitGenGraph/GraphVertex.hpp>
#include <gtest/gtest.h>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

using namespace CircuitGenGraph;

TEST(SequentialTests, TestSimpleTrigger) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *seq = graph->addSequential(ff, clk, data, "q");

  EXPECT_EQ(seq->toVerilog(), "always @(posedge clk) begin\n"
                              "    q <= data;\n"
                              "  end\n");

  auto *out = graph->addOutput("res");
  graph->addEdge(seq, out);

  auto strs = graph->toVerilog("../../", "testSeq1.v");
}

TEST(SequentialTests, TestSimpleLatch) {
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *en = graph->addInput("en");
  auto *data = graph->addInput("data");
  auto *seq = graph->addSequential(latch, en, data, "q");

  EXPECT_EQ(seq->toVerilog(), "always @(*) begin\n"
                              "    if (en) q <= data;\n"
                              "  end\n");

  auto *out = graph->addOutput("res");
  graph->addEdge(seq, out);

  auto strs = graph->toVerilog("../../", "testSeq2.v");
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
                              "    if (!rst) q <= 1'b0;\n"
                              "    else if (set) q <= 1'b1;\n"
                              "    else if (en) q <= data;\n"
                              "  end\n");

  auto *out = graph->addOutput("res");
  graph->addEdge(seq, out);

  auto strs = graph->toVerilog("../../", "testSeq3.v");
}
