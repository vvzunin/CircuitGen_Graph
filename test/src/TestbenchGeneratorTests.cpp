#include <CircuitGenGraph/TestbenchGenerator.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>

#include <gtest/gtest.h>
#include <fstream>
#include <cstdio> 

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

using namespace CG_Graph;

class TestbenchGeneratorTests : public ::testing::Test {
protected:
  void TearDown() override {
    std::remove("test_golden_valid.v");
    std::remove("test_golden_invalid.v");
    std::remove("test_tb_output.v");
  }

  void createDummyFile(const std::string& filename, const std::string& content) {
    std::ofstream f(filename);
    f << content;
    f.close();
  }
};

TEST_F(TestbenchGeneratorTests, NullGraphReturnsFalse) {
  EXPECT_FALSE(TestbenchGenerator::generate(nullptr, "dummy.v", "out.v"));
}

TEST_F(TestbenchGeneratorTests, MissingGoldenModelReturnsFalse) {
  GraphPtr graph = std::make_shared<OrientedGraph>("TestGraph");
  EXPECT_FALSE(TestbenchGenerator::generate(graph, "non_existent_file_123.v", "out.v"));
}

TEST_F(TestbenchGeneratorTests, InvalidGoldenModelReturnsFalse) {
  GraphPtr graph = std::make_shared<OrientedGraph>("TestGraph");
  createDummyFile("test_golden_invalid.v", "// Just some random comments\n// No module here");
  
  EXPECT_FALSE(TestbenchGenerator::generate(graph, "test_golden_invalid.v", "out.v"));
}

TEST_F(TestbenchGeneratorTests, SuccessfulGeneration) {
  GraphPtr graph = std::make_shared<OrientedGraph>("MyTestGraph");
  graph->addInput("in_A");
  graph->addInput("in_B");
  graph->addOutput("out_Y");

  std::string goldenContent = 
    "module golden_and(\n"
    "    input wire a,\n"
    "    input b,\n"
    "    output reg out\n"
    ");\n"
    "    always @(*) out = a & b;\n"
    "endmodule\n";
  createDummyFile("test_golden_valid.v", goldenContent);

  EXPECT_TRUE(TestbenchGenerator::generate(graph, "test_golden_valid.v", "test_tb_output.v"));

  std::ifstream tbFile("test_tb_output.v");
  ASSERT_TRUE(tbFile.is_open());
  
  std::string tbContent((std::istreambuf_iterator<char>(tbFile)), std::istreambuf_iterator<char>());
  
  EXPECT_TRUE(tbContent.find("module tb_MyTestGraph_vs_golden_and;") != std::string::npos);
  EXPECT_TRUE(tbContent.find("golden_and golden_inst") != std::string::npos);
  EXPECT_TRUE(tbContent.find("MyTestGraph graph_inst") != std::string::npos);
  EXPECT_TRUE(tbContent.find("test_in[0]") != std::string::npos);
  EXPECT_TRUE(tbContent.find("if (golden_out !== graph_out) begin") != std::string::npos);
}