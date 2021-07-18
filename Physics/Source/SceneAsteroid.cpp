#include "SceneAsteroid.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>
bool SceneAsteroid::isWaveCompleted[TOTAL_WAVES];

SceneAsteroid::SceneAsteroid()
{
}

SceneAsteroid::~SceneAsteroid()
{
}

void SceneAsteroid::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	m_worldHeight = 200.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / (float)Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;
	m_slowmo = 1.f;
	
	Math::InitRNG();

	//Exercise 2a: Construct 100 GameObject with type GO_ASTEROID and add into m_goList
	for (int i = 0; i < 100; i++)
	{
		GameObject* newGO = new GameObject(GameObject::GO_ASTEROID);
		m_goList.push_back(newGO);
	}

	//Exercise 2b: Initialize m_lives and m_score
	m_lives = 3;
	m_score = 0;
	bulletCount = 0;
	maxBullet = 50;
	asteroidSpawnTime = 0;
	bulletSpawnTime = 0;
	waveTime = 0;
	enemySpawnTime = 0;
	powerUpSpawnTime = 0;
	slowmoPowerUpTime = 0;
	increaseFireRatePowerUpTime = 0;
	m_nitro = 1.f;
	nitroTime = 0;
	bulletFireRate = 0.3f;

	//Exercise 2c: Construct m_ship, set active, type, scale and pos
	m_ship = new GameObject(GameObject::GO_SHIP);
	m_ship->active = true;
	m_ship->scale = Vector3(2, 2, 2);
	m_ship->pos = Vector3(50, 50, 0);
	m_ship->momentOfInertia = m_ship->mass;
	m_ship->angularVelocity = 0.f;
	m_ship->health = 100;

	m_laser = new GameObject(GameObject::GO_LASER);
	m_laser->active = false;
	m_laser->scale = Vector3(30, 2, 2);
	m_laser->pos = m_ship->pos;
	m_laser->vel = Vector3(0, 0, 0);

	m_bg = new GameObject(GameObject::GO_BACKGROUND);
	m_bg->scale = Vector3(1.92, 1.08, 1);
	m_bg->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight * 0.5f, -1);
	m_bg->vel = Vector3(0, 0, 0);

	for (int i = 0; i < TOTAL_WAVES; i++)
	{
		isWaveCompleted[i] = false;
	}

	GameObject* enemyStation = new GameObject(GameObject::GO_ENEMYSPACESTATION);
	enemyStation->isHit = true;
	enemyStation->scale = Vector3(15, 15, 5);
	enemyStation->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight - enemyStation->scale.y * 0.5f, 1);
	enemyStation->health = 10;
	enemyStation->active = true;
	m_goList.push_back(enemyStation);
	SceneBase::missionType = DESTROY_ENEMYSTATION_LEVEL1;
}

GameObject* SceneAsteroid::FetchGO()
{
	//Exercise 3a: Fetch a game object from m_goList and return it
	for (GameObject* go : m_goList)
	{
		if (!go->active)
		{
			go->active = true;
			go->isHit = false;
			go->health = 1;
			return go;
		}
	}
	for (int i = 0; i < 10; i++)
	{
		GameObject* newGO = new GameObject(GameObject::GO_ASTEROID);
		m_goList.push_back(newGO);
	}
	return FetchGO();
}

void SceneAsteroid::RestartGame()
{
	for (GameObject* go : m_goList)
	{
		if (go->active)
			go->active = false;
	}
	m_lives = 3;
	m_ship->health = 100;
	m_score = 0;
	bulletCount = 0;
	wave1Completed = false;
	wave2Completed = false;
	m_speed = 1;
	m_ship->pos = Vector3(50, 50, 0);
	m_ship->vel = Vector3(0, 0, 0);
	m_ship->dir = Vector3(1, 0, 0);
}

void SceneAsteroid::SplitAsteroid(GameObject* go)
{
	int numOfSplitAsteroids = Math::RandIntMinMax(2,4);
	std::cout << "SPLIT: " << numOfSplitAsteroids << std::endl;
	for (int i = 0; i < numOfSplitAsteroids; i++)
	{
		GameObject* splitAsteroid = FetchGO();
		splitAsteroid->type = GameObject::GO_ASTEROIDLIGHT;
		splitAsteroid->health = 1;
		splitAsteroid->isSplitable = false;
		splitAsteroid->scale = Vector3(go->scale.x * 0.4f, go->scale.y * 0.4f, go->scale.z * 0.4f);
		splitAsteroid->pos = go->pos + Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f), 0);
		splitAsteroid->isHit = true;
		if (i == numOfSplitAsteroids - 1)
		{
			splitAsteroid->vel = go->vel;
		}
		else
		{
			splitAsteroid->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f), 0);
		}
	}
}

bool SceneAsteroid::WaveOne(double dt)
{
	waveTime += dt * m_speed;
	enemySpawnTime += dt * m_speed;

	if (enemySpawnTime >= 3 && SceneBase::missionType == DESTROY_ENEMYSTATION_LEVEL1)
	{
		GameObject* newEnemy = FetchGO();
		newEnemy->type = GameObject::GO_ENEMY;
		newEnemy->health = 2;
		newEnemy->scale = Vector3(2, 2, 2);
		newEnemy->pos = Vector3(Math::RandFloatMinMax(m_worldWidth * 0.5, m_worldWidth * 0.5 + 5), Math::RandFloatMinMax(m_worldHeight - 30, m_worldHeight - 40), 0);
		newEnemy->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f), 0);
		enemySpawnTime = 0;
	}
	if (waveTime >= 120)
	{
		waveTime = 0;
		enemySpawnTime = 0;
		std::cout << "wave 1 ended" << std::endl;
		return true;
	}
	return false;
}

bool SceneAsteroid::WaveTwo(double dt)
{
	waveTime += dt * m_speed;
	enemySpawnTime += dt * m_speed;
	if (enemySpawnTime >= 3)
	{
		int random = Math::RandIntMinMax(1, 2);
		GameObject* newEnemy = FetchGO();
		switch (random)
		{
		case 1:
			newEnemy->type = GameObject::GO_ENEMY;
			newEnemy->health = 2;
			newEnemy->scale = Vector3(2, 2, 2);
			newEnemy->pos = Vector3(Math::RandFloatMinMax(m_worldWidth * 0.5, m_worldWidth * 0.5 + 5), Math::RandFloatMinMax(28, 30), 0);
			newEnemy->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f), 0);
			break;
		case 2:
			newEnemy->type = GameObject::GO_FIGHTER_ENEMY;
			newEnemy->health = 2;
			newEnemy->scale = Vector3(2, 2, 2);
			newEnemy->pos = Vector3(Math::RandFloatMinMax(m_worldWidth * 0.5, m_worldWidth * 0.5 + 5), Math::RandFloatMinMax(28, 30), 0);
			newEnemy->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f), 0);
			break;
		default:
			break;
		}
		enemySpawnTime = 0;
	}

	if (waveTime >= 120)
	{
		std::cout << "wave 2 ended" << std::endl;
		return true;
	}
	return false;
}

void SceneAsteroid::UpdatePowerUp(double dt)
{
	bulletSpawnTime += dt * m_speed;
	powerUpSpawnTime += dt * m_speed;
	slowmoPowerUpTime += dt * m_speed;
	increaseFireRatePowerUpTime += dt * m_speed;

	if (slowmoPowerUpTime > 5)
		m_slowmo = 1;

	if (increaseFireRatePowerUpTime > 5)
		bulletFireRate = .3f;

	if (powerUpSpawnTime > 5)
	{
		int random = Math::RandIntMinMax(1, 3);
		GameObject* newPowerUp = FetchGO();
		newPowerUp->scale = Vector3(3, 3, 3);
		newPowerUp->pos = Vector3(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0.f, m_worldHeight), 0);
		newPowerUp->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f), 0);
		switch (random)
		{
		case 1:
			newPowerUp->type = GameObject::GO_SLOWMO_POWERUP;
			break;
		case 2:
			newPowerUp->type = GameObject::GO_HEALTH_POWERUP;
			break;
		case 3:
			newPowerUp->type = GameObject::GO_INCREASE_FIRERATE_POWERUP;
			break;
		}
		powerUpSpawnTime = 0;
	}
}

void SceneAsteroid::UpdateKeyInputs(double dt)
{
	static bool keyDown[TOTAL_KEYS];
	for (int i = 0; i < TOTAL_KEYS; i++)
		keyDown[i] = false;

	if (Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if (Application::IsKeyPressed('0'))
	{
		m_speed += 0.1f;
	}

	m_force.SetZero();
	m_torque.SetZero();

	//Exercise 6: set m_force values based on WASD
	if (Application::IsKeyPressed('W'))
	{
		m_force += 100 * m_ship->dir * m_nitro;
		m_torque = Vector3(0, -1, 0).Cross(Vector3(0, 100, 0));
	}
	if (Application::IsKeyPressed('S'))
	{
		m_force += -100 * m_ship->dir * m_nitro;
		m_torque = Vector3(0, 1, 0).Cross(Vector3(0, -100, 0));
	}
	if (Application::IsKeyPressed('A'))
	{
		m_force += 50 * m_ship->dir * m_nitro;
		m_torque = Vector3(1, -1, 0).Cross(Vector3(0, 50, 0));
	}
	if (Application::IsKeyPressed('D'))
	{
		m_force += -50 * m_ship->dir * m_nitro;
		m_torque = Vector3(-1, -1, 0).Cross(Vector3(0, 50, 0));
	}

	//Exercise 8: use 2 keys to increase and decrease mass of ship
	if (Application::IsKeyPressed('Q'))
	{
		m_ship->mass = Math::Max(0.1f, m_ship->mass - 10.f * float(dt));
	}
	else if (Application::IsKeyPressed('E'))
	{
		m_ship->mass = Math::Min(m_ship->mass + 10.f * float(dt), 10.f);
	}
	//Exercise 11: use a key to spawn some asteroids
	if (Application::IsKeyPressed('P'))
	{
		GameObject* newAsteroid = FetchGO();
		newAsteroid->type = GameObject::GO_ASTEROID;
		newAsteroid->health = 2;
		newAsteroid->isSplitable = true;
		newAsteroid->scale = Vector3(3, 3, 3);
		newAsteroid->pos = Vector3(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0.f, m_worldHeight), 0);
		newAsteroid->vel = Vector3(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
	}
	if (Application::IsKeyPressed('C'))
		m_laser->active = true;
	else
		m_laser->active = false;

	if (Application::IsKeyPressed('V') && !keyDown[V])
	{
		keyDown[V] = true;
		GameObject* newMissile = FetchGO();
		newMissile->type = GameObject::GO_MISSILE;
		newMissile->scale = Vector3(1, 1, 1);
		newMissile->pos = m_ship->pos;
		newMissile->vel = Vector3(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
	}

	if (Application::IsKeyPressed('B') && !keyDown[B])
	{
		keyDown[B] = true;
		GameObject* newFighter = FetchGO();
		newFighter->type = GameObject::GO_FIGHTER_ENEMY;
		newFighter->scale = Vector3(2, 2, 2);
		newFighter->pos = Vector3(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0.f, m_worldHeight), 0);
		newFighter->vel = Vector3(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
	}

	m_laser->pos = m_ship->pos + m_laser->scale.x * m_ship->dir * m_speed;
	m_laser->dir = m_ship->dir * m_speed * dt;

	//Exercise 14: use a key to spawn a bullet  
	//Exercise 15: limit the spawn rate of bullets
	if ((Application::IsKeyPressed(VK_SPACE) || (Application::IsMousePressed(1))) && bulletCount < maxBullet && bulletSpawnTime > bulletFireRate)
	{
		GameObject* bullet = FetchGO();
		bullet->type = GameObject::GO_BULLET;
		bullet->scale = Vector3(1, .5, 1);
		bullet->dir = m_ship->dir;
		try
		{
			bullet->pos = m_ship->pos + m_ship->dir.Normalized() * m_ship->scale.Length();
			bullet->vel = m_ship->dir.Normalized() * BULLET_SPEED;
		}
		catch (DivideByZero ex)
		{
			bullet->pos = m_ship->pos + Vector3(1, 0, 0) * m_ship->scale.Length();
			bullet->vel = Vector3(1, 0, 0) * BULLET_SPEED;
		}
		bulletCount++;
		bulletSpawnTime = 0;
	}

	bool nitro = false;

	if (Application::IsKeyPressed(VK_LSHIFT))
	{
		if (nitroTime > 0)
		{
			nitro = true;
			nitroTime -= dt * m_speed;
		}
	}
	else
		nitroTime += dt * m_speed;

	if (nitro)
		m_nitro += 0.25f;
	else
		m_nitro -= 0.1f;

	m_nitro = Math::Clamp(m_nitro, 1.f, 10.f);
	nitroTime = Math::Clamp(nitroTime, 0.0, 2.0);
}

void SceneAsteroid::UpdateMouseInputs(double dt)
{
	//Mouse Section
	static bool bLButtonState = false;
	double x, y;
	Application::GetCursorPos(&x, &y);
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();
	float posX = x / w * m_worldWidth;
	float posY = (h - y) / h * m_worldHeight;

	if (Application::IsMousePressed(0)) //move ship to mouse movement
	{
		m_force = Vector3(((posX - m_ship->pos.x) / 5), ((posY - m_ship->pos.y) / 5), 0).Normalized() * 100;
	}

	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;
	}
	static bool bRButtonState = false;
	if (!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;
	}
	else if (bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;
	}

}

void SceneAsteroid::UpdateCamera(double dt)
{
	//move camera pos
	camera.position.x = m_ship->pos.x - m_worldWidth * 0.25;
	camera.position.y = m_ship->pos.y - m_worldHeight * 0.25;
	camera.target.x = m_ship->pos.x - m_worldWidth * 0.25;
	camera.target.y = m_ship->pos.y - m_worldHeight * 0.25;


	camera.position.x = Math::Clamp(camera.position.x, 0.f, m_worldWidth / 2);
	camera.position.y = Math::Clamp(camera.position.y, 0.f, m_worldHeight / 2);
	camera.target.x = Math::Clamp(camera.target.x, 0.f, m_worldWidth / 2);
	camera.target.y = Math::Clamp(camera.target.y, 0.f, m_worldHeight / 2);
}

bool SceneAsteroid::IsCollided(GameObject* go, GameObject* go2)
{
	Vector3 displacement = go2->pos - go->pos;
	float combinedRadii = go->scale.x + go2->scale.x;
	if (displacement.LengthSquared() < combinedRadii * combinedRadii)
	{
		return true;
	}
	return false;
}

void SceneAsteroid::Update(double dt)
{	
	SceneBase::Update(dt);
	Vector3 m_shipOldPos = m_ship->pos;

	//update current wave
	if (!isWaveCompleted[ONE])
		isWaveCompleted[ONE] = WaveOne(dt);
	else if (isWaveCompleted[ONE] && !isWaveCompleted[TWO])
		isWaveCompleted[TWO] = WaveTwo(dt);

	UpdatePowerUp(dt);
	UpdateKeyInputs(dt);
	UpdateMouseInputs(dt);
	UpdateCamera(dt);

	//Physics Simulation Section

	//Exercise 7: Update ship's velocity based on m_force
	Vector3 accel = m_force * (1 / m_ship->mass);
	m_ship->vel += accel * dt * m_speed;
	m_ship->pos += m_ship->vel * dt * m_speed;

	///turn a vector by angle
	float alpha = m_torque.z / m_ship->momentOfInertia;

	m_ship->angularVelocity += alpha * dt;

	m_ship->angularVelocity = Math::Clamp(m_ship->angularVelocity, -1.0f, 1.0f);
	float dTheta = m_ship->angularVelocity * dt;

	m_ship->dir.Set(m_ship->dir.x* cosf(dTheta) - m_ship->dir.y * sinf(dTheta),
		m_ship->dir.x* sinf(dTheta) + m_ship->dir.y * cosf(dTheta)); //turn a vector by angle

	//Exercise 10: Cap Vel to MaxSpeed
	if (m_ship->vel.Length() > MAX_SPEED) 
		m_ship->vel = m_ship->vel.Normalized() * MAX_SPEED;

	//Exercise 9: wrap ship position if it leaves screen
	m_ship->pos.x = Math::Wrap(m_ship->pos.x, 0 - m_ship->scale.x * 0.5f, m_worldWidth + m_ship->scale.x * 0.5f);
	m_ship->pos.y = Math::Wrap(m_ship->pos.y, 0 - m_ship->scale.y * 0.5f, m_worldHeight + m_ship->scale.y * 0.5f);

	enemiesAlive = 0;

	/*--------------------------COLLISION--------------------------------*/
	for (GameObject* go : m_goList)
	{
		if (go->active && (go->type == GameObject::GO_ASTEROID || go->type == GameObject::GO_ASTEROIDLIGHT))
		{
			go->pos += go->vel * dt * m_speed * m_slowmo;
			//Exercise 12: handle collision between GO_SHIP and GO_ASTEROID using simple distance-based check
			if (IsCollided(m_ship, go))
			{
				go->active = false;
				m_ship->vel = Vector3(-go->vel.x, -go->vel.y, -go->vel.z);
				m_ship->health--;
				break;
			}
			//Exercise 13: asteroids should wrap around the screen like the ship
			go->pos.x = Math::Wrap(go->pos.x, 0 - go->scale.x * 0.5f, m_worldWidth + go->scale.x * 0.5f);
			go->pos.y = Math::Wrap(go->pos.y, 0 - go->scale.y * 0.5f, m_worldHeight + go->scale.y * 0.5f);

			for (GameObject* collidedGO : m_goList)
			{
				//Check for collision with enemy and asteroid
				if (collidedGO->active && collidedGO->type == GameObject::GO_ENEMY || collidedGO->type == GameObject::GO_FIGHTER_ENEMY)
				{
					if (IsCollided(go, collidedGO))
					{
						std::cout << "ENEMY COLLIDED WITH ASTEROID" << std::endl;
						collidedGO->health--;
						collidedGO->isHit = true;
						go->active = false;
						if (collidedGO->health <= 0) //Check if enemy is dead
						{
							std::cout << "DELETED ENTITY" << std::endl;
							collidedGO->active = false;
							break;
						}
						break;
					}
				}
				
				if (collidedGO->active && collidedGO->type == GameObject::GO_ASTEROID || go->type == GameObject::GO_ASTEROIDLIGHT)
				{
					if (IsCollided(go, collidedGO))
					{
						collidedGO->vel = Vector3(-collidedGO->vel.x, -collidedGO->vel.y, -collidedGO->vel.z);
						go->vel = Vector3(-go->vel.x, -go->vel.y, -go->vel.z);
						break;
					}
				}
			}
		}

		if (go->active && go->type == GameObject::GO_BULLET)
		{
			//update bullet pos based on vel
			go->pos += go->vel * dt * m_speed;

			//Exercise 16: unspawn bullets when they leave screen
			if ((go->pos.x > m_worldWidth + go->scale.x * 0.5f || go->pos.x < 0 - go->scale.x * 0.5f) ||
				(go->pos.y > m_worldHeight + go->scale.y * 0.5f || go->pos.y < 0 - go->scale.y * 0.5f))
			{
				bulletCount--;
				std::cout << "minus bullet" << std::endl;
				go->active = false;
				break;
			}

			//Exercise 18: collision check between GO_BULLET and GO_ASTEROID
			for (GameObject* collidedGO : m_goList)
			{
				//collision check with asteroids
				if (collidedGO->active && collidedGO->type == GameObject::GO_ASTEROID || collidedGO->type == GameObject::GO_ASTEROIDLIGHT)
				{
					if (IsCollided(go, collidedGO))
					{
						collidedGO->health--;
						collidedGO->isHit = true;
						go->active = false;
						std::cout << "minus bullet" << std::endl;
						bulletCount--;
						if (collidedGO->health <= 0) //Check if asteroid is dead
						{
							std::cout << "DELETED" << std::endl;
							if (collidedGO->isSplitable) //Check if asteroid is splitable
							{
								SplitAsteroid(collidedGO);
								m_score += 2;
							}
							else
								m_score += 1;

							collidedGO->active = false;
						}
						break;
					}
				}

				//collision check with enemies
				if (collidedGO->active && collidedGO->type == GameObject::GO_ENEMY || collidedGO->type == GameObject::GO_FIGHTER_ENEMY)
				{
					

					if (IsCollided(go, collidedGO))
					{
						collidedGO->health--;
						collidedGO->isHit = true;
						bulletCount--;
						std::cout << "minus bullet" << std::endl;
						go->active = false;
						if (collidedGO->health <= 0) //Check if enemy is dead
						{
							std::cout << "DELETED enemy" << std::endl;
							collidedGO->active = false;
							break;
						}
						break;
					}
				}

				//collision check with enemystation
				if (collidedGO->active && collidedGO->type == GameObject::GO_ENEMYSPACESTATION)
				{
					if (IsCollided(go, collidedGO))
					{
						collidedGO->health--;
						bulletCount--;
						go->active = false;
						if (collidedGO->health <= 0) //Check if enemy is dead
						{
							SceneBase::flashingTime = 0;
							SceneBase::missionType = ELIMINATE_REMAINING_ENEMIES_LEVEL1;
							collidedGO->active = false;
							break;
						}
						break;
					}
				}
			}
		}


		if (go->active && (go->type == GameObject::GO_ENEMYSPACESTATION))
		{
			for (GameObject* collidedGO : m_goList)
			{
				if (collidedGO->active && collidedGO->type != GameObject::GO_SHIP || collidedGO->type != GameObject::GO_NONE || collidedGO->type != GameObject::GO_TOTAL)
				{
					continue;
				}
				//Collision check between OBJ and ENEMYSPACESTATION
				if (IsCollided(go, collidedGO))
				{
					collidedGO->active = false;
				}
			}

			if (IsCollided(m_ship, go))
			{
				m_ship->vel = Vector3(-m_ship->vel.x, -m_ship->vel.y, 1);
			}
		}

		if (go->active && (go->type == GameObject::GO_ENEMY))
		{
			enemiesAlive++;
			Vector3 enemyToShipDisplacement = m_shipOldPos - go->pos;
			go->dir = enemyToShipDisplacement;
			go->timer += dt;
			go->pos += go->vel * dt * m_speed * m_slowmo;

			//unspawn enemies when they leave screen
			if ((go->pos.x > m_worldWidth + go->scale.x * 0.5f || go->pos.x < 0 - go->scale.x * 0.5f) ||
				(go->pos.y > m_worldHeight + go->scale.y * 0.5f || go->pos.y < 0 - go->scale.y * 0.5f))
			{
				go->active = false;
				break;
			}

			//Collision check between GO_ENEMY and M_SHIP
			if (IsCollided(m_ship, go))
			{
				std::cout << "ENEMY COLLIDED WITH M_SHIP" << std::endl;
				go->active = false;
				m_ship->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight * 0.5f, 0);
				m_ship->vel.SetZero();
				m_ship->health--;
				break;
			}

			Vector3 displacement = m_ship->pos - go->pos;
			if (go->timer > 4 && displacement.Length() < 60)
			{
				GameObject* newBullet = FetchGO();
				newBullet->type = GameObject::GO_ENEMY_BULLET;
				newBullet->scale = Vector3(.8, .4, .8);
				newBullet->pos = go->pos;
				newBullet->dir = go->dir;
				newBullet->vel = enemyToShipDisplacement.Normalized() * BULLET_SPEED;
				go->timer = 0;
			}

			if (m_laser->active)
			{
				//Collision check between GO_ENEMY and M_LASER
				if (IsCollided(m_laser, go))
				{
					std::cout << "ENEMY COLLIDED WITH M_LASER" << std::endl;
					go->active = false;
					break;
				}
			}
		}

		if (go->active && (go->type == GameObject::GO_FIGHTER_ENEMY))
		{
			enemiesAlive++;
			Vector3 enemyToShipDisplacement = m_shipOldPos - go->pos;
			go->dir = enemyToShipDisplacement;
			go->timer += dt;
			go->pos += go->vel * dt * m_speed * m_slowmo;
			go->vel += enemyToShipDisplacement * dt * m_speed * m_slowmo;

			if (go->vel.Length() > MAX_ENEMY_SPEED)
				go->vel = go->vel.Normalized() * MAX_ENEMY_SPEED;

			go->pos.x = Math::Wrap(go->pos.x, 0 - go->scale.x * 0.5f, m_worldWidth + go->scale.x * 0.5f);
			go->pos.y = Math::Wrap(go->pos.y, 0 - go->scale.y * 0.5f, m_worldHeight + go->scale.y * 0.5f);

			//Collision check between GO_ENEMY and M_SHIP
			Vector3 displacement = m_ship->pos - go->pos;
			if (IsCollided(m_ship, go))
			{
				std::cout << "ENEMY COLLIDED WITH M_SHIP" << std::endl;
				go->active = false;
				m_ship->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight * 0.5f, 0);
				m_ship->vel.SetZero();
				m_ship->health--;
				break;
			}

			if (go->timer > 5 && displacement.Length() < 60)
			{
				GameObject* newBullet = FetchGO();
				newBullet->type = GameObject::GO_ENEMY_BULLET;
				newBullet->scale = Vector3(.8, .4, .8);
				newBullet->pos = go->pos;
				newBullet->dir = go->dir;
				newBullet->vel = enemyToShipDisplacement.Normalized() * BULLET_SPEED;
				go->timer = 0;
			}

			if (m_laser->active)
			{
				//Collision check between GO_ENEMY and M_LASER
				if (IsCollided(m_laser, go))
				{
					std::cout << "ENEMY COLLIDED WITH M_LASER" << std::endl;
					go->active = false;
					break;
				}
			}
		}

		if (go->active && (go->type == GameObject::GO_ENEMY_BULLET))
		{
			go->pos += go->vel * dt * m_speed * m_slowmo;

			//Unspawn enemy bullets when they leave screen
			if ((go->pos.x > m_worldWidth + go->scale.x * 0.5f || go->pos.x < 0 - go->scale.x * 0.5f) ||
				(go->pos.y > m_worldHeight + go->scale.y * 0.5f || go->pos.y < 0 - go->scale.y * 0.5f))
			{
				go->active = false;
				break;
			}

			//Collision check between GO_ENEMYBULLET and M_SHIP
			if (IsCollided(m_ship, go))
			{
				go->active = false;
				m_ship->health--;
				break;
			}
		}

		if (go->active && (go->type == GameObject::GO_SLOWMO_POWERUP))
		{
			go->pos += go->vel * dt * m_speed;

			//check for collision with m_ship and slowmo powerup
			if (IsCollided(m_ship, go))
			{
				go->active = false;
				m_slowmo = 0.25;
				slowmoPowerUpTime = 0;
			}
		}

		if (go->active && (go->type == GameObject::GO_HEALTH_POWERUP))
		{
			go->pos += go->vel * dt * m_speed;

			//check for collision with m_ship and slowmo powerup
			if (IsCollided(m_ship, go))
			{
				go->active = false;
				m_ship->health = 100;
			}
		}

		if (go->active && (go->type == GameObject::GO_INCREASE_FIRERATE_POWERUP))
		{
			go->pos += go->vel * dt * m_speed;

			//check for collision with m_ship and slowmo powerup
			if (IsCollided(m_ship, go))
			{
				go->active = false;
				bulletFireRate = 0.1f;
				increaseFireRatePowerUpTime = 0;
			}
		}

		if (go->active && (go->type == GameObject::GO_MISSILE))
		{
			go->pos += go->vel * dt * m_speed;
			Vector3 enemyDisplacement = Vector3(0, 0, 0);

			go->pos.x = Math::Wrap(go->pos.x, 0 - go->scale.x * 0.5f, m_worldWidth + go->scale.x * 0.5f);
			go->pos.y = Math::Wrap(go->pos.y, 0 - go->scale.y * 0.5f, m_worldHeight + go->scale.y * 0.5f);

			bool foundEnemy = false;
			for (GameObject* enemy : m_goList)
			{
				if (enemy->active && enemy->type == GameObject::GO_ENEMY || enemy->type == GameObject::GO_FIGHTER_ENEMY)
				{
					Vector3 tempDisplacement = enemy->pos - go->pos;
					if (tempDisplacement.LengthSquared() < enemyDisplacement.LengthSquared())
						enemyDisplacement = tempDisplacement;
					if (!foundEnemy)
					{
						enemyDisplacement = tempDisplacement;
						foundEnemy = true;
					}

					go->vel += enemyDisplacement * 0.1f * dt * m_speed;
					go->dir = go->vel;

					if (IsCollided(go, enemy))
					{
						go->active = false;
						enemy->active = false;
					}
				}
			}
		}
	}

	//Spawn Asteroids Every 4 Seconds
	asteroidSpawnTime += dt * m_speed;
	if (asteroidSpawnTime > 4)
	{
		GameObject* newAsteroid = FetchGO();
		newAsteroid->type = GameObject::GO_ASTEROID;
		newAsteroid->health = 2;
		newAsteroid->isSplitable = true;
		newAsteroid->scale = Vector3(5, 5, 5);
		newAsteroid->pos = Vector3(Math::RandFloatMinMax(2, 4) - m_worldWidth, Math::RandFloatMinMax(2, 4) + m_worldHeight, 0);
		newAsteroid->vel = Vector3(Math::RandFloatMinMax(-20, 20), Math::RandFloatMinMax(-20, 20), 0);
		asteroidSpawnTime = 0;
	}

	//check player health <= 0
	if (m_ship->health <= 0) //Check if enemy is dead
	{
		RestartGame();
	}

}


void SceneAsteroid::RenderGO(GameObject *go)
{
	if (go->isHit)
	{
		float scale = (go->health * (1.f/ 100.f)) * 200.f;
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y - 2 * go->scale.x, go->pos.z);
		modelStack.Scale(scale, 1, 1);
		RenderMesh(meshList[GEO_HEALTHBAR], false);
		modelStack.PopMatrix();
	}

	switch (go->type)
	{
	case GameObject::GO_SHIP:
		//Exercise 4a: render a sphere with radius 1
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree((atan2f(m_ship->dir.y, m_ship->dir.x))), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		switch (SceneBase::shipColor)
		{
		case 0:
			RenderMesh(meshList[GEO_SHIP_BLUE], false);
			break;
		case 1:
			RenderMesh(meshList[GEO_SHIP_GREEN], false);
			break;
		}		modelStack.PopMatrix();
		break;
		//Exercise 17a: render a ship texture or 3D ship model
		//Exercise 17b:	re-orientate the ship with velocity
	case GameObject::GO_ASTEROID:
		//Exercise 4b: render a cube with length 2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ASTEROID], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_ASTEROIDLIGHT:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ASTEROIDLIGHT], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_BULLET:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree((atan2f(go->dir.y, go->dir.x))), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		switch (SceneBase::shipColor)
		{
		case 0:
			RenderMesh(meshList[GEO_BULLET_BLUE], false);
			break;
		case 1:
			RenderMesh(meshList[GEO_BULLET_GREEN], false);
			break;
		}
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY:
		//Exercise 4b: render a enemy
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree((atan2f(go->dir.y, go->dir.x))), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ENEMYSHIP], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMY_BULLET:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree((atan2f(go->dir.y, go->dir.x))), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ENEMY_BULLET], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_SLOWMO_POWERUP:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_SLOWMO_POWERUP], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_HEALTH_POWERUP:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_HEALTH_POWERUP], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_INCREASE_FIRERATE_POWERUP:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_FIRERATE_POWERUP], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_LASER:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree((atan2f(go->dir.y, go->dir.x))), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_LASER], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_MISSILE:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree((atan2f(go->dir.y, go->dir.x))), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_MISSILE], false);
		modelStack.PopMatrix();
		break;	
	case GameObject::GO_FIGHTER_ENEMY:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree((atan2f(go->dir.y, go->dir.x))), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_FIGHTER_ENEMY], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_BACKGROUND:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BACKGROUND], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_HEALTHBAR:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ENEMYSPACESTATION:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ENEMYSPACESTATION], false);
		modelStack.PopMatrix();
		break;
	}
}

void SceneAsteroid::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth * 0.5, 0, m_worldHeight * 0.5, -10, 10);
	projectionStack.LoadMatrix(projection);
	
	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
						camera.position.x, camera.position.y, camera.position.z,
						camera.target.x, camera.target.y, camera.target.z,
						camera.up.x, camera.up.y, camera.up.z
					);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();
	
	RenderMesh(meshList[GEO_AXES], false);

	float w = Application::GetWindowWidth();
	float h = Application::GetWindowHeight();

	RenderGO(m_bg);

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
		}
	}

	RenderGO(m_ship);
	if (m_laser->active)
		RenderGO(m_laser);

	//On screen text
	//RenderMeshOnScreen(meshList[GEO_HEALTHBAR], (float)(m_ship->pos.x - camera.position.x) / m_ship->pos.x * camera.position.x, h/10 * 0.5f, m_ship->health, 1);

	//std::cout << w << std::endl;
	//std::cout << h << std::endl;

	//for (GameObject* go : m_goList)
	//{
	//	if (go->active && go->type == GameObject::GO_ASTEROID)
	//	{
	//		float posX = (go->pos.x - camera.position.x); //convert to screen space
	//		float posY = (go->pos.y - camera.position.y); //convert to screen space
	//		RenderMeshOnScreen(meshList[GEO_HEALTHBAR], posX, posY, 2 * go->health, 1);
	//	}
	//}


	////Exercise 5a: Render m_lives, m_score
	//std::ostringstream ss2;
	//ss2.str("");
	//ss2 << "m_lives: " << m_lives;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 9);

	//ss2.str("");
	//ss2 << "m_score: " << m_score;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 12);

	////Exercise 5b: Render position, velocity & mass of ship
	//ss2.str("");
	//ss2 << "ship pos: " << m_ship->pos;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 15);

	//ss2.str("");
	//ss2 << "ship vel: " << m_ship->vel;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 18);

	//ss2.str("");
	//ss2 << "ship mass: " << m_ship->mass;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 21);


	//speed and fps
	std::ostringstream ss;
	ss.precision(3);
	ss << "Speed: " << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);
	
	ss.str("");
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 0);

	ss.str("");
	ss.precision(5);
	ss << "TOTAL ENEMIES: " << enemiesAlive;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 9);

	ss.str("");
	ss.precision(5);
	ss << "CAM TAR: " << camera.target;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 12);

	ss.str("");
	ss.precision(5);
	ss << "Time left: " << waveTime;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 15);



	//missions
	switch (SceneBase::missionType)
	{
	case DESTROY_ENEMYSTATION_LEVEL1:
		RenderMissionOnScreen("SEARCH & DESTROY", "THE ENEMY STATION");
		break;
	case ELIMINATE_REMAINING_ENEMIES_LEVEL1:
		RenderMissionOnScreen("ELIMINATE ALL", "REMAINING ENEMIES");
		break;
	default:
		break;
	}


	ss.str("");
	if (!isWaveCompleted[ONE])
		ss.str("WAVE 1");
	else if (isWaveCompleted[ONE] && !isWaveCompleted[TWO])
		ss.str("WAVE 2");
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 6);
	
	RenderMeshOnScreen(meshList[GEO_HEALTHBAR], w / 10 * 0.5f, 2, m_ship->health * (1.f / 100.f) * 30.f, 2);
	RenderMeshOnScreen(meshList[GEO_NITROBAR], 180, 2, nitroTime * 4, 2);
}

void SceneAsteroid::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ship)
	{
		delete m_ship;
		m_ship = NULL;
	}
}