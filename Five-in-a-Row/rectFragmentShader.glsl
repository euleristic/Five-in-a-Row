#version 330 core

uniform vec4 colorUniform;
out vec4 fragColor;

void main() {
	fragColor = colorUniform;
}