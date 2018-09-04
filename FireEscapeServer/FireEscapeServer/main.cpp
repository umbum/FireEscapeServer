#include <iostream>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/transform_value_property_map.hpp>
#include <boost/graph/graph_utility.hpp>

#include "sensor.h"
#include "controller.h"
#include "Edge.h"

using namespace boost;
using namespace std;


class BuildingGraph {
private:
	typedef adjacency_list < vecS, vecS, undirectedS,
		no_property, Edge > GraphT;
	using vertex_descriptor = GraphT::vertex_descriptor;
	using vertex_iterator = GraphT::vertex_iterator;
	using edge_descriptor = GraphT::edge_descriptor;

public:
	GraphT g;
	BuildingGraph() {
		init();

	}
	void init() {
		auto v0 = add_vertex(g);
		auto v1 = add_vertex(g);
		auto v2 = add_vertex(g);
		auto v3 = add_vertex(g);
		add_edge(v0, v1, Edge(1), g);
		add_edge(v0, v2, Edge(2), g);
		add_edge(v1, v3, Edge(3), g);
	}
	void update() {

	}
	void evaluate() {
		vertex_descriptor exit = *(vertices(g).first);                  // 출발 정점. (출구)
		std::vector<vertex_descriptor> predecessors(num_vertices(g));	// 각각의 벡터에 해당 vertex까지의 shortest path 경로가 저장된다.
		std::vector<int> distances(num_vertices(g));					// 각각의 벡터에 해당 vertex까지의 distance가 저장된다.
		auto v_index = get(vertex_index, g);
		auto weight = boost::make_transform_value_property_map(std::mem_fn(&Edge::getWeight), get(boost::edge_bundle, g));

		edge_descriptor eee = *(edges(g).first);
		std::cout << eee.get_property() << std::endl;

		dijkstra_shortest_paths(g, exit,
			predecessor_map(make_iterator_property_map(predecessors.begin(), v_index))
			.distance_map(make_iterator_property_map(distances.begin(), v_index))
			.weight_map(weight));

		std::cout << "distances and parents:" << std::endl;

		vertex_iterator vi, vend;
		for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
			//std::cout << g[predecessors[vi]] << '\n';
			std::cout << "distance(" << ") = " << distances[*vi] << ", ";
			std::cout << "parent(" << ") = " << predecessors[*vi] << std::endl;
		}
		std::cout << std::endl;
	}
	void show() {
		boost::print_graph(g, get(boost::vertex_bundle, g));
	}
};



int main(int argc, char *argv[])
{
	BuildingGraph building;
	building.update();
	building.evaluate();

	return 0;
}