#pragma once
#ifndef GRAPHGENERATOR_H
#define GRAPHGENERATOR_H

#include <vector>

#include "snek.h"
#include "GameWorld.h"

struct Node {
	Node();
	Node(GridPosition node);
	
	void setNode(GridPosition node);
	void setID(int id);
	
	bool empty();

	//Node getNode();
	GridPosition getGridPosition();
	int getID();

	bool operator==(const Node& rhs);

private:
	GridPosition node;
	int id;
};

struct Connection {
	Connection();
	Connection(Node fromNode, Node toNode);

	void setFromNode(Node fromNode);
	void setToNode(Node toNode);
	
	Node getFromNode();
	Node getToNode();

private:
	Node fromNode;
	Node toNode;
};

struct Graph {
	Graph();
	void addNode(Node& node);
	void addConnection(Connection& connection);

	void setGrid(Cell* worldGrid);

	int getNumOfNodes();
	Node getNode(GridPosition gp);
	std::vector<Node>* getNodes();
	std::vector<Connection>* getConnections();
	std::vector<Node> getNeighbors(Node node);

private:
	std::vector<Node> nodesInGraph;
	std::vector<Connection> connectionsInGraph;
	Cell* grid;
};

struct GraphGenerator {
	GraphGenerator();

	Graph convertWorldToGraph(GameWorld& world);
};

#endif // !GRAPHGENERATOR_H
