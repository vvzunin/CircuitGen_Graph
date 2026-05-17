module simpleStructure ( a, b, c, q , q2) ;
input a, b, c ;
output q, q2 ;
wire e, f;
assign e = a | ~b | c ;
assign f = ~( a ^ e );
assign q = f & e ;
assign q2 = ~b;
endmodule
