#include "graphGenerator.h"

Node::Node() {

}

Node::Node(Cell cell) {
	node = cell;
}

void Node::setNode(Cell nodeToBeSet) {
	node = nodeToBeSet;
}

Connection::Connection() {

}

Connection::Connection(Node setFromNode, Node setToNode) {
	fromNode = setFromNode;
	toNode = setToNode;
}

void Connection::setFromNode(Node fromNodeArg) {
	fromNode = fromNodeArg;
}

void Connection::setToNode(Node toNodeArg) {
	toNode = toNodeArg;
}

Node Connection::getFromNode() {
	return fromNode;
}

Node Connection::getToNode() {
	return toNode;
}

Graph::Graph() {

}

void Graph::addNode(Node& node) {
	nodesInGraph.push_back(node);
}

void Graph::addConnection(Connection& connection) {
	connectionsInGraph.push_back(connection);
}

int Graph::getNumOfNodes() {
	return (int)nodesInGraph.size();
}

GraphGenerator::GraphGenerator() {

}

Graph GraphGenerator::convertWorldToGraph(GameWorld& world) {
	Graph worldGraph = Graph();

	Cell* worldGrid = world.getGrid();
	int rows = world.getNumOfRows();
	int cols = world.getNumOfCols();

	// get the nodes and connections
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			Cell* gridCell = (worldGrid + (row * cols) + col);
			bool cellIsOccupied = gridCell->getOccupied();
			if (!cellIsOccupied) {
				Node newNode = Node(*gridCell);
				worldGraph.addNode(newNode);
	
				// get the connections
				GridPosition thisCellsGP = gridCell->getGridPosition(); // get the grid position to verify that this cell is not on the rightward column or netheward row
				if (thisCellsGP.column < cols - 1) {
					Cell* cellToTheRight = (worldGrid + (row * cols) + (col + 1)); // NOTE: nodes on right column will not have rightward connections, need to fix this; can fix by looking at grid position?
					bool cellOnRightOccupied = cellToTheRight->getOccupied();
					if (!cellOnRightOccupied) {
						Node nodeOnRight = Node(*cellToTheRight);
						Connection newConnectionRight = Connection(newNode, nodeOnRight);
						worldGraph.addConnection(newConnectionRight);
					}
				}
				if (thisCellsGP.row < rows - 1) {
					Cell* cellBelow = (worldGrid + ((row + 1) * cols) + col); // NOTE: nodes on bottom row will not have netherward connections, need to fix this
					bool cellBelowOccupied = cellBelow->getOccupied();
					if (!cellBelowOccupied) {
						Node nodeBelow = Node(*cellBelow);
						Connection newConnectionBelow = Connection(newNode, nodeBelow);
						worldGraph.addConnection(newConnectionBelow);
					}
				}
			}
		}		
	}

	return worldGraph;
}