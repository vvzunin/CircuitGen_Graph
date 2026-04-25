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

} // namespace CG_Graph