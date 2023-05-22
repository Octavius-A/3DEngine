#include "firstPersonController.h"

#include "inputs/inputs.h"
#include "rendering/rendering.h"

#include <iostream>

FirstPersonController::FirstPersonController() {
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	direction = glm::vec3(0.0f, 0.0f, -1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	move = glm::vec3(0.0f, 0.0f, 0.0f);
}

void FirstPersonController::update() {
	double dTime = globalGameState.dTime;

	static double yaw = 0.0f;
	static double pitch = 0.0f;
	static double maxPitch = 89.0f;
	static double minPitch = -89.0f;


	Sint32 mouseX = 0;
	Sint32 mouseY = 0;
	getMouseXY(mouseX, mouseY);
	

	double xoffset = (double)mouseX * 100.0f * dTime;
	double yoffset = (double)mouseY * 100.0f * dTime;

	yaw += xoffset;
	pitch -= yoffset;

	if (pitch > maxPitch) pitch = maxPitch;
	if (pitch < minPitch) pitch = minPitch;

	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(direction);

	glm::vec3 moveDir;
	moveDir.x = cos(glm::radians(yaw));
	moveDir.y = 0;
	moveDir.z = sin(glm::radians(yaw));
	moveDir = glm::normalize(moveDir);

	static float playerSpeed = 0.01f;
	float finalSpeed = playerSpeed * dTime;
	float strafeModifier = 1.0f;
	float strafeSpeed = (playerSpeed * strafeModifier) * dTime;


	//move = glm::vec3(0.0f,0.0f,0.0f);
	move = glm::vec3(0.0f);
	
	if (checkInputState(W)) {
		//move += moveDir * finalSpeed;
		// 
		//move = moveDir;
		move += moveDir;
	}
	if (checkInputState(S)) {
		//move -= moveDir * finalSpeed;
		//move = -moveDir;
		move -= moveDir;
	}
	if (checkInputState(A)) {
		move -= glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
	}
	if (checkInputState(D)) {
		move += glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	/*float len = glm::length(move);
	std::cout << len << std::endl;*/
	
	//setCameraParams(position, direction, up);
}