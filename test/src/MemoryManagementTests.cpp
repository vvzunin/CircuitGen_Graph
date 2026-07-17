#include <CircuitGenGraph/GraphMemory.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>

#include <gtest/gtest.h>
#include <new>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

using namespace CG_Graph;

TEST(AllocatorTests, AllocateMethodTooBigStruct) {
  MultiLinearAllocator allocator(196, 154);
  struct BigStruct {
    // will take 256 bytes
    bytea many[256];
  };

  BigStruct *ptr = allocator.allocate<BigStruct>();
  EXPECT_EQ(ptr, nullptr);
}
TEST(AllocatorTests, AllocateMethodSmallStruct) {
  MultiLinearAllocator allocator(196, 154);
  struct SmallStruct {
    // will take 196 bytes
    bytea many[196];
  };

  SmallStruct *ptr1 = allocator.allocate<SmallStruct>();
  EXPECT_NE(ptr1, nullptr);
  ptr1 = allocator.allocate<SmallStruct>();
  EXPECT_EQ(ptr1, nullptr);
}

TEST(AllocatorTests, CreateThrowsWhenVertexDoesNotFitArena) {
  // Arena minimum is 154 bytes; GraphVertexBusGate is 160 → allocate fails.
  auto graph = std::make_shared<OrientedGraph>("tiny", /*buffer_size=*/154,
                                               /*chunk_size=*/154);
  EXPECT_THROW(graph->addGateBus(Gates::GateAnd, "g", 2), std::bad_alloc);
}
