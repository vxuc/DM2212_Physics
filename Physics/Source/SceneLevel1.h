#ifndef SCENE_LEVEL1_H
#define SCENE_LEVEL1_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneLevel1 : public SceneBase
{
	static const int MAX_SPEED = 15;
	static const int MAX_ENEMY_SPEED = 10;
	static const int MAX_ASTEROID_SPEED = 8;
	static const int BULLET_SPEED = 50;
	static const int MISSILE_SPEED = 20;
	static const int MISSILE_POWER = 1;

public:
	SceneLevel1();
	~SceneLevel1();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	GameObject* FetchGO();

	void RestartGame();
	void SplitAsteroid(GameObject*);

	void UpdatePowerUp(double);
	void UpdateKeyInputs(double);
	void UpdateMouseInputs(double);
	void UpdateCamera(double);
	void SpawnPowerUp(GameObject* go, float rng);

protected:

	//Physics
	std::vector<GameObject *> m_goList;
	float m_speed;
	float m_slowmo;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ship;
	GameObject *m_laser;
	GameObject* m_enemy;
	GameObject* m_ally;
	Vector3 m_force;
	Vector3 m_torque;
	Vector3 m_enemyToShipDir;

	//Game
	int m_objectCount;
	int m_lives;
	int m_score;
	int bulletCount;
	int maxBullet;
	float m_nitro;

	int enemiesAlive;
	int stationCount;
	int activeGOs;

	//Time 
	double asteroidSpawnTime;
	double bulletSpawnTime;
	double levelTime;
	double enemySpawnTime;
	double nitroTime;
	double enemiesKilledTime;
	double cooldownMovement;

	//power up
	double powerUpSpawnTime;
	double slowmoPowerUpTime;
	double increaseFireRatePowerUpTime;
	float bulletFireRate;
	float mana;

private:
	bool LevelOne(double dt);
	bool LevelOneBonus(double dt);
	void UpdateMenu();
};

#endif