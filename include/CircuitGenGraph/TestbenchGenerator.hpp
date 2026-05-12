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

/**
 * @file TestbenchGenerator.hpp
 * @author Theossr <feolab05@gmail.com>
 * * \~english
 * @brief Testbench generation and verification system for generated circuits.
 * * \~russian
 * @brief Система генерации тестбенчей и верификации для сгенерированных схем.
 */

namespace CG_Graph {

/**
 * \~english
 * @brief Information about a port of the golden Verilog model (for comparison
 * with the graph).
 * \~russian
 * @brief Информация о порте эталонной Verilog-модели (для сравнения с графом).
 */
struct PortInfo {
  std::string name;

  /**
   * \~english
   * @brief 1 for single-bit, N for vectors [N-1:0].
   * \~russian
   * @brief 1 для однобитных, N для векторов [N-1:0].
   */
  int width;

  /**
   * \~english
   * @brief True if the port is an input.
   * \~russian
   * @brief True, если порт является входным.
   */
  bool is_input = false;
};

/**
 * \~english
 * @brief Simulation result of a single test vector.
 * \~russian
 * @brief Результат симуляции одного тестового вектора.
 */
struct TestVector {
  /**
   * \~english
   * @brief Input values.
   * \~russian
   * @brief Входные значения.
   */
  std::vector<char> inputs;

  /**
   * \~english
   * @brief Expected output values.
   * \~russian
   * @brief Ожидаемые выходные значения.
   */
  std::vector<char> expected;

  /**
   * \~english
   * @brief Actual output values (after simulation).
   * \~russian
   * @brief Фактические выходные значения (после симуляции).
   */
  std::vector<char> actual;

  /**
   * \~english
   * @brief True if the test passed.
   * \~russian
   * @brief Прошел ли тест.
   */
  bool passed = false;
};

/**
 * \~english
 * @brief Result of circuit verification.
 * \~russian
 * @brief Результат верификации схемы.
 */
struct VerificationResult {
  /**
   * \~english
   * @brief Overall verification result.
   * \~russian
   * @brief Общий результат верификации.
   */
  bool success = false;

  /**
   * \~english
   * @brief Total number of tests.
   * \~russian
   * @brief Общее количество тестов.
   */
  size_t totalTests = 0;

  /**
   * \~english
   * @brief Number of passed tests.
   * \~russian
   * @brief Количество пройденных тестов.
   */
  size_t passedTests = 0;

  /**
   * \~english
   * @brief Number of failed tests.
   * \~russian
   * @brief Количество проваленных тестов.
   */
  size_t failedTests = 0;

  /**
   * \~english
   * @brief All test vectors.
   * \~russian
   * @brief Все тестовые векторы.
   */
  std::vector<TestVector> vectors;

  /**
   * \~english
   * @brief Error message (if any).
   * \~russian
   * @brief Сообщение об ошибке (если есть).
   */
  std::string errorMessage;

  /**
   * \~english
   * @brief Simulator output.
   * \~russian
   * @brief Вывод симулятора.
   */
  std::string simulatorOutput;

  /**
   * \~english
   * @brief Simulator exit code.
   * \~russian
   * @brief Код возврата симулятора.
   */
  int simulatorExitCode = 0;
};

/**
 * \~english
 * @brief Configuration of the testbench generator.
 * \~russian
 * @brief Конфигурация генератора тестбенчей.
 */
struct TestbenchConfig {
  /**
   * \~english
   * @brief Timescale for Verilog.
   * \~russian
   * @brief Timescale для Verilog.
   */
  std::string timescale = "1ns/1ps";

  /**
   * \~english
   * @brief Clock period (in timescale units).
   * \~russian
   * @brief Период тактового сигнала (в единицах timescale).
   */
  uint32_t clockPeriod = 10;

  /**
   * \~english
   * @brief Reset duration.
   * \~russian
   * @brief Длительность сброса.
   */
  uint32_t resetDuration = 20;

  /**
   * \~english
   * @brief Delay between stimuli.
   * \~russian
   * @brief Задержка между стимулами.
   */
  uint32_t stimulusDelay = 5;

  /**
   * \~english
   * @brief Generate VCD file for GTKWave.
   * \~russian
   * @brief Генерировать VCD файл для GTKWave.
   */
  bool generateVCD = true;

  /**
   * \~english
   * @brief VCD filename.
   * \~russian
   * @brief Имя VCD файла.
   */
  std::string vcdFilename = "dump.vcd";

  /**
   * \~english
   * @brief Detailed output.
   * \~russian
   * @brief Подробный вывод.
   */
  bool verbose = false;

  /**
   * \~english
   * @brief Simulation timeout (in timescale units).
   * \~russian
   * @brief Таймаут симуляции (в единицах timescale).
   */
  uint32_t timeout = 1000;
};

/**
 * \~english
 * @brief Strategy for generating test vectors.
 * \~russian
 * @brief Стратегия генерации тестовых векторов.
 */
enum class TestVectorStrategy {
  /**
   * \~english
   * @brief Exhaustive search of all combinations (for small number of inputs).
   * \~russian
   * @brief Полный перебор всех комбинаций (для малого числа входов).
   */
  Exhaustive,

  /**
   * \~english
   * @brief Random test vectors.
   * \~russian
   * @brief Случайные тестовые векторы.
   */
  Random,

  /**
   * \~english
   * @brief Custom test vectors.
   * \~russian
   * @brief Пользовательские тестовые векторы.
   */
  Custom
};

/**
 * \~english
 * @brief Class for generating Verilog testbenches.
 *
 * Allows automatic generation of testbenches to verify generated
 * combinational and sequential circuits.
 *
 * @code
 * // Creating a circuit graph
 * auto graph = std::make_shared<OrientedGraph>("my_circuit");
 * // ... adding circuit elements ...
 *
 * // Generating a testbench
 * TestbenchGenerator tbGen(graph);
 * tbGen.generateExhaustiveVectors();
 * tbGen.toVerilogTestbench("./", "my_circuit_tb");
 *
 * // Running verification with Icarus Verilog
 * auto result = tbGen.runIcarusVerification("./");
 * if (result.success) {
 * std::cout << "Verification passed!" << std::endl;
 * }
 * @endcode
 *
 * \~russian
 * @brief Класс для генерации Verilog тестбенчей.
 *
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
 * std::cout << "Verification passed!" << std::endl;
 * }
 * @endcode
 */
class TestbenchGenerator {
public:
  /**
   * \~english
   * @brief Constructor.
   * @param i_graph Pointer to the circuit graph.
   * @param i_config Generator configuration (optional).
   * * \~russian
   * @brief Конструктор.
   * @param i_graph Указатель на граф схемы.
   * @param i_config Конфигурация генератора (опционально).
   */
  explicit TestbenchGenerator(
      GraphPtr i_graph, const TestbenchConfig &i_config = TestbenchConfig());

  /**
   * \~english
   * @brief Destructor.
   * \~russian
   * @brief Деструктор.
   */
  ~TestbenchGenerator() = default;

  // Disable copying
  TestbenchGenerator(const TestbenchGenerator &) = delete;
  TestbenchGenerator &operator=(const TestbenchGenerator &) = delete;

  // Enable moving
  TestbenchGenerator(TestbenchGenerator &&) = default;
  TestbenchGenerator &operator=(TestbenchGenerator &&) = default;

  /**
   * \~english
   * @brief Exhaustive testbench: graph vs golden Verilog model.
   * @param graph Circuit graph.
   * @param goldenModelPath Path to the reference golden model.
   * @param outputTbPath Output path for the testbench.
   * @return True if successful.
   * * \~russian
   * @brief Тестбенч полного перебора: граф против эталонной Verilog-модели.
   * @param graph Граф схемы.
   * @param goldenModelPath Путь к эталонной модели.
   * @param outputTbPath Путь для сохранения тестбенча.
   * @return True в случае успеха.
   */
  static bool generate(std::shared_ptr<OrientedGraph> graph,
                       const std::string &goldenModelPath,
                       const std::string &outputTbPath);

  // ==================== Test Vectors Generation ====================

  /**
   * \~english
   * @brief Generates all possible input combinations (exhaustive search).
   * @warning For N inputs, 2^N tests are generated. Not recommended for N > 20.
   * @return Number of generated test vectors.
   * * \~russian
   * @brief Генерирует все возможные комбинации входов (полный перебор).
   * @warning Для N входов генерируется 2^N тестов. Не рекомендуется для N > 20.
   * @return Количество сгенерированных тестовых векторов.
   */
  size_t generateExhaustiveVectors();

  /**
   * \~english
   * @brief Generates random test vectors.
   * @param i_count Number of test vectors.
   * @param i_seed Seed for the random number generator (0 = random).
   * @return Number of generated test vectors.
   * * \~russian
   * @brief Генерирует случайные тестовые векторы.
   * @param i_count Количество тестовых векторов.
   * @param i_seed Seed для генератора случайных чисел (0 = случайный).
   * @return Количество сгенерированных тестовых векторов.
   */
  size_t generateRandomVectors(size_t i_count, uint32_t i_seed = 0);

  /**
   * \~english
   * @brief Adds a custom test vector.
   * @param i_inputs Input values.
   * @param i_expected Expected output values (optional, will be computed if
   * empty).
   * * \~russian
   * @brief Добавляет пользовательский тестовый вектор.
   * @param i_inputs Входные значения.
   * @param i_expected Ожидаемые выходные значения (опционально, будут
   * вычислены).
   */
  void addTestVector(const std::vector<char> &i_inputs,
                     const std::vector<char> &i_expected = {});

  /**
   * \~english
   * @brief Clears all test vectors.
   * \~russian
   * @brief Очищает все тестовые векторы.
   */
  void clearTestVectors();

  /**
   * \~english
   * @brief Returns the number of test vectors.
   * @return Test vector count.
   * * \~russian
   * @brief Возвращает количество тестовых векторов.
   * @return Количество тестовых векторов.
   */
  size_t getTestVectorCount() const { return d_testVectors.size(); }

  /**
   * \~english
   * @brief Returns the test vectors.
   * @return Constant reference to test vectors.
   * * \~russian
   * @brief Возвращает тестовые векторы.
   * @return Константная ссылка на тестовые векторы.
   */
  const std::vector<TestVector> &getTestVectors() const {
    return d_testVectors;
  }

  // ==================== Testbench Generation ====================

  /**
   * \~english
   * @brief Generates a Verilog testbench.
   * @param i_path Path to save the file.
   * @param i_filename Filename (without extension).
   * @return True if successful.
   * * \~russian
   * @brief Генерирует Verilog тестбенч.
   * @param i_path Путь для сохранения файла.
   * @param i_filename Имя файла (без расширения).
   * @return True в случае успеха.
   */
  bool toVerilogTestbench(const std::string &i_path,
                          const std::string &i_filename = "");

  /**
   * \~english
   * @brief Returns the testbench content as a string.
   * @return Verilog code of the testbench.
   * * \~russian
   * @brief Возвращает содержимое тестбенча как строку.
   * @return Verilog код тестбенча.
   */
  std::string getTestbenchCode() const;

  // ==================== Simulation and Verification ====================

  /**
   * \~english
   * @brief Performs simulation using the internal simulator (graphSimulation).
   * @return Verification result.
   * * \~russian
   * @brief Выполняет симуляцию на встроенном симуляторе (graphSimulation).
   * @return Результат верификации.
   */
  VerificationResult runInternalSimulation();

  /**
   * \~english
   * @brief Performs verification using Icarus Verilog.
   * @param i_workDir Working directory.
   * @param i_icarusPath Path to iverilog (defaults to searching in PATH).
   * @param i_vvpPath Path to vvp (defaults to searching in PATH).
   * @return Verification result.
   * * \~russian
   * @brief Выполняет верификацию с использованием Icarus Verilog.
   * @param i_workDir Рабочая директория.
   * @param i_icarusPath Путь к iverilog (по умолчанию ищется в PATH).
   * @param i_vvpPath Путь к vvp (по умолчанию ищется в PATH).
   * @return Результат верификации.
   */
  VerificationResult
  runIcarusVerification(const std::string &i_workDir,
                        const std::string &i_icarusPath = "iverilog",
                        const std::string &i_vvpPath = "vvp");

  /**
   * \~english
   * @brief Compares results of internal simulation and Icarus Verilog.
   * @param i_workDir Working directory.
   * @return Comparison result.
   * * \~russian
   * @brief Сравнивает результаты внутренней симуляции и Icarus Verilog.
   * @param i_workDir Рабочая директория.
   * @return Результат сравнения.
   */
  VerificationResult compareSimulations(const std::string &i_workDir);

  // ==================== Configuration ====================

  /**
   * \~english
   * @brief Sets the configuration.
   * @param i_config New configuration.
   * * \~russian
   * @brief Устанавливает конфигурацию.
   * @param i_config Новая конфигурация.
   */
  void setConfig(const TestbenchConfig &i_config) { d_config = i_config; }

  /**
   * \~english
   * @brief Returns the current configuration.
   * @return Configuration.
   * * \~russian
   * @brief Возвращает текущую конфигурацию.
   * @return Конфигурация.
   */
  const TestbenchConfig &getConfig() const { return d_config; }

  // ==================== Utilities ====================

  /**
   * \~english
   * @brief Checks if Icarus Verilog is installed.
   * @param i_icarusPath Path to iverilog.
   * @return True if Icarus Verilog is available.
   * * \~russian
   * @brief Проверяет, установлен ли Icarus Verilog.
   * @param i_icarusPath Путь к iverilog.
   * @return True если Icarus Verilog доступен.
   */
  static bool isIcarusAvailable(const std::string &i_icarusPath = "iverilog");

  /**
   * \~english
   * @brief Converts a char vector to a Verilog string literal.
   * @param i_values Vector of values.
   * @return String in Verilog format (e.g., "4'b0101").
   * * \~russian
   * @brief Конвертирует вектор char в строку для Verilog.
   * @param i_values Вектор значений.
   * @return Строка в формате Verilog (например, "4'b0101").
   */
  static std::string toVerilogLiteral(const std::vector<char> &i_values);

  /**
   * \~english
   * @brief Parses Icarus Verilog output to extract results.
   * @param i_output Simulator output.
   * @return Vector of results.
   * * \~russian
   * @brief Парсит вывод Icarus Verilog для извлечения результатов.
   * @param i_output Вывод симулятора.
   * @return Вектор результатов.
   */
  std::vector<std::vector<char>> parseIcarusOutput(const std::string &i_output);

private:
  /**
   * \~english
   * @brief Generates the testbench header.
   * \~russian
   * @brief Генерирует заголовок тестбенча.
   */
  std::string generateHeader() const;

  /**
   * \~english
   * @brief Generates signal declarations.
   * \~russian
   * @brief Генерирует объявления сигналов.
   */
  std::string generateSignalDeclarations() const;

  /**
   * \~english
   * @brief Generates DUT (Device Under Test) instantiation.
   * \~russian
   * @brief Генерирует инстанцирование DUT (Device Under Test).
   */
  std::string generateDUTInstantiation() const;

  /**
   * \~english
   * @brief Generates the stimulus block.
   * \~russian
   * @brief Генерирует блок stimulus.
   */
  std::string generateStimulusBlock() const;

  /**
   * \~english
   * @brief Generates the result checking block.
   * \~russian
   * @brief Генерирует блок проверки результатов.
   */
  std::string generateCheckBlock() const;

  /**
   * \~english
   * @brief Generates the VCD dump block.
   * \~russian
   * @brief Генерирует блок VCD dump.
   */
  std::string generateVCDDump() const;

  /**
   * \~english
   * @brief Computes expected outputs for an input vector.
   * @param i_inputs Input values.
   * @return Expected output values.
   * * \~russian
   * @brief Вычисляет ожидаемые выходы для вектора входов.
   * @param i_inputs Вектор входов.
   * @return Ожидаемые выходные значения.
   */
  std::vector<char> computeExpectedOutputs(const std::vector<char> &i_inputs);

  /**
   * \~english
   * @brief Executes a command and returns the result.
   * @param i_command Command to execute.
   * @return Pair of exit code and output string.
   * * \~russian
   * @brief Выполняет команду и возвращает результат.
   * @param i_command Команда для выполнения.
   * @return Пара (код возврата, вывод команды).
   */
  std::pair<int, std::string> executeCommand(const std::string &i_command);

  /**
   * \~english
   * @brief Executes a command safely via fork+execvp (without shell).
   * @param i_argv Array of command arguments (first is the program name).
   * @return Pair of exit code and output string.
   * * \~russian
   * @brief Выполняет команду безопасно через fork+execvp (без shell).
   * @param i_argv Массив аргументов команды (первый - имя программы).
   * @return Пара (код возврата, вывод команды).
   */
  std::pair<int, std::string>
  executeCommandSafe(const std::vector<std::string> &i_argv);

  /**
   * \~english
   * @brief Parses the golden model to extract port information.
   * @param filepath Path to the golden model.
   * @param moduleName Extracted module name.
   * @return Vector of PortInfo.
   * * \~russian
   * @brief Парсит эталонную модель для извлечения информации о портах.
   * @param filepath Путь к эталонной модели.
   * @param moduleName Извлеченное имя модуля.
   * @return Вектор PortInfo.
   */
  static std::vector<PortInfo> parseGoldenModel(const std::string &filepath,
                                                std::string &moduleName);

  GraphPtr d_graph; /*!< \~english Pointer to the circuit graph \~russian
                       Указатель на граф схемы */
  TestbenchConfig d_config; /*!< \~english Generator configuration \~russian
                               Конфигурация генератора */
  std::vector<TestVector>
      d_testVectors; /*!< \~english Test vectors \~russian Тестовые векторы */
  std::vector<std::string>
      d_inputNames; /*!< \~english Input names \~russian Имена входов */
  std::vector<std::string>
      d_outputNames; /*!< \~english Output names \~russian Имена выходов */
  bool d_hasSequential =
      false; /*!< \~english True if sequential elements exist \~russian Есть ли
                последовательностные элементы */
};

} // namespace CG_Graph