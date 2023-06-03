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

	initGame();

	Player* player = new Player();

	json levelData;
	loadJson("levels/level01.json", levelData);
	initLevel(levelData);


	// init the collision world
	//initGameObject3D(3, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	//initGameObject3D(4, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	//initGameObject3D(5, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	//initGameObject3D(6, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	//initGameObject3D(7, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	//initGameObject3D(8, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	//initGameObject3D(9, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	//initGameObject3D(10, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	//loadPhysicsWorld("assets/models/level_01_collision.obj");
	//loadPhysicsWorld("assets/models/lighting_test.obj");
	//loadPhysicsWorld("assets/models/collisiontest.obj");
	bool running = true;

	glm::vec3 playerOrigin = glm::vec3(levelData["playerStart"][0],
		levelData["playerStart"][1],
		levelData["playerStart"][2]);
	btKinematicCharacterController* pc = registerCharacterController(playerOrigin);

	//renderShadowMaps();
	
	while (running) {

		updateInputState();
		
		if (handleInput(QUIT)) {
			running = false;
			break;
		}
		updateGame();

		player->update();


		btScalar walkVelocity = btScalar(100);
		btScalar walkSpeed = walkVelocity * globalGameState.dTime;


		btVector3 mov = btVector3(player->controller->move.x, player->controller->move.y, player->controller->move.z);


		pc->setWalkDirection(mov * walkSpeed);

		updatePhysicsEngine(pc);

	
		btVector3 pos = pc->getGhostObject()->getWorldTransform().getOrigin();

		player->controller->position = glm::vec3(pos.x(), pos.y() + 0.75, pos.z());

		setCameraParams(player->controller->position, player->controller->direction, player->controller->up);
		//
		renderFrame();

	}
}