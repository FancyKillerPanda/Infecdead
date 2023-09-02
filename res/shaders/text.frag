#version 450 core

out vec4 colour;

in vec2 texCoord;

uniform sampler2D texSampler;

void main() {
	// TODO(fkp): From uniform values.
	const float boldness = 0.5; // Inverse boldness.
	const float smoothing = 0.1; // TODO(fkp): Recommended 0.1 / scale.
	const vec4 textColour = vec4(1.0, 1.0, 0.0, 1.0);

	float dist = texture(texSampler, texCoord).a;
	float alpha = smoothstep(boldness - smoothing, boldness + smoothing, dist);
	colour = vec4(textColour.rgb, textColour.a * alpha);
}
