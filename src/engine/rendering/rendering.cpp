#include "rendering.h"
#include "shader.h"
#include "model.h"
#include "../utils/io.h"
#include "../gameState.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>


ERROR_CODE initSDL(const char* windowTitle, int screenWidth, int screenHeight);
ERROR_CODE initGL();
ERROR_CODE initImGui();
void renderGameObjects(); // TODO: add flag to override shader?

SDL_Window* window = NULL;
SDL_GLContext glContext;
int screenWidth;
int screenHeight;

std::unordered_map<unsigned int, Shader*> shaderBank;
std::unordered_map<unsigned int, Model*> modelBank;


Camera camera{
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	80.0f
};

ERROR_CODE initRendering(json resources) {
	ERROR_CODE ec = SUCCESS;

	std::string windowTitle = resources["windowTitle"];
	screenWidth = resources["screenWidth"];
	screenHeight = resources["screenHeight"];
	
	ec = initSDL(windowTitle.c_str(), screenWidth, screenHeight);
	if (ec != SUCCESS) return ec;

	ec = initGL();
	if (ec != SUCCESS) return ec;

	ec = initImGui();
	if (ec != SUCCESS) return ec;

	json::iterator shaderIt;
	for (shaderIt = resources["shaders"].begin(); shaderIt != resources["shaders"].end(); ++shaderIt) {
		json shaderData = *shaderIt;
		unsigned int id = shaderData["id"];
		std::string vertPath = shaderData["vertPath"];
		std::string fragPath = shaderData["fragPath"];

		shaderBank[id] = new Shader(vertPath.c_str(), fragPath.c_str());
	}

	std::string modelDataPath = resources["modelData"];
	json models;
	ec = loadJson(modelDataPath.c_str(), models);
	if (ec != SUCCESS) return ec;

	json::iterator modelsIt;
	for (modelsIt = models["models"].begin(); modelsIt != models["models"].end(); ++modelsIt) {
		json modelData = *modelsIt;
		unsigned int modelId = modelData["id"];
		std::string path = modelData["path"];
		std::string shader = modelData["shader"];
		modelBank[modelId] = new Model(path.c_str(), shader);
	}

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

GLuint loadTexture(const char* path) {
	GLuint textureID;
	SDL_Surface* surface = IMG_Load(path);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	int mode = GL_RGB;

	if (surface->format->BytesPerPixel == 4) {
		mode = GL_RGBA;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	SDL_FreeSurface(surface);
	return textureID;
}

void renderFrame() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	glEnable(GL_DEPTH_TEST);

	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(camera.fov), (float)screenWidth / (float)screenHeight, 0.1f, 50.0f);

	// Render Game objects with the basic shader
	Shader* basicShader = shaderBank[0];
	basicShader->use();
	basicShader->setMat4("projection", projection);
	basicShader->setMat4("view", view);
	basicShader->setInt("material.diffuse", 0);
	std::vector<GameObject3D*>::iterator it;
	for (it = globalGameState.gameObjects.begin(); it != globalGameState.gameObjects.end(); ++it) {
		GameObject3D* obj = *it;
		glm::vec3 position = obj->position;
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		// rotate?
		basicShader->setMat4("model", model);
		modelBank[obj->modelId]->draw();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(window);
}

