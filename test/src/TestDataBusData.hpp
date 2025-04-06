#include <string_view>

namespace TestData {
constexpr std::string_view DATA_BUS_INPUT_TEST =
    "module graph_0(\n"
    "\tinput1, input2, input3, \n"
    "\toutput1, output2\n"
    ");\n"
    "\t// Writing inputs\n"
    "\tinput [2:0] input_bus;\n"
    "\tinput input1, input2, input3;\n"
    "\t// Writing outputs\n"
    "\toutput [1:0] output_bus;\n"
    "\toutput output1, output2;\n"
    "\t// Writing wires\n"
    "\twire [2:0] const_bus;\n"
    "\tassign const_bus = 3'b101;\n"
    "endmodule\n";

constexpr std::string_view DATA_BUS_SEQ_TEST =
    "module graph_0(\n"
    "\tclk, data, rst, set, en, \n"
    "\tseq_out\n"
    ");\n"
    "\t// Writing inputs\n"
    "\tinput clk, data, rst, set, en;\n"
    "\t// Writing outputs\n"
    "\toutput seq_out;\n"
    "\t// Writing registers\n"
    "\treg [2:0] seq_bus;\n"
    "\treg q;\n\t\n"
    "\talways @(posedge clk) begin\n"
    "\t\tif (!rst) q <= 1'b0;\n"
    "\t\telse if (set) q <= 1'b1;\n"
    "\t\telse if (en) q <= data;\n"
    "\tend\n\n"
    "\talways @(posedge clk) begin\n"
    "\t\tif (!rst) seq_bus <= 3'b000;\n"
    "\t\telse if (set) seq_bus <= 3'b111;\n"
    "\t\telse if (en) seq_bus <= {data, data, data};\n"
    "\tend\n\n"
    "\tassign seq_out = q;\n"
    "endmodule\n";

constexpr std::string_view DATA_BUS_REGULAR_TEST =
    "module graph_0(\n"
    "\tinput1, input2, input3, \n"
    "\toutput1, output2\n"
    ");\n"
    "\t// Writing inputs\n"
    "\tinput [2:0] input_bus;\n"
    "\tinput input1, input2, input3;\n"
    "\t// Writing outputs\n"
    "\toutput [1:0] output_bus;\n"
    "\toutput output1, output2;\n"
    "\t// Writing wires\n"
    "\twire [2:0] const_bus;\n"
    "\tassign const_bus = 3'b101;\n"
    "endmodule\n";

constexpr std::string_view DATA_BUS_REGULAR_CONST_TEST =
    "module graph_0(\n"
    "\tinput1, input2, input3, \n"
    "\toutput1, output2\n"
    ");\n"
    "\t// Writing inputs\n"
    "\tinput [2:0] input_bus;\n"
    "\tinput input1, input2, input3;\n"
    "\t// Writing outputs\n"
    "\toutput [1:0] output_bus;\n"
    "\toutput output1, output2;\n"
    "\t// Writing wires\n"
    "\twire [2:0] const_bus;\n"
    "\tassign const_bus = 3'b101;\n"
    "endmodule\n";
} // namespace TestData
