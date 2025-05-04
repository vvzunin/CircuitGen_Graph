//This file was generated automatically using CircuitGen_Graph at 04-05-2025 23-35-40.

module testGraph(
	a, b, 
	c
);
	// Writing inputs
	input a, b;
	// Writing outputs
	output c;
	// Writing gates for subGraphs outputs
	wire gate_88;
	
  testSubGraph testSubGraph_inst_0 (
    .a( a ),
    .b( b ),
    .c( gate_88 )
  ); 

	assign c = gate_88;
endmodule
