#pragma once

#include "engine/firstPersonController.h"
#include "engine/collision/collision.h"

class Player {
public:
	Player();
	~Player() {}
	void update();
	FirstPersonController* controller;
	CollisionSphere* collisionSphere;
private:

};