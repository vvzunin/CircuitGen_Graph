#pragma once

#include <memory_resource>
#include <set>
#include <string>
#include <string_view>

class GraphMemory {
public:
  GraphMemory() : strings {&memory} {}

  GraphMemory& operator=(GraphMemory&& other)      = delete;
  GraphMemory(GraphMemory&& other)                 = delete;
  GraphMemory& operator=(const GraphMemory& other) = delete;
  GraphMemory(const GraphMemory& other)            = delete;

  std::string_view internalize(std::string_view s) {
    return *strings.emplace(s).first;
  }

  std::string_view internalize(const std::string& s) {
    return *strings.emplace(s).first;
  }

private:
  std::pmr::monotonic_buffer_resource memory;
  std::pmr::set<std::string>          strings;
};
