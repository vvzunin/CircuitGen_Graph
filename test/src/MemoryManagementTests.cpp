#include <CircuitGenGraph/GraphMemory.hpp>

#include <gtest/gtest.h>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

using namespace CG_Graph;

TEST(AllocatorTests, AllocateMethodTooBigStruct) {
  MultiLinearAllocator allocator(196, 128);
  struct BigStruct {
    // will take 256 bytes
    bytea many[256];
  };

  BigStruct *ptr = allocator.allocate<BigStruct>();
  EXPECT_EQ(ptr, nullptr);
}
TEST(AllocatorTests, AllocateMethodSmallStruct) {
  MultiLinearAllocator allocator(196, 128);
  struct SmallStruct {
    // will take 196 bytes
    bytea many[196];
  };

  SmallStruct *ptr1 = allocator.allocate<SmallStruct>();
  EXPECT_NE(ptr1, nullptr);
  ptr1 = allocator.allocate<SmallStruct>();
  EXPECT_EQ(ptr1, nullptr);
}
