#pragma once

#include <memory_resource>
#include <set>
#include <string>
#include <string_view>

typedef unsigned char bytea;

class LinearAllocator {

};

/// @author Fuuulkrum7
class GraphMemory {
public:
  /// @param initial_size size, which would be used for memory buffer reserve.
  /// By default we allocate memory for 1024 verticies. Size of one vertex is
  /// suuposed to be 64 bytes by default.
  GraphMemory() : d_strings {&d_stringMemory} {
    // TODO how to use buffer????
  }

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
    return static_cast<T*>(d_vertexMemory.allocate(sizeof(T), alignof(T)));
  }

  // template<typename T>
  // T* allocateForGraph() {
  //   return static_cast<T*>(d_graphMemory.allocate(sizeof(T), alignof(T)));
  // }

  std::pmr::memory_resource& getVertexResource() { return d_vertexMemory; }

  // std::pmr::memory_resource& getGraphResource() { return d_graphMemory; }

private:
  std::pmr::monotonic_buffer_resource d_vertexMemory;
  // std::pmr::monotonic_buffer_resource d_graphMemory;
  std::pmr::monotonic_buffer_resource d_stringMemory;

  std::pmr::set<std::string>          d_strings;
};
