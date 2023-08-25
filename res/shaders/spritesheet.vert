#version 450 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

layout(std140, binding = 0) uniform Matrices {
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

void main() {
	gl_Position = projection * view * model * vec4(aPosition, 0.0, 1.0);
	texCoord = aTexCoord;
}