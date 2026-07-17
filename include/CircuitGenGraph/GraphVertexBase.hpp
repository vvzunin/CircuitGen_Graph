/**
 * @file GraphVertexBase.hpp
 * @author Vladimir Zunin
 * @author Fuuulkrum7
 * @author rainbowkittensss
 * @author Theossr
 * @author NonDif
 *
 * \~english
 * @brief Base class for all graph vertices with common functionality
 * \ingroup GraphCore
 *
 * Defines the abstract base class GraphVertexBase which provides:
 * - Connection management (input/output edges)
 * - Level calculation for topological ordering
 * - Hash computation for graph comparison
 * - Verilog/DOT/GraphML export capabilities
 * - Memory management integration
 *
 * \~russian
 * @brief Базовый класс для всех вершин графа с общей функциональностью
 * \ingroup GraphCore
 *
 * Определяет абстрактный базовый класс GraphVertexBase, который
 * предоставляет:
 * - Управление соединениями (входящие/исходящие ребра)
 * - Вычисление уровня для топологической сортировки
 * - Вычисление хэша для сравнения графов
 * - Возможности экспорта в форматы Verilog/DOT/GraphML
 * - Интеграцию управления памятью
 */
#pragma once
#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <CircuitGenGraph/enums.hpp>

#include <CircuitGenGraph/GraphMemory.hpp>
#include <CircuitGenGraph/GraphUtils.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

#define GraphPtr std::shared_ptr<CG_Graph::OrientedGraph>
#define GraphPtrWeak std::weak_ptr<CG_Graph::OrientedGraph>

#define VertexPtr CG_Graph::GraphVertexBase *

namespace CG_Graph {

class OrientedGraph;
class GraphVertexSequential;
class GraphVertexBus;
/**
 * @namespace VertexUtils
 * @author Vladimir Zunin
 * @author Fuuulkrum7
 * @author Theossr
 * @author rainbowkittensss
 *
 * \~english
 * @brief Namespace containing utility functions for working with vertices
 *
 * \~russian
 * @brief Пространство имен, содержащее вспомогательные функции
 * для работы с вершинами
 */
namespace VertexUtils {

/**
 * @author Fuuulkrum7
 *
 * \~english
 * @brief gateToString Converts a gate type enum value to its string
 * representation
 * @param i_type The gate type enum value
 * @return The string representation of the gate type.
 * @par Example
 * @code
 * Gates gateType = Gates::GateAnd;
 * std::string gateString = VertexUtils::gateToString(gateType);
 * std::cout << "String representation of gate type: " << gateString
 * << std::endl;
 * gateType = Gates::GateNot;
 * gateString = VertexUtils::gateToString(gateType);
 * std::cout << "String representation of gate type: " << gateString
 * << std::endl;
 * @endcode
 *
 * \~russian
 * @brief gateToString Преобразует значение перечисления типа вентиля в его
 * строковое представление
 * @param i_type Значение перечисления типа вентиля
 * @return Строковое представление типа вентиля.
 */
std::string gateToString(Gates i_type);

/**
 * @author Fuuulkrum7
 *
 * \~english
 * @brief vertexTypeToVerilog Converts a vertex type enum value to its
 * Verilog representation
 * @param i_type The vertex type enum value.
 * @return The Verilog representation of the vertex type
 * @par Example
 * @code
 * VertexTypes vertexType = VertexTypes::Input;
 * std::string verilogType = VertexUtils::vertexTypeToVerilog(vertexType);
 * std::cout << "Verilog representation of vertex type: " << verilogType
 * << std::endl;
 * @endcode
 *
 * \~russian
 * @brief vertexTypeToVerilog Преобразует значение перечисления типа
 * вершины в его представление для Verilog
 * @param i_type Значение перечисления типа вершины.
 * @return Представление типа вершины для Verilog
 */
std::string vertexTypeToVerilog(VertexTypes i_type);

/**
 * @author Fuuulkrum7
 *
 * \~english
 * @brief vertexTypeToComment
 * Converts a vertex type enum value to its comment representation
 * This function takes a vertex type enum value (from the VertexTypes enum)
 * and returns its corresponding comment representation.
 * The comment representation is based on the vertex type and is used for
 * generating comments or documentation
 * @param i_type The vertex type enum value (from the VertexTypes enum)
 * @return The comment representation of the vertex type.
 * @par Example
 * @code
 * VertexTypes vertexType = VertexTypes::Input;
 * std::string comment = VertexUtils::vertexTypeToComment(vertexType);
 * std::cout << "Comment for input vertex type: " << comment << std::endl;
 * @endcode
 *
 * \~russian
 * @brief vertexTypeToComment
 * Преобразует значение перечисления типа вершины в его представление в
 * виде комментария. Эта функция принимает значение перечисления типа
 * вершины (из перечисления VertexTypes) и возвращает соответствующее ему
 * представление в виде комментария. Представление в виде комментария
 * основано на типе вершины и используется для генерации комментариев или
 * документации.
 * @param i_type Значение перечисления типа вершины
 * @return Представление типа вершины в виде комментария.
 */
std::string vertexTypeToComment(VertexTypes i_type);

/**
 * @author Fuuulkrum7
 *
 * \~english
 * @brief Builds a human-readable comment for a sequential vertex.
 * @param i_seq Sequential vertex to describe.
 * @return Comment text for the specified sequential vertex.
 * @par Example
 * @code
 * GraphPtr graph = std::make_shared<OrientedGraph>();
 * auto *clk = graph->addInput("clk");
 * auto *data = graph->addInput("data");
 * auto *seq = graph->addSequential(ff, clk, data, "q");
 * std::string comment = VertexUtils::getSequentialComment(seq);
 * std::cout << "Comment for input vertex type: " << comment << std::endl;
 * @endcode
 *
 * \~russian
 * @brief Формирует человекочитаемый комментарий для последовательностной
 * вершины.
 * @param i_seq Последовательностная вершина, для которой формируется описание.
 * @return Текст комментария для указанной последовательностной вершины.
 */
std::string getSequentialComment(const GraphVertexSequential *i_seq);

} // namespace VertexUtils

/**
 * @class GraphVertexBase
 * @author Vladimir Zunin
 * @author Fuuulkrum7
 * @author Theossr
 * @author rainbowkittensss
 * @author NonDif
 *
 * \~english
 * @brief Base class for all graph vertices.
 * - d_baseGraph: A weak pointer to the base graph containing this vertex
 * - d_name: The name of the vertex. It is a string containing the name of a
 * vertex
 * - d_value: The value of the vertex
 * - d_level: The vertex level is represented by the uint32_t type
 * - d_inConnections: vector of pointers to input connections with other
 * vertices
 * - d_outConnections: vector of pointers to output connections with other
 * vertices
 * - d_type: Vertex Type - Defined by the VertexTypes enumeration
 * - d_count: Vertex counter for naming and other purposes. Represented by the
 * uint_fast64_t type
 * - d_hashed: A string containing the calculated hash value for the vertex
 * - d_hasHash: A HASH_CONDITION value containing hashing status
 * - d_needUpdate: A VERTEX_STATE value containing status in the context of
 * level calculating
 *
 * \~russian
 * @brief Базовый класс для всех вершин графа.
 * - d_baseGraph: Слабый указатель (weak pointer) на базовый граф, содержащий
 * эту вершину
 * - d_name: Имя вершины. Это строка, содержащая имя вершины
 * - d_value: Значение вершины
 * - d_level: Уровень вершины, представленный типом uint32_t
 * - d_inConnections: вектор указателей на входные соединения с другими
 * вершинами
 * - d_outConnections: вектор указателей на выходные соединения с другими
 * вершинами
 * - d_type: Тип вершины - определяется перечислением VertexTypes
 * - d_count: Счетчик вершин для именования и других целей. Представлен типом
 * uint_fast64_t
 * - d_hashed: Строка, содержащая вычисленное значение хэша для вершины
 * - d_hasHash: Значение HASH_CONDITION, содержащее статус хэширования
 * - d_needUpdate: Значение VERTEX_STATE, содержащее статус в контексте
 * вычисления уровня
 */
class GraphVertexBase {
  friend class OrientedGraph;
  friend class GraphVertexBus;

public:
  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Resets the global vertex ID counter.
   *
   * \~russian
   * @brief Сбрасывает глобальный счетчик уникальных идентификаторов вершин.
   */
  static void resetCounter() { d_count = 0ul; }

private:
  // Счетчик вершин для именования и подобного
  static std::atomic_uint64_t d_count;

public:
  enum HASH_CONDITION : uint8_t {
    HC_NOT_CALC = 0,    // not calculated
    HC_IN_PROGRESS = 1, // in progress
    HC_CALC = 2         // correct value saved in d_hashed
  };

  enum VERTEX_STATE : uint8_t {
    VS_NOT_CALC = 0u,         // not calculated
    VS_IN_PROGRESS = 1u << 0, // in progress
    VS_CALC = 1u << 1,        // correct level saved in d_level
    VS_USED_LEVEL = 1u << 2,  // flag for method @see getVerticesByLevel()
                              // called with possibly wrong levels for vertices
                              // and current vertex is used in the search
    VS_USED_CALC =
        VS_CALC |
        VS_USED_LEVEL // flag for method @see getVerticesByLevel() called
                      //  in correct mode and current vertex is
                      //  used in the search
  };

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Resets all cached calculation states (hash, level flags, etc.).
   *
   * \~russian
   * @brief Сбрасывает все кэшированные состояния вычислений (хэш, флаги
   * уровня и т.д.).
   */
  void resetAllStates() {
    d_needUpdate = VS_NOT_CALC;
    d_hasHash = HC_NOT_CALC;
  }

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Clears level-update state before level recomputation.
   *
   * \~russian
   * @brief Сбрасывает состояние обновления уровней перед пересчетом.
   */
  void resetNeedUpdateState() { d_needUpdate = VS_NOT_CALC; }

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Clears hash state before hash recomputation.
   *
   * \~russian
   * @brief Сбрасывает состояние хэша перед его пересчетом.
   */
  void resetHashState() { d_hasHash = HC_NOT_CALC; }

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Clears helper flags used by level-based traversal.
   *
   * \~russian
   * @brief Сбрасывает служебные флаги, используемые при обходе по уровням.
   */
  void resetUsedLevelState() {
    // remove flag using bitwise operations
    d_needUpdate = static_cast<VERTEX_STATE>(d_needUpdate & ~VS_USED_LEVEL);
  }

  /**
   * \~english
   * @brief Constructs vertex with specified type and optional owner graph.
   * @param i_type The type of the vertex (from the VertexTypes enum).
   * @param i_graph Optional shared pointer to the graph containing the vertex.
   *
   * \~russian
   * @brief Создает вершину с указанным типом и опциональным графом-владельцем.
   * @param i_type Тип вершины (из перечисления VertexTypes).
   * @param i_graph Опциональный shared-указатель на граф, содержащий вершину.
   */
  GraphVertexBase(const VertexTypes i_type, GraphPtr i_graph);

  /**
   * \~english
   * @brief Constructs vertex with specified type, name, and optional owner
   * graph.
   * @param i_type The type of the vertex (from the VertexTypes enum)
   * @param i_name The name of the vertex.
   * @param i_graph Optional shared pointer to the graph containing the vertex.
   *
   * \~russian
   * @brief Создает вершину с указанными типом, именем и опциональным
   * графом-владельцем.
   * @param i_type Тип вершины (из перечисления VertexTypes)
   * @param i_name Имя вершины.
   * @param i_graph Опциональный shared-указатель на граф, содержащий вершину.
   */
  GraphVertexBase(const VertexTypes i_type, std::string_view i_name,
                  GraphPtr i_graph);

  /**
   * @author Fuuulkrum7
   * \~english
   * @brief Copy assignment operator
   * \~russian
   * @brief Оператор копирующего присваивания
   */
  GraphVertexBase &operator=(const GraphVertexBase &other) = default;

  /**
   * @author Fuuulkrum7
   * \~english
   * @brief Move assignment operator
   * \~russian
   * @brief Оператор перемещающего присваивания
   */
  GraphVertexBase &operator=(GraphVertexBase &&other) = default;

  /**
   * \~english
   * @brief Copy constructor.
   * @param other The other vertex
   *
   * \~russian
   * @brief Конструктор копирования.
   * @param other Другая вершина
   */
  GraphVertexBase(const GraphVertexBase &other) = default;

  /**
   * \~english
   * @brief Move constructor.
   * @param other Vertex to move from.
   *
   * \~russian
   * @brief Конструктор перемещения.
   * @param other Вершина-источник для перемещения.
   */
  GraphVertexBase(GraphVertexBase &&other) = default;

  /**
   * \~english
   * @brief Virtual destructor.
   *
   * \~russian
   * @brief Виртуальный деструктор.
   */
  virtual ~GraphVertexBase();

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Returns vertex type as `VertexTypes` value.
   * @return The type of the vertex (from the VertexTypes enum).
   * @par Example
   * @code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * VertexTypes type = vertex.getType();
   * std::cout << "Type of the vertex: " << type << std::endl;
   * @endcode
   *
   * \~russian
   * @brief Возвращает тип вершины как значение `VertexTypes`.
   * @return Тип вершины (из перечисления VertexTypes).
   */
  VertexTypes getType() const;
  /**
   * \~english
   * @brief Returns full (extended) vertex type, including bus variants.
   * @return Full vertex type.
   *
   * \~russian
   * @brief Возвращает полный (расширенный) тип вершины, включая шинные
   * варианты.
   * @return Полный тип вершины.
   */
  VertexTypes getFullType() const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Returns string representation of vertex type.
   * @return The string representation of the vertex type.
   * @par Example
   * @code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * std::string typeName = vertex.getTypeName();
   * std::cout << "String representation of the vertex type: " << typeName;
   * @endcode
   *
   * \~russian
   * @brief Возвращает строковое представление типа вершины.
   * @return Строковое представление типа вершины.
   */
  std::string getTypeName() const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Sets vertex name.
   * @param i_name The new name for the vertex.
   * @par Example
   * @code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * vertex.setName("new_vertex_name");
   * std::cout << "New name of the vertex: " << vertex.getRawName()
   * << std::endl;
   * @endcode
   *
   * \~russian
   * @brief Устанавливает имя вершины.
   * @param i_name Новое имя для вершины.
   */
  void setName(std::string_view i_name);

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Returns vertex name.
   * @return The name of the vertex.
   * @par Example
   * @code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * std::string name = vertex.getName();
   * std::cout << "Name of the vertex: " << name << std::endl;
   * @endcode
   *
   * \~russian
   * @brief Возвращает имя вершины.
   * @return Имя вершины.
   */
  std::string getName() const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief getName Returns concatenation of the name of the vertex and
   * i_prefix
   * @param i_prefix String prefix to append
   * @return The concatenation of name of the vertex and i_prefix
   * @par Example
   * @code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * std::string s = vertex.getName("some prefix");
   * std::cout << "Name of the vertex: " << s << std::endl;
   * @endcode
   *
   * \~russian
   * @brief getName Возвращает конкатенацию имени вершины и i_prefix
   * @param i_prefix Строка-префикс для добавления
   * @return Конкатенация имени вершины и i_prefix
   */
  std::string getName(const std::string &i_prefix) const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Returns vertex name as `std::string_view`.
   * @return Vertex name as `std::string_view`.
   * @par Example
   * @code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * std::string s = vertex.getName("some prefix");
   * std::cout << "Name of the vertex: " << std::string(s) << std::endl;
   * @endcode
   *
   * \~russian
   * @brief Возвращает имя вершины как `std::string_view`.
   * @return Имя вершины как `std::string_view`.
   */
  std::string_view getRawName() const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Returns current vertex value.
   * @return The value of the vertex.
   * @par Example
   * @code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * char value = vertex.getValue();
   * std::cout << "Value of the vertex: " << value << std::endl;
   * @endcode
   *
   * \~russian
   * @brief Возвращает текущее значение вершины.
   * @return Значение вершины.
   */
  char getValue() const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief updateValue A virtual function for updating the vertex value.
   * The implementation is provided in derived classes
   * @return the value of the vertex after its update
   *
   * \~russian
   * @brief updateValue Виртуальная функция для обновления значения вершины.
   * Реализация предоставляется в производных классах
   * @return значение вершины после ее обновления
   */
  virtual char updateValue() = 0;

  /**
   * @author Theossr
   * \~english
   * @brief Removes value of the vertex
   * \~russian
   * @brief Удаляет значение вершины
   */
  virtual void removeValue();

  /**
   * \~english
   * @brief Marks the vertex value as undefined without walking connections.
   * Used by `OrientedGraph::graphSimulation` before evaluating a new vector.
   * \~russian
   * @brief Помечает значение вершины как неопределённое без обхода связей.
   * Используется в `OrientedGraph::graphSimulation` перед новым вектором.
   */
  void invalidateValue();

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Returns vertex level.
   * @return The level of the vertex
   * @par Example
   * @code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * uint32_t level = vertex.getLevel();
   * std::cout << "Level of the vertex: " << level << std::endl;
   * @endcode
   *
   * \~russian
   * @brief Возвращает уровень вершины.
   * @return Уровень вершины
   */
  uint32_t getLevel() const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Recalculates vertex level from input connections.
   * The level becomes `max(input levels) + 1`.
   * Before repeated calls, reset state flags used by level traversal.
   *
   * \~russian
   * @brief Пересчитывает уровень вершины по входным соединениям.
   * Уровень становится равным `max(уровни входов) + 1`.
   * Перед повторным вызовом сбросьте служебные флаги обхода по уровням.
   */
  virtual void updateLevel();

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief getVerticesByLevel Support method for
   * OrientedGraph::getVerticesByLevel() calculating
   * @param i_targetLevel level, vertices with which should be found
   * @param i_result reference to vector in which found values are stored
   * @param i_fromOut if true than begins search from outputs, else from
   * inputs (depends on level, if target level is closer to outputs or
   * inputs)
   * @return true if vertex has required level and false if not
   *
   * \~russian
   * @brief getVerticesByLevel Вспомогательный метод для вычисления
   * OrientedGraph::getVerticesByLevel()
   * @param i_targetLevel уровень, вершины с которым должны быть найдены
   * @param i_result ссылка на вектор, в котором сохраняются найденные
   * значения
   * @param i_fromOut если true, то начинает поиск от выходов, иначе от
   * входов (зависит от уровня, если целевой уровень ближе к выходам или
   * входам)
   * @return true, если вершина имеет требуемый уровень, и false, если нет
   */
  bool getVerticesByLevel(uint32_t i_targetLevel,
                          std::vector<VertexPtr> &i_result,
                          bool i_fromOut = true);

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Returns logic gate type represented by this vertex.
   * If the vertex is not a gate, returns `Gates::GateDefault`.
   * @return The type of the basic logic gate represented by this vertex
   * @par Example
   * @code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * Gates gateType = vertex.getGate();
   * @endcode
   *
   * \~russian
   * @brief Возвращает тип логического вентиля для вершины.
   * Если вершина не является вентилем, возвращает `Gates::GateDefault`.
   * @return Тип базового логического вентиля, представленного этой вершиной
   */
  virtual Gates getGate() const { return Gates::GateDefault; }

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Returns weak pointer to owning graph.
   * @return Weak pointer to graph associated with this vertex.
   *
   * \~russian
   * @brief Возвращает слабый указатель на граф-владелец.
   * @return Слабый указатель на граф, связанный с этой вершиной.
   */
  GraphPtrWeak getBaseGraph() const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief reserveInConnections reserving memory in d_inConnections for
   * i_size other vertices
   * @param i_size Size to reserve
   * @par Example
   * @code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * vertex.reserveInConnections(5);
   * @endcode
   *
   * \~russian
   * @brief reserveInConnections резервирует память в d_inConnections для
   * i_size других вершин
   * @param i_size Размер резервируемой памяти
   */
  void reserveInConnections(size_t i_size);

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief reserveOutConnections reserving memory in d_outConnections for
   * i_size other vertices
   * @param i_size Size to reserve
   * @par Example
   * @code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * vertex.reserveOutConnections(5);
   * @endcode
   *
   * \~russian
   * @brief reserveOutConnections резервирует память в d_outConnections для
   * i_size других вершин
   * @param i_size Размер резервируемой памяти
   */
  void reserveOutConnections(size_t i_size);

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief getInConnections
   * @return Vector of pointers to this vertex's input connections.
   * @par Example
   * @code
   * GraphPtr graph = std::make_shared<OrientedGraph>();
   * VertexPtr input = graph->addInput("input");
   * VertexPtr gate = graph->addGate(Gates::GateBuf, "buf");
   * graph->addEdge(input, gate);
   *
   * const auto &inConnections = gate->getInConnections();
   * for (VertexPtr connection : inConnections) {
   *   if (connection != nullptr) {
   *     std::cout << connection->getName() << std::endl;
   *   }
   * }
   * @endcode
   *
   * \~russian
   * @brief getInConnections
   * @return Константная ссылка на вектор указателей на входные соединения.
   */
  const std::vector<VertexPtr> &getInConnections() const;

  /**
   * @author rainbowkittensss
   *
   * \~english
   * @brief addVertexToInConnections
   * Appends a driver to the input connections of this vertex.
   * @param i_vert The vertex to be added to the input connections
   * @return `1` for the connection just added (duplicates are allowed at this
   * API level; use `getInConnections()` to inspect multiplicity)
   * @par Example
   * @code
   * GraphPtr graph = std::make_shared<OrientedGraph>();
   * VertexPtr sink = graph->addGate(Gates::GateAnd, "sink");
   * VertexPtr src = graph->addInput("src");
   * uint32_t added = sink->addVertexToInConnections(src);
   * std::cout << "Added in-connections: " << added << std::endl;
   * @endcode
   *
   * \~russian
   * @brief addVertexToInConnections
   * Добавляет драйвер во входные соединения этой вершины.
   * @param i_vert Вершина, которая будет добавлена во входные соединения
   * @return `1` для только что добавленной связи (дубликаты на этом уровне API
   * допускаются; кратность смотрите через `getInConnections()`)
   */
  virtual uint32_t addVertexToInConnections(VertexPtr i_vert);

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief getOutConnections
   * @return Vector of pointers to this vertex's output connections.
   * @par Example
   * @code
   * // Creating an instance of the GraphVertexBase class
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * // Creating another vertex
   * VertexPtr anotherVertex =
   * std::make_shared<GraphVertexInput>(VertexTypes::output, "vertex2");
   * // Adding the second vertex to the output connections of the first
   * vertex.addVertexToOutConnections(anotherVertex);
   * // get the vector of the output connections of the first vertex
   * const auto &outConnections = vertex.getOutConnections();
   * // output information about the output connections
   * std::cout << "Output connections of the first vertex:" << std::endl;
   * for (const auto& connection : outConnections)
   * {
   * std::cout << " - " << connection->getName() << std::endl;
   * }
   * @endcode
   *
   * \~russian
   * @brief getOutConnections
   * @return Константная ссылка на вектор указателей на выходные соединения.
   */
  const std::vector<VertexPtr> &getOutConnections() const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief addVertexToOutConnections
   * Adds a vertex to the output connections of this vertex if it is not
   * already present
   * @param i_vert The vertex to be added to the output connections
   * @return true if the vertex was successfully added, false if it was
   * already present in the output connections.
   * @par Example
   * @code
   * @todo добавить пример использования / Add example of usage
   * @endcode
   *
   * \~russian
   * @brief addVertexToOutConnections
   * Добавляет вершину в выходные соединения этой вершины, если ее там
   * еще нет
   * @param i_vert Вершина, которая будет добавлена в выходные соединения
   * @return true, если вершина была успешно добавлена, false, если она уже
   * присутствовала в выходных соединениях.
   */
  bool addVertexToOutConnections(VertexPtr i_vert);

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief calculateHash
   * Calculates the hash value for the vertex based on its outgoing
   * connections.
   * When running for a second time, set hash flags to default state
   * @return The hash value of the vertex based on its outgoing connections.
   * @par Example
   * @code
   * GraphPtr graph = std::make_shared<OrientedGraph>();
   * VertexPtr vertex = graph->addOutput("out");
   * VertexPtr vertex2 = graph->addInput("in2");
   * VertexPtr vertex3 = graph->addInput("in3");
   * graph->addEdge(vertex, vertex2);
   * graph->addEdge(vertex, vertex3);
   * std::string hashValue = std::to_string(vertex->calculateHash());
   * // Output of the result
   * std::cout << "Hash for the first vertex: " << hashValue << std::endl;
   * @endcode
   *
   * \~russian
   * @brief calculateHash
   * Вычисляет значение хэша для вершины на основе ее исходящих
   * соединений.
   * При повторном запуске установите флаги хэша в состояние по умолчанию
   * @return Значение хэша вершины на основе ее исходящих соединений.
   */
  virtual size_t calculateHash();

  /**
   * \~english
   * @brief Returns input vertex with the smallest bus width.
   * @return Pointer to minimal-width input vertex.
   *
   * \~russian
   * @brief Возвращает входную вершину с минимальной шириной шины.
   * @return Указатель на входную вершину с минимальной шириной.
   */
  VertexPtr minWidthVertex() const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief toVerilog Generates Verilog code for the vertex
   * @return A string containing Verilog code for the vertex, or an empty
   * string if the vertex type is not "output" or if the incoming
   * connection is invalid
   * @par Example
   * @code
   * // Creating an instance of the GraphVertexBase class
   * GraphPtr graph = std::make_shared<OrientedGraph>();
   * VertexPtr outputVertex = graph->addOutput("output");
   * // Creating another vertex with the type "input" and the name
   * VertexPtr inputVertex = graph->addInput("input_vertex");
   * // Setting the input connection for the vertex "output_vertex"
   * graph->addEdge(inputVertex, outputVertex);
   * // Generating the Verilog code for the vertex "output_vertex"
   * std::string verilogCode = outputVertex->toVerilog();
   * // Display the generated Verilog code on the screen
   * std::cout << "Generated Verilog code:\n" << verilogCode << std::endl;
   * @endcode
   *
   * \~russian
   * @brief toVerilog Генерирует код Verilog для вершины
   * @return Строка, содержащая код Verilog для вершины, или пустая строка,
   * если тип вершины не "output" или если входящее соединение
   * недействительно
   */
  virtual std::string toVerilog() const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief calls toVerilog and allows to write vertex as string to a
   * stream
   * @param stream Stream to write to
   * @param matrix Vertex to write
   *
   * \~russian
   * @brief вызывает toVerilog и позволяет записать вершину в виде строки
   * в поток
   * @param stream Поток для записи
   * @param matrix Вершина для записи
   */
  friend std::ostream &operator<<(std::ostream &stream,
                                  const GraphVertexBase &matrix);

  /**
   * @author Vladimir Zunin
   *
   * \~english
   * @brief toDOT Generates DOT code for the vertex
   * @return DOT representation
   *
   * \~russian
   * @brief toDOT Генерирует код DOT для вершины
   * @return DOT представление
   */
  virtual DotReturn toDOT();

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Used for check if vertex is a subGraph vertex output
   * @return true if is vertex is subGraph vertex output or false if not
   *
   * \~russian
   * @brief Используется для проверки, является ли вершина выходом вершины
   * подграфа
   * @return true, если вершина является выходом вершины подграфа, или
   * false, если нет
   */
  virtual bool isSubgraphBuffer() const { return false; }
  virtual bool
  isBus() const; // или можно сделать метод для получения *ширины* вершины
  /**
   * \~english
   * @brief Log helper for easylogging++.
   * @param os Output stream for easylogging++.
   *
   * \~russian
   * @brief Вспомогательный метод логирования для easylogging++.
   * @param os Поток вывода для easylogging++.
   */
#ifdef LOGFLAG
  /**
   * @author Vladimir Zunin
   *
   * \~english
   * @brief log Used for easylogging++
   * @param os Stream for easylogging
   *
   * \~russian
   * @brief log Используется для easylogging++
   * @param os Поток для easylogging
   */
  virtual void log(el::base::type::ostream_t &os) const;
#endif

protected:
  /**
   * @author rainbowkittensss
   *
   * \~english
   * @brief removeVertexToOutConnections
   * Removes a vertex from the output connections of this vertex.
   * @param i_vert The vertex to be removed from the output connections.
   * @return true if the vertex was successfully removed, false otherwise.
   * @par Example
   * @code
   * // Creating an instance of the GraphVertexBase class
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * // Creating another vertex
   * VertexPtr anotherVertex =
   * std::make_shared<GraphVertexInput>(VertexTypes::output, "vertex2");
   * // Adding the second vertex to the output connections
   * vertex.addVertexToOutConnections(anotherVertex);
   * // Removing the second vertex from the output connections
   * bool removed = vertex.removeVertexToOutConnections(anotherVertex);
   * // Output of the result
   * if (removed)
   * {
   * std::cout << "Removed successfully." << std::endl;
   * }
   * else
   * {
   * std::cout << "Not found." << std::endl;
   * }
   * @endcode
   *
   * \~russian
   * @brief removeVertexToOutConnections
   * Удаляет вершину из выходных соединений этой вершины.
   * @param i_vert Вершина, которую нужно удалить из выходных соединений.
   * @return true, если вершина была успешно удалена, false в противном
   * случае.
   */
  bool removeVertexToOutConnections(VertexPtr i_vert);

  /**
   * @author rainbowkittensss
   *
   * \~english
   * @brief removeVertexToInConnections
   * Removes a vertex from the input connections of this vertex.
   * @param i_vert The vertex to be removed from the input connections.
   * @return true if the vertex was successfully removed, false otherwise.
   * @par Example
   * @code
   * // Creating an instance of the GraphVertexBase class
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * // Creating another vertex
   * VertexPtr anotherVertex =
   * std::make_shared<GraphVertexInput>(VertexTypes::input, "vertex2");
   * // Adding a second vertex to the input connections
   * vertex.addVertexToInConnections(anotherVertex);
   * // Removing the second vertex from the input connections
   * bool removed = vertex.removeVertexToInConnections(anotherVertex);
   * // Output of the result
   * if (removed)
   * {
   * std::cout << "Removed successfully" << std::endl;
   * }
   * else
   * {
   * std::cout << "Not found" << std::endl;
   * }
   * @endcode
   *
   * \~russian
   * @brief removeVertexToInConnections
   * Удаляет вершину из входных соединений этой вершины.
   * @param i_vert Вершина, которую нужно удалить из входных соединений.
   * @return true, если вершина была успешно удалена, false в противном
   * случае.
   */
  bool removeVertexToInConnections(VertexPtr i_vert);

  std::vector<VertexPtr> d_inConnections;
  std::vector<VertexPtr> d_outConnections;
  GraphPtrWeak d_baseGraph;
  std::string_view d_name;

  size_t d_hashed = 0;
  uint32_t d_level;

  char d_value = ValueStates::UndefinedState;
  VERTEX_STATE d_needUpdate = VS_NOT_CALC;
  HASH_CONDITION d_hasHash = HC_NOT_CALC;

private:
  // Определяем тип вершины: подграф, вход, выход, константа или одна из базовых
  // логических операций.
  VertexTypes d_type;
};

static_assert(sizeof(GraphVertexBase) <= 104u);

/**
 * @author Fuuulkrum7
 * \~english
 * @brief Stream output operator for string conversion.
 *
 * \~russian
 * @brief Перегрузка оператора вывода в поток для строкового представления.
 */
std::ostream &operator<<(std::ostream &stream, const GraphVertexBase &vertex);

} // namespace CG_Graph
