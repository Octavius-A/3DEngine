#pragma once

#include "gameState.h"

class FirstPersonController {
public:
	FirstPersonController();
	~FirstPersonController(){}
	void update();
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;
};