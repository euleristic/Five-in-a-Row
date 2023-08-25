#include "scopedLibrary.hpp"

#include "GLincludes.hpp"
#include "reflections.hpp"

#include <stdexcept>
#include <format>

ScopedLibrary::ScopedLibrary() : owner(true) {
	if (!glfwInit()) {
		throw std::runtime_error(std::format("Could not initialize GLFW: {}", ReflectGLFWerror(glfwGetError(nullptr))));
	};
}

ScopedLibrary::~ScopedLibrary() noexcept {
	if (owner)
		glfwTerminate();
}

// Transfer ownership (if other is owning)
ScopedLibrary::ScopedLibrary(ScopedLibrary&& other) noexcept : 
	owner(std::exchange(other.owner, false)) {}


// Transfer ownership (if other is owning)
ScopedLibrary& ScopedLibrary::operator=(ScopedLibrary&& other) noexcept {
	owner = std::exchange(other.owner, false);
	return *this;
}

