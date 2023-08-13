#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define CheckError throw std::runtime_error(ReflectGLerror(glGetError()))