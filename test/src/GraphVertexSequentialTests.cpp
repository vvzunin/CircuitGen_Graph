#include "CircuitGenGraph/GraphUtils.hpp"
#include "CircuitGenGraph/GraphVertexBase.hpp"
#include "CircuitGenGraph/OrientedGraph.hpp"
#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/SequentialVerilogStorage.hpp>

#include <gtest/gtest.h>
#include <fmt/format.h>

#include <memory>
#include <string>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

#include <fstream>
#include <iostream>
#include <sstream>

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

  EXPECT_EQ(seq->toVerilog(),
            "// EN signal \"en\" - when it is in a logical one state, "
            "trigger writes data to the output\n"
            "\talways @(*) begin\n"
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

  EXPECT_EQ(seq->toVerilog(),
            "// RST signal \"rst\" - when it is in a logical zero "
            "state, trigger writes logical zero to the output\n"
            "\t// EN signal \"en\" - when it is in a logical one state, "
            "trigger writes data to the output\n"
            "\talways @(*) begin\n"
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

  EXPECT_EQ(seq->toVerilog(),
            "// CLR signal \"clr\" - when it is in a logical one "
            "state, trigger writes logical zero to the output\n"
            "\t// SET signal \"set\" - when it is in a logical one "
            "state, trigger writes logical one to the output\n"
            "\t// EN signal \"en\" - when it is in a logical one state, "
            "trigger writes data to the output\n"
            "\talways @(*) begin\n"
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

  EXPECT_EQ(seq->toVerilog(),
            "// RST signal \"rst\" - when it is in a logical zero state, "
            "trigger writes logical zero to the output\n"
            "\t// SET signal \"set\" - when it is in a logical one state, "
            "trigger writes logical one to the output\n"
            "\t// EN signal \"en\" - when it is in a logical one state, "
            "trigger writes data to the output\n"
            "\talways @(posedge clk) begin\n"
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
unsigned short countSignalsInType(SequentialTypes i_type) {
  return 1 + bool(i_type & ff) + bool(i_type & RST) + bool(i_type & CLR) +
         bool(i_type & EN) + bool(i_type & SET);
}

TEST(SequentialTests, TestNegedgeTriggerAsyncRstN_Set) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *rst_n = graph->addInput("rst_n");
  auto *set = graph->addInput("set");
  auto *seq = graph->addSequential(naffrs, clk, data, rst_n, set, "q");

  EXPECT_EQ(seq->toVerilog(),
            "// RST signal \"rst_n\" - when it is in a logical zero "
            "state, trigger writes logical zero to the output\n"
            "\t// RST signal \"rst_n\" is async - always block enables on "
            "negedge of RST\n"
            "\t// SET signal \"set\" - when it is in a logical one state, "
            "trigger writes logical one to the output\n"
            "\talways @(negedge clk or negedge rst_n) begin\n"
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
  auto *seq = graph->addSequential(affre, clk, data, en, rst_n, "q");

  EXPECT_EQ(seq->toVerilog(),
            "// RST signal \"rst_n\" - when it is in a logical zero state, "
            "trigger writes logical zero to the output\n"
            "\t// RST signal \"rst_n\" is async - always block enables on "
            "negedge of RST\n"
            "\t// EN signal \"en\" - when it is in a logical one state, "
            "trigger writes data to the output\n"
            "\talways @(posedge clk or negedge rst_n) begin\n"
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
  // affre wire order: EN then RST (see OrientedGraph::addSequential note)
  auto *seq1 = static_cast<GraphVertexSequential *>(
      graph1->addSequential(affre, clk1, data1, en1, rst_n1, "q"));
  EXPECT_EQ(seq1->getEn(), en1);
  EXPECT_EQ(seq1->getRst(), rst_n1);

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

  auto *seq1 = static_cast<GraphVertexSequential *>(
      graph->addSequential(affre, clk, data, en_or, rst_n, "q2"));
  auto *seq2 = static_cast<GraphVertexSequential *>(
      graph->addSequential(affre, clk, data, en_and, rst_n, "q2"));
  EXPECT_EQ(seq1->getEn(), en_or);
  EXPECT_EQ(seq1->getRst(), rst_n);
  EXPECT_EQ(seq2->getEn(), en_and);
  EXPECT_EQ(seq2->getRst(), rst_n);

  graph->addEdge(seq2, en_or);
  graph->addEdge(seq1, en_and);

  auto *out1 = graph->addOutput("res1");
  auto *out2 = graph->addOutput("res2");
  graph->addEdge(seq1, out1);
  graph->addEdge(seq2, out2);

  EXPECT_TRUE(graph->calculateHash().size());
}

TEST(SequentialTests, LatchUpdateValueLevelSensitive) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("latch_sim");
  auto *en = graph->addInput("en");
  auto *data = graph->addInput("data");
  auto *q = graph->addSequential(latch, en, data, "q");
  graph->addEdge(q, graph->addOutput("y"));

  // Transparent: en=1 captures data=1
  EXPECT_EQ(graph->graphSimulation({'1', '1'}), (std::vector<char>{'1'}));
  // Hold: en=0 keeps Q even if data changes
  EXPECT_EQ(graph->graphSimulation({'0', '0'}), (std::vector<char>{'1'}));
  // Transparent again
  EXPECT_EQ(graph->graphSimulation({'1', '0'}), (std::vector<char>{'0'}));
}

TEST(SequentialTests, FlipFlopUpdateValuePosedge) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("ff_sim");
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *q = graph->addSequential(ff, clk, data, "q");
  graph->addEdge(q, graph->addOutput("y"));

  // clk=0: no edge yet → X
  EXPECT_EQ(graph->graphSimulation({'0', '1'}), (std::vector<char>{'x'}));
  // posedge with data=1 → capture
  EXPECT_EQ(graph->graphSimulation({'1', '1'}), (std::vector<char>{'1'}));
  // clk stays 1, data flips → hold
  EXPECT_EQ(graph->graphSimulation({'1', '0'}), (std::vector<char>{'1'}));
  // negedge then posedge with data=0 → capture 0
  EXPECT_EQ(graph->graphSimulation({'0', '0'}), (std::vector<char>{'1'}));
  EXPECT_EQ(graph->graphSimulation({'1', '0'}), (std::vector<char>{'0'}));
}

TEST(SequentialTests, FlipFlopFirstVectorNoFalseEdge) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("ff_first");
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *q = graph->addSequential(ff, clk, data, "q");
  graph->addEdge(q, graph->addOutput("y"));

  // d_prevClk starts as 'x'; clk=1 alone is not a 0→1 edge
  EXPECT_EQ(graph->graphSimulation({'1', '1'}), (std::vector<char>{'x'}));
}

TEST(SequentialTests, FlipFlopEnableHoldsOnPosedge) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("ffe_sim");
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *en = graph->addInput("en");
  auto *q = graph->addSequential(ffe, clk, data, en, "q");
  graph->addEdge(q, graph->addOutput("y"));

  EXPECT_EQ(graph->graphSimulation({'0', '1', '1'}), (std::vector<char>{'x'}));
  EXPECT_EQ(graph->graphSimulation({'1', '1', '1'}), (std::vector<char>{'1'}));
  EXPECT_EQ(graph->graphSimulation({'0', '0', '0'}), (std::vector<char>{'1'}));
  EXPECT_EQ(graph->graphSimulation({'1', '0', '0'}), (std::vector<char>{'1'}));
}

TEST(SequentialTests, LatchRstForcesZero) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("latchr_sim");
  auto *en = graph->addInput("en");
  auto *data = graph->addInput("data");
  auto *rst = graph->addInput("rst");
  auto *q = graph->addSequential(latchr, en, data, rst, "q");
  graph->addEdge(q, graph->addOutput("y"));

  EXPECT_EQ(graph->graphSimulation({'1', '1', '1'}), (std::vector<char>{'1'}));
  EXPECT_EQ(graph->graphSimulation({'1', '1', '0'}), (std::vector<char>{'0'}));
}

TEST(SequentialTests, LatchClrActiveHigh) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("latchc_sim");
  auto *en = graph->addInput("en");
  auto *data = graph->addInput("data");
  auto *clr = graph->addInput("clr");
  auto *q = graph->addSequential(latchc, en, data, clr, "q");
  graph->addEdge(q, graph->addOutput("y"));

  EXPECT_EQ(graph->graphSimulation({'1', '1', '0'}), (std::vector<char>{'1'}));
  // CLR is active-high (unlike RST)
  EXPECT_EQ(graph->graphSimulation({'1', '1', '1'}), (std::vector<char>{'0'}));
  EXPECT_EQ(graph->graphSimulation({'1', '0', '0'}), (std::vector<char>{'0'}));
}

TEST(SequentialTests, SyncFfrClearsOnlyOnPosedge) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("ffr_sync");
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *rst = graph->addInput("rst");
  auto *q = graph->addSequential(ffr, clk, data, rst, "q");
  graph->addEdge(q, graph->addOutput("y"));

  EXPECT_EQ(graph->graphSimulation({'0', '1', '1'}), (std::vector<char>{'x'}));
  EXPECT_EQ(graph->graphSimulation({'1', '1', '1'}), (std::vector<char>{'1'}));
  // Sync RST asserted while clk is stable → hold
  EXPECT_EQ(graph->graphSimulation({'1', '1', '0'}), (std::vector<char>{'1'}));
  // Negedge then posedge with RST=0 → clear
  EXPECT_EQ(graph->graphSimulation({'0', '1', '0'}), (std::vector<char>{'1'}));
  EXPECT_EQ(graph->graphSimulation({'1', '1', '0'}), (std::vector<char>{'0'}));
}

TEST(SequentialTests, AsyncAffrClearsImmediately) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("affr_async");
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *rst = graph->addInput("rst");
  auto *q = graph->addSequential(affr, clk, data, rst, "q");
  graph->addEdge(q, graph->addOutput("y"));

  EXPECT_EQ(graph->graphSimulation({'0', '1', '1'}), (std::vector<char>{'x'}));
  EXPECT_EQ(graph->graphSimulation({'1', '1', '1'}), (std::vector<char>{'1'}));
  // Async RST while clk stable → clear immediately
  EXPECT_EQ(graph->graphSimulation({'1', '1', '0'}), (std::vector<char>{'0'}));
}

TEST(SequentialTests, AsyncRstStillSamplesClk) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("affr_prevclk");
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *rst = graph->addInput("rst");
  auto *q = graph->addSequential(affr, clk, data, rst, "q");
  graph->addEdge(q, graph->addOutput("y"));

  // Load Q=1
  EXPECT_EQ(graph->graphSimulation({'0', '1', '1'}), (std::vector<char>{'x'}));
  EXPECT_EQ(graph->graphSimulation({'1', '1', '1'}), (std::vector<char>{'1'}));
  // Async reset while clk rises: must still update d_prevClk so the next
  // 0→1 after release is a real edge.
  EXPECT_EQ(graph->graphSimulation({'0', '0', '0'}), (std::vector<char>{'0'}));
  EXPECT_EQ(graph->graphSimulation({'1', '0', '0'}), (std::vector<char>{'0'}));
  EXPECT_EQ(graph->graphSimulation({'1', '0', '1'}), (std::vector<char>{'0'}));
  EXPECT_EQ(graph->graphSimulation({'0', '0', '1'}), (std::vector<char>{'0'}));
  EXPECT_EQ(graph->graphSimulation({'1', '0', '1'}), (std::vector<char>{'0'}));
}

TEST(SequentialTests, SimulationRemoveClearsPrevClk) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("ff_rm");
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *q = graph->addSequential(ff, clk, data, "q");
  graph->addEdge(q, graph->addOutput("y"));

  EXPECT_EQ(graph->graphSimulation({'0', '1'}), (std::vector<char>{'x'}));
  EXPECT_EQ(graph->graphSimulation({'1', '1'}), (std::vector<char>{'1'}));
  graph->simulationRemove();
  // Fresh edge detector: clk=1 without a prior 0 must not capture
  EXPECT_EQ(graph->graphSimulation({'1', '0'}), (std::vector<char>{'x'}));
}

TEST(SequentialTests, FlipFlopThroughGateBufUpdates) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("ff_buf");
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *q = graph->addSequential(ff, clk, data, "q");
  auto *buf = graph->addGate(GateBuf);
  graph->addEdge(q, buf);
  graph->addEdge(buf, graph->addOutput("y"));

  EXPECT_EQ(graph->graphSimulation({'0', '1'}), (std::vector<char>{'x'}));
  EXPECT_EQ(graph->graphSimulation({'1', '1'}), (std::vector<char>{'1'}));
  EXPECT_EQ(graph->graphSimulation({'0', '0'}), (std::vector<char>{'1'}));
  EXPECT_EQ(graph->graphSimulation({'1', '0'}), (std::vector<char>{'0'}));
}

TEST(SequentialTests, FlipFlopEnXHoldsAndAsyncRstWins) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("ffe_enx");
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInput("data");
  auto *en = graph->addInput("en");
  auto *q = graph->addSequential(ffe, clk, data, en, "q");
  graph->addEdge(q, graph->addOutput("y"));

  EXPECT_EQ(graph->graphSimulation({'0', '1', '1'}), (std::vector<char>{'x'}));
  EXPECT_EQ(graph->graphSimulation({'1', '1', '1'}), (std::vector<char>{'1'}));
  // Mid-cycle EN=X must not corrupt Q (Verilog `if (en)` is not taken).
  EXPECT_EQ(graph->graphSimulation({'1', '0', 'x'}), (std::vector<char>{'1'}));

  OrientedGraph::resetCounter();
  auto g2 = std::make_shared<OrientedGraph>("affre_enx");
  auto *clk2 = g2->addInput("clk");
  auto *data2 = g2->addInput("data");
  auto *en2 = g2->addInput("en");
  auto *rst2 = g2->addInput("rst");
  auto *q2 = g2->addSequential(affre, clk2, data2, en2, rst2, "q");
  g2->addEdge(q2, g2->addOutput("y"));

  EXPECT_EQ(g2->graphSimulation({'0', '1', '1', '1'}),
            (std::vector<char>{'x'}));
  EXPECT_EQ(g2->graphSimulation({'1', '1', '1', '1'}),
            (std::vector<char>{'1'}));
  // Async RST still clears when EN is X.
  EXPECT_EQ(g2->graphSimulation({'1', '1', 'x', '0'}),
            (std::vector<char>{'0'}));
}

TEST(SequentialTests, FlipFlopChainSamplesPreEdgeQ) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("ff_chain");
  auto *clk = graph->addInput("clk");
  auto *d0 = graph->addInput("d0");
  auto *q0 = graph->addSequential(ff, clk, d0, "q0");
  auto *q1 = graph->addSequential(ff, clk, q0, "q1");
  graph->addEdge(q1, graph->addOutput("y"));

  EXPECT_EQ(graph->graphSimulation({'0', '1'}), (std::vector<char>{'x'}));
  EXPECT_EQ(graph->graphSimulation({'1', '1'}), (std::vector<char>{'x'}));
  // q0=1, q1 still x after first posedge (sampled old q0=x)
  EXPECT_EQ(graph->graphSimulation({'0', '0'}), (std::vector<char>{'x'}));
  // Second posedge: q1 captures pre-edge q0=1 even though d0=0 updates q0.
  EXPECT_EQ(graph->graphSimulation({'1', '0'}), (std::vector<char>{'1'}));
}

TEST(SequentialTests, BusSequentialToVerilogNonEmpty) {
  OrientedGraph::resetCounter();
  auto graph = std::make_shared<OrientedGraph>("bus_ff_v");
  auto *clk = graph->addInput("clk");
  auto *data = graph->addInputBus("data", 4);
  auto *q = graph->addSequentialBus(ff, clk, data, "q", 4);
  EXPECT_EQ(q->toVerilog(), "always @(posedge clk) begin\n"
                            "\t\tq <= data;\n"
                            "\tend\n");
  graph->addEdge(q, graph->addOutputBus("y", 4));
  // Scalar stimulus still drives broadcast Q into the bus string.
  graph->graphSimulation({'0', '1'});
  graph->graphSimulation({'1', '1'});
  EXPECT_EQ(GraphVertexBus::getBusPointer(q)->getValueBus(), "1111");
}

#ifdef LOGFLAG
TEST(ErrorOutputTest, CapturesLog) {
  std::stringstream buffer;
  // Redirect both cout and cerr since easylogging++ might use either depending
  // on config
  std::streambuf *old_cerr = std::cerr.rdbuf(buffer.rdbuf());
  std::streambuf *old_cout = std::cout.rdbuf(buffer.rdbuf());

  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();
  auto *en = graph->addInput("en");
  auto *data = graph->addInput("data");
  auto *rst = graph->addInput("rst");

  // latchrs is latch | RST | SET. We provide only RST wire, so SET will be
  // missing/invalid.
  graph->addSequential(latchrs, en, data, rst, "q");

  // Restore buffers
  std::cerr.rdbuf(old_cerr);
  std::cout.rdbuf(old_cout);

  std::string output = buffer.str();
  // Check that the error message is present in the output
  EXPECT_TRUE(output.find("Invalid flag found in used type: SET") !=
              std::string::npos)
      << "Expected error message not found in output: " << output;
}
#endif
