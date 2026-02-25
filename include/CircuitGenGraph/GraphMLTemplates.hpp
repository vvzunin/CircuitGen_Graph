#pragma once

#include <map>

#include <CircuitGenGraph/GraphUtils.hpp>

/// @file GraphMLTemplates.hpp
/// \~english
/// @brief Definitions of GraphML templates for various output formats
/// @details This header file contains string templates for exporting graphs in three formats:
/// Classic, OpenABCD, and PseudoABCD.
/// \~russian
/// @brief Определения шаблонов GraphML для различных форматов вывода
/// @details Этот заголовочный файл содержит строковые шаблоны для экспорта графов в трех форматах:
/// Classic, OpenABCD и PseudoABCD.
/// \~

/// \~english
/// @brief Contains all templates for classical translation in GraphML.
/// \~russian
/// @brief Содержит все шаблоны для классической трансляции в GraphML.
/// \~
namespace CG_Graph::ClassicGraphML {

/// @brief \~english Main GraphML template with XML declaration and schema \~russian Главный шаблон GraphML с объявлением XML и схемой
const char *mainTemplate = R"(<?xml version="1.0" encoding="UTF-8"?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
  <key id="t" for="node" attr.name="type" attr.type="string"/>
{}
</graphml>
)";

/// @brief \~english Template for the graph structure element \~russian Шаблон для элемента структуры графа
const char *rawGraphTemplate = R"({}  <graph id="{}" edgedefault="directed">
{}{}  </graph>)";

/// @brief \~english Template for a node with type data \~russian Шаблон для узла с данными о типе
const char *rawNodeTemplate = R"({}    <node id="{}">
{}      <data key="t">{}</data>{}
{}    </node>
)";

/// @brief \~english Template for a directed edge connection \~russian Шаблон для соединения ориентированным ребром
const char *rawEdgeTemplate = R"({}    <edge source="{}" target="{}"/>
)";

} // namespace CG_Graph::ClassicGraphML

/// \~english
/// @brief Contains templates for OpenABCD format.
/// \~russian
/// @brief Содержит шаблоны для формата OpenABCD.
/// \~
namespace CG_Graph::OpenABCD {

/// @brief \~english Main container template for OpenABCD GraphML \~russian Главный контейнерный шаблон для OpenABCD GraphML
const char *mainTemplate = R"(<?xml version='1.0' encoding='utf-8'?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
  <key id="d3" for="edge" attr.name="edge_type" attr.type="long" />
  <key id="d2" for="node" attr.name="num_inverted_predecessors" attr.type="long" />
  <key id="d1" for="node" attr.name="node_type" attr.type="long" />
  <key id="d0" for="node" attr.name="node_id" attr.type="string" />
  <graph edgedefault="directed">
{}  </graph>
</graphml>
)";

/// @brief \~english Node template with ABCD-specific data keys \~russian Шаблон узла с ключами данных, специфичными для ABCD
const char *nodeTemplate = R"(    <node id="{}">
      <data key="d0">{}</data>
      <data key="d1">{}</data>
      <data key="d2">{}</data>
    </node>
)";

/// @brief \~english Edge template with edge type data for OpenABCD \~russian Шаблон ребра с данными о типе ребра для OpenABCD
const char *edgeTemplate = R"(    <edge source="{}" target="{}">
      <data key="d3">{}</data>
    </edge>
)";

} // namespace CG_Graph::OpenABCD

/// \~english
/// @brief Contains templates for PseudoABCD format.
/// \~russian
/// @brief Содержит шаблоны для формата PseudoABCD.
/// \~
namespace CG_Graph::PseudoABCD {

/// @brief \~english Main container template for PseudoABCD GraphML \~russian Главный контейнерный шаблон для PseudoABCD GraphML
const char *mainTemplate = R"(<?xml version='1.0' encoding='utf-8'?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
  <key id="d1" for="node" attr.name="node_type" attr.type="long" />
  <key id="d0" for="node" attr.name="node_id" attr.type="string" />
  <graph edgedefault="directed">
{}  </graph>
</graphml>
)";

/// @brief \~english Simplified node template for PseudoABCD \~russian Упрощенный шаблон узла для PseudoABCD
const char *nodeTemplate = R"(    <node id="{}">
      <data key="d0">{}</data>
      <data key="d1">{}</data>
    </node>
)";

/// @brief \~english Simple edge template for PseudoABCD \~russian Простой шаблон ребра для PseudoABCD
const char *edgeTemplate = R"(    <edge source="{}" target="{}"/>
)";

} // namespace CG_Graph::PseudoABCD

namespace CG_Graph {

/// @brief
/// \~english Mapping of logic gate types to their corresponding ABCD numeric codes.
/// \~russian Сопоставление типов логических вентилей с соответствующими числовыми кодами ABCD.
/// \~
std::map<Gates, std::string> gateToABCDType = {
    {Gates::GateAnd, "2"},        {Gates::GateNand, "10"},
    {Gates::GateOr, "13"},        {Gates::GateNor, "14"},
    {Gates::GateNot, "12"},       {Gates::GateBuf, "11"},
    {Gates::GateXor, "15"},       {Gates::GateXnor, "16"},
    {Gates::GateDefault, "ERROR"}};

} // namespace CG_Graph