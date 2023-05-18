#include "engine/rendering/rendering.h"
#include "engine/inputs/inputs.h"

#include <iostream>

void mainLoop();

int main(int argc, char* args[]) {
	const char* resourcesPath = "resources.json";
	json resources;

	ERROR_CODE ec = SUCCESS;
	ec = loadJson(resourcesPath, resources);
	if (ec != SUCCESS) {
		std::cout << "Failed to load resources json" << std::endl;
		return -1;
	}
	
	ec = initRendering(resources);
	if (ec != SUCCESS) {
		std::cout << "Failed to init rendering " << ec << std::endl;
		return -1;
	}

	mainLoop();

	return 0;
}

void mainLoop() {

	bool running = true;

	while (running) {
		updateInputState();

		if (handleInput(QUIT)) {
			running = false;
			break;
		}
		
		renderFrame();

	}

}