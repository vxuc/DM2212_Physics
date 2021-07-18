#include "SceneLevelMain.h"
#include "SceneManager.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneLevelMain::SceneLevelMain()
{
}

SceneLevelMain::~SceneLevelMain()
{
}

void SceneLevelMain::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	m_worldHeight = 200.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / (float)Application::GetWindowHeight();

	//Physics code here
	m_speed = 0.f;
	m_slowmo = 1.f;
	
	Math::InitRNG();

	m_goList.clear();

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
	maxBullet = 100;
	asteroidSpawnTime = 0;
	bulletSpawnTime = 0;
	levelTime = 120;
	enemySpawnTime = 0;
	powerUpSpawnTime = 0;
	slowmoPowerUpTime = 0;
	increaseFireRatePowerUpTime = 0;
	m_nitro = 1.f;
	nitroTime = 0;
	bulletFireRate = 0.3f;
	mana = 0.f;
	stationCount = 3;
	activeGOs = 0;
	cooldownMovement = 0;

	//Exercise 2c: Construct m_ship, set active, type, scale and pos
	m_ship = new GameObject(GameObject::GO_SHIP);
	m_ship->active = true;
	m_ship->scale = Vector3(2, 2, 2);
	m_ship->pos = Vector3(50, 50, 1);
	m_ship->mass = 10;
	m_ship->momentOfInertia = m_ship->mass;
	m_ship->angularVelocity = 0.f;
	m_ship->health = 100;
	m_ship->maxHealth = 100;

	m_ally = new GameObject(GameObject::GO_ALLY_SHIP);
	m_ally->active = false;
	m_ally->scale = Vector3(2, 2, 2);
	m_ally->health = 10;
	m_ally->maxHealth = 10;
	m_ally->pos = Vector3(50, 50, 0);

	m_laser = new GameObject(GameObject::GO_LASER);
	m_laser->active = false;
	m_laser->scale = Vector3(30, 2, 2);
	m_laser->pos = m_ship->pos;
	m_laser->vel = Vector3(0, 0, 0);

	GameObject* enemyStation = FetchGO();
	enemyStation->type = GameObject::GO_ENEMYSPACESTATION;
	enemyStation->isHit = true;
	enemyStation->health = 20;
	enemyStation->maxHealth = 20;
	enemyStation->scale = Vector3(10, 10, 5);
	enemyStation->pos = Vector3(150, 150, 0);

	GameObject* enemyStation2 = FetchGO();
	enemyStation2->type = GameObject::GO_ENEMYSPACESTATION;
	enemyStation2->isHit = true;
	enemyStation2->health = 20;
	enemyStation->maxHealth = 20;
	enemyStation2->scale = Vector3(10, 10, 5);
	enemyStation2->pos = Vector3(200, 50, 0);

	GameObject* enemyStation3 = FetchGO();
	enemyStation3->type = GameObject::GO_ENEMYSPACESTATION;
	enemyStation3->isHit = true;
	enemyStation3->health = 20;
	enemyStation->maxHealth = 20;
	enemyStation3->scale = Vector3(10, 10, 5);
	enemyStation3->pos = Vector3(300, 100, 0);

	m_enemy = nullptr;

	SceneBase::missionType = DESTROY_ENEMYSTATION_LEVEL1;
	SceneBase::menuType = M_MAIN;
}

GameObject* SceneLevelMain::FetchGO()
{
	//Exercise 3a: Fetch a game object from m_goList and return it
	for (GameObject* go : m_goList)
	{
		if (!go->active)
		{
			go->active = true;
			go->isHit = false;
			go->health = 1;
			go->timer = 0;
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

void SceneLevelMain::RestartGame()
{
	for (GameObject* go : m_goList)
	{
		if (go->active)
			go->active = false;
	}

	SceneBase::missionType = DESTROY_ENEMYSTATION_LEVEL1;

	stationCount = 3;

	GameObject* enemyStation = FetchGO();
	enemyStation->type = GameObject::GO_ENEMYSPACESTATION;
	enemyStation->isHit = true;
	enemyStation->health = 20;
	enemyStation->scale = Vector3(10, 10, 5);
	enemyStation->pos = Vector3(150, 150, 0);

	GameObject* enemyStation2 = FetchGO();
	enemyStation2->type = GameObject::GO_ENEMYSPACESTATION;
	enemyStation2->isHit = true;
	enemyStation2->health = 20;
	enemyStation2->scale = Vector3(10, 10, 5);
	enemyStation2->pos = Vector3(200, 50, 0);

	GameObject* enemyStation3 = FetchGO();
	enemyStation3->type = GameObject::GO_ENEMYSPACESTATION;
	enemyStation3->isHit = true;
	enemyStation3->health = 20;
	enemyStation3->scale = Vector3(10, 10, 5);
	enemyStation3->pos = Vector3(300, 100, 0);

	m_score = 0;
	bulletCount = 0;
	maxBullet = 100;
	asteroidSpawnTime = 0;
	bulletSpawnTime = 0;
	levelTime = 120;
	enemySpawnTime = 0;
	powerUpSpawnTime = 0;
	slowmoPowerUpTime = 0;
	increaseFireRatePowerUpTime = 0;
	m_nitro = 1.f;
	nitroTime = 0;
	bulletFireRate = 0.3f;
	mana = 0.f;
	cooldownMovement = 0;

	m_ship->health = 100;
	m_ship->timer = 0;
	m_ship->active = true;
	m_ship->pos = Vector3(50, 50, 0);
	m_ship->vel = Vector3(0, 0, 0);
	m_ship->dir = Vector3(1, 0, 0);
}

void SceneLevelMain::SplitAsteroid(GameObject* go)
{
	Vector3 totalMomentum = go->mass * go->vel;

	int numOfSplitAsteroids = Math::RandIntMinMax(1, 3);
	for (int i = 0; i < numOfSplitAsteroids; i++)
	{
		GameObject* splitAsteroid = FetchGO();
		splitAsteroid->type = GameObject::GO_ASTEROIDLIGHT;
		splitAsteroid->health = 1;
		splitAsteroid->maxHealth = 1;
		splitAsteroid->isSplitable = false;
		splitAsteroid->scale = Vector3(go->scale.x * 0.4f, go->scale.y * 0.4f, go->scale.z * 0.4f);
		splitAsteroid->mass = go->mass / 2;
		splitAsteroid->pos = go->pos + Vector3(10 * i, 0, 0);
		splitAsteroid->isHit = true;
		if (i == numOfSplitAsteroids - 1)
		{
			splitAsteroid->vel = totalMomentum;
		}
		else
		{
			splitAsteroid->vel = Vector3(Math::RandFloatMinMax(-10.f, 10.f), Math::RandFloatMinMax(-10.f, 10.f), 0);
		}
	}
}

void SceneLevelMain::SpawnPowerUp(GameObject* go, float rng)
{
	float chance = Math::RandFloatMinMax(0.0f, 1.f);

	if (chance > rng)
		return;

	int random = Math::RandIntMinMax(1, 4);
	GameObject* newPowerUp = FetchGO();
	newPowerUp->scale = Vector3(3, 3, 3);
	newPowerUp->pos = go->pos;
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
	case 4:
		newPowerUp->type = GameObject::GO_SHIELD_POWERUP;
		break;
	}
}

void SceneLevelMain::UpdatePowerUp(double dt)
{
	bulletSpawnTime += dt * m_speed;
	slowmoPowerUpTime += dt * m_speed;
	increaseFireRatePowerUpTime += dt * m_speed;

	if (slowmoPowerUpTime > 5)
		m_slowmo = 1;

	if (increaseFireRatePowerUpTime > 5)
		bulletFireRate = .3f;
}

void SceneLevelMain::UpdateKeyInputs(double dt)
{
	m_force.SetZero();
	m_torque.SetZero();
	//Exercise 6: set m_force values based on WASD

	if (Application::IsKeyPressed('W'))
	{
		m_force += 150 * m_ship->dir * m_nitro;
		m_torque = Vector3(0, -1, 0).Cross(Vector3(0, 100, 0));
	}
	if (Application::IsKeyPressed('S'))
	{
		m_force += -150 * m_ship->dir * m_nitro;
		m_torque = Vector3(0, 1, 0).Cross(Vector3(0, -100, 0));
	}
	if (Application::IsKeyPressed('A'))
	{
		m_force += 75 * m_ship->dir * m_nitro;
		m_torque = Vector3(1, -1, 0).Cross(Vector3(0, 50, 0));
	}
	if (Application::IsKeyPressed('D'))
	{
		m_force += -75 * m_ship->dir * m_nitro;
		m_torque = Vector3(-1, -1, 0).Cross(Vector3(0, 50, 0));
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
		m_nitro += 2.5f;
	else
		m_nitro -= 1.0f;

	m_nitro = Math::Clamp(m_nitro, 1.f, 50.f);
	nitroTime = Math::Clamp(nitroTime, 0.0, 2.0);

	if (Application::IsKeyPressed('C') && mana >= 1.f)
	{
		m_laser->active = true;
		mana -= 1.f;
	}
	else
		m_laser->active = false;

	if (Application::IsKeyReleased('N'))
	{
		cam2 = !cam2;
	}

	if (Application::IsKeyPressed('V') && mana >= 100.f)
	{
		m_ally->active = true;
		m_ally->pos = m_ship->pos;
		m_ally->vel = Vector3(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
		mana = 0.f;
	}

	m_laser->pos = m_ship->pos + m_laser->scale.x * m_ship->dir * m_speed;
	m_laser->dir = m_ship->dir * m_speed * dt;

	//Exercise 14: use a key to spawn a bullet  
	//Exercise 15: limit the spawn rate of bullets
	if (Application::IsKeyPressed(VK_SPACE) && bulletCount < maxBullet && bulletSpawnTime > bulletFireRate)
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
}

void SceneLevelMain::UpdateMouseInputs(double dt)
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

void SceneLevelMain::UpdateCamera(double dt)
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

bool SceneLevelMain::LevelOne(double dt)
{
	levelTime -= dt * m_speed;

	if (levelTime <= 0)
	{
		m_speed = 0;
		m_ship->active = false;
		m_ship->timer += dt;
	}

	if (m_ship->timer >= 1)
	{
		SceneBase::menuType = M_GAMEOVER;
		levelTime = 120;
		enemySpawnTime = 0;
	}

	if (enemiesAlive <= 0 && enemiesKilledTime > 1 && SceneBase::missionType == ELIMINATE_REMAINING_ENEMIES_LEVEL1)
	{
		SceneBase::missionType = COMPLETED_LEVEL1;
		return true;
	}

	return false;
}

bool SceneLevelMain::LevelOneBonus(double dt)
{
	if (m_ship->pos.x > m_worldWidth)
	{
		RestartGame();
		SceneBase::flashingTime = 0;
		SceneBase::missionType = DESTROY_ENEMYSTATION_TURRET_LEVEL2;
		SceneManager::activeScene = S_LEVEL2;
		return true;
	}
	return false;
}

void SceneLevelMain::UpdateMenu()
{
	if (menuType == M_NONE)
		m_speed = 1;
	else
		m_speed = 0;

	if (Application::IsKeyReleased(VK_ESCAPE))
		SceneBase::menuType = M_PAUSE;

	if (Application::IsKeyReleased(VK_DOWN))
		selectorIndex++;
	else if (Application::IsKeyReleased(VK_UP))
		selectorIndex--;

	if (Application::IsKeyReleased(VK_RIGHT))
		colourIndex++;
	else if (Application::IsKeyReleased(VK_LEFT))
		colourIndex--;

	if (Application::IsKeyReleased(VK_RETURN))
	{
		switch (menuType)
		{
		case M_MAIN:
			UpdateMainMenu(m_speed);
			break;
		case M_CUSTOMISE:
			UpdateCustomisationMenu();
			break;
		case M_PAUSE:
			UpdatePauseMenu(m_speed);
			break;
		case M_GAMEOVER:
			UpdateGameOverMenu();
			break;
		case M_WIN:
			UpdateWinMenu();
			break;
		}
	}
}

void SceneLevelMain::Update(double dt)
{
	SceneBase::Update(dt);
	UpdateMenu();

	//dont update anything if in menu
	if (menuType != M_NONE)
		return;

	Vector3 m_shipOldPos = m_ship->pos;

	cooldownMovement -= dt;

	if (m_ship->active)
	{
		UpdatePowerUp(dt);
		if (cooldownMovement <= 0)
			UpdateKeyInputs(dt);
		UpdateCamera(dt);
	}

	if (SceneBase::missionType == COMPLETED_LEVEL1)
		LevelOneBonus(dt);
	else
		LevelOne(dt);


	//Physics Simulation Section

	//Exercise 7: Update ship's velocity based on m_force

	Vector3 accel = m_force * (1 / m_ship->mass);
	m_ship->vel += accel * dt * m_speed;
	m_ship->pos += m_ship->vel * dt * m_speed;

	///turn a vector by angle
	float alpha = m_torque.z / m_ship->momentOfInertia;

	m_ship->angularVelocity += alpha * dt * m_speed * 2;

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

			if (go->vel.Length() > 20)
				go->vel = go->vel.Normalized() * 20;

			//Exercise 12: handle collision between GO_SHIP and GO_ASTEROID using simple distance-based check
			if (IsCollided(m_ship, go))
			{
				//go->active = false;
				cooldownMovement = 0.15f;
				MomentumCollision(go, m_ship);
				m_ship->health--;
				break;
			}

			if (IsCollided(m_ally, go))
			{
				//go->active = false;
				MomentumCollision(go, m_ally);
				m_ally->health--;
				break;
			}

			//Exercise 13: asteroids should wrap around the screen like the ship
			go->pos.x = Math::Wrap(go->pos.x, 0 - go->scale.x * 0.5f, m_worldWidth + go->scale.x * 0.5f);
			go->pos.y = Math::Wrap(go->pos.y, 0 - go->scale.y * 0.5f, m_worldHeight + go->scale.y * 0.5f);

			for (GameObject* collidedGO : m_goList)
			{
				if (collidedGO == go)
					continue;

				//Check for collision with enemy and asteroid
				if (collidedGO->active && collidedGO->type == GameObject::GO_ENEMY || collidedGO->type == GameObject::GO_FIGHTER_ENEMY)
				{
					if (IsCollided(go, collidedGO))
					{
						std::cout << "ENEMY COLLIDED WITH ASTEROID" << std::endl;
						collidedGO->health--;
						collidedGO->isHit = true;
						MomentumCollision(go, collidedGO);
						if (collidedGO->health <= 0) //Check if enemy is dead
						{
							collidedGO->active = false;
							break;
						}
						break;
					}
				}
				
				if (collidedGO->active && (collidedGO->type == GameObject::GO_ASTEROID || collidedGO->type == GameObject::GO_ASTEROIDLIGHT))
				{
					if (IsCollided(go, collidedGO))
					{
						MomentumCollision(collidedGO, go);
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
				if (collidedGO->active && collidedGO->type == GameObject::GO_ASTEROID)
				{
					if (IsCollided(go, collidedGO))
					{
						collidedGO->health--;
						collidedGO->isHit = true;
						go->active = false;
						bulletCount--;
						if (collidedGO->health <= 0) //Check if asteroid is dead
						{
							if (collidedGO->isSplitable) //Check if asteroid is splitable
							{
								mana += 10;
								SpawnPowerUp(collidedGO, 0.25);
								SplitAsteroid(collidedGO);
								SceneBase::score += 5;
							}
							else
								SceneBase::score += 2;

							collidedGO->active = false;
							break;
						}
						break;
					}
				}
				
				if (collidedGO->active && collidedGO->type == GameObject::GO_ASTEROIDLIGHT)
				{
					if (IsCollided(go, collidedGO))
					{
						collidedGO->health--;
						go->active = false;
						bulletCount--;
						if (collidedGO->health <= 0) //Check if asteroid is dead
						{
							SceneBase::score += 2;
							collidedGO->active = false;
							break;
						}
						break;
					}
				}

				//collision check with enemies
				if (collidedGO->active && collidedGO->type == GameObject::GO_ENEMY)
				{
					if (IsCollided(go, collidedGO))
					{
						collidedGO->health--;
						collidedGO->isHit = true;
						bulletCount--;
						go->active = false;
						if (collidedGO->health <= 0) //Check if enemy is dead
						{
							SpawnPowerUp(collidedGO, 0.75);
							mana += 20;
							SceneBase::score += 10;
							collidedGO->active = false;
							break;
						}
						break;
					}
				}

				//collision check with enemies
				if (collidedGO->active && collidedGO->type == GameObject::GO_FIGHTER_ENEMY)
				{
					if (IsCollided(go, collidedGO))
					{
						collidedGO->health--;
						collidedGO->isHit = true;
						bulletCount--;
						go->active = false;
						if (collidedGO->health <= 0) //Check if enemy is dead
						{
							SceneBase::score += 15;
							SpawnPowerUp(collidedGO, 0.75);
							mana += 20;
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
							SceneBase::score += 35;
							stationCount--;
							collidedGO->active = false;
						}

						if (stationCount <= 0)
						{
							SceneBase::flashingTime = 0;
							SceneBase::missionType = ELIMINATE_REMAINING_ENEMIES_LEVEL1;
						}
						break;
					}
				}

				if (collidedGO->active && collidedGO->type == GameObject::GO_ENEMY_TURRET)
				{
					if (IsCollided(go, collidedGO))
					{
						collidedGO->health--;
						bulletCount--;
						go->active = false;
						if (collidedGO->health <= 0) //Check if enemy is dead
						{
							SceneBase::score += 25;
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
			go->timer += dt;

			for (GameObject* collidedGO : m_goList)
			{
				if (collidedGO->active && collidedGO->type == GameObject::GO_ASTEROID)
				{
					if (IsCollided(go, collidedGO))
					{
						MomentumCollision(collidedGO, go);
					}
				}
			}

			if (IsCollided(m_ship, go))
			{
				MomentumCollision(m_ship, go);
			}

			Vector3 displacement = m_ship->pos - go->pos;

			if (displacement.Length() > 100)
				continue;

			if (go->timer > 7)
			{
				GameObject* newEnemy = FetchGO();
				Vector3 randomPos = Vector3(1,1,1);
				int random = Math::RandIntMinMax(1, 2);
				newEnemy->scale = Vector3(2, 2, 2);
				newEnemy->mass = newEnemy->scale.x * 3;
				newEnemy->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f), 0);

				switch (random)
				{
				case 1:
					randomPos = Vector3(Math::RandFloatMinMax(-5, 5), 15 + Math::RandFloatMinMax(0, 5), 0);
					newEnemy->type = GameObject::GO_ENEMY;
					newEnemy->health = 3;
					break;
				case 2:
					randomPos = Vector3(Math::RandFloatMinMax(-5, 5), -15 - Math::RandFloatMinMax(0, 5), 0);
					newEnemy->type = GameObject::GO_FIGHTER_ENEMY;
					newEnemy->health = 2;
					break;
				default:
					break;
				}
				newEnemy->pos = go->pos + randomPos;
				go->timer = 0;
			}
		}

		if (go->active && (go->type == GameObject::GO_ENEMY))
		{
			enemiesAlive++;
			Vector3 enemyToShipDisplacement = m_shipOldPos - go->pos;
			go->dir = enemyToShipDisplacement;
			go->timer += dt;
			go->pos += go->vel * dt * m_speed * m_slowmo;

			if (go->vel.Length() > MAX_ENEMY_SPEED)
				go->vel = go->vel.Normalized() * MAX_ENEMY_SPEED;

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
				MomentumCollision(m_ship, go);
				m_ship->health--;
				break;
			}

			if (m_ally->active)
			{
				Vector3 displacement = m_ally->pos - go->pos;
				if (go->timer > 4 && displacement.Length() < 70)
				{
					GameObject* newBullet = FetchGO();
					newBullet->type = GameObject::GO_ENEMY_BULLET;
					newBullet->scale = Vector3(.8, .4, .8);
					newBullet->pos = go->pos;
					newBullet->dir = go->dir;
					newBullet->vel = displacement.Normalized() * BULLET_SPEED;
					go->timer = 0;
				}
			}

			if (m_laser->active)
			{
				//Collision check between GO_ENEMY and M_LASER
				if (IsCollided(m_laser, go))
				{
					go->health -= 0.1f;
					go->isHit = true;
					if (go->health <= 0.f) //Check if enemy is dead
					{
						SpawnPowerUp(go, 0.75);
						mana += 20;
						go->active = false;
						break;
					}
					break;
				}
			}

			if (m_ally->active)
				continue;

			Vector3 displacement = m_ship->pos - go->pos;
			if (go->timer > 4 && displacement.Length() < 70)
			{
				GameObject* newBullet = FetchGO();
				newBullet->type = GameObject::GO_ENEMY_BULLET;
				newBullet->scale = Vector3(.8, .4, .8);
				newBullet->pos = go->pos;
				newBullet->dir = go->dir;
				newBullet->vel = enemyToShipDisplacement.Normalized() * BULLET_SPEED;
				go->timer = 0;
			}
		}

		if (go->active && (go->type == GameObject::GO_FIGHTER_ENEMY))
		{
			enemiesAlive++;
			Vector3 enemyToShipDisplacement = m_shipOldPos - go->pos;

			if (enemyToShipDisplacement.Length() > 100)
				continue;

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

			if (displacement.Length() < 10)
				go->vel.SetZero();

			if (IsCollided(m_ship, go))
			{
				MomentumCollision(m_ship, go);
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
					go->health -= 0.1f;
					go->isHit = true;
					if (go->health <= 0.f) //Check if enemy is dead
					{
						SpawnPowerUp(go, 0.75);
						mana += 20;
						go->active = false;
						break;
					}
					break;
				}
			}
		}

		if (go->active && (go->type == GameObject::GO_ENEMY_TURRET))
		{
			Vector3 enemyToShipDisplacement = m_shipOldPos - go->pos;
			go->dir = enemyToShipDisplacement;
			go->timer += dt;

			//Collision check between GO_ENEMY and M_SHIP
			if (IsCollided(m_ship, go))
			{
				m_ship->health--;
				m_ship->vel = -m_ship->vel;
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

			//Collision check between GO_ENEMY and M_SHIP
			if (IsCollided(m_ally, go))
			{
				go->active = false;
				m_ally->health--;
				m_ally->isHit = true;
				if (m_ally->health <= 0)
					m_ally->active = false;
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

		if (go->active && (go->type == GameObject::GO_SHIELD_POWERUP))
		{
			go->pos += go->vel * dt * m_speed;

			//check for collision with m_ship and slowmo powerup
			if (IsCollided(m_ship, go))
			{
				go->active = false;
				GameObject* newShield = FetchGO();
				newShield->type = GameObject::GO_SHIELD;
				newShield->scale = Vector3(3, 4, 3);
				go->pos = m_ship->pos + go->scale.x * m_ship->dir * m_speed;
				go->dir = m_ship->dir * m_speed * dt;
			}
		}

		if (go->active && (go->type == GameObject::GO_SHIELD))
		{
			go->pos = m_ship->pos + go->scale.x * m_ship->dir * m_speed;
			go->dir = m_ship->dir * m_speed * dt;
			//go->pos = m_ship->pos + 5 * m_ship->dir;
			go->timer += dt;

			for (GameObject* collidedGO : m_goList)
			{
				if (collidedGO->active && collidedGO->type == GameObject::GO_ENEMY || collidedGO->type == GameObject::GO_ENEMY_BULLET || collidedGO->type == GameObject::GO_FIGHTER_ENEMY ||
					collidedGO->type == GameObject::GO_MISSILE || collidedGO->type == GameObject::GO_ASTEROID || collidedGO->type == GameObject::GO_ASTEROIDLIGHT)
				{
					//continue;
					if (IsCollided(go, collidedGO))
					{
						collidedGO->active = false;
					}
				}
			}

			if (go->timer > 10)
				go->active = false;
		}

		if (go->active && (go->type == GameObject::GO_MISSILE))
		{
			go->pos += go->vel * dt * m_speed * m_slowmo;
			//Vector3 enemyDisplacement = Vector3(0, 0, 0);

			go->pos.x = Math::Wrap(go->pos.x, 0 - go->scale.x * 0.5f, m_worldWidth + go->scale.x * 0.5f);
			go->pos.y = Math::Wrap(go->pos.y, 0 - go->scale.y * 0.5f, m_worldHeight + go->scale.y * 0.5f);

			
			Vector3 missileToShipDisplacement = m_ship->pos - go->pos;
			go->dir = missileToShipDisplacement;
			go->vel = missileToShipDisplacement.Normalized() * MISSILE_SPEED;

			if (go->vel.Length() > MISSILE_SPEED)
				go->vel = go->vel.Normalized() * MISSILE_SPEED;

			if (IsCollided(m_ship, go))
			{
				MomentumCollision(m_ship, go);
				go->active = false;
				m_ship->health -= 5;
			}

			for (GameObject* collidedGO : m_goList)
			{
				if (collidedGO->active && collidedGO->type == GameObject::GO_BULLET)
				{
					if (IsCollided(go, collidedGO))
					{
						go->active = false;
						collidedGO->active = false;
					}
				}
			}
				/*if (enemy->active && enemy->type == GameObject::GO_ENEMYSPACESTATION)
				{
					Vector3 displacement = enemy->pos - go->pos;
					go->vel += displacement * 0.1f * dt * m_speed;
					go->dir = go->vel;

					if (IsCollided(go, enemy))
					{
						go->active = false;
						enemy->health -= 5;
					}
				}*/
					

					
					/*Vector3 tempDisplacement = enemy->pos - go->pos;
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
						foundEnemy = false;
					}*/
		}

		if (go->active && (go->type == GameObject::GO_MANA))
		{
			go->pos += go->vel * dt * m_speed;
			if (IsCollided(m_ship, go))
			{
				mana += 1;
				go->active = false;
			}

			for (GameObject* collidedGO : m_goList)
			{
				if (collidedGO->active && collidedGO->type != GameObject::GO_SHIP && collidedGO->type != GameObject::GO_MANA)
				{
					if (IsCollided(go, collidedGO))
					{
						go->vel = -go->vel;
					}
				}
			}
		}
	}

	activeGOs = 0;

	for (GameObject* go : m_goList)
	{
		if (go->active)
			activeGOs++;
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
		newAsteroid->mass = newAsteroid->scale.x / 2;
		newAsteroid->pos = Vector3(Math::RandFloatMinMax(2, 4) - m_worldWidth, Math::RandFloatMinMax(2, 4) + m_worldHeight, 0);
		newAsteroid->vel = Vector3(Math::RandFloatMinMax(-20, 20), Math::RandFloatMinMax(-20, 20), 0);
		asteroidSpawnTime = 0;
	}

	if (m_ally->active)
	{
		m_ally->pos += m_ally->vel * dt * m_speed;
		m_ally->timer += dt;

		m_ally->pos.x = Math::Clamp(m_ally->pos.x, m_ship->pos.x - 15.f, m_ship->pos.x + 15.f);
		m_ally->pos.y = Math::Clamp(m_ally->pos.y, m_ship->pos.y - 15.f, m_ship->pos.y + 15.f);

		for (GameObject* lockedOnEnemy : m_goList)
		{
			if (lockedOnEnemy->active && (lockedOnEnemy->type == GameObject::GO_FIGHTER_ENEMY) || (lockedOnEnemy->type == GameObject::GO_ENEMY))
			{
				if (m_enemy == nullptr)
				{
					m_enemy = lockedOnEnemy;
					break;
				}

				if (!m_enemy->active)
					m_enemy = nullptr;
			}
		}

		if (m_enemy != nullptr)
		{
			Vector3 displacement = m_enemy->pos - m_ally->pos;
			m_ally->dir = displacement;

			std::cout << m_enemy->type << std::endl;
			if (m_ally->timer > 2)
			{
				GameObject* bullet = FetchGO();
				bullet->type = GameObject::GO_BULLET;
				bullet->scale = Vector3(1, .5, 1);
				bullet->pos = m_ally->pos;
				bullet->dir = displacement;
				bullet->vel = displacement.Normalized() * BULLET_SPEED;
				m_ally->timer = 0;
			}
		}

		if (enemiesAlive <= 0 && enemiesKilledTime > 1 && stationCount <= 0)
			m_ally->active = false;
	}

	mana = Math::Clamp(mana, 0.f, 100.f);

	//check player health <= 0
	if (m_ship->health <= 0)
	{
		m_speed = 0;
		m_ship->active = false;
		m_ship->timer += dt;
	}

	if (m_ship->timer >= 1)
	{
		SceneBase::menuType = M_GAMEOVER;
	}

	if (SceneBase::restartGame)
	{
		RestartGame();
		SceneBase::restartGame = false;
		SceneBase::menuType = M_NONE;
	}

	if (SceneBase::resetGame)
	{
		RestartGame();
		SceneBase::resetGame = false;
		SceneBase::menuType = M_MAIN;
	}
	
	if (enemiesAlive <= 0)
		enemiesKilledTime += dt;
	else
		enemiesKilledTime = 0;
}


void SceneLevelMain::RenderGO(GameObject *go)
{
	if (go->isHit)
	{
		float scale = (go->health * (1.f/ 100.f)) * 200.f;
		go->health = Math::Clamp(go->health, 0.f, 50.f);
		if (go->type == GameObject::GO_ALLY_SHIP)
		{
			modelStack.PushMatrix();
			modelStack.Translate(go->pos.x, go->pos.y - 2 * go->scale.x, go->pos.z);
			modelStack.Scale(scale, 1, 1);
			RenderMesh(meshList[GEO_HEALTHBAR], false);
			modelStack.PopMatrix();
		}
		else
		{
			modelStack.PushMatrix();
			modelStack.Translate(go->pos.x, go->pos.y - 2 * go->scale.x, go->pos.z);
			modelStack.Scale(scale, 1, 1);
			RenderMesh(meshList[GEO_ENEMY_HEALTHBAR], false);
			modelStack.PopMatrix();
		}
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
		}			
		RenderMesh(meshList[GEO_SHIP_BLUE], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_ALLY_SHIP:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree((atan2f(go->dir.y, go->dir.x))), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ALLY_SHIP], false);
		modelStack.PopMatrix();
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
	case GameObject::GO_SHIELD_POWERUP:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_SHIELD_POWERUP], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_SHIELD:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree((atan2f(go->dir.y, go->dir.x))), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_SHIELD], false);
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
	case GameObject::GO_ENEMY_TURRET:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree((atan2f(go->dir.y, go->dir.x))), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ENEMY_TURRET], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_MANA:
		//Exercise 4b: render a bullet sphere with scale 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_MANA], false);
		modelStack.PopMatrix();
		break;
	}
}

void SceneLevelMain::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (cam2)
	{
		Mtx44 projection2;
		projection2.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
		projectionStack.LoadMatrix(projection2);
		// Camera matrix
		viewStack.LoadIdentity();
		viewStack.LookAt(
			camera2.position.x, camera2.position.y, camera2.position.z,
			camera2.target.x, camera2.target.y, camera2.target.z,
			camera2.up.x, camera2.up.y, camera2.up.z
		);
	}
	else
	{

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
	}
	
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();
	
	RenderMesh(meshList[GEO_AXES], false);

	float w = Application::GetWindowWidth();
	float h = Application::GetWindowHeight();

	modelStack.PushMatrix();
	modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.5f, -1);
	modelStack.Scale(1.92, 1.08, 1);
	RenderMesh(meshList[GEO_BACKGROUND], false);
	modelStack.PopMatrix();

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
		}
	}

	if (m_ship->active)
		RenderGO(m_ship);
	if (m_laser->active)
		RenderGO(m_laser);
	if (m_ally->active)
		RenderGO(m_ally);


	switch (menuType)
	{
	case M_MAIN:
		RenderMainMenu();
		break;
	case M_CUSTOMISE:
		RenderCustomisationMenu();
		break;
	case M_PAUSE:
		RenderPauseMenu();
		break;
	case M_GAMEOVER:
		RenderGameOverMenu();
		break;
	case M_WIN:
		RenderWinMenu();
		break;
	}

	if (menuType != M_NONE)
		return;

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
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 6);

	ss.str("");
	ss.precision(5);
	ss << "ACTIVE GO: " << activeGOs;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 9);

	ss.str("");
	ss.precision(5);
	ss << "SCORE: " << SceneBase::score;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 12);

	ss.str("");
	ss.precision(5);
	ss << levelTime;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 5, 85, 100);



	//missions
	switch (SceneBase::missionType)
	{
	case DESTROY_ENEMYSTATION_LEVEL1:
		RenderMissionOnScreen("SEARCH & DESTROY", "THE ENEMY STATION");		
		break;
	case ELIMINATE_REMAINING_ENEMIES_LEVEL1:
		RenderMissionOnScreen("ELIMINATE ALL", "REMAINING ENEMIES");
		break;
	case COMPLETED_LEVEL1:
		RenderTextOnScreen(meshList[GEO_TEXT], "PROCEED-->", Color(0, 1, 0), 6, 130, 100);
		break;
	default:
		break;
	}

	RenderMeshOnScreen(meshList[GEO_HEALTHBAR], w / 10 * 0.5f, 2, m_ship->health * (1.f / 100.f) * 30.f, 2);
	RenderMeshOnScreen(meshList[GEO_HEALTHBAR_BOX], w / 10 * 0.5f, 2, m_ship->maxHealth * (1.f / 100.f) * 15.f, 1);

	RenderMeshOnScreen(meshList[GEO_NITROBAR], 180, 2, nitroTime * 4, 2);
	RenderMeshOnScreen(meshList[GEO_HEALTHBAR_BOX], 180, 2, 2 * 2.05f, 1);

	RenderMeshOnScreen(meshList[GEO_MANABAR], 190, 12, 2, (mana / 100) * 20);
	RenderMeshOnScreen(meshList[GEO_HEALTHBAR_BOX], 190, 12, (100 / 100) * 10, 1, 90);
}

void SceneLevelMain::Exit()
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