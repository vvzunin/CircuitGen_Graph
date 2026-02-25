#pragma once

#include <array>
#include <atomic>
#include <ctime>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include <CircuitGenGraph/enums.hpp>

#include <CircuitGenGraph/GraphMemory.hpp>
#include <CircuitGenGraph/GraphUtils.hpp>
#include <CircuitGenGraph/GraphVertexBase.hpp>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

/// \~english
/// TODO: Add checking for file names when adding new vertices
/// \~russian
/// TODO: Добавить проверку на имена файлов при добавлении новых вершин
/// \~

#define GraphPtr std::shared_ptr<CG_Graph::OrientedGraph>
#define GraphPtrWeak std::weak_ptr<CG_Graph::OrientedGraph>

#define VertexPtr CG_Graph::GraphVertexBase *

namespace CG_Graph {

class GraphVertexBase;

/// \class CG_Graph::OrientedGraph
///
/// \~english
/// @param d_countNewGraphInstance Static variable to count new graph instances
/// @param d_countGraph Current instance counter. Is used for setting a graphID
/// @param d_currentParentGraph Weak pointer to the current parent graph
/// @param d_edgesCount The number of edges in the graph
/// @param d_hashed Hashed value of the graph
/// @param d_parentGraphs Vector of weak pointers to parent graphs
/// @param d_name The name of the graph
/// @param d_needLevelUpdate A flag indicating whether the vertex levels in the graph need to be updated
/// @param d_graphInstanceToVerilogCount Map to count instances to Verilog.
/// This map is used to count how many times each subgraph instance has been
/// converted to Verilog. The key represents the instance number of the
/// subgraph, and the value represents the count of how many times it has been
/// converted to Verilog
/// @param d_graphInstanceToDotCount Map to count instances to DOT.
/// This map is used to count how many times each subgraph instance has been
/// converted to DOT. The key represents the instance number of the
/// subgraph, and the value represents the count of how many times it has been
/// converted to DOT
/// @param d_allSubGraphsOutputs Vector storing all outputs of subgraphs
/// @param d_subGraphs Set of subgraphs.
/// This set stores the subgraphs present in the graph. It is used to keep
/// track of all the subgraphs associated with the current graph instance
/// @param d_vertices Map of vertex types to vectors of vertex pointers
/// @param d_countGraph Static counter for the total number of graphs
/// @param d_gatesCount Map for quick gates count.
/// This map is used for quick counting of gates in the graph. It maps each
/// gate type (Gates) to the count of gates of that type present in the graph.
/// @param d_edgesGatesCount Map for quick count of edges of gate type.
/// This map is used for quick counting of edges between different types of
/// gates in the graph. It maps each gate type (Gates) to another map, where
/// the inner map maps gate types to the count of edges between them.
/// \~russian
/// @param d_countNewGraphInstance Статическая переменная для подсчета новых экземпляров графа
/// @param d_countGraph Текущий счетчик экземпляров. Используется для установки graphID
/// @param d_currentParentGraph Слабый указатель (weak pointer) на текущий родительский граф
/// @param d_edgesCount Количество ребер в графе
/// @param d_hashed Хэшированное значение графа
/// @param d_parentGraphs Вектор слабых указателей на родительские графы
/// @param d_name Имя графа
/// @param d_needLevelUpdate Флаг, указывающий, нужно ли обновлять уровни вершин в графе
/// @param d_graphInstanceToVerilogCount Словарь для подсчета экземпляров, преобразованных в Verilog.
/// Этот словарь используется для подсчета того, сколько раз каждый экземпляр подграфа
/// был преобразован в Verilog. Ключ представляет номер экземпляра
/// подграфа, а значение - количество раз, которое он был преобразован в Verilog
/// @param d_graphInstanceToDotCount Словарь для подсчета экземпляров, преобразованных в DOT.
/// Этот словарь используется для подсчета того, сколько раз каждый экземпляр подграфа
/// был преобразован в DOT. Ключ представляет номер экземпляра
/// подграфа, а значение - количество раз, которое он был преобразован в DOT
/// @param d_allSubGraphsOutputs Вектор, хранящий все выходы подграфов
/// @param d_subGraphs Множество подграфов.
/// Это множество хранит подграфы, присутствующие в графе. Используется для отслеживания
/// всех подграфов, связанных с текущим экземпляром графа
/// @param d_vertices Словарь, сопоставляющий типы вершин с векторами указателей на вершины
/// @param d_countGraph Статический счетчик общего количества графов
/// @param d_gatesCount Словарь для быстрого подсчета вентилей.
/// Этот словарь используется для быстрого подсчета вентилей в графе. Он сопоставляет каждый
/// тип вентиля (Gates) с количеством вентилей этого типа, присутствующих в графе.
/// @param d_edgesGatesCount Словарь для быстрого подсчета ребер между типами вентилей.
/// Этот словарь используется для быстрого подсчета ребер между различными типами
/// вентилей в графе. Он сопоставляет каждый тип вентиля (Gates) с другим словарем,
/// где внутренний словарь сопоставляет типы вентилей с количеством ребер между ними.
/// \~

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

  // \~english TODO: Add the use of gates_inputs_info. \~russian TODO: Добавить использование gates_inputs_info.

  ~OrientedGraph();

  OrientedGraph &operator=(const OrientedGraph &other) = delete;
  OrientedGraph &operator=(OrientedGraph &&other) = delete;
  OrientedGraph(const OrientedGraph &other) = delete;
  OrientedGraph(OrientedGraph &&other) = delete;

  // \~english Number of gates in the graph, excluding subgraphs \~russian Количество gate в графе, за исключением подграфов
  /// @brief \~english baseSize returns the number of "gate" type vertices in the graph \~russian baseSize возвращает количество вершин типа "gate" в графе
  /// @return \~english An integer value representing the number of "gate" type vertices in the graph \~russian Целочисленное значение, представляющее количество вершин типа "gate" в графе

  size_t baseSize() const;
  // \~english Number of gates in the graph, including subgraphs \~russian Количество gate в графе, включая подграфы
  /// @brief
  /// \~english
  /// fullSize returns the total number of vertices in the graph,
  /// including vertices from all subgraphs. It recursively traverses all
  /// the subgraphs and sums up the number of vertices in each of them
  /// \~russian
  /// fullSize возвращает общее количество вершин в графе,
  /// включая вершины из всех подграфов. Он рекурсивно обходит все
  /// подграфы и суммирует количество вершин в каждом из них
  /// \~
  /// @return \~english An integer value representing the total number of vertices in the graph, including vertices from all subgraphs \~russian Целочисленное значение, представляющее общее количество вершин в графе, включая вершины из всех подграфов

  size_t fullSize() const;
  // \~english sum of gates, inputs, outputs and consts sizes \~russian сумма размеров вентилей, входов, выходов и констант
  /// @brief
  /// \~english
  /// sumFullSize returns the total number of vertices of all types
  /// in the graph, including input vertices, constants, gates, and output vertices
  /// \~russian
  /// sumFullSize возвращает общее количество вершин всех типов
  /// в графе, включая входные вершины, константы, вентили и выходные вершины
  /// \~
  /// @return \~english the size of all vertices in the graph, summing the number of vertices of each type \~russian размер всех вершин в графе, путем суммирования количества вершин каждого типа
  size_t sumFullSize() const;

  // \~english Are there any gates in the circuit, including subgraphs \~russian Имеются ли gate в схеме, включая подграфы
  /// @brief \~english Checks, if there are any gates in graph (including subrapgs). In fact checks, if fullSize is equal to zero \~russian Проверяет, есть ли какие-либо вентили в графе (включая подграфы). Фактически проверяет, равен ли fullSize нулю
  bool isEmpty() const;

  /// @brief \~english Resets all internal states for all types of vertices \~russian Сбрасывает все внутренние состояния для всех типов вершин
  void clearAllStates();
  /// @brief \~english Resets hash states for all vertices of current graph (including subgraphs). Should be called before each hash recalculation \~russian Сбрасывает состояния хэша для всех вершин текущего графа (включая подграфы). Должен вызываться перед каждым пересчетом хэша
  void clearHashStates();
  /// @brief \~english Resets update for all vertices of current graph (including subgraphs). Should be called before each level recalculation \~russian Сбрасывает состояние обновления для всех вершин текущего графа (включая подграфы). Должен вызываться перед каждым пересчетом уровня
  void clearNeedUpdateStates();
  /// @brief
  /// \~english
  /// Sets for all vertices state, that shows, that all vertices
  /// were not used for for `getVerticesByLevel`. Is called by `getVerticesByLevel`.
  /// \~russian
  /// Устанавливает для всех вершин состояние, показывающее, что все вершины
  /// не использовались для `getVerticesByLevel`. Вызывается методом `getVerticesByLevel`.
  /// \~
  void clearUsedLevelStates();

  /// @brief
  /// \~english
  /// isEmptyFull It is used to check the emptiness of a graph,
  /// including all its subgraphs. It recursively traverses all the subgraphs
  /// and checks if they are empty
  /// \~russian
  /// isEmptyFull Используется для проверки пустоты графа,
  /// включая все его подграфы. Рекурсивно обходит все подграфы
  /// и проверяет, пусты ли они
  /// \~
  /// @return \~english bool - true if the graph and all its subgraphs are empty, and false if at least one of them contains vertice \~russian bool - true, если граф и все его подграфы пусты, и false, если хотя бы один из них содержит вершины
  bool isEmptyFull() const;

  /// @brief \~english setName Used to set the name of the graph. It takes a string as an argument and sets the d_name field to this string \~russian setName Используется для установки имени графа. Принимает строку в качестве аргумента и устанавливает поле d_name в эту строку
  /// @param i_name \~english the new name of the graph \~russian новое имя графа

  void setName(const std::string &i_name);

  /// @brief \~english getName Used to get the name of the graph \~russian getName Используется для получения имени графа
  /// @return \~english the name of the graph \~russian имя графа

  std::string getName() const;

  /// @brief \~english needToUpdateLevel it is used to check whether the vertex levels in the graph need to be updated \~russian needToUpdateLevel используется для проверки того, нужно ли обновлять уровни вершин в графе
  /// @return \~english bool - true if updating the vertex levels in the graph is required, and false if not required \~russian bool - true, если требуется обновление уровней вершин в графе, и false, если не требуется

  bool needToUpdateLevel() const;

  /// @brief
  /// \~english
  /// updateLevels Calculates level for all vertices inside graph.
  /// Level show, how far is vertex from an input, Inputs has level 0,
  /// vertices, which inConnections contains inputs only - 1 level, etc.
  /// Outputs (1 or more) always has max possible level, but
  /// not **all** outputs have max possible level.
  /// \~russian
  /// updateLevels Вычисляет уровень для всех вершин внутри графа.
  /// Уровень показывает, насколько далеко вершина находится от входа. Входы имеют уровень 0,
  /// вершины, чьи inConnections (входящие соединения) содержат только входы - уровень 1 и т.д.
  /// Выходы (1 или более) всегда имеют максимально возможный уровень, но
  /// не **все** выходы имеют максимально возможный уровень.
  /// \~

  void updateLevels();

  /// @brief
  /// \~english
  /// getMaxLevel
  /// Calculates and returns the maximum level of the output vertices in the graph
  /// \~russian
  /// getMaxLevel
  /// Вычисляет и возвращает максимальный уровень выходных вершин в графе
  /// \~
  /// @return \~english maximum level of the output vertices \~russian максимальный уровень выходных вершин
  /// @code
  ///  OrientedGraph graph("ExampleGraph");
  /// // Add vertices and edges to the graph (omitted for brevity)
  /// // Calculate the maximum level of output vertices
  /// uint32_t maxLevel = graph.getMaxLevel();
  /// // Output the result
  /// std::cout << "Maximum level of output vertices: " << maxLevel << '\n';
  /// @endcode

  uint32_t getMaxLevel();

  /// @brief
  /// \~english
  /// setCurrentParent
  /// Sets the current parent graph of the current graph
  /// \~russian
  /// setCurrentParent
  /// Устанавливает текущий родительский граф для текущего графа
  /// \~
  /// @param parent \~english A shared pointer to the parent graph to be set as the current parent \~russian Shared-указатель на родительский граф, который будет установлен в качестве текущего родителя
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Create another instance of OrientedGraph to be set as the current
  /// parent auto parentGraph = std::make_shared<OrientedGraph>("ParentGraph");
  /// // Set parentGraph as the current parent of graph
  /// graph->setCurrentParent(parentGraph);
  /// // Now parentGraph is set as the current parent of graph
  /// @endcode
  void setCurrentParent(GraphPtr parent);

  /// @brief
  /// \~english
  /// resetCounters
  /// Resets counters associated with a specific graph instance
  /// \~russian
  /// resetCounters
  /// Сбрасывает счетчики, связанные с конкретным экземпляром графа
  /// \~
  /// @param where \~english A shared pointer to the subgraph whose counters need to be reset \~russian Shared-указатель на подграф, счетчики которого необходимо сбросить
  void resetCounters(GraphPtr where);

  /// @brief
  /// \~english
  /// addInput
  /// Adds an input vertex to the current graph
  /// \~russian
  /// addInput
  /// Добавляет входную вершину в текущий граф
  /// \~
  /// @param i_name \~english The name of the input vertex to be added \~russian Имя добавляемой входной вершины
  /// @return \~english A shared pointer to the newly created input vertex \~russian Указатель (поинтер) на вновь созданную входную вершину
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Add an input vertex to the graph
  /// auto inputVertex = graph->addInput("Input1");
  /// // Now inputVertex is a shared pointer to the newly added input vertex
  /// @endcode

  VertexPtr addInput(const std::string &i_name = "");

  /// @brief
  /// \~english
  /// addOutput
  /// Adds an output vertex to the current graph
  /// \~russian
  /// addOutput
  /// Добавляет выходную вершину в текущий граф
  /// \~
  /// @param i_name \~english The name of the output vertex to be added \~russian Имя добавляемой выходной вершины
  /// @return \~english A shared pointer to the newly created output vertex \~russian Указатель на вновь созданную выходную вершину
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Add an output vertex to the graph
  /// auto outputVertex = graph->addOutput("Output1");
  /// // Now outputVertex is a shared pointer to the newly added output vertex
  /// @endcode

  VertexPtr addOutput(const std::string &i_name = "");

  /// @brief
  /// \~english
  /// addConst
  /// Adds a constant vertex to the current graph
  /// \~russian
  /// addConst
  /// Добавляет константную вершину в текущий граф
  /// \~
  /// @param i_value \~english The value of the constant vertex to be added \~russian Значение добавляемой константной вершины
  /// @param i_name \~english The name of the constant vertex to be added \~russian Имя добавляемой константной вершины
  /// @return \~english A shared pointer to the newly created constant vertex \~russian Указатель на вновь созданную константную вершину
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Add a constant vertex to the graph with value 'A' and name "Constant1"
  /// auto constantVertex = graph->addConst('A', "Constant1");
  /// // Now constantVertex is a shared pointer to the newly added constant
  /// vertex
  /// @endcode

  VertexPtr addConst(const char &i_value, const std::string &i_name = "");

  /// @brief
  /// \~english
  /// addGate
  /// Adds a gate vertex to the current graph
  /// \~russian
  /// addGate
  /// Добавляет вершину-вентиль (gate) в текущий граф
  /// \~
  /// @param i_gate \~english The type of the gate vertex to be added \~russian Тип добавляемой вершины-вентиля
  /// @param i_name \~english The name of the gate vertex to be added \~russian Имя добавляемой вершины-вентиля
  /// @return \~english A shared pointer to the newly created gate vertex \~russian Указатель на вновь созданную вершину-вентиль
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Add a gate vertex to the graph with type GateAnd and name "Gate1"
  /// auto gateVertex = graph->addGate(Gates::GateAnd, "Gate1");
  /// // Now gateVertex is a shared pointer to the newly added gate vertex
  /// @endcode

  VertexPtr addGate(const Gates &i_gate, const std::string &i_name = "");

  /// @brief \~english addSequential Adds a sequential vertex to the current graph. \~russian addSequential Добавляет последовательностную вершину в текущий граф.
  /// @param i_type \~english The type of the gsequential to be added; can be flip-flop (ff) or latch only \~russian Тип добавляемого последовательностного элемента; может быть только триггером (ff) или защелкой (latch)
  /// @param i_clk \~english Vertex, that is used as clock (or enable for latch) \~russian Вершина, которая используется как синхросигнал (или enable для защелки)
  /// @param i_data \~english Data vertex, should be written to a reg \~russian Вершина данных, должна быть записана в регистр
  /// @param i_name \~english The name of the gate vertex to be added \~russian Имя добавляемой вершины
  /// @return \~english A shared pointer to the newly created gate vertex \~russian Указатель на вновь созданную вершину
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// auto *clk = graph->addInput("clk");
  /// auto *data = graph->addInput("data");
  /// // Creates a simple d flip-flop
  /// auto *seq = graph->addSequential(ff, clk, data, "q");
  /// @endcode
  VertexPtr addSequential(const SequentialTypes &i_type, VertexPtr i_clk,
                          VertexPtr i_data, const std::string &i_name = "");

  /// @brief \~english addSequential Adds a sequential vertex to the current graph. \~russian addSequential Добавляет последовательностную вершину в текущий граф.
  /// @param i_type \~english The type of the gsequential to be added; can be any type, that need one additional signal. \~russian Тип добавляемого последовательностного элемента; может быть любым типом, требующим один дополнительный сигнал.
  /// @param i_clk \~english Vertex, that is used as clock (or enable for latch) EN for latch and CLK for FF \~russian Вершина, которая используется как синхросигнал (или enable для защелки). EN для защелки и CLK для триггера
  /// @param i_data \~english Data vertex, should be written to a reg \~russian Вершина данных, должна быть записана в регистр
  /// @param i_wire \~english RST or CLR or SET or EN (enable only if flip-flop) \~russian RST или CLR или SET или EN (enable только если это триггер)
  /// @param i_name \~english The name of the gate vertex to be added \~russian Имя добавляемой вершины
  /// @return \~english A shared pointer to the newly created gate vertex \~russian Указатель на вновь созданную вершину
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// auto *clk = graph->addInput("clk");
  /// auto *data = graph->addInput("data");
  /// auto *rst_n = graph->addInput("rst_n");
  /// // Creates a simple d flip-flop with async reset
  /// auto *seq = graph->addSequential(affr, clk, data, rst_n, "q");
  /// @endcode
  VertexPtr addSequential(const SequentialTypes &i_type, VertexPtr i_clk,
                          VertexPtr i_data, VertexPtr i_wire,
                          const std::string &i_name = "");

  /// @brief \~english addSequential Adds a sequential vertex to the current graph. \~russian addSequential Добавляет последовательностную вершину в текущий граф.
  /// @param i_type \~english The type of the gsequential to be added; can be flip-flop (ff) or latch only \~russian Тип добавляемого последовательностного элемента; может быть только триггером (ff) или защелкой
  /// @param i_clk \~english Vertex, that is used as clock (or enable for latch) \~russian Вершина, которая используется как синхросигнал (или enable для защелки)
  /// @param i_data \~english Data vertex, should be written to a reg \~russian Вершина данных, должна быть записана в регистр
  /// @param i_wire1 \~english RST or CLR or SET \~russian RST или CLR или SET
  /// @param i_wire2 \~english SET (double set is not allowed, for sure) or EN (en for flip-flop only) \~russian SET (двойной set не допускается) или EN (en только для триггера)
  /// @param i_name \~english The name of the gate vertex to be added \~russian Имя добавляемой вершины
  /// @return \~english A shared pointer to the newly created gate vertex \~russian Указатель на вновь созданную вершину
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// auto *clk = graph->addInput("clk");
  /// auto *data = graph->addInput("data");
  /// auto *clr = graph->addInput("clr");
  /// auto *set = graph->addInput("set");
  /// // Creates a latch with clr and set signals
  /// auto *seq = graph->addSequential(latchcs, clk, data, clr, set, "q");
  /// @endcode
  VertexPtr addSequential(const SequentialTypes &i_type, VertexPtr i_clk,
                          VertexPtr i_data, VertexPtr i_wire1,
                          VertexPtr i_wire2, const std::string &i_name = "");

  /// @brief \~english addSequential Adds a sequential vertex to the current graph. Use with FF only! \~russian addSequential Добавляет последовательностную вершину в текущий граф. Использовать ТОЛЬКО с триггерами (FF)!
  /// @param i_clk \~english CLK signal \~russian Сигнал CLK (синхронизация)
  /// @param i_data \~english what to write, D \~russian что записать, данные D
  /// @param i_rst \~english RST (or CLR) signal \~russian Сигнал сброса RST (или CLR)
  /// @param i_set \~english SET signal \~russian Сигнал установки SET
  /// @param i_en \~english EN signal \~russian Сигнал разрешения EN
  /// @param i_name \~english The name of the gate vertex to be added \~russian Имя добавляемой вершины
  /// @return \~english A shared pointer to the newly created gate vertex \~russian Указатель на вновь созданную вершину
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// auto *clk = graph->addInput("clk");
  /// auto *data = graph->addInput("data");
  /// auto *rst = graph->addInput("rst");
  /// auto *set = graph->addInput("set");
  /// auto *en = graph->addInput("en");
  /// // Flip-flop with negedge clk, sync reset signal, set and enable signals
  /// auto *seq = graph->addSequential(nffrse, clk, data, rst, set, en, "q");
  /// @endcode
  VertexPtr addSequential(const SequentialTypes &i_type, VertexPtr i_clk,
                          VertexPtr i_data, VertexPtr i_rst, VertexPtr i_set,
                          VertexPtr i_en, const std::string &i_name = "");

  /// @brief
  /// \~english
  /// addSubGraph
  /// Adds a subgraph to the current graph
  /// \~russian
  /// addSubGraph
  /// Добавляет подграф в текущий граф
  /// \~
  /// @param i_subGraph \~english A shared pointer to the subgraph to be added \~russian Shared-указатель на добавляемый подграф
  /// @param i_inputs \~english A vector containing the input vertices to be connected to the subgraph \~russian Вектор, содержащий входные вершины для подключения к подграфу
  /// @return \~english A vector containing the output vertices of the subgraph \~russian Вектор, содержащий выходные вершины подграфа
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Create a subgraph
  /// auto subGraph = std::make_shared<OrientedGraph>("SubGraph");
  /// // Create input vertices for the subgraph
  /// std::vector<VertexPtr> subGraphInputs;
  /// for (size_t i = 0; i < 3; ++i)
  /// {
  ///   auto inputVertex = graph->addInput("SubGraphInput" +
  ///   std::to_string(i+1)); subGraphInputs.push_back(inputVertex);
  /// }
  /// // Add the subgraph to the graph with its inputs
  /// auto subGraphOutputs = graph->addSubGraph(subGraph, subGraphInputs);
  /// // Now subGraphOutputs contains the output vertices of the subgraph
  /// @endcode
  /// @throws \~english std::invalid_argument if the number of inputs does not match the number of input vertices in the subgraph \~russian std::invalid_argument, если количество входов не совпадает с количеством входных вершин в подграфе

  std::vector<VertexPtr> addSubGraph(GraphPtr i_subGraph,
                                     std::vector<VertexPtr> i_inputs);

  /// @brief
  /// \~english
  /// removeWasteVertices()
  /// Some generated graphs have vertices which have not any path
  /// to outputs of scheme. The method deletes these ones.
  /// Firstly, it is removing all inner (gates, sequental, subgraph)
  /// vertices which d_level == 0 (good criterion because of
  /// OrientedGraph::updateLevel() algorithm specificity)
  /// and all edges from normal vertices to current wrong.
  /// After removing of inner ones, some inputs or constants
  /// can have not any element in d_outConnections(), these
  /// also will be removed.
  /// \~russian
  /// removeWasteVertices()
  /// В некоторых сгенерированных графах есть вершины, не имеющие пути
  /// к выходам схемы. Метод удаляет их.
  /// Сначала он удаляет все внутренние (вентили, последовательностные элементы, подграфы)
  /// вершины, у которых d_level == 0 (хороший критерий из-за специфики
  /// алгоритма OrientedGraph::updateLevel()), а также все ребра от нормальных вершин к этим ошибочным.
  /// После удаления внутренних вершин некоторые входы или константы
  /// могут остаться без элементов в d_outConnections(), такие вершины
  /// также будут удалены.
  /// \~
  std::vector<char> graphSimulation(std::vector<char> values);

  void simulationRemove();

  void removeWasteVertices();
  /// @brief
  /// \~english
  /// addEdge
  /// Adds an edge between two vertices in the current graph
  /// \~russian
  /// addEdge
  /// Добавляет ребро между двумя вершинами в текущем графе
  /// \~
  /// @param from \~english A shared pointer to the vertex where the edge originates \~russian Указатель на вершину, из которой исходит ребро
  /// @param to \~english A shared pointer to the vertex where the edge terminates \~russian Указатель на вершину, в которой ребро заканчивается
  /// @return \~english true if the edge was successfully added, false otherwise \~russian true, если ребро было успешно добавлено, false в противном случае
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Create two vertices
  /// auto vertex1 = graph->addInput("Vertex1");
  /// auto vertex2 = graph->addOutput("Vertex2");
  /// // Add an edge from vertex1 to vertex2
  /// bool edgeAdded = graph->addEdge(vertex1, vertex2);
  /// if (edgeAdded)
  /// {
  ///   std::cout << "Edge successfully added between Vertex1 and Vertex2\n";
  /// }
  /// else
  /// {
  ///   LOG(ERROR) << "Failed to add edge between Vertex1 and Vertex2\n";
  /// }
  /// @endcode
  /// @throws \~english std::invalid_argument if attempting to add an edge from one subgraph to another when the 'from' vertex is not an output, or when attempting to add an edge to another subgraph when the 'to' vertex is not an input \~russian std::invalid_argument при попытке добавить ребро из одного подграфа в другой, когда вершина 'from' не является выходом, или при попытке добавить ребро в другой подграф, когда вершина 'to' не является входом

  bool addEdge(VertexPtr from, VertexPtr to);

  /// @brief
  /// \~english
  /// addEdges
  /// Adds multiple edges from a vector of source vertices to a target vertex
  /// \~russian
  /// addEdges
  /// Добавляет несколько ребер от вектора исходных вершин к целевой вершине
  /// \~
  /// @param from1 \~english A vector containing the source vertices from which edges will originate \~russian Вектор, содержащий исходные вершины, от которых будут исходить ребра
  /// @param to \~english A shared pointer to the target vertex where the edges will terminate \~russian Указатель на целевую вершину, в которой ребра будут заканчиваться
  /// @return \~english true if all edges were successfully added, false if at least one edge failed to be added \~russian true, если все ребра были успешно добавлены, false, если хотя бы одно ребро не удалось добавить
  /// @code
  /// // Create an instance of OrientedGraph
  /// auto graph = std::make_shared<OrientedGraph>("ExampleGraph");
  /// // Create multiple source vertices
  /// std::vector<VertexPtr> sources;
  /// for (size_t i = 0; i < 3; ++i)
  /// {
  ///   auto vertex = graph->addInput("Source" + std::to_string(i+1));
  ///   sources.push_back(vertex);
  /// }
  /// // Create a target vertex
  /// auto target = graph->addOutput("Target");
  /// // Add edges from all source vertices to the target vertex
  /// bool edgesAdded = graph->addEdges(sources, target);
  /// if (edgesAdded)
  /// {
  ///   std::cout << "Edges successfully added from all sources to the
  ///   target\n";
  /// }
  /// else
  /// {
  ///   LOG(ERROR) << "Failed to add edges from one or more sources to the
  ///   target\n";
  /// }
  /// @endcode

  bool addEdges(std::vector<VertexPtr> from1, VertexPtr to);

  /// @brief
  /// \~english
  /// removeEdge
  /// Remove an edge from graph if it exists.
  /// \~russian
  /// removeEdge
  /// Удаляет ребро из графа, если оно существует.
  /// \~
  /// @param from1 \~english The vertex to be deleted to the input connections of "to" \~russian Вершина, которую нужно удалить из входных соединений "to"
  /// @param to \~english The vertex to be deleted to the output connections of "from" \~russian Вершина, которую нужно удалить из выходных соединений "from"
  /// @return \~english True if edge was found and deleted, false otherwise \~russian True, если ребро было найдено и удалено, false в противном случае
  /// @code
  /// // Creating an instance of the OrientedGraph and two vertices
  /// GraphPtr graphPtr = std::make_shared<OrientedGraph>("Graph");
  /// VertexPtr v1 = graphPtr->addInput();
  /// VertexPtr v2 = graphPtr->addGate(GateBuf);
  /// // Creating an edge
  /// graphPtr->addEdge(v1, v2);
  /// // remove the edge and check of result
  /// bool deleted = graphPtr->removeEdge(v1, v2);
  /// if (deleted) cout << "Edge removed successfully";
  /// else cout << "Edge from v1 to v2 is not exist";
  /// @endcode

  bool removeEdge(VertexPtr from1, VertexPtr to);

  /// @brief \~english getEdgesCount Retrieves the total number of edges in the graph \~russian getEdgesCount Возвращает общее количество ребер в графе
  /// @return \~english The total number of edges in the graph \~russian Общее количество ребер в графе
  size_t getEdgesCount() { return d_edgesCount; }

  /// @brief \~english Returns set af all subGraphs (graphs, which instances has current graph) \~russian Возвращает множество всех подграфов (графов, экземпляры которых содержит текущий граф)
  /// @return \~english set of subGrpahs \~russian множество подграфов
  std::set<GraphPtr> getSubGraphs() const;

  /// @brief \~english returns all vertices (as an array of vectors of pointers to the base class) \~russian возвращает все вершины (в виде массива векторов указателей на базовый класс)
  /// @return \~english d_vertices field \~russian поле d_vertices
  std::array<std::vector<VertexPtr>, VertexTypes::output + 1>
  getBaseVertexes() const;

  /// @brief
  /// \~english
  /// getVerticeByIndex returns a vertex from graph. Index should be
  /// smaller, that number of all vertices inside current graph. Index firstly
  /// is used for inputs, than - constants, than - gates, sequential, subGraphs
  /// and than - outputs.
  /// \~russian
  /// getVerticeByIndex возвращает вершину из графа. Индекс должен быть
  /// меньше, чем количество всех вершин внутри текущего графа. Индекс сначала
  /// используется для входов, затем - констант, затем - вентилей, последовательностных элементов, подграфов
  /// и затем - выходов.
  /// \~
  /// @throw \~english out_of_range if idx is bigger than number of all vertices in graph \~russian out_of_range, если idx больше количества всех вершин в графе
  VertexPtr getVerticeByIndex(size_t idx) const;

  /// @brief \~english method used for translating graph to verilog \~russian метод, используемый для трансляции графа в формат Verilog
  /// @param i_path \~english folder, in which file should be created \~russian папка, в которой должен быть создан файл
  /// @param i_filename \~english name of a file, which should be created \~russian имя файла, который должен быть создан
  /// @return \~english flag, if file was correctly vreated or not \~russian флаг, указывающий, был ли файл корректно создан или нет
  bool toVerilog(std::string i_path, std::string i_filename = "");

  /// @brief
  /// @return
  DotReturn toDOT();

  /// @brief
  /// @param i_path
  /// @param i_filename
  /// @return
  bool toDOT(std::string i_path, std::string i_filename = "");

  /// @brief
  /// \~english
  /// toGraphML Writes the graph structure in GraphML format to the specified output stream
  /// \~russian
  /// toGraphML Записывает структуру графа в формате GraphML в указанный выходной поток
  /// \~
  /// @param i_fileStream \~english A reference to the std::ofstream object, which represents the file in which the graph structure will be written in GraphML format \~russian Ссылка на объект std::ofstream, который представляет файл, в который будет записана структура графа в формате GraphML
  /// @return \~english bool: Returns true if the graph structure has been successfully written in GraphML format, and false otherwise. In this case, it always returns true. \~russian bool: Возвращает true, если структура графа была успешно записана в формате GraphML, и false в противном случае. В данном случае всегда возвращает true.
  bool toGraphMLClassic(std::ofstream &i_fileStream);

  /// @brief
  /// \~english
  /// toGraphMLPseudoABCD Writes the graph structure in GraphML format to the specified output stream
  /// \~russian
  /// toGraphMLPseudoABCD Записывает структуру графа в формате GraphML в указанный выходной поток
  /// \~
  /// @param i_fileStream \~english A reference to the std::ofstream object, which represents the file in which the graph structure will be written in GraphML format \~russian Ссылка на объект std::ofstream, который представляет файл, в который будет записана структура графа в формате GraphML
  /// @return \~english bool: Returns true if the graph structure has been successfully written in GraphML format, and false otherwise. In this case, it always returns true. \~russian bool: Возвращает true, если структура графа была успешно записана в формате GraphML, и false в противном случае. В данном случае всегда возвращает true.
  bool toGraphMLPseudoABCD(std::ofstream &i_fileStream);

  /// @brief
  /// \~english
  /// toGraphMLOpenABCD Writes the graph structure in GraphML format to the specified output stream
  /// \~russian
  /// toGraphMLOpenABCD Записывает структуру графа в формате GraphML в указанный выходной поток
  /// \~
  /// @param i_fileStream \~english A reference to the std::ofstream object, which represents the file in which the graph structure will be written in GraphML format \~russian Ссылка на объект std::ofstream, который представляет файл, в который будет записана структура графа в формате GraphML
  /// @return \~english bool: Returns true if the graph structure has been successfully written in GraphML format, and false otherwise. In this case, it always returns true. \~russian bool: Возвращает true, если структура графа была успешно записана в формате GraphML, и false в противном случае. В данном случае всегда возвращает true.
  bool toGraphMLOpenABCD(std::ofstream &i_fileStream);

  /// \~english TODO: add description \~russian TODO: добавить описание
  void parseVertexToGraphML(const VertexTypes &vertexType,
                            const std::vector<VertexPtr> &vertexVector,
                            const std::string &nodeTemplate,
                            const std::string &edgeTemplate,
                            const std::string &i_prefix, std::string &nodes,
                            std::string &edges);

  /// @brief \~english Is called by toGraphMLClassic. TODO: add description \~russian Вызывается методом toGraphMLClassic. TODO: добавить описание
  std::string toGraphMLClassic(uint16_t i_indent = 0,
                               const std::string &i_prefix = "");

  /// @brief \~english Is called by toGraphMLPseudoABCD. TODO: add description \~russian Вызывается методом toGraphMLPseudoABCD. TODO: добавить описание
  std::string toGraphMLPseudoABCD();

  /// @brief \~english Is called by toGraphMLOpenABCD. TODO: add description \~russian Вызывается методом toGraphMLOpenABCD. TODO: добавить описание
  std::string toGraphMLOpenABCD();

  /// @brief \~english used for looking for a vector of all vertices with required type \~russian используется для поиска вектора всех вершин требуемого типа
  /// @param i_type
  /// @param i_name
  /// @param i_addSubGraphs
  /// @return
  std::vector<VertexPtr>
  getVerticesByType(const VertexTypes &i_type, std::string_view i_name = "",
                    const bool &i_addSubGraphs = false) const;

  /// @brief \~english gets all vertices with requires level \~russian получает все вершины с требуемым уровнем
  std::vector<VertexPtr> getVerticesByLevel(uint32_t i_level);

  /// @brief \~english Looks for all vertices with given name in graph and subGrpahs (if required) \~russian Ищет все вершины с заданным именем в графе и подграфах (если требуется)
  /// @param i_name \~english name, which should have vertices \~russian имя, которое должны иметь вершины
  /// @param i_addSubGraphs \~english if true, looks inside subGraphs for vertices. Is false by default. \~russian если true, ищет вершины внутри подграфов. По умолчанию false.
  /// @return \~english vector with all found vertices. \~russian вектор со всеми найденными вершинами.
  std::vector<VertexPtr>
  getVerticesByName(std::string_view i_name,
                    const bool i_addSubGraphs = false) const;

  /// @brief \~english Call calculateHash before this check!!!! \~russian Вызовите calculateHash перед этой проверкой!!!!
  /// @param rhs \~english another value to be compared \~russian другое значение для сравнения
  /// @return \~english returns true, if hashes are equal \~russian возвращает true, если хэши равны
  bool operator==(const OrientedGraph &rhs);

  /// @brief
  /// \~english
  /// calculateHash calculates hash values for a graph based on the hash
  /// values of its vertices
  /// When running for a second time, set hash flags to default state
  /// \~russian
  /// calculateHash вычисляет значения хэша для графа на основе значений хэша
  /// его вершин
  /// При повторном запуске установите флаги хэша в состояние по умолчанию
  /// \~
  /// @return \~english A string representing the hash value of the graph \~russian Строка, представляющая значение хэша графа

  std::string calculateHash();

  // @brief
  /// \~english
  /// getGatesCount Returns a display containing the number of each gate
  /// type in the graph
  /// \~russian
  /// getGatesCount Возвращает словарь, содержащий количество вентилей каждого
  /// типа в графе
  /// \~
  /// @return \~english A display where each key is a type of gate (Gates), and the corresponding value is the number of gates of this type in the graph \~russian Словарь, где каждый ключ - это тип вентиля (Gates), а соответствующее значение - количество вентилей этого типа в графе

  std::map<Gates, size_t> getGatesCount() const;

  /// @brief
  /// \~english
  /// getEdgesGatesCount Returns a mapping containing the number of
  /// edges between different types of gates in the graph
  /// \~russian
  /// getEdgesGatesCount Возвращает словарь, содержащий количество
  /// ребер между различными типами вентилей в графе
  /// \~
  /// @return \~english A mapping where each external key is a gate type, and the corresponding value is an internal mapping containing the number of edges between different types of gates in the graph \~russian Словарь, где каждый внешний ключ - это тип вентиля, а соответствующее значение - это внутренний словарь, содержащий количество ребер между различными типами вентилей в графе

  std::map<Gates, std::map<Gates, size_t>> getEdgesGatesCount() const;

  /// @brief \~english reserve additional place in vector for given number of VertexPtr, where are located vertices of given type. \~russian резервирует дополнительное место в векторе для заданного количества VertexPtr, где расположены вершины заданного типа.
  /// @param i_type \~english Type, for which place should be reserved \~russian Тип, для которого должно быть зарезервировано место
  /// @param i_capacity \~english Number of vertices, which would be added later \~russian Количество вершин, которые будут добавлены позже
  void reserve(VertexTypes i_type, size_t i_capacity) {
    d_vertices[i_type].reserve(d_vertices[i_type].size() + i_capacity);
  }

  /// @brief \~english resets counter for graph IDs \~russian сбрасывает счетчик для идентификаторов графов
  static void resetCounter() { d_countGraph = 0ul; }

  /// @brief \~english Checks graph connectivity \~russian Проверяет связность графа
  /// @return \~english bool, true if the graph is connected, and false if not. \~russian bool, true, если граф связен, и false, если нет.
  bool isConnected(bool i_recalculate = false);

  /// @brief
  /// \~english
  /// A simple counter for subGrpah instances to give
  /// them unique names in Verilog
  /// \~russian
  /// Простой счетчик для экземпляров подграфов, чтобы дать
  /// им уникальные имена в Verilog
  /// \~
  /// @param i_id \~english GraphID of subGraph, instance of which is being created. \~russian GraphID подграфа, экземпляр которого создается.
  /// @return \~english new index for new unique name. \~russian новый индекс для нового уникального имени.
  std::uint64_t getGraphInstVerilog(GraphID i_id) {
    return d_graphInstanceToVerilogCount[i_id]++;
  }

  /// @brief
  /// \~english
  /// A simple counter for subGrpah instances to give
  /// them unique names in DOT format
  /// \~russian
  /// Простой счетчик для экземпляров подграфов, чтобы дать
  /// им уникальные имена в формате DOT
  /// \~
  /// @param i_id \~english GraphID of subGraph, instance of which is being created. \~russian GraphID подграфа, экземпляр которого создается.
  /// @return \~english new index for new unique name. \~russian новый индекс для нового уникального имени.
  std::uint64_t getGraphInstDOT(GraphID i_id) {
    return d_graphInstanceToDotCount[i_id]++;
  }

  /// @brief \~english getter for unique graph ID \~russian геттер для уникального ID графа
  /// @return \~english id of graph (size_t) \~russian id графа (size_t)
  GraphID getID() { return d_graphID; }

  /// @brief \~english Unrolls graph. TODO: add normal description \~russian Разворачивает граф. TODO: добавить нормальное описание
  GraphPtr unrollGraph();

  /// @brief \~english creates a majority element, represented by a graph. \~russian создает мажоритарный элемент, представленный графом.
  /// @return \~english GraphPtr to created graph \~russian GraphPtr на созданный граф
  static GraphPtr createMajoritySubgraph();
  /// @brief \~english creates majority element inside current graph \~russian создает мажоритарный элемент внутри текущего графа
  VertexPtr generateMajority(VertexPtr a, VertexPtr b, VertexPtr c);

  /// @brief \~english log Used for easylogging++ \~russian log Используется для easylogging++
  /// @param os \~english Stream for easylogging \~russian Поток для easylogging
#ifdef LOGFLAG
  virtual void log(el::base::type::ostream_t &os) const;
#endif

protected:
  /* memory management block */

  /// @author Fuuulkrum7
  /// @brief \~english Allocates memory and creates an instance of required type. Is used for creating ALL vertices of any type. \~russian Выделяет память и создает экземпляр требуемого типа. Используется для создания ВСЕХ вершин любого типа.
  /// @tparam T
  /// @tparam ...Args
  /// @param ...args
  /// @return
  template<typename T, typename... Args>
  T *create(Args &&...args) {
    return new (allocate<T>()) T(std::forward<Args>(args)...);
  }

  void dfs(VertexPtr i_startVertex, std::unordered_set<VertexPtr> &i_visited,
           std::unordered_set<VertexPtr> &i_dsg);

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
};

} // namespace CG_Graph