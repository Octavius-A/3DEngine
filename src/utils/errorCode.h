#pragma once

typedef enum {
	SUCCESS = 0,
	FAIL = 1,
	FAILED_INIT_SDL = 2,
	FAILED_INIT_GL = 3,
	FAILED_LOAD_RESOURCE = 4
} ERROR_CODE;