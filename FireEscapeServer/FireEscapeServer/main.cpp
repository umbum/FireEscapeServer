#include <iostream>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/transform_value_property_map.hpp>
#include <boost/graph/graph_utility.hpp>

#include "sensor.h"
#include "controller.h"
#include "Point.h"
#include "Edge.h"

using namespace boost;
using namespace std;


class BuildingGraph {
private:
	typedef adjacency_list < vecS, vecS, undirectedS,
		Point, Edge > GraphT;
	using vertex_descriptor = GraphT::vertex_descriptor;
	using vertex_iterator = GraphT::vertex_iterator;
	using edge_descriptor = GraphT::edge_descriptor;
	GraphT g;
	vector<vertex_descriptor> exits;
public:
	BuildingGraph() {
		init();
	}
	void init() {
		auto exit_l1 = add_vertex(Point("Exit L1"), g);
		auto exit_l2 = add_vertex(Point("Exit L2"), g);
		auto exit_m1 = add_vertex(Point("Exit M1"), g);
		auto exit_m2 = add_vertex(Point("Exit M2"), g);
		auto exit_r1 = add_vertex(Point("Exit R1"), g);
		exits.push_back(exit_l1);
		exits.push_back(exit_l2);
		exits.push_back(exit_m1);
		exits.push_back(exit_m2);
		exits.push_back(exit_r1);
		auto l1 = add_vertex(Point("L1"), g);
		auto l2 = add_vertex(Point("L2"), g);
		auto l3 = add_vertex(Point("L3"), g);
		auto l4 = add_vertex(Point("L4"), g);
		auto room4_l = add_vertex(Point("Room4 L door"), g);
		auto room4_r = add_vertex(Point("Room4 R door"), g);
		auto m1 = add_vertex(Point("M1"), g);
		auto m2 = add_vertex(Point("M2"), g);
		auto m3 = add_vertex(Point("M3"), g);
		auto m4 = add_vertex(Point("M4"), g);
		auto r1 = add_vertex(Point("R1"), g);
		auto r2 = add_vertex(Point("R2"), g);
		auto r3 = add_vertex(Point("R3"), g);
		
		auto _add_edge = [this](auto v1, auto v2, int w) {
			auto edge_name = g[v1].name + " <> " + g[v2].name;
			boost::add_edge(v1, v2, Edge(edge_name, w), g);
			
		};
		_add_edge(l1, exit_l1, 1);
		_add_edge(l1, l2, 1);
		_add_edge(l2, exit_l2, 1);
		_add_edge(l2, l3, 1);
		_add_edge(l3, l4, 1);
		_add_edge(room4_l, l4, 1);
		_add_edge(l4, m1, 1);
		_add_edge(room4_r, m1, 1);
		_add_edge(room4_r, m2, 1);
		_add_edge(m1, m2, 1);
		_add_edge(m2, exit_m1, 1);
		_add_edge(m2, exit_m2, 1);
		_add_edge(m2, m3, 1);
		_add_edge(m2, m3, 1);
		_add_edge(m3, m4, 1);
		_add_edge(m4, exit_r1, 1);
		_add_edge(m4, r1, 1);
		_add_edge(r1, r2, 1);
		_add_edge(r2, r3, 1);

	}

	void update() {

	}

	void evaluate() {
		vertex_descriptor exit = exits[0];
		std::vector<vertex_descriptor> predecessors(num_vertices(g));	// 각각의 벡터에 해당 vertex까지의 shortest path 경로가 저장된다.
		std::vector<int> distances(num_vertices(g));					// 각각의 벡터에 해당 vertex까지의 distance가 저장된다.
		auto v_index = get(vertex_index, g);
		auto weight = boost::make_transform_value_property_map(std::mem_fn(&Edge::getWeight), get(boost::edge_bundle, g));

		dijkstra_shortest_paths(g, exit,
			predecessor_map(make_iterator_property_map(predecessors.begin(), v_index))
			.distance_map(make_iterator_property_map(distances.begin(), v_index))
			.weight_map(weight));

		std::cout << "distances and parents:" << std::endl;
		/*
		vertex_iterator vi, vend;
		for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
			std::cout << g[predecessors[vi]] << '\n';
			std::cout << "distance(" << ") = " << distances[*vi] << ", ";
			std::cout << "parent(" << ") = " << predecessors[*vi] << std::endl;
		}*/
		for (auto v : boost::make_iterator_range(boost::vertices(g))) {
			std::cout << g[v].name << " : ";
			std::cout << "distance = " << distances[v] << ", ";
			std::cout << "parent = " << g[predecessors[v]].name << "\n";
		}
		std::cout << std::endl;
	}
	/*void show() {
		boost::print_graph(g, get(boost::vertex_bundle, g));
	}*/
};



int main(int argc, char *argv[])
{
	BuildingGraph building;
	building.update();
	building.evaluate();
	return 0;
}