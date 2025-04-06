#include <CircuitGenGraph/GraphVertex.hpp>
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

using namespace CG_Graph;

class DataBusTest : public ::testing::Test {
protected:
  void SetUp() override {
    graph = std::make_shared<OrientedGraph>();

    input1 = graph->addInput("input1");
    input2 = graph->addInput("input2");
    input3 = graph->addInput("input3");

    const1 = graph->addConst('1', "const1");
    const2 = graph->addConst('0', "const2");
    const3 = graph->addConst('1', "const3");

    output1 = graph->addOutput("output1");
    output2 = graph->addOutput("output2");

    clk = graph->addInput("clk");
    data = graph->addInput("data");
    rst = graph->addInput("rst");
    set = graph->addInput("set");
    en = graph->addInput("en");
    seq = graph->addSequential(ffrse, clk, data, rst, set, en, "q");
    seq_out = graph->addOutput("seq_out");
    graph->addEdge(seq, seq_out);
  }

  GraphPtr graph;
  VertexPtr input1;
  VertexPtr input2;
  VertexPtr input3;
  VertexPtr const1;
  VertexPtr const2;
  VertexPtr const3;
  VertexPtr output1;
  VertexPtr output2;
  VertexPtr clk;
  VertexPtr data;
  VertexPtr rst;
  VertexPtr set;
  VertexPtr en;
  VertexPtr seq;
  VertexPtr seq_out;
};

TEST_F(DataBusTest, TestEmptyBusCreation) {
  std::vector<VertexPtr> emptyVec;
  EXPECT_THROW(GraphVertexDataBus(tcb::span<VertexPtr>(emptyVec), graph),
               std::invalid_argument);
}

TEST_F(DataBusTest, TestMixedTypeBusCreation) {
  std::vector<VertexPtr> mixedVec = {input1, const1};
  EXPECT_THROW(GraphVertexDataBus(tcb::span<VertexPtr>(mixedVec), graph),
               std::invalid_argument);
}

TEST_F(DataBusTest, TestInputBusCreation) {
  std::vector<VertexPtr> inputs = {input1, input2, input3};
  GraphVertexDataBus inputBus(tcb::span<VertexPtr>(inputs), "input_bus", graph);

  EXPECT_EQ(inputBus.getType(), VertexTypes::dataBus);
  EXPECT_EQ(inputBus.getWidth(), 3);
  EXPECT_EQ(inputBus.getName(), "input_bus");
}

TEST_F(DataBusTest, TestBusOperations) {
  std::vector<VertexPtr> inputs = {input1, input2, input3, const1, const2};
  GraphVertexDataBus bus(tcb::span<VertexPtr>(inputs), "test_bus", graph);

  // Test operator[]
  EXPECT_EQ(bus[0], input1);
  EXPECT_EQ(bus[3], const1);
  EXPECT_THROW(bus[5], std::out_of_range);

  // Test slice
  auto sliced = bus.slice(1, 4);
  EXPECT_EQ(sliced.getWidth(), 3);
  EXPECT_EQ(sliced[0], input2);
  EXPECT_EQ(sliced[2], const1);

  // Test invalid slice
  EXPECT_THROW(bus.slice(3, 2), std::out_of_range);
  EXPECT_THROW(bus.slice(0, 6), std::out_of_range);
}

TEST_F(DataBusTest, TestVerilogGeneration) {
  std::vector<VertexPtr> inputs = {input1, input2, input3};
  GraphVertexDataBus inputBus(tcb::span<VertexPtr>(inputs), "input_bus", graph);

  std::string expectedInput = "input [2:0] input_bus;\n";
  EXPECT_EQ(inputBus.toVerilog(false), expectedInput);

  std::vector<VertexPtr> consts = {const1, const2, const3};
  GraphVertexDataBus constBus(tcb::span<VertexPtr>(consts), "const_bus", graph);

  std::string expectedConst = "wire [2:0] const_bus;\n"
                              "assign const_bus = 3'b101;\n";
  EXPECT_EQ(constBus.toVerilog(false), expectedConst);
}

TEST_F(DataBusTest, TestSequentialBus) {
  std::vector<VertexPtr> seqs = {seq, seq, seq};
  GraphVertexDataBus seqBus(tcb::span<VertexPtr>(seqs), "seq_bus", graph);

  std::string expectedSeq = "reg [2:0] seq_bus;\n"
                            "always @(posedge clk) begin\n"
                            "\t\tif (!rst) seq_bus <= 3'b000;\n"
                            "\t\telse if (set) seq_bus <= 3'b111;\n"
                            "\t\telse if (en) seq_bus <= {data, data, data};\n"
                            "\tend\n";
  EXPECT_EQ(seqBus.toVerilog(false), expectedSeq);
}

TEST_F(DataBusTest, TestToVerilog) {
  // Test 1: Пустая шина
  std::vector<VertexPtr> emptyVec;
  GraphVertexDataBus emptyBus(tcb::span<VertexPtr>(emptyVec), "empty_bus",
                              graph);
  EXPECT_THROW(emptyBus.toVerilog(false), std::invalid_argument);

  // Test 2: Множество выходных элементов
  std::vector<VertexPtr> outputs = {output1, output2};
  GraphVertexDataBus outputBus(tcb::span<VertexPtr>(outputs), "output_bus",
                               graph);
  std::string expectedOutput = "output [1:0] output_bus;\n"
                               "assign output_bus[0] = output1;\n"
                               "assign output_bus[1] = output2;\n";
  EXPECT_EQ(outputBus.toVerilog(false), expectedOutput);

  // Test 3: Последовательная шина
  std::vector<VertexPtr> seqs = {seq, seq, seq};
  GraphVertexDataBus seqBus(tcb::span<VertexPtr>(seqs), "seq_bus", graph);
  std::string expectedSeq = "reg [2:0] seq_bus;\n"
                            "always @(posedge clk) begin\n"
                            "\t\tif (!rst) seq_bus <= 3'b000;\n"
                            "\t\telse if (set) seq_bus <= 3'b111;\n"
                            "\t\telse if (en) seq_bus <= {data, data, data};\n"
                            "\tend\n";
  EXPECT_EQ(seqBus.toVerilog(false), expectedSeq);

  // Test 4: Шина с флагом true
  std::vector<VertexPtr> inputs = {input1, input2, input3};
  GraphVertexDataBus inputBus(tcb::span<VertexPtr>(inputs), "input_bus", graph);
  std::string expectedInput = "input input1;\n"
                              "input input2;\n"
                              "input input3;\n";
  EXPECT_EQ(inputBus.toVerilog(true), expectedInput);

  // Test 5: Несколько операторов assign
  std::string expectedAssigns = "assign input_bus[0] = input1;\n"
                                "assign input_bus[1] = input2;\n"
                                "assign input_bus[2] = input3;\n";
  EXPECT_EQ(inputBus.toVerilog(false), expectedAssigns);
}
