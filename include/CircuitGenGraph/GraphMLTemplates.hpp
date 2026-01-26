#pragma once

#include <map>

#include <CircuitGenGraph/GraphUtils.hpp>

/// @file GraphMLTemplates.hpp
/// @brief GraphML template definitions for different output formats
/// @ingroup GraphExport
/// 
/// This header contains string templates for exporting graphs in three
/// GraphML formats: Classic, OpenABCD, and PseudoABCD.
/// 


/// \namespace ClassicGraphML
/// \brief Contains all templates for classic GraphML translation
namespace CG_Graph::ClassicGraphML {

/// \var mainTemplate
/// \brief Main GraphML template with XML declaration and schema
/// \details Contains the root GraphML structure with proper namespaces
const char *mainTemplate = R"(<?xml version="1.0" encoding="UTF-8"?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
  <key id="t" for="node" attr.name="type" attr.type="string"/>
{}
</graphml>
)";

/// \var rawGraphTemplate
/// \brief Template for individual graph element
/// \details Defines a directed graph with customizable ID, nodes and edges
const char *rawGraphTemplate = R"({}  <graph id="{}" edgedefault="directed">
{}{}  </graph>)";

/// \var rawNodeTemplate
/// \brief Template for node element with type attribute
/// \details Includes node ID and type data for GraphML visualization
const char *rawNodeTemplate = R"({}    <node id="{}">
{}      <data key="t">{}</data>{}
{}    </node>
)";

/// \var rawEdgeTemplate
/// \brief Template for directed edge element
/// \details Defines connection between source and target nodes
const char *rawEdgeTemplate = R"({}    <edge source="{}" target="{}"/>
)";

} // namespace CG_Graph::ClassicGraphML

/// \namespace OpenABCD
/// \brief Contains all templates for OpenABCD GraphML translation
namespace CG_Graph::OpenABCD {

/// \var mainTemplate
/// \brief Main template for OpenABCD GraphML format
/// \details Includes specific keys for node_id, node_type, num_inverted_predecessors, and edge_type
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

/// \var nodeTemplate
/// \brief Template for node with ABCD-specific attributes
/// \details Includes node_id (d0), node_type (d1), and num_inverted_predecessors (d2)
const char *nodeTemplate = R"(    <node id="{}">
      <data key="d0">{}</data>
      <data key="d1">{}</data>
      <data key="d2">{}</data>
    </node>
)";

/// \var edgeTemplate
/// \brief Template for edge with edge_type attribute
/// \details Includes edge_type (d3) for ABCD compatibility
const char *edgeTemplate = R"(    <edge source="{}" target="{}">
      <data key="d3">{}</data>
    </edge>
)";

} // namespace CG_Graph::OpenABCD

/// \namespace PseudoABCD
/// \brief Contains all templates for PseudoABCD GraphML translation
namespace CG_Graph::PseudoABCD {

/// \var mainTemplate
/// \brief Main template for PseudoABCD GraphML format
/// \brief Simplified version with only node_id and node_type keys
const char *mainTemplate = R"(<?xml version='1.0' encoding='utf-8'?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
  <key id="d1" for="node" attr.name="node_type" attr.type="long" />
  <key id="d0" for="node" attr.name="node_id" attr.type="string" />
  <graph edgedefault="directed">
{}  </graph>
</graphml>
)";

/// \var nodeTemplate
/// \brief Template for node with simplified ABCD attributes
/// \details Includes only node_id (d0) and node_type (d1)
const char *nodeTemplate = R"(    <node id="{}">
      <data key="d0">{}</data>
      <data key="d1">{}</data>
    </node>
)";

/// \var edgeTemplate
/// \brief Template for edge without additional attributes
/// \details Simple directed edge for PseudoABCD format
const char *edgeTemplate = R"(    <edge source="{}" target="{}"/>
)";

} // namespace CG_Graph::PseudoABCD

namespace CG_Graph {

/// \var gateToABCDType
/// \brief Lookup table mapping gate types to ABCD GraphML type codes
/// \details Provides conversion between internal gate enumeration and
///          numeric type codes used in ABCD-compatible GraphML formats
/// \note Used by OpenABCD and PseudoABCD GraphML generators
/// \warning Gates::GateDefault maps to "ERROR" string
std::map<Gates, std::string> gateToABCDType = {
    {Gates::GateAnd, "2"},        {Gates::GateNand, "10"},
    {Gates::GateOr, "13"},        {Gates::GateNor, "14"},
    {Gates::GateNot, "12"},       {Gates::GateBuf, "11"},
    {Gates::GateXor, "15"},       {Gates::GateXnor, "16"},
    {Gates::GateDefault, "ERROR"}};

} // namespace CG_Graph
