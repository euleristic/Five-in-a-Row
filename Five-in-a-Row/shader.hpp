#pragma once

#include <filesystem>
#include "glm/glm.hpp"

class ShaderProgram {
public:
	ShaderProgram() noexcept;
	ShaderProgram(const std::filesystem::path vertexShaderPath, const std::filesystem::path fragmentShaderPath);

	ShaderProgram(ShaderProgram&& from) noexcept;
	ShaderProgram& operator=(ShaderProgram&& from) noexcept;
	~ShaderProgram() noexcept;

	void Run(const glm::mat3 transform, const glm::vec4 color);

	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;
private:
	unsigned int id;
	unsigned int transformUniform;
	unsigned int colorUniform;
};