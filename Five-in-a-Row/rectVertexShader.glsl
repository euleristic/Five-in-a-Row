#version 330 core

layout (location = 0) in vec2 vertexAttribute;

uniform mat3 transformUniform;

void main() {
	gl_Position = vec4(transformUniform * vec3(vertexAttribute, 1.0), 1.0);
}