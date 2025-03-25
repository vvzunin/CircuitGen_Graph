#pragma once

#include <cassert>
#include <memory_resource>
#include <set>
#include <string>
#include <string_view>

namespace CG_Graph {

constexpr int DEFAULT_BUF = 114688;
constexpr int CHUNK_SIZE = 14336;

typedef unsigned char bytea;

/// @author Fuuulkrum7
/// @brief This is a custom allocator, which allocates firstly `buf_size`
/// bytes, and than, if requires, allocates additionally `chunk_size` bytes.
/// Pointers on all allocated blocks are stored in a vector, so, if it is
/// possible, calculate buf_size correctly to reduce time for deallocation
struct MultiLinearAllocator {
  // clang-format off

  /// @brief buf_size should be more than 128 bytes,
  /// as the biggest vertex has such size
  /// @param buf_size size for the first allocation
  /// @param chunk_size size for further allocations (usually is less
  /// than `buf_size` value)
  MultiLinearAllocator(size_t buf_size, size_t chunk_size)
      : buf_size(buf_size)
      , chunk_size(chunk_size) {
    assert(buf_size >= 128);
    assert(chunk_size >= 128);
    blocks.reserve(5);
    blocks.push_back(offset = new bytea[buf_size]);
  }

  // clang-format on

  MultiLinearAllocator &operator=(MultiLinearAllocator &&other) = delete;
  MultiLinearAllocator(MultiLinearAllocator &&other) = delete;
  MultiLinearAllocator &operator=(const MultiLinearAllocator &other) = delete;
  MultiLinearAllocator(const MultiLinearAllocator &other) = delete;

  ~MultiLinearAllocator() {
    for (auto *block: blocks) {
      delete[] block;
    }
  }

  /// @brief Gives out memory for given type and returns a pointer on an object
  /// of given type. If memory of size `buf_size` ends, allocates `chunk_size`
  /// memory. If allocation of required size is impossible, returns `nullptr`
  /// @tparam T type, memory for which should be given.
  /// @return pointer of type T
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

  void deallocate() {}

private:
  /// @brief simple aligner for memory address
  /// @tparam T type, align of which we should get
  template<typename T>
  void align() {
    size_t suboffset = (uintptr_t)offset & (alignof(T) - 1);
    if (suboffset) {
      offset += (alignof(T) - suboffset);
    }
  }

private:
  /// @brief all pinters on memory blocks, which have been allocated
  std::vector<bytea *> blocks;
  /// @brief pointer on position, from which allocation should be started (from
  /// `offset` to `offset + sizeof(T)`)
  bytea *offset;
  /// @brief size of the first buffer
  size_t buf_size;
  /// @brief size for further allocations
  size_t chunk_size;
};

/// @author Fuuulkrum7
/// @brief This class is used for memory management. It has a
/// `MultiLinearAllocator` for allocating memory for graph vertices and
/// monotonic_buffer_resource, which is used for memory allocation for set of
/// strings, where all vertices names are stored.
class GraphMemory {
public:
  // clang-format off

  /// @param buf_size size, which would be used for memory buffer reserve.
  /// By default we allocate memory for 1024 base vertices. Size of one vertex
  /// is supposed to be 112 bytes by default.
  /// @param chunk_size additional size, which would allocated, if buffer ends.
  /// By default we allocate memory for 128 base vertices. Size of one vertex
  /// is supposed to be 112 bytes by default.
  GraphMemory(
        size_t buf_size = DEFAULT_BUF,
        size_t chunk_size = CHUNK_SIZE)
      : d_vertexMemory(buf_size, chunk_size)
      , d_strings {&d_stringMemory}
  {}

  // clang-format on

  GraphMemory &operator=(GraphMemory &&other) = delete;
  GraphMemory(GraphMemory &&other) = delete;
  GraphMemory &operator=(const GraphMemory &other) = delete;
  GraphMemory(const GraphMemory &other) = delete;

  /// @brief puts string_view to strings set and returns a string_view on the
  /// added string
  /// @param s string to be stored
  /// @return string_view from string in the set
  std::string_view internalize(std::string_view s) {
    return *d_strings.emplace(s).first;
  }

  /// @brief puts string_view to strings set and returns a string_view on the
  /// added string
  /// @param s string to be stored
  /// @return string_view from string in the set
  std::string_view internalize(const std::string &s) {
    return *d_strings.emplace(s).first;
  }

  /// @brief allocates memory for graph vertex
  /// @tparam T type of graph vertex
  /// @return pointer to the required vertex type
  template<typename T>
  T *allocate() {
    return d_vertexMemory.allocate<T>();
  }

  /// @brief allocates memory using span allocator
  /// @tparam T type of allocated object
  /// @return pointer to allocated object
  template<typename T>
  T *allocateSpan() {
    return static_cast<T *>(d_spanMemory.allocate(sizeof(T), alignof(T)));
  }

private:
  /// @brief allocator for vertices
  MultiLinearAllocator d_vertexMemory;
  /// @brief allocator for span memory
  std::pmr::monotonic_buffer_resource d_spanMemory;
  /// @brief resource for memory set without deallocations
  std::pmr::monotonic_buffer_resource d_stringMemory;
  /// @brief set with all vertices names
  std::pmr::set<std::string> d_strings;
};

} // namespace CG_Graph
