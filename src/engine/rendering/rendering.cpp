#include "rendering.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



ERROR_CODE initSDL(const char* windowTitle, int screenWidth, int screenHeight);
ERROR_CODE initGL();
ERROR_CODE initImGui();

SDL_Window* window = NULL;
SDL_GLContext glContext;

ERROR_CODE initRendering(json resources) {
	ERROR_CODE ec = SUCCESS;

	std::string windowTitle = resources["windowTitle"];
	int screenWidth = resources["screenWidth"];
	int screenHeight = resources["screenHeight"];
	
	ec = initSDL(windowTitle.c_str(), screenWidth, screenHeight);
	if (ec != SUCCESS) return ec;

	ec = initGL();
	if (ec != SUCCESS) return ec;

	ec = initImGui();
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

ERROR_CODE initGL() {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);


	glContext = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, glContext);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		return FAILED_INIT_GL;
	}

	return SUCCESS;
}

ERROR_CODE initImGui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	return SUCCESS;
}