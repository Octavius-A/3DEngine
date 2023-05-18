#include "gameState.h"
#include "rendering/rendering.h"

#include <iostream>

GameState globalGameState;

GameObject3D* initGameObject3D(unsigned int modelId, unsigned int shaderId,
	glm::vec3 position, glm::vec3 rotation) {
	GameObject3D* obj = new GameObject3D{
		modelId,
		shaderId,
		position,
		rotation,
		true
	};
	globalGameState.gameObjects.push_back(obj);
	return obj;
}