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

SDL_Window* window = NULL;
SDL_GLContext glContext;
int screenWidth;
int screenHeight;

std::unordered_map<unsigned int, Shader*> shaderBank;
std::unordered_map<unsigned int, Model*> modelBank;


Camera camera = Camera{
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
		modelBank[modelId] = new Model(path.c_str());
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
	
	SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	

	SDL_CaptureMouse(SDL_FALSE);

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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	return SUCCESS;
}

ERROR_CODE initImGui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
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

void exitRendering() {
	for (auto shaderPair : shaderBank) {
		delete shaderPair.second;
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyWindow(window);
	try {
		SDL_Quit();
	}
	catch (char* e) {
		std::cout << "Exception Caught: " << e << std::endl;
	}
}

void setCameraParams(glm::vec3 pos, glm::vec3 front, glm::vec3 up){
	camera.pos = pos;
	camera.front = front;
	camera.up = up;
}

void renderFrame() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(camera.fov), (float)screenWidth / (float)screenHeight, 0.1f, 50.0f);

	// Render Game objects with the basic shader
	Shader* basicShader = shaderBank[1];
	basicShader->use();
	basicShader->setMat4("projection", projection);
	basicShader->setMat4("view", view);
	basicShader->setInt("material.diffuse", 0);

	// Set the directional light
	basicShader->setVec3("dirLight.direction", -0.3f, -1.0f, -1.0f);
	basicShader->setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
	basicShader->setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);

	std::vector<GameObject3D*>::iterator it;
	for (it = globalGameState.gameObjects.begin(); it != globalGameState.gameObjects.end(); ++it) {
		GameObject3D* obj = *it;
		glm::vec3 position = obj->position;
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::scale(model, glm::vec3(obj->scale, obj->scale, obj->scale));
		// rotate?
		basicShader->setMat4("model", model);
		modelBank[obj->modelId]->draw();
	}


	ImGui::Begin("Debug");
	ImVec2 size(300, 100);
	ImVec2 pos(0, 0);
	ImGui::SetWindowSize(size);
	ImGui::SetWindowPos(pos);

	float camX = camera.pos.x;
	float camY = camera.pos.y;
	float camZ = camera.pos.z;
	ImGui::Text("pos: %f, %f, %f", camX, camY, camZ);

	float dirX = camera.front.x;
	float dirY = camera.front.y;
	float dirZ = camera.front.z;
	ImGui::Text("dir: %f, %f, %f", dirX, dirY, dirZ);

	int fps = globalGameState.fps;
	ImGui::Text("fps: %d", fps);
	ImGui::End();



	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(window);
}
