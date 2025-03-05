#include <CircuitGenGraph/DefaultAuxiliaryMethods.hpp>

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"
#endif

namespace CircuitGenGraph {

std::string operator*(std::string a, unsigned int b) {
  std::string output = "";
  while (b--) {
    output += a;
  }
  return output;
}

std::string AuxMethods::replacer(const std::string &i_fmt,
                                 const std::string &i_val) {
  std::string res;
  std::string::size_type pos = 0, prev_pos = 0;

  size_t count = std::count(i_fmt.begin(), i_fmt.end(), '%');
  res.reserve(i_fmt.length() + (i_val.length() - 1) * count);

  while ((pos = i_fmt.find('%', pos)) != std::string::npos) {
    res.append(i_fmt, prev_pos, pos - prev_pos);
    res.append(i_val);
    prev_pos = ++pos;
  }

  res.append(i_fmt, prev_pos, i_fmt.length() - prev_pos);
  return res;
}

std::string AuxMethods::dotReturnToString(DotReturn dot) {
  unsigned int tab = 0;
  std::string dotTab = "  ";
  std::string s = "";

  bool printingSubGraph = false;

  for (int i = 0; i < dot.size(); i++) {
    switch (dot[i].first) {
      case DotTypes::DotGraph:
        s += dotTab * tab++ + "digraph " + dot[i].second["name"] + " {\n";
        break;
      case DotTypes::DotInput:
        s += dotTab * tab + dot[i].second["name"] +
             " [shape=triangle, label=\"" + dot[i].second["label"] +
             "\\nlevel: " + dot[i].second["level"] + "\"];\n";
        break;
      case DotTypes::DotConstant:
        s += dotTab * tab + dot[i].second["name"] + " [shape=cds, label=\"" +
             dot[i].second["label"] + "\\n" + dot[i].second["value"] + "\"];\n";
        break;
      case DotTypes::DotOutput:
        s += dotTab * tab + dot[i].second["name"] +
             " [shape=invtriangle, label=\"" + dot[i].second["label"] +
             "\\nlevel: " + dot[i].second["level"] + "\"];\n";
        break;
      case DotTypes::DotGate:
        s += dotTab * tab + dot[i].second["name"] + " [label=\"" +
             dot[i].second["label"] + "\\nlevel: " + dot[i].second["level"] +
             "\"];\n";
        break;

      case DotTypes::DotEdge:
        s += dotTab * tab + dot[i].second["from"] + " -> " +
             dot[i].second["to"] + ";\n";
        break;
      case DotTypes::DotSubGraph:
        s += (dotTab * tab++) + "subgraph cluster_" +
             dot[i].second["instName"] + " {\n";
        printingSubGraph = true;
        break;
      case DotTypes::DotExit:
#ifdef LOGFLAG
        LOG(INFO) << "tab: " << tab;
#endif
        if (printingSubGraph)
          s += dotTab * --tab + "}\n";
        break;
    }
  }
  s += "}";

  return s;
}

} // namespace CircuitGenGraph
