//This file was generated automatically using CircuitGen_Graph at 01-08-2025 16-10-21.

module allGatesTest(
	input_0, input_1, input_2, 
	output_3, output_4
);
	// Writing inputs
	input input_0, input_1, input_2;
	// Writing outputs
	output output_3, output_4;
	// Writing gates for main graph
	wire gate_5, gate_6, gate_7, gate_8, gate_9, gate_10, gate_11, gate_12, gate_13, gate_14;
	
	assign gate_5 = gate_11 & gate_12;
	assign gate_6 = ~ ( gate_11 & input_1 );
	assign gate_7 = gate_6 | gate_9;
	assign gate_8 = ~ ( gate_10 | input_1 );
	assign gate_9 = input_0 ^ input_2 ^ input_0;
	assign gate_10 = ~ ( gate_7 ^ gate_11 );
	assign gate_11 = ~input_0;
	assign gate_12 = ~input_1;
	assign gate_13 = gate_5;
	assign gate_14 = gate_9;

	assign output_3 = gate_13;
	assign output_4 = gate_14;
endmodule
