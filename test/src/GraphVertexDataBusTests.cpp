#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>

#include <gtest/gtest.h>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

#include <fstream>
#include <sstream>

#include "TestDataBusData.hpp"

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

TEST(DataBusTests, TestVerilogConstantBus) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();

  std::vector<VertexPtr> constants;
  constants.push_back(graph->addConst('1'));
  constants.push_back(graph->addConst('0'));
  constants.push_back(graph->addConst('1'));

  std::shared_ptr<GraphVertexDataBus> bus =
      std::make_shared<GraphVertexDataBus>(tcb::span<VertexPtr>(constants),
                                           "constBus", graph);

  std::string expected = "wire [2:0] constBus;\n"
                         "assign constBus = 3'b101;\n";
  EXPECT_EQ(bus->toVerilog(false), expected);
}

TEST(DataBusTests, TestSliceValidRange) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();

  std::vector<VertexPtr> inputs;
  inputs.push_back(graph->addInput("a"));
  inputs.push_back(graph->addInput("b"));
  inputs.push_back(graph->addInput("c"));

  std::shared_ptr<GraphVertexDataBus> bus =
      std::make_shared<GraphVertexDataBus>(tcb::span<VertexPtr>(inputs),
                                           "inBus", graph);

  GraphVertexDataBus slice = bus->slice(1, 3);
  EXPECT_EQ(slice.getWidth(), 2);
  EXPECT_EQ(slice[0]->getName(), "b");
  EXPECT_EQ(slice[1]->getName(), "c");
}

TEST(DataBusTests, TestSliceOutOfRangeThrows) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();

  std::vector<VertexPtr> inputs;
  inputs.push_back(graph->addInput("a"));
  inputs.push_back(graph->addInput("b"));

  std::shared_ptr<GraphVertexDataBus> bus =
      std::make_shared<GraphVertexDataBus>(tcb::span<VertexPtr>(inputs), "bus",
                                           graph);

  EXPECT_THROW(bus->slice(1, 3), std::out_of_range);
  EXPECT_THROW(bus->slice(2, 2), std::out_of_range);
  EXPECT_THROW(bus->slice(2, 1), std::out_of_range);
}

TEST(DataBusTests, TestOperatorIndexingValid) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();

  VertexPtr a = graph->addInput("a");
  VertexPtr b = graph->addInput("b");
  VertexPtr c = graph->addInput("c");

  std::vector<VertexPtr> vec = {a, b, c};

  std::shared_ptr<GraphVertexDataBus> bus =
      std::make_shared<GraphVertexDataBus>(tcb::span<VertexPtr>(vec), "bus",
                                           graph);

  EXPECT_EQ((*bus)[0]->getName(), "a");
  EXPECT_EQ((*bus)[2]->getName(), "c");
}

TEST(DataBusTests, TestOperatorIndexingOutOfRangeThrows) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();

  std::vector<VertexPtr> inputs = {graph->addInput("a")};
  std::shared_ptr<GraphVertexDataBus> bus =
      std::make_shared<GraphVertexDataBus>(tcb::span<VertexPtr>(inputs), "bus",
                                           graph);

  EXPECT_THROW((*bus)[1], std::out_of_range);
}

TEST(DataBusTests, TestToVerilogPerLineFlag) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();

  VertexPtr a = graph->addInput("a");
  VertexPtr b = graph->addInput("b");

  std::vector<VertexPtr> vec = {a, b};
  std::shared_ptr<GraphVertexDataBus> bus =
      std::make_shared<GraphVertexDataBus>(tcb::span<VertexPtr>(vec), "bus",
                                           graph);

  std::ostringstream expected;
  expected << "input a;\n"
           << "input b;\n";

  EXPECT_EQ(bus->toVerilog(true), expected.str());
}

TEST(DataBusTests, TestDataBusToVerilog) {
  OrientedGraph::resetCounter();
  GraphPtr graph = std::make_shared<OrientedGraph>();

  VertexPtr a = graph->addInput("a");
  VertexPtr b = graph->addInput("b");
  VertexPtr c = graph->addInput("c");

  std::vector<VertexPtr> vec = {a, b, c};
  auto bus = std::make_shared<GraphVertexDataBus>(tcb::span<VertexPtr>(vec),
                                                  "bus", graph);

  // Проверка результата toVerilog с флагом форматирования true
  EXPECT_EQ(bus->toVerilog(), "input [2:0] bus;\n");

  // Проверка результата toVerilog с флагом форматирования false
  EXPECT_EQ(bus->toVerilog(false), "input [2:0] bus;");

  // Добавим выходной DataBus
  auto outBus = std::make_shared<GraphVertexDataBus>(tcb::span<VertexPtr>(vec),
                                                     "out", graph);

  // Проверка результата toVerilog с флагом форматирования true
  EXPECT_EQ(outBus->toVerilog(), "output [2:0] out;\n");

  // Проверка результата toVerilog с флагом форматирования false
  EXPECT_EQ(outBus->toVerilog(false), "output [2:0] out;");

  // Проверка вывода в файл
  const std::string fileName = "testDataBus.v";
  graph->toVerilog("./", fileName);
  testFile(fileName, TestData::DATABUS_TEST);
}
