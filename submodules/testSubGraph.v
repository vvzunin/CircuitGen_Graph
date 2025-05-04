//This file was generated automatically using CircuitGen_Graph at 04-05-2025 23-35-40.

module testSubGraph(
	a, b, 
	c
);
	// Writing inputs
	input a, b;
	// Writing outputs
	output c;
	// Writing gates for main graph
	wire andAB, orAnd11;
	
	wire const1;
	assign const1 = 1'b1;

	assign andAB = a & b;
	assign orAnd11 = andAB | const1;

	assign c = orAnd11;
endmodule
