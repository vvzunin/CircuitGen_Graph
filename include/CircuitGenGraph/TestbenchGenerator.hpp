#pragma once

#include <cstdint>
#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <vector>

#include <CircuitGenGraph/OrientedGraph.hpp>

/// @file TestbenchGenerator.hpp
/// @brief Система генерации тестбенчей и верификации для сгенерированных схем

namespace CG_Graph {

/// @brief Результат симуляции одного тестового вектора
struct TestVector {
  std::vector<char> inputs; ///< Входные значения
  std::vector<char> expected; ///< Ожидаемые выходные значения
  std::vector<char> actual; ///< Фактические выходные значения (после симуляции)
  bool passed = false;      ///< Прошёл ли тест
};

/// @brief Результат верификации схемы
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

/// @brief Конфигурация генератора тестбенчей
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

/// @brief Стратегия генерации тестовых векторов
enum class TestVectorStrategy {
  Exhaustive, ///< Полный перебор всех комбинаций (для малого числа входов)
  Random,     ///< Случайные тестовые векторы
  Custom ///< Пользовательские тестовые векторы
};

/// @brief Класс для генерации Verilog тестбенчей
///
/// Позволяет автоматически генерировать тестбенчи для верификации
/// сгенерированных комбинационных и последовательностных схем.
///
/// @code
/// // Создание графа схемы
/// auto graph = std::make_shared<OrientedGraph>("my_circuit");
/// // ... добавление элементов схемы ...
///
/// // Генерация тестбенча
/// TestbenchGenerator tbGen(graph);
/// tbGen.generateExhaustiveVectors();
/// tbGen.toVerilogTestbench("./", "my_circuit_tb");
///
/// // Запуск верификации с Icarus Verilog
/// auto result = tbGen.runIcarusVerification("./");
/// if (result.success) {
///     std::cout << "Verification passed!" << std::endl;
/// }
/// @endcode
class TestbenchGenerator {
public:
  /// @brief Конструктор
  /// @param i_graph Указатель на граф схемы
  /// @param i_config Конфигурация генератора (опционально)
  explicit TestbenchGenerator(
      GraphPtr i_graph, const TestbenchConfig &i_config = TestbenchConfig());

  /// @brief Деструктор
  ~TestbenchGenerator() = default;

  // Запрет копирования
  TestbenchGenerator(const TestbenchGenerator &) = delete;
  TestbenchGenerator &operator=(const TestbenchGenerator &) = delete;

  // Разрешение перемещения
  TestbenchGenerator(TestbenchGenerator &&) = default;
  TestbenchGenerator &operator=(TestbenchGenerator &&) = default;

  // ==================== Генерация тестовых векторов ====================

  /// @brief Генерирует все возможные комбинации входов (полный перебор)
  /// @warning Для N входов генерируется 2^N тестов. Не рекомендуется для N > 20
  /// @return Количество сгенерированных тестовых векторов
  size_t generateExhaustiveVectors();

  /// @brief Генерирует случайные тестовые векторы
  /// @param i_count Количество тестовых векторов
  /// @param i_seed Seed для генератора случайных чисел (0 = случайный)
  /// @return Количество сгенерированных тестовых векторов
  size_t generateRandomVectors(size_t i_count, uint32_t i_seed = 0);

  /// @brief Добавляет пользовательский тестовый вектор
  /// @param i_inputs Входные значения
  /// @param i_expected Ожидаемые выходные значения (опционально, будут
  /// вычислены)
  void addTestVector(const std::vector<char> &i_inputs,
                     const std::vector<char> &i_expected = {});

  /// @brief Очищает все тестовые векторы
  void clearTestVectors();

  /// @brief Возвращает количество тестовых векторов
  size_t getTestVectorCount() const { return d_testVectors.size(); }

  /// @brief Возвращает тестовые векторы
  const std::vector<TestVector> &getTestVectors() const {
    return d_testVectors;
  }

  // ==================== Генерация тестбенча ====================

  /// @brief Генерирует Verilog тестбенч
  /// @param i_path Путь для сохранения файла
  /// @param i_filename Имя файла (без расширения)
  /// @return true если успешно
  bool toVerilogTestbench(const std::string &i_path,
                          const std::string &i_filename = "");

  /// @brief Возвращает содержимое тестбенча как строку
  /// @return Verilog код тестбенча
  std::string getTestbenchCode() const;

  // ==================== Симуляция и верификация ====================

  /// @brief Выполняет симуляцию на встроенном симуляторе (graphSimulation)
  /// @return Результат верификации
  VerificationResult runInternalSimulation();

  /// @brief Выполняет верификацию с использованием Icarus Verilog
  /// @param i_workDir Рабочая директория
  /// @param i_icarusPath Путь к iverilog (по умолчанию ищется в PATH)
  /// @param i_vvpPath Путь к vvp (по умолчанию ищется в PATH)
  /// @return Результат верификации
  VerificationResult
  runIcarusVerification(const std::string &i_workDir,
                        const std::string &i_icarusPath = "iverilog",
                        const std::string &i_vvpPath = "vvp");

  /// @brief Сравнивает результаты внутренней симуляции и Icarus Verilog
  /// @param i_workDir Рабочая директория
  /// @return Результат сравнения
  VerificationResult compareSimulations(const std::string &i_workDir);

  // ==================== Конфигурация ====================

  /// @brief Устанавливает конфигурацию
  /// @param i_config Новая конфигурация
  void setConfig(const TestbenchConfig &i_config) { d_config = i_config; }

  /// @brief Возвращает текущую конфигурацию
  /// @return Конфигурация
  const TestbenchConfig &getConfig() const { return d_config; }

  // ==================== Утилиты ====================

  /// @brief Проверяет, установлен ли Icarus Verilog
  /// @param i_icarusPath Путь к iverilog
  /// @return true если Icarus Verilog доступен
  static bool isIcarusAvailable(const std::string &i_icarusPath = "iverilog");

  /// @brief Конвертирует вектор char в строку для Verilog
  /// @param i_values Вектор значений
  /// @return Строка в формате Verilog (например, "4'b0101")
  static std::string toVerilogLiteral(const std::vector<char> &i_values);

  /// @brief Парсит вывод Icarus Verilog для извлечения результатов
  /// @param i_output Вывод симулятора
  /// @return Вектор результатов
  std::vector<std::vector<char>> parseIcarusOutput(const std::string &i_output);

private:
  /// @brief Генерирует заголовок тестбенча
  std::string generateHeader() const;

  /// @brief Генерирует объявления сигналов
  std::string generateSignalDeclarations() const;

  /// @brief Генерирует инстанцирование DUT (Device Under Test)
  std::string generateDUTInstantiation() const;

  /// @brief Генерирует блок stimulus
  std::string generateStimulusBlock() const;

  /// @brief Генерирует блок проверки результатов
  std::string generateCheckBlock() const;

  /// @brief Генерирует блок VCD dump
  std::string generateVCDDump() const;

  /// @brief Вычисляет ожидаемые выходы для вектора входов
  std::vector<char> computeExpectedOutputs(const std::vector<char> &i_inputs);

  /// @brief Выполняет команду и возвращает результат
  std::pair<int, std::string> executeCommand(const std::string &i_command);

  /// @brief Выполняет команду безопасно через fork+execvp (без shell)
  /// @param i_argv Массив аргументов команды (первый - имя программы)
  /// @return Пара (код возврата, вывод команды)
  std::pair<int, std::string>
  executeCommandSafe(const std::vector<std::string> &i_argv);

  GraphPtr d_graph;         ///< Указатель на граф схемы
  TestbenchConfig d_config; ///< Конфигурация генератора
  std::vector<TestVector> d_testVectors;  ///< Тестовые векторы
  std::vector<std::string> d_inputNames;  ///< Имена входов
  std::vector<std::string> d_outputNames; ///< Имена выходов
  bool d_hasSequential = false; ///< Есть ли последовательностные элементы
};

} // namespace CG_Graph
