module outputAsGate(
clk,
someInput,
q1,
q2,
q3,
q4
);
input clk;
input someInput;
wire nand, buf;
output q1, q2, q3,q4;
assign q1 = clk & someInput;
assign q2 = ~q1;
assign q3 = q1 | q2 | someInput;
assign nand = ~ (q3 & clk);
assign buf = nand;
assign q4 = buf; 

endmodule
