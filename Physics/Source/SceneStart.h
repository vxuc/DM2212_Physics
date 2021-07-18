#ifndef SCENE_START_H
#define SCENE_START_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneStart : public SceneBase
{
	static const int MAX_SPEED = 10;
	static const int MAX_ENEMY_SPEED = 10;
	static const int BULLET_SPEED = 50;
	static const int MISSILE_SPEED = 20;
	static const int MISSILE_POWER = 1;

public:
	SceneStart();
	~SceneStart();

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
	GameObject* m_bg;
	GameObject *m_laser;
	GameObject* m_enemy;
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

	//Time 
	double asteroidSpawnTime;
	double bulletSpawnTime;
	double levelTime;
	double enemySpawnTime;
	double nitroTime;
	double enemiesKilledTime;

	//power up
	double powerUpSpawnTime;
	double slowmoPowerUpTime;
	double increaseFireRatePowerUpTime;
	float bulletFireRate;
	float mana;

private:
	void RenderMenu();
	void RenderCustomisationMenu();

	int selectorIndex = 0;
	int menuIndex = 0;
	int colourIndex = 0;
};

#endif