#include "firstPersonController.h"

#include "inputs/inputs.h"
#include "rendering/rendering.h"

#include <iostream>

FirstPersonController::FirstPersonController() {
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	direction = glm::vec3(0.0f, 0.0f, -1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void FirstPersonController::update() {
	double dTime = globalGameState.dTime;

	static float yaw = 0.0f;
	static float pitch = 0.0f;
	static float maxPitch = 89.0f;
	static float minPitch = -89.0f;

	int mouseX;
	int mouseY;
	getMouseXY(mouseX, mouseY);

	float xoffset = (float)mouseX * 100.0f * dTime;
	float yoffset = (float)mouseY * 100.0f * dTime;

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

	static float playerSpeed = 2.0f;
	float finalSpeed = playerSpeed * dTime;
	float strafeSpeed = (playerSpeed * 0.5) * dTime;

	move = glm::vec3(0.0f,0.0f,0.0f);

	if (checkInputState(W)) {
		move += moveDir * finalSpeed;
	}
	if (checkInputState(S)) {
		move -= moveDir * finalSpeed;
	}
	if (checkInputState(A)) {
		move -= glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f))) * strafeSpeed;
	}
	if (checkInputState(D)) {
		move += glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f))) * strafeSpeed;
	}

	//setCameraParams(position, direction, up);
}