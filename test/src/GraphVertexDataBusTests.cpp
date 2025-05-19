#include <CircuitGenGraph/GraphVertex.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>
#include <gtest/gtest.h>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include "TestDataBusData.hpp"

using namespace CG_Graph;

class DataBusTest : public ::testing::Test {
protected:
  void SetUp() override {
    graph = std::make_shared<OrientedGraph>();
    subGraph = std::make_shared<OrientedGraph>();

    for (int i = 0; i < 4; ++i) {
      VertexPtr input = graph->addInput("in_" + std::to_string(i));
      VertexPtr constant = graph->addConst('1', "const_" + std::to_string(i));
      inputs.push_back(input);
      constants.push_back(constant);
    }

    for (int i = 0; i < 2; ++i) {
      VertexPtr subInput = subGraph->addInput("sub_in_" + std::to_string(i));
      subGraphInputs.push_back(subInput);
    }

    // Создаем DataBus из inputs — это обязательно, чтобы потом slice работал
    dataBus = graph->addDataBus(inputs, "dataBus");
  }

  GraphVertexDataBus &getDataBus(VertexPtr vertex) {
    if (vertex->getType() != VertexTypes::dataBus) {
      throw std::bad_cast();
    }
    return static_cast<GraphVertexDataBus &>(*vertex);
  }

  GraphPtr graph;
  GraphPtr subGraph;
  std::vector<VertexPtr> inputs;
  std::vector<VertexPtr> constants;
  std::vector<VertexPtr> subGraphInputs;

  VertexPtr dataBus;
};

TEST_F(DataBusTest, ConstructorEmptyBus) {
  std::vector<VertexPtr> empty;
  EXPECT_DEATH(graph->addDataBus(tcb::span<VertexPtr>(empty), "empty_bus"),
               ".*");
}

TEST_F(DataBusTest, ConstructorValidBus) {
  VertexPtr input_bus =
      graph->addDataBus(tcb::span<VertexPtr>(inputs), "input_bus");
  VertexPtr const_bus =
      graph->addDataBus(tcb::span<VertexPtr>(constants), "const_bus");

  EXPECT_EQ(input_bus->getType(), VertexTypes::dataBus);
  EXPECT_EQ(const_bus->getType(), VertexTypes::dataBus);
}

TEST_F(DataBusTest, SliceOperations) {
  // Создаем шину данных
  VertexPtr bus_ptr =
      graph->addDataBus(tcb::span<VertexPtr>(inputs), "test_bus");
  GraphVertexDataBus &bus = getDataBus(bus_ptr);

  // Проверяем корректный срез
  // std::cout << "DEBUG: Bus width = " << bus.getWidth() << std::endl;
  auto slice = bus.slice(1, 3);
  // std::cout << "DEBUG: Actual slice width = " << slice.getWidth() <<
  // std::endl;
  EXPECT_EQ(slice.getWidth(), 2);

  // Проверяем исключения при некорректных границах
  EXPECT_THROW(bus.slice(3, 3), std::out_of_range);
  EXPECT_THROW(bus.slice(3, 2), std::out_of_range);
  EXPECT_THROW(bus.slice(0, bus.getWidth() + 1), std::out_of_range);
}

TEST_F(DataBusTest, IndexOperator) {
  VertexPtr bus_ptr =
      graph->addDataBus(tcb::span<VertexPtr>(inputs), "test_bus");
  GraphVertexDataBus &bus = getDataBus(bus_ptr);

  // Проверяем доступ к элементам
  for (size_t i = 0; i < inputs.size(); ++i) {
    EXPECT_EQ(bus[i]->getName(), inputs[i]->getName());
  }

  // Проверяем выход за границы
  EXPECT_THROW(bus[inputs.size()], std::out_of_range);
}

TEST_F(DataBusTest, GetWidth) {
  VertexPtr bus_ptr =
      graph->addDataBus(tcb::span<VertexPtr>(inputs), "test_bus");
  GraphVertexDataBus &bus = getDataBus(bus_ptr);
  EXPECT_EQ(bus.getWidth(), inputs.size());
}

TEST_F(DataBusTest, ToVerilogInputBus) {
  VertexPtr bus_ptr =
      graph->addDataBus(tcb::span<VertexPtr>(inputs), "input_bus");
  GraphVertexDataBus &bus = getDataBus(bus_ptr);
  std::string result = bus.toVerilog(false);
  // std::cout << "Generated Verilog:\n" << result << std::endl;
  // Проверяем основные элементы вывода
  EXPECT_NE(result.find("input [" + std::to_string(inputs.size() - 1) +
                        ":0] input_bus;"),
            std::string::npos);
}

TEST_F(DataBusTest, ToVerilogConstantBus) {
  VertexPtr bus_ptr =
      graph->addDataBus(tcb::span<VertexPtr>(constants), "const_bus");
  GraphVertexDataBus &bus = getDataBus(bus_ptr);
  std::string result = bus.toVerilog(false);
  // std::cout << "Generated Verilog:\n" << result << std::endl;
  // Проверяем основные элементы вывода
  EXPECT_NE(result.find("wire [" + std::to_string(constants.size() - 1) +
                        ":0] const_bus;"),
            std::string::npos);
  EXPECT_NE(result.find("assign const_bus = " +
                        std::to_string(constants.size()) + "'b"),
            std::string::npos);
}

TEST_F(DataBusTest, ToVerilogOutputBus) {
  std::vector<VertexPtr> outputs;
  for (size_t i = 0; i < inputs.size(); ++i) {
    VertexPtr output = graph->addOutput("out_" + std::to_string(i));
    graph->addEdge(inputs[i], output);
    outputs.push_back(output);
  }

  VertexPtr bus_ptr =
      graph->addDataBus(tcb::span<VertexPtr>(outputs), "output_bus");
  GraphVertexDataBus &bus = getDataBus(bus_ptr);
  std::string result = bus.toVerilog(false);
  // std::cout << "Generated Verilog:\n" << result << std::endl;
  // Проверяем основные элементы вывода
  EXPECT_NE(result.find("output [" + std::to_string(outputs.size() - 1) +
                        ":0] output_bus;"),
            std::string::npos);
  for (size_t i = 0; i < outputs.size(); ++i) {
    EXPECT_NE(result.find("assign output_bus[" + std::to_string(i) +
                          "] = " + inputs[i]->getName() + ";"),
              std::string::npos);
  }
}
