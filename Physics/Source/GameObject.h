#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_BALL,
		GO_SPHERE_ENERGY,
		GO_BLACKHOLE,
		GO_WALL,
		GO_FLIPPER_PILLAR,
		GO_WALL_ENERGY,
		GO_WALL_ONESIDED,
		GO_WALL_BALLDROP,
		GO_BIG_BALL_POWERUP,
		GO_SPAWN_BALLS_POWERUP,
		GO_SPRING,
		GO_FLIPPER,
		GO_PILLAR,
		GO_CUBE,
		GO_BACKGROUND,
		GO_ASTEROID,
		GO_ASTEROIDLIGHT,
		GO_SHIP,
		GO_BULLET,
		GO_ENEMY,
		GO_FIGHTER_ENEMY,
		GO_ENEMY_BULLET,
		GO_LASER,
		GO_MISSILE,
		GO_SLOWMO_POWERUP,
		GO_HEALTH_POWERUP,
		GO_SHIELD_POWERUP,
		GO_SHIELD,
		GO_INCREASE_FIRERATE_POWERUP,
		GO_HEALTHBAR,
		GO_ENEMYSPACESTATION,
		GO_ENEMY_BOSS,
		GO_ENEMY_TURRET,
		GO_MANA,
		GO_ALLY_SHIP,
		GO_TOTAL, //must be last
	};

	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	Vector3 dir;
	Vector3 normal;
	float momentOfInertia;
	float angularVelocity;
	bool active;
	bool isSplitable;
	bool isHit;
	bool isLeft;
	bool isSucked;
	bool allowCollision;
	float mass;
	float health;
	float maxHealth;
	float timer;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();
};

#endif