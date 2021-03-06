package main;

import "core:fmt"
import "core:math"
import "core:math/rand"
import "core:strings"

import sdl "vendor:sdl2"

PLAYER_HEALTH_BAR_WIDTH :: 400;
PLAYER_HEALTH_BAR_HEIGHT :: 40;

Character :: struct {
	game: ^Game,
	type: enum {
		Player,
		Zombie,
		Hostage,
	},

	worldPosition: Vector2,
	dimensions: Vector2,
	rotation: f64,

	velocity: Vector2,
	acceleration: Vector2,

	currentSpritesheet: ^Spritesheet,
	walkSpritesheet: ^Spritesheet,

	currentAnimationFrame: u32,
	timeSinceLastFrameChange: f64,

	health: f64,
}

Player :: struct {
	using character: Character,
	walkWithPistolSpritesheet: ^Spritesheet,

	inventorySlots: [4] InventoryItem,
	currentlySelectedInventorySlot: u32,

	activeBullets: [dynamic] Bullet,
	timeSinceLastShot: f64,
}

InventoryItem :: struct {
	type: enum {
		Empty,
		Pistol,
		MedKit,
	},

	data: union {
		PistolData,
	},

	currentText: Text,
}

PistolData :: struct {
	bulletsLeft: u32,
	maxBullets: u32,
}

Bullet :: struct {
	worldPosition: Vector2,
	velocity: Vector2,
	lifeTime: f64,
	damage: f64,

	spritesheet: ^Spritesheet,
}

create_player :: proc(game: ^Game) -> (player: Player) {
	init_character(game, &player, 0);
	player.type = .Player;
	
	player.walkSpritesheet = new(Spritesheet);
	init_spritesheet(player.walkSpritesheet, game.renderer, PLAYER_PNG_DATA, get_game_data(game).characterDimensions, { 16, 16 }, 32, 4, nil, 0);
	player.walkWithPistolSpritesheet = new(Spritesheet);
	init_spritesheet(player.walkWithPistolSpritesheet, game.renderer, PLAYER_WITH_PISTOL_PNG_DATA, get_game_data(game).characterDimensions, { 16, 16 }, 32, 4, nil, 0);
	player.currentSpritesheet = player.walkSpritesheet;

	return;
}

init_character :: proc(game: ^Game, character: ^Character, position: Vector2) {
	character.game = game;

	character.dimensions = get_game_data(game).characterDimensions / game.currentOutputTileSize;
	character.worldPosition = position;
	character.health = 1.0;

	// character.rotation = 0; // TODO(fkp): Pass in

	character.velocity = 0;
	character.acceleration = 0;
}

handle_player_events :: proc(using player: ^Player, event: ^sdl.Event) {
	#partial switch event.type {
		case .KEYDOWN:
			#partial switch event.key.keysym.scancode {
				case .NUM1: fallthrough;
				case .NUM2: fallthrough;
				case .NUM3: fallthrough;
				case .NUM4:
					currentlySelectedInventorySlot = u32(event.key.keysym.scancode - sdl.Scancode.NUM1);
					swap_current_slot_with_chest(player);

				case .E:
					use_item(player);
			}

		case .MOUSEWHEEL:
			if event.wheel.y > 0 {
				currentlySelectedInventorySlot += len(inventorySlots);
				currentlySelectedInventorySlot -= 1;
			} else if event.wheel.y < 0 {
				currentlySelectedInventorySlot += 1;
			}

			currentlySelectedInventorySlot %= len(inventorySlots);

		case .MOUSEBUTTONDOWN:
			if event.button.button == sdl.BUTTON_LEFT {
				shoot(player);
			}
	}
}

update_player :: proc(using player: ^Player, deltaTime: f64) {
	rotationRadians := math.to_radians_f64(rotation);
	rotationVector := vec2_normalise({ math.cos_f64(rotationRadians), -math.sin_f64(rotationRadians) });
	
	// Movement
	acceleration = { 0, 0 };
	if game.keysPressed[sdl.Scancode.W] {
		acceleration = rotationVector * get_game_data(game).playerWalkAcceleration;
	}
	if game.keysPressed[sdl.Scancode.S] {
		acceleration = rotationVector * -get_game_data(game).playerWalkAcceleration * 0.5;
	}
	if game.keysPressed[sdl.Scancode.A] {
		rotation += get_game_data(game).playerRotationSpeed;
	}
	if game.keysPressed[sdl.Scancode.D] {
		rotation += 360.0;
		rotation -= get_game_data(game).playerRotationSpeed;
	}
	
	rotation = math.mod_f64(rotation, 360.0);
	velocity += acceleration * deltaTime;
	velocity *= get_game_data(game).playerFriction;

	if abs(velocity.x) < 0.15 do velocity.x = 0;
	if abs(velocity.y) < 0.15 do velocity.y = 0;
	
	// Updates position and does collision checking
	update_character_position(player, deltaTime);

	// Shooting
	timeSinceLastShot += deltaTime;

	bulletLoop: for bulletIndex := 0; bulletIndex < len(activeBullets); {
		bullet := &activeBullets[bulletIndex];
		bullet.worldPosition += bullet.velocity * deltaTime;

		bullet.lifeTime += deltaTime;
		if bullet.lifeTime >= get_game_data(game).pistolShotLifetime ||
		   bullet.worldPosition.x < 0 || bullet.worldPosition.y < 0 ||
		   bullet.worldPosition.x > game.currentWorldDimensions.x || bullet.worldPosition.y > game.currentWorldDimensions.y {
			destroy_bullet(player, bulletIndex);
			continue;
		}

		bulletRect := create_sdl_rect((bullet.worldPosition * game.currentOutputTileSize) - (bullet.spritesheet.outputSize / 2), bullet.spritesheet.outputSize);

		// TODO(fkp): Friendly fire
		for zombie, zombieIndex in &game.zombies {
			zombieRect := create_sdl_rect((zombie.worldPosition - (zombie.dimensions / 2)) * game.currentOutputTileSize, zombie.dimensions * game.currentOutputTileSize);
			
			if sdl.HasIntersection(&bulletRect, &zombieRect) {
				zombie.health -= bullet.damage;
				if zombie.health <= 0.0 {
					destory_zombie(&zombie, zombieIndex);
				} else {
					zombie.worldPosition += vec2_normalise(bullet.velocity) * get_game_data(game).pistolKnockback;
				}

				destroy_bullet(player, bulletIndex);
				continue bulletLoop;
			}
		}

		bulletIndex += 1;
	}
	
	// Texturing
	if inventorySlots[currentlySelectedInventorySlot].type == .Empty {
		currentSpritesheet = walkSpritesheet;
	} else if inventorySlots[currentlySelectedInventorySlot].type == .Pistol {
		currentSpritesheet = walkWithPistolSpritesheet;
	}
	
	update_character_texture(player, deltaTime);
}

update_character_position :: proc(using character: ^Character, deltaTime: f64) {
	worldPosition.x += velocity.x * deltaTime;
	worldPositionRect := get_character_world_rect_multiplied(character, game.currentOutputTileSize);

	for object in &game.currentTilemap.objects {
		objectRect := multiply_sdl_rect(object, game.currentOutputTileSize);
		if sdl.HasIntersection(&worldPositionRect, &objectRect) {
			worldPosition.x -= velocity.x * deltaTime;
			velocity.x = 0;
			break;
		}
	}

	worldPosition.y += velocity.y * deltaTime;
	worldPositionRect = get_character_world_rect_multiplied(character, game.currentOutputTileSize);

	for object in &game.currentTilemap.objects {
		objectRect := multiply_sdl_rect(object, game.currentOutputTileSize);
		if sdl.HasIntersection(&worldPositionRect, &objectRect) {
			worldPosition.y -= velocity.y * deltaTime;
			velocity.y = 0;
			break;
		}
	}

	worldPosition.x = clamp(worldPosition.x, dimensions.x / 2.0, game.currentTilemap.numberOfTiles.x - (dimensions.x / 2.0));
	worldPosition.y = clamp(worldPosition.y, dimensions.y / 2.0, game.currentTilemap.numberOfTiles.y - (dimensions.y / 2.0));
}

update_character_texture :: proc(using character: ^Character, deltaTime: f64) {
	timeSinceLastFrameChange += deltaTime;
	if timeSinceLastFrameChange >= 0.15 {
		timeSinceLastFrameChange = 0;

		currentAnimationFrame += 1;
		currentAnimationFrame %= currentSpritesheet.subrectsPerRow;
	}

	if velocity == { 0, 0 } {
		currentAnimationFrame = 0;
	}

	row := u32(math.mod_f64(rotation + 22.5, 360.0) / 45.0);
	spritesheet_set_frame(currentSpritesheet, (row * currentSpritesheet.subrectsPerRow) + currentAnimationFrame);
}

draw_player :: proc(using player: ^Player, viewOffset: Vector2) {
	draw_spritesheet(player.currentSpritesheet, game.currentTilemapOutputPosition + ((player.worldPosition - viewOffset) * game.currentOutputTileSize), get_game_data(game).characterDimensions);

	for bullet in activeBullets {
		draw_spritesheet(bullet.spritesheet, game.currentTilemapOutputPosition + ((bullet.worldPosition - viewOffset) * game.currentOutputTileSize));
	}
}

draw_player_on_minimap :: proc(using player: ^Player, minimapPosition: Vector2) {
	minimapPlayerRect: sdl.Rect = {
		// -1 to centre the rect
		i32(minimapPosition.x + (player.worldPosition.x * MINIMAP_TILE_SIZE.x) - 1),
		i32(minimapPosition.y + (player.worldPosition.y * MINIMAP_TILE_SIZE.y) - 1),
		i32(MINIMAP_TILE_SIZE.x * 3),
		i32(MINIMAP_TILE_SIZE.y * 3),
	};
	
	sdl.SetRenderDrawColor(game.renderer, 0, 0, 255, 255);
	sdl.RenderFillRect(game.renderer, &minimapPlayerRect);
}

healthBarFrame: f64;

draw_character_health_bar :: proc(using character: ^Character, viewOffset: Vector2) {
	get_health_colour :: proc(health: f64) -> sdl.Colour {
		if health > 0.75 {
			return { 0, 192, 0, 255 };
		} else if health > 0.5 {
			return { 0, 255, 0, 255 };
		} else if health > 0.25 {
			return { 255, 255, 0, 255 };
		} else {
			return { 255, 0, 0, 255 };
		}
	}
	
	if type == .Player {
		player := cast(^Player) character;

		// Draws the icon (we can't use draw_spritesheet() here because
		// we need to specify the size, not just the position.)
		iconRect := create_sdl_rect({ game.screenDimensions.x * 1 / 150, game.screenDimensions.y * 2 / 100 },
									{ PLAYER_HEALTH_BAR_HEIGHT, PLAYER_HEALTH_BAR_HEIGHT });
		sdl.RenderCopy(game.renderer, game.medKitIcon.texture, nil, &iconRect);

		// Draws the health bar itself
		fullHealthBarRect := create_sdl_rect({ game.screenDimensions.x * 4 / 100, game.screenDimensions.y * 2 / 100 },
											 { PLAYER_HEALTH_BAR_WIDTH, PLAYER_HEALTH_BAR_HEIGHT });
		healthBarRect := fullHealthBarRect;
		healthBarRect.w = i32(f64(healthBarRect.w) * health);
		
		colour := get_health_colour(health);
		sdl.SetRenderDrawColor(game.renderer, colour.r, colour.g, colour.b, colour.a);
		sdl.RenderFillRect(game.renderer, &healthBarRect);
		
		if player.inventorySlots[player.currentlySelectedInventorySlot].type == .MedKit {
			healthBarRect.x += healthBarRect.w;
			healthBarRect.w = i32(f64(fullHealthBarRect.w) * get_game_data(game).medKitHealthBoost);

			if healthBarRect.x + healthBarRect.w > fullHealthBarRect.x + fullHealthBarRect.w {
				healthBarRect.w = (fullHealthBarRect.x + fullHealthBarRect.w) - healthBarRect.x;
			}

			colour = get_health_colour(health + get_game_data(game).medKitHealthBoost);
			sdl.SetRenderDrawColor(game.renderer, colour.r, colour.g, colour.b, u8(((math.sin(healthBarFrame) + 1) / 2) * 127));
			sdl.RenderFillRect(game.renderer, &healthBarRect);

			healthBarFrame += 0.1;
		}
		
		sdl.SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
		sdl.RenderDrawRect(game.renderer, &fullHealthBarRect);
		
		// Draws a second pixel of outline
		fullHealthBarRect.x += 1;
		fullHealthBarRect.y += 1;
		fullHealthBarRect.w -= 2;
		fullHealthBarRect.h -= 2;
		sdl.RenderDrawRect(game.renderer, &fullHealthBarRect);
	} else {
		if health < 1.0 {
			fullHealthBarRect: sdl.Rect = {
				i32(game.currentTilemapOutputPosition.x + ((worldPosition.x - viewOffset.x - (dimensions.x / 2)) * game.currentOutputTileSize.x)),
				i32(game.currentTilemapOutputPosition.y + ((worldPosition.y - viewOffset.y - (dimensions.y / 2)) * game.currentOutputTileSize.y) - (ZOMBIE_HEALTH_BAR_HEIGHT * 2)),
				i32(dimensions.x * game.currentOutputTileSize.x),
				i32(ZOMBIE_HEALTH_BAR_HEIGHT),
			}

			healthBarRect: sdl.Rect = {
				fullHealthBarRect.x,
				fullHealthBarRect.y,
				i32(f64(fullHealthBarRect.w) * health),
				fullHealthBarRect.h,
			};

			sdl.SetRenderDrawColor(game.renderer, 0, 255, 0, 255);
			sdl.RenderFillRect(game.renderer, &healthBarRect);
			sdl.SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
			sdl.RenderDrawRect(game.renderer, &fullHealthBarRect);
		}
	}
}

shoot :: proc(using player: ^Player) {
	slot := &inventorySlots[currentlySelectedInventorySlot];
	if slot.type == .Pistol {
		if slot.data.(PistolData).bulletsLeft > 0 {
			if timeSinceLastShot >= get_game_data(game).pistolShotCooldown {
				timeSinceLastShot = 0;
				(&slot.data.(PistolData)).bulletsLeft -= 1;

				newText := fmt.tprintf("{}/{}", slot.data.(PistolData).bulletsLeft, slot.data.(PistolData).maxBullets);
				free_text(&slot.currentText);
				slot.currentText = create_text(game.renderer, game.menu.textFont, strings.clone_to_cstring(newText));

				append(&activeBullets, create_pistol_bullet(player));
			}
		} else {
			printf("Magazine is empty!\n");
		}
	}
}

take_damage :: proc(using player: ^Player, damage: f64) {
	health -= damage;

	if health <= 0 {
		health = 0;

		game.state = .GameOver;
		game.gameWon = false;

		return;
	}
}

use_item :: proc(using player: ^Player) {
	playerRect := get_character_world_rect_multiplied(player, game.currentOutputTileSize);
	for door in game.currentTilemap.doors {
		doorRect := multiply_sdl_rect(door, game.currentOutputTileSize);
		if sdl.HasIntersection(&doorRect, &playerRect) {
			// TODO(fkp): Do this based on information from the tilemap itself
			if game.currentTilemap == game.outsideTilemap {
				set_current_map(game, game.townHallTilemap);
			} else {
				set_current_map(game, game.outsideTilemap);
			}

			return;
		}
	}
	
	for chest in game.chests {
		if chest.isOpen {
			swap_current_slot_with_chest(player);
			return; // You can't use an item while looking at a chest
		}
	}
	
	switch inventorySlots[currentlySelectedInventorySlot].type {
		case .Empty:
		case .Pistol:
			// Do nothing

		case .MedKit:
			health += get_game_data(game).medKitHealthBoost;
			if health > 1.0 do health = 1.0;

			inventorySlots[currentlySelectedInventorySlot] = InventoryItem { type = .Empty };
	}
}

get_character_world_rect :: proc(using character: ^Character) -> sdl.Rect {
	return get_character_world_rect_multiplied(character, { 1, 1 });
}

get_character_world_rect_multiplied :: proc(using character: ^Character, factor: Vector2) -> sdl.Rect {
	return {
		i32((worldPosition.x - (dimensions.x / 2.0)) * factor.x),
		i32((worldPosition.y + (dimensions.y / 4.0)) * factor.y),
		i32(dimensions.x * factor.x),
		i32((dimensions.y / 2.0) * factor.y),
	};
}

swap_current_slot_with_chest :: proc(using player: ^Player) {
	for chest in &game.chests {
		if chest.isOpen {
			temp := inventorySlots[currentlySelectedInventorySlot];
			inventorySlots[currentlySelectedInventorySlot] = chest.contents;
			chest.contents = temp;
		}
	}
}

create_pistol_bullet :: proc(using player: ^Player) -> (bullet: Bullet) {
	rotationRadians := math.to_radians_f64(rotation);
	
	bullet.worldPosition = worldPosition;
	bullet.velocity.x = math.cos_f64(rotationRadians) * get_game_data(game).pistolShotVelocity;
	bullet.velocity.y = -math.sin_f64(rotationRadians) * get_game_data(game).pistolShotVelocity;

	bullet.spritesheet = new(Spritesheet);
	init_spritesheet(bullet.spritesheet, game.renderer, PISTOL_BULLET_PNG_DATA, { 0, 0 }, { 0, 0 }, 1, 1, nil, 0);

	bullet.damage = rand.float64_range(get_game_data(game).pistolMinDamage, get_game_data(game).pistolMaxDamage);
	
	return;
}

destroy_bullet :: proc(player: ^Player, index: int) {
	free(player.activeBullets[index].spritesheet);
	ordered_remove(&player.activeBullets, index);
}
