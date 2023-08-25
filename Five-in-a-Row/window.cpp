#include "window.hpp"

#include "GLincludes.hpp"
#include "constants.hpp"
#include "reflections.hpp"
#include "board.hpp"

#include <format>
#include <string>

using namespace Constants;

// Input state representation

constexpr uint8_t mouseDownBit = 1 << 0;
constexpr uint8_t mouseClickedBit = 1 << 1;
constexpr uint8_t resetDownBit = 1 << 2;
constexpr uint8_t resetPressedBit = 1 << 3;
constexpr uint8_t undoDownBit = 1 << 4;
constexpr uint8_t undoPressedBit = 1 << 5;
constexpr uint8_t helpDownBit = 1 << 6;
constexpr uint8_t helpPressedBit = 1 << 7;

// Returns the result of setting the mask bit(s) of byte to value
uint8_t SetByte(const uint8_t byte, const uint8_t mask, const bool value) {
	return (byte & ~mask) | (value ? mask : 0);
}

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

bool Window::ShouldClose() const {
	return glfwWindowShouldClose(handle);
}

bool Window::ResetPressed() const {
	return inputState & resetPressedBit;
}

bool Window::UndoPressed() const {
	return inputState & undoPressedBit;
}

bool Window::HelpPressed() const {
	return inputState & helpPressedBit;
}

bool Window::Clicked() const {
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

void Window::Update() {
	glfwPollEvents();

	// Update input state
	// We only want Clicked() to be true for the frame that the mouse button was released,
	// and the various pressed-predicates to return true the frame the key was initially pressed

	// Mouse
	inputState = SetByte(inputState, mouseClickedBit, glfwGetMouseButton(handle, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE 
		&& (inputState & mouseDownBit));
	inputState = SetByte(inputState, mouseDownBit, 
		glfwGetMouseButton(handle, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS);

	// Reset Button
	inputState = SetByte(inputState, resetPressedBit,glfwGetKey(handle, GLFW_KEY_R) == GLFW_PRESS 
		&& !(inputState & resetDownBit));
	inputState = SetByte(inputState, resetDownBit, glfwGetKey(handle, GLFW_KEY_R) == GLFW_PRESS);

	// Undo Button
	inputState = SetByte(inputState, undoPressedBit,
		glfwGetKey(handle, GLFW_KEY_Z) == GLFW_PRESS && !(inputState & undoDownBit));
	inputState = SetByte(inputState, undoDownBit, glfwGetKey(handle, GLFW_KEY_Z) == GLFW_PRESS);

	// Help Button
	inputState = SetByte(inputState, helpPressedBit, glfwGetKey(handle, GLFW_KEY_H) == GLFW_PRESS
		&& !(inputState & helpDownBit));
	inputState = SetByte(inputState, helpDownBit, glfwGetKey(handle, GLFW_KEY_H) == GLFW_PRESS);
}

void Window::DrawFrame() {
	glfwSwapBuffers(handle);
}

Window::Window(Window&& other) noexcept :
	handle(std::exchange(other.handle, nullptr)),
	inputState(other.inputState) {}

Window& Window::operator=(Window&& other) noexcept {
	handle = std::exchange(other.handle, nullptr);
	inputState = other.inputState;
}