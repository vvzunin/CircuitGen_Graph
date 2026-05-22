#pragma once

#include <cstdint>
#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <vector>

#include "CircuitGenGraph/OrientedGraph.hpp"

/// \~english
/// @file TestbenchGenerator.hpp
/// @brief Testbench generation and verification subsystem.
///
/// \~russian
/// @brief Система генерации тестбенчей и верификации для сгенерированных схем.

namespace CG_Graph {

/// \~english
/// @brief Port information of a golden Verilog model.
///
/// \~russian
/// @brief Информация о порте эталонной Verilog-модели (для сравнения с графом).
struct PortInfo {
  std::string name;
  int width; ///< 1 для однобитных, N для векторов [N-1:0]
  bool is_input = false;
};

/// \~english
/// @brief Simulation result for a single test vector.
///
/// \~russian
/// @brief Результат симуляции одного тестового вектора.
struct TestVector {
  std::vector<char> inputs; ///< Входные значения
  std::vector<char> expected; ///< Ожидаемые выходные значения
  std::vector<char> actual; ///< Фактические выходные значения (после симуляции)
  bool passed = false;      ///< Прошел ли тест
};

/// \~english
/// @brief Verification result for a circuit.
///
/// \~russian
/// @brief Результат верификации схемы.
struct VerificationResult {
  bool success = false; ///< Общий результат верификации
  size_t totalTests = 0; ///< Общее количество тестов
  size_t passedTests = 0; ///< Количество пройденных тестов
  size_t failedTests = 0; ///< Количество проваленных тестов
  std::vector<TestVector> vectors; ///< Все тестовые векторы
  std::string errorMessage; ///< Сообщение об ошибке (если есть)
  std::string simulatorOutput; ///< Вывод симулятора
  int simulatorExitCode = 0;   ///< Код возврата симулятора
};

/// \~english
/// @brief Testbench generator configuration.
///
/// \~russian
/// @brief Конфигурация генератора тестбенчей.
struct TestbenchConfig {
  std::string timescale = "1ns/1ps"; ///< Timescale для Verilog
  uint32_t clockPeriod =
      10; ///< Период тактового сигнала (в единицах timescale)
  uint32_t resetDuration = 20; ///< Длительность сброса
  uint32_t stimulusDelay = 5; ///< Задержка между стимулами
  bool generateVCD = true; ///< Генерировать VCD файл для GTKWave
  std::string vcdFilename = "dump.vcd"; ///< Имя VCD файла
  bool verbose = false;                 ///< Подробный вывод
  uint32_t timeout = 1000; ///< Таймаут симуляции (в единицах timescale)
};

/// \~english
/// @brief Test-vector generation strategy.
///
/// \~russian
/// @brief Стратегия генерации тестовых векторов.
enum class TestVectorStrategy {
  Exhaustive, ///< Полный перебор всех комбинаций (для малого числа входов)
  Random,     ///< Случайные тестовые векторы
  Custom ///< Пользовательские тестовые векторы
};

/**
 * \~english
 * @brief Class for generating Verilog testbenches.
 * Supports automated verification for combinational and sequential circuits.
 *
 * \~russian
 * @brief Класс для генерации Verilog тестбенчей.
 * Позволяет автоматически генерировать тестбенчи для верификации
 * сгенерированных комбинационных и последовательностных схем.
 *
 * @code
 * // Создание графа схемы
 * auto graph = std::make_shared<OrientedGraph>("my_circuit");
 * // ... добавление элементов схемы ...
 *
 * // Генерация тестбенча
 * TestbenchGenerator tbGen(graph);
 * tbGen.generateExhaustiveVectors();
 * tbGen.toVerilogTestbench("./", "my_circuit_tb");
 *
 * // Запуск верификации с Icarus Verilog
 * auto result = tbGen.runIcarusVerification("./");
 * if (result.success) {
 *     std::cout << "Verification passed!" << std::endl;
 * }
 * @endcode
 */
class TestbenchGenerator {
public:
  /// \~english
  /// @brief Constructor.
  /// @param i_graph Shared pointer to circuit graph.
  /// @param i_config Generator configuration (optional).
  ///
  /// \~russian
  /// @brief Конструктор.
  /// @param i_graph Shared-указатель на граф схемы.
  /// @param i_config Конфигурация генератора (опционально).
  explicit TestbenchGenerator(
      GraphPtr i_graph, const TestbenchConfig &i_config = TestbenchConfig());

  /// \~english
  /// @brief Destructor.
  ///
  /// \~russian
  /// @brief Деструктор.
  ~TestbenchGenerator() = default;

  // Запрет копирования
  TestbenchGenerator(const TestbenchGenerator &) = delete;
  TestbenchGenerator &operator=(const TestbenchGenerator &) = delete;

  // Разрешение перемещения
  TestbenchGenerator(TestbenchGenerator &&) = default;
  TestbenchGenerator &operator=(TestbenchGenerator &&) = default;

  /// \~english
  /// @brief Exhaustive testbench: graph against golden Verilog model.
  ///
  /// \~russian
  /// @brief Тестбенч полного перебора: граф против эталонной Verilog-модели.
  static bool generate(std::shared_ptr<OrientedGraph> graph,
                       const std::string &goldenModelPath,
                       const std::string &outputTbPath);

  // ==================== Генерация тестовых векторов ====================

  /// \~english
  /// @brief Generates all possible input combinations (exhaustive mode).
  /// @warning For N inputs it creates 2^N tests. Not recommended for N > 20.
  /// @return Number of generated test vectors.
  ///
  /// \~russian
  /// @brief Генерирует все возможные комбинации входов (полный перебор).
  /// @warning Для N входов генерируется 2^N тестов. Не рекомендуется для N
  /// > 20.
  /// @return Количество сгенерированных тестовых векторов.
  size_t generateExhaustiveVectors();

  /// \~english
  /// @brief Generates random test vectors.
  /// @param i_count Number of test vectors.
  /// @param i_seed Seed for random generator (`0` means random seed).
  /// @return Number of generated test vectors.
  ///
  /// \~russian
  /// @brief Генерирует случайные тестовые векторы.
  /// @param i_count Количество тестовых векторов.
  /// @param i_seed Seed для генератора случайных чисел (0 = случайный).
  /// @return Количество сгенерированных тестовых векторов.
  size_t generateRandomVectors(size_t i_count, uint32_t i_seed = 0);

  /// \~english
  /// @brief Adds a custom test vector.
  /// @param i_inputs Input values.
  /// @param i_expected Expected output values (optional, can be computed).
  ///
  /// \~russian
  /// @brief Добавляет пользовательский тестовый вектор.
  /// @param i_inputs Входные значения.
  /// @param i_expected Ожидаемые выходные значения (опционально, будут
  /// вычислены).
  void addTestVector(const std::vector<char> &i_inputs,
                     const std::vector<char> &i_expected = {});

  /// \~english
  /// @brief Clears all test vectors.
  ///
  /// \~russian
  /// @brief Очищает все тестовые векторы.
  void clearTestVectors();

  /// \~english
  /// @brief Returns number of test vectors.
  ///
  /// \~russian
  /// @brief Возвращает количество тестовых векторов.
  size_t getTestVectorCount() const { return d_testVectors.size(); }

  /// \~english
  /// @brief Returns current test vectors.
  ///
  /// \~russian
  /// @brief Возвращает тестовые векторы.
  const std::vector<TestVector> &getTestVectors() const {
    return d_testVectors;
  }

  // ==================== Генерация тестбенча ====================

  /// \~english
  /// @brief Generates Verilog testbench.
  /// @param i_path Output directory path.
  /// @param i_filename File name (without extension).
  /// @return `true` on success.
  ///
  /// \~russian
  /// @brief Генерирует Verilog тестбенч.
  /// @param i_path Путь для сохранения файла.
  /// @param i_filename Имя файла (без расширения).
  /// @return true если успешно.
  bool toVerilogTestbench(const std::string &i_path,
                          const std::string &i_filename = "");

  /// \~english
  /// @brief Returns generated testbench content as string.
  /// @return Verilog testbench code.
  ///
  /// \~russian
  /// @brief Возвращает содержимое тестбенча как строку.
  /// @return Verilog код тестбенча.
  std::string getTestbenchCode() const;

  // ==================== Симуляция и верификация ====================

  /// \~english
  /// @brief Runs simulation using internal graph simulator.
  /// @return Verification result.
  ///
  /// \~russian
  /// @brief Выполняет симуляцию на встроенном симуляторе (graphSimulation).
  /// @return Результат верификации.
  VerificationResult runInternalSimulation();

  /// \~english
  /// @brief Runs verification with Icarus Verilog.
  /// @param i_workDir Working directory.
  /// @param i_icarusPath Path to `iverilog` (searched in PATH by default).
  /// @param i_vvpPath Path to `vvp` (searched in PATH by default).
  /// @return Verification result.
  ///
  /// \~russian
  /// @brief Выполняет верификацию с использованием Icarus Verilog.
  /// @param i_workDir Рабочая директория.
  /// @param i_icarusPath Путь к iverilog (по умолчанию ищется в PATH).
  /// @param i_vvpPath Путь к vvp (по умолчанию ищется в PATH).
  /// @return Результат верификации.
  VerificationResult
  runIcarusVerification(const std::string &i_workDir,
                        const std::string &i_icarusPath = "iverilog",
                        const std::string &i_vvpPath = "vvp");

  /// \~english
  /// @brief Compares internal simulation against Icarus Verilog.
  /// @param i_workDir Working directory.
  /// @return Comparison result.
  ///
  /// \~russian
  /// @brief Сравнивает результаты внутренней симуляции и Icarus Verilog.
  /// @param i_workDir Рабочая директория.
  /// @return Результат сравнения.
  VerificationResult compareSimulations(const std::string &i_workDir);

  // ==================== Конфигурация ====================

  /// \~english
  /// @brief Sets generator configuration.
  /// @param i_config New configuration.
  ///
  /// \~russian
  /// @brief Устанавливает конфигурацию.
  /// @param i_config Новая конфигурация.
  void setConfig(const TestbenchConfig &i_config) { d_config = i_config; }

  /// \~english
  /// @brief Returns current configuration.
  /// @return Configuration object.
  ///
  /// \~russian
  /// @brief Возвращает текущую конфигурацию.
  /// @return Конфигурация.
  const TestbenchConfig &getConfig() const { return d_config; }

  // ==================== Утилиты ====================

  /// \~english
  /// @brief Checks whether Icarus Verilog is available.
  /// @param i_icarusPath Path to `iverilog`.
  /// @return `true` if Icarus Verilog is available.
  ///
  /// \~russian
  /// @brief Проверяет, установлен ли Icarus Verilog.
  /// @param i_icarusPath Путь к iverilog.
  /// @return true если Icarus Verilog доступен.
  static bool isIcarusAvailable(const std::string &i_icarusPath = "iverilog");

  /// \~english
  /// @brief Converts char vector to Verilog literal string.
  /// @param i_values Vector of values.
  /// @return Verilog-formatted string (e.g. `4'b0101`).
  ///
  /// \~russian
  /// @brief Конвертирует вектор char в строку для Verilog.
  /// @param i_values Вектор значений.
  /// @return Строка в формате Verilog (например, "4'b0101").
  static std::string toVerilogLiteral(const std::vector<char> &i_values);

  /// \~english
  /// @brief Parses Icarus output and extracts simulation results.
  /// @param i_output Simulator output text.
  /// @return Parsed result vectors.
  ///
  /// \~russian
  /// @brief Парсит вывод Icarus Verilog для извлечения результатов.
  /// @param i_output Вывод симулятора.
  /// @return Вектор результатов.
  std::vector<std::vector<char>> parseIcarusOutput(const std::string &i_output);

private:
  /// \~english
  /// @brief Generates testbench header.
  ///
  /// \~russian
  /// @brief Генерирует заголовок тестбенча.
  std::string generateHeader() const;

  /// \~english
  /// @brief Generates signal declarations.
  ///
  /// \~russian
  /// @brief Генерирует объявления сигналов.
  std::string generateSignalDeclarations() const;

  /// \~english
  /// @brief Generates DUT (Device Under Test) instantiation.
  ///
  /// \~russian
  /// @brief Генерирует инстанцирование DUT (Device Under Test).
  std::string generateDUTInstantiation() const;

  /// \~english
  /// @brief Generates stimulus block.
  ///
  /// \~russian
  /// @brief Генерирует блок stimulus.
  std::string generateStimulusBlock() const;

  /// \~english
  /// @brief Generates result-check block.
  ///
  /// \~russian
  /// @brief Генерирует блок проверки результатов.
  std::string generateCheckBlock() const;

  /// \~english
  /// @brief Generates VCD dump block.
  ///
  /// \~russian
  /// @brief Генерирует блок VCD dump.
  std::string generateVCDDump() const;

  /// \~english
  /// @brief Computes expected outputs for one input vector.
  ///
  /// \~russian
  /// @brief Вычисляет ожидаемые выходы для вектора входов.
  std::vector<char> computeExpectedOutputs(const std::vector<char> &i_inputs);

  /// \~english
  /// @brief Executes command and returns code/output pair.
  ///
  /// \~russian
  /// @brief Выполняет команду и возвращает результат.
  std::pair<int, std::string> executeCommand(const std::string &i_command);

  /// \~english
  /// @brief Executes command safely via fork+execvp (without shell).
  /// @param i_argv Command arguments (`argv[0]` is executable name).
  /// @return Pair of exit code and captured output.
  ///
  /// \~russian
  /// @brief Выполняет команду безопасно через fork+execvp (без shell).
  /// @param i_argv Массив аргументов команды (первый - имя программы).
  /// @return Пара (код возврата, вывод команды).
  std::pair<int, std::string>
  executeCommandSafe(const std::vector<std::string> &i_argv);

  static std::vector<PortInfo> parseGoldenModel(const std::string &filepath,
                                                std::string &moduleName);

  GraphPtr d_graph; ///< Shared-указатель на граф схемы
  TestbenchConfig d_config; ///< Конфигурация генератора
  std::vector<TestVector> d_testVectors;  ///< Тестовые векторы
  std::vector<std::string> d_inputNames;  ///< Имена входов
  std::vector<std::string> d_outputNames; ///< Имена выходов
  bool d_hasSequential = false; ///< Есть ли последовательностные элементы
};

} // namespace CG_Graph
