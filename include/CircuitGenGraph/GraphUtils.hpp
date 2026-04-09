/**
 * @file GraphUtils.hpp
 *
 * \~english
 * @brief Utility functions, enumerations and templates for graph operations
 * \ingroup GraphCore
 *
 * This header contains:
 * - Graph-related enumerations (VertexTypes, Gates, SequentialTypes, etc.)
 * - String conversion utilities
 * - Template functions for pair lookup
 * - Logic operation mappings
 *
 * \~russian
 * @brief Вспомогательные функции, перечисления и шаблоны для операций
 * с графами
 * \ingroup GraphCore
 *
 * Этот заголовочный файл содержит:
 * - Связанные с графами перечисления (VertexTypes, Gates,
 * SequentialTypes и т.д.)
 * - Утилиты преобразования строк
 * - Шаблонные функции для поиска пар
 * - Сопоставления логических операций
 * * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 * @author Theossr <feolab05@gmail.com>
 * @author Чернявских Илья Игоревич <fuuulkrum7@gmail.com>
 */
#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace CG_Graph {

#ifndef DotReturn
/*!
 * \def DotReturn
 *
 * \~english
 * @brief Type alias for DOT format return structure
 * @details Vector of pairs where each pair contains a DotType and
 * associated properties
 *
 * \~russian
 * @brief Псевдоним типа для структуры возврата формата DOT
 * @details Вектор пар, где каждая пара содержит DotType и связанные с ним
 * свойства
 */
#define DotReturn \
  std::vector<std::pair<DotTypes, std::map<std::string, std::string>>>
#endif

/**
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief Enumeration of all possible vertex types in a graph
 * @enum VertexTypes
 *
 * \~russian
 * @brief Перечисление всех возможных типов вершин в графе
 * @enum VertexTypes
 */
enum VertexTypes : uint8_t {
  /*!
   * \~english Input vertex
   * \~russian Входная вершина
   */
  input = 0,
  /*!
   * \~english Output vertex
   * \~russian Выходная вершина
   */
  output = 6,
  /*!
   * \~english Constant vertex
   * \~russian Константная вершина
   */
  constant = 1,
  /*!
   * \~english Vertex representing a logical element
   * \~russian Вершина, представляющая логический элемент
   */
  gate = 2,
  /*!
   * \~english Subgraph that makes up the vertex
   * \~russian Подграф, составляющий вершину
   */
  subGraph = 3,
  /*!
   * \~english Bus vertex type. Not supported yet
   * \~russian Тип вершины шины. Пока не поддерживается
   */
  dataBus = 4,
  /*!
   * \~english Sequential vertex type (d-latch or d-flip-flop)
   * \~russian Последовательностный тип вершины (D-защелка или D-триггер)
   */
  sequential = 5
};

/*!
 * \def CGG_FF_TYPE
 *
 * \~english
 * @brief Macro for defining flip-flop type pairs (normal and negedge
 * versions)
 * @param S Suffix for the flip-flop type name
 * @param V Base value combining control signals
 * @details Generates both standard and negedge-clocked versions of a
 * flip-flop type
 * @note CGG stands for CircuitGenGraph
 *
 * \~russian
 * @brief Макрос для определения пар типов триггеров (обычная версия и
 * версия по спаду)
 * @param S Суффикс для имени типа триггера
 * @param V Базовое значение, объединяющее управляющие сигналы
 * @details Генерирует как стандартную версию типа триггера, так и версию
 * с синхронизацией по спаду (negedge)
 * @note CGG означает CircuitGenGraph
 */
#define CGG_FF_TYPE(S, V) S = V, n##S = NEGEDGE | V

/**
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 * @author Theossr <feolab05@gmail.com>
 *
 * \~english
 * @brief Types of all sequential cells being supported
 * @enum SequentialTypes
 *
 * \~russian
 * @brief Типы всех поддерживаемых последовательностных ячеек
 * @enum SequentialTypes
 */
enum SequentialTypes : uint8_t {
  /*!
   * \~english Enable signal, writes data to output if is equal to 1'b1
   * \~russian Сигнал разрешения (Enable), записывает данные на выход,
   * если равен 1'b1
   */
  EN = 1 << 0,
  /*!
   * \~english Set signal, writes 1'b1 to output if is equal to 1'b1
   * \~russian Сигнал установки (Set), записывает 1'b1 на выход,
   * если равен 1'b1
   */
  SET = 1 << 1,
  /*!
   * \~english Clear signal, writes 1'b0 to output if is equal to 1'b1
   * \~russian Сигнал очистки (Clear), записывает 1'b0 на выход,
   * если равен 1'b1
   */
  CLR = 1 << 2,
  /*!
   * \~english Reset signal, writes 1'b0 to output if is equal to 1'b0
   * \~russian Сигнал сброса (Reset), записывает 1'b0 на выход,
   * если равен 1'b0
   */
  RST = 1 << 3,
  /*!
   * \~english Use with reset only, makes ff async (adds negedge rst to
   * signals list)
   * \~russian Использовать только со сбросом, делает триггер асинхронным
   * (добавляет negedge rst в список сигналов)
   */
  ASYNC = 1 << 5,
  /*!
   * \~english If used, activates always on negedge of clk signal (ff-only)
   * \~russian Если используется, активируется всегда по спаду (negedge)
   * сигнала clk (только для триггеров)
   */
  NEGEDGE = 1 << 6,
  /*!
   * \~english DEFAULT TYPES
   * \~russian ТИПЫ ПО УМОЛЧАНИЮ
   */
  latch = EN,
  CGG_FF_TYPE(ff, 1 << 4),
  /*!
   * \~english ASYNC FLIP-FLOPS
   * \~russian АСИНХРОННЫЕ ТРИГГЕРЫ
   */
  CGG_FF_TYPE(affr, ASYNC | ff | RST),
  CGG_FF_TYPE(affre, ASYNC | ff | EN | RST),
  CGG_FF_TYPE(affrs, ASYNC | ff | SET | RST),
  CGG_FF_TYPE(affrse, ASYNC | ff | EN | SET | RST),
  /*!
   * \~english LATCHES
   * \~russian ЗАЩЕЛКИ
   */
  latchr = latch | RST,
  latchc = latch | CLR,
  latchs = latch | SET,
  /*!
   * \~english COMBINED LATCHES
   * \~russian КОМБИНИРОВАННЫЕ ЗАЩЕЛКИ
   */
  latchrs = latch | RST | SET,
  latchcs = latch | CLR | SET,
  /*!
   * \~english FLIP-FLOPS
   * \~russian ТРИГГЕРЫ
   */
  CGG_FF_TYPE(ffe, ff | EN),
  CGG_FF_TYPE(ffr, ff | RST),
  CGG_FF_TYPE(ffc, ff | CLR),
  CGG_FF_TYPE(ffs, ff | SET),
  /*!
   * \~english COMBINED FLIP-FLOPS
   * \~russian КОМБИНИРОВАННЫЕ ТРИГГЕРЫ
   */
  CGG_FF_TYPE(ffre, ff | EN | RST),
  CGG_FF_TYPE(ffce, ff | EN | CLR),
  CGG_FF_TYPE(ffse, ff | EN | SET),
  CGG_FF_TYPE(ffrs, ff | RST | SET),
  CGG_FF_TYPE(ffcs, ff | CLR | SET),
  CGG_FF_TYPE(ffrse, ff | EN | RST | SET),
  CGG_FF_TYPE(ffcse, ff | EN | CLR | SET)
};

/**
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief Enumeration of logic gate types
 * @enum Gates
 * @details Defines the various types of logic elements that can be used in
 * digital circuits.
 *
 * \~russian
 * @brief Перечисление типов логических вентилей
 * @details Определяет различные типы логических элементов, которые могут
 * использоваться в цифровых схемах.
 */
enum Gates : uint8_t {
  /*!
   * \~english Logical element - AND
   * \~russian Логический элемент - И (AND)
   */
  GateAnd,
  /*!
   * \~english Logical element - "AND-NOT" (NAND)
   * \~russian Логический элемент - "И-НЕ" (NAND)
   */
  GateNand,
  /*!
   * \~english Logical element - OR
   * \~russian Логический элемент - ИЛИ (OR)
   */
  GateOr,
  /*!
   * \~english Logical element - "OR-NOT" (NOR)
   * \~russian Логический элемент - "ИЛИ-НЕ" (NOR)
   */
  GateNor,
  /*!
   * \~english Logical element - XOR (Exclusive OR)
   * \~russian Логический элемент - ИСКЛЮЧАЮЩЕЕ ИЛИ (XOR)
   */
  GateXor,
  /*!
   * \~english Logical element - XNOR
   * \~russian Логический элемент - ИСКЛЮЧАЮЩЕЕ ИЛИ-НЕ (XNOR)
   */
  GateXnor,
  /*!
   * \~english Logical element - NOT
   * \~russian Логический элемент - НЕ (NOT)
   */
  GateNot,
  /*!
   * \~english Logical element - Buffer
   * \~russian Логический элемент - Буфер (BUF)
   */
  GateBuf,
  /*!
   * \~english Default logical element (error state)
   * \~russian Логический элемент по умолчанию (состояние ошибки)
   */
  GateDefault
};

/**
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief Enumeration for DOT graph generation components
 * @enum DotTypes
 *
 * \~russian
 * @brief Перечисление компонентов для генерации графа в формате DOT
 */
enum DotTypes : uint8_t {
  /*!
   * \~english DOT type representing graph
   * \~russian DOT-тип, представляющий граф
   */
  DotGraph = 0,
  /*!
   * \~english DOT type representing input of a graph
   * \~russian DOT-тип, представляющий вход графа
   */
  DotInput = 1,
  /*!
   * \~english DOT type representing constant value
   * \~russian DOT-тип, представляющий константное значение
   */
  DotConstant = 2,
  /*!
   * \~english DOT type representing output of a graph
   * \~russian DOT-тип, представляющий выход графа
   */
  DotOutput = 3,
  /*!
   * \~english DOT type representing logic gate in DOT format
   * \~russian DOT-тип, представляющий логический вентиль в формате DOT
   */
  DotGate = 4,
  /*!
   * \~english DOT type representing an edge between vertices
   * \~russian DOT-тип, представляющий ребро между вершинами
   */
  DotEdge = 5,
  /*!
   * \~english DOT type representing subgraph inside a graph
   * \~russian DOT-тип, представляющий подграф внутри графа
   */
  DotSubGraph = 6,
  /*!
   * \~english DOT type representing end of graph
   * \~russian DOT-тип, представляющий конец графа
   */
  DotExit = 7
};

/**
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 * @author Theossr <feolab05@gmail.com>
 *
 * \~english
 * @brief Enumeration of possible signal values in circuit simulation
 * @enum ValueStates
 *
 * \~russian
 * @brief Перечисление возможных значений сигнала при симуляции схемы
 */
enum ValueStates : char {
  /*!
   * \~english Undefined/unknown state
   * \~russian Неопределенное/неизвестное состояние
   */
  UndefinedState = 'n',
  /*!
   * \~english Logical true (high)
   * \~russian Логическая истина (высокий уровень)
   */
  TrueValue = '1',
  /*!
   * \~english Logical false (low)
   * \~russian Логическая ложь (низкий уровень)
   */
  FalseValue = '0',
  /*!
   * \~english High impedance (tri-state)
   * \~russian Высокое сопротивление (Z-состояние)
   */
  HighImpedance = 'z',
  /*!
   * \~english No signal/invalid
   * \~russian Нет сигнала/недействительно (X-состояние)
   */
  NoSignal = 'x'
};

/*!
 * \namespace GraphUtils
 * \~english
 * @brief Utility functions and data for graph operations
 *
 * Contains static data and helper functions for:
 * - Converting between string and enum representations
 * - Logic operation mappings
 * - Graph hierarchy utilities
 *
 * \~russian
 * @brief Вспомогательные функции и данные для операций с графами
 *
 * Содержит статические данные и вспомогательные функции для:
 * - Преобразования между строковым и перечислимым представлениями
 * - Сопоставлений логических операций
 * - Утилит иерархии графа
 */
namespace GraphUtils {

/**
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief Gets information about a logical operation by its name
 * @param i_op A string containing the name of the logical operation
 * @return std::pair<std::string_view, int32_t> A pair containing the name
 * and ID of the logical operation
 * @par Example
 * @code
 * std::pair<std::string_view, int32_t> operationInfo =
 * GraphUtils::getLogicOperation("and");
 * // Output information about the logical operation
 * std::cout << "Operation name: " << operationInfo.first << std::endl;
 * std::cout << "Operation ID: " << operationInfo.second << std::endl;
 * @endcode
 * @throws std::runtime_error if operation name is not found
 *
 * \~russian
 * @brief Получает информацию о логической операции по ее имени
 * @param i_op Строка, содержащая имя логической операции
 * @return std::pair<std::string_view, int32_t> Пара, содержащая имя и ID
 * логической операции
 * @throws std::runtime_error, если имя операции не найдено
 */
std::pair<std::string_view, int32_t> getLogicOperation(const std::string &i_op);

/**
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief Returns the keys of logical operations
 * @return std::vector<Gates> A vector containing the keys of logical
 * operations
 *
 * \~russian
 * @brief Возвращает ключи логических операций
 * @return std::vector<Gates> Вектор, содержащий ключи логических операций
 */
std::vector<Gates> getLogicOperationsKeys();

/**
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief Returns logical operations along with information about single
 * input gates
 * @return std::pair<std::vector<bool>, std::vector<Gates>> A pair of
 * vectors: information about single-input gates and the keys of logical
 * operations
 * @par Example
 * @code
 * auto logicOperationsInfo = GraphUtils::getLogicOperationsWithGates();
 * for (size_t i = 0; i < logicOperationsInfo.second.size(); ++i) {
 * std::string operationName =
 * GraphUtils::parseGateToString(logicOperationsInfo.second[i]);
 * bool hasOneInput = logicOperationsInfo.first[i];
 * std::cout << "Operation: " << operationName;
 * if (hasOneInput) {
 * std::cout << " (Has one input)";
 * } else {
 * std::cout << " (Does not have one input)";
 * }
 * std::cout << std::endl;
 * }
 * @endcode
 *
 * \~russian
 * @brief Возвращает логические операции вместе с информацией о вентилях с
 * одним входом
 * @return std::pair<std::vector<bool>, std::vector<Gates>> Пара векторов:
 * информация о вентилях с одним входом и ключи логических операций
 */
std::pair<std::vector<bool>, std::vector<Gates>> getLogicOperationsWithGates();

/**
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief Converts operation string to its formal name
 * @param i_op a string representing the operation
 * @return std::string Operation name
 * @par Example
 * @code
 * // Convert the operation to its name
 * std::string operationName = GraphUtils::fromOperationsToName("and");
 * std::cout << "Operation name: " << operationName << std::endl;
 * @endcode
 *
 * \~russian
 * @brief Преобразует строку операции в ее формальное имя
 * @param i_op строка, представляющая операцию
 * @return std::string Имя операции
 */
std::string fromOperationsToName(std::string_view i_op);

/**
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief Converts hierarchy key to its corresponding operation value
 * @param key Required hierarchy key
 * @return std::string_view The value representing the operation
 * @throws std::out_of_range if the provided key does not exist
 * @par Example
 * @code
 * // Get the hierarchy associated with the operation key 5
 * std::string_view element = GraphUtils::fromHierarchyToOperation(5);
 * // Output the element
 * std::cout << element << " ";
 * @endcode
 *
 * \~russian
 * @brief Преобразует ключ иерархии в соответствующее значение операции
 * @param key Требуемый ключ иерархии
 * @return std::string_view Значение, представляющее операцию
 * @throws std::out_of_range, если предоставленный ключ не существует
 */
std::string_view fromHierarchyToOperation(int32_t key);

/**
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief Converts a string representation of a gate to its corresponding
 * enum value
 * @param i_gate The string representation of the gate
 * @return Gates The enum value corresponding to the provided string
 * representation
 * @par Example
 * @code
 * // Convert the string representation "and" to its corresponding enum
 * Gates gate = GraphUtils::parseStringToGate("and");
 * std::cout << "Enum value of 'and': " << gate << std::endl;
 * @endcode
 *
 * \~russian
 * @brief Преобразует строковое представление вентиля в соответствующее
 * значение перечисления
 * @param i_gate Строковое представление вентиля
 * @return Gates Значение перечисления, соответствующее предоставленному
 * строковому представлению
 */
Gates parseStringToGate(std::string i_gate);

/**
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief Converts an enum value of a vertex type to its string
 * representation
 * @param vertex The enum value representing the vertex type
 * @return std::string The string representation of the vertex type
 * @par Example
 * @code
 * // Convert the enum value VertexTypes::input to its string
 * std::string vertexString =
 * GraphUtils::parseVertexToString(VertexTypes::input);
 * std::cout << "String representation of VertexTypes::input: "
 * << vertexString << std::endl;
 * @endcode
 *
 * \~russian
 * @brief Преобразует значение перечисления типа вершины в его строковое
 * представление
 * @param vertex Значение перечисления, представляющее тип вершины
 * @return std::string Строковое представление типа вершины
 */
std::string parseVertexToString(VertexTypes vertex);

/**
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief Converts an enum value of a gate to its string representation
 * @param gate The enum value representing the gate
 * @return std::string The string representation of the gate
 * @par Example
 * @code
 * // Convert the enum value Gates::GateAnd to its string
 * std::string gateString =
 * GraphUtils::parseGateToString(Gates::GateAnd);
 * std::cout << "String representation of Gates::GateAnd: "
 * << gateString << std::endl;
 * @endcode
 *
 * \~russian
 * @brief Преобразует значение перечисления вентиля в его строковое
 * представление
 * @param gate Значение перечисления, представляющее вентиль
 * @return std::string Строковое представление вентиля
 */
std::string parseGateToString(Gates gate);

/**
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief Looks for a std::pair with a given key in an array of pairs
 * @tparam T Key type in pairs array
 * @tparam M Value type in pairs array
 * @tparam N Number of elements in the array
 * @param iterable Array of pairs to search in
 * @param key The key to search for (pair.first)
 * @return Pointer to found pair or nullptr if not found
 *
 * \~russian
 * @brief Ищет std::pair с заданным ключом в массиве пар
 * @tparam T Тип ключа в массиве пар
 * @tparam M Тип значения в массиве пар
 * @tparam N Количество элементов в массиве
 * @param iterable Массив пар для поиска
 * @param key Ключ для поиска (pair.first)
 * @return Указатель на найденную пару или nullptr, если не найдено
 */
template<typename T, typename M, size_t N>
static std::pair<T, M> *findPairByKey(std::pair<T, M> (&iterable)[N],
                                      const T &key) {
  auto *iter = std::find_if(std::begin(iterable), std::end(iterable),
                            [key](const auto &x) { return x.first == key; });
  return iter;
}

/*!
 * @var d_hierarchySize
 * \~english
 * @brief Size of the logic operations hierarchy (max value + 1 from
 * d_logicOperations)
 *
 * \~russian
 * @brief Размер иерархии логических операций (максимальное значение + 1
 * из d_logicOperations)
 */
static constexpr size_t d_hierarchySize = 11;

/*!
 * @var d_logicOperations
 * \~english
 * @brief Static array mapping operation names to their string views and IDs
 *
 * \~russian
 * @brief Статический массив, сопоставляющий имена операций с их строковыми
 * представлениями и ID
 */
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

/*!
 * @var stringToGate
 * \~english
 * @brief Array for converting strings to Gates enum values
 *
 * \~russian
 * @brief Массив для преобразования строк в значения перечисления Gates
 */
static std::pair<std::string, Gates> stringToGate[] = {
    {"and", Gates::GateAnd}, {"nand", Gates::GateNand}, {"or", Gates::GateOr},
    {"nor", Gates::GateNor}, {"not", Gates::GateNot},   {"buf", Gates::GateBuf},
    {"xor", Gates::GateXor}, {"xnor", Gates::GateXnor}};

/*!
 * @var vertexToString
 * \~english
 * @brief Array for converting VertexTypes enum to string representations
 *
 * \~russian
 * @brief Массив для преобразования перечисления VertexTypes в строковые 
 * представления
 */
static std::pair<VertexTypes, std::string_view> vertexToString[] = {
    {VertexTypes::input, "input"},
    {VertexTypes::output, "output"},
    {VertexTypes::constant, "const"},
    {VertexTypes::subGraph, "subGraph"},
    {VertexTypes::sequential, "sequential"},
    {VertexTypes::gate, "gate"}};

/*!
 * @var gateToString
 * \~english
 * @brief Array for converting Gates enum to string representations
 * @todo Optimize gateToString lookup by using Gates enum as direct array index
 * @note Current implementation requires linear search via findPairByKey
 *
 * \~russian
 * @brief Массив для преобразования перечисления Gates в строковые представления
 * @todo Оптимизировать поиск gateToString, используя перечисление Gates в 
 * качестве прямого индекса массива
 * @note Текущая реализация требует линейного поиска через findPairByKey
 */
static std::pair<Gates, std::string_view> gateToString[] = {
    {Gates::GateAnd, "and"},      {Gates::GateNand, "nand"},
    {Gates::GateOr, "or"},        {Gates::GateNor, "nor"},
    {Gates::GateXor, "xor"},      {Gates::GateXnor, "xnor"},
    {Gates::GateNot, "not"},      {Gates::GateBuf, "buf"},
    {Gates::GateDefault, "ERROR"}};

} // namespace GraphUtils
} // namespace CG_Graph