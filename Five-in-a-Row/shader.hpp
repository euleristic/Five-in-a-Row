// This header defines the ShaderProgram class, which encapsulates an OpenGL program object

#pragma once

#include <filesystem>
#include "glm/glm.hpp"

class ShaderProgram {
public:
	ShaderProgram() noexcept;

	// Loads, compiles and links the shaders at the given paths to the program.
	// The program is limited to the use of two uniforms:
	// a 3x3 matrix named "transformUniform", and a 4d vector named "colorUniform"

	ShaderProgram(const std::filesystem::path vertexShaderPath, const std::filesystem::path fragmentShaderPath);

	ShaderProgram(ShaderProgram&& from) noexcept;
	ShaderProgram& operator=(ShaderProgram&& from) noexcept;
	~ShaderProgram() noexcept;

	// Runs the program, i.e. draws, with the transform
	void Run(const glm::mat3 transform, const glm::vec4 color);

	// ShaderProgram cannot be copied
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;
private:
	unsigned int id{};
	unsigned int transformUniform;
	unsigned int colorUniform;
};