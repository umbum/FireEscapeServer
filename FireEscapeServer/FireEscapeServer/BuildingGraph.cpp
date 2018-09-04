#include "BuildingGraph.h"

using namespace boost;

BuildingGraph::BuildingGraph() {
	// Nothing
}

void BuildingGraph::addEdge(vertex_descriptor v1, vertex_descriptor v2, int length)
{
	auto edge_name = g[v1].name + " <> " + g[v2].name;
	boost::add_edge(v1, v2, Edge(edge_name, length), g);
}

void BuildingGraph::update() {
	for (auto v : boost::make_iterator_range(boost::edges(g))) {
		//std::cout << "[Update Weight] " << g[v].name << '\n';
		g[v].updateWeight(1, 0);
	}
	//std::cout << std::endl;
}

void BuildingGraph::evaluate() {
	vertex_descriptor exit = exits[0];
	std::vector<vertex_descriptor> predecessors(num_vertices(g));	// 원소는 exit까지 최단거리로 도달하기 위해서 거쳐야 할 다음 vertex.
	std::vector<int> distances(num_vertices(g));					// 원소는 해당 vertex에서 exit까지의 distance.
	auto v_index = get(vertex_index, g);
	auto weight = boost::make_transform_value_property_map(std::mem_fn(&Edge::getWeight), get(boost::edge_bundle, g));

	dijkstra_shortest_paths(g, exit,
		predecessor_map(make_iterator_property_map(predecessors.begin(), v_index))
		.distance_map(make_iterator_property_map(distances.begin(), v_index))
		.weight_map(weight));

	std::cout << "[*] distances and parents:" << std::endl;
	for (auto v : boost::make_iterator_range(boost::vertices(g))) {
		std::cout << g[v].name << " : ";
		std::cout << "distance = " << distances[v] << ", ";
		std::cout << "parent = " << g[predecessors[v]].name << "\n";
	}
	std::cout << std::endl;
}

//void BuildingGraph::show() {
//	boost::print_graph(g, get(boost::vertex_bundle, g));
//}
