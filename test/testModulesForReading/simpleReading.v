module simpleReading(
clk,
someInput,
q
);
input clk;
input someInput;
output q;
assign q = clk & someInput;
endmodule
