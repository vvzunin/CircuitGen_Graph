/// @file example_testbench.cpp

#include <iostream>

#include <CircuitGenGraph/OrientedGraph.hpp>
#include <CircuitGenGraph/TestbenchGenerator.hpp>

using namespace CG_Graph;

/// @brief Создает полусумматор
GraphPtr createHalfAdder() {
  auto graph = std::make_shared<OrientedGraph>("half_adder");

  auto *a = graph->addInput("a");
  auto *b = graph->addInput("b");
  auto *xorGate = graph->addGate(Gates::GateXor, "xor1");
  auto *andGate = graph->addGate(Gates::GateAnd, "and1");
  auto *sum = graph->addOutput("sum");
  auto *carry = graph->addOutput("carry");

  graph->addEdges({a, b}, xorGate);
  graph->addEdges({a, b}, andGate);
  graph->addEdge(xorGate, sum);
  graph->addEdge(andGate, carry);
  graph->updateLevels();

  return graph;
}

/// @brief Создает полный сумматор
GraphPtr createFullAdder() {
  auto graph = std::make_shared<OrientedGraph>("full_adder");

  auto *a = graph->addInput("a");
  auto *b = graph->addInput("b");
  auto *cin = graph->addInput("cin");

  auto *xor1 = graph->addGate(Gates::GateXor, "xor1");
  auto *xor2 = graph->addGate(Gates::GateXor, "xor2");
  auto *and1 = graph->addGate(Gates::GateAnd, "and1");
  auto *and2 = graph->addGate(Gates::GateAnd, "and2");
  auto *or1 = graph->addGate(Gates::GateOr, "or1");

  auto *sum = graph->addOutput("sum");
  auto *cout = graph->addOutput("cout");

  // sum = a ^ b ^ cin
  graph->addEdges({a, b}, xor1);
  graph->addEdges({xor1, cin}, xor2);
  graph->addEdge(xor2, sum);

  // cout = (a & b) | (cin & (a ^ b))
  graph->addEdges({a, b}, and1);
  graph->addEdges({xor1, cin}, and2);
  graph->addEdges({and1, and2}, or1);
  graph->addEdge(or1, cout);

  graph->updateLevels();
  return graph;
}

int main() {
  std::cout << "=== CircuitGenGraph Testbench Generator Demo ===" << std::endl;
  std::cout << std::endl;

  // ===== Пример 1: Полусумматор с полным перебором =====
  std::cout << "--- Example 1: Half Adder with Exhaustive Testing ---"
            << std::endl;
  {
    auto graph = createHalfAdder();
    TestbenchGenerator tbGen(graph);

    // Генерируем все комбинации входов
    size_t count = tbGen.generateExhaustiveVectors();
    std::cout << "Generated " << count << " test vectors" << std::endl;

    // Запускаем внутреннюю симуляцию
    auto result = tbGen.runInternalSimulation();
    std::cout << "Internal simulation: " << result.passedTests << "/"
              << result.totalTests << " passed" << std::endl;

    // Сохраняем тестбенч
    tbGen.toVerilogTestbench("./output", "half_adder_tb");
    std::cout << "Testbench saved to ./output/half_adder_tb.v" << std::endl;

    // Сохраняем схему
    graph->toVerilog("./output", "half_adder");
    std::cout << "Module saved to ./output/half_adder.v" << std::endl;

    // Если Icarus доступен, запускаем верификацию
    if (TestbenchGenerator::isIcarusAvailable()) {
      std::cout << "Running Icarus Verilog verification..." << std::endl;
      auto icarusResult = tbGen.runIcarusVerification("./output");
      std::cout << "Icarus verification: " << icarusResult.passedTests << "/"
                << icarusResult.totalTests << " passed" << std::endl;
      if (icarusResult.success) {
        std::cout << "STATUS: ALL TESTS PASSED" << std::endl;
      }
    } else {
      std::cout << "Icarus Verilog not available, skipping verification"
                << std::endl;
    }
  }
  std::cout << std::endl;

  // ===== Пример 2: Полный сумматор со случайными тестами =====
  std::cout << "--- Example 2: Full Adder with Random Testing ---" << std::endl;
  {
    auto graph = createFullAdder();

    TestbenchConfig config;
    config.generateVCD = true;
    config.vcdFilename = "full_adder.vcd";
    config.stimulusDelay = 10;

    TestbenchGenerator tbGen(graph, config);

    // Генерируем случайные тесты
    size_t count = tbGen.generateRandomVectors(20, 42);
    std::cout << "Generated " << count << " random test vectors" << std::endl;

    // Проверяем результаты
    auto result = tbGen.runInternalSimulation();
    std::cout << "Internal simulation: " << result.passedTests << "/"
              << result.totalTests << " passed" << std::endl;
    
    // Показываем проваленные тесты
    if (result.failedTests > 0) {
      std::cout << "WARNING: " << result.failedTests << " test(s) failed!" << std::endl;
      for (size_t i = 0; i < result.vectors.size(); ++i) {
        if (!result.vectors[i].passed) {
          auto &tv = result.vectors[i];
          std::cout << "  FAILED Test " << i << ": ";
          std::cout << "a=" << tv.inputs[0] << " b=" << tv.inputs[1] 
                    << " cin=" << tv.inputs[2];
          std::cout << " Expected: sum=" << tv.expected[0] << " cout=" << tv.expected[1];
          std::cout << " Got: sum=" << tv.actual[0] << " cout=" << tv.actual[1] << std::endl;
        }
      }
    }

    // Выводим несколько тестовых векторов
    std::cout << "Sample test vectors:" << std::endl;
    auto &vectors = tbGen.getTestVectors();
    for (size_t i = 0; i < std::min(size_t(5), vectors.size()); ++i) {
      auto &tv = vectors[i];
      std::cout << "  a=" << tv.inputs[0] << " b=" << tv.inputs[1]
                << " cin=" << tv.inputs[2] << " -> sum=" << tv.expected[0]
                << " cout=" << tv.expected[1] << std::endl;
    }
  }
  std::cout << std::endl;

  // ===== Пример 3: Пользовательские тестовые векторы =====
  std::cout << "--- Example 3: Custom Test Vectors ---" << std::endl;
  {
    auto graph = createHalfAdder();
    TestbenchGenerator tbGen(graph);

    // Добавляем только интересующие нас комбинации
    tbGen.addTestVector({'0', '0'}); // Ожидаемый результат вычислится автоматически
    tbGen.addTestVector({'1', '1'});
    tbGen.addTestVector({'0', '1'}, {'1', '0'}); // С явным указанием ожидаемых выходов

    std::cout << "Added " << tbGen.getTestVectorCount() << " custom test vectors"
              << std::endl;

    auto result = tbGen.runInternalSimulation();
    std::cout << "Simulation result: " << result.passedTests << "/"
              << result.totalTests << " passed" << std::endl;
  }
  std::cout << std::endl;

  // ===== Пример 4: Получение кода тестбенча как строки =====
  std::cout << "--- Example 4: Get Testbench as String ---" << std::endl;
  {
    auto graph = createHalfAdder();
    TestbenchGenerator tbGen(graph);
    tbGen.addTestVector({'0', '0'});
    tbGen.addTestVector({'1', '1'});

    std::string code = tbGen.getTestbenchCode();
    std::cout << "Generated testbench (" << code.size() << " bytes):" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // Выводим первые 1000 символов
    std::cout << code.substr(0, 1000) << "..." << std::endl;
    std::cout << "----------------------------------------" << std::endl;
  }

  std::cout << std::endl;
  std::cout << "=== Demo completed ===" << std::endl;

  return 0;
}
