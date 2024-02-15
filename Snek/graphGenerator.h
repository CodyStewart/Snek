#pragma once
#ifndef GRAPHGENERATOR_H
#define GRAPHGENERATOR_H

#include <vector>

#include "snek.h"
#include "GameWorld.h"

struct Node {
	Node();
	Node(Cell cell);
	
	void setNode(Cell node);
	
	//Node getNode();

private:
	Cell node;
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

	int getNumOfNodes();

private:
	std::vector<Node> nodesInGraph;
	std::vector<Connection> connectionsInGraph;
};

struct GraphGenerator {
	GraphGenerator();

	Graph convertWorldToGraph(GameWorld& world);
};

#endif // !GRAPHGENERATOR_H
