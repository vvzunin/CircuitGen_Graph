#pragma once

#include <cassert>
#include <memory_resource>
#include <set>
#include <string>
#include <string_view>

#define DEFAULT_BUF 114688
#define CHUNK_SIZE 14336

typedef unsigned char bytea;


/// @author Fuuulkrum7
struct MultiLinearAllocator {
  MultiLinearAllocator(size_t buf_size, size_t chunk_size)
      : buf_size(buf_size)
      , chunk_size(chunk_size) {
    assert(buf_size >= 96);
    assert(chunk_size >= 96);
    blocks.reserve(5);
    blocks.push_back(offset = new bytea[buf_size]);
  }

  ~MultiLinearAllocator() {
    for (auto block: blocks) {
      delete[] block;
    }
  }

  template<typename T>
  T* allocate() {
    bytea *current = offset;
    offset += sizeof(T);
    align<T>();
    if (offset > blocks.back() + buf_size) {
      // bad alloc - memory was taken, but allocation is impossible, and we try
      // to allocate more memory, than is located in buffer
      if (offset == blocks.back()) {
        return nullptr;
      }
      blocks.push_back(offset = new bytea[chunk_size]);
      buf_size = chunk_size;
      return allocate<T>();
    }
    return reinterpret_cast<T*>(current);
  }

  void deallocate() {}

private:
  template<typename T>
  void align() {
    size_t suboffset = (uintptr_t)offset & (alignof(T) - 1);
    if (suboffset) {
      offset += (alignof(T) - suboffset);
    }
  }

private:
  std::vector<bytea *> blocks;
  bytea* offset;
  size_t buf_size;
  size_t chunk_size;
};

/// @author Fuuulkrum7
class GraphMemory {
public:
  /// @param buf_size size, which would be used for memory buffer reserve.
  /// By default we allocate memory for 1024 base verticies. Size of one vertex
  /// is supposed to be 80 bytes by default.
  /// @param chunk_size additional size, which would allocated, if buffer ends.
  /// By default we allocate memory for 128 base verticies. Size of one vertex
  /// is supposed to be 80 bytes by default.
  GraphMemory(
        size_t buf_size = DEFAULT_BUF,
        size_t chunk_size = CHUNK_SIZE)
      : d_vertexMemory(buf_size, chunk_size)
      , d_strings {&d_stringMemory}
  {}

  GraphMemory& operator=(GraphMemory&& other)      = delete;
  GraphMemory(GraphMemory&& other)                 = delete;
  GraphMemory& operator=(const GraphMemory& other) = delete;
  GraphMemory(const GraphMemory& other)            = delete;

  std::string_view internalize(std::string_view s) {
    return *d_strings.emplace(s).first;
  }

  std::string_view internalize(const std::string& s) {
    return *d_strings.emplace(s).first;
  }

  template<typename T>
  T* allocate() {
    return d_vertexMemory.allocate<T>();
  }

private:
  MultiLinearAllocator d_vertexMemory;

  std::pmr::monotonic_buffer_resource d_stringMemory;
  std::pmr::set<std::string>          d_strings;
};
