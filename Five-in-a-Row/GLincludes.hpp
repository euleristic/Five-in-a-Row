#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

// A little useful macro for debugging
#define CheckError() if (GLenum err = glGetError(); err != GL_NO_ERROR) throw std::runtime_error(ReflectGLerror(err))