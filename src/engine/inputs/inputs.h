#pragma once
#include <SDL.h>

enum INPUT {
	QUIT,
	W,
	A,
	S,
	D,
	Q,
	MOUSE_LEFT,
	MOUSE_RIGHT
};

void updateInputState();
bool handleInput(INPUT input);