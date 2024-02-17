#include "graphGenerator.h"

Node::Node() {
	node = { -1,-1 };
}

Node::Node(GridPosition lNode) {
	node = lNode;
}

void Node::setNode(GridPosition nodeToBeSet) {
	node = nodeToBeSet;
}

void Node::setID(int nodeID) {
	id = nodeID;
}

bool Node::empty() {
	return node.column == -1 && node.row == -1;
}

GridPosition Node::getGridPosition() {
	return node;
}

int Node::getID() {
	return id;
}

bool Node::operator==(const Node& rhs) {
	return this->node.column == rhs.node.column && this->node.row == rhs.node.row;
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

Node findNodeInGraph(GridPosition gp, Graph* graph) {
	std::vector<Node>* nodes = graph->getNodes();
	for (Node node : *nodes) {
		if (node.getGridPosition().column == gp.column && node.getGridPosition().row == gp.row)
			return node;
	}

	return Node();
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

void Graph::setGrid(Cell* worldGrid) {
	grid = worldGrid;
}

Node Graph::getNode(GridPosition gp) {
	Node node = findNodeInGraph(gp, this);
	return node;
}

std::vector<Node>* Graph::getNodes() {
	return &nodesInGraph;
}

std::vector<Connection>* Graph::getConnections() {
	return &connectionsInGraph;
}

std::vector<Node> Graph::getNeighbors(Node node) {
	std::vector<Node> listOfNeighbors;
	GridPosition gp = node.getGridPosition();

	// search the rightward neighbor
	if (gp.column < NUMOFCOLS - 1) {
		Cell cellToTheRight = grid[gp.row * NUMOFCOLS + gp.column + 1];
		bool nodeOnRightOccupied = cellToTheRight.getOccupied();
		if (!nodeOnRightOccupied) {
			GridPosition rightGP = cellToTheRight.getGridPosition();
			Node nodeOnRight = this->getNode(rightGP);
			listOfNeighbors.push_back(nodeOnRight);
		}
	}

	// search the leftward neighbor
	if (gp.column > 0) {
		Cell cellToTheLeft = grid[gp.row * NUMOFCOLS + gp.column - 1];
		bool nodeOnleftOccupied = cellToTheLeft.getOccupied();
		if (!nodeOnleftOccupied) {
			GridPosition leftGP = cellToTheLeft.getGridPosition();
			Node nodeOnLeft = this->getNode(leftGP);
			listOfNeighbors.push_back(nodeOnLeft);
		}
	}

	// search the upward neighbor
	if (gp.row > 0) {
		Cell cellAbove = grid[(gp.row - 1) * NUMOFCOLS + gp.column];
		bool nodeAboveOccupied = cellAbove.getOccupied();
		if (!nodeAboveOccupied) {
			GridPosition aboveGP = cellAbove.getGridPosition();
			Node nodeAbove = this->getNode(aboveGP);
			listOfNeighbors.push_back(nodeAbove);
		}
	}

	// search the netherward neighbor
	if (gp.row < NUMOFROWS - 1) {
		Cell cellBelow = grid[(gp.row + 1) * NUMOFCOLS + gp.column];
		bool nodeBelowOccupied = cellBelow.getOccupied();
		if (!nodeBelowOccupied) {
			GridPosition belowGP = cellBelow.getGridPosition();
			Node nodeBelow = this->getNode(belowGP);
			listOfNeighbors.push_back(nodeBelow);
		}
	}

	return listOfNeighbors;
}

GraphGenerator::GraphGenerator() {

}

Graph GraphGenerator::convertWorldToGraph(GameWorld& world) {
	Graph worldGraph = Graph();

	Cell* worldGrid = world.getGrid();
	int rows = world.getNumOfRows();
	int cols = world.getNumOfCols();

	worldGraph.setGrid(worldGrid);

	int nodeID = 0;

	// get the nodes and connections
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			Cell* gridCell = (worldGrid + (row * cols) + col);
			bool cellIsOccupied = gridCell->getOccupied();
			if (!cellIsOccupied) {
				Node newNode = Node(gridCell->getGridPosition());
				newNode.setID(nodeID);
				worldGraph.addNode(newNode);
	
				// get the connections
				GridPosition thisCellsGP = gridCell->getGridPosition(); // get the grid position to verify that this cell is not on the rightward column or netheward row
				if (thisCellsGP.column < cols - 1) {
					Cell* cellToTheRight = (worldGrid + (row * cols) + (col + 1)); // NOTE: nodes on right column will not have rightward connections, need to fix this; can fix by looking at grid position?
					bool cellOnRightOccupied = cellToTheRight->getOccupied();
					if (!cellOnRightOccupied) {
						Node nodeOnRight = Node(cellToTheRight->getGridPosition());
						Connection newConnectionRight = Connection(newNode, nodeOnRight);
						worldGraph.addConnection(newConnectionRight);
					}
				}
				if (thisCellsGP.row < rows - 1) {
					Cell* cellBelow = (worldGrid + ((row + 1) * cols) + col); // NOTE: nodes on bottom row will not have netherward connections, need to fix this
					bool cellBelowOccupied = cellBelow->getOccupied();
					if (!cellBelowOccupied) {
						Node nodeBelow = Node(cellBelow->getGridPosition());
						Connection newConnectionBelow = Connection(newNode, nodeBelow);
						worldGraph.addConnection(newConnectionBelow);
					}
				}
			}
			nodeID++;
		}		
	}

	return worldGraph;
}