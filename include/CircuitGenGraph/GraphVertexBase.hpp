#pragma once

/*!
 * \file GraphVertexBase.hpp
 *
 * \~english
 * \brief Base class for all graph vertices with common functionality
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
 * \brief Базовый класс для всех вершин графа с общей функциональностью
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

/*!
 * \~english
 * \brief VertexUtils Namespace containing utility functions for working
 * with vertices
 *
 * \~russian
 * \brief VertexUtils Пространство имен, содержащее вспомогательные функции
 * для работы с вершинами
 */
namespace VertexUtils {

/*!
 * \~english
 * \brief gateToString Converts a gate type enum value to its string
 * representation
 * \param i_type The gate type enum value
 * \return The string representation of the gate type.
 * \code
 * Gates gateType = Gates::GateAnd;
 * std::string gateString = VertexUtils::gateToString(gateType);
 * std::cout << "String representation of gate type: " << gateString
 * << std::endl;
 * gateType = Gates::GateNot;
 * gateString = VertexUtils::gateToString(gateType);
 * std::cout << "String representation of gate type: " << gateString
 * << std::endl;
 * \endcode
 *
 * \~russian
 * \brief gateToString Преобразует значение перечисления типа вентиля в его
 * строковое представление
 * \param i_type Значение перечисления типа вентиля
 * \return Строковое представление типа вентиля.
 */
std::string gateToString(Gates i_type);

/*!
 * \~english
 * \brief vertexTypeToComment Converts a vertex type enum value to its
 * Verilog representation
 * \param i_type The vertex type enum value.
 * \return The Verilog representation of the vertex type
 * \code
 * VertexTypes vertexType = VertexTypes::Input;
 * std::string verilogType = VertexUtils::vertexTypeToVerilog(vertexType);
 * std::cout << "Verilog representation of vertex type: " << verilogType
 * << std::endl;
 * \endcode
 *
 * \~russian
 * \brief vertexTypeToComment Преобразует значение перечисления типа
 * вершины в его представление для Verilog
 * \param i_type Значение перечисления типа вершины.
 * \return Представление типа вершины для Verilog
 */
std::string vertexTypeToVerilog(VertexTypes i_type);

/*!
 * \~english
 * \brief vertexTypeToComment
 * Converts a vertex type enum value to its comment representation
 * This function takes a vertex type enum value (from the VertexTypes enum)
 * and returns its corresponding comment representation.
 * The comment representation is based on the vertex type and is used for
 * generating comments or documentation
 * \param i_type The vertex type enum value (from the VertexTypes enum)
 * \return The comment representation of the vertex type.
 * \code
 * VertexTypes vertexType = VertexTypes::Input;
 * std::string comment = VertexUtils::vertexTypeToComment(vertexType);
 * std::cout << "Comment for input vertex type: " << comment << std::endl;
 * \endcode
 *
 * \~russian
 * \brief vertexTypeToComment
 * Преобразует значение перечисления типа вершины в его представление в
 * виде комментария. Эта функция принимает значение перечисления типа
 * вершины (из перечисления VertexTypes) и возвращает соответствующее ему
 * представление в виде комментария. Представление в виде комментария
 * основано на типе вершины и используется для генерации комментариев или
 * документации.
 * \param i_type Значение перечисления типа вершины
 * \return Представление типа вершины в виде комментария.
 */
std::string vertexTypeToComment(VertexTypes i_type);

/*!
 * \~english
 * \brief getSequentialComment
 * Converts a Sequential vertex object to its comment representation
 * This function takes a GraphVertexSequental object and returns its
 * corresponding comment representation. The comment representation
 * is based on the signals, included for current element (enable, reset,
 * clear and some more) and is used for generating comments or
 * documentation
 * \param i_seq Sequential element to getting information
 * \return The comment representation of the GraphVertexSequental object.
 * \code
 * GraphPtr graph = std::make_shared<OrientedGraph>();
 * auto *clk = graph->addInput("clk");
 * auto *data = graph->addInput("data");
 * auto *seq = graph->addSequential(ff, clk, data, "q");
 * std::string comment = VertexUtils::getSequentialComment(seq);
 * std::cout << "Comment for input vertex type: " << comment << std::endl;
 * \endcode
 *
 * \~russian
 * \brief getSequentialComment
 * Преобразует объект последовательностной вершины в его представление в
 * виде комментария. Эта функция принимает объект GraphVertexSequential и
 * возвращает соответствующее ему представление в виде комментария.
 * Представление в виде комментария основано на сигналах, включенных для
 * текущего элемента (enable, reset, clear и некоторые другие), и
 * используется для генерации комментариев или документации.
 * \param i_seq Последовательностный элемент для получения информации
 * \return Представление объекта GraphVertexSequential в виде комментария.
 */
std::string getSequentialComment(const GraphVertexSequential *i_seq);

} // namespace VertexUtils

/*!
 * \class GraphVertexBase
 *
 * \~english
 * \param d_baseGraph A weak pointer to the base graph containing this
 * vertex
 * \param d_name The name of the vertex. It is a string containing the name
 * of a vertex
 * \param d_value The value of the vertex
 * \param d_level The vertex level is represented by the uint32_t type
 * \param d_inConnections vector of weak pointers to input connections with
 * other vertices
 * \param d_outConnections vector of strong pointers to output connections
 * with other vertices
 * \param d_type Vertex Type - Defined by the VertexTypes enumeration
 * \param d_count Vertex counter for naming and other purposes. Represented
 * by the uint_fast64_t type
 * \param d_hashed A string containing the calculated hash value for the
 * vertex
 * \param d_hasHash A HASH_CONDITION value containing hashing status
 * \see HASH_CONDITION
 * \param d_needUpdate A VERTEX_STATE value containing status in the
 * context of level calculating
 * \see VERTEX_STATE
 *
 * \~russian
 * \param d_baseGraph Слабый указатель (weak pointer) на базовый граф,
 * содержащий эту вершину
 * \param d_name Имя вершины. Это строка, содержащая имя вершины
 * \param d_value Значение вершины
 * \param d_level Уровень вершины, представленный типом uint32_t
 * \param d_inConnections вектор слабых указателей на входные соединения
 * с другими вершинами
 * \param d_outConnections вектор сильных указателей на выходные
 * соединения с другими вершинами
 * \param d_type Тип вершины - определяется перечислением VertexTypes
 * \param d_count Счетчик вершин для именования и других целей.
 * Представлен типом uint_fast64_t
 * \param d_hashed Строка, содержащая вычисленное значение хэша для
 * вершины
 * \param d_hasHash Значение HASH_CONDITION, содержащее статус хэширования
 * \see HASH_CONDITION
 * \param d_needUpdate Значение VERTEX_STATE, содержащее статус в
 * контексте вычисления уровня
 * \see VERTEX_STATE
 */
class GraphVertexBase {
  friend class OrientedGraph;

public:
  /*!
   * \~english
   * \brief resetes counter of unique id-s for vertices
   *
   * \~russian
   * \brief сбрасывает счетчик уникальных идентификаторов для вершин
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

  /*!
   * \~english
   * \brief used for reset for all states being used (hash, updateValue,
   * etc)
   *
   * \~russian
   * \brief используется для сброса всех используемых состояний (hash,
   * updateValue и т.д.)
   */
  void resetAllStates() {
    d_needUpdate = VS_NOT_CALC;
    d_hasHash = HC_NOT_CALC;
  }

  /*!
   * \~english
   * \brief required before recalculation of the levels of graph
   *
   * \~russian
   * \brief требуется перед пересчетом уровней графа
   */
  void resetNeedUpdateState() { d_needUpdate = VS_NOT_CALC; }

  /*!
   * \~english
   * \brief required before hash recalculation
   *
   * \~russian
   * \brief требуется перед пересчетом хэша
   */
  void resetHashState() { d_hasHash = HC_NOT_CALC; }

  /*!
   * \~english
   * \brief required before get vertices by level
   *
   * \~russian
   * \brief требуется перед получением вершин по уровню
   */
  void resetUsedLevelState() {
    // remove flag using bitwise operations
    d_needUpdate = static_cast<VERTEX_STATE>(d_needUpdate & ~VS_USED_LEVEL);
  }

  /*!
   * \~english
   * \brief GraphVertexBase Constructs a GraphVertexBase object with the
   * specified vertex type and optional graph
   * \param i_type The type of the vertex (from the VertexTypes enum).
   * \param i_graph Optional pointer to the graph containing the vertex
   *
   * \~russian
   * \brief GraphVertexBase Создает объект GraphVertexBase с указанным
   * типом вершины и опциональным графом
   * \param i_type Тип вершины (из перечисления VertexTypes).
   * \param i_graph Опциональный указатель на граф, содержащий вершину
   */
  GraphVertexBase(const VertexTypes i_type, GraphPtr i_graph);

  /*!
   * \~english
   * \brief GraphVertexBase Constructs a GraphVertexBase object with the
   * specified vertex type, name, and optional graph.
   * \param i_type The type of the vertex (from the VertexTypes enum)
   * \param i_name The name of the vertex.
   * \param i_graph Optional pointer to the graph containing the vertex.
   *
   * \~russian
   * \brief GraphVertexBase Создает объект GraphVertexBase с указанным
   * типом вершины, именем и опциональным графом.
   * \param i_type Тип вершины (из перечисления VertexTypes)
   * \param i_name Имя вершины.
   * \param i_graph Опциональный указатель на граф, содержащий вершину.
   */
  GraphVertexBase(const VertexTypes i_type, std::string_view i_name,
                  GraphPtr i_graph);

  GraphVertexBase &operator=(const GraphVertexBase &other) =
      default; // оператор копирующего присваивания
  GraphVertexBase &operator=(GraphVertexBase &&other) =
      default; // оператор перемещающего присваивания

  /*!
   * \~english
   * \brief GraphVertexBase Constructs a GraphVertexBase object with
   * parameters similar to other GraphVertexBase object
   * \param other The other vertex
   *
   * \~russian
   * \brief GraphVertexBase Создает объект GraphVertexBase с параметрами,
   * аналогичными другому объекту GraphVertexBase
   * \param other Другая вершина
   */
  GraphVertexBase(const GraphVertexBase &other) = default;

  /*!
   * \~english
   * \brief GraphVertexBase Move constructor for the class
   * \param other Vertex to move or copy
   *
   * \~russian
   * \brief GraphVertexBase Конструктор перемещения для класса
   * \param other Вершина для перемещения или копирования
   */
  GraphVertexBase(GraphVertexBase &&other) = default;

  virtual ~GraphVertexBase();

  /*!
   * \~english
   * \brief getType
   * This method returns the type of the vertex as a value of the
   * VertexTypes enum
   * \return The type of the vertex (from the VertexTypes enum).
   * \code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * VertexTypes type = vertex.getType();
   * std::cout << "Type of the vertex: " << type << std::endl;
   * \endcode
   *
   * \~russian
   * \brief getType
   * Этот метод возвращает тип вершины как значение перечисления VertexTypes
   * \return Тип вершины (из перечисления VertexTypes).
   */
  VertexTypes getType() const;

  // Get для типа вершины в фомате строки

  /*!
   * \~english
   * \brief getTypeName
   * This method returns the string representation of the vertex type by
   * parsing the vertex type enum value using the settings object
   * associated with the vertex
   * \return The string representation of the vertex type.
   * \code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * std::string typeName = vertex.getTypeName();
   * std::cout << "String representation of the vertex type: " << typeName;
   * \endcode
   *
   * \~russian
   * \brief getTypeName
   * Этот метод возвращает строковое представление типа вершины путем
   * разбора значения перечисления типа вершины с использованием объекта
   * настроек, связанного с вершиной
   * \return Строковое представление типа вершины.
   */
  std::string getTypeName() const;

  // Get-Set для имен входов

  /*!
   * \~english
   * \brief setName This method sets the name of the vertex to the
   * specified string.
   * \param i_name The new name for the vertex.
   * \code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * vertex.setName("new_vertex_name");
   * std::cout << "New name of the vertex: " << vertex.getRawName()
   * << std::endl;
   * \endcode
   *
   * \~russian
   * \brief setName Этот метод устанавливает имя вершины в указанную строку.
   * \param i_name Новое имя для вершины.
   */
  void setName(std::string_view i_name);

  /*!
   * \~english
   * \brief getName Returns the name of the vertex
   * \return The name of the vertex.
   * \code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * std::string name = vertex.getName();
   * std::cout << "Name of the vertex: " << name << std::endl;
   * \endcode
   *
   * \~russian
   * \brief getName Возвращает имя вершины
   * \return Имя вершины.
   */
  std::string getName() const;

  /*!
   * \~english
   * \brief getName Returns concatenation of the name of the vertex and
   * i_prefix
   * \return The concatenation of name of the vertex and i_prefix
   * \code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * std::string s = vertex.getName("some prefix");
   * std::cout << "Name of the vertex: " << s << std::endl;
   * \endcode
   *
   * \~russian
   * \brief getName Возвращает конкатенацию имени вершины и i_prefix
   * \return Конкатенация имени вершины и i_prefix
   */
  std::string getName(const std::string &i_prefix) const;

  /*!
   * \~english
   * \brief getRawName Returns string_view object with name of the vertex
   * \return The string_view object with name of the vertex
   * \code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * std::string s = vertex.getName("some prefix");
   * std::cout << "Name of the vertex: " << std::string(s) << std::endl;
   * \endcode
   *
   * \~russian
   * \brief getRawName Возвращает объект string_view с именем вершины
   * \return Объект string_view с именем вершины
   */
  std::string_view getRawName() const;

  // Get для значения вершины

  /*!
   * \~english
   * \brief getValue Returns the value of the vertex.
   * \return The value of the vertex.
   * \code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * char value = vertex.getValue();
   * std::cout << "Value of the vertex: " << value << std::endl;
   * \endcode
   *
   * \~russian
   * \brief getValue Возвращает значение вершины.
   * \return Значение вершины.
   */
  char getValue() const;

  /*!
   * \~english
   * \brief updateValue A virtual function for updating the vertex value.
   * The implementation is provided in derived classes
   * \return the value of the vertex after its update
   *
   * \~russian
   * \brief updateValue Виртуальная функция для обновления значения вершины.
   * Реализация предоставляется в производных классах
   * \return значение вершины после ее обновления
   */
  virtual char updateValue() = 0;

  virtual void removeValue();

  // Get-Set для уровня

  /*!
   * \~english
   * \brief getLevel Returns the level of the vertex
   * \return The level of the vertex
   * \code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * uint32_t level = vertex.getLevel();
   * std::cout << "Level of the vertex: " << level << std::endl;
   * \endcode
   *
   * \~russian
   * \brief getLevel Возвращает уровень вершины
   * \return Уровень вершины
   */
  uint32_t getLevel() const;

  /*!
   * \~english
   * \brief updateLevel
   * This method updates the level of the vertex based on the levels of its
   * input connections. It iterates through each input connection and sets
   * the vertex level to the maximum level of its input connections plus
   * one. If you are going to call this method for a second time, please,
   * set all flags, used in updateLevel to their default state.
   *
   * \~russian
   * \brief updateLevel
   * Этот метод обновляет уровень вершины на основе уровней ее
   * входных соединений. Он итерируется по каждому входному соединению и
   * устанавливает уровень вершины равным максимальному уровню ее входных
   * соединений плюс один. Если вы собираетесь вызвать этот метод во второй
   * раз, пожалуйста, установите все флаги, используемые в updateLevel,
   * в их состояние по умолчанию.
   */
  virtual void updateLevel();

  /*!
   * \~english
   * \brief getVerticesByLevel Support method for
   * OrientedGraph::getVerticesByLevel() calculating
   * \param i_targetLevel level, vertices with which should be found
   * \param i_result reference to vector in which found values are stored
   * \param i_fromOut if true than begins search from outputs, else from
   * inputs (depends on level, if target level is closer to outputs or
   * inputs)
   * \return true if vertex has required level and false if not
   *
   * \~russian
   * \brief getVerticesByLevel Вспомогательный метод для вычисления
   * OrientedGraph::getVerticesByLevel()
   * \param i_targetLevel уровень, вершины с которым должны быть найдены
   * \param i_result ссылка на вектор, в котором сохраняются найденные
   * значения
   * \param i_fromOut если true, то начинает поиск от выходов, иначе от
   * входов (зависит от уровня, если целевой уровень ближе к выходам или
   * входам)
   * \return true, если вершина имеет требуемый уровень, и false, если нет
   */
  bool getVerticesByLevel(uint32_t i_targetLevel,
                          std::vector<VertexPtr> &i_result,
                          bool i_fromOut = true);

  /*!
   * \~english
   * \brief getGate
   * Returns the type of the basic logic gate represented by this vertex. If
   * the vertex does not correspond to any basic logic gate, it returns Gate
   * Default
   * \return The type of the basic logic gate represented by this vertex
   * \code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * Gates gateType = vertex.getGate();
   * \endcode
   *
   * \~russian
   * \brief getGate
   * Возвращает тип базового логического вентиля, представленного этой
   * вершиной. Если вершина не соответствует ни одному базовому
   * логическому вентилю, возвращает Gate Default (по умолчанию)
   * \return Тип базового логического вентиля, представленного этой вершиной
   */
  virtual Gates getGate() const { return Gates::GateDefault; }

  // Get-Set для базового графа
  // void setBaseGraph(std::shared_ptr<OrientedGraph> const i_baseGraph);

  /*!
   * \~english
   * \brief getBaseGraph
   * \return a weak pointer to the base graph associated with this vertex.
   *
   * \~russian
   * \brief getBaseGraph
   * \return слабый указатель (weak pointer) на базовый граф, связанный с
   * этой вершиной.
   */
  GraphPtrWeak getBaseGraph() const;

  /*!
   * \~english
   * \brief reserveInConnections reserving memory in d_inConnections for
   * i_size other vertices
   * \code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * vertex.reserveInConnections(5);
   * \endcode
   *
   * \~russian
   * \brief reserveInConnections резервирует память в d_inConnections для
   * i_size других вершин
   */
  void reserveInConnections(size_t i_size);

  /*!
   * \~english
   * \brief reserveOutConnections reserving memory in d_outConnections for
   * i_size other vertices
   * \code
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * vertex.reserveOutConnections(5);
   * \endcode
   *
   * \~russian
   * \brief reserveOutConnections резервирует память в d_outConnections для
   * i_size других вершин
   */
  void reserveOutConnections(size_t i_size);

  /*!
   * \~english
   * \brief getInConnections
   * \return A vector of weak pointers to the input connections of this
   * vertex
   * \code
   * // Creating an instance of the GraphVertexBase class
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * // Get the vector of the input connections of this vertex
   * std::pmr::vector<VertexPtr>& inConnections = vertex.getInConnections();
   * // Iterate over the input connections and do something with them
   * for (const auto& connection : inConnections)
   * {
   * // Checking if the connection is valid
   * if (!connection.expired())
   * {
   * // Getting shared_ptr from weak_ptr
   * VertexPtr inputVertex = connection.lock();
   * if (inputVertex)
   * {
   * // Doing something with the input Vertex
   * }
   * }
   * }
   * \endcode
   *
   * \~russian
   * \brief getInConnections
   * \return Вектор слабых указателей на входные соединения этой вершины
   */
  std::vector<VertexPtr> getInConnections() const;

  /*!
   * \~english
   * \brief addVertexToInConnections
   * Adds a vertex to the input connections of this vertex and returns the
   * count of occurrences of the given vertex in the input connections
   * \param i_vert The vertex to be added to the input connections
   * \return The count of occurrences of the given vertex in the input
   * connections after adding it
   * \code
   * // Creating an instance of the GraphVertexBase class
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * // Creating another vertex
   * VertexPtr anotherVertex =
   * std::make_shared<GraphVertexInput>(VertexTypes::input, "vertex2");
   * // Adding a second vertex to the input connections of the first vertex
   * // and getting the number of occurrences
   * uint32_t occurrences = vertex.addVertexToInConnections(anotherVertex);
   * // Output of the result
   * std::cout << "Occurrences in input connections: " << occurrences
   * << std::endl;
   * \endcode
   *
   * \~russian
   * \brief addVertexToInConnections
   * Добавляет вершину во входные соединения этой вершины и возвращает
   * количество вхождений данной вершины во входных соединениях
   * \param i_vert Вершина, которая будет добавлена во входные соединения
   * \return Количество вхождений данной вершины во входных соединениях
   * после ее добавления
   */
  virtual uint32_t addVertexToInConnections(VertexPtr i_vert);

  /*!
   * \~english
   * \brief getOutConnections
   * \return A vector of shared pointers to the output connections of this
   * vertex
   * \code
   * // Creating an instance of the GraphVertexBase class
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * // Creating another vertex
   * VertexPtr anotherVertex =
   * std::make_shared<GraphVertexInput>(VertexTypes::output, "vertex2");
   * // Adding the second vertex to the output connections of the first
   * vertex.addVertexToOutConnections(anotherVertex);
   * // get the vector of the output connections of the first vertex
   * auto& outConnections = vertex.getOutConnections();
   * // output information about the output connections
   * std::cout << "Output connections of the first vertex:" << std::endl;
   * for (const auto& connection : outConnections)
   * {
   * std::cout << " - " << connection->getName() << std::endl;
   * }
   * \endcode
   *
   * \~russian
   * \brief getOutConnections
   * \return Вектор сильных указателей на выходные соединения этой вершины
   */
  std::vector<VertexPtr> getOutConnections() const;

  /*!
   * \~english
   * \brief addVertexToOutConnections
   * Adds a vertex to the output connections of this vertex if it is not
   * already present
   * \param i_vert The vertex to be added to the output connections
   * \return true if the vertex was successfully added, false if it was
   * already present in the output connections.
   * \code
   * TO DO:
   * \endcode
   *
   * \~russian
   * \brief addVertexToOutConnections
   * Добавляет вершину в выходные соединения этой вершины, если ее там
   * еще нет
   * \param i_vert Вершина, которая будет добавлена в выходные соединения
   * \return true, если вершина была успешно добавлена, false, если она уже
   * присутствовала в выходных соединениях.
   */
  bool addVertexToOutConnections(VertexPtr i_vert);

  /*!
   * \~english
   * \brief calculateHash
   * Calculates the hash value for the vertex based on its outgoing
   * connections.
   * When running for a second time, set hash flags to default state
   * \return The hash value of the vertex based on its outgoing connections.
   * \code
   * // Creating an instance of the GraphVertexBase class
   * GraphVertexBase vertex(VertexTypes::output, "vertex1");
   * // Creating two more vertices
   * VertexPtr vertex2 = std::make_shared<GraphVertexInput>(
   * VertexTypes::input, "vertex2");
   * VertexPtr vertex3 = std::make_shared<GraphVertexInput>(
   * VertexTypes::input, "vertex3");
   * // Adding the second and third vertices to the output connections
   * vertex.addVertexToOutConnections(vertex2);
   * vertex.addVertexToOutConnections(vertex3);
   * // Calculating the hash for the first vertex
   * std::string hashValue = vertex.calculateHash();
   * // Output of the result
   * std::cout << "Hash for the first vertex: " << hashValue << std::endl;
   * \endcode
   *
   * \~russian
   * \brief calculateHash
   * Вычисляет значение хэша для вершины на основе ее исходящих
   * соединений.
   * При повторном запуске установите флаги хэша в состояние по умолчанию
   * \return Значение хэша вершины на основе ее исходящих соединений.
   */
  virtual size_t calculateHash();

  /*!
   * \~english
   * \brief toVerilog Generates Verilog code for the vertex
   * \return A string containing Verilog code for the vertex, or an empty
   * string if the vertex type is not "output" or if the incoming
   * connection is invalid
   * \code
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
   * \endcode
   *
   * \~russian
   * \brief toVerilog Генерирует код Verilog для вершины
   * \return Строка, содержащая код Verilog для вершины, или пустая строка,
   * если тип вершины не "output" или если входящее соединение
   * недействительно
   */
  virtual std::string toVerilog() const;

  /*!
   * \~english
   * \brief calls toVerilog and allows to write vertex as string to a
   * stream
   *
   * \~russian
   * \brief вызывает toVerilog и позволяет записать вершину в виде строки
   * в поток
   */
  friend std::ostream &operator<<(std::ostream &stream,
                                  const GraphVertexBase &matrix);

  /*!
   * \~english
   * \brief toDOT Generates DOT code for the vertex
   * \return
   *
   * \~russian
   * \brief toDOT Генерирует код DOT для вершины
   * \return
   */
  virtual DotReturn toDOT();

  /*!
   * \~english
   * \brief Used for check if vertex is a subGraph vertex output
   * \return true if is vertex is subGraph vertex output or false if not
   *
   * \~russian
   * \brief Используется для проверки, является ли вершина выходом вершины
   * подграфа
   * \return true, если вершина является выходом вершины подграфа, или
   * false, если нет
   */
  virtual bool isSubgraphBuffer() const { return false; }

  /*!
   * \~english
   * \brief log Used for easylogging++
   * \param os Stream for easylogging
   *
   * \~russian
   * \brief log Используется для easylogging++
   * \param os Поток для easylogging
   */
#ifdef LOGFLAG
  virtual void log(el::base::type::ostream_t &os) const;
#endif

protected:
  /*!
   * \~english
   * \brief removeVertexToOutConnections
   * Removes a vertex from the output connections of this vertex.
   * \param i_vert The vertex to be removed from the output connections.
   * \return true if the vertex was successfully removed, false otherwise.
   * \code
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
   * \endcode
   *
   * \~russian
   * \brief removeVertexToOutConnections
   * Удаляет вершину из выходных соединений этой вершины.
   * \param i_vert Вершина, которую нужно удалить из выходных соединений.
   * \return true, если вершина была успешно удалена, false в противном
   * случае.
   */
  bool removeVertexToOutConnections(VertexPtr i_vert);

  /*!
   * \~english
   * \brief removeVertexToInConnections
   * Removes a vertex from the input connections of this vertex.
   * \param i_vert The vertex to be removed from the input connections.
   * \param i_full lag indicating whether to remove all occurrences of the
   * vertex from the input connections. (Not available in current version)
   * If true, all occurrences will be removed. If false, only the first
   * occurrence will be removed. Default - false
   * \return true if the vertex was successfully removed, false otherwise.
   * \code
   * // Creating an instance of the GraphVertexBase class
   * GraphVertexBase vertex(VertexTypes::input, "vertex1");
   * // Creating another vertex
   * VertexPtr anotherVertex =
   * std::make_shared<GraphVertexInput>(VertexTypes::input, "vertex2");
   * // Adding a second vertex to the input connections
   * vertex.addVertexToInConnections(anotherVertex);
   * // Removing the second vertex from the input connections
   * bool removed = vertex.removeVertexToInConnections(anotherVertex,
   * false);
   * // Output of the result
   * if (removed)
   * {
   * std::cout << "Removed successfully" << std::endl;
   * }
   * else
   * {
   * std::cout << "Not found" << std::endl;
   * }
   * \endcode
   *
   * \~russian
   * \brief removeVertexToInConnections
   * Удаляет вершину из входных соединений этой вершины.
   * \param i_vert Вершина, которую нужно удалить из входных соединений.
   * \param i_full флаг, указывающий, нужно ли удалять все вхождения
   * вершины из входных соединений. (Недоступно в текущей версии).
   * \return true, если вершина была успешно удалена, false в противном
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
  // Определяем тип вершины: подграф, вход, выход, константа или одна из
  // базовых логических операций.
  VertexTypes d_type;
};

static_assert(sizeof(GraphVertexBase) <= 104u);

std::ostream &operator<<(std::ostream &stream, const GraphVertexBase &vertex);

} // namespace CG_Graph