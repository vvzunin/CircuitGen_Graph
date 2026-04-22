/**
 * @file GraphMemory.hpp
 * @brief Аллокатор MultiLinearAllocator и утилиты управления памятью графа.
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 * @author Theossr <feolab05@gmail.com>
 */
#pragma once
#include <cassert>
#include <memory_resource>
#include <set>
#include <string>
#include <string_view>

namespace CG_Graph {

constexpr int DEFAULT_BUF = 106496;
constexpr int CHUNK_SIZE = 13312;

typedef unsigned char bytea;

/**
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief This is a custom allocator, which allocates firstly `buf_size`
 * bytes, and then, if required, allocates additionally `chunk_size` bytes.
 * Pointers on all allocated blocks are stored in a vector, so, if it is
 * possible, calculate buf_size correctly to reduce time for deallocation.
 *
 * \~russian
 * @brief Это пользовательский аллокатор, который сначала выделяет
 * `buf_size` байт, а затем, при необходимости, дополнительно выделяет
 * `chunk_size` байт. Указатели на все выделенные блоки хранятся в
 * векторе, поэтому, по возможности, правильно рассчитывайте buf_size,
 * чтобы сократить время на освобождение памяти.
 */
struct MultiLinearAllocator {
  // clang-format off

  /*!
   * \~english
   * @brief buf_size should be more than 154 bytes, as the biggest vertex
   * has such size
   * @param buf_size size for the first allocation
   * @param chunk_size size for further allocations (usually is less than
   * `buf_size` value)
   *
   * \~russian
   * @brief buf_size должен быть больше 154 байт, так как самая большая
   * вершина имеет такой размер
   * @param buf_size размер для первого выделения памяти
   * @param chunk_size размер для последующих выделений памяти (обычно
   * меньше значения `buf_size`)
   */
  MultiLinearAllocator(size_t buf_size, size_t chunk_size)
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
      : buf_size(buf_size)
      , chunk_size(chunk_size) {
    assert(buf_size >= 154);
    assert(chunk_size >= 154);
    blocks.reserve(5);
    blocks.push_back(offset = new bytea[buf_size]);
  }

  // clang-format on

  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  MultiLinearAllocator &operator=(MultiLinearAllocator &&other) = delete;
  MultiLinearAllocator(MultiLinearAllocator &&other) = delete;
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  MultiLinearAllocator &operator=(const MultiLinearAllocator &other) = delete;
  MultiLinearAllocator(const MultiLinearAllocator &other) = delete;

  ~MultiLinearAllocator() {
    for (auto *block: blocks) {
      delete[] block;
    }
  }

  /*!
   * \brief
   * \~english
   * Gives out memory for given type and returns a pointer on an object
   * of given type. If memory of size `buf_size` ends, allocates
   * `chunk_size` memory. If allocation of required size is impossible,
   * returns `nullptr`
   * \tparam T type, memory for which should be given.
   * \return pointer of type T
   *
   * \~russian
   * Выдает память для заданного типа и возвращает указатель на объект
   * этого типа. Если память размера `buf_size` заканчивается, выделяет
   * память размера `chunk_size`. Если выделение требуемого размера
   * невозможно, возвращает `nullptr`
   * \tparam T тип, для которого должна быть выделена память.
   * \return указатель типа T
   */
  template<typename T>
  T *allocate() {
    bytea *current = offset;
    offset += sizeof(T);
    align<T>();
    if (offset > blocks.back() + buf_size) {
      // bad alloc - memory was taken, but allocation is impossible, and we try
      // to allocate more memory, than is located in buffer
      if (current == blocks.back()) {
        return nullptr;
      }
      blocks.push_back(offset = new bytea[chunk_size]);
      buf_size = chunk_size;
      return allocate<T>();
    }
    return reinterpret_cast<T *>(current);
  }

  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  void deallocate() {}

private:
  /*!
   * \~english
   * \brief simple aligner for memory address
   * \tparam T type, align of which we should get
   *
   * \~russian
   * \brief простой выравниватель для адресов памяти
   * \tparam T тип, выравнивание которого мы должны получить
   */
  template<typename T>
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  void align() {
    size_t suboffset = (uintptr_t)offset & (alignof(T) - 1);
    if (suboffset) {
      offset += (alignof(T) - suboffset);
    }
  }

private:
  /**
   * \~english
   * @brief all pointers on memory blocks, which have been allocated
   *
   * \~russian
   * @brief все указатели на блоки памяти, которые были выделены
   */
  std::vector<bytea *> blocks;

  /*!
   * \~english
   * \brief pointer on position, from which allocation should be started
   * (from `offset` to `offset + sizeof(T)`)
   *
   * \~russian
   * \brief указатель на позицию, с которой должно начинаться выделение
   * (от `offset` до `offset + sizeof(T)`)
   */
  bytea *offset;

  /*!
   * \~english
   * \brief size of the first buffer
   *
   * \~russian
   * \brief размер первого буфера
   */
  size_t buf_size;

  /*!
   * \~english
   * \brief size for further allocations
   *
   * \~russian
   * \brief размер для последующих выделений памяти
   */
  size_t chunk_size;
};

/**
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief This class is used for memory management. It has a
 * `MultiLinearAllocator` for allocating memory for graph vertices and
 * monotonic_buffer_resource, which is used for memory allocation for set
 * of strings, where all vertices names are stored.
 *
 * \~russian
 * @brief Этот класс используется для управления памятью. Он содержит
 * `MultiLinearAllocator` для выделения памяти под вершины графа и
 * monotonic_buffer_resource, который используется для выделения памяти
 * под множество строк, где хранятся все имена вершин.
 */
class GraphMemory {
public:
  // clang-format off

  /*!
   * \~english
   * @param buf_size size, which would be used for memory buffer reserve.
   * By default we allocate memory for 1024 base vertices. Size of one
   * vertex is supposed to be 112 bytes by default.
   * @param chunk_size additional size, which would be allocated, if buffer
   * ends. By default we allocate memory for 128 base vertices. Size of one
   * vertex is supposed to be 112 bytes by default.
   *
   * \~russian
   * @param buf_size размер, который будет использоваться для резерва
   * буфера памяти. По умолчанию выделяется память для 1024 базовых вершин.
   * Предполагается, что размер одной вершины по умолчанию составляет
   * 112 байт.
   * @param chunk_size дополнительный размер, который будет выделен, если
   * буфер закончится. По умолчанию выделяется память для 128 базовых
   * вершин. Предполагается, что размер одной вершины по умолчанию
   * составляет 112 байт.
   */
  GraphMemory(
        size_t buf_size = DEFAULT_BUF,
        size_t chunk_size = CHUNK_SIZE)
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
      : d_vertexMemory(buf_size, chunk_size)
      , d_strings {&d_stringMemory}
  {}

  // clang-format on

  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  GraphMemory &operator=(GraphMemory &&other) = delete;
  GraphMemory(GraphMemory &&other) = delete;
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  GraphMemory &operator=(const GraphMemory &other) = delete;
  GraphMemory(const GraphMemory &other) = delete;

  /**
   * \~english
   * @brief puts string_view to strings set and returns a string_view on
   * the added string
   * @param s string to be stored
   * @return string_view from string in the set
   *
   * \~russian
   * @brief помещает string_view в множество строк и возвращает
   * string_view на добавленную строку
   * @param s сохраняемая строка
   * @return string_view из строки в множестве
   */
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  std::string_view internalize(std::string_view s) {
    return *d_strings.emplace(s).first;
  }

  /**
   * \~english
   * @brief puts string_view to strings set and returns a string_view on
   * the added string
   * @param s string to be stored
   * @return string_view from string in the set
   *
   * \~russian
   * @brief помещает string_view в множество строк и возвращает
   * string_view на добавленную строку
   * @param s сохраняемая строка
   * @return string_view из строки в множестве
   */
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  std::string_view internalize(const std::string &s) {
    return *d_strings.emplace(s).first;
  }

  /**
   * \~english
   * @brief puts string_view to strings set and returns a string_view on
   * the added string
   * @param s string to be stored
   * @return string_view from string in the set
   *
   * \~russian
   * @brief помещает string_view в множество строк и возвращает
   * string_view на добавленную строку
   * @param s сохраняемая строка
   * @return string_view из строки в множестве
   */
  /** @author Fuuulkrum7 <ilka747428@gmail.com> */
  std::string_view internalize(const std::string &s) {
    return *d_strings.emplace(s).first;
  }

  /*!
   * \~english
   * \brief allocates memory for graph vertex
   * \tparam T type of graph vertex
   * \return pointer to the required vertex type
   *
   * \~russian
   * \brief выделяет память для вершины графа
   * \tparam T тип вершины графа
   * \return указатель на требуемый тип вершины
   */
  template<typename T>
  T *allocate() {
    return d_vertexMemory.allocate<T>();
  }

private:
  /*!
   * \~english
   * \brief allocator for vertices
   *
   * \~russian
   * \brief аллокатор для вершин
   */
  MultiLinearAllocator d_vertexMemory;

  /*!
   * \~english
   * \brief resource for memory set without deallocations
   *
   * \~russian
   * \brief ресурс для памяти множества без освобождений (deallocations)
   */
  std::pmr::monotonic_buffer_resource d_stringMemory;

  /*!
   * \~english
   * \brief set with all vertices names
   *
   * \~russian
   * \brief множество со всеми именами вершин
   */
  std::pmr::set<std::string> d_strings;
};

} // namespace CG_Graph