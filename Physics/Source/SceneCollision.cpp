#include "SceneCollision.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneCollision::SceneCollision()
{
}

SceneCollision::~SceneCollision()
{
}

void SceneCollision::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / (float)Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;
	
	Math::InitRNG();

	//Exercise 1: initialize m_objectCount
	m_objectCount = 0;

	m_ghost = new GameObject(GameObject::GO_BALL);

	m_gravity = Vector3(0, -50, 0);


	m_flipperLeft = new GameObject(GameObject::GO_FLIPPER);
	m_flipperLeft->active = true;
	m_flipperLeft->isLeft = true;
	m_flipperLeft->scale = Vector3(1, 10, 1);
	m_flipperLeft->normal = Vector3(cos(Math::DegreeToRadian(45.f)), sin(Math::DegreeToRadian(45.f)), 0);
	m_flipperLeft->pos = Vector3(m_worldWidth * 0.5 - 10, m_worldHeight * 0.25, 0);
	m_goList.push_back(m_flipperLeft);

	m_flipperRight = new GameObject(GameObject::GO_FLIPPER);
	m_flipperRight->active = true;
	m_flipperRight->isLeft = false;
	m_flipperRight->scale = Vector3(1, 5, 1);
	m_flipperRight->normal = Vector3(cos(Math::DegreeToRadian(135.f)), sin(Math::DegreeToRadian(135.f)), 0);
	m_flipperRight->pos = Vector3(m_worldWidth * 0.5 + 10, m_worldHeight * 0.25, 0);
	m_goList.push_back(m_flipperRight);

	m_spring = new GameObject(GameObject::GO_SPRING);
	m_spring->active = true;
	m_spring->scale = Vector3(5, 2, 1);
	m_spring->normal = Vector3(0, 1, 0);
	m_spring->pos = Vector3(m_worldWidth * 0.94, m_worldHeight * 0.1, 0);
	m_goList.push_back(m_spring);

	GameObject* ball = FetchGO();
	ball->scale = Vector3(2, 2, 2);
	ball->mass = 2;
	ball->pos = Vector3(m_worldWidth * 0.94, m_worldHeight * 0.1, 0) + m_spring->normal * (m_spring->scale.x + ball->scale.x + 5);

	//CreateThickWall(Vector3(12, 12, 1), Vector3(m_worldWidth * 0.5f, m_worldHeight * 0.5f, 0));

	//top corner curved walls
	int n = 8;
	for (int i = 0; i < n; i++)
	{
		GameObject* go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->active = true;
		go->scale.Set(2, 2, 1);
		go->normal.Set(cos(Math::DegreeToRadian(100 * i / n)), sin(Math::DegreeToRadian(100 * i / n)), 0);
		go->pos = Vector3(m_worldWidth * 0.875, m_worldHeight * 0.75, 0) + (go->normal * (2 * n));
	}

	for (int i = 0; i < n; i++)
	{
		GameObject* go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->active = true;
		go->scale.Set(2, 2, 1);
		go->normal.Set(cos(Math::DegreeToRadian(85 + 100 * i / n)), sin(Math::DegreeToRadian(85 + 100 * i / n)), 0);
		go->pos = Vector3(m_worldWidth * 0.175, m_worldHeight * 0.75, 0) + (go->normal * (2 * n));
	}

	/*GameObject* go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->active = true;
	go->scale.Set(2, 2, 1);
	go->normal.Set(0, 1, 0);
	go->pos = Vector3(m_worldWidth * 0.25, m_worldHeight * 0.25, 0);*/

	//for (int i = -1; i <= 1; i += 2)
	//{
	//	GameObject* go2 = FetchGO();
	//	go2->type = GameObject::GO_PILLAR;
	//	go2->scale.Set(go->scale.x, go->scale.x, 1);
	//	go2->pos = Vector3(go->pos.x + i * go->scale.y, go->pos.y, 0);
	//}

	////thick wall
	//GameObject* go1 = FetchGO();
	//go1->type = GameObject::GO_WALL;
	//go1->active = true;
	//go1->scale.Set(12, 12, 1);
	//go1->normal.Set(0, 1, 0);
	//go1->pos = Vector3(m_worldWidth * 0.5, m_worldHeight * 0.5, 0);
	//GameObject* go2 = FetchGO();
	//go2->type = GameObject::GO_WALL;
	//go2->active = true;
	//go2->scale.Set(12, 12, 1);
	//go2->normal.Set(1, 0, 0);
	//go2->pos = Vector3(m_worldWidth * 0.5, m_worldHeight * 0.5, 0);
	//for (int i = -1; i <= 1; i += 2)
	//{
	//	GameObject* go3 = FetchGO();
	//	go3->type = GameObject::GO_PILLAR;
	//	go3->active = true;
	//	go3->scale.Set(0.2, 0.2, 0.2);
	//	go3->pos = Vector3(go1->pos.x + i * go1->scale.y, go1->pos.y - i * go1->scale.y, 0);
	//}
	//for (int i = -1; i <= 1; i += 2)
	//{
	//	GameObject* go4 = FetchGO();
	//	go4->type = GameObject::GO_PILLAR;
	//	go4->active = true;
	//	go4->scale.Set(0.2, 0.2, 0.2);
	//	go4->pos = Vector3(go1->pos.x + i * go1->scale.y, go1->pos.y + i * go1->scale.y, 0);
	//}

	//GameObject* go3 = FetchGO();
	//go3->type = GameObject::GO_WALL;
	//go3->active = true;
	//go3->scale.Set(2, 12, 1);
	//go3->normal.Set(cos(Math::DegreeToRadian(45)), sin(Math::DegreeToRadian(45)), 0);
	//go3->pos = Vector3(m_worldWidth * 0.25, m_worldHeight * 0.25, 0);

	/*GameObject* go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->active = true;
	go->scale.Set(2, 2, 1);
	go->normal.Set(0, 1, 0);
	go->pos = Vector3(m_worldWidth * 0.5, m_worldHeight * 0.5, 0) + (go->normal * (2 * n));*/
}

GameObject* SceneCollision::FetchGO()
{
	//Exercise 2a: implement FetchGO()
	for (auto go : m_goList)
	{
		if (!go->active)
		{
			m_objectCount++;
			go->active = true;
			return go;
		}
	}
	for (int i = 0; i < 10; i++)
	{
		GameObject* newGO = new GameObject(GameObject::GO_BALL);
		m_goList.push_back(newGO);
	}
	//Exercise 2b: increase object count every time an object is set to active
	FetchGO();
}

void SceneCollision::ReturnGO(GameObject *go)
{
	//Exercise 3: implement ReturnGO()
	go->active = false;
	m_objectCount--;
}

bool SceneCollision::CheckCollision(GameObject* go, GameObject* go2, float dt)
{
	if (go2->type == GameObject::GO_BALL)
	{
		Vector3 displacement = go2->pos - go->pos;
		float combinedRadii = go->scale.x + go2->scale.x;
		if ((displacement.LengthSquared() < combinedRadii * combinedRadii) && 
			(go2->vel - go->vel).Dot(go2->pos - go->pos) < 0) //prevents internal collision with ball and ball
			return true;
	}
	else if (go2->type == GameObject::GO_WALL || go2->type == GameObject::GO_SPRING || go2->type == GameObject::GO_FLIPPER)
	{
		Vector3 N = go2->normal;
		Vector3 NP = Vector3(N.y, -N.x, 0);
		Vector3 w0_b1 = go2->pos - go->pos;
		float r = go->scale.x;
		float h_2 = go2->scale.x;
		float l_2 = go2->scale.y;

		if (w0_b1.Dot(N) < 0) //flip normal based on ball dir
			N = -N;

		if ((w0_b1).Dot(N) < r + h_2 &&
			Math::FAbs((w0_b1).Dot(NP)) < l_2 &&
			N.Dot(go->vel) > 0) //prevents internal collision with ball and wall
		{
			return true;
		}
	}
	else if (go2->type == GameObject::GO_PILLAR)
	{
		Vector3 u = go->vel;
		Vector3 p2_p1 = go2->pos - go->pos;
		float r1 = go->scale.x;
		float r2 = go2->scale.x;

		if (p2_p1.Length() < r1 + r2 && 
			p2_p1.Dot(u) > 0) //prevents internal collision with ball and pillar
		{
			return true;
		}
	}
	return false;
}

void SceneCollision::CollisionResponse(GameObject* go, GameObject* go2)
{
	if (go2->type == GameObject::GO_BALL)
	{
		m1 = go->mass;
		m2 = go2->mass;
		u1 = go->vel;
		u2 = go2->vel;

		//Vector3 N = (go->pos - go2->pos).Normalized();
		//float mass = (2 * m2 / (m1 + m2));

		//v1 = u1 + mass * ((u2 - u1).Dot(N) * N);
		//v2 = u2 + mass * ((u1 - u2).Dot(N) * N);

		float dp = (u1 - u2).Dot(go->pos - go2->pos) / (go->pos - go2->pos).LengthSquared();
		v1 = u1 - 2 * m2 / (m1 + m2) * dp * (go->pos - go2->pos);
		v2 = u2 - 2 * m1 / (m1 + m2) * dp * (go2->pos - go->pos);
		go->vel = v1;
		go2->vel = v2;
	}
	else if (go2->type == GameObject::GO_WALL)
	{
		Vector3 N = go2->normal;
		Vector3 u = go->vel;
		go->vel = u - (2 * u.Dot(N)) * N * 0.7f;

		Vector3 w0_b1 = go2->pos - go->pos;
		if (w0_b1.Dot(N) > 0)
			N = -N;

		Vector3 projection = (w0_b1.Dot(N) / N.Dot(N)) * N; //find the projection of the direction vector on the normal
		if (projection.Length() < go2->scale.x + go->scale.x)
		{
			std::cout << "inside" << std::endl;
			Vector3 line = w0_b1 - projection;
			go->pos = go2->pos - line + N * (go2->scale.x + go->scale.x); //set the ball to the top of the wall in the direction of the wall's normal
		}
	}
	else if (go2->type == GameObject::GO_PILLAR)
	{
		Vector3 N = (go2->pos - go->pos).Normalized();
		Vector3 u = go->vel;
		go->vel = u - (2 * u.Dot(N)) * N;
	}
	else if (go2->type == GameObject::GO_SPRING)
	{
		if (go2->vel.IsZero()) {
			Vector3 N = go2->normal;
			Vector3 u = go->vel;
			go->vel = u - (2 * u.Dot(N)) * N * 0.9f;

			Vector3 w0_b1 = go2->pos - go->pos;
			if (w0_b1.Dot(N) > 0)
				N = -N;

			Vector3 projection = (w0_b1.Dot(N) / N.Dot(N)) * N; //find the projection of the direction vector on the normal
			if (projection.Length() < go2->scale.x + go->scale.x)
			{
				std::cout << "inside" << std::endl;
				Vector3 line = w0_b1 - projection;
				go->pos = go2->pos - line + N * (go2->scale.x + go->scale.x); //set the ball to the top of the wall in the direction of the wall's normal
			}
		}
		else {
			m1 = go->mass;
			m2 = go2->mass;
			u1 = go->vel;
			u2 = go2->vel;

			Vector3 N = (go->pos - go2->pos).Normalized();
			float mass = (2 * m2 / (m1 + m2));

			v1 = u1 + mass * ((u2 - u1).Dot(N) * N);
			go->vel = v1;
		}
	}
	else if (go2->type == GameObject::GO_FLIPPER)
	{
		if (go2->isLeft) {
			if (!leftFlipperOn)
			{
				Vector3 N = go2->normal;
				Vector3 u = go->vel;
				go->vel = u - (2 * u.Dot(N)) * N * 0.5f;

				Vector3 w0_b1 = go2->pos - go->pos;
				if (w0_b1.Dot(N) > 0)
					N = -N;

				Vector3 projection = (w0_b1.Dot(N) / N.Dot(N)) * N; //find the projection of the direction vector on the normal
				if (projection.Length() < go2->scale.x + go->scale.x)
				{
					std::cout << "inside" << std::endl;
					Vector3 line = w0_b1 - projection;
					go->pos = go2->pos - line + N * (go2->scale.x + go->scale.x); //set the ball to the top of the wall in the direction of the wall's normal
				}
			}
			else
			{
				Vector3 N = go2->normal;
				Vector3 u = go->vel;
				go->vel = u - (2 * u.Dot(N)) * N * (rotationLeft / 45.f);

				Vector3 w0_b1 = go2->pos - go->pos;
				if (w0_b1.Dot(N) > 0)
					N = -N;

				Vector3 projection = (w0_b1.Dot(N) / N.Dot(N)) * N; //find the projection of the direction vector on the normal
				if (projection.Length() < go2->scale.x + go->scale.x)
				{
					std::cout << "inside" << std::endl;
					Vector3 line = w0_b1 - projection;
					go->pos = go2->pos - line + N * (go2->scale.x + go->scale.x); //set the ball to the top of the wall in the direction of the wall's normal
				}
			}
		}
		else
		{
			if (!rightFlipperOn)
			{
				Vector3 N = go2->normal;
				Vector3 u = go->vel;
				go->vel = u - (2 * u.Dot(N)) * N;

				Vector3 w0_b1 = go2->pos - go->pos;
				if (w0_b1.Dot(N) > 0)
					N = -N;

				Vector3 projection = (w0_b1.Dot(N) / N.Dot(N)) * N; //find the projection of the direction vector on the normal
				if (projection.Length() < go2->scale.x + go->scale.x)
				{
					std::cout << "inside" << std::endl;
					Vector3 line = w0_b1 - projection;
					go->pos = go2->pos - line + N * (go2->scale.x + go->scale.x); //set the ball to the top of the wall in the direction of the wall's normal
				}
			}
			else
			{
				Vector3 N = go2->normal;
				Vector3 u = go->vel;
				go->vel = u - (2 * u.Dot(N)) * N * (rotationRight / 45.f);

				Vector3 w0_b1 = go2->pos - go->pos;
				if (w0_b1.Dot(N) > 0)
					N = -N;

				Vector3 projection = (w0_b1.Dot(N) / N.Dot(N)) * N; //find the projection of the direction vector on the normal
				if (projection.Length() < go2->scale.x + go->scale.x)
				{
					std::cout << "inside" << std::endl;
					Vector3 line = w0_b1 - projection;
					go->pos = go2->pos - line + N * (go2->scale.x + go->scale.x); //set the ball to the top of the wall in the direction of the wall's normal
				}
			}
		}
	}
}

void SceneCollision::CreateThickWall(Vector3 _scale, Vector3 _pos)
{
	//thick wall
	GameObject* go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->active = true;
	go->scale = _scale;
	go->normal.Set(0, 1, 0);
	go->pos = _pos;
	GameObject* go2 = FetchGO();
	go2->type = GameObject::GO_WALL;
	go2->active = true;
	go->scale = _scale;
	go2->normal.Set(1, 0, 0);
	go->pos = _pos;
	for (int i = -1; i <= 1; i += 2)
	{
		GameObject* go3 = FetchGO();
		go3->type = GameObject::GO_PILLAR;
		go3->active = true;
		go3->scale.Set(1,1,1);
		go3->pos = Vector3(go->pos.x + i * go->scale.y, go->pos.y - i * go->scale.y, 0);
	}
	for (int i = -1; i <= 1; i += 2)
	{
		GameObject* go4 = FetchGO();
		go4->type = GameObject::GO_PILLAR;
		go4->active = true;
		go4->scale.Set(1,1,1);
		go4->pos = Vector3(go->pos.x + i * go->scale.y, go->pos.y + i * go->scale.y, 0);
	}
}

void SceneCollision::Update(double dt)
{
	SceneBase::Update(dt);
	
	if(Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if(Application::IsKeyPressed('0'))
	{
		m_speed += 0.1f;
	}

	//Flippers
	if (Application::IsKeyPressed('A'))
	{
		leftFlipperOn = true;
	}
	if (leftFlipperOn)
	{
		rotationLeft += FLIPPER_UP_SPEED * dt;
		if (rotationLeft >= FLIPPER_MAX_ROTATION)
			leftFlipperTimer += dt;
		if (leftFlipperTimer > FLIPPER_COOLDOWN_TIMER)
		{
			leftFlipperOn = false;
			leftFlipperTimer = 0;
		}
	}
	else
	{
		rotationLeft -= FLIPPER_DOWN_SPEED * dt;
	}


	if (Application::IsKeyPressed('D'))
	{
		rightFlipperOn = true;
	}
	if (rightFlipperOn)
	{
		rotationRight -= FLIPPER_UP_SPEED * dt;
		if (rotationRight <= FLIPPER_MIN_ROTATION)
			rightFlipperTimer += dt;
		if (rightFlipperTimer > FLIPPER_COOLDOWN_TIMER)
		{
			rightFlipperOn = false;
			rightFlipperTimer = 0;
		}
	}
	else
	{
		rotationRight += FLIPPER_DOWN_SPEED * dt;
	}

	rotationLeft = Math::Clamp(rotationLeft, FLIPPER_MIN_ROTATION, FLIPPER_MAX_ROTATION);
	rotationRight = Math::Clamp(rotationRight, FLIPPER_MIN_ROTATION, FLIPPER_MAX_ROTATION);
	m_flipperLeft->normal = Vector3(cos(Math::DegreeToRadian(rotationLeft)), sin(Math::DegreeToRadian(rotationLeft)), 0);
	m_flipperRight->normal = Vector3(cos(Math::DegreeToRadian(rotationRight)), sin(Math::DegreeToRadian(rotationRight)), 0);
	//m_flipperLeft->vel = Vector3(0, 10, 0);
	
	//Spring
	if (Application::IsKeyPressed(VK_SPACE))
	{
		m_spring->pos.y -= .25f;
	}
	else
	{
		if (m_spring->pos.y >= m_worldHeight * 0.1f) {
			m_spring->vel.SetZero();
		}
		else {
			float pullDistance = m_worldHeight * 0.1f - m_spring->pos.y;
			m_spring->vel = Vector3(0, 50 * pullDistance, 0);
		}
	}
      
	m_spring->pos.y += m_spring->vel.y * dt * m_speed;
	m_spring->pos.y = Math::Clamp(m_spring->pos.y, m_worldHeight * 0.05f, m_worldHeight * 0.1f);

	//Mouse Section
	static bool bLButtonState = false;
	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;

		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float posX = x / w * m_worldWidth;
		float posY = (h - y) / h * m_worldHeight;

		if (!m_ghost->active)
		{
			m_ghost->active = true;
			m_ghost->pos = Vector3(posX, posY, 0);
		}
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;

		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float posX = x / w * m_worldWidth;
		float posY = (h - y) / h * m_worldHeight;

		Vector3 distanceFromGhostPos = m_ghost->pos - Vector3(posX, posY, 0);
		float scaleBasedOnDistance = distanceFromGhostPos.LengthSquared() * 0.005f;
		scaleBasedOnDistance = Math::Clamp(scaleBasedOnDistance, 2.f, 10.f);

		//Exercise 6: spawn small GO_BALL
		GameObject* smallBall = FetchGO();
		smallBall->type = GameObject::GO_BALL;
		smallBall->pos = m_ghost->pos;
		smallBall->vel = distanceFromGhostPos;
		smallBall->scale = Vector3(scaleBasedOnDistance, scaleBasedOnDistance, 1);
		smallBall->mass = smallBall->scale.x;
		m_ghost->active = false;
	}

	static bool bRButtonState = false;
	if(!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;

		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float posX = x / w * m_worldWidth;
		float posY = (h - y) / h * m_worldHeight;

		m_ghost->pos = Vector3(posX, posY, 0);
	}
	else if(bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;

		//Exercise 10: spawn large GO_BALL
		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float posX = x / w * m_worldWidth;
		float posY = (h - y) / h * m_worldHeight;

		//Exercise 6: spawn big GO_BALL
		GameObject* bigBall = FetchGO();
		bigBall->type = GameObject::GO_BALL;
		bigBall->pos = m_ghost->pos;
		bigBall->vel = m_ghost->pos - Vector3(posX, posY, 0);
		bigBall->scale = Vector3(3, 3, 3);
		bigBall->mass = 27;
	}   

	//Physics Simulation Section

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			if (go->type == GameObject::GO_BALL)
			{
				go->vel.y += m_gravity.y * dt * m_speed;
				go->pos += go->vel * dt * m_speed;
			}

			//Exercise 7: handle out of bound game objects
			if (go->pos.x + go->scale.x > m_worldWidth && go->vel.x > 0 ||
				go->pos.x - go->scale.x < 0 && go->vel.x < 0) {
				go->vel.x *= -1;
			}
			if (go->pos.y + go->scale.y > m_worldHeight && go->vel.y > 0 ||
				go->pos.y - go->scale.y < 0 && go->vel.y < 0) {
				go->vel.y *= -1;
			}

			if ((go->pos.x > m_worldWidth + go->scale.x || go->pos.x < 0 - go->scale.x) ||
				(go->pos.y > m_worldHeight + go->scale.y || go->pos.y < 0 - go->scale.y))
			{
				ReturnGO(go);
				break;
			}

			//Exercise 8a: handle collision between GO_BALL and GO_BALL using velocity swap
			for (std::vector<GameObject*>::iterator it2 = it + 1; it2 != m_goList.end(); ++it2)
			{
				GameObject* go2 = (GameObject*)*it2;
				if (go2->active)
				{
					GameObject* ball = go;
					GameObject* other = go2;
					if (ball->type != GameObject::GO_BALL)
					{
						if (other->type != GameObject::GO_BALL)
							continue;
						ball = go2;
						other = go;
					}
					if (CheckCollision(ball, other, dt)) //check for collision
					{
						CollisionResponse(ball, other); //collision response
					}
				}
			}
		}
	}
}


void SceneCollision::RenderGO(GameObject *go)
{
	switch(go->type)
	{
	case GameObject::GO_BALL:
		//Exercise 4: render a sphere using scale and pos
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		//Exercise 11: think of a way to give balls different colors
		break;
	case GameObject::GO_WALL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_PILLAR:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_SPRING:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_FLIPPER:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();
		break;
	}
}

void SceneCollision::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

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

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
		}
	}
	
	if (m_ghost->active)
		RenderGO(m_ghost);
	if (m_flipperLeft->active)
		RenderGO(m_flipperLeft);
	if (m_flipperRight->active)
		RenderGO(m_flipperRight);
	if (m_spring->active)
		RenderGO(m_spring);

	//On screen text

	//Exercise 5: Render m_objectCount

	//Exercise 8c: Render initial and final momentum

	std::ostringstream ss2;
	ss2.precision(3);
	ss2 << "Speed: " << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 6);
	
	std::ostringstream ss;
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);
	
	RenderTextOnScreen(meshList[GEO_TEXT], "Collision", Color(0, 1, 0), 3, 0, 0);
	
	ss.str("");
	ss << "Object Count: " << m_objectCount;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 9);

	ss.str("");
	ss << "Initial KE: " << 0.5f * m1 * u1.LengthSquared() + 0.5f * m2 * u2.LengthSquared();
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 12);

	ss.str("");
	ss << "Final KE: " << 0.5f * m1 * v1.LengthSquared() + 0.5f * m2 * v2.LengthSquared();
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 15);
	 
	ss.str("");
	ss << "Initial Momentum: " << u1 * m1 + u2 * m2;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 18);
	
	ss.str("");
	ss << "Final Momentum: " << v1 * m1 + v2 * m2;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 21);
}

void SceneCollision::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ghost)
	{
		delete m_ghost;
		m_ghost = NULL;
	}
}
