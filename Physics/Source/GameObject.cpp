
#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue)
	: type(typeValue),
	scale(1, 1, 1),
	active(false),
	mass(1.f),
	health(1),
	maxHealth(1),
	dir(1, 0, 0),
	isSplitable(false),
	momentOfInertia(1.f),
	angularVelocity(0.f),
	timer(0.0f),
	isHit(false),
	isLeft(false),
	isSucked(false),
	allowCollision(true)
{
}

GameObject::~GameObject()
{
}