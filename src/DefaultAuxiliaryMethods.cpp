#include <CircuitGenGraph/DefaultAuxiliaryMethods.hpp>

#include "easyloggingpp/easylogging++.h"

std::string operator*(std::string a, unsigned int b) {
  std::string output = "";
  while (b--) {
    output += a;
  }
  return output;
}

std::string AuxMethods::replacer(const std::string &i_s,
                                 const std::string &i_r) {
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
        // LOG(INFO) << "tab: " << tab;
        if (printingSubGraph)
          s += dotTab * --tab + "}\n";
        break;
    }
  }
  s += "}";

  return s;
}