#include <string>

#include <CircuitGenGraph/DefaultAuxiliaryMethods.hpp>

#include <gtest/gtest.h>

#ifdef LOGFLAG
#include "easylogging++Init.hpp"
#endif

using namespace CG_Graph;

TEST(DefaultAuxMethodsTests, TestFormat) {
  std::string_view target =
      "Formatted by: Ilya, year: 2025, m, pi approx = 3.14";
  std::string to_format = "Formatted by: {}, year: {}, {}, pi approx = {}";

  std::string res =
      AuxMethodsGraph::format(to_format, "Ilya", 2025, 'm', 3.14f);
  EXPECT_EQ(res, target);
}

TEST(DefaultAuxMethodsTests, TestReplacer) {
  std::string_view target = "Formatted by by by by by";
  std::string to_format = "Formatted % % % % %";

  std::string res = AuxMethodsGraph::replacer(to_format, "by");
  EXPECT_EQ(res, target);
}
