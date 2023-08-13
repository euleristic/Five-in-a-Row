#pragma once

#include <utility>

struct GLFWwindow;

class Window {
public:
	Window();
	~Window() noexcept;

	bool ShouldClose() const noexcept;
	bool ResetPressed() const noexcept;
	bool Clicked() const noexcept;
	void SetTitle(const char* to);
	std::pair<double, double> CursorPosition() const;

	void Update() noexcept;
	void DrawFrame() noexcept;

private:
	GLFWwindow* handle;

	// Reset pressed, mouse clicked
	uint8_t inputState{};
};