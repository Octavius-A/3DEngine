#include "rendering.h"

ERROR_CODE initSDL(const char* windowTitle, int screenWidth, int screenHeight);

SDL_Window* window = NULL;
SDL_GLContext* glContext = NULL;

ERROR_CODE initRendering(json resources) {
	ERROR_CODE ec = SUCCESS;

	std::string windowTitle = resources["windowTitle"];
	int screenWidth = resources["screenWidth"];
	int screenHeight = resources["screenHeight"];
	
	ec = initSDL(windowTitle.c_str(), screenWidth, screenHeight);

	if (ec != SUCCESS) return ec;

	return ec;
}

ERROR_CODE initSDL(const char* windowTitle, int screenWidth, int screenHeight) {
	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screenWidth, screenHeight, SDL_WINDOW_OPENGL);

	if (window == NULL) {
		return FAILED_INIT_SDL;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_CaptureMouse(SDL_TRUE);

	return SUCCESS;
}