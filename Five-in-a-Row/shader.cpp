#include "shader.hpp"
#include "reflections.hpp"

#include "glm/gtc/type_ptr.hpp"

#include <fstream>
#include <format>

// A utility for loading the file at path into the returned string
std::string ReadFile(const std::filesystem::path path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error(std::format("Could not open the file {}.", path.generic_string()));
	}
	std::stringstream stream;
	stream << file.rdbuf();
	return stream.str();
}

// A shader class to simplify the code, which is hidden from the user of ShaderProgram
class Shader {
public:
	Shader(const GLenum type, const std::filesystem::path path) {
		id = glCreateShader(type);
		if (!id) {
			throw std::runtime_error(std::format("Could not create shader: {}.", ReflectGLerror(glGetError())));
		}
		const auto sourceString = ReadFile(path);
		const auto ptr = sourceString.c_str();

		glShaderSource(id, 1, &ptr, nullptr);
		glCompileShader(id);

		int success;
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(id, 512, nullptr, infoLog);
			throw std::runtime_error(std::format("Could not compile shader {}: {}", path.generic_string(), infoLog));
		}
	}

	~Shader() noexcept {
		// A moved from shader will be ignored
		glDeleteShader(id);
	};

	Shader(Shader&& other) noexcept :
		id(std::exchange(other.id, 0)) {}


	Shader& operator=(Shader&& other) noexcept {
		id = std::exchange(other.id, 0);
		return *this;
	}

	GLuint getID() const {
		return id;
	}

	// Shader cannot be copied
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader& other) = delete;
private:
	GLuint id;
};

ShaderProgram::ShaderProgram() noexcept : id{}, transformUniform{}, colorUniform{} {}

ShaderProgram::ShaderProgram(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath) {
	Shader vertexShader(GL_VERTEX_SHADER, vertexShaderPath);
	Shader fragmentShader(GL_FRAGMENT_SHADER, fragmentShaderPath);
	//const auto fragmentShaderSource = ReadFile(vertexShaderPath);

	id = glCreateProgram();
	glAttachShader(id, vertexShader.getID());
	glAttachShader(id, fragmentShader.getID());
	glLinkProgram(id);

	int success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(id, 512, nullptr, infoLog);
		throw std::runtime_error(std::format("Could not link shader: {}", infoLog));
	}

	transformUniform = glGetUniformLocation(id, "transformUniform");
	colorUniform = glGetUniformLocation(id, "colorUniform");
}

ShaderProgram::ShaderProgram(ShaderProgram&& from) noexcept {
	id = std::exchange(from.id, 0);
	transformUniform = from.transformUniform;
	colorUniform = from.colorUniform;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& from) noexcept {
	id = std::exchange(from.id, 0);
	transformUniform = from.transformUniform;
	colorUniform = from.colorUniform;
	return *this;
}

ShaderProgram::~ShaderProgram() noexcept {
	glDeleteProgram(id);
}

void ShaderProgram::Run(const glm::mat3 transform, const glm::vec4 color) {
	glUseProgram(id);
	glUniformMatrix3fv(transformUniform, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4f(colorUniform, color.r, color.g, color.b, color.a);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
}