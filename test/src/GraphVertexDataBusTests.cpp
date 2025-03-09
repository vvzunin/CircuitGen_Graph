#include <gtest/gtest.h>
#include <CircuitGenGraph/GraphVertexBase.hpp>

GraphPtr memoryOwnerGraph = std::make_shared<OrientedGraph>();

TEST(TestGraphVertexBaseConstructor, DefaultConstructor) {
    GraphVertexBase vertex(memoryOwnerGraph);
    EXPECT_EQ(vertex.getWidth(), 1);
    EXPECT_EQ(vertex.getBits().size(), 1);
    EXPECT_EQ(vertex.getBaseGraph().lock(), memoryOwnerGraph);
}

TEST(TestGraphVertexBaseConstructor, ConstructorWithWidth) {
    GraphVertexBase vertex(memoryOwnerGraph, 4);
    EXPECT_EQ(vertex.getWidth(), 4);
    EXPECT_EQ(vertex.getBits().size(), 4);
}

TEST(TestGraphVertexBaseMethods, SetAndGetWidth) {
    GraphVertexBase vertex(memoryOwnerGraph);
    vertex.setWidth(8);
    EXPECT_EQ(vertex.getWidth(), 8);
    EXPECT_EQ(vertex.getBits().size(), 8);
}

TEST(TestGraphVertexBaseMethods, SliceMethod) {
    GraphVertexBase vertex(memoryOwnerGraph, 8);
    auto slicedVertex = vertex.slice(2, 5);
    EXPECT_EQ(slicedVertex.getWidth(), 4);
}

TEST(TestGraphVertexBaseMethods, OperatorAccess) {
    GraphVertexBase vertex(memoryOwnerGraph, 4);
    EXPECT_NO_THROW(vertex[2]);
    EXPECT_THROW(vertex[4], std::out_of_range);
}

TEST(TestGraphVertexBaseErrors, InvalidWidth) {
    EXPECT_THROW(GraphVertexBase(memoryOwnerGraph, 0), std::invalid_argument);
}