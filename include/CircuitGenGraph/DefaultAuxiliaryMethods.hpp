/**
 * @file DefaultAuxiliaryMethods.hpp
 * @brief Вспомогательные методы: AuxMethodsGraph (replacer, dotReturnToString,
 * TuplePrinter).
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 */
#pragma once
#include <fstream>
#include <sstream>
#include <string>

#include <CircuitGenGraph/GraphUtils.hpp>

#include "fmt/core.h"

/*!
 * \file AuxiliaryMethods.hpp
 *
 * \~english
 * @todo ToEnum
 * @todo if need LineReader
 * @todo if need CopyDirectory
 *
 * \~russian
 * @todo ToEnum
 * @todo если нужен LineReader (построчное чтение)
 * @todo если нужен CopyDirectory (копирование директорий)
 */

/*!
 * <summary>
 * \~english
 * @brief namespace CG_Graph::AuxMethodsGraph was created to organize a set
 * of auxiliary functions and methods that can be used in various parts of a
 * software project.
 * Functions: print to an string stream, used to replace % to str,
 * format, needed for for backward compatibility (now fmt is used)
 *
 * \~russian
 * @brief Пространство имен CG_Graph::AuxMethodsGraph создано для
 * организации набора вспомогательных функций и методов, которые могут
 * применяться в различных частях программного проекта.
 * Функции: вывод в строковый поток; замена % на строку;
 * format — оставлен для обратной совместимости (сейчас используется fmt).
 * </summary>
 */
namespace CG_Graph::AuxMethodsGraph {

/**
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 */
std::string replacer(const std::string &i_s, const std::string &i_r);

/**
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 */
std::string dotReturnToString(DotReturn dot);

/**
 * @details code from here https://gist.github.com/en4bz/f07ef13706c3ae3a4fb2
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 */
template<class Tuple, std::size_t N>
struct TuplePrinter {
  /**
   * @author Vladimir Zunin <vzunin@hse.ru>
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   */
  static void print(const std::string &fmt, std::ostream &os, const Tuple &t) {
    const size_t idx = fmt.find_last_of('%');
    TuplePrinter<Tuple, N - 1>::print(std::string(fmt, 0, idx), os, t);
    os << std::get<N - 1>(t) << std::string(fmt, idx + 1);
  }
};

/**
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 */
template<class Tuple>
/** @author Fuuulkrum7 <ilka747428@gmail.com> */
struct TuplePrinter<Tuple, 1> {
  /**
   * @author Vladimir Zunin <vzunin@hse.ru>
   * @author Fuuulkrum7 <ilka747428@gmail.com>
   */
  static void print(const std::string &fmt, std::ostream &os, const Tuple &t) {
    const size_t idx = fmt.find_first_of('%');
    os << std::string(fmt, 0, idx) << std::get<0>(t)
       << std::string(fmt, idx + 1);
  }
};

/*!
 * @author Vladimir Zunin <vzunin@hse.ru>
 * @author Fuuulkrum7 <ilka747428@gmail.com>
 *
 * \~english
 * @brief A simple function, which calls fmt
 * @return formatted string
 *
 * \~russian
 * @brief Простая функция, вызывающая fmt
 * @return отформатированная строка
 */
template<typename... Args>
auto format(Args &&...args)
    -> decltype(fmt::format(std::forward<Args>(args)...)) {
  return fmt::format(std::forward<Args>(args)...);
}

} // namespace CG_Graph::AuxMethodsGraph