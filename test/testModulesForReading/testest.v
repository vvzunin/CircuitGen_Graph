//This file was generated automatically using CircuitGen_Graph at 18-04-2026 23-22-18.

module testest(
	clk, someInput, 
	q1, q2, q3, q4
);
	// Writing inputs
	input clk, someInput;
	// Writing outputs
	output q1, q2, q3, q4;
	// Writing gates for main graph
	wire nand, buf, q1_gate, q1_not, q3_gate;
	
	assign nand = ~ ( q3 & clk );
	assign buf = nand;
	assign q1_gate = clk & someInput;
	assign q1_not = ~q1;
	assign q3_gate = q1 | q2 | someInput;

	assign q1 = q1_gate;
	assign q2 = q1_not;
	assign q3 = q3_gate;
	assign q4 = buf;
endmodule
