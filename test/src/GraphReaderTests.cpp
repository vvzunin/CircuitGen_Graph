#include <gtest/gtest.h>
#include<CircuitGenGraph/GraphReader.hpp>
#include<lorina/lorina.hpp>
using namespace CG_Graph;
TEST(SimpleReadingTest, SomeStringPrints){
GraphReader reader = GraphReader();
EXPECT_NO_THROW(read_verilog("temp.v", reader));
}