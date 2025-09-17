#pragma once

#include <cstddef>
#include <string>

#define VertexPtr CG_Graph::GraphVertexBase *
namespace CG_Graph {
class GraphVertexBase;
class GraphVertexBusSlice;
/// \class GraphVertexBus
/// @brief The class provides methods, specific for bus as type
/// of vertex
/// @param d_width Width of bus
/// @param d_value Value of the bus through simulation
/// @param d_inConnectionSlices Bus may be connected to others as a
/// GraphVertexSlice (to set which part of it will be used). There information
/// about these "custom" edges will be stored.
///
class GraphVertexBus {
  friend class GraphVertexBusSlice;
  /// \class GraphVertexBus
  /// @brief The class provides methods, specific for bus as type
  /// of vertex
  /// @param d_width Width of bus
  /// @param d_value Value of the bus through simulation

  /* Необходимо решить задачи:
  1. Определения длины и получения срезов +
  2. Проведения ребер между шинами и между обычными вершинами и шинами +
  3. Приведения полученного графа в форму файла на верилоге с помощью
  OrientedGraph::toVerilog +
  4. Хэширования +
  5. Симуляции +-
  6. Приведения к dot, graphML
  */
  // 1
public:
  GraphVertexBus(
      size_t width); // это конструктор для шины-вершины, у него start и end
                     // вычисляются а d_isSlice = 0, пользователям можно
                     // применять его. а срезы создавать методом getSlice.
  /// @brief toVerilog
  /// Generates Verilog code for the vertex
  /// @return A string containing Verilog code for the vertex, or an empty
  /// string if the vertex type is not "output" or if the incoming connection
  /// is invalid
  GraphVertexBusSlice getSlice(size_t start, size_t end);
  /// @brief toVerilog
  /// Generates Verilog code for the vertex
  /// @return A string containing Verilog code for the vertex, or an empty
  /// string if the vertex type is not "output" or if the incoming connection
  /// is invalid
  size_t getWidth() const;
  /// @brief toVerilog
  /// Generates Verilog code for the vertex
  /// @return A string containing Verilog code for the vertex, or an empty
  /// string if the vertex type is not "output" or if the incoming connection
  /// is invalid
  static GraphVertexBus *getBusPointer(VertexPtr vertex);
  static GraphVertexBus *getBusPointer(const VertexPtr vertex);
  /// @brief toVerilog
  /// Generates Verilog code for the vertex
  /// @return A string containing Verilog code for the vertex, or an empty
  /// string if the vertex type is not "output" or if the incoming connection
  /// is invalid
  std::string
  getBusNameSuffix(); // возвращает что то в духе [7:0], вероятно это будет
                      // удобно в
                      //  OrientedGraph::toVerilog() и вспомогательных методах
                      // для вывода графа в верилог, по крайней мере более
                      // аккуратного решения пока не придумывается :(
  //  template<class T>
  //  T *as_if();
  /// @brief toVerilog
  /// Generates Verilog code for the vertex
  /// @return A string containing Verilog code for the vertex, or an empty
  /// string if the vertex type is not "output" or if the incoming connection
  /// is invalid
  static bool compareBusWidth(const VertexPtr left, const VertexPtr right);
  virtual std::string toOneBitVerilog()
      const = 0; // пусть будет примерно как у graphVertexBase, то есть
                 // работающий для выходов ради единообразия. Не совпадает с тем
                 // что у вершины потому что гипотетически может понадобиться
                 // записать assign q = hehehe[5:3];
  /// @brief toVerilog
  /// Generates Verilog code for the vertex
  /// @return A string containing Verilog code for the vertex, or an empty
  /// string if the vertex type is not "output" or if the incoming connection
  /// is invalid
  virtual std::string updateValueBus();
  /// @brief toVerilog
  /// Generates Verilog code for the vertex
  /// @return A string containing Verilog code for the vertex, or an empty
  /// string if the vertex type is not "output" or if the incoming connection
  /// is invalid
private:
  void setWidth(size_t i_width);
  size_t d_width;
  std::string d_value; //(из ValueStates) вью потому что основан на d_value
                       // в соответствующей вершине
};

} // namespace CG_Graph