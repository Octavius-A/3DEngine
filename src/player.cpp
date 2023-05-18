#pragma once

#include "player.h"

#include <iostream>

Player::Player() {
	controller = new FirstPersonController();	
	controller->position = glm::vec3(0.0f, 0.0f, 0.0f);
	collisionSphere = new CollisionSphere{ controller->position, 0.5f };
	registerCollisionSphere(collisionSphere);
}

void Player::update() {
	controller->update();
	// Need a better way of doing this.
	collisionSphere->position = controller->position;
}