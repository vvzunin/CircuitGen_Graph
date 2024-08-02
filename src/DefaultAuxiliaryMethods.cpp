#include <CircuitGenGraph/DefaultAuxiliaryMethods.hpp>
#include "easyloggingpp/easylogging++.h"

std::string
    AuxMethods::replacer(const std::string& i_s, const std::string& i_r) {
  std::string res;
  res.reserve(i_s.length() * 2);

  std::string::size_type pos = 0, prev_pos = 0;

  while ((pos = i_s.find('%', pos)) != std::string::npos) {
    res.append(i_s, prev_pos, pos - prev_pos);
    res.append(i_r);
    prev_pos = ++pos;
  }

  res.append(i_s, prev_pos, i_s.length() - prev_pos);
  return res;
}
