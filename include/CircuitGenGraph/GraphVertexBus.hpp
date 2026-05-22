#pragma once

#include <cstddef>
#include <string>

#define VertexPtr CG_Graph::GraphVertexBase *
namespace CG_Graph {
class GraphVertexBase;
class GraphVertexBusSlice;

/**
 * @class GraphVertexBus
 * \~english
 * @brief Provides methods specific to bus-type vertices.
 * @param d_width Bus width.
 * @param d_value Bus value during simulation.
 *
 * \~russian
 * @brief Предоставляет методы, специфичные для вершин типа шина.
 * @param d_width Ширина шины.
 * @param d_value Значение шины в ходе симуляции.
 */
class GraphVertexBus {
  friend class GraphVertexBusSlice;

public:
  GraphVertexBus(size_t i_width);

  /**
   * \~english
   * @brief Create or obtain a slice of the bus.
   *
   * Returns an object representing a subrange (slice) of the bus,
   * defined by bit indices. The slice provides access to a subset
   * of the bus bits and can be used as an independent vertex in
   * graph operations and code generation.
   *
   * @param i_start Index of the most significant bit of the slice.
   * @param i_end Index of the least significant bit of the slice.
   * @return Pointer to a GraphVertexBusSlice representing the selected range.
   *
   * \~russian
   * @brief Создает или возвращает срез шины.
   * @param i_start Индекс старшего бита среза.
   * @param i_end Индекс младшего бита среза.
   * @return Указатель на GraphVertexBusSlice для выбранного диапазона.
   */
  GraphVertexBusSlice getSlice(size_t i_start, size_t i_end);

  /**
   * \~english
   * @brief Obtain a pointer to the GraphVertexBus interface from a base vertex.
   *
   * This helper method performs a safe cast from GraphVertexBase to
   * GraphVertexBus. It is typically used when working with generic
   * vertex pointers that are known to represent bus vertices.
   *
   * @param i_vertex Pointer to a graph vertex.
   * @return Pointer to GraphVertexBus if the vertex represents a bus,
   *         otherwise nullptr.
   *
   * \~russian
   * @brief Возвращает указатель на интерфейс GraphVertexBus из базовой вершины.
   * @param i_vertex Указатель на вершину графа.
   * @return Указатель на GraphVertexBus, если вершина является шиной,
   * иначе `nullptr`.
   */
  static GraphVertexBus *getBusPointer(VertexPtr i_vertex);
  /**
   * \~english
   * @brief Const overload of bus pointer extractor.
   * @param i_vertex Pointer to a graph vertex.
   * @return Const pointer to GraphVertexBus if available, otherwise `nullptr`.
   *
   * \~russian
   * @brief Константная перегрузка получения указателя на шину.
   * @param i_vertex Указатель на вершину графа.
   * @return Константный указатель на GraphVertexBus либо `nullptr`.
   */
  static const GraphVertexBus *getBusPointer(const VertexPtr i_vertex);

  /**
   * \~english
   * @brief Get width of the bus.
   * @return Width as integer value.
   *
   * \~russian
   * @brief Возвращает ширину шины.
   * @return Ширина в виде целого значения.
   */

  size_t getWidth() const;

  /**
   * \~english
   * @brief Get suffix for the bus name.
   *
   * Usually used for name mangling in generated HDL code
   * (e.g. "[7:0]" for an 8-bit bus).
   *
   * @return Bus name suffix as a string.
   *
   * \~russian
   * @brief Возвращает суффикс имени шины.
   * @return Суффикс имени шины в виде строки.
   */

  std::string getBusNameSuffix();

  /**
   * \~english
   * @brief Convert the bus to a one-bit Verilog representation.
   *
   * This method must be implemented in derived classes.
   *
   * @return Verilog code representing a single bit of the bus.
   *
   * \~russian
   * @brief Преобразует шину в однобитное Verilog-представление.
   * @return Код Verilog, представляющий один бит шины.
   */
  virtual std::string toOneBitVerilog() const = 0;

  /**
   * \~english
   * @brief Compare two vertices by their bus width.
   *
   * This method is intended to be used as a comparator,
   * for example in sorting routines. Equal to i_left <= i_right
   *
   * @param i_left Pointer to the first vertex.
   * @param i_right Pointer to the second vertex.
   * @return True if the left bus width is smaller than the right one.
   *
   * \~russian
   * @brief Сравнивает две вершины по ширине шины.
   * @param i_left Указатель на первую вершину.
   * @param i_right Указатель на вторую вершину.
   * @return `true`, если ширина левой шины меньше правой.
   */
  static bool hasSmallerWidth(const VertexPtr i_left, const VertexPtr i_right);

  /**
   * \~english
   * @brief Update the bus value during simulation.
   *
   * If the input value is not empty, it replaces the current value.
   *
   * @param i_value New bus value.
   *
   * \~russian
   * @brief Обновляет значение шины в ходе симуляции.
   * @param i_value Новое значение шины.
   */
  virtual void updateValueBus(std::string i_value = "");

private:
  /**
   * \~english
   * @brief Sets bus width.
   * @param i_width New width value.
   *
   * \~russian
   * @brief Устанавливает ширину шины.
   * @param i_width Новое значение ширины.
   */
  void setWidth(size_t i_width);
  std::string d_value;
  size_t d_width;
};

} // namespace CG_Graph