#include "SceneMenu.h"
#include "SceneManager.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>
MENU_TYPE SceneMenu::menuType;

SceneMenu::SceneMenu()
{
}

SceneMenu::~SceneMenu()
{
}

void SceneMenu::Init()
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

GameObject* SceneMenu::FetchGO()
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

void SceneMenu::RestartGame()
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
void SceneMenu::SplitAsteroid(GameObject* go)
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

void SceneMenu::SpawnPowerUp(GameObject* go, float rng)
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

void SceneMenu::UpdatePowerUp(double dt)
{
	bulletSpawnTime += dt * m_speed;
	slowmoPowerUpTime += dt * m_speed;
	increaseFireRatePowerUpTime += dt * m_speed;

	if (slowmoPowerUpTime > 5)
		m_slowmo = 1;

	if (increaseFireRatePowerUpTime > 5)
		bulletFireRate = .3f;
}

void SceneMenu::UpdateKeyInputs(double dt)
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
void SceneMenu::UpdateMouseInputs(double dt)
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

	if (Application::IsKeyReleased('1'))
		menuType = static_cast<MENU_TYPE>(static_cast<int>(menuType) + 1);
	else if (Application::IsKeyReleased('2'))
		menuType = static_cast<MENU_TYPE>(static_cast<int>(menuType) - 1);

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
			UpdateMainMenu();
			break;
		case M_CUSTOMISE:
			UpdateCustomisationMenu();
			break;
		case M_PAUSE:
			UpdatePauseMenu();
			break;
		case M_GAMEOVER:
			UpdateGameOverMenu();
			break;
		}
	}
}

void SceneMenu::RenderMainMenu()
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

void SceneMenu::RenderCustomisationMenu()
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

void SceneMenu::RenderPauseMenu()
{
	RenderMeshOnScreen(meshList[GEO_PAUSE_MENU], 96, 25, 45, 45);

	switch (selectorIndex)
	{
	case 0:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 26, 45, 45);
		break;
	case 1:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 10, 45, 45);
		break;
	}

	selectorIndex = Math::Clamp(selectorIndex, 0, 1);
}

void SceneMenu::RenderGameOverMenu()
{
	RenderMeshOnScreen(meshList[GEO_GAMEOVER_MENU], 96, 25, 45, 45);

	switch (selectorIndex)
	{
	case 0:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 26, 45, 45);
		break;
	case 1:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 10, 45, 45);
		break;
	}

	selectorIndex = Math::Clamp(selectorIndex, 0, 1);
}

void SceneMenu::UpdateMainMenu()
{
	switch (selectorIndex)
	{
	case 0:
		SceneManager::activeScene = S_LEVEL1;
		break;
	case 1:
		menuType = M_CUSTOMISE;
		selectorIndex = 0;
		break;
	case 2:
		Application::gameExit = true;
		break;
	}
}

void SceneMenu::UpdateCustomisationMenu()
{
	switch (selectorIndex)
	{
	case 0:
		break;
	case 1:
		menuType = M_MAIN;
		break;
	}
}

void SceneMenu::UpdatePauseMenu()
{
	switch (selectorIndex)
	{
	case 0:
		SceneManager::activeScene = SceneManager::prevScene;
		break;
	case 1:
		menuType = M_MAIN;
		break;
	}
}

void SceneMenu::UpdateGameOverMenu()
{
	switch (selectorIndex)
	{
	case 0:
		SceneManager::activeScene = S_LEVEL1;
		break;
	case 1:
		menuType = M_MAIN;
		break;
	}
}

void SceneMenu::Update(double dt)
{
	UpdateMouseInputs(dt);
}


void SceneMenu::RenderGO(GameObject *go)
{
}


void SceneMenu::Render()
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

	modelStack.PushMatrix();
	modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.5f, -1);
	modelStack.Scale(1.92, 1.08, 1);
	RenderMesh(meshList[GEO_BACKGROUND], false);
	modelStack.PopMatrix();

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
	}
}

void SceneMenu::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
}