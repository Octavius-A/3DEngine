#pragma once

#include "player.h"

#include <iostream>

Player::Player() {
	controller = new FirstPersonController();	
	controller->position = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Player::update() {
	controller->update();
}