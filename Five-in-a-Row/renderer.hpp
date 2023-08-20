// This header contains the Renderer class declaration.
// Renderer provides an interface to the application window and its drawing.

#pragma once

#include "shader.hpp"

class Window;
class Board;

class Renderer {

public:
	// Window width, height and title are specified in constants.hpp
	Renderer();
	~Renderer() noexcept;

	void Draw(Window& window, const Board& board, const bool drawSelected, 
		const std::optional<std::pair<size_t, size_t>> lastMove);

	// Renderer cannot be copied
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
private:

	ShaderProgram rectShader;
	ShaderProgram circleShader;
	unsigned int normalSquareVAO;
	unsigned int normalSquareVBO;
};