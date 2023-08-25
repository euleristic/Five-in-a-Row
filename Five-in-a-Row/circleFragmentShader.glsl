#version 330 core

in vec2 fragmentCoordinate;

out vec4 fragColor;

uniform vec4 colorUniform;

void main() {
	// Branchless, yay! Set the color to colorUniform if within the unit radius;
	// else, set it transparent
	fragColor = colorUniform * float(dot(fragmentCoordinate, fragmentCoordinate) <= 1.0);
}