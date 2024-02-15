#pragma once
#ifndef AI_H
#define AI_H

#include "snek.h"
#include "actor.h"
#include "graphGenerator.h"

struct AI {
	AI();

	Direction getDecision(Graph* graph, Snake* snake);

private:

};

#endif // !AI_H