#pragma once

#include <map>

const std::map<char, std::map<char, char>> tableAnd {
    {'0', {{'0', '0'}, {'1', '0'}, {'x', '0'}, {'z', '0'}}},
    {'1', {{'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'x', {{'0', '0'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', '0'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

const std::map<char, std::map<char, char>> tableNand {
    {'0', {{'0', '1'}, {'1', '1'}, {'x', '1'}, {'z', '1'}}},
    {'1', {{'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'x', {{'0', '1'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', '1'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

const std::map<char, std::map<char, char>> tableOr {
    {'0', {{'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'1', {{'0', '1'}, {'1', '1'}, {'x', '1'}, {'z', '1'}}},
    {'x', {{'0', 'x'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
};

const std::map<char, std::map<char, char>> tableNor {
    {'0', {{'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'1', {{'0', '0'}, {'1', '0'}, {'x', '0'}, {'z', '0'}}},
    {'x', {{'0', 'x'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
};

const std::map<char, std::map<char, char>> tableXor {
    {'0', {{'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'1', {{'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'x', {{'0', 'x'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

const std::map<char, std::map<char, char>> tableXnor {
    {'0', {{'0', '1'}, {'1', '0'}, {'x', 'x'}, {'z', 'x'}}},
    {'1', {{'0', '0'}, {'1', '1'}, {'x', 'x'}, {'z', 'x'}}},
    {'x', {{'0', 'x'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

const std::map<char, char> tableBuf {
    {'0', '0'},
    {'1', '1'},
    {'x', 'x'},
    {'z', 'x'}};
const std::map<char, char> tableNot {
    {'0', '1'},
    {'1', '0'},
    {'x', 'x'},
    {'z', 'x'}};

// L - a value 0 or z.
// H - a value 1 or z.
const std::map<char, std::map<char, char>> tableBufIf0 {
    {'0', {{'0', '0'}, {'1', 'z'}, {'x', 'L'}, {'z', 'L'}}},
    {'1', {{'0', '1'}, {'1', 'z'}, {'x', 'H'}, {'z', 'H'}}},
    {'x', {{'0', 'x'}, {'1', 'z'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', 'z'}, {'x', 'x'}, {'z', 'x'}}},
};

const std::map<char, std::map<char, char>> tableBufIf1 {
    {'0', {{'0', 'z'}, {'1', '0'}, {'x', 'L'}, {'z', 'L'}}},
    {'1', {{'0', 'z'}, {'1', '1'}, {'x', 'H'}, {'z', 'H'}}},
    {'x', {{'0', 'z'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'z'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};

const std::map<char, std::map<char, char>> tableNotIf0 {
    {'0', {{'0', '1'}, {'1', 'z'}, {'x', 'H'}, {'z', 'H'}}},
    {'1', {{'0', '0'}, {'1', 'z'}, {'x', 'L'}, {'z', 'L'}}},
    {'x', {{'0', 'x'}, {'1', 'z'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'x'}, {'1', 'z'}, {'x', 'x'}, {'z', 'x'}}},
};

const std::map<char, std::map<char, char>> tableNotIf1 {
    {'0', {{'0', 'z'}, {'1', '1'}, {'x', 'H'}, {'z', 'H'}}},
    {'1', {{'0', 'z'}, {'1', '0'}, {'x', 'L'}, {'z', 'L'}}},
    {'x', {{'0', 'z'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
    {'z', {{'0', 'z'}, {'1', 'x'}, {'x', 'x'}, {'z', 'x'}}},
};
