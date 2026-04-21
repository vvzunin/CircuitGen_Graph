/**
 * @file GraphVertex.hpp
 * @brief Объявления вершин графа: GraphVertexInput, GraphVertexOutput,
 * GraphVertexGates, GraphVertexConstant, GraphVertexSubGraph,
 * GraphVertexSequential.
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 * @author Theossr <feolab05@gmail.com>
 * @author rainbowkittensss <viktorrrrry20@gmail.com>
 * @author Чернявских Илья Игоревич <fuuulkrum7@gmail.com>
 */
#pragma once
#include <string>
#include <vector>
#include <CircuitGenGraph/GraphVertexBase.hpp>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CG_Graph {

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
   * the provided base graph pointer and type
   * @param i_baseGraph Pointer to the base graph.
   * @param i_type i_type Type of the vertex. Default is VertexTypes::input.
   *
   * \~russian
   * @brief GraphVertexInput Инициализирует объект GraphVertexInput с
   * предоставленным указателем на базовый граф и типом
   * @param i_baseGraph Указатель на базовый граф.
   * @param i_type Тип вершины. По умолчанию VertexTypes::input.
   */
  GraphVertexInput(GraphPtr i_baseGraph,
                   const VertexTypes i_type = VertexTypes::input);

  /**
   * \~english
   * @brief GraphVertexInput Initializes the GraphVertexInput object with
   * the provided name, base graph pointer, and type
   * @param i_name Name of the vertex.
   * @param i_baseGraph Pointer to the base graph.
   * @param i_type Type of the vertex.
   *
   * \~russian
   * @brief GraphVertexInput Инициализирует объект GraphVertexInput с
   * предоставленным именем, указателем на базовый граф и типом
   * @param i_name Имя вершины.
   * @param i_baseGraph Указатель на базовый граф.
   * @param i_type Тип вершины.
   */
  GraphVertexInput(std::string_view i_name, GraphPtr i_baseGraph,
                   const VertexTypes i_type = VertexTypes::input);

  /**
   * @author Theossr <feolab05@gmail.com>
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
   * @author Vladimir Zunin <vzunin@hse.ru>
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

  /** @author Theossr <feolab05@gmail.com> */
  virtual void removeValue() override;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   * \~english
   * @brief updateLevel This method updates the level of the vertex based
   * on the levels of its input connections. It iterates through each input
   * connection and sets the vertex level to the maximum level of its input
   * connections plus one.
   * If you are going to call this method for a second time, please, set
   * all flags, used in updateLevel to their default state.
   *
   * \~russian
   * @brief updateLevel Этот метод обновляет уровень вершины на основе
   * уровней ее входных соединений. Он итерируется по каждому входному
   * соединению и устанавливает уровень вершины равным максимальному
   * уровню ее входных соединений плюс один.
   * Если вы собираетесь вызвать этот метод во второй раз, пожалуйста,
   * установите все флаги, используемые в updateLevel, в их состояние по
   * умолчанию.
   */
  virtual void updateLevel() override;

  /**
   * @author Vladimir Zunin <vzunin@hse.ru>
   * \~english
   * @brief writes vertex to dot
   *
   * \~russian
   * @brief записывает вершину в формат dot
   */
  DotReturn toDOT() override;

#ifdef LOGFLAG
  /**
   * @author Vladimir Zunin <vzunin@hse.ru>
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
   * base graph pointer and char symbol
   * @param i_const char symbol for constant (1, 0, z, x)
   * @param i_baseGraph Pointer to the base graph.
   *
   * \~russian
   * @brief Инициализирует объект GraphVertexConstant с предоставленным
   * указателем на базовый граф и символом char
   * @param i_const символ char для константы (1, 0, z, x)
   * @param i_baseGraph Указатель на базовый граф.
   */
  GraphVertexConstant(char i_const, GraphPtr i_baseGraph);

  /**
   * \~english
   * @brief Initializes the GraphVertexConstant object with the provided
   * base graph pointer and char symbol
   * @param i_const char symbol for constant (1, 0, z, x)
   * @param i_name Name of the vertex.
   * @param i_baseGraph Pointer to the base graph.
   *
   * \~russian
   * @brief Инициализирует объект GraphVertexConstant с предоставленным
   * указателем на базовый граф и символом char
   * @param i_const символ char для константы (1, 0, z, x)
   * @param i_name Имя вершины.
   * @param i_baseGraph Указатель на базовый граф.
   */
  GraphVertexConstant(char i_const, std::string_view i_name,
                      GraphPtr i_baseGraph);

  ~GraphVertexConstant() override {};

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief calculates hash for constant.
   * Calculates the hash value for the vertex based on its outgoing
   * connections.
   * When running for a second time, set hash flags to default state
   * @return The hash value of the vertex based on its outgoing connections.
   * @par Example
   * @code
   * // Creating an instance of the GraphVertexBase class
   * GraphVertexBase vertex(VertexTypes::output, "vertex1");
   * // Creating two more vertices
   * VertexPtr vertex2 = std::make_shared<GraphVertexBase>(
   * VertexTypes::input, "vertex2");
   * VertexPtr vertex3 = std::make_shared<GraphVertexBase>(
   * VertexTypes::input, "vertex3");
   * // Adding the second and third vertices to the output connections of the
   * // first vertex
   * vertex.addVertexToOutConnections(vertex2);
   * vertex.addVertexToOutConnections(vertex3);
   * // Calculating the hash for the first vertex
   * std::string hashValue = vertex.calculateHash();
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
   * @author Fuuulkrum7 <ilka747428@gmail.com>
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
  std::string toVerilog() const override;

  /** @author Vladimir Zunin <vzunin@hse.ru> */
  DotReturn toDOT() override;

  /**
   * @author Vladimir Zunin <vzunin@hse.ru>
   *
   * \~english
   * @brief getDefaultInstance Creates simple verilog const instance
   * (as a wire)
   *
   * \~russian
   * @brief getDefaultInstance Создает простой экземпляр константы
   * verilog (как wire)
   */
  std::string getVerilogInstance();

#ifdef LOGFLAG
  /**
   * @author Vladimir Zunin <vzunin@hse.ru>
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
 * @class GraphVertexSubGraph
 * \~english
 * @brief It is a class, having a subGruph pointer inside. Is used for
 * storing this pointer for providing graph connectivity.
 *
 * \~russian
 * @brief Это класс, содержащий внутри указатель на подграф (subGraph).
 * Используется для хранения этого указателя с целью обеспечения связности
 * графа.
 */
class GraphVertexSubGraph : public GraphVertexBase {
public:
  /**
   * \~english
   * @brief Initializes the GraphVertexSubGraph object with the provided
   * base graph pointer and subGraph pointer
   * @param i_subGraph Pointer to the subGraph.
   * @param i_baseGraph Pointer to the base graph.
   *
   * \~russian
   * @brief Инициализирует объект GraphVertexSubGraph с предоставленным
   * указателем на базовый граф и указателем на подграф
   * @param i_subGraph Указатель на подграф.
   * @param i_baseGraph Указатель на базовый граф.
   */
  GraphVertexSubGraph(GraphPtr i_subGraph, GraphPtr i_baseGraph);
  GraphVertexSubGraph(GraphPtr i_subGraph, std::string_view i_name,
                      GraphPtr i_baseGraph);

  ~GraphVertexSubGraph() override {};

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
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

  /** @author Theossr <feolab05@gmail.com> */
  void removeValue() override;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief updateLevel
   * This method updates the level of the vertex based on the levels of its
   * input connections. It iterates through each input connection and sets
   * the vertex level to the maximum level of its input connections plus
   * one. If you are going to call this method for a second time, please,
   * set all flags, used in updateLevel to their default state.
   *
   * \~russian
   * @brief updateLevel
   * Этот метод обновляет уровень вершины на основе уровней ее входных
   * соединений. Он итерируется по каждому входному соединению и
   * устанавливает уровень вершины равным максимальному уровню ее входных
   * соединений плюс один. Если вы собираетесь вызвать этот метод во второй
   * раз, пожалуйста, установите все флаги, используемые в updateLevel,
   * в их состояние по умолчанию.
   */
  void updateLevel() override;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
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

  /** @author Vladimir Zunin <vzunin@hse.ru> */
  DotReturn toDOT() override;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief This method is used as a substructure for OrientedGraph methods
   * @param i_path path to future file storing. Do not add submodule here,
   * folder would be created there
   * @param i_filename name of file to be created (default is same as
   * graph name)
   * @return bool, meaning was file writing successful or not
   *
   * \~russian
   * @brief Этот метод используется как подструктура для методов
   * OrientedGraph
   * @param i_path путь для будущего сохранения файла. Не добавляйте сюда
   * подмодуль, папка будет создана там
   * @param i_filename имя создаваемого файла (по умолчанию совпадает с
   * именем графа)
   * @return bool, означающий, была ли запись файла успешной или нет
   */
  bool toVerilog(std::string i_path, std::string i_filename = "");

  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  bool toDOT(std::string i_path, std::string i_filename = "");

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief This method is used as a substructure for OrientedGraph methods
   *
   * \~russian
   * @brief Этот метод используется как подструктура для методов
   * OrientedGraph
   */
  bool toGraphML(std::ofstream &i_fileStream) const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief This method is used as a substructure for OrientedGraph
   * methods
   * @param i_indent Indentation level
   * @param i_prefix Prefix string
   * @return Formatted GraphML string
   *
   * \~russian
   * @brief Этот метод используется как подструктура для методов
   * OrientedGraph
   * @param i_indent Уровень отступа
   * @param i_prefix Строка префикса
   * @return Отформатированная строка GraphML
   */
  std::string toGraphML(uint16_t i_indent = 0, std::string i_prefix = "") const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief This method is used as a substructure for OrientedGraph
   * methods. When running it for a second time, clear hash flags
   * @return Hash value
   *
   * \~russian
   * @brief Этот метод используется как подструктура для методов
   * OrientedGraph. При повторном запуске очистите флаги хэша
   * @return Значение хэша
   */
  size_t calculateHash() override;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief sets new subgraph to the vertex
   *
   * \~russian
   * @brief устанавливает новый подграф для вершины
   */
  void setSubGraph(GraphPtr i_subGraph);

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @return pointer to subgraph, being stored in vertex
   *
   * \~russian
   * @return указатель на подграф, хранящийся в вершине
   */
  GraphPtr getSubGraph() const;

  /**
   * \~english
   * @brief returns all vertices of outputs, which are influenced by given
   * input to vertex
   * @param i_outerInput Input vertex
   * @return Vector of output vertices
   *
   * \~russian
   * @brief возвращает все вершины выходов, на которые влияет заданный вход
   * в вершину
   * @param i_outerInput Входная вершина
   * @return Вектор выходных вершин
   */
  std::vector<VertexPtr>
  getOutputBuffersByOuterInput(VertexPtr i_outerInput) const;

  /**
   * \~english
   * @brief returns all vertices of inputs, which are influenced by given
   * output to vertex
   * @param i_outputBuffer Output buffer vertex
   * @return Vector of input vertices
   *
   * \~russian
   * @brief возвращает все вершины входов, на которые влияет заданный выход
   * в вершину
   * @param i_outputBuffer Вершина выходного буфера
   * @return Вектор входных вершин
   */
  std::vector<VertexPtr>
  getOuterInputsByOutputBuffer(VertexPtr i_outputBuffer) const;

#ifdef LOGFLAG
  /**
   * @author Vladimir Zunin <vzunin@hse.ru>
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
  GraphVertexOutput(GraphPtr i_baseGraph);

  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  GraphVertexOutput(std::string_view i_name, GraphPtr i_baseGraph);

  /**
   * @author Theossr <feolab05@gmail.com>
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
   * @author Fuuulkrum7 <ilka747428@gmail.com>
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

  /** @author Vladimir Zunin <vzunin@hse.ru> */
  DotReturn toDOT() override;

#ifdef LOGFLAG
  /**
   * @author Vladimir Zunin <vzunin@hse.ru>
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
  GraphVertexGates(Gates i_gate, GraphPtr i_baseGraph);

  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  GraphVertexGates(Gates i_gate, std::string_view i_name, GraphPtr i_baseGraph);

  ~GraphVertexGates() override {};

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief updateValue Updates the value of the vertex
   * @return The updated value of the vertex
   * @todo Add example of usage
   * @throws std::invalid_argument if any of the input connections point
   * to a nullptr
   *
   * \~russian
   * @brief updateValue Обновляет значение вершины
   * @return Обновленное значение вершины
   * @todo Добавить пример использования
   * @throws std::invalid_argument, если любое из входных соединений
   * указывает на nullptr
   */
  virtual char updateValue() override;

  /** @author Theossr <feolab05@gmail.com> */
  void removeValue() override;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
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
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief getVerilogString Gets a string in Verilog format representing
   * the current vertex
   * @return A string in Verilog format representing the current vertex
   * @throws std::invalid_argument if any input connection is invalid
   *
   * \~russian
   * @brief getVerilogString Получает строку в формате Verilog,
   * представляющую текущую вершину
   * @return Строка в формате Verilog, представляющая текущую вершину
   * @throws std::invalid_argument, если какое-либо входное соединение
   * недействительно
   */
  std::string getVerilogString() const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief getGate Returns the type of valve associated with the current
   * vertex
   * @return The type of valve associated with the current vertex
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
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief addVertexToInConnections
   * Buffer and Not types of gates must have only one element in
   * d_inConnections, so realization for GraphVertexGates has a check
   * before adding
   * @param i_vert Vertex that will be added to d_inConnections of this
   * @return The count of occurrences of the given vertex in the input
   * connections after adding it
   * @throws std::overflow_error in case of connecting more than one
   * vertex in d_inConnections
   *
   * \~russian
   * @brief addVertexToInConnections
   * Типы вентилей Буфер (Buffer) и НЕ (Not) должны иметь только один
   * элемент в d_inConnections, поэтому реализация для GraphVertexGates
   * содержит проверку перед добавлением
   * @param i_vert Вершина, которая будет добавлена в d_inConnections
   * этой вершины
   * @return Количество вхождений данной вершины во входные соединения
   * после ее добавления
   * @throws std::overflow_error в случае подключения более одной
   * вершины в d_inConnections
   */
  uint32_t addVertexToInConnections(VertexPtr i_vert) override;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief toVerilog generates a string in Verilog format for the current
   * vertex, representing the valve according to its type and input
   * connections. If a vertex has no input connections, an empty string
   * is returned. If any input connection is invalid, an exception is
   * thrown.
   * @return A Verilog format string for the current vertex
   * @throws std::invalid_argument if any input connection is invalid
   *
   * \~russian
   * @brief toVerilog генерирует строку в формате Verilog для текущей
   * вершины, представляющую вентиль в соответствии с его типом и входными
   * соединениями. Если у вершины нет входных соединений, возвращается
   * пустая строка. Если какое-либо входное соединение недействительно,
   * выбрасывается исключение.
   * @return Строка в формате Verilog для текущей вершины
   * @throws std::invalid_argument, если какое-либо входное соединение
   * недействительно
   */
  std::string toVerilog() const override;

  /** @author Vladimir Zunin <vzunin@hse.ru> */
  DotReturn toDOT() override;

  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  bool isSubgraphBuffer() const override;

#ifdef LOGFLAG
  /**
   * @author Vladimir Zunin <vzunin@hse.ru>
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
   * @param i_baseGraph pointer to the base graph
   * @param i_name name of the vertex
   *
   * \~russian
   * @brief Конструктор для последовательностной вершины
   * @param i_type тип последовательностной вершины
   * @param i_clk сигнал синхронизации (clock) для триггера и сигнал
   * разрешения (enable) для защелки
   * @param i_data значение данных
   * @param i_baseGraph указатель на базовый граф
   * @param i_name имя вершины
   */
  GraphVertexSequential(SequentialTypes i_type,
                        VertexPtr i_clk,
                        VertexPtr i_data,
                        GraphPtr i_baseGraph,
                        std::string_view i_name);

  /**
   * \~english
   * @brief Constructor for sequential vertex
   * @param i_type type of sequential vertex
   * @param i_clk clock signal for a ff and enable signal for a latch
   * @param i_data data value
   * @param i_wire RST or CLR or SET or EN
   * @param i_baseGraph pointer to the base graph
   * @param i_name name of the vertex
   *
   * \~russian
   * @brief Конструктор для последовательностной вершины
   * @param i_type тип последовательностной вершины
   * @param i_clk сигнал синхронизации (clock) для триггера и сигнал
   * разрешения (enable) для защелки
   * @param i_data значение данных
   * @param i_wire RST или CLR или SET или EN
   * @param i_baseGraph указатель на базовый граф
   * @param i_name имя вершины
   */
  GraphVertexSequential(SequentialTypes i_type,
                        VertexPtr i_clk,
                        VertexPtr i_data,
                        VertexPtr i_wire,
                        GraphPtr i_baseGraph,
                        std::string_view i_name);

  /**
   * \~english
   * @brief Constructor for sequential vertex
   * @param i_type type of sequential vertex
   * @param i_clk EN for latch and CLK for ff
   * @param i_data data value
   * @param i_wire1 RST or CLR or SET
   * @param i_wire2 SET or EN
   * @param i_baseGraph pointer to the base graph
   * @param i_name name of the vertex
   *
   * \~russian
   * @brief Конструктор для последовательностной вершины
   * @param i_type тип последовательностной вершины
   * @param i_clk EN для защелки и CLK для триггера
   * @param i_data значение данных
   * @param i_wire1 RST или CLR или SET
   * @param i_wire2 SET или EN
   * @param i_baseGraph указатель на базовый граф
   * @param i_name имя вершины
   */
  GraphVertexSequential(SequentialTypes i_type,
                        VertexPtr i_clk,
                        VertexPtr i_data,
                        VertexPtr i_wire1,
                        VertexPtr i_wire2,
                        GraphPtr i_baseGraph,
                        std::string_view i_name);

  /**
   * \~english
   * @brief Constructor for sequential vertex
   * @param i_type type of Sequential - (a/n/an)ff(r/c)se
   * @param i_clk clock for flip-flop
   * @param i_data data value
   * @param i_rst clear (or reset signal)
   * @param i_set set signal
   * @param i_en enable
   * @param i_baseGraph pointer to the base graph
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
   * @param i_baseGraph указатель на базовый граф
   * @param i_name имя вершины
   */
  GraphVertexSequential(SequentialTypes i_type,
                        VertexPtr i_clk,
                        VertexPtr i_data,
                        VertexPtr i_rst,
                        VertexPtr i_set,
                        VertexPtr i_en,
                        GraphPtr i_baseGraph,
                        std::string_view i_name);

  // clang-format on

  ~GraphVertexSequential() override {};

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief calculateHash Calculates the hash value of the vertex.
   * When running for a second time, set hash flags to default state
   * @return The calculated hash value as a number
   * @throws None.
   * @todo Add example of usage
   *
   * \~russian
   * @brief calculateHash Вычисляет значение хэша вершины. При повторном
   * запуске установите флаги хэша в состояние по умолчанию
   * @return Вычисленное значение хэша в виде числа
   * @throws Нет.
   * @todo Добавить пример использования
   */
  size_t calculateHash() override;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief toVerilog generates a string in Verilog format for the
   * current vertex, representing the valve according to its type and
   * input connections. If a vertex has no input connections, an empty
   * string is returned. If any input connection is invalid, an
   * exception is thrown.
   * @return A Verilog format string for the current vertex
   * @throws std::invalid_argument if any input connection is invalid
   *
   * \~russian
   * @brief toVerilog генерирует строку в формате Verilog для текущей
   * вершины, представляющую вентиль в соответствии с его типом и
   * входными соединениями. Если у вершины нет входных соединений,
   * возвращается пустая строка. Если какое-либо входное соединение
   * недействительно, выбрасывается исключение.
   * @return Строка в формате Verilog для текущей вершины
   * @throws std::invalid_argument, если какое-либо входное соединение
   * недействительно
   */
  std::string toVerilog() const override;

  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  DotReturn toDOT() override;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
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
  char updateValue() override { return ValueStates::FalseValue; };

  /**
   * \~english
   * @brief return true if sequential cell is
   *
   * \~russian
   * @brief возвращает true, если последовательностная ячейка
   */
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  bool isFF() const;
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  bool isAsync() const;
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  bool isNegedge() const;
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  SequentialTypes getSeqType() const;

  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  VertexPtr getClk() const;
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  VertexPtr getData() const;
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  VertexPtr getEn() const;
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  VertexPtr getRst() const;
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  VertexPtr getSet() const;

private:
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  void setSignalByType(VertexPtr i_wire, SequentialTypes i_type,
                       unsigned &factType);
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  void formatAlwaysBegin(std::string &verilog) const;

private:
  SequentialTypes d_seqType;

  VertexPtr d_clk = nullptr;
  VertexPtr d_data = nullptr;
  VertexPtr d_en = nullptr;
  VertexPtr d_rst = nullptr;
  VertexPtr d_set = nullptr;
};

} // namespace CG_Graph
