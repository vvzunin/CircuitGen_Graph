module lilModule(
input clk,
input [3:0] someInput,
output reg q,
);
assign q = clk & someInput[0];
endmodule