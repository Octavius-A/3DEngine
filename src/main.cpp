#include "engine/rendering/rendering.h"
#include "engine/inputs/inputs.h"
#include "engine/gameState.h"
#include "engine/physics/physics.h"
#include "engine/rendering/model.h"
#include "player.h"

#include <iostream>

void mainLoop();

int main(int argc, char* args[]) {
	const char* resourcesPath = "resources.json";
	json resources;

	ERROR_CODE ec = SUCCESS;
	ec = loadJson(resourcesPath, resources);
	if (ec != SUCCESS) {
		std::cout << "Failed to load resources json" << std::endl;
		return -1;
	}
	
	ec = initRendering(resources);
	if (ec != SUCCESS) {
		std::cout << "Failed to init rendering " << ec << std::endl;
		return -1;
	}
	initPhysicsEngine();


	mainLoop();

	exitPhysicsEngine();
	exitRendering();

	return 0;
}

void mainLoop() {
	Player* player = new Player();

	initGameObject3D(1, 0, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

	
	glm::vec3 vert1 = glm::vec3(-0.5f, -0.5f, 0.5f);
	glm::vec3 vert2 = glm::vec3(-0.5f, -0.5f, -0.5f);
	glm::vec3 vert3 = glm::vec3(-0.5f, 0.5f, 0.5f);

	initGameObject3D(2, 0, vert1, glm::vec3(0.0f), 0.2f);
	initGameObject3D(2, 0, vert2, glm::vec3(0.0f), 0.2f);
	initGameObject3D(2, 0, vert3, glm::vec3(0.0f), 0.2f);

	bool running = true;

	btRigidBody* playerSphere = registerCollisionSphere(glm::vec3(0, 10, 0));

	glm::vec3 collPosition = glm::vec3(1.0f, 0.0f, 0.0f);
	initGameObject3D(2, 0, collPosition, glm::vec3(0.0f), 1.0f);
	btRigidBody* boxMesh = registerStaticCollisionMesh("assets/models/green.obj", collPosition);

	while (running) {

		updateInputState();
		
		if (handleInput(QUIT)) {
			running = false;
			break;
		}
		updateGame();

		player->update();
		btVector3 mov = btVector3(player->controller->move.x, player->controller->move.y, player->controller->move.z);
		playerSphere->translate(mov);

		updatePhysicsEngine();

		btVector3 pos = playerSphere->getCenterOfMassPosition();

		player->controller->position = glm::vec3(pos.x(), pos.y(), pos.z());
		setCameraParams(player->controller->position, player->controller->direction, player->controller->up);
		
		renderFrame();

	}
}