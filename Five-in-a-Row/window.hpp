// This header defines the Window class, which encapsulates a GLFW window and manages relevant input
#pragma once

#include <utility>

struct GLFWwindow;

class Window {
public:
	// Window parameters are defined in "constants.hpp"
	Window();
	~Window() noexcept;

	// Should the window close?
	bool ShouldClose() const;
	// Was the R-key pressed?
	bool ResetPressed() const;
	// Was the Z-key pressed?
	bool UndoPressed() const;
	// Was the H-key pressed?
	bool HelpPressed() const;
	// Was the left mouse button clicked?
	bool Clicked() const;
	// Sets the title of the window
	void SetTitle(const char* to);
	// Returns the position of the cursor in window space
	std::pair<double, double> CursorPosition() const;

	// Polls events and updates the input state
	void Update();

	// Swaps the window buffers
	void DrawFrame();

	Window(Window&& other) noexcept;
	Window& operator=(Window&& other) noexcept;

	// Window cannot be copied
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

private:
	GLFWwindow* handle;

	// Reset pressed, mouse clicked
	uint8_t inputState{};
};