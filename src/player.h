#pragma once

#include "engine/firstPersonController.h"

class Player {
public:
	Player();
	~Player() {}
	void update();
private:
	FirstPersonController* controller;
};