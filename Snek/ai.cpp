#include "ai.h"

// NOTE: have both the traipse and pathfinding done by A*. Pick specific nodes for traipse (i.e. the ones one diagonal from the corners)
// Can have a hard and easy mode. Easy mode sticks to the outskirts while hard mode centralizes the snake

AI::AI() {

}

Direction AI::getDecision(Graph* graph, Snake* snake) {
	Direction chosenDirection = UP;

	return chosenDirection;
}