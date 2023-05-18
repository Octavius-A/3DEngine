/*
Functions to managed game state. Initialising game, adding objects, etc.
*/

#pragma once


#include <glm/glm.hpp>
#include <vector>

struct GameObject3D {
	unsigned int modelId;
	unsigned int shaderId;
	glm::vec3 position;
	glm::vec3 rotation;
	bool visible;
};

struct GameState {
	std::vector<GameObject3D*> gameObjects;
	double dTime;
	int fps;
};

extern GameState globalGameState;

void updateGame();

// Initialises a new 3D game object, adds it to the game object list, and returns a pointer
// That can be managed by whatever called.
GameObject3D* initGameObject3D(unsigned int modelId, unsigned int shaderId,
	glm::vec3 position, glm::vec3 rotation);