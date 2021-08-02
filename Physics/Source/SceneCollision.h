#ifndef SCENE_COLLISION_H
#define SCENE_COLLISION_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneCollision : public SceneBase
{
public:
	SceneCollision();
	~SceneCollision();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	GameObject* FetchGO();
	void ReturnGO(GameObject *go);

	bool CheckCollision(GameObject* go, GameObject* go2, float dt);

	void CollisionResponse(GameObject* go, GameObject* go2);

	void CreateThickWall(Vector3 scale, Vector3 pos);

	void RenderMap();

	void SpawnPowerup(double dt);
protected:

	//Physics
	std::vector<GameObject *> m_goList;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ghost;
	int m_objectCount;

	//Auditing
	float m1, m2;
	Vector3 u1, u2, v1, v2;
	Vector3 m_gravity;

	//Flipper
	GameObject* m_flipperLeft;
	GameObject* m_flipperRight;
	GameObject* m_flipperLeftPillar;
	GameObject* m_flipperRightPillar;
	GameObject* m_spring;
	double leftFlipperTimer;
	double rightFlipperTimer;
	float rotationLeft = 0.f;
	float rotationRight = 0.f;
	float periodT = 1.f;
	bool leftFlipperOn;
	bool rightFlipperOn;

	//POWERUP
	float powerUpSpawnTime = 0.f;

	//GAME
	bool activateBallDrop = false;
	float ballDropTimer = 0.f;

	float score = 0.f;

private:
	const float FLIPPER_UP_SPEED = 1000.f;
	const float FLIPPER_DOWN_SPEED = 500.f;
	const float FLIPPER_MIN_ROTATION = 75.f;
	const float FLIPPER_MAX_ROTATION = 105.f;
	const float FLIPPER_COOLDOWN_TIMER = 0.1f;
	const float BALL_MAX_SPEED = 200.f;
};

#endif