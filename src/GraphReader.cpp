#include<CircuitGenGraph/OrientedGraph.hpp>
#include <CircuitGenGraph/GraphReader.hpp>
#include <lorina/lorina.hpp>
namespace CG_Graph {

void GraphReader::on_module_header(const std::string& module_name, const std::vector<std::string> & inouts) {
  currentGraph = std::make_shared<OrientedGraph>(module_name);
  for (auto i : inouts)
    std::cout << i;
}
void GraphReader::on_inputs( const std::vector<std::string>& inputs, std::string const& size) const {
  for (auto i : inputs) 
    std::cout << i;
}
void GraphReader::on_outputs( const std::vector<std::string>& inputs, std::string const& size) const {
    for (auto i : inputs) 
      std::cout << i;
}
void GraphReader::on_endmodule() const {
graphs[currentGraph.get()->getName()] = currentGraph;
}
GraphPtr GraphReader::getGraphByName(std::string_view name) {
    return this->graphs[std::string(name)];
}

}