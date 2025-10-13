#pragma once
#include "Engine/Audio/AudioSystem.hpp"

struct Vec2;
struct Rgba8;

enum GameState
{
	GAME_STATE_INVALID = -1,
	GAME_STATE_ATTRACT,
	GAME_STATE_PLAY
};

constexpr char FUNCTION_KEY_1 = 'p';
constexpr char FUNCTION_KEY_8 = 'w';
constexpr int NUM_STARTING_ASTEROIDS = 6;
constexpr int MAX_ASTEROIDS = 100;
constexpr int MAX_BULLETS = 200;
constexpr int MAX_BEETLES = 60;
constexpr int MAX_WASPS = 60;
constexpr int MAX_DEBRIS = 600;
constexpr int MAX_EXPLOSIONS = MAX_ASTEROIDS;
constexpr int MAX_STARS = 2000;
constexpr int NUM_WAVES = 5;
constexpr float WORLD_SIZE_X = 400.f;
constexpr float WORLD_SIZE_Y = 200.f;
constexpr float WORLD_VIEW_SIZE_X = 200.f;
constexpr float WORLD_VIEW_SIZE_Y = 100.f;
constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;
constexpr float SCREEN_CENTER_X = SCREEN_SIZE_X / 2.f;
constexpr float SCREEN_CENTER_Y = SCREEN_SIZE_Y / 2.f;
constexpr float ASTEROID_SPEED = 30.f;
constexpr float ASTEROID_PHYSICS_RADIUS = 3.5f;
constexpr float ASTEROID_COSMETIC_RADIUS = 4.0f;
constexpr float BULLET_LIFETIME_SECONDS = 2.0f;
constexpr float BULLET_SPEED = 50.f;
constexpr float BULLET_PHYSICS_RADIUS = 0.5f;
constexpr float BULLET_COSMETIC_RADIUS = 2.0f;
constexpr float PLAYER_SHIP_ACCELERATION = 45.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS = 1.75f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS = 2.25f;
constexpr float BEETLE_SPEED = 30.f;
constexpr float BEETLE_PHYSICS_RADIUS = 1.75f;
constexpr float BEETLE_COSMETIC_RADIUS = 2.25f;
constexpr float WASP_ACCELERATION = 15.f;
constexpr float WASP_PHYSICS_RADIUS = 1.75f;
constexpr float WASP_COSMETIC_RADIUS = 2.25f;
constexpr float DEBRIS_PHYSICS_RADIUS = 0.5f;
constexpr float DEBRIS_COSMETIC_RADIUS = 2.f;
constexpr float EXPLOSION_PHYSICS_RADIUS = 10.f;
constexpr float EXPLOSION_COSMETIC_RADIUS = 10.f;
constexpr float DEBUG_LINE_THICKNESS = 0.2f;

extern SoundID audio_music;
extern SoundID audio_selectSound;
extern SoundID audio_fireShoot;
extern SoundID audio_fireExplosion;
extern SoundID audio_iceShoot;
extern SoundID audio_iceExplosion;
extern SoundID audio_hurt;
extern SoundID audio_shipExplosion;
extern SoundID audio_enemyExplosion;
extern SoundID audio_waveStart;
extern SoundID audio_victory;
extern SoundID audio_gameOver;

void DebugDrawRing( Vec2 const& center, float radius, float thickness, Rgba8 const& color);
void DebugDrawLine( Vec2 const& startPos, Vec2 const& endPos, float thickness, Rgba8 const& color);
void CreateSounds();
void DrawGlow( Vec2 pos, Rgba8 color, float alpha, float radius );