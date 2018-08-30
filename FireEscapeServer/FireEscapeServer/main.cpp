#include <iostream>
#include <vector>

#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>

#include "sensor.h"
#include "controller.h"

using namespace boost;
using namespace std;

/*
 1. ������ ����
 2. ���� �����͸� class�� ����� �����ؼ� ����
 3. IN ��� ������ �̿� OUT ����ġ ����
 4. shortest path ���
 5. Led Color ��� ���� ����
 6. �� ��Ʈ�� �۽�
 ------------
 ��� �߰��Ǹ�?? ��� ��� initialize�Ұ���. �߰��� �߰��Ǵ� ������ �Ƶ��̳�� ��� ��������.
 -> �̰� ����ȭ ��Ŀ������ ���� Server���� ����ȭ �Լ� ȣ���ؼ� ����ȭ ��Ŷ ������ �Ƶ��̳뿡�� �ڱ� ���� ����Ʈ ������ ������ �ؾ��� ��.
 */ 


vector<Controller> controllers;
void init() {
	auto c1 = Controller("c1");
	c1.addSensor(Sensor("s1"));
	c1.addSensor(Sensor("s2"));
	c1.sensors[0].updateWeight(60, 300);
	c1.sensors[1].updateWeight(50, 300);

	auto c2 = Controller("c2");
	c2.addSensor(Sensor("s1"));
	c2.sensors[0].updateWeight(63, 250);

	auto c3 = Controller("c3");
	c3.addSensor(Sensor("s1"));
	c3.addSensor(Sensor("s2"));
	c3.addSensor(Sensor("s3"));
	c3.sensors[0].updateWeight(57, 300);
	c3.sensors[1].updateWeight(59, 310);
	c3.sensors[2].updateWeight(61, 320);

	controllers.push_back(c1);
	controllers.push_back(c2);
	controllers.push_back(c3);
}


int main(int argc, char *argv[])
{
	typedef adjacency_list < listS, vecS, undirectedS,
		no_property, property < edge_weight_t, int > > graph_t;
	typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
	typedef std::pair<int, int> Edge;

	const int num_nodes = 5;
	enum nodes { A, B, C, D, E };
	char name[] = "ABCDE";
	Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E),
	  Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A), Edge(E, B)
	};
	int weights[] = { 1, 2, 1, 2, 7, 3, 1, 1, 1 };
	int num_arcs = sizeof(edge_array) / sizeof(Edge); 


	graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
	std::vector<vertex_descriptor> p(num_vertices(g));    // ������ ���Ϳ� �ش� vertex������ shortest path ��ΰ� ����ȴ�.
	std::vector<int> d(num_vertices(g));			      // ������ ���Ϳ� �ش� vertex������ distance�� ����ȴ�.
	vertex_descriptor s = vertex(A, g);                   // ��� ����. (�ⱸ)
	
	dijkstra_shortest_paths(g, s,
		predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g))).
		distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));

	std::cout << "distances and parents:" << std::endl;
	graph_traits < graph_t >::vertex_iterator vi, vend;
	for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
		std::cout << "distance(" << name[*vi] << ") = " << d[*vi] << ", ";
		std::cout << "parent(" << name[*vi] << ") = " << name[p[*vi]] << std::endl;
	}
	std::cout << std::endl;

	return 0;
}