#include <string_view>

namespace TestData {

constexpr std::string_view SEQ_1_TEST = "module graph_0(\n"
                                        "\tclk, data, \n"
                                        "\tres\n"
                                        ");\n"
                                        "\t// Writing inputs\n"
                                        "\tinput clk, data;\n"
                                        "\t// Writing outputs\n"
                                        "\toutput res;\n"
                                        "\t// Writing registers\n"
                                        "\treg q;\n\t\n"
                                        "\talways @(posedge clk) begin\n"
                                        "\t\tq <= data;\n"
                                        "\tend\n\n"
                                        "\tassign res = q;\n"
                                        "endmodule\n";

constexpr std::string_view SEQ_2_TEST =
    "module graph_0(\n"
    "\ten, data, \n"
    "\tres\n"
    ");\n"
    "\t// Writing inputs\n"
    "\tinput en, data;\n"
    "\t// Writing outputs\n"
    "\toutput res;\n"
    "\t// Writing registers\n"
    "\treg q;\n\t\n"
    "\t// EN signal \"en\""
    " - when it is in a logical one state, trigger writes data to the output\n"
    "\talways @(*) begin\n"
    "\t\tif (en) q <= data;\n"
    "\tend\n\n"
    "\tassign res = q;\n"
    "endmodule\n";

constexpr std::string_view SEQ_3_TEST =
    "module graph_0(\n"
    "\tclk, data, rst, set, en, \n"
    "\tres\n"
    ");\n"
    "\t// Writing inputs\n"
    "\tinput clk, data, rst, set, en;\n"
    "\t// Writing outputs\n"
    "\toutput res;\n"
    "\t// Writing registers\n"
    "\treg q;\n\t\n"
    "\t// RST signal \"rst\" - when it is in a logical zero state, trigger "
    "writes logical zero to the output\n"
    "\t// SET signal \"set\" - when it is in a logical one state, trigger "
    "writes logical one to the output\n"
    "\t// EN signal \"en\" - when it is in a logical one state, trigger writes "
    "data to the output\n"
    "\talways @(posedge clk) begin\n"
    "\t\tif (!rst) q <= 1'b0;\n"
    "\t\telse if (set) q <= 1'b1;\n"
    "\t\telse if (en) q <= data;\n"
    "\tend\n\n"
    "\tassign res = q;\n"
    "endmodule\n";

} // namespace TestData
