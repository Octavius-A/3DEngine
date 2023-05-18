#include "gameState.h"
#include "rendering/rendering.h"

#include <iostream>

GameState globalGameState;

void updateGame() {
	static Uint64 NOW = 0;
	static Uint64 LAST = 0;
	
	LAST = NOW;
	NOW = SDL_GetPerformanceCounter();
	
	double deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency()) * 0.001;

	globalGameState.dTime = deltaTime;
	globalGameState.fps = (int)(1.0f / deltaTime);
}

GameObject3D* initGameObject3D(unsigned int modelId, unsigned int shaderId,
	glm::vec3 position, glm::vec3 rotation, float scale) {
	GameObject3D* obj = new GameObject3D{
		modelId,
		shaderId,
		position,
		rotation,
		scale,
		true
	};
	globalGameState.gameObjects.push_back(obj);
	return obj;
}