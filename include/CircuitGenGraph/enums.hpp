/**
 * @file enums.hpp
 * @author Fuuulkrum7
 *
 * \~english
 * @brief Enumerations and truth tables for graph simulation (Gates,
 * VertexTypes, etc.).
 *
 * \~russian
 * @brief Перечисления и таблицы истинности для симуляции графа (Gates,
 * VertexTypes и др.).
 */
#pragma once
#include <map>
#include <tuple>

/**
 * @namespace CG_Graph
 *
 * \~english
 * @brief Contains all classes and functions, used for this graph
 *
 * \~russian
 * @brief Содержит все классы и функции, используемые для этого графа
 */
namespace CG_Graph {

/**
 * \~english
 * @brief tableAnd Lookup table for AND operation. Used for simulation
 *
 * \~russian
 * @brief Таблица поиска для операции И (AND). Используется для симуляции
 */
const std::map<char, std::map<char, char>> tableAnd{
    {'0', {{'0', '0'}, {'1', '0'}, {'x', '0'}, {'z', '0'}}},
    {'1', {{'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'x', {{'0', '0'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', '0'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

/**
 * \~english
 * @brief tableNand Lookup table for NAND (not-and) operation. Used for
 * simulation
 *
 * \~russian
 * @brief Таблица поиска для операции И-НЕ (NAND). Используется для
 * симуляции
 */
const std::map<char, std::map<char, char>> tableNand{
    {'0', {{'0', '1'}, {'1', '1'}, {'x', '1'}, {'z', '1'}}},
    {'1', {{'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'x', {{'0', '1'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', '1'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

/**
 * \~english
 * @brief tableOr Lookup table for OR operation. Used for simulation
 *
 * \~russian
 * @brief Таблица поиска для операции ИЛИ (OR). Используется для симуляции
 */
const std::map<char, std::map<char, char>> tableOr{
    {'0', {{'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'1', {{'0', '1'}, {'1', '1'}, {'x', '1'}, {'z', '1'}}},
    {'x', {{'0', 'x'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
};

/**
 * \~english
 * @brief tableNor Lookup table for NOR (not-or) operation. Used for
 * simulation
 *
 * \~russian
 * @brief Таблица поиска для операции ИЛИ-НЕ (NOR). Используется для
 * симуляции
 */
const std::map<char, std::map<char, char>> tableNor{
    {'0', {{'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'1', {{'0', '0'}, {'1', '0'}, {'x', '0'}, {'z', '0'}}},
    {'x', {{'0', 'x'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
};

/**
 * \~english
 * @brief tableXor Lookup table for XOR operation. Used for simulation
 *
 * \~russian
 * @brief Таблица поиска для операции ИСКЛЮЧАЮЩЕЕ ИЛИ (XOR). Используется
 * для симуляции
 */
const std::map<char, std::map<char, char>> tableXor{
    {'0', {{'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'1', {{'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'x', {{'0', 'x'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

/**
 * \~english
 * @brief tableXnor Lookup table for XNOR (not-xor) operation. Used for
 * simulation
 *
 * \~russian
 * @brief Таблица поиска для операции ИСКЛЮЧАЮЩЕЕ ИЛИ-НЕ (XNOR).
 * Используется для симуляции
 */
const std::map<char, std::map<char, char>> tableXnor{
    {'0', {{'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'1', {{'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'x', {{'0', 'x'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

/**
 * \~english
 * @brief tableBuf Lookup table for BUF operation. Used for simulation
 *
 * \~russian
 * @brief Таблица поиска для операции БУФЕР (BUF). Используется для
 * симуляции
 */
const std::map<char, char> tableBuf{
    {'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}};

/**
 * \~english
 * @brief tableNot Lookup table for NOT operation. Used for simulation
 *
 * \~russian
 * @brief Таблица поиска для операции НЕ (NOT). Используется для симуляции
 */
const std::map<char, char> tableNot{
    {'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}};

/**
 * \~english
 * @brief tableBufIf0 Lookup table for BUFIF0 (buffer if control is 0).
 * First key is data, second key is control. L means 0 or z; H means 1 or z.
 *
 * \~russian
 * @brief Таблица поиска для BUFIF0 (буфер, если управление равно 0).
 * Первый ключ — данные, второй — управление. L — 0 или z; H — 1 или z.
 */
const std::map<char, std::map<char, char>> tableBufIf0{
    {'0', {{'0', '0'}, {'1', 'z'}, {'x', 'L'}, {'z', 'L'}}},
    {'1', {{'0', '1'}, {'1', 'z'}, {'x', 'H'}, {'z', 'H'}}},
    {'x', {{'0', 'x'}, {'1', 'z'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', 'z'}, {'x', 'x'}, {'z', 'x'}}},
};

/**
 * \~english
 * @brief tableBufIf1 Lookup table for BUFIF1 (buffer if control is 1).
 * First key is data, second key is control. L means 0 or z; H means 1 or z.
 *
 * \~russian
 * @brief Таблица поиска для BUFIF1 (буфер, если управление равно 1).
 * Первый ключ — данные, второй — управление. L — 0 или z; H — 1 или z.
 */
const std::map<char, std::map<char, char>> tableBufIf1{
    {'0', {{'0', 'z'}, {'1', '0'}, {'x', 'L'}, {'z', 'L'}}},
    {'1', {{'0', 'z'}, {'1', '1'}, {'x', 'H'}, {'z', 'H'}}},
    {'x', {{'0', 'z'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'z'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

/**
 * \~english
 * @brief tableNotIf0 Lookup table for NOTIF0 (inverter if control is 0).
 * First key is data, second key is control. L means 0 or z; H means 1 or z.
 *
 * \~russian
 * @brief Таблица поиска для NOTIF0 (инвертор, если управление равно 0).
 * Первый ключ — данные, второй — управление. L — 0 или z; H — 1 или z.
 */
const std::map<char, std::map<char, char>> tableNotIf0{
    {'0', {{'0', '1'}, {'1', 'z'}, {'x', 'H'}, {'z', 'H'}}},
    {'1', {{'0', '0'}, {'1', 'z'}, {'x', 'L'}, {'z', 'L'}}},
    {'x', {{'0', 'x'}, {'1', 'z'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', 'z'}, {'x', 'x'}, {'z', 'x'}}},
};

/**
 * \~english
 * @brief tableNotIf1 Lookup table for NOTIF1 (inverter if control is 1).
 * First key is data, second key is control. L means 0 or z; H means 1 or z.
 *
 * \~russian
 * @brief Таблица поиска для NOTIF1 (инвертор, если управление равно 1).
 * Первый ключ — данные, второй — управление. L — 0 или z; H — 1 или z.
 */
const std::map<char, std::map<char, char>> tableNotIf1{
    {'0', {{'0', 'z'}, {'1', '1'}, {'x', 'H'}, {'z', 'H'}}},
    {'1', {{'0', 'z'}, {'1', '0'}, {'x', 'L'}, {'z', 'L'}}},
    {'x', {{'0', 'z'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'z'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

/**
 * \~english
 * @brief tableMAJ3 Lookup table for 3-input majority. Key is (a, b, c).
 * Defined for 0/1 inputs only.
 *
 * \~russian
 * @brief Таблица поиска для majority на 3 входа. Ключ — (a, b, c).
 * Определена только для входов 0/1.
 */
const std::map<std::tuple<char, char, char>, char> tableMAJ3{
    {{'0', '0', '0'}, '0'}, {{'0', '0', '1'}, '0'}, {{'0', '1', '0'}, '0'},
    {{'0', '1', '1'}, '1'}, {{'1', '0', '0'}, '0'}, {{'1', '0', '1'}, '1'},
    {{'1', '1', '0'}, '1'}, {{'1', '1', '1'}, '1'},
};

/**
 * \~english
 * @brief tableNMAJ3 Lookup table for 3-input not-majority. Key is (a, b, c).
 * Defined for 0/1 inputs only.
 *
 * \~russian
 * @brief Таблица поиска для not-majority на 3 входа. Ключ — (a, b, c).
 * Определена только для входов 0/1.
 */
const std::map<std::tuple<char, char, char>, char> tableNMAJ3{
    {{'0', '0', '0'}, '1'}, {{'0', '0', '1'}, '1'}, {{'0', '1', '0'}, '1'},
    {{'0', '1', '1'}, '0'}, {{'1', '0', '0'}, '1'}, {{'1', '0', '1'}, '0'},
    {{'1', '1', '0'}, '0'}, {{'1', '1', '1'}, '0'},
};

} // namespace CG_Graph