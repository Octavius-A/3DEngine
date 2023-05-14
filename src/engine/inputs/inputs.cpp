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

void updateInputState() {
	SDL_Event event;

	//SDL_GetRelativeMouseState(&g_inputState.mouseState.mouseX, &g_inputState.mouseState.mouseY);

	ImGui_ImplSDL2_ProcessEvent(&event);

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
			}
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
		default: break;
		}
	}
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