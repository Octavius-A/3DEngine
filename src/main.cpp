#include "engine/rendering/rendering.h"
#include "engine/inputs/inputs.h"
#include "engine/gameState.h"
#include "engine/collision/collision.h"
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
	
	mainLoop();

	exitRendering();

	return 0;
}

void mainLoop() {

	Player* player = new Player();

	/*initGameObject3D(0, 0, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
	initGameObject3D(0, 0, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
	initGameObject3D(0, 0, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
	initGameObject3D(0, 0, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);*/
	initGameObject3D(1, 0, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

	
	glm::vec3 vert1 = glm::vec3(-0.5f, -0.5f, 0.5f);
	glm::vec3 vert2 = glm::vec3(-0.5f, -0.5f, -0.5f);
	glm::vec3 vert3 = glm::vec3(-0.5f, 0.5f, 0.5f);

	initGameObject3D(2, 0, vert1, glm::vec3(0.0f), 0.2f);
	initGameObject3D(2, 0, vert2, glm::vec3(0.0f), 0.2f);
	initGameObject3D(2, 0, vert3, glm::vec3(0.0f), 0.2f);

	Model* testModel = new Model("assets/models/green.obj");

	Mesh testMesh = testModel->meshes.at(0);
	for (auto i : testMesh.indices) {
		std::cout << i << std::endl;
	}


	//CollisionMesh* testMesh = new CollisionMesh{
	//	{{glm::vec3(-0.5f, -1.0f, 0.5f), glm::vec3(-0.5f, -1.0f, -0.5f), glm::vec3(0.5f, -1.0f, 0.5f)}},
	//	{{glm::vec3(0.5f, -1.0f, 0.5f), glm::vec3(-0.5f, -1.0f, -0.5f), glm::vec3(0.5f, -1.0f, -0.5f)}}
	//};

	//registerCollisionMesh(testMesh);

	bool running = true;

	while (running) {

		updateInputState();

		if (handleInput(QUIT)) {
			running = false;
			break;
		}
		updateGame();

		player->update();

		//updateCollisionObjects();

		player->controller->position = player->collisionSphere->position;
		
		renderFrame();

	}

}