#pragma once

#include "inputs.h"

#include "imgui_impl_sdl.h"

#include <map>

// This needs some thought
std::map<INPUT, bool> currentInputState = {
	{QUIT, false},
	{W, false},
	{A, false},
	{S, false},
	{D, false},
	{Q, false},
	{MOUSE_LEFT, false},
	{MOUSE_RIGHT, false},
};

int mouseX = 0;
int mouseY = 0;

Sint32 mouseRelX = 0;
Sint32 mouseRelY = 0;

void updateInputState() {
	SDL_Event event;
	
	mouseRelX = 0;
	mouseRelY = 0;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: currentInputState[QUIT] = true; break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_q: currentInputState[QUIT] = true; break;
			case SDLK_w: currentInputState[W] = true; break;
			case SDLK_a: currentInputState[A] = true; break;
			case SDLK_s: currentInputState[S] = true; break;
			case SDLK_d: currentInputState[D] = true; break;
			default: break;
			} break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_q: currentInputState[QUIT] = false; break;
			case SDLK_w: currentInputState[W] = false; break;
			case SDLK_a: currentInputState[A] = false; break;
			case SDLK_s: currentInputState[S] = false; break;
			case SDLK_d: currentInputState[D] = false; break;
			default: break;
			} break;
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button) {
			case SDL_BUTTON_LEFT: currentInputState[MOUSE_LEFT] = true; break;
			case SDL_BUTTON_RIGHT: currentInputState[MOUSE_RIGHT] = true; break;
			default: break;
			} break;
		case SDL_MOUSEBUTTONUP:
			switch (event.button.button) {
			case SDL_BUTTON_LEFT: currentInputState[MOUSE_LEFT] = false; break;
			case SDL_BUTTON_RIGHT: currentInputState[MOUSE_RIGHT] = false; break;
			} break;
		case SDL_MOUSEMOTION:
			mouseRelX = event.motion.xrel;
			mouseRelY = event.motion.yrel;
			break;
		default: break;
		}
	}

	//ImGui_ImplSDL2_ProcessEvent(&event);
}

bool handleInput(INPUT input) {
	bool ret = false;
	if (currentInputState[input]) {
		ret = true;
		// key has been "handled"
		currentInputState[input] = false;
	}
	return ret;
}

bool checkInputState(INPUT input) {
	return currentInputState[input];
}

void getMouseXY(Sint32& x, Sint32& y) {
	x = mouseRelX;
	y = mouseRelY;
}