#include "window.hpp"

#include "GLincludes.hpp"
#include "constants.hpp"
#include "reflections.hpp"
#include "board.hpp"

#include <format>
#include <string>

using namespace Constants;

// Mouse

constexpr uint8_t mouseDownBit = 0b0001;
constexpr uint8_t mouseClickedBit = 0b0010;
constexpr uint8_t resetDownBit = 0b0100;
constexpr uint8_t resetPressedBit = 0b1000;

Window::Window() {

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	std::string title = std::string(APPLICATION_NAME) + (PLAYER_FIRST ? PLAYER_TURN_SUFFIX : COMPUTER_TURN_SUFFIX);
	handle = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title.c_str(), nullptr, nullptr);
	if (!handle) {
		throw std::runtime_error(std::format("Could not create window: {}", ReflectGLFWerror(glfwGetError(nullptr))));
	}

	glfwMakeContextCurrent(handle);
}

Window::~Window() noexcept {
	if (handle) {
		glfwDestroyWindow(handle);
	}
}

bool Window::ShouldClose() const noexcept {
	return glfwWindowShouldClose(handle);
}

bool Window::ResetPressed() const noexcept {
	return inputState & resetPressedBit;
}

bool Window::Clicked() const noexcept {
	return inputState & mouseClickedBit;
}

void Window::SetTitle(const char* to) {
	if (!to)
		throw std::runtime_error("Could not set title, arg \"to\" was nullptr");
	glfwSetWindowTitle(handle, to);
}

std::pair<double, double> Window::CursorPosition() const {
	double x, y;
	glfwGetCursorPos(handle, &x, &y);
	return std::make_pair(x, y);
}

void Window::Update() noexcept {
	glfwPollEvents();

	// Update input state (it's a bit convoluded, but basically we only want clicked-/pressedBit to be set for only one tick)
	// (And we want to conserve memory, so it's only a byte)

	// Mouse
	inputState = (inputState & ~mouseClickedBit) | ((glfwGetMouseButton(handle, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE
		&& (inputState & mouseDownBit) != 0) ? mouseClickedBit : 0);
	inputState = (inputState & ~mouseDownBit) | ((glfwGetMouseButton(handle, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) ?
		mouseDownBit : 0);

	// Reset Button
	inputState = (inputState & ~resetPressedBit) | ((glfwGetKey(handle, GLFW_KEY_R) == GLFW_PRESS
		&& (inputState & resetDownBit) == 0) ? resetPressedBit : 0);
	inputState = (inputState & ~resetDownBit) | ((glfwGetKey(handle, GLFW_KEY_R) == GLFW_PRESS) ?
		resetDownBit : 0);
}

void Window::DrawFrame() noexcept {
	glfwSwapBuffers(handle);
}