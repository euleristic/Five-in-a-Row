#include "scopedLibrary.hpp"

#include "GLincludes.hpp"
#include "reflections.hpp"

#include <stdexcept>
#include <format>

ScopedLibrary::ScopedLibrary() {
	if (!glfwInit()) {
		throw std::runtime_error(std::format("Could not initialize GLFW: {}", ReflectGLFWerror(glfwGetError(nullptr))));
	};
}

ScopedLibrary::~ScopedLibrary() noexcept {
	glfwTerminate();
}