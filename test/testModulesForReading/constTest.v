//This file was generated automatically using CircuitGen_Graph at 01-08-2025 16-10-21.

module constTest(
	input_2, 
	output_4
);
	// Writing inputs
	input input_2;
	// Writing outputs
	output output_4;
	// Writing gates for main graph
	wire gate_3, gate_5, gate_6;
	// Writing consts
	wire const_0, const_1;
	assign const_0 = 1'b1;
	assign const_1 = 1'b0;

	assign gate_3 = ~ ( const_0 & input_2 );
	assign gate_5 = const_1;
	assign gate_6 = ~ ( gate_5 ^ gate_3 );

	assign output_4 = gate_6;
endmodule
