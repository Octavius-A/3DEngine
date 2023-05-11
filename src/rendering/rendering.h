#pragma once

#include "../utils/errorCode.h"
#include "../utils/io.h"

#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_Image.h>

// This should just take a single path to a "resources" json file.
// This should include the title of the window, the link to the texture data, model data, etc.
// also a link to the shaders??


ERROR_CODE initRendering(json resources);
//void exitRendering();
//void renderFrame();

