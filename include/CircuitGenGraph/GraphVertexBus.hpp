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
class GraphVertexBus {
  friend class GraphVertexBusSlice;

public:
  GraphVertexBus(size_t i_width);
  /**
   * @brief Create or obtain a slice of the bus.
   *
   * Returns an object representing a subrange (slice) of the bus,
   * defined by bit indices. The slice provides access to a subset
   * of the bus bits and can be used as an independent vertex in
   * graph operations and code generation.
   *
   * @param i_left  Index of the most significant bit of the slice.
   * @param i_right Index of the least significant bit of the slice.
   * @return Pointer to a GraphVertexBusSlice representing the selected range.
   */
  GraphVertexBusSlice getSlice(size_t i_start, size_t i_end);

  size_t getWidth() const;
  /**
   * @brief Obtain a pointer to the GraphVertexBus interface from a base vertex.
   *
   * This helper method performs a safe cast from GraphVertexBase to
   * GraphVertexBus. It is typically used when working with generic
   * vertex pointers that are known to represent bus vertices.
   *
   * @param i_vertex Pointer to a graph vertex.
   * @return Pointer to GraphVertexBus if the vertex represents a bus,
   *         otherwise nullptr.
   */
  static GraphVertexBus *getBusPointer(VertexPtr i_vertex);
  static const GraphVertexBus *getBusPointer(const VertexPtr i_vertex);
  /**
   * @brief Get suffix for the bus name.
   *
   * Usually used for name mangling in generated HDL code
   * (e.g. "[7:0]" for an 8-bit bus).
   *
   * @return Bus name suffix as a string.
   */
  std::string getBusNameSuffix();
  /**
   * @brief Compare two vertices by their bus width.
   *
   * This method is intended to be used as a comparator,
   * for example in sorting routines.
   *
   * @param left Pointer to the first vertex.
   * @param right Pointer to the second vertex.
   * @return True if the left bus width is smaller than the right one.
   */
  static bool compareBusWidth(const VertexPtr i_left, const VertexPtr i_right);
  /**
   * @brief Convert the bus to a one-bit Verilog representation.
   *
   * This method must be implemented in derived classes.
   *
   * @return Verilog code representing a single bit of the bus.
   */
  virtual std::string toOneBitVerilog() const = 0;
  /**
   * @brief Update the bus value during simulation.
   *
   * If the input value is not empty, it replaces the current value.
   *
   * @param i_value New bus value.
   */
  virtual void updateValueBus(std::string i_value = "");

private:
  void setWidth(size_t i_width);
  size_t d_width;
  std::string d_value;
};

} // namespace CG_Graph