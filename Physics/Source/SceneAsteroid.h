#ifndef SCENE_ASTEROID_H
#define SCENE_ASTEROID_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneAsteroid : public SceneBase
{
	static const int MAX_SPEED = 10;
	static const int MAX_ENEMY_SPEED = 10;
	static const int BULLET_SPEED = 50;
	static const int MISSILE_SPEED = 20;
	static const int MISSILE_POWER = 1;

public:
	SceneAsteroid();
	~SceneAsteroid();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	GameObject* FetchGO();

	void RestartGame();
	void SplitAsteroid(GameObject*);

	bool WaveOne(double);
	bool WaveTwo(double);
	void UpdatePowerUp(double);
	void UpdateKeyInputs(double);
	void UpdateMouseInputs(double);
	void UpdateCamera(double);
	bool IsCollided(GameObject* go, GameObject* go2);

protected:

	enum KEY_TYPES 
	{
		W = 0,
		V,
		B,
		TOTAL_KEYS
	};

	enum WAVES
	{
		ONE = 0,
		TWO, 
		THREE, 
		TOTAL_WAVES
	};

	static bool isWaveCompleted[WAVES::TOTAL_WAVES];

	//Physics
	std::vector<GameObject *> m_goList;
	float m_speed;
	float m_slowmo;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ship;
	GameObject* m_bg;
	GameObject *m_laser;
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
	
	bool wave1Completed = false;
	bool wave2Completed = false;

	//Time 
	double asteroidSpawnTime;
	double bulletSpawnTime;
	double waveTime;
	double enemySpawnTime;
	double nitroTime;

	//power up
	double powerUpSpawnTime;
	double slowmoPowerUpTime;
	double increaseFireRatePowerUpTime;
	float bulletFireRate;
};

#endif