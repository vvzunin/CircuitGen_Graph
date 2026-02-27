#pragma once

/**
 * @file DefaultAuxiliaryMethods.hpp
 * @brief Вспомогательные методы: AuxMethodsGraph (replacer, dotReturnToString, TuplePrinter).
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 */
#include <fstream>
#include <sstream>
#include <string>

#include <CircuitGenGraph/GraphUtils.hpp>

#include "../../lib/fmt/core.h"

/// TODO: ToEnum
/// TODO: if need LineReader
/// TODO: if need CopyDirectory

/// namespace CG_Graph::AuxMethodsGraph was created to organize a set
/// of auxiliary functions and methods that can be used
/// in various parts of a software project.
/// Functions: print to an string stream, used to replace % to str,
/// format, needed for for backward compatibility (now fmt is used)
/// </summary>

namespace CG_Graph::AuxMethodsGraph {

/** @author Fuuulkrum7 <ilka747428@gmail.com> */
std::string replacer(const std::string &i_s, const std::string &i_r);
/** @author Vladimir Zunin <vzunin@hse.ru> */
std::string dotReturnToString(DotReturn dot);

// code from here https://gist.github.com/en4bz/f07ef13706c3ae3a4fb2
template<class Tuple, std::size_t N>
struct TuplePrinter {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  static void print(const std::string &fmt, std::ostream &os, const Tuple &t) {
    const size_t idx = fmt.find_last_of('%');
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    TuplePrinter<Tuple, N - 1>::print(std::string(fmt, 0, idx), os, t);
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    os << std::get<N - 1>(t) << std::string(fmt, idx + 1);
  }
};

template<class Tuple>
struct TuplePrinter<Tuple, 1> {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  static void print(const std::string &fmt, std::ostream &os, const Tuple &t) {
    const size_t idx = fmt.find_first_of('%');
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    os << std::string(fmt, 0, idx) << std::get<0>(t)
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
       << std::string(fmt, idx + 1);
  }
};

/// @brief A simple function, which calls fmt
/// @return formatted string
template<typename... Args>
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
auto format(Args &&...args)
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
    -> decltype(fmt::format(std::forward<Args>(args)...)) {
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
  return fmt::format(std::forward<Args>(args)...);
}

} // namespace CG_Graph::AuxMethodsGraph
