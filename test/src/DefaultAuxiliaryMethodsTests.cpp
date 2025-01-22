#include <string>

#include <CircuitGenGraph/DefaultAuxiliaryMethods.hpp>
#include <gtest/gtest.h>

#include "easylogging++Init.hpp"

TEST(DefaultAuxMethodsTests, TestFormat) {
  std::string_view target =
      "Formated by: Ilya, year: 2025, m, pi approx = 3.14";
  std::string to_format = "Formated by: %, year: %, %, pi approx = %";

  std::string res = AuxMethods::format(to_format, "Ilya", 2025, 'm', 3.14f);
  EXPECT_EQ(res, target);
}

TEST(DefaultAuxMethodsTests, TestReplacer) {
  std::string_view target = "Formated by by by by by";
  std::string to_format = "Formated % % % % %";

  std::string res = AuxMethods::replacer(to_format, "by");
  EXPECT_EQ(res, target);
}
