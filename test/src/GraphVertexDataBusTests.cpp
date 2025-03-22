#include <sstream>
#include <CircuitGenGraph/GraphVertex.hpp>
#include <gtest/gtest.h>
#include "span.hpp"
#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif
#include <memory>
#include <vector>

// Указатель на граф, который владеет памятью
GraphPtr memoryOwnerGraph = std::make_shared<OrientedGraph>();

// Тесты для конструктора GraphVertexDataBus с вертексами
TEST(TestGraphVertexDataBusConstructor, ConstructorWithVertices) {
  // Инициализация вектора вертексов
  std::vector<VertexPtr> vertices = {
      std::make_shared<GraphVertexBase>(memoryOwnerGraph),
      std::make_shared<GraphVertexBase>(memoryOwnerGraph)};
  // Создание объекта GraphVertexDataBus и проверка его ширины
  GraphVertexDataBus bus(vertices, memoryOwnerGraph);
  EXPECT_EQ(bus.getWidth(), vertices.size());
}

// Тесты для конструктора GraphVertexDataBus с вертексами и именем
TEST(TestGraphVertexDataBusConstructor, ConstructorWithVerticesAndName) {
  // Инициализация вектора вертексов
  std::vector<VertexPtr> vertices = {
      std::make_shared<GraphVertexBase>(memoryOwnerGraph),
      std::make_shared<GraphVertexBase>(memoryOwnerGraph)};
  // Создание объекта GraphVertexDataBus с заданным именем
  GraphVertexDataBus bus(vertices, "test_bus", memoryOwnerGraph);
  // Проверка ширины и имени шины
  EXPECT_EQ(bus.getWidth(), vertices.size());
  EXPECT_EQ(bus.getName(), "test_bus");
}
// Тесты для конструктора GraphVertexDataBus с копированием
TEST(TestGraphVertexDataBusConstructor, ConstructorWithVerticesAndCopy) {
  // Инициализация вектора вертексов
  std::vector<VertexPtr> vertices = {
      std::make_shared<GraphVertexBase>(memoryOwnerGraph),
      std::make_shared<GraphVertexBase>(memoryOwnerGraph)};
  // Создание оригинальной шины
  GraphVertexDataBus originalBus(vertices, memoryOwnerGraph);
  // Создание копии шины
  GraphVertexDataBus copiedBus(vertices, originalBus);
  // Проверка ширины оригинальной и скопированной шин
  EXPECT_EQ(copiedBus.getWidth(), originalBus.getWidth());
}

// Тесты для метода slice в GraphVertexDataBus
TEST(TestGraphVertexDataBusMethods, SliceMethod) {
  // Инициализация вектора вертексов
  std::vector<VertexPtr> vertices = {
      std::make_shared<GraphVertexBase>(memoryOwnerGraph),
      std::make_shared<GraphVertexBase>(memoryOwnerGraph),
      std::make_shared<GraphVertexBase>(memoryOwnerGraph)};
  // Создание объекта GraphVertexDataBus
  GraphVertexDataBus bus(vertices, memoryOwnerGraph);
  // Вырезка части шины и проверка ширины
  auto slicedBus = bus.slice(1, 3);
  EXPECT_EQ(slicedBus.getWidth(), 2);
}

// Тесты для оператора доступа в GraphVertexDataBus
TEST(TestGraphVertexDataBusMethods, OperatorAccess) {
  // Инициализация вектора вертексов
  std::vector<VertexPtr> vertices = {
      std::make_shared<GraphVertexBase>(memoryOwnerGraph),
      std::make_shared<GraphVertexBase>(memoryOwnerGraph)};
  // Создание объекта GraphVertexDataBus
  GraphVertexDataBus bus(vertices, memoryOwnerGraph);
  // Проверка доступа через оператор []
  EXPECT_NO_THROW(bus[1]);
  EXPECT_THROW(bus[2], std::out_of_range); // Проверка выхода за пределы
}

// Тесты для метода toVerilog в GraphVertexDataBus
TEST(TestGraphVertexDataBusMethods, ToVerilog) {
  // Инициализация вектора вертексов
  std::vector<VertexPtr> vertices = {
      std::make_shared<GraphVertexBase>(memoryOwnerGraph),
      std::make_shared<GraphVertexBase>(memoryOwnerGraph)};
  // Создание объекта GraphVertexDataBus с именем
  GraphVertexDataBus bus(vertices, "test_bus", memoryOwnerGraph);
  // Генерация кода для Verilog и проверка его непустоты
  std::string verilogCode = bus.toVerilog(false);
  EXPECT_FALSE(verilogCode.empty());
}

// Тесты для метода toDOT в GraphVertexDataBus
TEST(TestGraphVertexDataBusMethods, ToDOT) {
  // Инициализация вектора вертексов
  std::vector<VertexPtr> vertices = {
      std::make_shared<GraphVertexBase>(memoryOwnerGraph),
      std::make_shared<GraphVertexBase>(memoryOwnerGraph)};
  // Создание объекта GraphVertexDataBus
  GraphVertexDataBus bus(vertices, memoryOwnerGraph);
  // Генерация DOT представления и проверка его непустоты
  DotReturn dot = bus.toDOT();
  EXPECT_FALSE(dot.empty());
}

// Тесты для ошибок в GraphVertexDataBus (пустой вектор вертексов)
TEST(TestGraphVertexDataBusErrors, EmptyVertices) {
  std::vector<VertexPtr> vertices;
  // Проверка на выброс исключения при пустом векторе вертексов
  EXPECT_THROW(GraphVertexDataBus(vertices, memoryOwnerGraph),
               std::invalid_argument);
}

// Тесты для ошибок в GraphVertexDataBus (невалидный диапазон для slice)
TEST(TestGraphVertexDataBusErrors, InvalidSliceRange) {
  // Инициализация вектора вертексов
  std::vector<VertexPtr> vertices = {
      std::make_shared<GraphVertexBase>(memoryOwnerGraph),
      std::make_shared<GraphVertexBase>(memoryOwnerGraph)};
  // Создание объекта GraphVertexDataBus
  GraphVertexDataBus bus(vertices, memoryOwnerGraph);
  // Проверка на выброс исключений при некорректных диапазонах
  EXPECT_THROW(bus.slice(2, 1), std::out_of_range);
  EXPECT_THROW(bus.slice(1, 3), std::out_of_range);
}

TEST(GraphVertexDataBusTest, ToVerilogConstantBus) {
  // Создаем массив константных вершин
  std::vector<std::shared_ptr<GraphVertexBase>> constants;
  constants.push_back(std::make_shared<GraphVertexConstant>('0'));
  constants.push_back(std::make_shared<GraphVertexConstant>('1'));
  constants.push_back(std::make_shared<GraphVertexConstant>('0'));

  // Создаем массив сырых указателей
  std::vector<GraphVertexBase *> raw_constants;
  for (const auto &ptr: constants) {
    raw_constants.push_back(
        ptr.get()); // Преобразуем std::shared_ptr в сырой указатель
  }

  // Преобразуем std::vector в tcb::span
  tcb::span<VertexPtr> constants_span(raw_constants.data(),
                                      raw_constants.size());

  // Создаем шину данных с константами
  GraphVertexDataBus bus(constants_span, "const_bus", nullptr);

  // Ожидаемый Verilog код
  std::string expected = "wire [2:0] const_bus;\nassign const_bus = 3'b010;\n";

  // Проверяем, что метод toVerilog возвращает ожидаемый результат
  EXPECT_EQ(bus.toVerilog(false), expected);
}

TEST(GraphVertexDataBusTest, ToVerilogInputBus) {
  // Создаем массив входных вершин
  std::vector<std::shared_ptr<GraphVertexBase>> inputs;
  inputs.push_back(std::make_shared<GraphVertexInput>("input1"));
  inputs.push_back(std::make_shared<GraphVertexInput>("input2"));

  // Создаем массив сырых указателей
  std::vector<GraphVertexBase *> raw_inputs;
  for (const auto &ptr: inputs) {
    raw_inputs.push_back(
        ptr.get()); // Преобразуем std::shared_ptr в сырой указатель
  }

  // Преобразуем std::vector в tcb::span
  tcb::span<VertexPtr> inputs_span(raw_inputs.data(), raw_inputs.size());

  // Создаем шину данных с входами
  GraphVertexDataBus bus(inputs_span, "input_bus", nullptr);

  // Ожидаемый Verilog код
  std::string expected = "input [1:0] input_bus;\n";

  // Проверяем, что метод toVerilog возвращает ожидаемый результат
  EXPECT_EQ(bus.toVerilog(false), expected);
}

TEST(GraphVertexDataBusTest, ToVerilogOutputBus) {
  // Создаем массив выходных вершин
  std::vector<std::shared_ptr<GraphVertexBase>> outputs;
  outputs.push_back(std::make_shared<GraphVertexOutput>("output1"));
  outputs.push_back(std::make_shared<GraphVertexOutput>("output2"));

  // Создаем массив сырых указателей
  std::vector<GraphVertexBase *> raw_outputs;
  for (const auto &ptr: outputs) {
    raw_outputs.push_back(
        ptr.get()); // Преобразуем std::shared_ptr в сырой указатель
  }

  // Преобразуем std::vector в tcb::span
  tcb::span<VertexPtr> outputs_span(raw_outputs.data(), raw_outputs.size());

  // Создаем шину данных с выходами
  GraphVertexDataBus bus(outputs_span, "output_bus", nullptr);

  // Ожидаемый Verilog код
  std::string expected = "output [1:0] output_bus;\n";

  // Проверяем, что метод toVerilog возвращает ожидаемый результат
  EXPECT_EQ(bus.toVerilog(false), expected);
}

TEST(GraphVertexDataBusTest, ToVerilogConstantBus) {
  // Создаем массив константных вершин
  std::vector<std::shared_ptr<GraphVertexBase>> constants;
  constants.push_back(std::make_shared<GraphVertexConstant>('0'));
  constants.push_back(std::make_shared<GraphVertexConstant>('1'));
  constants.push_back(std::make_shared<GraphVertexConstant>('0'));

  // Создаем массив сырых указателей
  std::vector<GraphVertexBase *> raw_constants;
  for (const auto &ptr: constants) {
    raw_constants.push_back(
        ptr.get()); // Преобразуем std::shared_ptr в сырой указатель
  }

  // Преобразуем std::vector в tcb::span
  tcb::span<VertexPtr> constants_span(raw_constants.data(),
                                      raw_constants.size());

  // Создаем шину данных с константами
  GraphVertexDataBus bus(constants_span, "const_bus", nullptr);

  // Получаем результат работы toVerilog
  std::string verilogCode = bus.toVerilog(false);

  // Выводим результат в терминал
  // std::cout << "Generated Verilog code:\n" << verilogCode << std::endl;
  // Ожидаемый Verilog код
  std::string expected = "wire [2:0] const_bus;\nassign const_bus = 3'b010;\n";

  // Проверяем, что метод toVerilog возвращает ожидаемый результат
  EXPECT_EQ(verilogCode, expected);
}

TEST(GraphVertexDataBusTest, ToVerilogConstantBusFlagTrue) {
  // Создаем массив константных вершин
  std::vector<std::shared_ptr<GraphVertexBase>> constants;
  constants.push_back(std::make_shared<GraphVertexConstant>('0'));
  constants.push_back(std::make_shared<GraphVertexConstant>('1'));
  constants.push_back(std::make_shared<GraphVertexConstant>('0'));

  // Создаем массив сырых указателей
  std::vector<GraphVertexBase *> raw_constants;
  for (const auto &ptr: constants) {
    raw_constants.push_back(
        ptr.get()); // Преобразуем std::shared_ptr в сырой указатель
  }

  // Преобразуем std::vector в tcb::span
  tcb::span<VertexPtr> constants_span(raw_constants.data(),
                                      raw_constants.size());

  // Создаем шину данных с константами
  GraphVertexDataBus bus(constants_span, "const_bus", nullptr);

  // Получаем результат работы toVerilog с flag = true
  std::string verilogCode = bus.toVerilog(true);

  // Ожидаемый Verilog код
  std::string expected = "wire const_bus_0;\nassign const_bus_0 = 1'b0;\n"
                         "wire const_bus_1;\nassign const_bus_1 = 1'b1;\n"
                         "wire const_bus_2;\nassign const_bus_2 = 1'b0;\n";

  // Выводим результат в терминал для отладки
  // std::cout << "Generated Verilog code (flag = true):\n" << verilogCode <<
  // std::endl;

  // Проверяем, что метод toVerilog возвращает ожидаемый результат
  EXPECT_EQ(verilogCode, expected);
}

TEST(GraphVertexDataBusTest, ToVerilogInputBusFlagTrue) {
  // Создаем массив входных вершин
  std::vector<std::shared_ptr<GraphVertexBase>> inputs;
  inputs.push_back(std::make_shared<GraphVertexInput>("input1"));
  inputs.push_back(std::make_shared<GraphVertexInput>("input2"));

  // Создаем массив сырых указателей
  std::vector<GraphVertexBase *> raw_inputs;
  for (const auto &ptr: inputs) {
    raw_inputs.push_back(
        ptr.get()); // Преобразуем std::shared_ptr в сырой указатель
  }

  // Преобразуем std::vector в tcb::span
  tcb::span<VertexPtr> inputs_span(raw_inputs.data(), raw_inputs.size());

  // Создаем шину данных с входами
  GraphVertexDataBus bus(inputs_span, "input_bus", nullptr);

  // Получаем результат работы toVerilog с flag = true
  std::string verilogCode = bus.toVerilog(true);

  // Ожидаемый Verilog код
  std::string expected = "input input_bus_0;\n"
                         "input input_bus_1;\n";

  // Выводим результат в терминал для отладки
  // std::cout << "Generated Verilog code (flag = true):\n" << verilogCode <<
  // std::endl;

  // Проверяем, что метод toVerilog возвращает ожидаемый результат
  EXPECT_EQ(verilogCode, expected);
}

TEST(GraphVertexDataBusTest, ToVerilogOutputBusFlagTrue) {
  // Создаем массив выходных вершин
  std::vector<std::shared_ptr<GraphVertexBase>> outputs;
  outputs.push_back(std::make_shared<GraphVertexOutput>("output1"));
  outputs.push_back(std::make_shared<GraphVertexOutput>("output2"));

  // Создаем массив сырых указателей
  std::vector<GraphVertexBase *> raw_outputs;
  for (const auto &ptr: outputs) {
    raw_outputs.push_back(
        ptr.get()); // Преобразуем std::shared_ptr в сырой указатель
  }

  // Преобразуем std::vector в tcb::span
  tcb::span<VertexPtr> outputs_span(raw_outputs.data(), raw_outputs.size());

  // Создаем шину данных с выходами
  GraphVertexDataBus bus(outputs_span, "output_bus", nullptr);

  // Получаем результат работы toVerilog с flag = true
  std::string verilogCode = bus.toVerilog(true);

  // Ожидаемый Verilog код
  std::string expected = "output output_bus_0;\n"
                         "output output_bus_1;\n";

  // Выводим результат в терминал для отладки
  // std::cout << "Generated Verilog code (flag = true):\n" << verilogCode <<
  // std::endl;

  // Проверяем, что метод toVerilog возвращает ожидаемый результат
  EXPECT_EQ(verilogCode, expected);
}