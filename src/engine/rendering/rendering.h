#pragma once

#include "../utils/errorCode.h"
#include "../utils/io.h"
#include "../gameState.h"

#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_Image.h>
#include <glm/glm.hpp>

struct Camera {
	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;
	float fov;
};

ERROR_CODE initRendering(json resources);
//void exitRendering();
void renderFrame();

