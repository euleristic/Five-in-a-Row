// This header contains constant expression reflections of error codes, since the C++ standard does not yet have it

#pragma once

#include "GLincludes.hpp"

constexpr const char* ReflectGLerror(GLenum code) {
	switch (code) {
	case 0: return "No error";
	case GL_INVALID_ENUM:					return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:					return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:				return "GL_INVALID_OPERATION";
	case GL_STACK_OVERFLOW:					return "GL_STACK_OVERFLOW";
	case GL_STACK_UNDERFLOW:				return "GL_STACK_UNDERFLOW";
	case GL_OUT_OF_MEMORY:					return "GL_OUT_OF_MEMORY";
	case GL_INVALID_FRAMEBUFFER_OPERATION:	return "GL_INVALID_FRAMEBUFFER_OPERATION";
	case GL_CONTEXT_LOST:					return "GL_CONTEXT_LOST";
	default:								return "UNKNOWN GL ERROR CODE";
	}
}

constexpr const char* ReflectGLFWerror(int code) {
	switch (code) {
	case 0:									return "No error";
	case GLFW_NOT_INITIALIZED:				return "GLFW_NOT_INITIALIZED";
	case GLFW_NO_CURRENT_CONTEXT:			return "GLFW_NO_CURRENT_CONTEXT";
	case GLFW_INVALID_ENUM:					return "GLFW_INVALID_ENUM";
	case GLFW_INVALID_VALUE:				return "GLFW_INVALID_VALUE";
	case GLFW_OUT_OF_MEMORY:				return "GLFW_OUT_OF_MEMORY";
	case GLFW_API_UNAVAILABLE:				return "GLFW_API_UNAVAILABLE";
	case GLFW_VERSION_UNAVAILABLE:			return "GLFW_VERSION_UNAVAILABLE";
	case GLFW_PLATFORM_ERROR:				return "GLFW_PLATFORM_ERROR";
	case GLFW_FORMAT_UNAVAILABLE:			return "GLFW_FORMAT_UNAVAILABLE";
	default:								return "UNKNOWN GLFW ERROR CODE";
	}
}