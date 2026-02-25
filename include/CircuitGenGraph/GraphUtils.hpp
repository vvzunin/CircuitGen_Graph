#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

/// @file GraphUtils.hpp
/// @brief \~english Utility functions, enumerations and templates for graph operations \~russian Вспомогательные функции, перечисления и шаблоны для операций с графами
/// @ingroup GraphCore
///
/// \~english
/// This header contains:
/// - Graph-related enumerations (VertexTypes, Gates, SequentialTypes, etc.)
/// - String conversion utilities
/// - Template functions for pair lookup
/// - Logic operation mappings
/// \~russian
/// Этот заголовочный файл содержит:
/// - Связанные с графами перечисления (VertexTypes, Gates, SequentialTypes и т.д.)
/// - Утилиты преобразования строк
/// - Шаблонные функции для поиска пар
/// - Сопоставления логических операций
/// \~

namespace CG_Graph {

#ifndef DotReturn
/// @def DotReturn
/// @brief \~english Type alias for DOT format return structure \~russian Псевдоним типа для структуры возврата формата DOT
/// @details \~english Vector of pairs where each pair contains a DotType and associated properties \~russian Вектор пар, где каждая пара содержит DotType и связанные с ним свойства
#define DotReturn \
  std::vector<std::pair<DotTypes, std::map<std::string, std::string>>>
#endif

/// @brief \~english Enumeration of all possible vertex types in a graph \~russian Перечисление всех возможных типов вершин в графе
/// @enum VertexTypes
enum VertexTypes : uint8_t {
  input = 0,     ///< \~english Input vertex \~russian Входная вершина
  output = 6,    ///< \~english Output vertex \~russian Выходная вершина
  constant = 1,  ///< \~english Constant vertex \~russian Константная вершина
  gate = 2,      ///< \~english Vertex representing a logical element \~russian Вершина, представляющая логический элемент
  subGraph = 3,  ///< \~english Subgraph that makes up the vertex \~russian Подграф, составляющий вершину
  dataBus = 4,   ///< \~english Bus vertex type. Not supported yet \~russian Тип вершины шины. Пока не поддерживается
  sequential = 5 ///< \~english Sequential vertex type (d-latch or d-flip-flop) \~russian Последовательностный тип вершины (D-защелка или D-триггер)
};

/// @def CGG_FF_TYPE
/// @brief \~english Macro for defining flip-flop type pairs (normal and negedge versions) \~russian Макрос для определения пар типов триггеров (обычная версия и версия по спаду)
/// @param S \~english Suffix for the flip-flop type name \~russian Суффикс для имени типа триггера
/// @param V \~english Base value combining control signals \~russian Базовое значение, объединяющее управляющие сигналы
/// @details \~english Generates both standard and negedge-clocked versions of a flip-flop type \~russian Генерирует как стандартную версию типа триггера, так и версию с синхронизацией по спаду (negedge)
/// @note \~english CGG stands for CircuitGenGraph \~russian CGG означает CircuitGenGraph
#define CGG_FF_TYPE(S, V) S = V, n##S = NEGEDGE | V

/// @brief \~english Types of all sequential cells being supported \~russian Типы всех поддерживаемых последовательностных ячеек
/// @enum SequentialTypes
enum SequentialTypes : uint8_t {
  /// \~english Enable signal, writes data to output if is equal to 1'b1 \~russian Сигнал разрешения (Enable), записывает данные на выход, если равен 1'b1
  EN = 1 << 0,
  /// \~english Set signal, writes 1'b1 to output if is equal to 1'b1 \~russian Сигнал установки (Set), записывает 1'b1 на выход, если равен 1'b1
  SET = 1 << 1,
  /// \~english Clear signal, writes 1'b0 to output if is equal to 1'b1 \~russian Сигнал очистки (Clear), записывает 1'b0 на выход, если равен 1'b1
  CLR = 1 << 2,
  /// \~english Reset signal, writes 1'b0 to output if is equal to 1'b0 \~russian Сигнал сброса (Reset), записывает 1'b0 на выход, если равен 1'b0
  RST = 1 << 3,
  /// \~english Use with reset only, makes ff async (adds negedge rst to signals list) \~russian Использовать только со сбросом, делает триггер асинхронным (добавляет negedge rst в список сигналов)
  ASYNC = 1 << 5,
  /// \~english If used, activates always on negedge of clk signal (ff-only) \~russian Если используется, активируется всегда по спаду (negedge) сигнала clk (только для триггеров)
  NEGEDGE = 1 << 6,

  /// \~english DEFAULT TYPES \~russian ТИПЫ ПО УМОЛЧАНИЮ
  latch = EN,
  CGG_FF_TYPE(ff, 1 << 4),

  /// \~english ASYNC FLIP-FLOPS \~russian АСИНХРОННЫЕ ТРИГГЕРЫ
  CGG_FF_TYPE(affr, ASYNC | ff | RST),
  CGG_FF_TYPE(affre, ASYNC | ff | EN | RST),
  CGG_FF_TYPE(affrs, ASYNC | ff | SET | RST),
  CGG_FF_TYPE(affrse, ASYNC | ff | EN | SET | RST),

  /// \~english LATCHES \~russian ЗАЩЕЛКИ
  latchr = latch | RST,
  latchc = latch | CLR,
  latchs = latch | SET,
  /// \~english COMBINED LATCHES \~russian КОМБИНИРОВАННЫЕ ЗАЩЕЛКИ
  latchrs = latch | RST | SET,
  latchcs = latch | CLR | SET,

  /// \~english FLIP-FLOPS \~russian ТРИГГЕРЫ
  CGG_FF_TYPE(ffe, ff | EN),
  CGG_FF_TYPE(ffr, ff | RST),
  CGG_FF_TYPE(ffc, ff | CLR),
  CGG_FF_TYPE(ffs, ff | SET),
  /// \~english COMBINED FLIP-FLOPS \~russian КОМБИНИРОВАННЫЕ ТРИГГЕРЫ
  CGG_FF_TYPE(ffre, ff | EN | RST),
  CGG_FF_TYPE(ffce, ff | EN | CLR),
  CGG_FF_TYPE(ffse, ff | EN | SET),

  CGG_FF_TYPE(ffrs, ff | RST | SET),
  CGG_FF_TYPE(ffcs, ff | CLR | SET),

  CGG_FF_TYPE(ffrse, ff | EN | RST | SET),
  CGG_FF_TYPE(ffcse, ff | EN | CLR | SET)
};

/// @brief \~english Enumeration of logic gate types \~russian Перечисление типов логических вентилей
/// @enum Gates
/// @details \~english Defines the various types of logic elements that can be used in digital circuits. \~russian Определяет различные типы логических элементов, которые могут использоваться в цифровых схемах.
enum Gates : uint8_t {
  GateAnd,    ///< \~english Logical element - AND \~russian Логический элемент - И (AND)
  GateNand,   ///< \~english Logical element - "AND-NOT" (NAND) \~russian Логический элемент - "И-НЕ" (NAND)
  GateOr,     ///< \~english Logical element - OR \~russian Логический элемент - ИЛИ (OR)
  GateNor,    ///< \~english Logical element - "OR-NOT" (NOR) \~russian Логический элемент - "ИЛИ-НЕ" (NOR)
  GateXor,    ///< \~english Logical element - XOR (Exclusive OR) \~russian Логический элемент - ИСКЛЮЧАЮЩЕЕ ИЛИ (XOR)
  GateXnor,   ///< \~english Logical element - XNOR \~russian Логический элемент - ИСКЛЮЧАЮЩЕЕ ИЛИ-НЕ (XNOR)
  GateNot,    ///< \~english Logical element - NOT \~russian Логический элемент - НЕ (NOT)
  GateBuf,    ///< \~english Logical element - Buffer \~russian Логический элемент - Буфер (BUF)
  GateDefault ///< \~english Default logical element (error state) \~russian Логический элемент по умолчанию (состояние ошибки)
};

/// @brief \~english Enumeration for DOT graph generation components \~russian Перечисление компонентов для генерации графа в формате DOT
/// @enum DotTypes
enum DotTypes : uint8_t {
  DotGraph = 0,    ///< \~english DOT type representing graph \~russian DOT-тип, представляющий граф
  DotInput = 1,    ///< \~english DOT type representing input of a graph \~russian DOT-тип, представляющий вход графа
  DotConstant = 2, ///< \~english DOT type representing constant value \~russian DOT-тип, представляющий константное значение
  DotOutput = 3,   ///< \~english DOT type representing output of a graph \~russian DOT-тип, представляющий выход графа
  DotGate = 4,     ///< \~english DOT type representing logic gate in DOT format \~russian DOT-тип, представляющий логический вентиль в формате DOT
  DotEdge = 5,     ///< \~english DOT type representing an edge between vertices \~russian DOT-тип, представляющий ребро между вершинами
  DotSubGraph = 6, ///< \~english DOT type representing subgraph inside a graph \~russian DOT-тип, представляющий подграф внутри графа
  DotExit = 7      ///< \~english DOT type representing end of graph \~russian DOT-тип, представляющий конец графа
};

/// @brief \~english Enumeration of possible signal values in circuit simulation \~russian Перечисление возможных значений сигнала при симуляции схемы
/// @enum ValueStates
enum ValueStates : char {
  UndefinedState = 'n', ///< \~english Undefined/unknown state \~russian Неопределенное/неизвестное состояние
  TrueValue = '1',      ///< \~english Logical true (high) \~russian Логическая истина (высокий уровень)
  FalseValue = '0',     ///< \~english Logical false (low) \~russian Логическая ложь (низкий уровень)
  HighImpedance = 'z',  ///< \~english High impedance (tri-state) \~russian Высокое сопротивление (Z-состояние)
  NoSignal = 'x'        ///< \~english No signal/invalid \~russian Нет сигнала/недействительно (X-состояние)
};

/// \namespace GraphUtils
/// \brief \~english Utility functions and data for graph operations \~russian Вспомогательные функции и данные для операций с графами
///
/// \~english
/// Contains static data and helper functions for:
/// - Converting between string and enum representations
/// - Logic operation mappings
/// - Graph hierarchy utilities
/// \~russian
/// Содержит статические данные и вспомогательные функции для:
/// - Преобразования между строковым и перечислимым представлениями
/// - Сопоставлений логических операций
/// - Утилит иерархии графа
/// \~
namespace GraphUtils {

/// @brief \~english Gets information about a logical operation by its name \~russian Получает информацию о логической операции по ее имени
/// @param i_op \~english A string containing the name of the logical operation \~russian Строка, содержащая имя логической операции
/// @return \~english std::pair<std::string_view, int32_t> A pair containing the name and ID of the logical operation \~russian std::pair<std::string_view, int32_t> Пара, содержащая имя и ID логической операции
/// @code
/// std::pair<std::string_view, int32_t> operationInfo =
/// GraphUtils::getLogicOperation("and");
/// // Output information about the logical operation
/// std::cout << "Operation name: " << operationInfo.first << std::endl;
/// std::cout << "Operation ID: " << operationInfo.second << std::endl;
/// @endcode
/// @throws \~english std::runtime_error if operation name is not found \~russian std::runtime_error, если имя операции не найдено
std::pair<std::string_view, int32_t> getLogicOperation(const std::string &i_op);

/// @brief \~english Returns the keys of logical operations \~russian Возвращает ключи логических операций
/// @return \~english std::vector<Gates> A vector containing the keys of logical operations \~russian std::vector<Gates> Вектор, содержащий ключи логических операций
std::vector<Gates> getLogicOperationsKeys();

/// @brief \~english Returns logical operations along with information about single input gates \~russian Возвращает логические операции вместе с информацией о вентилях с одним входом
/// @return \~english std::pair<std::vector<bool>, std::vector<Gates>> A pair of vectors: information about single-input gates and the keys of logical operations \~russian std::pair<std::vector<bool>, std::vector<Gates>> Пара векторов: информация о вентилях с одним входом и ключи логических операций
/// @code
/// auto logicOperationsInfo = GraphUtils::getLogicOperationsWithGates();
/// for (size_t i = 0; i < logicOperationsInfo.second.size(); ++i)
/// {
///   std::string operationName =
///     GraphUtils::parseGateToString(logicOperationsInfo.second[i]);
///   bool hasOneInput = logicOperationsInfo.first[i];
///   std::cout << "Operation: " << operationName;
///   if (hasOneInput)
///   {
///     std::cout << " (Has one input)";
///   }
///   else
///   {
///     std::cout << " (Does not have one input)";
///   }
///   std::cout << std::endl;
/// }
/// @endcode
std::pair<std::vector<bool>, std::vector<Gates>> getLogicOperationsWithGates();

/// @brief \~english Converts operation string to its formal name \~russian Преобразует строку операции в ее формальное имя
/// @param i_op \~english a string representing the operation \~russian строка, представляющая операцию
/// @return \~english std::string Operation name \~russian std::string Имя операции
/// @code
///   // Convert the operation to its name
///   std::string operationName = GraphUtils::fromOperationsToName("and");
///   std::cout << "Operation name: " << operationName << std::endl;
/// @endcode
std::string fromOperationsToName(std::string_view i_op);

/// @brief \~english Converts hierarchy key to its corresponding operation value \~russian Преобразует ключ иерархии в соответствующее значение операции
/// @param key \~english Required hierarchy key \~russian Требуемый ключ иерархии
/// @return \~english std::string_view The value representing the operation \~russian std::string_view Значение, представляющее операцию
/// @throws \~english std::out_of_range if the provided key does not exist \~russian std::out_of_range, если предоставленный ключ не существует
/// @code
///   // Get the hierarchy associated with the operation key 5
///   std::string_view element = GraphUtils::fromHierarchyToOperation(5);
///   // Output the element
///   std::cout << element << " ";
/// @endcode
std::string_view fromHierarchyToOperation(int32_t key);

/// @brief \~english Converts a string representation of a gate to its corresponding enum value \~russian Преобразует строковое представление вентиля в соответствующее значение перечисления
/// @param i_gate \~english The string representation of the gate \~russian Строковое представление вентиля
/// @return \~english Gates The enum value corresponding to the provided string representation \~russian Gates Значение перечисления, соответствующее предоставленному строковому представлению
/// @code
///   // Convert the string representation "and" to its corresponding enum value
///   Gates gate = GraphUtils::parseStringToGate("and");
///   std::cout << "Enum value of 'and': " << gate << std::endl;
/// @endcode
Gates parseStringToGate(std::string i_gate);

/// @brief \~english Converts an enum value of a vertex type to its string representation \~russian Преобразует значение перечисления типа вершины в его строковое представление
/// @param vertex \~english The enum value representing the vertex type \~russian Значение перечисления, представляющее тип вершины
/// @return \~english std::string The string representation of the vertex type \~russian std::string Строковое представление типа вершины
/// @code
///   // Convert the enum value VertexTypes::input to its corresponding string
///   representation std::string vertexString =
///   GraphUtils::parseVertexToString(VertexTypes::input); std::cout << "String
///   representation of VertexTypes::input: " << vertexString << std::endl;
/// @endcode
std::string parseVertexToString(VertexTypes vertex);

/// @brief \~english Converts an enum value of a gate to its string representation \~russian Преобразует значение перечисления вентиля в его строковое представление
/// @param gate \~english The enum value representing the gate \~russian Значение перечисления, представляющее вентиль
/// @return \~english std::string The string representation of the gate \~russian std::string Строковое представление вентиля
/// @code
///   // Convert the enum value Gates::GateAnd to its corresponding string
///   representation std::string gateString =
///   GraphUtils::parseGateToString(Gates::GateAnd); std::cout << "String
///   representation of Gates::GateAnd: " << gateString << std::endl;
/// @endcode
std::string parseGateToString(Gates gate);

/// @brief \~english Looks for a std::pair with a given key in an array of pairs \~russian Ищет std::pair с заданным ключом в массиве пар
/// @tparam T \~english Key type in pairs array \~russian Тип ключа в массиве пар
/// @tparam M \~english Value type in pairs array \~russian Тип значения в массиве пар
/// @tparam N \~english Number of elements in the array \~russian Количество элементов в массиве
/// @param iterable \~english Array of pairs to search in \~russian Массив пар для поиска
/// @param key \~english The key to search for (pair.first) \~russian Ключ для поиска (pair.first)
/// @return \~english Pointer to found pair or nullptr if not found \~russian Указатель на найденную пару или nullptr, если не найдено
template<typename T, typename M, size_t N>
static std::pair<T, M> *findPairByKey(std::pair<T, M> (&iterable)[N],
                                      const T &key) {
  auto *iter = std::find_if(std::begin(iterable), std::end(iterable),
                            [key](const auto &x) { return x.first == key; });
  return iter;
}

/// @var d_hierarchySize
/// @brief \~english Size of the logic operations hierarchy (max value + 1 from d_logicOperations) \~russian Размер иерархии логических операций (максимальное значение + 1 из d_logicOperations)
static constexpr size_t d_hierarchySize = 11;

/// @var d_logicOperations
/// @brief \~english Static array mapping operation names to their string views and IDs \~russian Статический массив, сопоставляющий имена операций с их строковыми представлениями и ID
static constexpr std::array<
    std::pair<std::string_view, std::pair<std::string_view, int32_t>>,
    d_hierarchySize>
    d_logicOperations = {{{"input", {"", 10}},
                          {"output", {"=", 0}},
                          {"const", {"1'b0", 9}},
                          {"and", {"and", 4}},
                          {"nand", {"nand", 3}},
                          {"or", {"or", 6}},
                          {"nor", {"nor", 5}},
                          {"not", {"not", 7}},
                          {"buf", {"buf", 8}},
                          {"xor", {"xor", 2}},
                          {"xnor", {"xnor", 1}}}};

/// @var stringToGate
/// @brief \~english Static array for converting strings to Gates enum values \~russian Статический массив для преобразования строк в значения перечисления Gates
static std::pair<std::string, Gates> stringToGate[] = {
    {"and", Gates::GateAnd}, {"nand", Gates::GateNand}, {"or", Gates::GateOr},
    {"nor", Gates::GateNor}, {"not", Gates::GateNot},   {"buf", Gates::GateBuf},
    {"xor", Gates::GateXor}, {"xnor", Gates::GateXnor}};

/// @var vertexToString
/// @brief \~english Static array for converting VertexTypes enum to string representations \~russian Статический массив для преобразования перечисления VertexTypes в строковые представления
static std::pair<VertexTypes, std::string_view> vertexToString[] = {
    {VertexTypes::input, "input"},
    {VertexTypes::output, "output"},
    {VertexTypes::constant, "const"},
    {VertexTypes::subGraph, "subGraph"},
    {VertexTypes::sequential, "sequential"},
    {VertexTypes::gate, "gate"}};

/// @var gateToString
/// @brief \~english Static array for converting Gates enum to string representations \~russian Статический массив для преобразования перечисления Gates в строковые представления
/// @todo \~english Optimize gateToString lookup by using Gates enum as direct array index \~russian Оптимизировать поиск gateToString, используя перечисление Gates в качестве прямого индекса массива
/// @note \~english Current implementation requires linear search via findPairByKey \~russian Текущая реализация требует линейного поиска через findPairByKey
static std::pair<Gates, std::string_view> gateToString[] = {
    {Gates::GateAnd, "and"},      {Gates::GateNand, "nand"},
    {Gates::GateOr, "or"},        {Gates::GateNor, "nor"},
    {Gates::GateXor, "xor"},      {Gates::GateXnor, "xnor"},
    {Gates::GateNot, "not"},      {Gates::GateBuf, "buf"},
    {Gates::GateDefault, "ERROR"}};

} // namespace GraphUtils

} // namespace CG_Graph