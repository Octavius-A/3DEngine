/*
Functions to managed game state. Initialising game, adding objects, etc.
*/

#pragma once

#include "utils/io.h"

#include <glm/glm.hpp>
#include <vector>

struct GameObject3D {
	unsigned int modelId;
	unsigned int shaderId;
	glm::vec3 position;
	glm::vec3 rotation;
	float scale;
	bool visible;
};

struct GameState {
	std::vector<GameObject3D*> gameObjects;
	double dTime;
	int fps;
};

extern GameState globalGameState;

void initGame(); // right now this is just for starting delta time
void updateGame();

// Initialises a new 3D game object, adds it to the game object list, and returns a pointer
// That can be managed by whatever called.
GameObject3D* initGameObject3D(unsigned int modelId, unsigned int shaderId,
	glm::vec3 position, glm::vec3 rotation, float scale);

void initLevel(json levelData);