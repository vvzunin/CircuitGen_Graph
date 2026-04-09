#pragma once

#include <string>
#include <vector>
#include <memory>
#include "CircuitGenGraph/OrientedGraph.hpp"

namespace CG_Graph {

// Структура для хранения информации о портах эталонной модели
struct PortInfo {
    std::string name;
    int width; // 1 для однобитных, N для векторов [N-1:0]
    bool is_input;
};

class TestbenchGenerator {
public:
    // Главный метод генерации тестбенча
    static bool generate(std::shared_ptr<OrientedGraph> graph,
                         const std::string& goldenModelPath,
                         const std::string& outputTbPath);

private:
    // Вспомогательный метод для парсинга Verilog-файла эталона
    static std::vector<PortInfo> parseGoldenModel(const std::string& filepath, std::string& moduleName);
};

} // namespace CG_Graph