/**
 * @file GraphVertex.hpp
 * \~english
 * @brief Graph vertex declarations: GraphVertexInput, GraphVertexOutput,
 * GraphVertexGates, GraphVertexConstant, GraphVertexSubGraph,
 * GraphVertexSequential.
 *
 * \~russian
 * @brief Объявления вершин графа: GraphVertexInput, GraphVertexOutput,
 * GraphVertexGates, GraphVertexConstant, GraphVertexSubGraph,
 * GraphVertexSequential.
 * @author Vladimir Zunin
 * @author Fuuulkrum7
 * @author Theossr
 * @author rainbowkittensss
 */
#pragma once

#include "CircuitGenGraph/GraphUtils.hpp"
#include "CircuitGenGraph/GraphVertexBus.hpp"
#include <CircuitGenGraph/GraphVertexBase.hpp>

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace CG_Graph {
class GraphVertexBus;
/**
 * \~english
 * @todo Maybe Description some virtual methods for Graph's classes,
 * such as GraphVertexGates
 *
 * @class GraphVertexInput
 * @brief Represents a vertex in a directed graph that serves as an input
 * vertex. It inherits from the GraphVertexBase class and extends its
 * functionality to work with input vertices.
 *
 * \~russian
 * @todo Возможно, стоит добавить описание некоторых виртуальных
 * методов для классов графа, таких как GraphVertexGates
 *
 * @class GraphVertexInput
 * @brief Представляет вершину в ориентированном графе, которая служит
 * входной вершиной. Наследуется от класса GraphVertexBase и расширяет его
 * функциональность для работы с входными вершинами.
 */
class GraphVertexInput : public GraphVertexBase {
public:
  /**
   * \~english
   * @brief GraphVertexInput Initializes the GraphVertexInput object with
   * the provided shared pointer to the base graph and vertex type.
   * @param i_baseGraph Shared pointer to the base graph.
   * @param i_type i_type Type of the vertex. Default is VertexTypes::input.
   *
   * \~russian
   * @brief GraphVertexInput Инициализирует объект GraphVertexInput с
   * предоставленным shared-указателем на базовый граф и типом вершины.
   * @param i_baseGraph Shared-указатель на базовый граф.
   * @param i_type Тип вершины. По умолчанию VertexTypes::input.
   */
  GraphVertexInput(GraphPtr i_baseGraph,
                   const VertexTypes i_type = VertexTypes::input);

  /**
   * \~english
   * @brief GraphVertexInput Initializes the GraphVertexInput object with
   * the provided name, shared pointer to the base graph, and vertex type.
   * @param i_name Name of the vertex.
   * @param i_baseGraph Shared pointer to the base graph.
   * @param i_type Type of the vertex.
   *
   * \~russian
   * @brief GraphVertexInput Инициализирует объект GraphVertexInput с
   * предоставленными именем, shared-указателем на базовый граф и типом
   * вершины.
   * @param i_name Имя вершины.
   * @param i_baseGraph Shared-указатель на базовый граф.
   * @param i_type Тип вершины.
   */
  GraphVertexInput(std::string_view i_name, GraphPtr i_baseGraph,
                   const VertexTypes i_type = VertexTypes::input);

  /**
   * @author Theossr
   * \~english
   * @brief Sets a new value for the vertex.
   * @param value The value to set.
   *
   * \~russian
   * @brief Устанавливает новое значение для вершины.
   * @param value Устанавливаемое значение.
   */
  void setValue(const char value);

  /**
   * @author Vladimir Zunin
   * \~english
   * @brief updateValue A virtual function for updating the vertex value.
   * The implementation is provided in derived classes
   * @return the value of the vertex after its update
   *
   * \~russian
   * @brief updateValue Виртуальная функция для обновления значения
   * вершины. Реализация предоставляется в производных классах
   * @return значение вершины после ее обновления
   */
  virtual char updateValue() override;

  /** @author Theossr */
  virtual void removeValue() override;

  /**
   * @author Fuuulkrum7
   * \~english
   * @brief Recalculates vertex level from input connections.
   * The level becomes `max(input levels) + 1`.
   * Before repeated calls, reset helper flags used by level traversal.
   *
   * \~russian
   * @brief Пересчитывает уровень вершины по входным соединениям.
   * Уровень становится равным `max(уровни входов) + 1`.
   * Перед повторным вызовом сбросьте служебные флаги обхода по уровням.
   */
  virtual void updateLevel() override;

  /**
   * @author Vladimir Zunin
   * \~english
   * @brief writes vertex to dot
   *
   * \~russian
   * @brief записывает вершину в формат dot
   */
  DotReturn toDOT() override;

#ifdef LOGFLAG
  /**
   * @author Vladimir Zunin
   * \~english
   * @brief log Used for easylogging++
   * @param os Stream for easylogging
   *
   * \~russian
   * @brief log Используется для easylogging++
   * @param os Поток для easylogging
   */
  virtual void log(el::base::type::ostream_t &os) const override;
#endif

protected:
  GraphVertexInput(VertexTypes i_type);

private:
};

/**
 * @class GraphVertexConstant
 * \~english
 * @brief A simple class, making available to use constant values in circuits
 *
 * \~russian
 * @brief Простой класс, позволяющий использовать константные значения в
 * схемах
 */
class GraphVertexConstant : public GraphVertexInput {
public:
  /**
   * \~english
   * @brief Initializes the GraphVertexConstant object with the provided
   * shared pointer to the base graph and constant character.
   * @param i_const char symbol for constant (1, 0, z, x)
   * @param i_baseGraph Shared pointer to the base graph.
   *
   * \~russian
   * @brief Инициализирует объект GraphVertexConstant с предоставленным
   * shared-указателем на базовый граф и символом константы.
   * @param i_const символ char для константы (1, 0, z, x)
   * @param i_baseGraph Shared-указатель на базовый граф.
   */
  GraphVertexConstant(char i_const, GraphPtr i_baseGraph, bool i_isBus = false);

  /**
   * \~english
   * @brief Initializes the GraphVertexConstant object with the provided
   * shared pointer to the base graph and constant character.
   * @param i_const char symbol for constant (1, 0, z, x)
   * @param i_name Name of the vertex.
   * @param i_baseGraph Shared pointer to the base graph.
   *
   * \~russian
   * @brief Инициализирует объект GraphVertexConstant с предоставленным
   * shared-указателем на базовый граф и символом константы.
   * @param i_const символ char для константы (1, 0, z, x)
   * @param i_name Имя вершины.
   * @param i_baseGraph Shared-указатель на базовый граф.
   */
  GraphVertexConstant(char i_const, std::string_view i_name,
                      GraphPtr i_baseGraph, bool i_isBus = false);

  // clang-format off
                      
  ~GraphVertexConstant() override {};

  // clang-format on

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief calculates hash for constant.
   * Calculates the hash value for the vertex based on its outgoing
   * connections.
   * When running for a second time, set hash flags to default state
   * @return The hash value of the vertex based on its outgoing connections.
   * @par Example
   * @code
   * GraphPtr graph = std::make_shared<OrientedGraph>();
   * VertexPtr vertex = graph->addConst('1', "const1");
   * VertexPtr vertex2 = graph->addOutput("out2");
   * VertexPtr vertex3 = graph->addOutput("out3");
   * graph->addEdge(vertex, vertex2);
   * graph->addEdge(vertex, vertex3);
   * std::string hashValue = std::to_string(vertex->calculateHash());
   * // Output of the result
   * std::cout << "Hash for the first vertex: " << hashValue << std::endl;
   * @endcode
   *
   * \~russian
   * @brief вычисляет хэш для константы.
   * Вычисляет значение хэша для вершины на основе ее исходящих соединений.
   * При повторном запуске установите флаги хэша в состояние по умолчанию
   * @return Значение хэша вершины на основе ее исходящих соединений.
   */
  size_t calculateHash() override;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief toVerilog Generates Verilog code for the constant vertex
   * @return A string containing Verilog code for the vertex, or an empty
   * string if the vertex type is not "output" or if the incoming
   * connection is invalid
   * @par Example
   * @code
   * // Creating an instance of the GraphVertexBase class with the type
   * // "output" and the name "output_vertex"
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
   * @brief toVerilog Генерирует код Verilog для константной вершины
   * @return Строка, содержащая код Verilog для вершины, или пустая строка,
   * если тип вершины не "output" или если входящее соединение
   * недействительно
   */
  virtual std::string toVerilog() const override;

  /** @author Vladimir Zunin */
  DotReturn toDOT() override;

  /**
   * @author Vladimir Zunin
   *
   * \~english
   * @brief Builds a default Verilog instance for constant vertex
   * (wire-style declaration).
   *
   * \~russian
   * @brief Формирует стандартный Verilog-инстанс для константной вершины
   * (в виде wire-объявления).
   */
  virtual std::string getVerilogInstance();

  /**
   * \~english
   * @brief Constants keep their fixed value across simulation clears.
   * \~russian
   * @brief Константы сохраняют фиксированное значение при очистке симуляции.
   */
  void removeValue() override;

#ifdef LOGFLAG
  /**
   * @author Vladimir Zunin
   * \~english
   * @brief log Used for easylogging++
   * @param os Stream for easylogging
   *
   * \~russian
   * @brief log Используется для easylogging++
   * @param os Поток для easylogging
   */
  virtual void log(el::base::type::ostream_t &os) const override;
#endif

private:
  char d_constValue = ValueStates::FalseValue;
};

/**
 * @class GraphVertexSubGraph
 * \~english
 * @brief Vertex that stores a shared pointer to a nested subgraph.
 * Used to keep hierarchical connectivity in the parent graph.
 *
 * \~russian
 * @brief Вершина, хранящая shared-указатель на вложенный подграф.
 * Используется для поддержки иерархической связности графа.
 */
class GraphVertexSubGraph : public GraphVertexBase {
public:
  /**
   * \~english
   * @brief Sets path to an external Verilog file for this subgraph vertex.
   * @param verilogPath Path to Verilog file.
   *
   * \~russian
   * @brief Устанавливает путь к внешнему Verilog-файлу для этой
   * вершины-подграфа.
   * @param verilogPath Путь к Verilog-файлу.
   */
  void setVerilogPath(const std::string &verilogPath) {
    d_verilogPath = verilogPath;
  }

  /**
   * \~english
   * @brief Returns path to an external Verilog file.
   * @return Path string, or empty string if it is not set.
   *
   * \~russian
   * @brief Возвращает путь к внешнему Verilog-файлу.
   * @return Строка пути или пустая строка, если путь не задан.
   */
  const std::string &getVerilogPath() const { return d_verilogPath; }

  /**
   * \~english
   * @brief Initializes the GraphVertexSubGraph object with the provided
   * shared pointers to the base graph and nested subgraph.
   * @param i_subGraph Shared pointer to the subGraph.
   * @param i_baseGraph Shared pointer to the base graph.
   *
   * \~russian
   * @brief Инициализирует объект GraphVertexSubGraph с предоставленным
   * shared-указателями на базовый граф и подграф.
   * @param i_subGraph Shared-указатель на подграф.
   * @param i_baseGraph Shared-указатель на базовый граф.
   */
  GraphVertexSubGraph(GraphPtr i_subGraph, GraphPtr i_baseGraph);
  GraphVertexSubGraph(GraphPtr i_subGraph, std::string_view i_name,
                      GraphPtr i_baseGraph);

  // clang-format off

  ~GraphVertexSubGraph() override {};

  // clang-format on

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief updateValue A virtual function for updating the vertex value.
   * The implementation is provided in derived classes
   * @return the value of the vertex after its update
   *
   * \~russian
   * @brief updateValue Виртуальная функция для обновления значения
   * вершины. Реализация предоставляется в производных классах
   * @return значение вершины после ее обновления
   */
  char updateValue() override;

  /** @author Theossr */
  void removeValue() override;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Recalculates vertex level from input connections.
   * The level becomes `max(input levels) + 1`.
   * Before repeated calls, reset helper flags used by level traversal.
   *
   * \~russian
   * @brief Пересчитывает уровень вершины по входным соединениям.
   * Уровень становится равным `max(уровни входов) + 1`.
   * Перед повторным вызовом сбросьте служебные флаги обхода по уровням.
   */
  void updateLevel() override;

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
   * // Creating an instance of the GraphVertexBase class with the type
   * // "output" and the name "output_vertex"
   * GraphPtr graph = std::make_shared<OrientedGraph>();
   * GraphPtr subgraph = std::make_shared<OrientedGraph>();
   * subgraph->addEdge(
   * subgraph->addInput("in"),
   * subgraph->addOutput("out"));
   * VertexPtr outputVertex = graph->addOutput("output");
   * // Creating another vertex with the type "input" and the name
   * VertexPtr inputVertex = graph->addInput("input_vertex");
   * // Creating another vertex with the type "subGraph" and the name
   * VertexPtr subgraphOut = graph->addSubGraph(subgraph, inputVertex).back();
   * // Setting the input connection for the vertex "output_vertex"
   * graph->addEdge(inputVertex, subgraphOut);
   * graph->addEdge(subgraphOut, outputVertex);
   * // Generating the Verilog code for the vertex subgraph
   * VertexPtr subgaphItself = getVerticesByLevel(1u).back();
   * std::cout << subgaphItself << '\n';
   * @endcode
   *
   * \~russian
   * @brief toVerilog Генерирует код Verilog для вершины
   * @return Строка, содержащая код Verilog для вершины, или пустая строка,
   * если тип вершины не "output" или если входящее соединение
   * недействительно
   */
  std::string toVerilog() const override;

  /** @author Vladimir Zunin */
  DotReturn toDOT() override;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Internal helper used by `OrientedGraph` export methods.
   * @param i_path Output directory path. Do not append submodule name here;
   * it is created automatically.
   * @param i_filename Output file name (defaults to graph name).
   * @return `true` if file was written successfully, otherwise `false`.
   *
   * \~russian
   * @brief Внутренний вспомогательный метод для методов экспорта
   * `OrientedGraph`.
   * @param i_path Путь к каталогу вывода. Подмодуль добавлять не нужно:
   * каталог создается автоматически.
   * @param i_filename Имя выходного файла (по умолчанию совпадает с именем
   * графа).
   * @return `true`, если файл успешно записан, иначе `false`.
   */
  bool toVerilog(std::string i_path, std::string i_filename = "");

  /** @author Fuuulkrum7 */
  bool toDOT(std::string i_path, std::string i_filename = "");

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Internal helper used by `OrientedGraph` export methods.
   *
   * \~russian
   * @brief Внутренний вспомогательный метод для методов экспорта
   * `OrientedGraph`.
   */
  bool toGraphML(std::ofstream &i_fileStream) const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Internal helper used by `OrientedGraph` export methods.
   * @param i_indent Indentation level
   * @param i_prefix Prefix string
   * @return Formatted GraphML string
   *
   * \~russian
   * @brief Внутренний вспомогательный метод для методов экспорта
   * `OrientedGraph`.
   * @param i_indent Уровень отступа
   * @param i_prefix Строка префикса
   * @return Отформатированная строка GraphML
   */
  std::string toGraphML(uint16_t i_indent = 0, std::string i_prefix = "") const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Internal helper used by `OrientedGraph` hash calculation.
   * If called repeatedly, clear hash flags beforehand.
   * @return Hash value
   *
   * \~russian
   * @brief Внутренний вспомогательный метод для вычисления хэша в
   * `OrientedGraph`. При повторном запуске предварительно сбросьте флаги
   * хэша.
   * @return Значение хэша
   */
  size_t calculateHash() override;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Assigns a new subgraph to this vertex.
   *
   * \~russian
   * @brief Устанавливает новый подграф для этой вершины.
   */
  void setSubGraph(GraphPtr i_subGraph);

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Returns the subgraph stored in this vertex.
   * @return Shared pointer to stored subgraph.
   *
   * \~russian
   * @brief Возвращает подграф, связанный с этой вершиной.
   * @return Shared-указатель на сохраненный подграф.
   */
  GraphPtr getSubGraph() const;

  /**
   * \~english
   * @brief Returns output buffer vertices affected by specified external input.
   * @param i_outerInput Input vertex
   * @return Vector of output vertices
   *
   * \~russian
   * @brief Возвращает выходные буферные вершины, на которые влияет заданный
   * внешний вход.
   * @param i_outerInput Входная вершина
   * @return Вектор выходных вершин
   */
  std::vector<VertexPtr>
  getOutputBuffersByOuterInput(VertexPtr i_outerInput) const;

  /**
   * \~english
   * @brief Returns external input vertices that affect specified output buffer.
   * @param i_outputBuffer Output buffer vertex
   * @return Vector of input vertices
   *
   * \~russian
   * @brief Возвращает внешние входные вершины, влияющие на заданный выходной
   * буфер.
   * @param i_outputBuffer Вершина выходного буфера
   * @return Вектор входных вершин
   */
  std::vector<VertexPtr>
  getOuterInputsByOutputBuffer(VertexPtr i_outputBuffer) const;

  /**
   * \~english
   * @brief Value of the nested output that drives this instance GateBuf.
   * Requires `updateValue()` to have run for the current simulation vector.
   * \~russian
   * @brief Значение вложенного выхода, питающего этот instance GateBuf.
   */
  char bufferedOutputValue(VertexPtr i_buffer) const;

#ifdef LOGFLAG
  /**
   * @author Vladimir Zunin
   * \~english
   * @brief log Used for easylogging++
   * @param os Stream for easylogging
   *
   * \~russian
   * @brief log Используется для easylogging++
   * @param os Поток для easylogging
   */
  virtual void log(el::base::type::ostream_t &os) const override;
#endif

private:
  GraphPtr d_subGraph;
  /*!
   * \~english Path to an external Verilog file
   * \~russian Путь к внешнему Verilog-файлу
   */
  std::string d_verilogPath = "";
  /// Cached nested outputs for the current `graphSimulation` vector.
  std::vector<char> d_simOutputs;
};

/**
 * @class GraphVertexOutput
 * \~english
 * @brief It is a vertex of the graph, specially designed for data output.
 * It inherits from the GraphVertexBase class and adds additional
 * functionality related to data output
 *
 * \~russian
 * @brief Это вершина графа, специально предназначенная для вывода данных.
 * Наследуется от класса GraphVertexBase и добавляет дополнительную
 * функциональность, связанную с выводом данных
 */
class GraphVertexOutput : public GraphVertexBase {
public:
  GraphVertexOutput(GraphPtr i_baseGraph, bool i_isBus = false);

  /** @author Fuuulkrum7 */
  GraphVertexOutput(std::string_view i_name, GraphPtr i_baseGraph,
                    bool i_isBus = false);

  /**
   * @author Theossr
   *
   * \~english
   * @brief updateValue updates the value of the current vertex of the
   * graph based on the values of its incoming connections and the type of
   * logical element (or "gate"). Depending on the type of gate and the
   * values of the incoming links, the method uses the truth tables to
   * perform the corresponding logical operation and sets a new value for
   * the current vertex
   * @return A char that represents the new value of the current vertex
   * after updating based on the values of its incoming connections and
   * the type of logical element (or "gate").
   *
   * \~russian
   * @brief updateValue обновляет значение текущей вершины графа на основе
   * значений ее входящих соединений и типа логического элемента (или
   * "вентиля"). В зависимости от типа вентиля и значений входящих связей,
   * метод использует таблицы истинности для выполнения соответствующей
   * логической операции и устанавливает новое значение для текущей вершины
   * @return Символ char, представляющий новое значение текущей вершины
   * после обновления на основе значений ее входящих соединений и типа
   * логического элемента (или "вентиля").
   */
  char updateValue() override;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief updateLevel updates the level of the current vertex in the
   * graph based on the levels of its incoming connections. If a vertex has
   * incoming connections, the method determines the highest level among
   * all the vertices to which it is connected, and sets the level of the
   * current vertex to one higher than the highest level
   *
   * \~russian
   * @brief updateLevel обновляет уровень текущей вершины в графе на основе
   * уровней ее входящих соединений. Если вершина имеет входящие
   * соединения, метод определяет самый высокий уровень среди всех вершин,
   * к которым она подключена, и устанавливает уровень текущей вершины на
   * единицу больше, чем самый высокий уровень
   */
  void updateLevel() override;

  /** @author Vladimir Zunin */
  DotReturn toDOT() override;

#ifdef LOGFLAG
  /**
   * @author Vladimir Zunin
   * \~english
   * @brief log Used for easylogging++
   * @param os Stream for easylogging
   *
   * \~russian
   * @brief log Используется для easylogging++
   * @param os Поток для easylogging
   */
  virtual void log(el::base::type::ostream_t &os) const override;
#endif

private:
};

/**
 * @class GraphVertexGates
 * \~english
 * @brief Class representing a vertex in a graph with gates
 *
 * \~russian
 * @brief Класс, представляющий вершину в графе с логическими вентилями
 */
class GraphVertexGates : public GraphVertexBase {
public:
  GraphVertexGates(Gates i_gate, GraphPtr i_baseGraph, bool i_isBus = false);

  /** @author Fuuulkrum7 */
  GraphVertexGates(Gates i_gate, std::string_view i_name, GraphPtr i_baseGraph,
                   bool i_isBus = false);

  // clang-format off

  ~GraphVertexGates() override {};


  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief updateValue Updates the value of the vertex
   * @return The updated value of the vertex
   * @todo Add example of usage
   * @throws{std::invalid_argument} if any of the input connections point
   * to a nullptr
   *
   * \~russian
   * @brief updateValue Обновляет значение вершины
   * @return Обновленное значение вершины
   * @todo Добавить пример использования
   * @throws{std::invalid_argument} если любое из входных соединений
   * указывает на nullptr
   */
  virtual char updateValue() override;

  /** @author Theossr */
  void removeValue() override;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief calculateHash Calculates the hash value of the vertex. When
   * running for a second time, set hash flags to default state
   * @return The calculated hash value as a number
   *
   * \~russian
   * @brief calculateHash Вычисляет значение хэша вершины. При повторном
   * запуске установите флаги хэша в состояние по умолчанию
   * @return Вычисленное значение хэша в виде числа
   */
  size_t calculateHash() override;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief getVerilogString Gets a string in Verilog format representing
   * the current vertex
   * @return A string in Verilog format representing the current vertex
   * @throws{std::invalid_argument} if any input connection is invalid
   *
   * \~russian
   * @brief getVerilogString Получает строку в формате Verilog,
   * представляющую текущую вершину
   * @return Строка в формате Verilog, представляющая текущую вершину
   * @throws{std::invalid_argument} если какое-либо входное соединение
   * недействительно
   */
  std::string getVerilogString() const;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Returns gate type associated with this vertex.
   * @return Gate type of this vertex.
   * @par Example
   * @code
   * GraphVertexGates vertex(Gates::GateAnd);
   * Gates gateType = vertex.getGate();
   * std:: cout << "Gate type : " << gateType << std::endl;
   * @endcode
   *
   * \~russian
   * @brief getGate Возвращает тип вентиля, связанный с текущей вершиной
   * @return Тип вентиля, связанный с текущей вершиной
   */
  Gates getGate() const override;

  /**
   * \~english
   * @brief Sets gate type only if current type is `GateDefault`.
   * In all other cases, no changes are made.
   * @par Example
   * @code
   * GraphVertexGates vertex(Gates::GateDefault);
   * vertex.setGateIfDefault(Gates::GateAnd);
   * Gates gateType = vertex.getGate();
   * std::cout << "Gate type: " << gateType << std::endl;
   * @endcode
   *
   * \~russian
   * @brief Устанавливает тип вентиля только если текущий тип `GateDefault`.
   * Во всех остальных случаях изменений не происходит.
   * @par Пример
   * @code
   * GraphVertexGates vertex(Gates::GateDefault);
   * vertex.setGateIfDefault(Gates::GateAnd);
   * Gates gateType = vertex.getGate();
   * std::cout << "Тип вентиля: " << gateType << std::endl;
   * @endcode
   */

  void setGateIfDefault(Gates type);

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief addVertexToInConnections
   * Buffer and Not types of gates must have only one element in
   * d_inConnections, so realization for GraphVertexGates has a check
   * before adding
   * @param i_vert Vertex that will be added to d_inConnections of this
   * @return `1` for the connection just added
   * @throws{std::length_error} in case of connecting more than one
   * vertex in d_inConnections for Buf/Not
   *
   * \~russian
   * @brief addVertexToInConnections
   * Типы вентилей Буфер (Buffer) и НЕ (Not) должны иметь только один
   * элемент в d_inConnections, поэтому реализация для GraphVertexGates
   * содержит проверку перед добавлением
   * @param i_vert Вершина, которая будет добавлена в d_inConnections
   * этой вершины
   * @return `1` для только что добавленной связи
   * @throws{std::length_error} при подключении более одного входа к Buf/Not
   */
  uint32_t addVertexToInConnections(VertexPtr i_vert) override;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Generates Verilog code for current gate vertex according to its type and input
   * connections. If a vertex has no input connections, an empty string
   * is returned. If any input connection is invalid, an exception is
   * thrown.
   * @return A Verilog format string for the current vertex
   * @throws{std::invalid_argument} if any input connection is invalid
   *
   * \~russian
   * @brief Генерирует Verilog-код для текущей вершины-вентиля с учетом ее
   * типа и входных
   * соединениями. Если у вершины нет входных соединений, возвращается
   * пустая строка. Если какое-либо входное соединение недействительно,
   * выбрасывается исключение.
   * @return Строка в формате Verilog для текущей вершины
   * @throws{std::invalid_argument} если какое-либо входное соединение
   * недействительно
   */
  virtual std::string
  toVerilog() const override; // помечен виртуальным потому что у этого класса
                              // есть потомок-шина со своим методом

  /** @author Vladimir Zunin */
  DotReturn toDOT() override;

  /** @author Fuuulkrum7 */
  bool isSubgraphBuffer() const override;

#ifdef LOGFLAG
  /**
   * @author Vladimir Zunin
   * \~english
   * @brief log Used for easylogging++
   * @param os Stream for easylogging
   *
   * \~russian
   * @brief log Используется для easylogging++
   * @param os Поток для easylogging
   */
  virtual void log(el::base::type::ostream_t &os) const override;
#endif
protected:
  std::string
  toVerilogCommon(std::function<std::string()> printBinaryOperators,
                  std::function<std::string()> printUnaryOperators) const;
  Gates d_gate;
  friend class GraphVertexSubGraph;
};

/**
 * @class GraphVertexSequential
 * \~english
 * @brief Class representing a sequential logic vertex in the graph
 * (e.g., flip-flops, latches)
 *
 * \~russian
 * @brief Класс, представляющий последовательностную логическую вершину
 * в графе (например, триггеры, защелки)
 */
class GraphVertexSequential : public GraphVertexBase {
public:
  // clang-format off

  /**
   * \~english
   * @brief Constructor for sequential vertex
   * @param i_type type of sequential vertex
   * @param i_clk clock signal for a ff and enable signal for a latch
   * @param i_data data value
   * @param i_baseGraph Shared pointer to the base graph.
   * @param i_name name of the vertex
   *
   * \~russian
   * @brief Конструктор для последовательностной вершины
   * @param i_type тип последовательностной вершины
   * @param i_clk сигнал синхронизации (clock) для триггера и сигнал
   * разрешения (enable) для защелки
   * @param i_data значение данных
   * @param i_baseGraph Shared-указатель на базовый граф.
   * @param i_name имя вершины
   */
  GraphVertexSequential(SequentialTypes i_type,
                        VertexPtr i_clk,
                        VertexPtr i_data,
                        GraphPtr i_baseGraph,
                        std::string_view i_name,
                        bool i_isBus = false);

  /**
   * \~english
   * @brief Constructor for sequential vertex
   * @param i_type type of sequential vertex
   * @param i_clk clock signal for a ff and enable signal for a latch
   * @param i_data data value
   * @param i_wire RST or CLR or SET or EN
   * @param i_baseGraph Shared pointer to the base graph.
   * @param i_name name of the vertex
   *
   * \~russian
   * @brief Конструктор для последовательностной вершины
   * @param i_type тип последовательностной вершины
   * @param i_clk сигнал синхронизации (clock) для триггера и сигнал
   * разрешения (enable) для защелки
   * @param i_data значение данных
   * @param i_wire RST или CLR или SET или EN
   * @param i_baseGraph Shared-указатель на базовый граф.
   * @param i_name имя вершины
   */
  GraphVertexSequential(SequentialTypes i_type,
                        VertexPtr i_clk,
                        VertexPtr i_data,
                        VertexPtr i_wire,
                        GraphPtr i_baseGraph,
                        std::string_view i_name,
                        bool i_isBus = false);

  /**
   * \~english
   * @brief Constructor for sequential vertex
   * @param i_type type of sequential vertex
   * @param i_clk EN for latch and CLK for ff
   * @param i_data data value
   * @param i_wire1 For `*re`/`*ce` types: EN; otherwise RST/CLR/SET
   * @param i_wire2 For `*re`/`*ce` types: RST/CLR; otherwise SET or EN
   * @param i_baseGraph Shared pointer to the base graph.
   * @param i_name name of the vertex
   *
   * \~russian
   * @brief Конструктор для последовательностной вершины
   * @param i_type тип последовательностной вершины
   * @param i_clk EN для защелки и CLK для триггера
   * @param i_data значение данных
   * @param i_wire1 Для `*re`/`*ce`: EN; иначе RST/CLR/SET
   * @param i_wire2 Для `*re`/`*ce`: RST/CLR; иначе SET или EN
   * @param i_baseGraph Shared-указатель на базовый граф.
   * @param i_name имя вершины
   */
  GraphVertexSequential(SequentialTypes i_type,
                        VertexPtr i_clk,
                        VertexPtr i_data,
                        VertexPtr i_wire1,
                        VertexPtr i_wire2,
                        GraphPtr i_baseGraph,
                        std::string_view i_name,
                        bool i_isBus = false);

  /**
   * \~english
   * @brief Constructor for sequential vertex
   * @param i_type type of Sequential - (a/n/an)ff(r/c)se
   * @param i_clk clock for flip-flop
   * @param i_data data value
   * @param i_rst clear (or reset signal)
   * @param i_set set signal
   * @param i_en enable
   * @param i_baseGraph Shared pointer to the base graph.
   * @param i_name name of the vertex
   *
   * \~russian
   * @brief Конструктор для последовательностной вершины
   * @param i_type тип Sequential - (a/n/an)ff(r/c)se
   * @param i_clk синхронизация для триггера
   * @param i_data значение данных
   * @param i_rst очистка (или сигнал сброса)
   * @param i_set сигнал установки
   * @param i_en разрешение (enable)
   * @param i_baseGraph Shared-указатель на базовый граф.
   * @param i_name имя вершины
   */
  GraphVertexSequential(SequentialTypes i_type,
                        VertexPtr i_clk,
                        VertexPtr i_data,
                        VertexPtr i_rst,
                        VertexPtr i_set,
                        VertexPtr i_en,
                        GraphPtr i_baseGraph,
                        std::string_view i_name,
                        bool i_isBus = false);

  ~GraphVertexSequential() override {};

  // clang-format on

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief calculateHash Calculates the hash value of the vertex.
   * When running for a second time, set hash flags to default state
   * @return The calculated hash value as a number
   * @todo Add example of usage
   *
   * \~russian
   * @brief calculateHash Вычисляет значение хэша вершины. При повторном
   * запуске установите флаги хэша в состояние по умолчанию
   * @return Вычисленное значение хэша в виде числа
   * @todo Добавить пример использования
   */
  size_t calculateHash() override;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief Generates Verilog code for current sequential vertex based on
   * its type and input connections.
   * Returns an empty string when there are no input connections.
   * Throws if any input connection is invalid.
   * @return Verilog string for current vertex.
   * @throws{std::invalid_argument} if any input connection is invalid
   *
   * \~russian
   * @brief Генерирует Verilog-код для текущей последовательностной вершины
   * с учетом ее типа и входных соединений.
   * Если входных соединений нет, возвращается пустая строка.
   * При недействительном входном соединении выбрасывается исключение.
   * @return Строка Verilog для текущей вершины.
   * @throws{std::invalid_argument} если какое-либо входное соединение
   * недействительно
   */
  virtual std::string toVerilog() const override;

  /**
   * \~english
   * @brief Builds a default Verilog instance snippet for this sequential
   * vertex.
   * @return Verilog instance string.
   *
   * \~russian
   * @brief Формирует стандартный Verilog-инстанс для этой
   * последовательностной вершины.
   * @return Строка с Verilog-инстансом.
   */
  virtual std::string getVerilogInstance();
  /**
   * \~english
   * @brief Produces Verilog body for a sequential element from typed inputs.
   * @param i_type Sequential type.
   * @param i_name Instance/register name.
   * @param i_inputs Ordered input names.
   * @return Verilog code string.
   *
   * \~russian
   * @brief Формирует Verilog-код последовательностного элемента по типу и
   * входам.
   * @param i_type Тип последовательностного элемента.
   * @param i_name Имя инстанса/регистра.
   * @param i_inputs Упорядоченный список имен входов.
   * @return Строка с Verilog-кодом.
   */
  static std::string
  getSequentialString(SequentialTypes i_type, std::string_view i_name,
                      std::vector<std::string_view> i_inputs);

  /**
   * \~english
   * @brief Builds Verilog instance from vertex and explicit signal aliases.
   * @param vertex Sequential vertex.
   * @param i_inputDataName Data input net name.
   * @param i_qOutputName Output net name.
   * @param i_dataName Alias for internal data port.
   * @param i_qName Alias for internal output port.
   * @param i_instanceName Optional instance name.
   * @return Verilog instance string.
   *
   * \~russian
   * @brief Формирует Verilog-инстанс по вершине и явным именам сигналов.
   * @param vertex Последовательностная вершина.
   * @param i_inputDataName Имя входной сети данных.
   * @param i_qOutputName Имя выходной сети.
   * @param i_dataName Алиас внутреннего порта данных.
   * @param i_qName Алиас внутреннего выходного порта.
   * @param i_instanceName Необязательное имя инстанса.
   * @return Строка с Verilog-инстансом.
   */
  static std::string getVerilogInstance(const VertexPtr vertex,
                                        std::string_view i_inputDataName,
                                        std::string_view i_qOutputName,
                                        std::string_view i_dataName = "data",
                                        std::string_view i_qName = "q",
                                        std::string_view i_instanceName = "");
  /** @author Fuuulkrum7 */
  DotReturn toDOT() override;

  /**
   * @author Fuuulkrum7
   *
   * \~english
   * @brief updateValue A virtual function for updating the vertex value.
   * The implementation is provided in derived classes
   * @return the value of the vertex after its update
   *
   * \~russian
   * @brief updateValue Виртуальная функция для обновления значения
   * вершины. Реализация предоставляется в производных классах
   * @return значение вершины после ее обновления
   */
  /**
   * \~english
   * @brief Updates sequential state for combinational-style simulation.
   * Latches are level-sensitive on EN; flip-flops capture on a clk edge
   * (posedge by default, negedge if `NEGEDGE` is set), using the previous
   * clock sample stored in the cell. `RST` is active-low, `CLR`/`SET` are
   * active-high. With `ASYNC`, only reset/clear apply immediately; `SET`
   * still waits for the clock edge (matching exported Verilog sensitivity).
   * `EN` is sampled only when capturing (`if (en)`); `EN=X/Z` holds Q.
   * \~russian
   * @brief Обновляет состояние sequential при симуляции.
   * Защёлки чувствительны к уровню EN; триггеры захватывают по фронту clk
   * (posedge по умолчанию, negedge при `NEGEDGE`). `RST` — активный 0,
   * `CLR`/`SET` — активный 1. При `ASYNC` сразу только сброс/очистка;
   * `SET` — по фронту clk. `EN=X/Z` удерживает Q.
   */
  char updateValue() override;

  /**
   * \~english
   * @brief Computes next FF state without committing (NBA-style).
   * Used by `OrientedGraph::graphSimulation` so multi-FF chains sample
   * pre-edge Q. Latches should use `updateValue()` instead.
   * \~russian
   * @brief Считает следующее состояние FF без записи (стиль NBA).
   */
  virtual void stageValue();

  /**
   * \~english
   * @brief Commits state previously computed by `stageValue`.
   * \~russian
   * @brief Записывает состояние, посчитанное в `stageValue`.
   */
  virtual void commitStagedValue();

  /** @author Theossr */
  void removeValue() override;

  /**
   * \~english
   * @brief return true if sequential cell is
   *
   * \~russian
   * @brief возвращает true, если последовательностная ячейка
   */
  /** @author Fuuulkrum7 */
  bool isFF() const;
  /** @author Fuuulkrum7 */
  bool isAsync() const;
  /** @author Fuuulkrum7 */
  bool isNegedge() const;
  /** @author Fuuulkrum7 */
  SequentialTypes getSeqType() const;

  /** @author Fuuulkrum7 */
  VertexPtr getClk() const;
  /** @author Fuuulkrum7 */
  VertexPtr getData() const;
  /** @author Fuuulkrum7 */
  VertexPtr getEn() const;
  /** @author Fuuulkrum7 */
  VertexPtr getRst() const;
  /** @author Fuuulkrum7 */
  VertexPtr getSet() const;

protected:
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  void setSignalByType(VertexPtr i_wire, SequentialTypes i_type,
                       unsigned &factType);

protected:
  SequentialTypes d_seqType;
  /// Previous clock sample for edge detection in `updateValue` (FF only).
  char d_prevClk = ValueStates::NoSignal;
  /// Staged next state for NBA-style FF updates in `graphSimulation`.
  char d_stagedValue = ValueStates::UndefinedState;
  char d_stagedPrevClk = ValueStates::NoSignal;
  bool d_hasStaged = false;
};

/**
 * \~english
 * @brief Bus-specialized input vertex.
 *
 * \~russian
 * @brief Входная вершина, специализированная для шин.
 */
class GraphVertexBusInput : public GraphVertexInput, public GraphVertexBus {
public:
  GraphVertexBusInput(std::string_view i_name, GraphPtr i_baseGraph,
                      size_t i_width);
  /**
   * \~english
   * @brief Converts bus input to one-bit Verilog representation.
   * @return One-bit Verilog code.
   *
   * \~russian
   * @brief Преобразует входную шину в однобитное Verilog-представление.
   * @return Однобитный Verilog-код.
   */
  std::string toOneBitVerilog() const override final;

  //  size_t calculateHash() override final;
  //  std::string updateValueBus() override final;
};
/**
 * \~english
 * @brief Bus-specialized output vertex.
 *
 * \~russian
 * @brief Выходная вершина, специализированная для шин.
 */
class GraphVertexBusOutput : public GraphVertexOutput, public GraphVertexBus {
public:
  GraphVertexBusOutput(std::string_view i_name, GraphPtr i_baseGraph,
                       size_t i_width);
  // size_t calculateHash() override final;
  //  std::string updateValueBus() override final;
  /**
   * \~english
   * @brief Converts bus output to one-bit Verilog representation.
   * @return One-bit Verilog code.
   *
   * \~russian
   * @brief Преобразует выходную шину в однобитное Verilog-представление.
   * @return Однобитный Verilog-код.
   */
  std::string toOneBitVerilog() const override final;
  //
};

class GraphVertexBusConstant :
    public GraphVertexConstant,
    public GraphVertexBus {
public:
  GraphVertexBusConstant(std::string_view i_name, GraphPtr i_baseGraph,
                         size_t i_width);
  // size_t calculateHash() override final;
  // std::string updateValueBus() override final;
  /**
   * \~english
   * @brief Generates Verilog for bus constant vertex.
   * @return Verilog code string.
   *
   * \~russian
   * @brief Генерирует Verilog-код для шинной константы.
   * @return Строка Verilog-кода.
   */
  std::string toVerilog() const override final;
  /**
   * \~english
   * @brief Returns default Verilog instance for bus constant.
   * @return Verilog instance string.
   *
   * \~russian
   * @brief Возвращает стандартный Verilog-инстанс шинной константы.
   * @return Строка с Verilog-инстансом.
   */
  std::string getVerilogInstance() override final;
  /**
   * \~english
   * @brief Returns Verilog instance with separate definition style.
   * @return Verilog instance string.
   *
   * \~russian
   * @brief Возвращает Verilog-инстанс в режиме раздельных определений.
   * @return Строка с Verilog-инстансом.
   */
  std::string getVerilogInstanceSeparate();
  /**
   * \~english
   * @brief Sets bus constant value.
   * @param i_value New bus value literal.
   *
   * \~russian
   * @brief Устанавливает значение шинной константы.
   * @param i_value Новое литеральное значение шины.
   */
  void setValue(std::string i_value);
  /**
   * \~english
   * @brief Converts bus constant to one-bit Verilog representation.
   * @return One-bit Verilog code.
   *
   * \~russian
   * @brief Преобразует шинную константу в однобитное Verilog-представление.
   * @return Однобитный Verilog-код.
   */
  std::string toOneBitVerilog() const override final;

private:
  std::string d_valueBus;
};

/**
 * \~english
 * @brief Gate vertex specialized for bus operations.
 *
 * \~russian
 * @brief Вершина-вентиль, специализированная для операций с шинами.
 */
class GraphVertexBusGate : public GraphVertexGates, public GraphVertexBus {
public:
  GraphVertexBusGate(Gates i_gate, std::string_view i_name,
                     GraphPtr i_baseGraph, size_t i_width);
  //  size_t calculateHash() override;
  //  std::string updateValueBus() override;
  /**
   * \~english
   * @brief Generates Verilog for bus gate vertex.
   * @return Verilog code string.
   *
   * \~russian
   * @brief Генерирует Verilog-код для шинного вентиля.
   * @return Строка Verilog-кода.
   */
  std::string toVerilog() const override;
  /**
   * \~english
   * @brief Converts bus gate to one-bit Verilog representation.
   * @return One-bit Verilog code.
   *
   * \~russian
   * @brief Преобразует шинный вентиль в однобитное Verilog-представление.
   * @return Однобитный Verilog-код.
   */
  std::string toOneBitVerilog() const override;
};

/**
 * \~english
 * @brief Bus slice vertex.
 *
 * \~russian
 * @brief Вершина-срез шины.
 */
class GraphVertexBusSlice : public GraphVertexBusGate {
public:
  GraphVertexBusSlice(std::string_view i_name, GraphPtr i_baseGraph,
                      size_t i_begin, size_t i_width);
  // size_t calculateHash() override final; // может и не понадобится.........
  //  std::string updateValueBus() override final;
  /**
   * \~english
   * @brief Returns textual suffix for slice indices.
   * @return Slice suffix string.
   *
   * \~russian
   * @brief Возвращает строковый суффикс индексов среза.
   * @return Суффикс среза в виде строки.
   */
  std::string getSliceSuffix() const;
  /**
   * \~english
   * @brief Generates Verilog for bus slice.
   * @return Verilog code string.
   *
   * \~russian
   * @brief Генерирует Verilog-код для среза шины.
   * @return Строка Verilog-кода.
   */
  std::string toVerilog() const override final;
  /**
   * \~english
   * @brief Converts bus slice to one-bit Verilog representation.
   * @return One-bit Verilog code.
   *
   * \~russian
   * @brief Преобразует срез шины в однобитное Verilog-представление.
   * @return Однобитный Verilog-код.
   */
  std::string toOneBitVerilog() const override final;

private:
  size_t d_begin;
};

class GraphVertexBusSequential :
    public GraphVertexSequential,
    public GraphVertexBus {
public:
  /**
   * \~english
   * @brief Constructor for default bus sequential types.
   * @param i_type Sequential type (can be only `(n)ff` or `latch=EN`).
   * @param i_clk Clock signal for FF and enable signal for latch.
   * @param i_data Data signal.
   * @param i_baseGraph Shared pointer to the base graph.
   * @param i_name Vertex name.
   *
   * \~russian
   * @brief Конструктор для базовых последовательностных шинных типов.
   * @param i_type Тип sequential (только `(n)ff` или `latch=EN`).
   * @param i_clk Сигнал clock для FF и enable для latch.
   * @param i_data Сигнал данных.
   * @param i_baseGraph Shared-указатель на базовый граф.
   * @param i_name Имя вершины.
   */
  GraphVertexBusSequential(SequentialTypes i_type, VertexPtr i_clk,
                           VertexPtr i_data, GraphPtr i_baseGraph,
                           std::string_view i_name, size_t i_width);

  /**
   * \~english
   * @brief Constructor with one additional control wire.
   * @param i_type Sequential type.
   * @param i_clk Clock signal for FF and enable signal for latch.
   * @param i_data Data signal.
   * @param i_wire RST or CLR or SET or EN.
   * @param i_baseGraph Shared pointer to the base graph.
   * @param i_name Vertex name.
   *
   * \~russian
   * @brief Конструктор с одним дополнительным управляющим сигналом.
   * @param i_type Тип sequential.
   * @param i_clk Сигнал clock для FF и enable для latch.
   * @param i_data Сигнал данных.
   * @param i_wire RST или CLR или SET или EN.
   * @param i_baseGraph Shared-указатель на базовый граф.
   * @param i_name Имя вершины.
   */
  GraphVertexBusSequential(SequentialTypes i_type, VertexPtr i_clk,
                           VertexPtr i_data, VertexPtr i_wire,
                           GraphPtr i_baseGraph, std::string_view i_name,
                           size_t i_width);

  /**
   * \~english
   * @brief Constructor with two additional control wires.
   * @param i_type Sequential type.
   * @param i_clk EN for latch and CLK for FF.
   * @param i_data Data signal.
   * @param i_wire1 For `*re`/`*ce` types: EN; otherwise RST/CLR/SET.
   * @param i_wire2 For `*re`/`*ce` types: RST/CLR; otherwise SET or EN.
   * @param i_baseGraph Shared pointer to the base graph.
   *
   * \~russian
   * @brief Конструктор с двумя дополнительными управляющими сигналами.
   * @param i_type Тип sequential.
   * @param i_clk EN для latch и CLK для FF.
   * @param i_data Сигнал данных.
   * @param i_wire1 Для `*re`/`*ce`: EN; иначе RST/CLR/SET.
   * @param i_wire2 Для `*re`/`*ce`: RST/CLR; иначе SET или EN.
   * @param i_baseGraph Shared-указатель на базовый граф.
   */
  GraphVertexBusSequential(SequentialTypes i_type, VertexPtr i_clk,
                           VertexPtr i_data, VertexPtr i_wire1,
                           VertexPtr i_wire2, GraphPtr i_baseGraph,
                           std::string_view i_name, size_t i_width);

  /**
   * \~english
   * @brief Constructor for FF types with `rst`, `set`, and `en`.
   * @param i_type Sequential type like `(a/n/an)ff(r/c)se`.
   * @param i_clk Clock for flip-flop.
   * @param i_data Data signal.
   * @param i_rst Clear/reset signal.
   * @param i_set Set signal.
   * @param i_en Enable signal.
   * @param i_baseGraph Shared pointer to the base graph.
   *
   * \~russian
   * @brief Конструктор для FF-типов с `rst`, `set` и `en`.
   * @param i_type Тип sequential вида `(a/n/an)ff(r/c)se`.
   * @param i_clk Сигнал clock для flip-flop.
   * @param i_data Сигнал данных.
   * @param i_rst Сигнал сброса/очистки.
   * @param i_set Сигнал установки.
   * @param i_en Сигнал разрешения.
   * @param i_baseGraph Shared-указатель на базовый граф.
   */
  GraphVertexBusSequential(SequentialTypes i_type, VertexPtr i_clk,
                           VertexPtr i_data, VertexPtr i_rst, VertexPtr i_set,
                           VertexPtr i_en, GraphPtr i_baseGraph,
                           std::string_view i_name, size_t i_width);

  //  size_t calculateHash() override final;
  char updateValue() override;
  void commitStagedValue() override;
  void removeValue() override;
  /**
   * \~english
   * @brief Generates Verilog for bus sequential vertex.
   * @return Verilog code string.
   *
   * \~russian
   * @brief Генерирует Verilog-код для шинной последовательностной вершины.
   * @return Строка Verilog-кода.
   */
  std::string toVerilog() const override final;
  /**
   * \~english
   * @brief Converts bus sequential vertex to one-bit Verilog representation.
   * @return One-bit Verilog code.
   *
   * \~russian
   * @brief Преобразует шинную последовательностную вершину в однобитное
   * Verilog-представление.
   * @return Однобитный Verilog-код.
   */
  std::string toOneBitVerilog() const override final;
};
/**
 * @struct VerilogPorts
 * \~english
 * @brief Container with parsed Verilog module ports.
 *
 * \~russian
 * @brief Контейнер с распарсенными портами Verilog-модуля.
 */
struct VerilogPorts {
  /**
   * \~english
   * @brief Names of input ports.
   *
   * \~russian
   * @brief Имена входных портов.
   */
  std::vector<std::string> inputs;

  /**
   * \~english
   * @brief Names of output ports.
   *
   * \~russian
   * @brief Имена выходных портов.
   */
  std::vector<std::string> outputs;
};

/**
 * \~english
 * @brief Parses a Verilog file and extracts input/output port names.
 * Supports both ANSI-style module headers and separate input/output
 * declarations. Ignores Verilog type keywords (wire, reg, logic, signed,
 * unsigned, etc.).
 * @param filepath Path to the Verilog file.
 * @return Parsed Verilog ports.
 * @throws{std::runtime_error} if the file cannot be opened.
 *
 * \~russian
 * @brief Разбирает Verilog-файл и извлекает имена входных и выходных портов.
 * Поддерживает ANSI-описание портов в заголовке модуля и отдельные
 * объявления input/output. Игнорирует ключевые слова типов Verilog
 * (wire, reg, logic, signed, unsigned и т.д.).
 * @param filepath Путь к Verilog-файлу.
 * @return Распарсенные порты Verilog.
 * @throws{std::runtime_error} если файл не удалось открыть.
 */
VerilogPorts parseVerilogPorts(const std::string &filepath);

/**
 * \~english
 * @brief Checks whether graph ports match parsed Verilog ports.
 * @param graph Graph whose input and output ports are checked.
 * @param verilogPorts Parsed Verilog ports for comparison.
 * @param errorMsg Output parameter for error description on mismatch;
 * cleared on success.
 * @return true if ports match; otherwise false.
 *
 * \~russian
 * @brief Проверяет совпадение портов графа с портами, распарсенными из
 * Verilog.
 * @param graph Граф, у которого проверяются входные и выходные порты.
 * @param verilogPorts Распарсенные Verilog-порты для сравнения.
 * @param errorMsg Выходной параметр с описанием ошибки при несовпадении;
 * при успехе очищается.
 * @return true, если порты совпадают; иначе false.
 */
bool checkPortsMatch(const GraphPtr &graph, const VerilogPorts &verilogPorts,
                     std::string &errorMsg);

} // namespace CG_Graph
