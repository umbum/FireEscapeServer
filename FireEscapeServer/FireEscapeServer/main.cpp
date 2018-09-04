#include "BuildingGraph.h"

void init1F(BuildingGraph& g) {
	auto exit_l1 = g.addExit(Point("Exit L1"));
	auto exit_l2 = g.addExit(Point("Exit L2"));
	auto exit_m1 = g.addExit(Point("Exit M1"));
	auto exit_m2 = g.addExit(Point("Exit M2"));
	auto exit_r1 = g.addExit(Point("Exit R1"));
	auto l1 = g.addVertex(Point("L1"));
	auto l2 = g.addVertex(Point("L2"));
	auto l3 = g.addVertex(Point("L3"));
	auto l4 = g.addVertex(Point("L4"));
	auto room4_l = g.addVertex(Point("Room4 L door"));
	auto room4_r = g.addVertex(Point("Room4 R door"));
	auto m1 = g.addVertex(Point("M1"));
	auto m2 = g.addVertex(Point("M2"));
	auto m3 = g.addVertex(Point("M3"));
	auto m4 = g.addVertex(Point("M4"));
	auto r1 = g.addVertex(Point("R1"));
	auto r2 = g.addVertex(Point("R2"));
	auto r3 = g.addVertex(Point("R3"));

	g.addEdge(l1, exit_l1, 1);
	g.addEdge(l1, l2, 1);
	g.addEdge(l2, exit_l2, 1);
	g.addEdge(l2, l3, 1);
	g.addEdge(l3, l4, 1);
	g.addEdge(room4_l, l4, 1);
	g.addEdge(l4, m1, 1);
	g.addEdge(room4_r, m1, 1);
	g.addEdge(room4_r, m2, 1);
	g.addEdge(m1, m2, 1);
	g.addEdge(m2, exit_m1, 1);
	g.addEdge(m2, exit_m2, 1);
	g.addEdge(m2, m3, 1);
	g.addEdge(m2, m3, 1);
	g.addEdge(m3, m4, 1);
	g.addEdge(m4, exit_r1, 1);
	g.addEdge(m4, r1, 100);
	g.addEdge(r1, r2, 1);
	g.addEdge(r2, r3, 1);
}

int main(int argc, char *argv[])
{
	BuildingGraph building;
	init1F(building);
	building.update();
	building.evaluate();

	return 0;
}
