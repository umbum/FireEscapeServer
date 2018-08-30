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
 1. 데이터 수신
 2. 받은 데이터를 class의 멤버에 대입해서 갱신
 3. IN 멤버 변수를 이용 OUT 가중치 갱신
 4. shortest path 계산
 5. Led Color 멤버 변수 설정
 6. 각 포트로 송신
 ------------
 노드 추가되면?? 노드 어떻게 initialize할건지. 중간에 추가되는 센서나 아두이노는 어떻게 받을건지.
 -> 이건 동기화 메커니즘을 만들어서 Server에서 동기화 함수 호출해서 동기화 패킷 보내면 아두이노에서 자기 센서 리스트 보내는 식으로 해야할 듯.
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
	std::vector<vertex_descriptor> p(num_vertices(g));    // 각각의 벡터에 해당 vertex까지의 shortest path 경로가 저장된다.
	std::vector<int> d(num_vertices(g));			      // 각각의 벡터에 해당 vertex까지의 distance가 저장된다.
	vertex_descriptor s = vertex(A, g);                   // 출발 정점. (출구)
	
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