// This header defines the Renderer class, which renders a board to a window

#pragma once

#include "shader.hpp"

class Window;
class Board;

class Renderer {

public:
	Renderer();
	~Renderer() noexcept;

	// Draws the board to the window argument. If drawSelected, a ghost piece is drawn
	// at cursor's selected cell, if existent. If lastPly has a value, the cell is darkened slightly,
	// (indicating this cell was the one that was just played on)
	void Draw(Window& window, const Board& board, const bool drawSelected, 
		const std::optional<size_t> lastPly);

	// Renderer move is not trivial, it manages resources
	Renderer(Renderer&& other) noexcept;
	Renderer& operator=(Renderer&& other) noexcept;

	// Renderer cannot be copied
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
private:

	ShaderProgram rectShader;
	ShaderProgram circleShader;
	unsigned int normalSquareVAO;
	unsigned int normalSquareVBO;
};