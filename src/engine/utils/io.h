#pragma once
#include "errorCode.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

ERROR_CODE loadJson(const char* path, json& out);