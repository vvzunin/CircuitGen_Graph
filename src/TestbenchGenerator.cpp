#include "CircuitGenGraph/TestbenchGenerator.hpp"
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <cmath>

namespace CG_Graph {

std::vector<PortInfo> TestbenchGenerator::parseGoldenModel(const std::string& filepath, std::string& moduleName) {
    std::vector<PortInfo> ports;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open golden model file: " << filepath << "\n";
        return ports;
    }

    std::string line;
    // Ищем имя модуля
    std::regex moduleRegex(R"(\bmodule\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    // Ищем порты (input или output)
    std::regex portRegex(R"(\b(input|output)\s+(?:wire\s+|reg\s+)?(?:\[(\d+):(\d+)\]\s+)?([a-zA-Z0-9_,\s]+))");

    while (std::getline(file, line)) {
        std::smatch match;
        
        if (std::regex_search(line, match, moduleRegex)) {
            moduleName = match[1].str();
        }
        
        if (std::regex_search(line, match, portRegex)) {
            bool is_input = (match[1].str() == "input");
            
            int width = 1;
            if (match[2].matched && match[3].matched) {
                int msb = std::stoi(match[2].str());
                int lsb = std::stoi(match[3].str());
                width = std::abs(msb - lsb) + 1;
            }

            // Вытаскиваем имя порта (убираем запятые и точки с запятой)
            std::string port_name = match[4].str();
            port_name.erase(std::remove_if(port_name.begin(), port_name.end(), 
                            [](char c) { return c == ',' || c == ';' || c == ' '; }), port_name.end());
            
            if (!port_name.empty()) {
                ports.push_back({port_name, width, is_input});
            }
        }
    }
    return ports;
}

bool TestbenchGenerator::generate(std::shared_ptr<OrientedGraph> graph,
                                  const std::string& goldenModelPath,
                                  const std::string& outputTbPath) {
    if (!graph) return false;

    std::string goldenModuleName;
    std::vector<PortInfo> goldenPorts = parseGoldenModel(goldenModelPath, goldenModuleName);

    if (goldenModuleName.empty() || goldenPorts.empty()) {
        std::cerr << "Error: Failed to parse golden model.\n";
        return false;
    }

    auto graphInputs = graph->getVerticesByType(VertexTypes::input);
    auto graphOutputs = graph->getVerticesByType(VertexTypes::output);

    int totalGoldenInputs = 0;
    int totalGoldenOutputs = 0;
    for (const auto& p : goldenPorts) {
        if (p.is_input) totalGoldenInputs += p.width;
        else totalGoldenOutputs += p.width;
    }

    std::ofstream tbFile(outputTbPath);
    if (!tbFile.is_open()) return false;

    tbFile << "`timescale 1ns / 1ps\n\n";
    tbFile << "module tb_" << graph->getName() << "_vs_" << goldenModuleName << ";\n\n";

    tbFile << "    reg [" << (totalGoldenInputs - 1) << ":0] test_in;\n";
    tbFile << "    wire [" << (totalGoldenOutputs - 1) << ":0] golden_out;\n";
    tbFile << "    wire [" << (totalGoldenOutputs - 1) << ":0] graph_out;\n\n";

    // Инстанс Golden Model
    tbFile << "    // --- Golden Model ---\n";
    tbFile << "    " << goldenModuleName << " golden_inst (\n";
    int in_offset = 0, out_offset = 0;
    for (size_t i = 0; i < goldenPorts.size(); ++i) {
        const auto& p = goldenPorts[i];
        tbFile << "        ." << p.name << "(";
        if (p.is_input) {
            if (p.width == 1) tbFile << "test_in[" << in_offset << "]";
            else tbFile << "test_in[" << (in_offset + p.width - 1) << ":" << in_offset << "]";
            in_offset += p.width;
        } else {
            if (p.width == 1) tbFile << "golden_out[" << out_offset << "]";
            else tbFile << "golden_out[" << (out_offset + p.width - 1) << ":" << out_offset << "]";
            out_offset += p.width;
        }
        tbFile << ")" << ((i == goldenPorts.size() - 1) ? "" : ",") << "\n";
    }
    tbFile << "    );\n\n";

    // Инстанс OrientedGraph
    tbFile << "    // --- Graph Model ---\n";
    tbFile << "    " << graph->getName() << " graph_inst (\n";
    for (size_t i = 0; i < graphInputs.size(); ++i) {
        tbFile << "        ." << graphInputs[i]->getName() << "(test_in[" << i << "]),\n";
    }
    for (size_t i = 0; i < graphOutputs.size(); ++i) {
        tbFile << "        ." << graphOutputs[i]->getName() << "(graph_out[" << i << "])" 
               << ((i == graphOutputs.size() - 1) ? "" : ",\n");
    }
    tbFile << "    );\n\n";

    // Полный перебор
    tbFile << "    integer i;\n";
    tbFile << "    integer errors = 0;\n\n";
    tbFile << "    initial begin\n";
    tbFile << "        for (i = 0; i < (1 << " << totalGoldenInputs << "); i = i + 1) begin\n";
    tbFile << "            test_in = i;\n";
    tbFile << "            #10;\n";
    tbFile << "            if (golden_out !== graph_out) begin\n";
    tbFile << "                $display(\"ERROR! Input: %b | Golden: %b | Graph: %b\", test_in, golden_out, graph_out);\n";
    tbFile << "                errors = errors + 1;\n";
    tbFile << "            end\n";
    tbFile << "        end\n\n";
    tbFile << "        if (errors == 0) $display(\"SUCCESS! All combinations passed.\");\n";
    tbFile << "        else $display(\"FAILED with %0d errors.\", errors);\n";
    tbFile << "        $finish;\n";
    tbFile << "    end\n\n";
    tbFile << "endmodule\n";

    tbFile.close();
    return true;
}

} // namespace CG_Graph