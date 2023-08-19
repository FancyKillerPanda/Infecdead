#pragma once

#include <glm/glm.hpp>

#include <SDL.h>

#include "common.hpp"

inline u32 min(u32 a, u32 b) {
	return a < b ? a : b;
}

inline u32 max(u32 a, u32 b) {
	return a > b ? a : b;
}

inline SDL_Rect to_rect(glm::vec4 box) {
	return SDL_Rect {
		(s32) box.x,
		(s32) box.y,
		(s32) box.z,
		(s32) box.w,
	};
}

inline SDL_Rect to_rect(glm::vec2 position, glm::vec2 dimensions) {
	return SDL_Rect {
		(s32) position.x,
		(s32) position.y,
		(s32) dimensions.x,
		(s32) dimensions.y,
	};
}

inline glm::vec4 colour_to_vec4(SDL_Color colour) {
	return glm::vec4 { colour.r, colour.g, colour.b, colour.a };
}

inline SDL_Color vec4_to_colour(glm::vec4 colour) {
	return SDL_Color { (u8) colour.r, (u8) colour.g, (u8) colour.b, (u8) colour.a };
}

inline bool operator==(SDL_Color first, SDL_Color second) {
	return first.r == second.r && first.g == second.g && first.b == second.b && first.a == second.a;
}

inline bool operator!=(SDL_Color first, SDL_Color second) {
	return !(first == second);
}
