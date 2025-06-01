#pragma once

#include <map>

/// \namespace Contains all classes and functions, used for this graph
namespace CG_Graph {

/// \var Lookup table for AND operation. Used for simulation
const std::map<char, std::map<char, char>> tableAnd{
    {'0', {{'0', '0'}, {'1', '0'}, {'x', '0'}, {'z', '0'}}},
    {'1', {{'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'x', {{'0', '0'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', '0'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

/// \var Lookup table for NAND (not-and) operation. Used for simulation
const std::map<char, std::map<char, char>> tableNand{
    {'0', {{'0', '1'}, {'1', '1'}, {'x', '1'}, {'z', '1'}}},
    {'1', {{'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'x', {{'0', '1'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', '1'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

/// \var Lookup table for OR operation. Used for simulation
const std::map<char, std::map<char, char>> tableOr{
    {'0', {{'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'1', {{'0', '1'}, {'1', '1'}, {'x', '1'}, {'z', '1'}}},
    {'x', {{'0', 'x'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
};

/// \var Lookup table for NOR (not-or) operation. Used for simulation
const std::map<char, std::map<char, char>> tableNor{
    {'0', {{'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'1', {{'0', '0'}, {'1', '0'}, {'x', '0'}, {'z', '0'}}},
    {'x', {{'0', 'x'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
};

/// \var Lookup table for XOR operation. Used for simulation
const std::map<char, std::map<char, char>> tableXor{
    {'0', {{'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'1', {{'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'x', {{'0', 'x'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

/// \var Lookup table for XNOR (not-xor) operation. Used for simulation
const std::map<char, std::map<char, char>> tableXnor{
    {'0', {{'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'1', {{'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'x', {{'0', 'x'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

/// \var Lookup table for BUF operation. Used for simulation
const std::map<char, char> tableBuf{
    {'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}};
/// \var Lookup table for NOT operation. Used for simulation
const std::map<char, char> tableNot{
    {'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}};

} // namespace CG_Graph
