#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <tuple>

#include <CircuitGenGraph/DefaultSettings.hpp>

/// @file AuxiliaryMethods.hpp
/// TODO: ToEnum
/// TODO: if need LineReader
/// TODO: if need CopyDirectory

/// namespace AuxMethods It was created to organize a set of auxiliary
/// functions and methods that can be used in various parts of a
/// software project
/// Functions: Random number generation, Reading files,
/// Data sorting and processing (includes methods for sorting
/// dictionaries by their values and for working with spaces in strings)
/// TO DO: list the other functions
/// </summary>

namespace AuxMethods {

std::string replacer(const std::string &i_s, const std::string &i_r);
std::string dotReturnToString(DotReturn dot);

// code from here https://gist.github.com/en4bz/f07ef13706c3ae3a4fb2
template<class Tuple, std::size_t N>
struct TuplePrinter {
  static void print(const std::string &fmt, std::ostream &os, const Tuple &t) {
    const size_t idx = fmt.find_last_of('%');
    TuplePrinter<Tuple, N - 1>::print(std::string(fmt, 0, idx), os, t);
    os << std::get<N - 1>(t) << std::string(fmt, idx + 1);
  }
};

template<class Tuple>
struct TuplePrinter<Tuple, 1> {
  static void print(const std::string &fmt, std::ostream &os, const Tuple &t) {
    const size_t idx = fmt.find_first_of('%');
    os << std::string(fmt, 0, idx) << std::get<0>(t)
       << std::string(fmt, idx + 1);
  }
};

template<class... Args>
std::string format(const std::string &fmt, Args &&...args) {
  std::stringstream ss;

  const auto t = std::make_tuple(std::forward<Args>(args)...);

  TuplePrinter<decltype(t), sizeof...(Args)>::print(fmt, ss, t);
  return ss.str();
}

} // namespace AuxMethods