/**
 * @file OrientedGraph.hpp
 * @brief Ориентированный граф схемы: вершины, связи, экспорт в
 * Verilog/DOT/GraphML.
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 * @author Theossr <feolab05@gmail.com>
 * @author rainbowkittensss <viktorrrrry20@gmail.com>
 * @author Andrey <shapkin.andrey123@gmail.com>
 * @author Чернявских Илья Игоревич <fuuulkrum7@gmail.com>
 * @author NonDif <shapkin.andrey123@gmail.com>
 */
#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <ctime>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include <CircuitGenGraph/enums.hpp>
#include <CircuitGenGraph/GraphMemory.hpp>
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/GraphUtils.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>
#include <CircuitGenGraph/GraphVertexBus.hpp>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

/**
 * \~english
 * @todo Add checking for file names when adding new vertices
 *
 * \~russian
 * @todo Добавить проверку на имена файлов при добавлении новых вершин
 */

#define GraphPtr std::shared_ptr<CG_Graph::OrientedGraph>
#define GraphPtrWeak std::weak_ptr<CG_Graph::OrientedGraph>
#define VertexPtr CG_Graph::GraphVertexBase *

namespace CG_Graph {

class GraphVertexBase;
class GraphReader;
class Context;
class GraphVertexBus;

/**
 * @class CG_Graph::OrientedGraph
 *
 * \~english
 * @brief Main class representing a directed graph
 * - d_countNewGraphInstance Static variable to count new graph instances
 * - d_countGraph Current instance counter. Is used for setting a graphID
 * - d_currentParentGraph Weak pointer to the current parent graph
 * - d_edgesCount The number of edges in the graph
 * - d_hashed Hashed value of the graph
 * - d_parentGraphs Vector of weak pointers to parent graphs
 * - d_name The name of the graph
 * - d_needLevelUpdate A flag indicating whether the vertex levels in the graph
 * need to be updated
 * - d_graphInstanceToVerilogCount Map to count instances to Verilog.
 * - d_graphInstanceToDotCount Map to count instances to DOT.
 * - d_allSubGraphsOutputs Vector storing all outputs of subgraphs
 * - d_subGraphs Set of subgraphs.
 * - d_vertices Map of vertex types to vectors of vertex pointers
 * - d_countGraph Static counter for the total number of graphs
 * - d_gatesCount Map for quick gates count.
 * - d_edgesGatesCount Map for quick count of edges of gate type.
 *
 * \~russian
 * @brief Главный класс, представляющий ориентированный граф
 * - d_countNewGraphInstance Статическая переменная для подсчета новых
 * экземпляров графа
 * - d_countGraph Текущий счетчик экземпляров. Используется для установки
 * graphID
 * - d_currentParentGraph Слабый указатель (weak pointer) на текущий
 * родительский граф
 * - d_edgesCount Количество ребер в графе
 * - d_hashed Хэшированное значение графа
 * - d_parentGraphs Вектор слабых указателей на родительские графы
 * - d_name Имя графа
 * - d_needLevelUpdate Флаг, указывающий, нужно ли обновлять уровни вершин в
 * графе
 * - d_graphInstanceToVerilogCount Словарь для подсчета экземпляров,
 * преобразованных в Verilog.
 * - d_graphInstanceToDotCount Словарь для подсчета экземпляров, преобразованных
 * в DOT.
 * - d_allSubGraphsOutputs Вектор, хранящий все выходы подграфов
 * - d_subGraphs Множество подграфов.
 * - d_vertices Словарь, сопоставляющий типы вершин с векторами указателей на
 * вершины
 * - d_countGraph Статический счетчик общего количества графов
 * - d_gatesCount Словарь для быстрого подсчета вентилей.
 * - d_edgesGatesCount Словарь для быстрого подсчета ребер между типами
 * вентилей.
 */

// clang-format off
class OrientedGraph
    : public GraphMemory
    , public std::enable_shared_from_this<OrientedGraph>
#ifdef LOGFLAG
    , public el::Loggable
#endif
{
  // clang-format on
public:
  OrientedGraph(const std::string &i_name = "",
                size_t buffer_size = DEFAULT_BUF,
                size_t chunk_size = CHUNK_SIZE);

  using GraphID = std::size_t;

  /**
   * \~english
   * @todo Add the use of gates_inputs_info.
   *
   * \~russian
   * @todo Добавить использование gates_inputs_info.
   */

  ~OrientedGraph();

  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  OrientedGraph &operator=(const OrientedGraph &other) = delete;
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  OrientedGraph &operator=(OrientedGraph &&other) = delete;
  OrientedGraph(const OrientedGraph &other) = delete;
  OrientedGraph(OrientedGraph &&other) = delete;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief baseSize returns the number of "gate" type vertices in the
   * graph
   * @note Number of gates in the graph, excluding subgraphs
   * @return An integer value representing the number of "gate" type
   * vertices in the graph
   *
   * \~russian
   * @brief baseSize возвращает количество вершин типа "gate" в графе
   * @note Количество gate в графе, за исключением подграфов
   * @return Целочисленное значение, представляющее количество вершин типа
   * "gate" в графе
   */
  size_t baseSize() const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief fullSize returns the total number of vertices in the graph,
   * including vertices from all subgraphs. It recursively traverses all
   * the subgraphs and sums up the number of vertices in each of them
   * @note Number of gates in the graph, including subgraphs
   * @return An integer value representing the total number of vertices in
   * the graph, including vertices from all subgraphs
   *
   * \~russian
   * @brief fullSize возвращает общее количество вершин в графе,
   * включая вершины из всех подграфов. Он рекурсивно обходит все
   * подграфы и суммирует количество вершин в каждом из них
   * @note Количество gate в графе, включая подграфы
   * @return Целочисленное значение, представляющее общее количество вершин
   * в графе, включая вершины из всех подграфов
   */
  size_t fullSize() const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief sumFullSize returns the total number of vertices of all types
   * in the graph, including input vertices, constants, gates, and output
   * vertices
   * @note sum of gates, inputs, outputs and consts sizes
   * @return the size of all vertices in the graph, summing the number of
   * vertices of each type
   *
   * \~russian
   * @brief sumFullSize возвращает общее количество вершин всех типов
   * в графе, включая входные вершины, константы, вентили и выходные
   * вершины
   * @note сумма размеров вентилей, входов, выходов и констант
   * @return размер всех вершин в графе, путем суммирования количества
   * вершин каждого типа
   */
  size_t sumFullSize() const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief Checks, if there are any gates in graph (including subrapgs).
   * In fact checks, if fullSize is equal to zero
   * @note Are there any gates in the circuit, including subgraphs
   * @return true if empty, false otherwise
   *
   * \~russian
   * @brief Проверяет, есть ли какие-либо вентили в графе (включая
   * подграфы). Фактически проверяет, равен ли fullSize нулю
   * @note Имеются ли gate в схеме, включая подграфы
   * @return true если пуст, false в противном случае
   */
  bool isEmpty() const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief Resets all internal states for all types of vertices
   *
   * \~russian
   * @brief Сбрасывает все внутренние состояния для всех типов вершин
   */
  void clearAllStates();

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief Resets hash states for all vertices of current graph (including
   * subgraphs). Should be called before each hash recalculation
   *
   * \~russian
   * @brief Сбрасывает состояния хэша для всех вершин текущего графа
   * (включая подграфы). Должен вызываться перед каждым пересчетом хэша
   */
  void clearHashStates();

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief Resets update for all vertices of current graph (including
   * subgraphs). Should be called before each level recalculation
   *
   * \~russian
   * @brief Сбрасывает состояние обновления для всех вершин текущего графа
   * (включая подграфы). Должен вызываться перед каждым пересчетом уровня
   */
  void clearNeedUpdateStates();

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief Sets for all vertices state, that shows, that all vertices
   * were not used for for `getVerticesByLevel`. Is called by
   * `getVerticesByLevel`.
   *
   * \~russian
   * @brief Устанавливает для всех вершин состояние, показывающее, что все
   * вершины не использовались для `getVerticesByLevel`. Вызывается методом
   * `getVerticesByLevel`.
   */
  void clearUsedLevelStates();

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief isEmptyFull It is used to check the emptiness of a graph,
   * including all its subgraphs. It recursively traverses all the subgraphs
   * and checks if they are empty
   * @return bool - true if the graph and all its subgraphs are empty, and
   * false if at least one of them contains vertice
   *
   * \~russian
   * @brief isEmptyFull Используется для проверки пустоты графа,
   * включая все его подграфы. Рекурсивно обходит все подграфы
   * и проверяет, пусты ли они
   * @return bool - true, если граф и все его подграфы пусты, и false,
   * если хотя бы один из них содержит вершины
   */
  bool isEmptyFull() const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief setName Used to set the name of the graph. It takes a string as
   * an argument and sets the d_name field to this string
   * @param i_name the new name of the graph
   *
   * \~russian
   * @brief setName Используется для установки имени графа. Принимает строку
   * в качестве аргумента и устанавливает поле d_name в эту строку
   * @param i_name новое имя графа
   */
  void setName(const std::string &i_name);

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief getName Used to get the name of the graph
   * @return the name of the graph
   *
   * \~russian
   * @brief getName Используется для получения имени графа
   * @return имя графа
   */
  std::string getName() const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief needToUpdateLevel it is used to check whether the vertex levels
   * in the graph need to be updated
   * @return bool - true if updating the vertex levels in the graph is
   * required, and false if not required
   *
   * \~russian
   * @brief needToUpdateLevel используется для проверки того, нужно ли
   * обновлять уровни вершин в графе
   * @return bool - true, если требуется обновление уровней вершин в графе,
   * и false, если не требуется
   */
  bool needToUpdateLevel() const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief updateLevels Calculates level for all vertices inside graph.
   * Level show, how far is vertex from an input, Inputs has level 0,
   * vertices, which inConnections contains inputs only - 1 level, etc.
   * Outputs (1 or more) always has max possible level, but
   * not **all** outputs have max possible level.
   *
   * \~russian
   * @brief updateLevels Вычисляет уровень для всех вершин внутри графа.
   * Уровень показывает, насколько далеко вершина находится от входа. Входы
   * имеют уровень 0, вершины, чьи inConnections (входящие соединения)
   * содержат только входы - уровень 1 и т.д.
   * Выходы (1 или более) всегда имеют максимально возможный уровень, но
   * не **все** выходы имеют максимально возможный уровень.
   */
  void updateLevels();

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief getMaxLevel
   * Calculates and returns the maximum level of the output vertices in the
   * graph
   * @return maximum level of the output vertices
   * @par Example
   * @code
   * OrientedGraph graph("ExampleGraph");
   * // Add vertices and edges to the graph (omitted for brevity)
   * // Calculate the maximum level of output vertices
   * uint32_t maxLevel = graph.getMaxLevel();
   * // Output the result
   * std::cout << "Maximum level: " << maxLevel << '\n';
   * @endcode
   *
   * \~russian
   * @brief getMaxLevel
   * Вычисляет и возвращает максимальный уровень выходных вершин в графе
   * @return максимальный уровень выходных вершин
   */
  uint32_t getMaxLevel();

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief setCurrentParent
   * Sets the current parent graph of the current graph
   * @param parent A shared pointer to the parent graph to be set as the
   * current parent
   * @par Example
   * @code
   * // Create an instance of OrientedGraph
   * auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
   * // Create another instance of OrientedGraph to be set as the parent
   * auto parentGraph = std::make_shared<OrientedGraph>("ParentGraph");
   * // Set parentGraph as the current parent of graph
   * graph->setCurrentParent(parentGraph);
   * // Now parentGraph is set as the current parent of graph
   * @endcode
   *
   * \~russian
   * @brief setCurrentParent
   * Устанавливает текущий родительский граф для текущего графа
   * @param parent Shared-указатель на родительский граф, который будет
   * установлен в качестве текущего родителя
   */
  void setCurrentParent(GraphPtr parent);

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief resetCounters
   * Resets counters associated with a specific graph instance
   * @param where A shared pointer to the subgraph whose counters need to
   * be reset
   *
   * \~russian
   * @brief resetCounters
   * Сбрасывает счетчики, связанные с конкретным экземпляром графа
   * @param where Shared-указатель на подграф, счетчики которого
   * необходимо сбросить
   */
  void resetCounters(GraphPtr where);

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief addInput
   * Adds an input vertex to the current graph
   * @param i_name The name of the input vertex to be added
   * @return A shared pointer to the newly created input vertex
   * @par Example
   * @code
   * // Create an instance of OrientedGraph
   * auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
   * // Add an input vertex to the graph
   * auto inputVertex = graph->addInput("Input1");
   * // Now inputVertex is a shared pointer to the newly added input vertex
   * @endcode
   *
   * \~russian
   * @brief addInput
   * Добавляет входную вершину в текущий граф
   * @param i_name Имя добавляемой входной вершины
   * @return Указатель (поинтер) на вновь созданную входную вершину
   */
  VertexPtr addInput(const std::string &i_name = "");
  VertexPtr addInputBus(const std::string &i_name = "", size_t width = 1);
  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief addOutput
   * Adds an output vertex to the current graph
   * @param i_name The name of the output vertex to be added
   * @return A shared pointer to the newly created output vertex
   * @par Example
   * @code
   * // Create an instance of OrientedGraph
   * auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
   * // Add an output vertex to the graph
   * auto outputVertex = graph->addOutput("Output1");
   * // Now outputVertex is a shared pointer to the newly added output
   * @endcode
   *
   * \~russian
   * @brief addOutput
   * Добавляет выходную вершину в текущий граф
   * @param i_name Имя добавляемой выходной вершины
   * @return Указатель на вновь созданную выходную вершину
   */
  VertexPtr addOutput(const std::string &i_name = "");
  VertexPtr addOutputBus(const std::string &i_name = "", size_t width = 1);
  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief addConst
   * Adds a constant vertex to the current graph
   * @param i_value The value of the constant vertex to be added
   * @param i_name The name of the constant vertex to be added
   * @return A shared pointer to the newly created constant vertex
   * @par Example
   * @code
   * // Create an instance of OrientedGraph
   * auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
   * // Add a constant vertex to the graph with value 'A', name "Constant1"
   * auto constantVertex = graph->addConst('A', "Constant1");
   * // Now constantVertex is a shared pointer to the newly added constant
   * @endcode
   *
   * \~russian
   * @brief addConst
   * Добавляет константную вершину в текущий граф
   * @param i_value Значение добавляемой константной вершины
   * @param i_name Имя добавляемой константной вершины
   * @return Указатель на вновь созданную константную вершину
   */
  VertexPtr addConst(const char &i_value, const std::string &i_name = "");
  VertexPtr addConstBus(const std::string &i_name = "", size_t width = 1);
  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief addGate
   * Adds a gate vertex to the current graph
   * @param i_gate The type of the gate vertex to be added
   * @param i_name The name of the gate vertex to be added
   * @return A shared pointer to the newly created gate vertex
   * @par Example
   * @code
   * // Create an instance of OrientedGraph
   * auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
   * // Add a gate vertex to the graph with type GateAnd and name "Gate1"
   * auto gateVertex = graph->addGate(Gates::GateAnd, "Gate1");
   * // Now gateVertex is a shared pointer to the newly added gate vertex
   * @endcode
   *
   * \~russian
   * @brief addGate
   * Добавляет вершину-вентиль (gate) в текущий граф
   * @param i_gate Тип добавляемой вершины-вентиля
   * @param i_name Имя добавляемой вершины-вентиля
   * @return Указатель на вновь созданную вершину-вентиль
   */
  VertexPtr addGate(const Gates &i_gate, const std::string &i_name = "");
  VertexPtr addGateBus(const Gates &i_gate, const std::string &i_name = "",
                       size_t width = 1);
  VertexPtr addSliceBus(VertexPtr i_bus, size_t begin, size_t i_width,
                        const std::string &i_name = "");
  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief addSequential Adds a sequential vertex to the current graph.
   * @param i_type The type of the gsequential to be added; can be
   * flip-flop (ff) or latch only
   * @param i_clk Vertex, that is used as clock (or enable for latch)
   * @param i_data Data vertex, should be written to a reg
   * @param i_name The name of the gate vertex to be added
   * @return A shared pointer to the newly created gate vertex
   * @par Example
   * @code
   * // Create an instance of OrientedGraph
   * auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
   * auto *clk = graph->addInput("clk");
   * auto *data = graph->addInput("data");
   * // Creates a simple d flip-flop
   * auto *seq = graph->addSequential(ff, clk, data, "q");
   * @endcode
   *
   * \~russian
   * @brief addSequential Добавляет последовательностную вершину в граф.
   * @param i_type Тип добавляемого последовательностного элемента; может
   * быть только триггером (ff) или защелкой (latch)
   * @param i_clk Вершина, которая используется как синхросигнал (или
   * enable для защелки)
   * @param i_data Вершина данных, должна быть записана в регистр
   * @param i_name Имя добавляемой вершины
   * @return Указатель на вновь созданную вершину
   */
  VertexPtr addSequential(const SequentialTypes &i_type, VertexPtr i_clk,
                          VertexPtr i_data, const std::string &i_name = "");
  VertexPtr addSequentialBus(const SequentialTypes &i_type, VertexPtr i_clk,
                             VertexPtr i_data, const std::string &i_name = "",
                             size_t i_width = 1);
  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief addSequential Adds a sequential vertex to the current graph.
   * @param i_type The type of the gsequential to be added; can be any
   * type, that need one additional signal.
   * @param i_clk Vertex, that is used as clock (or enable for latch) EN
   * for latch and CLK for FF
   * @param i_data Data vertex, should be written to a reg
   * @param i_wire RST or CLR or SET or EN (enable only if flip-flop)
   * @param i_name The name of the gate vertex to be added
   * @return A shared pointer to the newly created gate vertex
   * @par Example
   * @code
   * // Create an instance of OrientedGraph
   * auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
   * auto *clk = graph->addInput("clk");
   * auto *data = graph->addInput("data");
   * auto *rst_n = graph->addInput("rst_n");
   * // Creates a simple d flip-flop with async reset
   * auto *seq = graph->addSequential(affr, clk, data, rst_n, "q");
   * @endcode
   *
   * \~russian
   * @brief addSequential Добавляет последовательностную вершину в граф.
   * @param i_type Тип добавляемого последовательностного элемента; может
   * быть любым типом, требующим один дополнительный сигнал.
   * @param i_clk Вершина, которая используется как синхросигнал. EN для
   * защелки и CLK для триггера
   * @param i_data Вершина данных, должна быть записана в регистр
   * @param i_wire RST или CLR или SET или EN (enable только для триггера)
   * @param i_name Имя добавляемой вершины
   * @return Указатель на вновь созданную вершину
   */
  VertexPtr addSequential(const SequentialTypes &i_type, VertexPtr i_clk,
                          VertexPtr i_data, VertexPtr i_wire,
                          const std::string &i_name = "");
  VertexPtr addSequentialBus(const SequentialTypes &i_type, VertexPtr i_clk,
                             VertexPtr i_data, VertexPtr i_wire,
                             const std::string &i_name = "",
                             size_t i_width = 1);
  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief addSequential Adds a sequential vertex to the current graph.
   * @param i_type The type of the gsequential to be added; can be
   * flip-flop (ff) or latch only
   * @param i_clk Vertex, that is used as clock (or enable for latch)
   * @param i_data Data vertex, should be written to a reg
   * @param i_wire1 RST or CLR or SET
   * @param i_wire2 SET (double set is not allowed, for sure) or EN (en for
   * flip-flop only)
   * @param i_name The name of the gate vertex to be added
   * @return A shared pointer to the newly created gate vertex
   * @par Example
   * @code
   * // Create an instance of OrientedGraph
   * auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
   * auto *clk = graph->addInput("clk");
   * auto *data = graph->addInput("data");
   * auto *clr = graph->addInput("clr");
   * auto *set = graph->addInput("set");
   * // Creates a latch with clr and set signals
   * auto *seq = graph->addSequential(latchcs, clk, data, clr, set, "q");
   * @endcode
   *
   * \~russian
   * @brief addSequential Добавляет последовательностную вершину в граф.
   * @param i_type Тип добавляемого последовательностного элемента; может
   * быть только триггером (ff) или защелкой
   * @param i_clk Вершина, которая используется как синхросигнал (или
   * enable для защелки)
   * @param i_data Вершина данных, должна быть записана в регистр
   * @param i_wire1 RST или CLR или SET
   * @param i_wire2 SET (двойной set не допускается) или EN (en только
   * для триггера)
   * @param i_name Имя добавляемой вершины
   * @return Указатель на вновь созданную вершину
   */
  VertexPtr addSequential(const SequentialTypes &i_type, VertexPtr i_clk,
                          VertexPtr i_data, VertexPtr i_wire1,
                          VertexPtr i_wire2, const std::string &i_name = "");
  VertexPtr addSequentialBus(const SequentialTypes &i_type, VertexPtr i_clk,
                             VertexPtr i_data, VertexPtr i_wire1,
                             VertexPtr i_wire2, const std::string &i_name = "",
                             size_t i_width = 1);
  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief addSequential Adds a sequential vertex to the current graph.
   * Use with FF only!
   * @param i_type The type of the gsequential to be added
   * @param i_clk CLK signal
   * @param i_data what to write, D
   * @param i_rst RST (or CLR) signal
   * @param i_set SET signal
   * @param i_en EN signal
   * @param i_name The name of the gate vertex to be added
   * @return A shared pointer to the newly created gate vertex
   * @par Example
   * @code
   * // Create an instance of OrientedGraph
   * auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
   * auto *clk = graph->addInput("clk");
   * auto *data = graph->addInput("data");
   * auto *rst = graph->addInput("rst");
   * auto *set = graph->addInput("set");
   * auto *en = graph->addInput("en");
   * // Flip-flop with negedge clk, sync reset, set and enable signals
   * auto *seq = graph->addSequential(nffrse, clk, data, rst, set, en, "q");
   * @endcode
   *
   * \~russian
   * @brief addSequential Добавляет последовательностную вершину в граф.
   * Использовать ТОЛЬКО с триггерами (FF)!
   * @param i_type Тип добавляемого последовательностного элемента
   * @param i_clk Сигнал CLK (синхронизация)
   * @param i_data что записать, данные D
   * @param i_rst Сигнал сброса RST (или CLR)
   * @param i_set Сигнал установки SET
   * @param i_en Сигнал разрешения EN
   * @param i_name Имя добавляемой вершины
   * @return Указатель на вновь созданную вершину
   */
  VertexPtr addSequential(const SequentialTypes &i_type, VertexPtr i_clk,
                          VertexPtr i_data, VertexPtr i_rst, VertexPtr i_set,
                          VertexPtr i_en, const std::string &i_name = "");
  VertexPtr addSequentialBus(
      const SequentialTypes &i_type, VertexPtr i_clk, VertexPtr i_data,
      VertexPtr i_rst, VertexPtr i_set, VertexPtr i_en,
      const std::string &i_name = "",
      size_t i_width =
          1); /**
               * \~english
               * @brief addSubGraph
               * Adds a subgraph to the current graph
               * @param i_subGraph A shared pointer to the subgraph to be added
               * @param i_inputs A vector containing the input vertices to be
               * connected to the subgraph
               * @return A vector containing the output vertices of the subgraph
               * @par Example
               * @code
               * // Create an instance of OrientedGraph
               * auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
               * // Create a subgraph
               * auto subGraph = std::make_shared<OrientedGraph>("SubGraph");
               * // Create input vertices for the subgraph
               * std::vector<VertexPtr> subGraphInputs;
               * for (size_t i = 0; i < 3; ++i) {
               * auto inputVertex = graph->addInput("SubGraphInput" +
               * std::to_string(i + 1));
               * subGraphInputs.push_back(inputVertex);
               * }
               * // Add the subgraph to the graph with its inputs
               * auto subGraphOutputs = graph->addSubGraph(subGraph,
               * subGraphInputs);
               * // Now subGraphOutputs contains the output vertices of the
               * subgraph
               * @endcode
               * @throws std::invalid_argument if the number of inputs does not
               * match the number of input vertices in the subgraph
               *
               * \~russian
               * @brief addSubGraph
               * Добавляет подграф в текущий граф
               * @param i_subGraph Shared-указатель на добавляемый подграф
               * @param i_inputs Вектор, содержащий входные вершины для
               * подключения к подграфу
               * @return Вектор, содержащий выходные вершины подграфа
               * @throws std::invalid_argument, если количество входов не
               * совпадает с количеством входных вершин в подграфе
               */
  std::vector<VertexPtr> addSubGraph(GraphPtr i_subGraph,
                                     std::vector<VertexPtr> i_inputs);

  /** * @author Theossr <feolab05@gmail.com>
   * \~english
   * @brief Starts graph simulation
   * @param values Vector of input values
   * @return Vector of output simulation results
   * * \~russian
   * @brief Запускает симуляцию графа
   * @param values Вектор входных значений
   * @return Вектор выходных результатов симуляции
   */
  std::vector<char> graphSimulation(std::vector<char> values);

  /** * @author Theossr <feolab05@gmail.com>
   * \~english
   * @brief Resets values after simulation
   * * \~russian
   * @brief Сбрасывает значения после симуляции
   */
  void simulationRemove();

  /**
   * @author rainbowkittensss <viktorrrrry20@gmail.com>
   *
   * \~english
   * @brief removeWasteVertices()
   * Some generated graphs have vertices which have not any path
   * to outputs of scheme. The method deletes these ones.
   * Firstly, it is removing all inner (gates, sequental, subgraph)
   * vertices which d_level == 0 (good criterion because of
   * OrientedGraph::updateLevel() algorithm specificity)
   * and all edges from normal vertices to current wrong.
   * After removing of inner ones, some inputs or constants
   * can have not any element in d_outConnections(), these
   * also will be removed.
   *
   * \~russian
   * @brief removeWasteVertices()
   * В некоторых сгенерированных графах есть вершины, не имеющие пути
   * к выходам схемы. Метод удаляет их.
   * Сначала он удаляет все внутренние (вентили, последовательностные
   * элементы, подграфы) вершины, у которых d_level == 0 (хороший критерий
   * из-за специфики алгоритма OrientedGraph::updateLevel()), а также все
   * ребра от нормальных вершин к этим ошибочным.
   * После удаления внутренних вершин некоторые входы или константы
   * могут остаться без элементов в d_outConnections(), такие вершины
   * также будут удалены.
   */
  void removeWasteVertices();
  void updateEdgesGatesCount(VertexPtr vertex, Gates type);

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief addEdge
   * Adds an edge between two vertices in the current graph
   * @param from A shared pointer to the vertex where the edge originates
   * @param to A shared pointer to the vertex where the edge terminates
   * @return true if the edge was successfully added, false otherwise
   * @par Example
   * @code
   * // Create an instance of OrientedGraph
   * auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
   * // Create two vertices
   * auto vertex1 = graph->addInput("Vertex1");
   * auto vertex2 = graph->addOutput("Vertex2");
   * // Add an edge from vertex1 to vertex2
   * bool edgeAdded = graph->addEdge(vertex1, vertex2);
   * if (edgeAdded) {
   * std::cout << "Edge added successfully between Vertex1 and Vertex2\n";
   * } else {
   * LOG(ERROR) << "Failed to add edge between Vertex1 and Vertex2\n";
   * }
   * @endcode
   * @throws std::invalid_argument if attempting to add an edge from one
   * subgraph to another when the 'from' vertex is not an output, or when
   * attempting to add an edge to another subgraph when the 'to' vertex is
   * not an input
   *
   * \~russian
   * @brief addEdge
   * Добавляет ребро между двумя вершинами в текущем графе
   * @param from Указатель на вершину, из которой исходит ребро
   * @param to Указатель на вершину, в которой ребро заканчивается
   * @return true, если ребро было успешно добавлено, false в противном
   * случае
   * @throws std::invalid_argument при попытке добавить ребро из одного
   * подграфа в другой, когда вершина 'from' не является выходом, или при
   * попытке добавить ребро в другой подграф, когда вершина 'to' не
   * является входом
   */
  bool addEdge(VertexPtr from, VertexPtr to);

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief addEdges
   * Adds multiple edges from a vector of source vertices to a target vertex
   * @param from1 A vector containing the source vertices from which edges
   * will originate
   * @param to A shared pointer to the target vertex where the edges will
   * terminate
   * @return true if all edges were successfully added, false if at least
   * one edge failed to be added
   * @par Example
   * @code
   * // Create an instance of OrientedGraph
   * auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
   * // Create multiple source vertices
   * std::vector<VertexPtr> sources;
   * for (size_t i = 0; i < 3; ++i) {
   * auto vertex = graph->addInput("Source" + std::to_string(i+1));
   * sources.push_back(vertex);
   * }
   * // Create a target vertex
   * auto target = graph->addOutput("Target");
   * // Add edges from all source vertices to the target vertex
   * bool edgesAdded = graph->addEdges(sources, target);
   * if (edgesAdded) {
   * std::cout << "Edges successfully added from all sources to the "
   * << "target\n";
   * } else {
   * LOG(ERROR) << "Failed to add edges from one or more sources to the "
   * << "target\n";
   * }
   * @endcode
   *
   * \~russian
   * @brief addEdges
   * Добавляет несколько ребер от вектора исходных вершин к целевой вершине
   * @param from1 Вектор, содержащий исходные вершины, от которых будут
   * исходить ребра
   * @param to Указатель на целевую вершину, в которой ребра будут
   * заканчиваться
   * @return true, если все ребра были успешно добавлены, false, если хотя
   * бы одно ребро не удалось добавить
   */
  bool addEdges(std::vector<VertexPtr> from1, VertexPtr to);

  /**
   * @author rainbowkittensss <viktorrrrry20@gmail.com>
   *
   * \~english
   * @brief removeEdge
   * Remove an edge from graph if it exists.
   * @param from1 The vertex to be deleted to the input connections of "to"
   * @param to The vertex to be deleted to the output connections of "from"
   * @return True if edge was found and deleted, false otherwise
   * @par Example
   * @code
   * // Creating an instance of the OrientedGraph and two vertices
   * GraphPtr graphPtr = std::make_shared<OrientedGraph>("Graph");
   * VertexPtr v1 = graphPtr->addInput();
   * VertexPtr v2 = graphPtr->addGate(GateBuf);
   * // Creating an edge
   * graphPtr->addEdge(v1, v2);
   * // remove the edge and check of result
   * bool deleted = graphPtr->removeEdge(v1, v2);
   * if (deleted) cout << "Edge removed successfully";
   * else cout << "Edge from v1 to v2 is not exist";
   * @endcode
   *
   * \~russian
   * @brief removeEdge
   * Удаляет ребро из графа, если оно существует.
   * @param from1 Вершина, которую нужно удалить из входных соединений "to"
   * @param to Вершина, которую нужно удалить из выходных соединений "from"
   * @return True, если ребро было найдено и удалено, false в противном
   * случае
   */
  bool removeEdge(VertexPtr from1, VertexPtr to);

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief getEdgesCount Retrieves the total number of edges in the graph
   * @return The total number of edges in the graph
   *
   * \~russian
   * @brief getEdgesCount Возвращает общее количество ребер в графе
   * @return Общее количество ребер в графе
   */
  size_t getEdgesCount() { return d_edgesCount; }

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief Returns set af all subGraphs (graphs, which instances has
   * current graph)
   * @return set of subGrpahs
   *
   * \~russian
   * @brief Возвращает множество всех подграфов (графов, экземпляры которых
   * содержит текущий граф)
   * @return множество подграфов
   */
  std::set<GraphPtr> getSubGraphs() const;

  /**
   * \~english
   * @brief returns all vertices (as an array of vectors of pointers to the
   * base class)
   * @return d_vertices field
   *
   * \~russian
   * @brief возвращает все вершины (в виде массива векторов указателей на
   * базовый класс)
   * @return поле d_vertices
   */
  std::array<std::vector<VertexPtr>, VertexTypes::output + 1>
  getBaseVertexes() const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief getVerticeByIndex returns a vertex from graph. Index should be
   * smaller, that number of all vertices inside current graph. Index
   * firstly is used for inputs, than - constants, than - gates,
   * sequential, subGraphs and than - outputs.
   * @param idx Index of the vertex to retrieve
   * @return Pointer to the requested vertex
   * @throw out_of_range if idx is bigger than number of all vertices in
   * graph
   *
   * \~russian
   * @brief getVerticeByIndex возвращает вершину из графа. Индекс должен
   * быть меньше, чем количество всех вершин внутри текущего графа. Индекс
   * сначала используется для входов, затем - констант, затем - вентилей,
   * последовательностных элементов, подграфов и затем - выходов.
   * @param idx Индекс вершины для получения
   * @return Указатель на запрошенную вершину
   * @throw out_of_range, если idx больше количества всех вершин в графе
   */
  VertexPtr getVerticeByIndex(size_t idx) const;

  static void readVerilog(std::string i_path, Context &context);
  static CG_Graph::Context readVerilog(std::string i_path);
  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief method used for translating graph to verilog
   * @param i_path folder, in which file should be created
   * @param i_filename name of a file, which should be created
   * @return flag, if file was correctly created or not
   *
   * \~russian
   * @brief метод, используемый для трансляции графа в формат Verilog
   * @param i_path папка, в которой должен быть создан файл
   * @param i_filename имя файла, который должен быть создан
   * @return флаг, указывающий, был ли файл корректно создан или нет
   */
  bool toVerilog(std::string i_path, std::string i_filename = "",
                 bool i_sequentialByInstance = false);

  bool toVerilogBusEnabled(std::string i_path, std::string i_filename = "",
                           bool i_sequentialByInstance = false);

  bool toVerilogBusEnabledAsOneBit(std::string i_path,
                                   std::string i_filename = "");

  /**
   * @author Vladimir Zunin <vzunin@hse.ru>
   *
   * \~english
   * @brief toDOT returns DOT format representation
   * @return DOT structure
   *
   * \~russian
   * @brief toDOT возвращает представление в формате DOT
   * @return Структура DOT
   */
  DotReturn toDOT();

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief toDOT translates graph to DOT format
   * @param i_path file path
   * @param i_filename file name
   * @return true if successful
   *
   * \~russian
   * @brief toDOT транслирует граф в формат DOT
   * @param i_path путь к файлу
   * @param i_filename имя файла
   * @return true, если успешно
   */
  bool toDOT(std::string i_path, std::string i_filename = "");

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief toGraphMLClassic Writes the graph structure in GraphML format
   * to the specified output stream
   * @param i_fileStream A reference to the std::ofstream object, which
   * represents the file in which the graph structure will be written in
   * GraphML format
   * @return bool: Returns true if the graph structure has been successfully
   * written in GraphML format, and false otherwise. In this case, it
   * always returns true.
   *
   * \~russian
   * @brief toGraphMLClassic Записывает структуру графа в формате GraphML в
   * указанный выходной поток
   * @param i_fileStream Ссылка на объект std::ofstream, который
   * представляет файл, в который будет записана структура графа в формате
   * GraphML
   * @return bool: Возвращает true, если структура графа была успешно
   * записана в формате GraphML, и false в противном случае. В данном
   * случае всегда возвращает true.
   */
  bool toGraphMLClassic(std::ofstream &i_fileStream);

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief toGraphMLPseudoABCD Writes the graph structure in GraphML
   * format to the specified output stream
   * @param i_fileStream A reference to the std::ofstream object, which
   * represents the file in which the graph structure will be written in
   * GraphML format
   * @return bool: Returns true if the graph structure has been successfully
   * written in GraphML format, and false otherwise.
   *
   * \~russian
   * @brief toGraphMLPseudoABCD Записывает структуру графа в формате
   * GraphML в указанный выходной поток
   * @param i_fileStream Ссылка на объект std::ofstream, который
   * представляет файл, в который будет записана структура графа в формате
   * GraphML
   * @return bool: Возвращает true, если структура графа была успешно
   * записана в формате GraphML, и false в противном случае.
   */
  bool toGraphMLPseudoABCD(std::ofstream &i_fileStream);

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief toGraphMLOpenABCD Writes the graph structure in GraphML format
   * to the specified output stream
   * @param i_fileStream A reference to the std::ofstream object, which
   * represents the file in which the graph structure will be written in
   * GraphML format
   * @return bool: Returns true if the graph structure has been successfully
   * written in GraphML format, and false otherwise.
   *
   * \~russian
   * @brief toGraphMLOpenABCD Записывает структуру графа в формате GraphML
   * в указанный выходной поток
   * @param i_fileStream Ссылка на объект std::ofstream, который
   * представляет файл, в который будет записана структура графа в формате
   * GraphML
   * @return bool: Возвращает true, если структура графа была успешно
   * записана в формате GraphML, и false в противном случае.
   */
  bool toGraphMLOpenABCD(std::ofstream &i_fileStream);

  /**
   * \~english
   * @brief Parses vertex information to GraphML format components
   * @param vertexType The type of the vertex being parsed
   * @param vertexVector Vector containing pointers to the vertices
   * @param nodeTemplate String template for formatting nodes
   * @param edgeTemplate String template for formatting edges
   * @param i_prefix Prefix string for ID generation
   * @param nodes Reference to string where parsed nodes will be appended
   * @param edges Reference to string where parsed edges will be appended
   * @todo add description
   *
   * \~russian
   * @brief Парсит информацию о вершине в компоненты формата GraphML
   * @param vertexType Тип парсируемой вершины
   * @param vertexVector Вектор, содержащий указатели на вершины
   * @param nodeTemplate Строковый шаблон для форматирования узлов
   * @param edgeTemplate Строковый шаблон для форматирования ребер
   * @param i_prefix Строка-префикс для генерации ID
   * @param nodes Ссылка на строку, куда будут добавлены распарсенные узлы
   * @param edges Ссылка на строку, куда будут добавлены распарсенные ребра
   * @todo добавить описание
   */
  void parseVertexToGraphML(const VertexTypes &vertexType,
                            const std::vector<VertexPtr> &vertexVector,
                            const std::string &nodeTemplate,
                            const std::string &edgeTemplate,
                            const std::string &i_prefix, std::string &nodes,
                            std::string &edges);

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief Is called by toGraphMLClassic. Generates classical GraphML format.
   * @param i_indent Indentation level for formatting
   * @param i_prefix Prefix for graph components
   * @return Formatted GraphML string
   * @todo add description
   *
   * \~russian
   * @brief Вызывается методом toGraphMLClassic. Генерирует классический формат
   * GraphML.
   * @param i_indent Уровень отступа для форматирования
   * @param i_prefix Префикс для компонентов графа
   * @return Отформатированная строка GraphML
   * @todo добавить описание
   */
  std::string toGraphMLClassic(uint16_t i_indent = 0,
                               const std::string &i_prefix = "");

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief Is called by toGraphMLPseudoABCD.
   * @return Formatted pseudo-ABCD GraphML string
   * @todo add description
   *
   * \~russian
   * @brief Вызывается методом toGraphMLPseudoABCD.
   * @return Отформатированная строка псевдо-ABCD GraphML
   * @todo добавить описание
   */
  std::string toGraphMLPseudoABCD();

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief Is called by toGraphMLOpenABCD.
   * @return Formatted open-ABCD GraphML string
   * @todo add description
   *
   * \~russian
   * @brief Вызывается методом toGraphMLOpenABCD.
   * @return Отформатированная строка open-ABCD GraphML
   * @todo добавить описание
   */
  std::string toGraphMLOpenABCD();

  /**
   * \~english
   * @brief used for looking for a vector of all vertices with required type
   * @param i_type The required vertex type
   * @param i_name Optional name to filter by
   * @param i_addSubGraphs Whether to search inside subgraphs
   * @return Vector of vertices matching the criteria
   *
   * \~russian
   * @brief используется для поиска вектора всех вершин требуемого типа
   * @param i_type Требуемый тип вершины
   * @param i_name Опциональное имя для фильтрации
   * @param i_addSubGraphs Выполнять ли поиск внутри подграфов
   * @return Вектор вершин, соответствующих критериям
   */
  std::vector<VertexPtr>
  getVerticesByType(const VertexTypes &i_type, std::string_view i_name = "",
                    const bool &i_addSubGraphs = false) const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief gets all vertices with requires level
   * @param i_level The target level to retrieve
   * @return Vector of vertices at the specified level
   *
   * \~russian
   * @brief получает все вершины с требуемым уровнем
   * @param i_level Целевой уровень для поиска
   * @return Вектор вершин на указанном уровне
   */
  std::vector<VertexPtr> getVerticesByLevel(uint32_t i_level);

  /**
   * \~english
   * @brief Looks for all vertices with given name in graph and subGrpahs
   * (if required)
   * @param i_name name, which should have vertices
   * @param i_addSubGraphs if true, looks inside subGraphs for vertices. Is
   * false by default.
   * @return vector with all found vertices.
   *
   * \~russian
   * @brief Ищет все вершины с заданным именем в графе и подграфах (если
   * требуется)
   * @param i_name имя, которое должны иметь вершины
   * @param i_addSubGraphs если true, ищет вершины внутри подграфов. По
   * умолчанию false.
   * @return вектор со всеми найденными вершинами.
   */
  std::vector<VertexPtr>
  getVerticesByName(std::string_view i_name,
                    const bool i_addSubGraphs = false) const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief Call calculateHash before this check!!!!
   * @param rhs another value to be compared
   * @return returns true, if hashes are equal
   *
   * \~russian
   * @brief Вызовите calculateHash перед этой проверкой!!!!
   * @param rhs другое значение для сравнения
   * @return возвращает true, если хэши равны
   */
  bool operator==(const OrientedGraph &rhs);

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief calculateHash calculates hash values for a graph based on the
   * hash values of its vertices
   * When running for a second time, set hash flags to default state
   * @return A string representing the hash value of the graph
   *
   * \~russian
   * @brief calculateHash вычисляет значения хэша для графа на основе
   * значений хэша его вершин
   * При повторном запуске установите флаги хэша в состояние по умолчанию
   * @return Строка, представляющая значение хэша графа
   */
  std::string calculateHash();

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief getGatesCount Returns a display containing the number of each
   * gate type in the graph
   * @return A display where each key is a type of gate (Gates), and the
   * corresponding value is the number of gates of this type in the graph
   *
   * \~russian
   * @brief getGatesCount Возвращает словарь, содержащий количество
   * вентилей каждого типа в графе
   * @return Словарь, где каждый ключ - это тип вентиля (Gates), а
   * соответствующее значение - количество вентилей этого типа в графе
   */
  std::map<Gates, size_t> getGatesCount() const;

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief getEdgesGatesCount Returns a mapping containing the number of
   * edges between different types of gates in the graph
   * @return A mapping where each external key is a gate type, and the
   * corresponding value is an internal mapping containing the number of
   * edges between different types of gates in the graph
   *
   * \~russian
   * @brief getEdgesGatesCount Возвращает словарь, содержащий количество
   * ребер между различными типами вентилей в графе
   * @return Словарь, где каждый внешний ключ - это тип вентиля, а
   * соответствующее значение - это внутренний словарь, содержащий
   * количество ребер между различными типами вентилей в графе
   */
  std::map<Gates, std::map<Gates, size_t>> getEdgesGatesCount() const;
  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief reserve additional place in vector for given number of
   * VertexPtr, where are located vertices of given type.
   * @param i_type Type, for which place should be reserved
   * @param i_capacity Number of vertices, which would be added later
   *
   * \~russian
   * @brief резервирует дополнительное место в векторе для заданного
   * количества VertexPtr, где расположены вершины заданного типа.
   * @param i_type Тип, для которого должно быть зарезервировано место
   * @param i_capacity Количество вершин, которые будут добавлены позже
   */
  void reserve(VertexTypes i_type, size_t i_capacity);

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief resets counter for graph IDs
   *
   * \~russian
   * @brief сбрасывает счетчик для идентификаторов графов
   */
  static void resetCounter() { d_countGraph = 0ul; }

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief Checks graph connectivity
   * @param i_recalculate Flag to force recalculation of connectivity
   * @return bool, true if the graph is connected, and false if not.
   *
   * \~russian
   * @brief Проверяет связность графа
   * @param i_recalculate Флаг для принудительного пересчета связности
   * @return bool, true, если граф связен, и false, если нет.
   */
  bool isConnected(bool i_recalculate = false);

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief A simple counter for subGrpah instances to give them unique
   * names in Verilog
   * @param i_id GraphID of subGraph, instance of which is being created.
   * @return new index for new unique name.
   *
   * \~russian
   * @brief Простой счетчик для экземпляров подграфов, чтобы дать им
   * уникальные имена в Verilog
   * @param i_id GraphID подграфа, экземпляр которого создается.
   * @return новый индекс для нового уникального имени.
   */
  std::uint64_t getGraphInstVerilog(GraphID i_id) {
    return d_graphInstanceToVerilogCount[i_id]++;
  }

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief A simple counter for subGrpah instances to give them unique
   * names in DOT format
   * @param i_id GraphID of subGraph, instance of which is being created.
   * @return new index for new unique name.
   *
   * \~russian
   * @brief Простой счетчик для экземпляров подграфов, чтобы дать им
   * уникальные имена в формате DOT
   * @param i_id GraphID подграфа, экземпляр которого создается.
   * @return новый индекс для нового уникального имени.
   */
  std::uint64_t getGraphInstDOT(GraphID i_id) {
    return d_graphInstanceToDotCount[i_id]++;
  }

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief getter for unique graph ID
   * @return id of graph (size_t)
   *
   * \~russian
   * @brief геттер для уникального ID графа
   * @return id графа (size_t)
   */
  GraphID getID() { return d_graphID; }

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief Unrolls graph.
   * @return Pointer to unrolled graph
   * @todo add normal description
   *
   * \~russian
   * @brief Разворачивает граф.
   * @return Указатель на развернутый граф
   * @todo добавить нормальное описание
   */
  GraphPtr unrollGraph();

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief creates a majority element, represented by a graph.
   * @return GraphPtr to created graph
   *
   * \~russian
   * @brief создает мажоритарный элемент, представленный графом.
   * @return GraphPtr на созданный граф
   */
  static GraphPtr createMajoritySubgraph();
  VertexPtr majorityAsLogic(VertexPtr a, VertexPtr b, VertexPtr c,
                            VertexPtr output);

  /**
   * @author Andrey <shapkin.andrey123@gmail.com>
   *
   * \~english
   * @brief creates majority element inside current graph
   * @param a First input vertex
   * @param b Second input vertex
   * @param c Third input vertex
   * @return Vertex pointer to the generated majority output
   *
   * \~russian
   * @brief создает мажоритарный элемент внутри текущего графа
   * @param a Первая входная вершина
   * @param b Вторая входная вершина
   * @param c Третья входная вершина
   * @return Указатель на вершину сгенерированного мажоритарного выхода
   */
  VertexPtr generateMajority(VertexPtr a, VertexPtr b, VertexPtr c);

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
  virtual void log(el::base::type::ostream_t &os) const;
#endif

protected:
  /* memory management block */

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   *
   * \~english
   * @brief Allocates memory and creates an instance of required type. Is
   * used for creating ALL vertices of any type.
   * @tparam T Type of vertex to create
   * @tparam Args Template argument pack for constructor
   * @param args Arguments forwarded to constructor
   * @return Pointer to newly created instance
   *
   * \~russian
   * @brief Выделяет память и создает экземпляр требуемого типа.
   * Используется для создания ВСЕХ вершин любого типа.
   * @tparam T Тип создаваемой вершины
   * @tparam Args Пакет шаблонных аргументов для конструктора
   * @param args Аргументы, передаваемые в конструктор
   * @return Указатель на созданный экземпляр
   */
  template<typename T, typename... Args>
  T *create(Args &&...args) {
    return new (allocate<T>()) T(std::forward<Args>(args)...);
  }

  /**
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   * \~english
   * @brief Depth-first search traversal
   * @param i_startVertex Starting vertex for traversal
   * @param i_visited Set tracking visited vertices
   * @param i_dsg Set tracking subgraphs
   * * \~russian
   * @brief Обход в глубину (DFS)
   * @param i_startVertex Стартовая вершина для обхода
   * @param i_visited Множество отслеживаемых посещенных вершин
   * @param i_dsg Множество отслеживаемых подграфов
   */
  void dfs(VertexPtr i_startVertex, std::unordered_set<VertexPtr> &i_visited,
           std::unordered_set<VertexPtr> &i_dsg);

private:
  static bool printSequentialModules(GraphPtr i_graph,
                                     std::ofstream &i_fileStream);

  /**
   * @brief This method is a stage of generating verilog output by toVerilog()
   * toVerilogBusEnabled() or toVerilogSeparateVariables().
   * @param i_graph graph for output
   * @param i_fileStream output stream
   *
   * @param i_path path for output verilog file
   * @param i_filename file name
   *@return true if file created correctly, false otherwise
   */
  static bool verilogFileCreating(GraphPtr i_graph, std::string i_path,
                                  std::string i_filename,
                                  std::ofstream &i_fileStream);

  /**
   * @brief The method is a stage of generating verilog output by toVerilog()
   * toVerilogBusEnabled() or toVerilogSeparateVariables().
   * @param i_graph graph for output
   * @param i_fileStream output stream
   *
   * @param i_printPin function, defined in toVerilog..(), print correct
   * name of vertex to i_fileStream
   *
   */
  static void verilogInoutsWriting(GraphPtr i_graph,
                                   std::ofstream &i_fileStream,
                                   std::function<void(VertexPtr)> i_printPin);

  /**
   * @brief The method is a stage of generating verilog output by toVerilog()
   * toVerilogBusEnabled() or toVerilogSeparateVariables().
   * @param i_graph graph for output
   * @param i_fileStream output stream
   *
   * @param i_printFunction function, defined in toVerilog..(), print all
   * vertices of type (i_usedType from VertexTypes) to i_fileStream
   */
  static void
  verilogVerticesDeclaration(GraphPtr i_graph, std::ofstream &i_fileStream,
                             std::function<void(std::vector<GraphVertexBase *>,
                                                VertexTypes i_usedType)>
                                 i_printFunction);

  /**
   * @brief The method is a stage of generating verilog output by toVerilog()
   * toVerilogBusEnabled() or toVerilogSeparateVariables().
   * @param i_graph graph for output
   * @param i_fileStream output stream
   *
   * @param i_getInstance function, defined in toVerilog..(),
   * print correct constant instance to i_fileStream
   */
  static void
  verilogConstantWriting(GraphPtr i_graph, std::ofstream &i_fileStream,
                         std::function<void(VertexPtr)> i_getInstance,
                         std::function<void(VertexPtr)> i_getDefinition);

  /**
   * @brief The method is a stage of generating verilog output by toVerilog()
   * toVerilogBusEnabled() or toVerilogSeparateVariables().
   * @param i_graph graph for output
   * @param i_fileStream output stream
   *
   * @param i_path path to create verilog files for subgraphs
   * @return true if all files created correctly, false otherwise
   */
  static bool verilogSubgraphWriting(GraphPtr i_graph,
                                     std::ofstream &i_fileStream,
                                     std::string i_path);

  /**
   * @brief The method is a stage of generating verilog output by toVerilog()
   * toVerilogBusEnabled() or toVerilogSeparateVariables().
   * @param i_graph graph for output
   * @param i_fileStream output stream
   *
   * @param i_printDefinition function, defined in toVerilog..(),
   * print correct definitions to i_fileStream
   */
  static void verilogVerticesDefining(
      GraphPtr i_graph, std::ofstream &i_fileStream,
      std::function<void(VertexPtr)> i_printDefinitionSequential,
      std::function<void(const VertexPtr)> i_printDefinitionGates);

  /**
   * @brief The method is a stage of generating verilog output by toVerilog()
   * toVerilogBusEnabled() or toVerilogSeparateVariables().
   * @param i_graph graph for output
   * @param i_fileStream output stream
   *
   * @return true if file saved correctly, false otherwise
   */
  static bool verilogFinalOperations(GraphPtr i_graph,
                                     std::ofstream &i_fileStream);

private:
  static std::atomic_size_t d_countNewGraphInstance;
  static std::atomic_size_t d_countGraph;

private:
  enum HASH_STATE : uint8_t {
    HC_NOT_CALC = 0,
    HC_IN_PROGRESS = 1,
    HC_CALC = 2
  };
  // used for quick gates count
  std::map<Gates, size_t> d_gatesCount = {
      {Gates::GateAnd, 0}, {Gates::GateNand, 0}, {Gates::GateOr, 0},
      {Gates::GateNor, 0}, {Gates::GateNot, 0},  {Gates::GateBuf, 0},
      {Gates::GateXor, 0}, {Gates::GateXnor, 0}};

  // used for quick edges of gate type count;
  std::map<Gates, std::map<Gates, size_t>> d_edgesGatesCount;

  // We can add a subgraph multiple times
  // so we need to count instances to verilog.
  // We are counting to know, which inputs and outputs should we use now
  std::map<GraphID, uint64_t> d_graphInstanceToVerilogCount;
  std::map<GraphID, uint64_t> d_graphInstanceToDotCount;

  // each subgraph has one or more outputs. We save them,
  // depending on subgraph instance number
  std::vector<VertexPtr> d_allSubGraphsOutputs;

  std::set<GraphPtr> d_subGraphs;
  std::array<std::vector<VertexPtr>, VertexTypes::output + 1> d_vertices;

  // as we can have multiple parents, we save
  // for toVerilog current parent graph
  GraphPtrWeak d_currentParentGraph;

  std::string d_name;

  GraphID d_graphID;
  size_t d_edgesCount = 0;

  size_t d_hashed = 0;
  HASH_STATE d_hashState = HC_NOT_CALC;

  bool d_isSubGraph = false;
  // Пока не реализован функционал.
  bool d_needLevelUpdate = true;

  // also we need to now, was .v file for subgraph created, or not
  bool d_alreadyParsedVerilog = false;
  bool d_alreadyParsedDot = false;

  // -1 if false, 0 if undefined, 1 if true
  int8_t d_connected = 0;
  static GraphReader *graphReader;
};

} // namespace CG_Graph
