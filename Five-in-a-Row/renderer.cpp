#include "renderer.hpp"
#include "reflections.hpp"
#include "constants.hpp"
#include "board.hpp"
#include "window.hpp"

#include "glm/glm.hpp"

#include <utility>
#include <stdexcept>
#include <format>

using namespace Constants;

Renderer::Renderer() {

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw std::runtime_error("Failed to initialize GLAD.");
	}

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	rectShader = ShaderProgram("rectVertexShader.glsl", "rectFragmentShader.glsl");
	circleShader = ShaderProgram("circleVertexShader.glsl", "circleFragmentShader.glsl");

	glGenVertexArrays(1, &normalSquareVAO);
	glBindVertexArray(normalSquareVAO);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &normalSquareVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalSquareVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(NORMAL_SQUARE_VERTEX_BUFFER), NORMAL_SQUARE_VERTEX_BUFFER, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), reinterpret_cast<void*>(0));

	// We'll leave these bound for the entirety of the application, since it's the only ones we'll use

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Renderer::~Renderer() noexcept {
	glDeleteBuffers(1, &normalSquareVBO);
	glDeleteVertexArrays(1, &normalSquareVAO);
}

void Renderer::Draw(Window& window, const Board& board, const bool drawSelected) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	auto selected = board.Selected(window.CursorPosition());

	for (size_t j = 0; j < BOARD_HEIGHT; ++j) {
		for (size_t i = 0; i < BOARD_WIDTH; ++i) {
			glm::mat3 transform(0.0f);
			transform[0][0] = static_cast<float>(CELL_WIDTH_FACTOR) / static_cast<float>(BOARD_WIDTH);
			transform[1][1] = static_cast<float>(CELL_WIDTH_FACTOR) / static_cast<float>(BOARD_HEIGHT);
			transform[2][2] = 1.0f;

			transform[2][0] = -1.0f + // left edge
				1.0f / static_cast<float>(BOARD_WIDTH) + // initial offset
				static_cast<float>(i) * 2.0f / static_cast<float>(BOARD_WIDTH); // additional offset per i
			
			transform[2][1] = 1.0f - // top edge
				1.0f / static_cast<float>(BOARD_HEIGHT) - // initial offset
				static_cast<float>(j) * 2.0f / static_cast<float>(BOARD_HEIGHT); // additional offset per j

			const auto color = (drawSelected && selected != board.EmptyEnd() 
				&& selected.Position().first == i && selected.Position().second == j) ? 
				glm::vec4(0.8f, 0.8f, 0.8f, 1.0f) : glm::vec4(1.0f);

			rectShader.Run(transform, color);
			
			if (auto state = board.At(i, j); state != CellState::EMPTY) {

				// "Outer" circle
				transform[0][0] *= PIECE_WIDTH_FACTOR;
				transform[1][1] *= PIECE_WIDTH_FACTOR;
				glm::vec4 color = state == CellState::BLUE ? glm::vec4(0.0f, 0.0f, OUTER_COLOR_FACTOR, 1.0f)
					: glm::vec4(OUTER_COLOR_FACTOR, 0.0f, 0.0f, 1.0f);
				circleShader.Run(transform, color);

				// "Inner" circle
				transform[0][0] *= PIECE_INNER_FACTOR;
				transform[1][1] *= PIECE_INNER_FACTOR;
				color = state == CellState::BLUE ? glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
					: glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
				circleShader.Run(transform, color);
			}
		}
	}
	
	window.DrawFrame();

}