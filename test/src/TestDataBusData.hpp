#include <string_view>

namespace TestData {
inline constexpr std::string_view DATABUS_CONST_TEST =
    "wire [3:0] bus;\n"
    "assign bus = 4'b1010;\n";

inline constexpr std::string_view DATABUS_INPUT_TEST = "input [3:0] bus;\n";

inline constexpr std::string_view DATABUS_OUTPUT_TEST = "output [3:0] bus;\n"
                                                        "assign bus[0] = a;\n"
                                                        "assign bus[1] = b;\n";

inline constexpr std::string_view DATABUS_TEST = "input [2:0] bus;\n"
                                                 "output [2:0] out;\n";
} // namespace TestData
