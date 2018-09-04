#pragma once
#include <vector>
#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/transform_value_property_map.hpp>
#include <boost/graph/graph_utility.hpp>

#include "Point.h"
#include "Edge.h"


class BuildingGraph {
private:
	typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS,
		Point, Edge > GraphT;
	using vertex_descriptor = GraphT::vertex_descriptor;
	using vertex_iterator = GraphT::vertex_iterator;
	using edge_descriptor = GraphT::edge_descriptor;
	GraphT g;
	std::vector<vertex_descriptor> exits;
public:
	BuildingGraph();
	void addEdge(vertex_descriptor v1, vertex_descriptor v2, int length);
	void update();
	void evaluate();
	//void show();
	/*********************** inline ********************/
	inline vertex_descriptor addExit(Point v) {
		auto exit = boost::add_vertex(v, g);
		exits.push_back(exit);
		return exit;
	}
	inline vertex_descriptor addExit(std::string name) {
		auto exit = boost::add_vertex(Point(name), g);
		exits.push_back(exit);
		return exit;
	}
	inline vertex_descriptor addVertex(Point v) {
		return boost::add_vertex(v, g);
	}
	inline vertex_descriptor addVertex(std::string name) {
		return boost::add_vertex(Point(name), g);
	}
};