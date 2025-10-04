#pragma once

#include <cstddef>
#include <string>

#define VertexPtr CG_Graph::GraphVertexBase *
namespace CG_Graph {
class GraphVertexBase;
class GraphVertexBusSlice;
class GraphVertexBus {
  friend class GraphVertexBusSlice;
  /// \class GraphVertexBus
  /// @brief The class provides methods, specific for bus as type
  /// of vertex
  /// @param d_width Width of bus
  /// @param d_value Value of the bus through simulation

public:
  GraphVertexBus(size_t width);
  GraphVertexBusSlice getSlice(size_t start, size_t end);
  size_t getWidth() const;
  static GraphVertexBus *getBusPointer(VertexPtr vertex);
  static GraphVertexBus *getBusPointer(const VertexPtr vertex);
  std::string getBusNameSuffix();
  static bool compareBusWidth(const VertexPtr left, const VertexPtr right);
  virtual std::string toOneBitVerilog() const = 0;
  virtual void updateValueBus(std::string i_value = "");

private:
  void setWidth(size_t i_width);
  size_t d_width;
  std::string d_value;
};

} // namespace CG_Graph