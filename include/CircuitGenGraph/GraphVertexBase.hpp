#pragma once

/// @file GraphVertexBase.hpp
/// @brief \~english Base class for all graph vertices with common functionality \~russian Базовый класс для всех вершин графа с общей функциональностью
/// @ingroup GraphCore
///
/// \~english
/// Defines the abstract base class GraphVertexBase which provides:
/// - Connection management (input/output edges)
/// - Level calculation for topological ordering
/// - Hash computation for graph comparison
/// - Verilog/DOT/GraphML export capabilities
/// - Memory management integration
/// \~russian
/// Определяет абстрактный базовый класс GraphVertexBase, который предоставляет:
/// - Управление соединениями (входящие/исходящие ребра)
/// - Вычисление уровня для топологической сортировки
/// - Вычисление хэша для сравнения графов
/// - Возможности экспорта в форматы Verilog/DOT/GraphML
/// - Интеграцию управления памятью
/// \~

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

/// @brief \~english VertexUtils Namespace containing utility functions for working with vertices \~russian VertexUtils Пространство имен, содержащее вспомогательные функции для работы с вершинами

namespace VertexUtils {

/// @brief \~english gateToString Converts a gate type enum value to its string representation \~russian gateToString Преобразует значение перечисления типа вентиля в его строковое представление
/// @param i_type \~english The gate type enum value \~russian Значение перечисления типа вентиля
/// @return \~english The string representation of the gate type. \~russian Строковое представление типа вентиля.
/// @code
/// Gates gateType = Gates::GateAnd;
/// std::string gateString = VertexUtils::gateToString(gateType);
/// std::cout << "String representation of gate type: " << gateString <<
/// std::endl; gateType = Gates::GateNot; gateString =
/// VertexUtils::gateToString(gateType); std::cout << "String representation of
/// gate type: " << gateString << std::endl;
/// @endcode
std::string gateToString(Gates i_type);

/// @brief \~english vertexTypeToComment Converts a vertex type enum value to its Verilog representation \~russian vertexTypeToComment Преобразует значение перечисления типа вершины в его представление для Verilog
/// @param i_type \~english The vertex type enum value. \~russian Значение перечисления типа вершины.
/// @return \~english The Verilog representation of the vertex type \~russian Представление типа вершины для Verilog
/// @code
/// VertexTypes vertexType = VertexTypes::Input;
/// std::string verilogType = VertexUtils::vertexTypeToVerilog(vertexType);
/// std::cout << "Verilog representation of vertex type: " << verilogType <<
/// std::endl;
/// @endcode
std::string vertexTypeToVerilog(VertexTypes i_type);

/// @brief
/// \~english
/// vertexTypeToComment
/// Converts a vertex type enum value to its comment representation
/// This function takes a vertex type
/// enum value (from the VertexTypes enum) and
/// returns its corresponding comment representation.
/// The comment representation is based on
/// the vertex type and is used for generating comments or
/// documentation
/// \~russian
/// vertexTypeToComment
/// Преобразует значение перечисления типа вершины в его представление в виде комментария.
/// Эта функция принимает значение перечисления типа вершины (из перечисления VertexTypes)
/// и возвращает соответствующее ему представление в виде комментария.
/// Представление в виде комментария основано на типе вершины и используется
/// для генерации комментариев или документации.
/// \~
/// @param i_type \~english The vertex type enum value (from the VertexTypes enum) \~russian Значение перечисления типа вершины (из перечисления VertexTypes)
/// @return \~english The comment representation of the vertex type. \~russian Представление типа вершины в виде комментария.
/// @code
/// VertexTypes vertexType = VertexTypes::Input;
/// std::string comment = VertexUtils::vertexTypeToComment(vertexType);
/// std::cout << "Comment for input vertex type: " << comment << std::endl;
/// @endcode
std::string vertexTypeToComment(VertexTypes i_type);

/// @brief
/// \~english
/// getSequentialComment
/// Converts a Sequential vertex object to its comment representation
/// This function takes a GraphVertexSequental object and
/// returns its corresponding comment
/// representation. The comment representation
/// is based on the signals, included for current element (enable, reset,
/// clear and some more) and is used for generating comments or documentation
/// \~russian
/// getSequentialComment
/// Преобразует объект последовательностной вершины в его представление в виде комментария.
/// Эта функция принимает объект GraphVertexSequential и возвращает соответствующее ему
/// представление в виде комментария. Представление в виде комментария
/// основано на сигналах, включенных для текущего элемента (enable, reset,
/// clear и некоторые другие), и используется для генерации комментариев или документации.
/// \~
/// @param i_seq \~english Sequential element to getting information \~russian Последовательностный элемент для получения информации
/// @return \~english The comment representation of the GraphVertexSequental object. \~russian Представление объекта GraphVertexSequential в виде комментария.
/// @code
/// GraphPtr graph = std::make_shared<OrientedGraph>();
/// auto *clk = graph->addInput("clk");
/// auto *data = graph->addInput("data");
/// auto *seq = graph->addSequential(ff, clk, data, "q");
/// std::string comment = VertexUtils::getSequentialComment(seq);
/// std::cout << "Comment for input vertex type: " << comment << std::endl;
/// @endcode
std::string getSequentialComment(const GraphVertexSequential *i_seq);

} // namespace VertexUtils

/// \~english
/// class GraphVertexBase
/// \~russian
/// class GraphVertexBase
/// \~
/// @param d_baseGraph \~english A weak pointer to the base graph containing this vertex \~russian Слабый указатель (weak pointer) на базовый граф, содержащий эту вершину
/// @param d_name \~english The name of the vertex. It is a string containing the name of a vertex \~russian Имя вершины. Это строка, содержащая имя вершины
/// @param d_value \~english The value of the vertex \~russian Значение вершины
/// @param d_level \~english The vertex level is represented by the uint32_t type \~russian Уровень вершины, представленный типом uint32_t
/// @param d_inConnections \~english vector of weak pointers to input connections with other vertices \~russian вектор слабых указателей на входные соединения с другими вершинами
/// @param d_outConnections \~english vector of strong pointers to output connections with other vertices \~russian вектор сильных указателей на выходные соединения с другими вершинами
/// @param d_type \~english Vertex Type - Defined by the VertexTypes enumeration \~russian Тип вершины - определяется перечислением VertexTypes
/// @param d_count \~english Vertex counter for naming and other purposes. Represented by the uint_fast64_t type \~russian Счетчик вершин для именования и других целей. Представлен типом uint_fast64_t
/// @param d_hashed \~english A string containing the calculated hash value for the vertex \~russian Строка, содержащая вычисленное значение хэша для вершины
/// @param d_hasHash \~english A HASH_CONDITION value containing hashing status \~russian Значение HASH_CONDITION, содержащее статус хэширования
/// @see HASH_CONDITION
/// @param d_needUpdate \~english A VERTEX_STATE value containing status in the context of level calculating \~russian Значение VERTEX_STATE, содержащее статус в контексте вычисления уровня
/// @see VERTEX_STATE

class GraphVertexBase {
  friend class OrientedGraph;

public:
  /// @brief \~english resetes counter of unique id-s for vertices \~russian сбрасывает счетчик уникальных идентификаторов для вершин
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

  /// @brief \~english used for reset for all states being used (hash, updateValue, etc) \~russian используется для сброса всех используемых состояний (hash, updateValue и т.д.)
  void resetAllStates() {
    d_needUpdate = VS_NOT_CALC;
    d_hasHash = HC_NOT_CALC;
  }

  /// @brief \~english required before recalculation of the levels of graph \~russian требуется перед пересчетом уровней графа
  void resetNeedUpdateState() { d_needUpdate = VS_NOT_CALC; }

  /// @brief \~english required before hash recalculation \~russian требуется перед пересчетом хэша
  void resetHashState() { d_hasHash = HC_NOT_CALC; }

  /// @brief \~english required before get vertices by level \~russian требуется перед получением вершин по уровню
  void resetUsedLevelState() {
    // remove flag using bitwise operations
    d_needUpdate = static_cast<VERTEX_STATE>(d_needUpdate & ~VS_USED_LEVEL);
  }

  /// @brief \~english GraphVertexBase Constructs a GraphVertexBase object with the specified vertex type and optional graph \~russian GraphVertexBase Создает объект GraphVertexBase с указанным типом вершины и опциональным графом
  /// @param i_type \~english The type of the vertex (from the VertexTypes enum). \~russian Тип вершины (из перечисления VertexTypes).
  /// @param i_graph \~english Optional pointer to the graph containing the vertex \~russian Опциональный указатель на граф, содержащий вершину
  GraphVertexBase(const VertexTypes i_type, GraphPtr i_graph);

  /// @brief \~english GraphVertexBase Constructs a GraphVertexBase object with the specified vertex type, name, and optional graph. \~russian GraphVertexBase Создает объект GraphVertexBase с указанным типом вершины, именем и опциональным графом.
  /// @param i_type \~english The type of the vertex (from the VertexTypes enum) \~russian Тип вершины (из перечисления VertexTypes)
  /// @param i_name \~english The name of the vertex. \~russian Имя вершины.
  /// @param i_graph \~english Optional pointer to the graph containing the vertex. \~russian Опциональный указатель на граф, содержащий вершину.
  GraphVertexBase(const VertexTypes i_type, std::string_view i_name,
                  GraphPtr i_graph);

  GraphVertexBase &operator=(const GraphVertexBase &other) =
      default; // оператор копирующего присваивания
  GraphVertexBase &operator=(GraphVertexBase &&other) =
      default; // оператор перемещающего присваивания
  /// @brief \~english GraphVertexBase Constructs a GraphVertexBase object with parameters similar to other GraphVertexBase object \~russian GraphVertexBase Создает объект GraphVertexBase с параметрами, аналогичными другому объекту GraphVertexBase
  /// @param other \~english The other vertex \~russian Другая вершина
  GraphVertexBase(const GraphVertexBase &other) = default;
  /// @brief \~english GraphVertexBase Move constructor for the class \~russian GraphVertexBase Конструктор перемещения для класса
  /// @param other \~english Vertex to move or copy \~russian Вершина для перемещения или копирования
  GraphVertexBase(GraphVertexBase &&other) = default;

  virtual ~GraphVertexBase();

  /// @brief
  /// \~english
  /// getType
  /// This method returns the type of the vertex as a value of the VertexTypes enum
  /// \~russian
  /// getType
  /// Этот метод возвращает тип вершины как значение перечисления VertexTypes
  /// \~
  /// @return \~english The type of the vertex (from the VertexTypes enum). \~russian Тип вершины (из перечисления VertexTypes).
  /// @code
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// VertexTypes type = vertex.getType();
  /// std::cout << "Type of the vertex: " << type << std::endl;
  /// @endcode

  VertexTypes getType() const;

  // Get для типа вершины в фомате строки

  /// @brief
  /// \~english
  /// getTypeName
  /// This method returns the string representation of the vertex type by
  /// parsing the vertex type enum value using the settings object
  /// associated with the vertex
  /// \~russian
  /// getTypeName
  /// Этот метод возвращает строковое представление типа вершины путем
  /// разбора значения перечисления типа вершины с использованием объекта настроек,
  /// связанного с вершиной
  /// \~
  /// @return \~english The string representation of the vertex type. \~russian Строковое представление типа вершины.
  /// @code
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// std::string typeName = vertex.getTypeName();
  /// std::cout << "String representation of the vertex type: " << typeName;
  /// @endcode

  std::string getTypeName() const;

  // Get-Set для имен входов

  /// @brief \~english setName This method sets the name of the vertex to the specified string. \~russian setName Этот метод устанавливает имя вершины в указанную строку.
  /// @param i_name \~english The new name for the vertex. \~russian Новое имя для вершины.
  /// @code
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// vertex.setName("new_vertex_name");
  /// std::cout << "New name of the vertex: " << vertex.getRawName() <<
  /// std::endl;
  /// @endcode

  void setName(std::string_view i_name);

  /// @brief \~english getName Returns the name of the vertex \~russian getName Возвращает имя вершины
  /// @return \~english The name of the vertex. \~russian Имя вершины.
  /// @code
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// std::string name = vertex.getName();
  /// std::cout << "Name of the vertex: " << name << std::endl;
  /// @endcode

  std::string getName() const;
  /// @brief \~english getName Returns concatenation of the name of the vertex and i_prefix \~russian getName Возвращает конкатенацию имени вершины и i_prefix
  /// @return \~english The concatenation of name of the vertex and i_prefix \~russian Конкатенация имени вершины и i_prefix
  /// @code
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// std::string s = vertex.getName("some prefix");
  /// std::cout << "Name of the vertex: " << s << std::endl;
  /// @endcode

  std::string getName(const std::string &i_prefix) const;

  /// @brief \~english getRawName Returns string_view object with name of the vertex \~russian getRawName Возвращает объект string_view с именем вершины
  /// @return \~english The string_view object with name of the vertex \~russian Объект string_view с именем вершины
  /// @code
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// std::string s = vertex.getName("some prefix");
  /// std::cout << "Name of the vertex: " << std::string(s) << std::endl;
  /// @endcode

  std::string_view getRawName() const;

  // Get для значения вершины
  /// @brief \~english getValue Returns the value of the vertex. \~russian getValue Возвращает значение вершины.
  /// @return \~english The value of the vertex. \~russian Значение вершины.
  /// @code
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// char value = vertex.getValue();
  /// std::cout << "Value of the vertex: " << value << std::endl;
  /// @endcode

  char getValue() const;

  /// @brief \~english updateValue A virtual function for updating the vertex value. The implementation is provided in derived classes \~russian updateValue Виртуальная функция для обновления значения вершины. Реализация предоставляется в производных классах
  /// @return \~english the value of the vertex after its update \~russian значение вершины после ее обновления

  virtual char updateValue() = 0;

  virtual void removeValue();

  // Get-Set для уровня

  /// @brief \~english getLevel Returns the level of the vertex \~russian getLevel Возвращает уровень вершины
  /// @return \~english The level of the vertex \~russian Уровень вершины
  /// @code
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// uint32_t level = vertex.getLevel();
  /// std::cout << "Level of the vertex: " << level << std::endl;
  /// @endcode

  uint32_t getLevel() const;

  /// @brief
  /// \~english
  /// updateLevel
  /// This method updates the level of the vertex based on the levels of its
  /// input connections. It iterates through each input connection and sets
  /// the vertex level to the maximum level of its input connections plus one.
  /// If you are going to call this method for a second time, please, set
  /// all flags, used in updateLevel to their default state.
  /// \~russian
  /// updateLevel
  /// Этот метод обновляет уровень вершины на основе уровней ее
  /// входных соединений. Он итерируется по каждому входному соединению и устанавливает
  /// уровень вершины равным максимальному уровню ее входных соединений плюс один.
  /// Если вы собираетесь вызвать этот метод во второй раз, пожалуйста, установите
  /// все флаги, используемые в updateLevel, в их состояние по умолчанию.
  /// \~

  virtual void updateLevel();

  /// @brief \~english getVerticesByLevel Support method for OrientedGraph::getVerticesByLevel() calculating \~russian getVerticesByLevel Вспомогательный метод для вычисления OrientedGraph::getVerticesByLevel()
  /// @param i_targetLevel \~english level, vertices with which should be found \~russian уровень, вершины с которым должны быть найдены
  /// @param i_result \~english reference to vector in which found values are stored \~russian ссылка на вектор, в котором сохраняются найденные значения
  /// @param i_fromOut \~english if true than begins search from outputs, else from inputs (depends on level, if target level is closer to outputs or inputs) \~russian если true, то начинает поиск от выходов, иначе от входов (зависит от уровня, если целевой уровень ближе к выходам или входам)
  /// @return \~english true if vertex has required level and false if not \~russian true, если вершина имеет требуемый уровень, и false, если нет
  bool getVerticesByLevel(uint32_t i_targetLevel,
                          std::vector<VertexPtr> &i_result,
                          bool i_fromOut = true);

  /// @brief
  /// \~english
  /// getGate
  /// Returns the type of the basic logic gate represented by this vertex. If
  /// the vertex does not correspond to any basic logic gate, it returns Gate
  /// Default
  /// \~russian
  /// getGate
  /// Возвращает тип базового логического вентиля, представленного этой вершиной. Если
  /// вершина не соответствует ни одному базовому логическому вентилю, возвращает Gate
  /// Default (по умолчанию)
  /// \~
  /// @return \~english The type of the basic logic gate represented by this vertex \~russian Тип базового логического вентиля, представленного этой вершиной
  /// @code
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// Gates gateType = vertex.getGate();
  /// @endcode

  virtual Gates getGate() const { return Gates::GateDefault; }

  // Get-Set для базового графа
  // void setBaseGraph(std::shared_ptr<OrientedGraph> const i_baseGraph);

  /// @brief \~english getBaseGraph \~russian getBaseGraph
  /// @return \~english a weak pointer to the base graph associated with this vertex. \~russian слабый указатель (weak pointer) на базовый граф, связанный с этой вершиной.

  GraphPtrWeak getBaseGraph() const;

  /// @brief \~english reserveInConnections reserving memory in d_inConnections for i_size other vertices \~russian reserveInConnections резервирует память в d_inConnections для i_size других вершин
  /// @code
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// vertex.reserveInConnections(5);
  /// @endcode
  void reserveInConnections(size_t i_size);
  /// @brief \~english reserveInConnections reserving memory in d_outConnections for i_size other vertices \~russian reserveInConnections резервирует память в d_outConnections для i_size других вершин
  /// @code
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// vertex.reserveOutConnections(5);
  /// @endcode
  void reserveOutConnections(size_t i_size);

  /// @brief \~english getInConnections \~russian getInConnections
  /// @return \~english A vector of weak pointers to the input connections of this vertex \~russian Вектор слабых указателей на входные соединения этой вершины
  /// @code
  /// // Creating an instance of the GraphVertexBase class
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// // Get the vector of the input connections of this vertex
  /// std::pmr::vector<VertexPtr>& inConnections = vertex.getInConnections();
  /// // Iterate over the input connections and do something with them
  /// for (const auto& connection : inConnections)
  /// {
  /// // Checking if the connection is valid
  ///   if (!connection.expired())
  /// {
  /// // Getting shared_ptr from weak_ptr
  /// VertexPtr inputVertex = connection.lock();
  /// if (inputVertex)
  /// {
  ///   // Doing something with the input Vertex
  /// }
  /// }
  /// }
  /// @endcode

  std::vector<VertexPtr> getInConnections() const;

  /// @brief
  /// \~english
  /// addVertexToInConnections
  /// Adds a vertex to the input connections of this vertex and returns the
  /// count of occurrences of the given vertex in the input connections
  /// \~russian
  /// addVertexToInConnections
  /// Добавляет вершину во входные соединения этой вершины и возвращает
  /// количество вхождений данной вершины во входных соединениях
  /// \~
  /// @param i_vert \~english The vertex to be added to the input connections \~russian Вершина, которая будет добавлена во входные соединения
  /// @return \~english The count of occurrences of the given vertex in the input connections after adding it \~russian Количество вхождений данной вершины во входных соединениях после ее добавления
  /// @code
  /// // Creating an instance of the GraphVertexBase class
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// // Creating another vertex
  /// VertexPtr anotherVertex =
  /// std::make_shared<GraphVertexInput>(VertexTypes::input, "vertex2");
  /// // Adding a second vertex to the input connections of the first vertex
  /// and getting the number of occurrences
  /// uint32_t occurrences = vertex.addVertexToInConnections(anotherVertex);
  /// // Output of the result
  /// std::cout << "The number of occurrences of the second vertex in the input
  /// connections of the first vertex: " << occurrences << std::endl;
  /// @endcode

  virtual uint32_t addVertexToInConnections(VertexPtr i_vert);

  /// @brief \~english getOutConnections \~russian getOutConnections
  /// @return \~english A vector of shared pointers to the output connections of this vertex \~russian Вектор сильных указателей на выходные соединения этой вершины
  /// @code
  /// // Creating an instance of the GraphVertexBase class
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// // Creating another vertex
  /// VertexPtr anotherVertex =
  /// std::make_shared<GraphVertexInput>(VertexTypes::output, "vertex2");
  /// // Adding the second vertex to the output connections of the first vertex
  /// vertex.addVertexToOutConnections(anotherVertex);
  /// // get the vector of the output connections of the first vertex
  /// std::pmr::vector<VertexPtr>& outConnections = vertex.getOutConnections();
  /// // output information about the output connections
  /// std::cout << "Output connections of the first vertex:" << std::endl;
  /// for (const auto& connection : outConnections)
  /// {
  ///   std::cout << " - " << connection->getName() << std::endl;
  /// }
  /// @endcode

  std::vector<VertexPtr> getOutConnections() const;

  /// @brief
  /// \~english
  /// addVertexToOutConnections
  /// Adds a vertex to the output connections of this vertex if it is not
  /// already present
  /// \~russian
  /// addVertexToOutConnections
  /// Добавляет вершину в выходные соединения этой вершины, если ее там
  /// еще нет
  /// \~
  /// @param i_vert \~english The vertex to be added to the output connections \~russian Вершина, которая будет добавлена в выходные соединения
  /// @return \~english true if the vertex was successfully added, false if it was already present in the output connections. \~russian true, если вершина была успешно добавлена, false, если она уже присутствовала в выходных соединениях.
  /// @code
  /// TO DO:
  /// @endcode

  bool addVertexToOutConnections(VertexPtr i_vert);

  /// @brief
  /// \~english
  /// calculateHash
  /// Calculates the hash value for the vertex based on its outgoing
  /// connections.
  ///  When running for a second time, set hash flags to default state
  /// \~russian
  /// calculateHash
  /// Вычисляет значение хэша для вершины на основе ее исходящих
  /// соединений.
  ///  При повторном запуске установите флаги хэша в состояние по умолчанию
  /// \~
  /// @return \~english The hash value of the vertex based on its outgoing connections. \~russian Значение хэша вершины на основе ее исходящих соединений.
  /// @code
  /// // Creating an instance of the GraphVertexBase class
  /// GraphVertexBase vertex(VertexTypes::output, "vertex1");
  /// // Creating two more vertices
  /// VertexPtr vertex2 = std::make_shared<GraphVertexInput>(VertexTypes::input,
  /// "vertex2"); VertexPtr vertex3 =
  /// std::make_shared<GraphVertexInput>(VertexTypes::input, "vertex3");
  /// // Adding the second and third vertices to the output connections of the
  /// first vertex vertex.addVertexToOutConnections(vertex2);
  /// vertex.addVertexToOutConnections(vertex3);
  /// // Calculating the hash for the first vertex
  /// std::string hashValue = vertex.calculateHash();
  /// // Output of the result
  /// std::cout << "Hash for the first vertex: " << hashValue << std::endl;
  /// @endcode

  virtual size_t calculateHash();

  /// @brief
  /// \~english
  /// toVerilog
  /// Generates Verilog code for the vertex
  /// \~russian
  /// toVerilog
  /// Генерирует код Verilog для вершины
  /// \~
  /// @return \~english A string containing Verilog code for the vertex, or an empty string if the vertex type is not "output" or if the incoming connection is invalid \~russian Строка, содержащая код Verilog для вершины, или пустая строка, если тип вершины не "output" или если входящее соединение недействительно
  /// @code
  /// // Creating an instance of the GraphVertexBase class with the type
  /// "output" and the name "output_vertex"
  /// GraphPtr graph = std::make_shared<OrientedGraph>();
  /// VertexPtr outputVertex = graph->addOutput("output");
  /// // Creating another vertex with the type "input" and the name
  /// VertexPtr inputVertex = graph->addInput("input_vertex");
  /// // Setting the input connection for the vertex "output_vertex"
  /// graph->addEdge(inputVertex, outputVertex);
  /// // Generating the Verilog code for the vertex "output_vertex"
  /// std::string verilogCode = outputVertex->toVerilog();
  /// // Display the generated Verilog code on the screen
  /// std::cout << "Generated Verilog code:\n" << verilogCode << std::endl;
  /// @endcode

  virtual std::string toVerilog() const;

  /// @brief \~english calls toVerilog and allows to write vertex as string to a stream \~russian вызывает toVerilog и позволяет записать вершину в виде строки в поток
  friend std::ostream &operator<<(std::ostream &stream,
                                  const GraphVertexBase &matrix);

  /// @brief \~english toDOT Generates DOT code for the vertex \~russian toDOT Генерирует код DOT для вершины
  /// @return

  virtual DotReturn toDOT();

  /// @brief \~english Used for check if vertex is a subGraph vertex output \~russian Используется для проверки, является ли вершина выходом вершины подграфа
  /// @return \~english true if is vertex is subGraph vertex output or false if not \~russian true, если вершина является выходом вершины подграфа, или false, если нет
  virtual bool isSubgraphBuffer() const { return false; }

  /// @brief \~english log Used for easylogging++ \~russian log Используется для easylogging++
  /// @param os \~english Stream for easylogging \~russian Поток для easylogging
#ifdef LOGFLAG
  virtual void log(el::base::type::ostream_t &os) const;
#endif

protected:
  /// @brief
  /// \~english
  /// removeVertexToOutConnections
  /// Removes a vertex from the output connections of this vertex.
  /// \~russian
  /// removeVertexToOutConnections
  /// Удаляет вершину из выходных соединений этой вершины.
  /// \~
  /// @param i_vert \~english The vertex to be removed from the output connections. \~russian Вершина, которую нужно удалить из выходных соединений.
  /// @return \~english true if the vertex was successfully removed, false otherwise. \~russian true, если вершина была успешно удалена, false в противном случае.
  /// @code
  /// // Creating an instance of the GraphVertexBase class
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// // Creating another vertex
  /// VertexPtr anotherVertex =
  /// std::make_shared<GraphVertexInput>(VertexTypes::output, "vertex2");
  /// // Adding the second vertex to the output connections of the first vertex
  /// vertex.addVertexToOutConnections(anotherVertex);
  /// // Removing the second vertex from the output connections of the first
  /// vertex bool removed = vertex.removeVertexToOutConnections(anotherVertex);
  /// // Output of the result
  /// if (removed)
  /// {
  ///   std::cout << "Вторая вершина успешно удалена из выходных соединений
  ///   первой вершины." << std::endl;
  /// }
  /// else
  /// {
  ///   std::cout << "Вторая вершина не найдена в выходных соединениях первой
  ///   вершины." << std::endl;
  /// }
  /// @endcode

  bool removeVertexToOutConnections(VertexPtr i_vert);
  /// @brief
  /// \~english
  /// removeVertexToInConnections
  /// Removes a vertex from the input connections of this vertex.
  /// \~russian
  /// removeVertexToInConnections
  /// Удаляет вершину из входных соединений этой вершины.
  /// \~
  /// @param i_vert \~english The vertex to be removed from the input connections. \~russian Вершина, которую нужно удалить из входных соединений.
  /// @param i_full \~english lag indicating whether to remove all occurrences of the vertex from the input connections. (Not available in current version) If true, all occurrences will be removed. If false, only the first occurrence will be removed. Default - false \~russian флаг, указывающий, нужно ли удалять все вхождения вершины из входных соединений. (Недоступно в текущей версии) Если true, все вхождения будут удалены. Если false, будет удалено только первое вхождение. По умолчанию - false
  /// @return \~english true if the vertex was successfully removed, false otherwise. \~russian true, если вершина была успешно удалена, false в противном случае.
  /// @code
  /// // Creating an instance of the GraphVertexBase class
  /// GraphVertexBase vertex(VertexTypes::input, "vertex1");
  /// // Creating another vertex
  /// VertexPtr anotherVertex =
  /// std::make_shared<GraphVertexInput>(VertexTypes::input, "vertex2");
  /// // Adding a second vertex to the input connections of the first vertex
  /// vertex.addVertexToInConnections(anotherVertex);
  /// // Removing the second vertex from the input connections of the first
  /// vertex bool removed = vertex.removeVertexToInConnections(anotherVertex,
  /// false);
  /// // Output of the result
  /// if (removed)
  /// {
  ///    std::cout << "The second vertex has been successfully removed from the
  ///    input connections of the first vertex" << std::endl;
  /// }
  /// else
  /// {
  ///    std::cout << "The second vertex was not found in the input connections
  ///    of the first vertex" << std::endl;
  /// }
  /// @endcode

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

std::ostream &operator<<(std::ostream &stream, const GraphVertexBase &vertex);

} // namespace CG_Graph