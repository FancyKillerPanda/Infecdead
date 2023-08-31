#version 450 core

out vec4 colour;

in vec2 texCoord;

uniform sampler2D texSampler;

void main() {
	colour = texture(texSampler, texCoord);
}
