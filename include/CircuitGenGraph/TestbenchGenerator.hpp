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
/// @brief Testbench generation and verification subsystem (combinational and
/// sequential).
///
/// \~russian
/// @brief Система генерации тестбенчей и верификации для сгенерированных схем
/// (комбинационных и последовательностных).

namespace CG_Graph {

/// \~english
/// @brief Port information of a golden Verilog model.
///
/// \~russian
/// @brief Информация о порте эталонной Verilog-модели (для сравнения с графом).
struct PortInfo {
  /*!
   * \~english Port name
   * \~russian Имя порта
   */
  std::string name;
  /*!
   * \~english `1` for single-bit ports, `N` for vectors `[N-1:0]`
   * \~russian `1` для однобитных портов, `N` для векторов `[N-1:0]`
   */
  int width;
  /*!
   * \~english `true` if the port is an input
   * \~russian `true`, если порт является входом
   */
  bool is_input = false;
};

/// \~english
/// @brief Simulation result for a single test vector.
///
/// \~russian
/// @brief Результат симуляции одного тестового вектора.
struct TestVector {
  /*!
   * \~english Input values
   * \~russian Входные значения
   */
  std::vector<char> inputs;
  /*!
   * \~english Expected output values
   * \~russian Ожидаемые выходные значения
   */
  std::vector<char> expected;
  /*!
   * \~english Actual output values (after simulation)
   * \~russian Фактические выходные значения (после симуляции)
   */
  std::vector<char> actual;
  /*!
   * \~english `true` if the test passed
   * \~russian `true`, если тест пройден
   */
  bool passed = false;
};

/// \~english
/// @brief Verification result for a circuit.
///
/// \~russian
/// @brief Результат верификации схемы.
struct VerificationResult {
  /*!
   * \~english Overall verification result
   * \~russian Общий результат верификации
   */
  bool success = false;
  /*!
   * \~english Total number of tests
   * \~russian Общее количество тестов
   */
  size_t totalTests = 0;
  /*!
   * \~english Number of passed tests
   * \~russian Количество пройденных тестов
   */
  size_t passedTests = 0;
  /*!
   * \~english Number of failed tests
   * \~russian Количество проваленных тестов
   */
  size_t failedTests = 0;
  /*!
   * \~english All test vectors
   * \~russian Все тестовые векторы
   */
  std::vector<TestVector> vectors;
  /*!
   * \~english Error message (if any)
   * \~russian Сообщение об ошибке (если есть)
   */
  std::string errorMessage;
  /*!
   * \~english Simulator output
   * \~russian Вывод симулятора
   */
  std::string simulatorOutput;
  /*!
   * \~english Simulator exit code
   * \~russian Код возврата симулятора
   */
  int simulatorExitCode = 0;
};

/// \~english
/// @brief Testbench generator configuration.
///
/// \~russian
/// @brief Конфигурация генератора тестбенчей.
struct TestbenchConfig {
  /*!
   * \~english Verilog timescale
   * \~russian Timescale для Verilog
   */
  std::string timescale = "1ns/1ps";
  /*!
   * \~english Clock period (in timescale units)
   * \~russian Период тактового сигнала (в единицах timescale)
   */
  uint32_t clockPeriod = 10;
  /*!
   * \~english Reset duration
   * \~russian Длительность сброса
   */
  uint32_t resetDuration = 20;
  /*!
   * \~english Delay between stimuli
   * \~russian Задержка между стимулами
   */
  uint32_t stimulusDelay = 5;
  /*!
   * \~english Generate a VCD file for GTKWave
   * \~russian Генерировать VCD-файл для GTKWave
   */
  bool generateVCD = true;
  /*!
   * \~english VCD file name
   * \~russian Имя VCD-файла
   */
  std::string vcdFilename = "dump.vcd";
  /*!
   * \~english Verbose output
   * \~russian Подробный вывод
   */
  bool verbose = false;
  /*!
   * \~english Simulation timeout (in timescale units)
   * \~russian Таймаут симуляции (в единицах timescale)
   */
  uint32_t timeout = 1000;
  /*!
   * \~english Active reset level (`0` = active-low, `1` = active-high)
   * \~russian Активный уровень сброса (`0` = active-low, `1` = active-high)
   */
  uint32_t resetActiveValue = 0;
};

/// \~english
/// @brief Test vector for one clock cycle of a sequential circuit.
///
/// \~russian
/// @brief Тестовый вектор для одного такта последовательностной схемы.
struct SequentialTestVector {
  /*!
   * \~english Data input values (excluding clk/rst)
   * \~russian Входные данные (без clk/rst)
   */
  std::vector<char> dataInputs;
  /*!
   * \~english Expected output values (optional)
   * \~russian Ожидаемые выходные значения (опционально)
   */
  std::vector<char> expectedOutputs;
  /*!
   * \~english Whether outputs should be checked on this cycle
   * \~russian Нужно ли проверять выходы на этом такте
   */
  bool checkOutputs = false;
};

/// \~english
/// @brief Test-vector generation strategy.
///
/// \~russian
/// @brief Стратегия генерации тестовых векторов.
enum class TestVectorStrategy {
  /*!
   * \~english Exhaustive enumeration of all combinations (small input count)
   * \~russian Полный перебор всех комбинаций (для малого числа входов)
   */
  Exhaustive,
  /*!
   * \~english Random test vectors
   * \~russian Случайные тестовые векторы
   */
  Random,
  /*!
   * \~english User-provided test vectors
   * \~russian Пользовательские тестовые векторы
   */
  Custom
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

  /// \~english Copying is disabled.
  /// \~russian Копирование запрещено.
  TestbenchGenerator(const TestbenchGenerator &) = delete;
  TestbenchGenerator &operator=(const TestbenchGenerator &) = delete;

  /// \~english Move operations are enabled.
  /// \~russian Перемещение разрешено.
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

  // ==================== Test-vector generation / Генерация тестовых векторов
  // ====================

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

  // ==================== Sequential tests / Последовательностные тесты
  // ====================

  /// \~english
  /// @brief Generates random sequential test vectors (one per cycle).
  /// @param i_numCycles Number of simulation cycles.
  /// @param i_seed Random seed (`0` means random seed).
  /// @return Number of generated cycles.
  ///
  /// \~russian
  /// @brief Генерирует случайные тестовые векторы для последовательностной
  /// схемы.
  /// @param i_numCycles Количество тактов симуляции.
  /// @param i_seed Seed для генератора (`0` = случайный).
  /// @return Количество сгенерированных тактов.
  size_t generateSequentialTestVectors(size_t i_numCycles, uint32_t i_seed = 0);

  /// \~english
  /// @brief Adds a sequential test vector for one cycle.
  /// @param i_dataInputs Data input values (excluding clk/rst).
  /// @param i_expectedOutputs Expected outputs (optional).
  /// @param i_checkOutputs Whether to check outputs on this cycle.
  ///
  /// \~russian
  /// @brief Добавляет тестовый вектор для одного такта последовательностной
  /// схемы.
  /// @param i_dataInputs Входные значения (без clk/rst).
  /// @param i_expectedOutputs Ожидаемые выходы (опционально).
  /// @param i_checkOutputs Проверять ли выходы на этом такте.
  void addSequentialTestVector(const std::vector<char> &i_dataInputs,
                               const std::vector<char> &i_expectedOutputs = {},
                               bool i_checkOutputs = false);

  /// \~english
  /// @brief Clears all sequential test vectors.
  ///
  /// \~russian
  /// @brief Очищает последовательностные тестовые векторы.
  void clearSequentialTestVectors();

  /// \~english
  /// @brief Returns the number of sequential test vectors.
  ///
  /// \~russian
  /// @brief Возвращает количество последовательностных тестовых векторов.
  size_t getSequentialTestVectorCount() const {
    return d_seqTestVectors.size();
  }

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

  // ==================== Testbench generation / Генерация тестбенча
  // ====================

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

  // ==================== Simulation and verification / Симуляция и верификация
  // ====================

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

  // ==================== Configuration / Конфигурация ====================

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

  // ==================== Utilities / Утилиты ====================

  /// \~english
  /// @brief Checks whether the circuit contains sequential elements.
  /// @return `true` if flip-flops or latches are present.
  ///
  /// \~russian
  /// @brief Проверяет, содержит ли схема последовательностные элементы.
  /// @return `true`, если есть триггеры или защелки.
  bool isSequentialCircuit() const { return d_hasSequential; }

  /// \~english
  /// @brief Returns data input names (excluding clk/rst).
  ///
  /// \~russian
  /// @brief Возвращает имена входов данных (без clk/rst).
  const std::vector<std::string> &getDataInputNames() const {
    return d_dataInputNames;
  }

  /// \~english
  /// @brief Returns detected clock signal names.
  ///
  /// \~russian
  /// @brief Возвращает имена тактовых сигналов.
  const std::vector<std::string> &getClockNames() const { return d_clockNames; }

  /// \~english
  /// @brief Returns detected reset signal names.
  ///
  /// \~russian
  /// @brief Возвращает имена сигналов сброса.
  const std::vector<std::string> &getResetNames() const { return d_resetNames; }

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

  /// \~english
  /// @brief Parses ports of a golden Verilog model.
  /// @param filepath Path to the golden `.v` file.
  /// @param[out] moduleName Detected module name.
  /// @return Parsed port list.
  ///
  /// \~russian
  /// @brief Разбирает порты эталонной Verilog-модели.
  /// @param filepath Путь к эталонному `.v` файлу.
  /// @param[out] moduleName Обнаруженное имя модуля.
  /// @return Список портов.
  static std::vector<PortInfo> parseGoldenModel(const std::string &filepath,
                                                std::string &moduleName);

  // ==================== Sequential generation / Последовательностная генерация
  // ====================

  /// \~english
  /// @brief Detects clock and reset signals from the graph.
  ///
  /// \~russian
  /// @brief Определяет тактовые и сбросовые сигналы из графа.
  void detectClockSignals();

  /// \~english
  /// @brief Generates signal declarations for a sequential testbench.
  ///
  /// \~russian
  /// @brief Генерирует объявления сигналов для последовательностного тестбенча.
  std::string generateSequentialSignalDeclarations() const;

  /// \~english
  /// @brief Generates the clock-generation block.
  ///
  /// \~russian
  /// @brief Генерирует блок тактового сигнала.
  std::string generateClockBlock() const;

  /// \~english
  /// @brief Generates DUT instantiation for a sequential testbench.
  ///
  /// \~russian
  /// @brief Генерирует инстанцирование DUT для последовательностного тестбенча.
  std::string generateSequentialDUTInstantiation() const;

  /// \~english
  /// @brief Generates the stimulus block for a sequential testbench.
  ///
  /// \~russian
  /// @brief Генерирует блок стимулов для последовательностного тестбенча.
  std::string generateSequentialStimulusBlock() const;

  /*!
   * \~english Shared pointer to the circuit graph
   * \~russian Shared-указатель на граф схемы
   */
  GraphPtr d_graph;
  /*!
   * \~english Generator configuration
   * \~russian Конфигурация генератора
   */
  TestbenchConfig d_config;
  /*!
   * \~english Combinational test vectors
   * \~russian Тестовые векторы (комбинационные)
   */
  std::vector<TestVector> d_testVectors;
  /*!
   * \~english All input port names
   * \~russian Имена всех входов
   */
  std::vector<std::string> d_inputNames;
  /*!
   * \~english Output port names
   * \~russian Имена выходов
   */
  std::vector<std::string> d_outputNames;
  /*!
   * \~english `true` if the circuit contains sequential elements
   * \~russian `true`, если в схеме есть последовательностные элементы
   */
  bool d_hasSequential = false;

  /*!
   * \~english Per-cycle sequential test vectors
   * \~russian Тестовые векторы по тактам
   */
  std::vector<SequentialTestVector> d_seqTestVectors;
  /*!
   * \~english Clock signal names
   * \~russian Имена тактовых сигналов
   */
  std::vector<std::string> d_clockNames;
  /*!
   * \~english Reset signal names
   * \~russian Имена сигналов сброса
   */
  std::vector<std::string> d_resetNames;
  /*!
   * \~english Data input names (excluding clk/rst/enable)
   * \~russian Имена входов данных (без clk/rst/enable)
   */
  std::vector<std::string> d_dataInputNames;
  /*!
   * \~english Latch enable signal names (asserted while applying data)
   * \~russian Имена сигналов разрешения защелки (поднимаются при подаче данных)
   */
  std::vector<std::string> d_enableNames;
};

} // namespace CG_Graph
