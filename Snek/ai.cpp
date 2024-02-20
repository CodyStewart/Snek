#include "ai.h"

// NOTE: have both the traipse and pathfinding done by A*. Pick specific nodes for traipse (i.e. the ones one diagonal from the corners)
// Can have a hard and easy mode. Easy mode sticks to the outskirts while hard mode centralizes the snake

const int NUMOFNEARESTPICKUPSTOKEEP = 4;
const int numOfTraipseNodes = 8;
GridPosition traipseNodes[numOfTraipseNodes] = { {1,1}, {23,1}, {1,23}, {23,23}, {2,12}, {12,2}, {22, 12}, {12,22} };

struct ListOfNearestPickups {
	Node closestPickups[NUMOFNEARESTPICKUPSTOKEEP];
};


int absValue(int value) {
	return value < 0 ? -value: value;
}

int sq(int value) {
	return value * value;
}

int getEuclidDistance(GridPosition snakeGP, GridPosition closestPickup) {
	int sqrdDist = sq(snakeGP.column - closestPickup.column) + sq(snakeGP.row - closestPickup.row);

	return sqrdDist;
}

Path::Path() {

}

Path::Path(std::vector<GridPosition> pathVector) {
	path = pathVector;
}

std::vector<GridPosition> Path::getPath() {
	return path;
}

GridPosition Path::getFirst() {
	return path[0];
}

// we choose the pickup nearest to us determined by Euclidean distance
ListOfNearestPickups choosePickupFromList(Snake* snake, std::vector<PickUp*>* pickupList, Graph* graph) {
	GridPosition snakeGP = snake->getGridPosition();
	GridPosition closestPickup = pickupList->at(0)->getPosition();
	GridPosition nextClosestPickup = closestPickup;
	GridPosition thirdClosestPickup = closestPickup;
	GridPosition fourthClosestPickup = closestPickup;

	int smallestEuclidDist = getEuclidDistance(snakeGP, closestPickup);

	for (int i = 1; i < pickupGathering.size(); i++) {
		GridPosition currentPickup = pickupList->at(i)->getPosition();
		int currentEuclidDist = getEuclidDistance(snakeGP, currentPickup);

		if (currentEuclidDist < smallestEuclidDist) {
			smallestEuclidDist = currentEuclidDist; // set the smallest euclidean distance
			fourthClosestPickup = thirdClosestPickup;
			thirdClosestPickup = nextClosestPickup;
			nextClosestPickup = closestPickup;	// set the next closest pickup to what was the closest
			closestPickup = currentPickup;	// set the closest pickup to this one
		}
	}

	ListOfNearestPickups nearestPickups = ListOfNearestPickups();
	nearestPickups.closestPickups[0] = closestPickup;
	nearestPickups.closestPickups[1] = nextClosestPickup;
	nearestPickups.closestPickups[2] = thirdClosestPickup;
	nearestPickups.closestPickups[3] = fourthClosestPickup;

	return nearestPickups;
}

// with no pickups to gather, we will traipse the graph by choosing one from a list of target nodes
Node getNewTraipseTarget(Graph* graph) {
	srand(SDL_GetTicks());
	int randNum = rand() % numOfTraipseNodes;
	GridPosition gp = traipseNodes[randNum];
	Node node = graph->getNode(gp);
	
	return node;
}

std::vector<int> getPath(Node startNode, Node endNode, std::map<int, int> cameFrom) {
	std::vector<int> path;

	int previousNode = endNode.getID();

	if (cameFrom.find(previousNode) == cameFrom.end()) {
		return path;	// we return empty path as there is no path to the node
	}
	
	while (previousNode != startNode.getID()) {
		path.push_back(previousNode);
		auto previous = cameFrom.find(previousNode);
		previousNode = previous->second;
	}

	std::reverse(path.begin(), path.end());

	return path;
}

Path convertIDsToGridPositions(std::vector<int> pathToEndNode, Graph* graph) {
	std::vector<GridPosition> path;
	std::vector<Node>* nodesInGraph = graph->getNodes();

	for (int id : pathToEndNode) {
		for (Node node : *nodesInGraph) {
			if (node.getID() == id) {
				path.push_back(node.getGridPosition());
			}
		}
	}

	return Path(path);
}

// we use the euclidean distance as our heurisitic
int heuristic(Node* thisNode, Node* goalNode) {
	return getEuclidDistance(thisNode->getGridPosition(), goalNode->getGridPosition());
}

struct WeightCompare {
	bool operator()(Node& lhs, Node& rhs) {
		return lhs.getWeight() > rhs.getWeight();
	}
};

Path FindPathViaAStar(Node startNode, Node endNode, Graph* graph) {
	/* Starting from startNode, we need to get to endNode. If no such path exists, return empty*/
	std::vector<int> pathToEndNode;

	std::vector<Node>* nodesInThisGraph = graph->getNodes();
	//std::vector<Connection>* connectionsInThisGraph = graph->getConnections();

	// verify that startNode exists in this graph
	bool startNodeInGraph = false;
	for (int i = 0; i < nodesInThisGraph->size(); i++) {
		if (nodesInThisGraph->at(i) == startNode) {
			startNodeInGraph = true;
			break;
		}
	}

	if (!startNodeInGraph)
		return Path();

	// create open and closed list
	std::map<int, Node> closedList;
	std::map<int, Node> openList;
	std::map<int, int> cameFrom;
	//std::queue<Node> priority;
	std::priority_queue<Node, std::vector<Node>, WeightCompare> priority;
	// add startNode to the closed list 
	closedList.insert({ startNode.getID(), startNode});
	std::pair<int, int> pr = std::make_pair(startNode.getID(), startNode.getID());
	cameFrom.insert(pr);
	int costSoFar = 0;
	// add neighbors to the open list
	for (Node next : graph->getNeighbors(startNode)) {
		costSoFar = next.getWeight() + heuristic(&next, &endNode);
		next.setWeight(costSoFar);
		openList.insert({ next.getID(), next});
		priority.push(next);
		pr = std::make_pair(next.getID(), startNode.getID());
		cameFrom.insert(pr);
	}

	//// pick the node of highest priority from the open list and add to the closed
	while (!openList.empty()) {
		Node currentNode;
		if (priority.size() > 0) {
			currentNode = priority.top();
			priority.pop();
		}
		else {
			printf("priority size is 0. return an empty path\n");
			return Path();
		}

		if (currentNode == endNode)
			break;

		// NOTE: for A* we have to check if our cost so far is below the CSF of the node in the open list (if it appears there)
		for (Node next : graph->getNeighbors(currentNode)) {
			auto searchClosed = closedList.find(next.getID());
			if (searchClosed == closedList.end()) {	// neighbor not found in closedList
				auto searchOpen = openList.find(next.getID());
				if (searchOpen == openList.end()) { // neighbor also not found in openList
					costSoFar = next.getWeight() + heuristic(&next, &endNode);
					next.setWeight(costSoFar);
					openList.insert({ next.getID(), next});	// add currentNode's neighbors to the open list to be examined later
					priority.push(next);
					pr = std::make_pair(next.getID(), currentNode.getID());
					cameFrom.insert(pr);
				}
				else {	// neighbor in openList, check if we found a better path
					costSoFar = next.getWeight() + heuristic(&next, &endNode);
					if (next.getWeight() > costSoFar) {
						next.setWeight(costSoFar);
						openList.erase(next.getID());
						openList.insert({ next.getID(), next });	
						priority.push(next);
						pr = std::make_pair(next.getID(), currentNode.getID());
						cameFrom.insert(pr);
					}
				}
			}
		}

		openList.erase(currentNode.getID());
		closedList.insert({ currentNode.getID(), currentNode});
	}

	pathToEndNode = getPath(startNode, endNode, cameFrom);
	Path pathToEnd = convertIDsToGridPositions(pathToEndNode, graph);

	return pathToEnd;
}

AI::AI() {
	traipseTarget = Node(GridPosition({ -1,-1 }));
	Path currentPath;
}

Direction chooseDirection(GridPosition snakePos, GridPosition nextMove) {
	if (snakePos.column < nextMove.column)
		return RIGHT;
	else if (snakePos.column > nextMove.column)
		return LEFT;
	else if (snakePos.row < nextMove.row)
		return DOWN;
	
	return UP;
}

Direction chooseRandomDirection() {
	srand(SDL_GetTicks());
	int randomNum = rand() % 4;

	if (randomNum == 0)
		return UP;
	else if (randomNum == 1)
		return LEFT;
	else if (randomNum == 2)
		return RIGHT;
	else
		return DOWN;
}

GridPosition nearbyValidNodeToMoveTo(Snake* snake, Graph* graph) {
	GridPosition snakePosition = snake->getGridPosition();
	printf("Inside nearbyValidNodeToMoveTo\n");
	
	GridPosition prospectivePositionToMoveTo = { snakePosition.row, snakePosition.column - 1 };	// check cell on the left
	Node prospectiveNode = graph->getNode(prospectivePositionToMoveTo);
	if (!prospectiveNode.empty())	// node is in graph and therefore valid
		return prospectivePositionToMoveTo;	// move left
	
	prospectivePositionToMoveTo = { snakePosition.row, snakePosition.column + 1 };	// check cell on the right
	prospectiveNode = graph->getNode(prospectivePositionToMoveTo);
	if (!prospectiveNode.empty())	// node is in graph and therefore valid
		return prospectivePositionToMoveTo;	// move right

	prospectivePositionToMoveTo = { snakePosition.row - 1, snakePosition.column };	// check cell above
	prospectiveNode = graph->getNode(prospectivePositionToMoveTo);
	if (!prospectiveNode.empty())	// node is in graph and therefore valid
		return prospectivePositionToMoveTo;	// move up

	prospectivePositionToMoveTo = { snakePosition.row + 1, snakePosition.column};	// check cell below
	prospectiveNode = graph->getNode(prospectivePositionToMoveTo);
	if (!prospectiveNode.empty())	// node is in graph and therefore valid
		return prospectivePositionToMoveTo;	// move down

	printf("No valid move could be found in nearbyValidNodeToMoveTo. Sending a down move by default\n");
	return prospectivePositionToMoveTo;
}


Direction AI::getDecision(Graph* graph, Snake* snake, std::vector<PickUp*>* pickupList) {
	Direction chosenDirection = UP;

	GridPosition snakePosition = snake->getGridPosition();
	Node startNode = graph->getNode(snakePosition);
	Node goalNode = Node();

	if (pickupList->size() > 0) {
		int pickupNum = 0;
		ListOfNearestPickups listOfPickups = choosePickupFromList(snake, pickupList, graph);
		Node nodeToReach = listOfPickups.closestPickups[pickupNum];
		goalNode = graph->getNode(nodeToReach.getGridPosition());

		Path chosenPath = FindPathViaAStar(startNode, goalNode, graph);

		if (chosenPath.getPath().size() == 0) {	// path could not be found, choose a different target
			while (chosenPath.getPath().size() == 0 && ++pickupNum < NUMOFNEARESTPICKUPSTOKEEP) {
				goalNode = listOfPickups.closestPickups[pickupNum];
				chosenPath = FindPathViaAStar(startNode, goalNode, graph);
			}
			if (chosenPath.getPath().size() == 0) {	// we could not find a path to any of the closest pickups, return a random direction
				goto fallback;
			}
			
			return chooseDirection(snake->getGridPosition(), chosenPath.getFirst());
		}
		else {
			GridPosition gpOfNextMove = chosenPath.getFirst();
			chosenDirection = chooseDirection(snake->getGridPosition(), gpOfNextMove);
		}
	}
	// we probably want to traipse to our previously selected traipse node so long as a path exists. if it doesn't we get a new traipse node
	else {
		fallback:
		if (traipseTarget.getGridPosition() == GridPosition({ -1, -1 })) {	// we do not currently have a traipse target, get one
			goalNode = getNewTraipseTarget(graph);
			traipseTarget = goalNode;
		}
		else if (!(traipseTarget.getGridPosition() == GridPosition({ -1, -1})) && !(snake->getGridPosition() == traipseTarget.getGridPosition())) {	// we have not yet acquired the traipseTarget, continue seeking it
			goalNode = traipseTarget;
		}
		else {	// we have reached the traipseTarget, get a new target
			goalNode = getNewTraipseTarget(graph);
			traipseTarget = goalNode;
		}

		Path chosenPath = FindPathViaAStar(startNode, goalNode, graph);

		if (chosenPath.getPath().size() == 0) {	// path could not be found, choose a different target
			int MAXLOOPTIMES = 25;	// this is the maximum times we will search for a traipse target, if we cannot find one we simply choose a valid nearby node
			while (chosenPath.getPath().size() == 0 && MAXLOOPTIMES-- > 0) {
				goalNode = getNewTraipseTarget(graph);
				traipseTarget = goalNode;
				chosenPath = FindPathViaAStar(startNode, goalNode, graph);
			}
			if (MAXLOOPTIMES <= -1)
				return chooseDirection(snake->getGridPosition(), nearbyValidNodeToMoveTo(snake, graph));
			else
				return chooseDirection(snake->getGridPosition(), chosenPath.getFirst());
		}
		else {
			GridPosition gpOfNextMove = chosenPath.getFirst();
			chosenDirection = chooseDirection(snake->getGridPosition(), gpOfNextMove);
		}
	}

	return chosenDirection;
}

void AI::reset() {
	traipseTarget = Node({ -1,-1 });
}