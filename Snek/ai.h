#pragma once
#ifndef AI_H
#define AI_H

#include <random>
#include <queue>
#include <map>
#include <utility>

#include "snek.h"
#include "actor.h"
#include "graphGenerator.h"

struct Path {
	Path();
	Path(std::vector<GridPosition> path);

	std::vector<GridPosition> getPath();
	GridPosition getFirst();

private:
	std::vector<GridPosition> path;
};

struct AI {
	AI();

	Direction getDecision(Graph* graph, Snake* snake, std::vector<PickUp*>* pickupList);

	void reset();

private:
	Node traipseTarget;
	Path currentPath;
};

#endif // !AI_H