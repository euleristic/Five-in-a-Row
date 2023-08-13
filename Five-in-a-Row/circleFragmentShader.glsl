#version 330 core

in vec2 interpolatedCoordinate;

out vec4 fragColor;

uniform vec4 colorUniform;

void main() {
	fragColor = colorUniform * float(dot(interpolatedCoordinate, interpolatedCoordinate) <= 1.0);
}