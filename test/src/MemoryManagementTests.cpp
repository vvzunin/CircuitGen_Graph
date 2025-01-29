#include <CircuitGenGraph/GraphMemory.hpp>

#include <gtest/gtest.h>

#include "easylogging++Init.hpp"

TEST(AllocatorTests, AllocateMethodTooBigStruct) {
  MultiLinearAllocator allocator(196, 128);
  struct BigStruct {
    // will take 256 bytes
    bytea many[256];
  };

  BigStruct *ptr = allocator.allocate<BigStruct>();
  CHECK_EQ(ptr, nullptr);
}
TEST(AllocatorTests, AllocateMethodSmallStruct) {
  MultiLinearAllocator allocator(196, 128);
  struct SmallStruct {
    // will take 196 bytes
    bytea many[196];
  };

  SmallStruct *ptr1 = allocator.allocate<SmallStruct>();
  CHECK_NE(ptr1, nullptr);
  ptr1 = allocator.allocate<SmallStruct>();
  CHECK_EQ(ptr1, nullptr);
}
