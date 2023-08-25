#version 330 core

out vec4 colour;

in vec2 texCoord;

const vec2 SPRITESHEET_CELL_DIMENSIONS = vec2(32.0, 32.0);
const vec2 SPRITESHEET_NUM_CELLS = vec2(4, 8);
const vec2 SPRITESHEET_DIMENSIONS = SPRITESHEET_CELL_DIMENSIONS * SPRITESHEET_NUM_CELLS;

uniform int spritesheetColumn;
uniform sampler2D texSampler;

void main() {
	// TODO(fkp): Figure out based on rotation.
	int spritesheetRow = 0;
	
	vec2 spritesheetCellPixels = vec2(spritesheetColumn, spritesheetRow) * SPRITESHEET_CELL_DIMENSIONS;
	vec2 spritesheetCell = spritesheetCellPixels / SPRITESHEET_DIMENSIONS;
	vec2 spritesheetTexCoord = spritesheetCell + (texCoord / SPRITESHEET_NUM_CELLS);

	colour = texture(texSampler, spritesheetTexCoord);
}
