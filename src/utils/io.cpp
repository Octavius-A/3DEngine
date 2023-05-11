#include <fstream>

#include "io.h"

ERROR_CODE loadJson(const char* path, json& output) {
	ERROR_CODE ec = SUCCESS;
	try {
		std::ifstream i(path);
		i >> output;
	}
	catch (const std::ifstream::failure& e) {
		ec = FAILED_LOAD_RESOURCE;
	}
	return ec;
}