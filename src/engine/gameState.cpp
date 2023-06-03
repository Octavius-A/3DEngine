#include "gameState.h"
#include "rendering/rendering.h"
#include "physics/physics.h"

#include <iostream>

GameState globalGameState;
Uint32 previousTime = 0;
Uint32 currentTime = 0;

int frameCount = 0;
float totTime = 0.0f;


void initGame() {
	previousTime = SDL_GetTicks();
}

void updateGame() {
	/*static Uint64 NOW = 0;
	static Uint64 LAST = 0;
	
	LAST = NOW;
	NOW = SDL_GetPerformanceCounter();
	
	double deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency()) * 0.001;*/

	currentTime = SDL_GetTicks();
	double deltaTime = (currentTime - previousTime) / 1000.0f;
	previousTime = currentTime;

	globalGameState.dTime = deltaTime;
	
	//globalGameState.fps = (unsigned int)(1.0f / deltaTime);

	frameCount += 1;

	totTime += deltaTime;
	if (totTime >= 1.0f) {
		globalGameState.fps = frameCount / totTime;
		frameCount = 0;
		totTime = 0.0f;
	}
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

void initLevel(json levelData) {
	unsigned int levelMeshID = levelData["levelGeom"];
	std::string collisionPath = levelData["levelCollision"];
	initGameObject3D(levelMeshID, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	loadPhysicsWorld(collisionPath.c_str());
	//glm::vec3 playerOrigin = glm::vec3(levelData["playerStart"][0],
	//	levelData["playerStart"][1],
	//	levelData["playerStart"][2]);
	initLights(levelData["lights"], levelData["light_params"]);
}