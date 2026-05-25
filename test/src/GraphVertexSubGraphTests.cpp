#include <CircuitGenGraph/GraphVertex.hpp>

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <vector>

using namespace CG_Graph;

namespace {

namespace fs = std::filesystem;

class ScopedTempDir {
public:
  ScopedTempDir() {
    const auto uniqueSuffix = std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());

    d_path = fs::temp_directory_path() /
             ("cg_graph_verilog_submodules_test_" + uniqueSuffix);

    fs::create_directories(d_path);
  }

  ~ScopedTempDir() {
    std::error_code ec;
    fs::remove_all(d_path, ec);
  }

  const fs::path &path() const { return d_path; }

private:
  fs::path d_path;
};

std::string loadTextFile(const fs::path &p) {
  std::ifstream file(p, std::ios_base::binary);
  if (!file) {
    throw std::runtime_error("Cannot open file: " + p.string());
  }

  return std::string(std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>());
}

bool contains(const std::string &text, const std::string &pattern) {
  return text.find(pattern) != std::string::npos;
}

} // namespace

TEST(TestToVerilogSubmodules, CreatesSingleSubmodulesDirAndWritesAllModules) {
  CG_Graph::GraphVertexBase::resetCounter();
  CG_Graph::OrientedGraph::resetCounter();

  ScopedTempDir tempDir;

  GraphPtr leafGraph = std::make_shared<OrientedGraph>("leafGraph");
  VertexPtr leafIn = leafGraph->addInput("leaf_in");
  VertexPtr leafOut = leafGraph->addOutput("leaf_out");
  leafGraph->addEdge(leafIn, leafOut);

  GraphPtr wrapperGraph = std::make_shared<OrientedGraph>("wrapperGraph");
  VertexPtr wrapperIn = wrapperGraph->addInput("wrapper_in");

  std::vector<VertexPtr> leafOutputs =
      wrapperGraph->addSubGraph(leafGraph, {wrapperIn});

  ASSERT_EQ(leafOutputs.size(), 1u);

  VertexPtr wrapperOut = wrapperGraph->addOutput("wrapper_out");
  wrapperGraph->addEdge(leafOutputs.at(0), wrapperOut);

  GraphPtr topGraph = std::make_shared<OrientedGraph>("topGraph");
  VertexPtr topIn0 = topGraph->addInput("top_in_0");
  VertexPtr topIn1 = topGraph->addInput("top_in_1");

  std::vector<VertexPtr> wrapperOutputs0 =
      topGraph->addSubGraph(wrapperGraph, {topIn0});
  std::vector<VertexPtr> wrapperOutputs1 =
      topGraph->addSubGraph(wrapperGraph, {topIn1});

  ASSERT_EQ(wrapperOutputs0.size(), 1u);
  ASSERT_EQ(wrapperOutputs1.size(), 1u);

  VertexPtr topOut0 = topGraph->addOutput("top_out_0");
  VertexPtr topOut1 = topGraph->addOutput("top_out_1");

  topGraph->addEdge(wrapperOutputs0.at(0), topOut0);
  topGraph->addEdge(wrapperOutputs1.at(0), topOut1);

  const auto wrapperInstances =
      topGraph->getVerticesByType(VertexTypes::subGraph);
  const auto leafInstances =
      wrapperGraph->getVerticesByType(VertexTypes::subGraph);

  ASSERT_EQ(wrapperInstances.size(), 2u);
  ASSERT_EQ(leafInstances.size(), 1u);

  ASSERT_TRUE(topGraph->toVerilog(tempDir.path().string(), "topGraph.v"));

  const fs::path topFile = tempDir.path() / "topGraph.v";
  const fs::path submodulesDir = tempDir.path() / "submodules";
  const fs::path wrapperFile = submodulesDir / "wrapperGraph.v";
  const fs::path leafFile = submodulesDir / "leafGraph.v";

  ASSERT_TRUE(fs::exists(topFile));
  ASSERT_TRUE(fs::is_regular_file(topFile));

  ASSERT_TRUE(fs::exists(submodulesDir));
  ASSERT_TRUE(fs::is_directory(submodulesDir));

  ASSERT_TRUE(fs::exists(wrapperFile));
  ASSERT_TRUE(fs::is_regular_file(wrapperFile));

  ASSERT_TRUE(fs::exists(leafFile));
  ASSERT_TRUE(fs::is_regular_file(leafFile));

  EXPECT_FALSE(fs::exists(submodulesDir / "submodules"))
      << "Nested submodules directory must not be created";

  size_t submoduleFilesCount = 0;
  for (const auto &entry: fs::directory_iterator(submodulesDir)) {
    EXPECT_TRUE(entry.is_regular_file())
        << "Only Verilog files are expected inside submodules, got: "
        << entry.path();

    if (entry.is_regular_file()) {
      ++submoduleFilesCount;
    }
  }

  EXPECT_EQ(submoduleFilesCount, 2u);

  const std::string topVerilog = loadTextFile(topFile);
  const std::string wrapperVerilog = loadTextFile(wrapperFile);
  const std::string leafVerilog = loadTextFile(leafFile);

  EXPECT_EQ(wrapperInstances.at(0)->getName(), "wrapperGraph_inst_0");
  EXPECT_EQ(wrapperInstances.at(1)->getName(), "wrapperGraph_inst_1");

  EXPECT_TRUE(contains(topVerilog, "wrapperGraph wrapperGraph_inst_0"));
  EXPECT_TRUE(contains(topVerilog, "wrapperGraph wrapperGraph_inst_1"));

  EXPECT_EQ(leafInstances.at(0)->getName(), "leafGraph_inst_0");
  EXPECT_TRUE(contains(wrapperVerilog, "leafGraph leafGraph_inst_0"));

  EXPECT_FALSE(contains(topVerilog, "leafGraph "))
      << "topGraph.v should instantiate only its direct subgraphs";
}
