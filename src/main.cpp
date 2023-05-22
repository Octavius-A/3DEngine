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

	/*initGameObject3D(1, 0, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

	
	glm::vec3 vert1 = glm::vec3(-0.5f, -0.5f, 0.5f);
	glm::vec3 vert2 = glm::vec3(-0.5f, -0.5f, -0.5f);
	glm::vec3 vert3 = glm::vec3(-0.5f, 0.5f, 0.5f);

	initGameObject3D(2, 0, vert1, glm::vec3(0.0f), 0.2f);
	initGameObject3D(2, 0, vert2, glm::vec3(0.0f), 0.2f);
	initGameObject3D(2, 0, vert3, glm::vec3(0.0f), 0.2f);*/


	// init the collision world
	initGameObject3D(3, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	initGameObject3D(4, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	initGameObject3D(5, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	initGameObject3D(6, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	initGameObject3D(7, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	initGameObject3D(8, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	initGameObject3D(9, 0, glm::vec3(0.0f), glm::vec3(0.0f), 1.0f);
	registerStaticCollisionMesh("assets/models/collisiontest_ground.obj", glm::vec3(0.0f));
	registerStaticCollisionMesh("assets/models/collisiontest_long_slope.obj", glm::vec3(0.0f));
	registerStaticCollisionMesh("assets/models/collisiontest_short_slope.obj", glm::vec3(0.0f));
	registerStaticCollisionMesh("assets/models/collisiontest_wall_1.obj", glm::vec3(0.0f));
	registerStaticCollisionMesh("assets/models/collisiontest_wall_2.obj", glm::vec3(0.0f));
	registerStaticCollisionMesh("assets/models/collisiontest_wall_3.obj", glm::vec3(0.0f));
	registerStaticCollisionMesh("assets/models/collisiontest_wall_4.obj", glm::vec3(0.0f));
	bool running = true;

	//btRigidBody* playerSphere = registerCollisionSphere(glm::vec3(0, 10, 0), 0.5f, 80.f);
	//btRigidBody* playerRB = registerCollisionCapsule(glm::vec3(0, 10, 0), 0.25, 0.5, 80);

	btKinematicCharacterController* pc = registerCharacterController();

	/*glm::vec3 collPosition = glm::vec3(1.0f, 0.0f, 0.0f);
	initGameObject3D(2, 0, collPosition, glm::vec3(0.0f), 1.0f);
	btRigidBody* boxMesh = registerStaticCollisionMesh("assets/models/green.obj", collPosition);*/

	while (running) {

		updateInputState();
		
		if (handleInput(QUIT)) {
			running = false;
			break;
		}
		updateGame();

		player->update();




		//btVector3 mov = btVector3(player->controller->move.x, player->controller->move.y, player->controller->move.z);
		//playerSphere->translate(mov);
		//playerRB->setLinearVelocity(mov);

		btTransform xform;
		xform = pc->getGhostObject()->getWorldTransform();
		btVector3 forwardDir = xform.getBasis()[2];
		//	printf("forwardDir=%f,%f,%f\n",forwardDir[0],forwardDir[1],forwardDir[2]);
		


		
		
		
		btVector3 upDir = xform.getBasis()[1];
		btVector3 strafeDir = xform.getBasis()[0];
		forwardDir.normalize();

		btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
		//btScalar walkVelocity = btScalar(1.1) * 4.0; // 4 km/h -> 1.1 m/s
		btScalar walkVelocity = btScalar(100);
		btScalar walkSpeed = walkVelocity * globalGameState.dTime;

		

		//if (checkInputState(W)) {
		//	//move += moveDir * finalSpeed;
		//	walkDirection += forwardDir;
		//}
		//if (checkInputState(S)) {
		//	//move += moveDir * finalSpeed;
		//	walkDirection -= forwardDir;
		//}

		btVector3 mov = btVector3(player->controller->move.x, player->controller->move.y, player->controller->move.z);


		pc->setWalkDirection(mov * walkSpeed);

		updatePhysicsEngine(pc);

		/*btVector3 linVel = playerRB->getLinearVelocity();*/

		//btVector3 pos = playerRB->getCenterOfMassPosition();

	
		btVector3 pos = pc->getGhostObject()->getWorldTransform().getOrigin();

		player->controller->position = glm::vec3(pos.x(), pos.y() + 0.5f, pos.z());
		//player->controller->position = glm::vec3(0, 0, 0);


		//player->controller->move = glm::vec3(linVel.x(), linVel.y(), linVel.z());
		setCameraParams(player->controller->position, player->controller->direction, player->controller->up);
		
		renderFrame();

	}
}