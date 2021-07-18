#include "SceneGameOver.h"
#include "SceneManager.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneGameOver::SceneGameOver()
{
}

SceneGameOver::~SceneGameOver()
{
}

void SceneGameOver::Init()
{
	SceneBase::Init();

	////Calculating aspect ratio
	//m_worldHeight = 100.f;
	//m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / (float)Application::GetWindowHeight();

	////Physics code here
	//m_speed = 1.f;
	//m_slowmo = 1.f;
	//
	//Math::InitRNG();

	////Exercise 2a: Construct 100 GameObject with type GO_ASTEROID and add into m_goList
	//for (int i = 0; i < 100; i++)
	//{
	//	GameObject* newGO = new GameObject(GameObject::GO_ASTEROID);
	//	m_goList.push_back(newGO);
	//}

	////Exercise 2b: Initialize m_lives and m_score
	//m_lives = 3;
	//m_score = 0;
	//bulletCount = 0;
	//maxBullet = 100;
	//asteroidSpawnTime = 0;
	//bulletSpawnTime = 0;
	//levelTime = 0;
	//enemySpawnTime = 0;
	//powerUpSpawnTime = 0;
	//slowmoPowerUpTime = 0;
	//increaseFireRatePowerUpTime = 0;
	//m_nitro = 1.f;
	//nitroTime = 0;
	//bulletFireRate = 0.3f;
	//mana = 0.f;
	//stationCount = 5;

	////Exercise 2c: Construct m_ship, set active, type, scale and pos
	//m_ship = new GameObject(GameObject::GO_SHIP);
	//m_ship->active = true;
	//m_ship->scale = Vector3(2, 2, 2);
	//m_ship->pos = Vector3(50, 50, 0);
	//m_ship->momentOfInertia = m_ship->mass;
	//m_ship->angularVelocity = 0.f;
	//m_ship->health = 100;

	//m_laser = new GameObject(GameObject::GO_LASER);
	//m_laser->active = false;
	//m_laser->scale = Vector3(30, 2, 2);
	//m_laser->pos = m_ship->pos;
	//m_laser->vel = Vector3(0, 0, 0);

	//m_bg = new GameObject(GameObject::GO_BACKGROUND);
	//m_bg->scale = Vector3(1.92, 1.08, 1);
	//m_bg->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight * 0.5f, -1);
	//m_bg->vel = Vector3(0, 0, 0);
	//

	//for (int i = 0; i < stationCount; i++)
	//{
	//	GameObject* enemyStation = FetchGO();
	//	enemyStation->type = GameObject::GO_ENEMYSPACESTATION;
	//	enemyStation->isHit = true;
	//	enemyStation->health = 1;
	//	enemyStation->scale = Vector3(10, 10, 5);
	//	enemyStation->pos = Vector3(Math::RandFloatMinMax(0 + enemyStation->scale.x, m_worldWidth - enemyStation->scale.x), Math::RandFloatMinMax(0 + enemyStation->scale.x, m_worldHeight - enemyStation->scale.x), 1);
	//}

	//m_enemy = nullptr;

	//SceneBase::missionType = DESTROY_ENEMYSTATION_LEVEL1;
}

GameObject* SceneGameOver::FetchGO()
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

void SceneGameOver::RestartGame()
{
	for (GameObject* go : m_goList)
	{
		if (go->active)
			go->active = false;
	}
	levelTime = 0;
	enemySpawnTime = 0;
	m_lives = 3;
	m_ship->health = 100;
	m_score = 0;
	bulletCount = 0;
	m_speed = 1;
	m_ship->pos = Vector3(50, 50, 0);
	m_ship->vel = Vector3(0, 0, 0);
	m_ship->dir = Vector3(1, 0, 0);

	//spawn and reset enemy station and turrets
}

/*
void SceneGameOver::SplitAsteroid(GameObject* go)
{
	int numOfSplitAsteroids = Math::RandIntMinMax(2,4);
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

void SceneGameOver::SpawnPowerUp(GameObject* go, float rng)
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
		newPowerUp->type = GameObject::GO_SHIELD;
		break;
	}
}

void SceneGameOver::UpdatePowerUp(double dt)
{
	bulletSpawnTime += dt * m_speed;
	slowmoPowerUpTime += dt * m_speed;
	increaseFireRatePowerUpTime += dt * m_speed;

	if (slowmoPowerUpTime > 5)
		m_slowmo = 1;

	if (increaseFireRatePowerUpTime > 5)
		bulletFireRate = .3f;
}

void SceneGameOver::UpdateKeyInputs(double dt)
{
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
		newAsteroid->mass = newAsteroid->scale.x / 2;
		newAsteroid->pos = Vector3(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0.f, m_worldHeight), 0);
		newAsteroid->vel = Vector3(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
	}
	if (Application::IsKeyPressed('C'))
		m_laser->active = true;
	else
		m_laser->active = false;

	if (Application::IsKeyPressed('N'))
	{
		cam2 = !cam2;
	}

	if (Application::IsKeyPressed('V') && mana >= 100.f)
	{
		GameObject* newAlly = FetchGO();
		newAlly->type = GameObject::GO_ALLY_SHIP;
		newAlly->scale = Vector3(1, 1, 1);
		newAlly->pos = m_ship->pos;
		newAlly->vel = Vector3(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
		mana = 0.f;
	}

	if (Application::IsKeyPressed('B'))
	{
		GameObject* newShieldPowerUp = FetchGO();
		newShieldPowerUp->type = GameObject::GO_SHIELD_POWERUP;
		newShieldPowerUp->scale = Vector3(2, 2, 2);
		newShieldPowerUp->pos = Vector3(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0.f, m_worldHeight), 0);
		newShieldPowerUp->vel = Vector3(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
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
*/
void SceneGameOver::UpdateMouseInputs(double dt)
{
	//Mouse Section
	static bool bLButtonState = false;
	double x, y;
	Application::GetCursorPos(&x, &y);
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();
	float posX = x / w * m_worldWidth;
	float posY = (h - y) / h * m_worldHeight;

	if (!bLButtonState && Application::IsMousePressed(0)) //move ship to mouse movement
	{
		bLButtonState = true;
		std::cout << posX << " " << posY << std::endl;
		if (posX >= 80 && posX <= 95 && posY >= 43 && posY <= 50)
		{
			std::cout << "touch play" << std::endl;
		}
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
		if (menuIndex == 0)
		{
			switch (selectorIndex)
			{
			case 0:
				SceneManager::activeScene = S_LEVEL1;
				break;
			case 1:
				menuIndex = 1;
				selectorIndex = 0;
				break;
			case 2:
				Application::gameExit = true;
				break;
			}
		}
		else
		{
			switch (selectorIndex)
			{
			case 0:
				break;
			case 1:
				menuIndex = 0;
				break;
			}
		}
	}
}
/*
void SceneGameOver::UpdateCamera(double dt)
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

bool SceneGameOver::LevelOne(double dt)
{
	levelTime += dt * m_speed;
	enemySpawnTime += dt * m_speed;

	if (enemySpawnTime >= 3 && SceneBase::missionType == DESTROY_ENEMYSTATION_LEVEL1)
	{
		GameObject* newEnemy = FetchGO();
		newEnemy->type = GameObject::GO_ENEMY;
		newEnemy->health = 2;
		newEnemy->scale = Vector3(2, 2, 2);
		newEnemy->mass = newEnemy->scale.x;
		newEnemy->pos = Vector3(Math::RandFloatMinMax(m_worldWidth * 0.5, m_worldWidth * 0.5 + 5), Math::RandFloatMinMax(m_worldHeight - 30, m_worldHeight - 40), 0);
		newEnemy->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f), 0);
		enemySpawnTime = 0;
	}

	if (levelTime >= 120)
	{
		levelTime = 0;
		enemySpawnTime = 0;
		RestartGame();
		std::cout << "GAME OVER" << std::endl;
		//switch to game over menu
	}

	if (enemiesAlive <= 0 && enemiesKilledTime > 1 && SceneBase::missionType == ELIMINATE_REMAINING_ENEMIES_LEVEL1)
	{
		SceneBase::missionType = COMPLETED_LEVEL1;
		std::cout << "COMPLETED LEVEL 1" << std::endl;
		return true;
	}

	return false;
}

bool SceneGameOver::LevelOneBonus(double dt)
{
	if (m_ship->pos.x > m_worldWidth)
	{
		SceneManager::activeScene = S_LEVEL2;
		SceneBase::flashingTime = 0;
		SceneBase::missionType = DESTROY_ENEMYSTATION_TURRET_LEVEL2;
		return true;
	}
	return false;
}
*/
void SceneGameOver::RenderMenu()
{
	RenderMeshOnScreen(meshList[GEO_MAIN_MENU], 96, 25, 45, 45);

	switch (selectorIndex)
	{
	case 0:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 40, 45, 45);
		break;
	case 1:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 25, 45, 45);
		break;
	case 2:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 10, 45, 45);
		break;
	}

	selectorIndex = Math::Clamp(selectorIndex, 0, 2);

}

void SceneGameOver::RenderCustomisationMenu()
{
	RenderMeshOnScreen(meshList[GEO_CUSTOMISATION_MENU], 96, 25, 45, 45);

	switch (selectorIndex)
	{
	case 0:
		//RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 25, 45, 45);
		break;
	case 1:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 10, 45, 45);
		break;
	}
	
	RenderMeshOnScreen(meshList[GEO_PLAYER_SHIP_BLUE_UI], 86, 25, 5, 5);
	RenderMeshOnScreen(meshList[GEO_PLAYER_SHIP_GREEN_UI], 106, 25, 5, 5);
	switch (colourIndex)
	{
	case 0:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 86, 25, 15, 45);
		SceneBase::shipColor = SHIP_BLUE;
		break;
	case 1:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 106, 25, 15, 45);
		SceneBase::shipColor = SHIP_GREEN;
		break;
	}
	
	selectorIndex = Math::Clamp(selectorIndex, 0, 1);
	colourIndex = Math::Clamp(colourIndex, 0, 1);

}

void SceneGameOver::Update(double dt)
{
	UpdateMouseInputs(dt);
}


void SceneGameOver::RenderGO(GameObject *go)
{/*
	if (go->isHit)
	{
		float scale = (go->health * (1.f/ 100.f)) * 200.f;
		go->health = Math::Clamp(go->health, 0, 50);
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
		}		
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
	}*/
}


void SceneGameOver::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
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

	RenderGO(m_bg);

	//for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	//{
	//	GameObject *go = (GameObject *)*it;
	//	if(go->active)
	//	{
	//		RenderGO(go);
	//	}
	//}

	//RenderGO(m_ship);
	//if (m_laser->active)
	//	RenderGO(m_laser);

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


	////speed and fps
	//std::ostringstream ss;
	//ss.precision(3);
	//ss << "Speed: " << m_speed;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);
	//
	//ss.str("");
	//ss.precision(5);
	//ss << "FPS: " << fps;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 0);

	//ss.str("");
	//ss.precision(5);
	//ss << "TOTAL ENEMIES: " << enemiesAlive;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 9);

	//ss.str("");
	//ss.precision(5);
	//ss << "CAM TAR: " << camera.target;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 12);

	//ss.str("");
	//ss.precision(5);
	//ss << "Time left: " << levelTime;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 15);

	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 6);
	//
	//RenderMeshOnScreen(meshList[GEO_HEALTHBAR], w / 10 * 0.5f, 2, m_ship->health * (1.f / 100.f) * 30.f, 2);
	//RenderMeshOnScreen(meshList[GEO_NITROBAR], 180, 2, nitroTime * 4, 2);
	//RenderMeshOnScreen(meshList[GEO_NITROBAR], 190, 5, 2, (mana / 100) * 10);

	switch (menuIndex)
	{
	case 0:
		RenderMenu();
		break;
	case 1:
		RenderCustomisationMenu();
		break;
	}
}

void SceneGameOver::Exit()
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