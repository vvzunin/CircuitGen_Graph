#pragma once

#include <memory_resource>
#include <set>
#include <string>
#include <string_view>

class GraphMemory {
public:
  GraphMemory() {}

  std::string_view internalize(std::string_view s) {
    return *strings.emplace(s).first;
  }

  std::string_view internalize(const std::string &s) {
    return *strings.emplace(s).first;
  }

private:
  std::set<std::string> strings;
};
