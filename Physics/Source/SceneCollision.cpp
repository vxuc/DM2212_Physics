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
	m_worldHeight = 200.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / (float)Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;
	
	Math::InitRNG();

	//Exercise 1: initialize m_objectCount
	m_objectCount = 0;

	m_ghost = new GameObject(GameObject::GO_BALL);

	m_gravity = Vector3(0, -50, 0);

	RenderMap();


	//spawn top balls
	for (int i = -2; i < 3; i++)
	{
		GameObject* go2 = FetchGO();
		go2->type = GameObject::GO_BALL;
		go2->scale.Set(3, 3, 3);
		go2->pos = Vector3(m_worldWidth * 0.5f + (i * 15), m_worldHeight * 0.99f, 0);
	}

	/*GameObject* go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->active = true;
	go->scale.Set(2, 2, 1);
	go->normal.Set(0, 1, 0);
	go->pos = Vector3(m_worldWidth * 0.25, m_worldHeight * 0.25, 0);*/


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
	if (go2->type == GameObject::GO_BALL || go2->type == GameObject::GO_SPHERE_ENERGY)
	{
		if (!go->allowCollision)
			return false;
		Vector3 displacement = go2->pos - go->pos;
		float combinedRadii = go->scale.x + go2->scale.x;
		if ((displacement.LengthSquared() < combinedRadii * combinedRadii) &&
			(go2->vel - go->vel).Dot(go2->pos - go->pos) < 0) //prevents internal collision with ball and ball
			return true;
	}
	else if (go2->type == GameObject::GO_BLACKHOLE && !go->isSucked)
	{
		Vector3 displacement = go2->pos - go->pos;
		float combinedRadii = go->scale.x + go2->scale.x;
		if ((displacement.LengthSquared() < combinedRadii * combinedRadii) &&
			(go2->vel - go->vel).Dot(go2->pos - go->pos) < 0) //prevents internal collision with ball and ball
		{
			go->allowCollision = false;
			return true;
		}
	}
	else if (go2->type == GameObject::GO_WALL || go2->type == GameObject::GO_SPRING || go2->type == GameObject::GO_FLIPPER || go2->type == GameObject::GO_WALL_ENERGY || 
		go2->type == GameObject::GO_BIG_BALL_POWERUP || go2->type == GameObject::GO_SPAWN_BALLS_POWERUP)
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
	else if (go2->type == GameObject::GO_WALL_BALLDROP)
	{
		Vector3 N = go2->normal;
		Vector3 NP = Vector3(N.y, -N.x, 0);
		Vector3 w0_b1 = go2->pos - go->pos;
		float r = go->scale.x;
		float h_2 = go2->scale.x;
		float l_2 = go2->scale.y;

		if (w0_b1.Dot(N) < 0) //flip normal based on ball dir
		{
			if (activateBallDrop)
				return false;
			else
				N = -N;
		}

		if ((w0_b1).Dot(N) < r + h_2 &&
			Math::FAbs((w0_b1).Dot(NP)) < l_2 &&
			N.Dot(go->vel) > 0) //prevents internal collision with ball and wall
		{
			return true;
		}
	}
	else if (go2->type == GameObject::GO_WALL_ONESIDED)
	{
		Vector3 N = go2->normal;
		Vector3 NP = Vector3(N.y, -N.x, 0);
		Vector3 w0_b1 = go2->pos - go->pos;
		float r = go->scale.x;
		float h_2 = go2->scale.x;
		float l_2 = go2->scale.y;

		if (w0_b1.Dot(N) < 0) //flip normal based on ball dir
			return false;

		if ((w0_b1).Dot(N) < r + h_2 &&
			Math::FAbs((w0_b1).Dot(NP)) < l_2 &&
			N.Dot(go->vel) > 0) //prevents internal collision with ball and wall
		{
			return true;
		}
	}
	else if (go2->type == GameObject::GO_PILLAR || go2->type == GameObject::GO_FLIPPER_PILLAR)
	{
		Vector3 u = go->vel;
		Vector3 p2_p1 = go2->pos - go->pos;
		float r1 = go->scale.x;
		float r2 = go2->scale.x;

		if (p2_p1.Length() < r1 + r2 && 
			p2_p1.Dot(u) > 0) //prevents internal collision with ball and pillar
		{
			return true;
			std::cout << "DAD" << std::endl;
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
	else if (go2->type == GameObject::GO_SPHERE_ENERGY)
	{
		Vector3 N = (go->pos - go2->pos).Normalized();
		Vector3 u = go->vel;
		go->vel = u - (2 * u.Dot(N)) * N * 1.5f;

		score += u.Length();
	}
	else if (go2->type == GameObject::GO_BLACKHOLE)
	{
		Vector3 w0_b1 = go2->pos - go->pos;
		go->vel = w0_b1 * 3;
		go->isSucked = true;
	}
	else if (go2->type == GameObject::GO_BIG_BALL_POWERUP)
	{
		go->scale.Set(8, 8, 8);
		go->mass = go->scale.x * 3;
		go2->active = false;
	}
	else if (go2->type == GameObject::GO_SPAWN_BALLS_POWERUP)
	{
		activateBallDrop = true;
		go2->active = false;
	}
	else if (go2->type == GameObject::GO_WALL || go2->type == GameObject::GO_WALL_BALLDROP)
	{
		Vector3 N = go2->normal;
		Vector3 u = go->vel;
		go->vel = u - (2 * u.Dot(N)) * N * 0.9f;

		Vector3 w0_b1 = go2->pos - go->pos;
		if (w0_b1.Dot(N) > 0)
			N = -N;

		Vector3 projection = (w0_b1.Dot(N) / N.Dot(N)) * N; //find the projection of the direction vector on the normal
		if (projection.Length() < go2->scale.x + go->scale.x)
		{
			Vector3 line = w0_b1 - projection;
			go->pos = go2->pos - line + N * (go2->scale.x + go->scale.x); //set the ball to the top of the wall in the direction of the wall's normal
		}
	}
	else if (go2->type == GameObject::GO_WALL_ONESIDED)
	{
		Vector3 N = go2->normal;
		Vector3 u = go->vel;
		go->vel = u - (2 * u.Dot(N)) * N * 2.f;

		Vector3 w0_b1 = go2->pos - go->pos;
		if (w0_b1.Dot(N) > 0)
			N = -N;

		Vector3 projection = (w0_b1.Dot(N) / N.Dot(N)) * N; //find the projection of the direction vector on the normal
		if (projection.Length() < go2->scale.x + go->scale.x)
		{
			Vector3 line = w0_b1 - projection;
			go->pos = go2->pos - line + N * (go2->scale.x + go->scale.x); //set the ball to the top of the wall in the direction of the wall's normal
		}
	}
	else if (go2->type == GameObject::GO_WALL_ENERGY)
	{
		if (go->scale.x > 3)
		{
			go->scale.x -= 0.01f * go->vel.Length();
			go->scale.y -= 0.01f * go->vel.Length();
			go->scale.x = Math::Clamp(go->scale.x, 3.f, 8.f);
			go->scale.y = Math::Clamp(go->scale.y, 3.f, 8.f);
		}
		Vector3 N = go2->normal;
		Vector3 u = go->vel;
		go->vel = u - (2 * u.Dot(N)) * N * 1.5f;

		Vector3 w0_b1 = go2->pos - go->pos;
		if (w0_b1.Dot(N) > 0)
			N = -N;

		Vector3 projection = (w0_b1.Dot(N) / N.Dot(N)) * N; //find the projection of the direction vector on the normal
		if (projection.Length() < go2->scale.x + go->scale.x)
		{
			Vector3 line = w0_b1 - projection;
			go->pos = go2->pos - line + N * (go2->scale.x + go->scale.x); //set the ball to the top of the wall in the direction of the wall's normal
		}

		score += u.Length();
	}
	else if (go2->type == GameObject::GO_PILLAR)
	{
		if (go->scale.x > 3)
		{
			go->scale.x -= 0.01f * go->vel.Length();
			go->scale.y -= 0.01f * go->vel.Length();
			go->scale.x = Math::Clamp(go->scale.x, 3.f, 8.f);
			go->scale.y = Math::Clamp(go->scale.y, 3.f, 8.f);
		}
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
				go->vel = u - (2 * u.Dot(N)) * N;

				Vector3 w0_b1 = go2->pos - go->pos;
				if (w0_b1.Dot(N) > 0)
					N = -N;

				Vector3 projection = (w0_b1.Dot(N) / N.Dot(N)) * N; //find the projection of the direction vector on the normal
				if (projection.Length() < go2->scale.x + go->scale.x)
				{
					Vector3 line = w0_b1 - projection;
					go->pos = go2->pos - line + N * (go2->scale.x + go->scale.x); //set the ball to the top of the wall in the direction of the wall's normal
				}
			}
			else
			{
				float angularVelocity = 2 * ((Math::PI) / 5.f);
				Vector3 N = go2->normal;
				Vector3 u = go->vel;
				go->vel = u - (2 * u.Dot(N)) * N * (rotationLeft / 45) * angularVelocity;
			}

			//Vector3 wN = go2->normal;
			//Vector3 w0_b1 = go2->pos - go->pos;
			//if (w0_b1.Dot(wN) > 0)
			//	wN = -wN;

			//Vector3 projection = (w0_b1.Dot(wN) / wN.Dot(wN)) * wN; //find the projection of the direction vector on the normal
			//Vector3 line;
			//line.SetZero();
			//if (projection.Length() < go2->scale.x + go->scale.x)
			//{
			//	line = w0_b1 - projection;
			//}

			//m1 = go->mass;
			//m2 = go2->mass;
			//u1 = go->vel;
			//go2->vel = wN * line.Length() * m_flipperLeft->angularVelocity;
			//u2 = go2->vel;
			//

			//if (u2.IsZero())
			//{
			//	Vector3 N = go2->normal;
			//	Vector3 u = go->vel;
			//	go->vel = u - (2 * u.Dot(N)) * N;
			//	return;
			//}

			//Vector3 N = (go->pos - go2->pos).Normalized();
			//float mass = (2 * m2 / (m1 + m2));

			//v1 = u1 + mass * ((u2 - u1).Dot(N) * N);
			//go->vel = v1;
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
					Vector3 line = w0_b1 - projection;
					go->pos = go2->pos - line + N * (go2->scale.x + go->scale.x); //set the ball to the top of the wall in the direction of the wall's normal
				}
			}
			else
			{
				float angularVelocity = 2 * ((Math::PI) / 5.f);
				Vector3 N = go2->normal;
				Vector3 u = go->vel;
				go->vel = u - (2 * u.Dot(N)) * N * (rotationLeft / 45) * angularVelocity;
			}
		}
	}
	else if (go2->type == GameObject::GO_FLIPPER_PILLAR)
	{
		if (go2->isLeft) {
			if (!leftFlipperOn)
			{
				Vector3 N = (go2->pos - go->pos).Normalized();
				Vector3 u = go->vel;
				go->vel = u - (2 * u.Dot(N)) * N;
			}
			else
			{
				float angularVelocity = 2 * ((Math::PI) / 1.f);
				Vector3 N = (go2->pos - go->pos).Normalized();
				Vector3 u = go->vel;
				go->vel = u - (2 * u.Dot(N)) * N * (rotationRight / 45) * angularVelocity;
			}
		}
		else
		{
			if (!rightFlipperOn)
			{
				Vector3 N = (go2->pos - go->pos).Normalized();
				Vector3 u = go->vel;
				go->vel = u - (2 * u.Dot(N)) * N;
			}
			else
			{
				float angularVelocity = 2 * ((Math::PI) / 1.f);
				Vector3 N = (go2->pos - go->pos).Normalized();
				Vector3 u = go->vel;
				go->vel = u - (2 * u.Dot(N)) * N * (rotationRight / 45) * angularVelocity;
			}
		}
		
	}
}

void SceneCollision::CreateThickWall(Vector3 _scale, Vector3 _pos, bool isElastic)
{
	////thick wall
	//GameObject* go = FetchGO();
	//go->type = GameObject::GO_WALL;
	//go->scale = _scale;
	//go->normal.Set(0, 1, 0);
	//go->pos = _pos;
	//GameObject* go2 = FetchGO();
	//go2->type = GameObject::GO_WALL;
	//go->scale = _scale;
	//go2->normal.Set(1, 0, 0);
	//go->pos = _pos;
	//for (int i = -1; i <= 1; i += 2)
	//{
	//	GameObject* go3 = FetchGO();
	//	go3->type = GameObject::GO_PILLAR;
	//	go3->active = true;
	//	go3->scale.Set(1,1,1);
	//	go3->pos = Vector3(go->pos.x + i * go->scale.y, go->pos.y - i * go->scale.y, 0);
	//}
	//for (int i = -1; i <= 1; i += 2)
	//{
	//	GameObject* go4 = FetchGO();
	//	go4->type = GameObject::GO_PILLAR;
	//	go4->active = true;
	//	go4->scale.Set(1,1,1);
	//	go4->pos = Vector3(go->pos.x + i * go->scale.y, go->pos.y + i * go->scale.y, 0);
	//}

	//Top and bottom
	GameObject* go = FetchGO();

	go->scale = _scale;
	go->normal.Set(0, 1, 0);
	go->pos = _pos;

	//Left and Right
	GameObject* go2 = FetchGO();
	go2->scale = Vector3(_scale.y, _scale.x, 1);
	go2->normal.Set(1, 0, 0);
	go2->pos = _pos;

	if (isElastic)
	{
		go->type = GameObject::GO_WALL_ENERGY;
		go2->type = GameObject::GO_WALL_ENERGY;
	}
	else
	{
		go->type = GameObject::GO_WALL;
		go2->type = GameObject::GO_WALL;
	}
	for (int i = -1; i <= 1; i += 2)
	{
		GameObject* go3 = FetchGO();
		go3->type = GameObject::GO_PILLAR;
		go3->active = true;
		go3->scale.Set(1,1,1);
		go3->pos = Vector3(go->pos.x + i * go->scale.y - i * go3->scale.x, go->pos.y - i * go->scale.x + i * go3->scale.y, 0);
	}
	for (int i = -1; i <= 1; i += 2)
	{
		GameObject* go4 = FetchGO();
		go4->type = GameObject::GO_PILLAR;
		go4->active = true;
		go4->scale.Set(1,1,1);
		go4->pos = Vector3(go->pos.x + i * go->scale.y - i * go4->scale.x, go->pos.y + i * go->scale.x - i * go4->scale.y, 0);
	}
}

void SceneCollision::RenderMap()
{
	//game objects
	m_flipperLeft = new GameObject(GameObject::GO_FLIPPER);
	m_flipperLeft->active = true;
	m_flipperLeft->isLeft = true;
	m_flipperLeft->scale = Vector3(2, 10, 1);
	//m_flipperLeft->mass = 100;
	m_flipperLeft->normal = Vector3(cos(Math::DegreeToRadian(75.f)), sin(Math::DegreeToRadian(75.f)), 0);
	m_flipperLeft->pos = Vector3(m_worldWidth * 0.5 - m_flipperLeft->scale.y * 1.5, m_worldHeight * 0.1 - m_flipperLeft->scale.x * 1.5f, 0);
	m_goList.push_back(m_flipperLeft);

	m_flipperRight = new GameObject(GameObject::GO_FLIPPER);
	m_flipperRight->active = true;
	m_flipperRight->isLeft = false;
	//m_flipperRight->mass = 100;
	m_flipperRight->scale = Vector3(2, 10, 1);
	m_flipperRight->normal = Vector3(cos(Math::DegreeToRadian(105.f)), sin(Math::DegreeToRadian(105.f)), 0);
	m_flipperRight->pos = Vector3(m_worldWidth * 0.5 + m_flipperRight->scale.y * 1.5, m_worldHeight * 0.1 - m_flipperRight->scale.x * 1.5f, 0);
	m_goList.push_back(m_flipperRight);

	m_flipperLeftPillar = new GameObject(GameObject::GO_FLIPPER_PILLAR);
	m_flipperLeftPillar->active = true;
	m_flipperLeftPillar->isLeft = true;
	//m_flipperLeftPillar->mass = 100;
	m_flipperLeftPillar->scale = Vector3(2, 2, 1);
	m_flipperLeftPillar->pos = m_flipperLeft->pos + Vector3(cos(Math::DegreeToRadian(rotationLeft - 90.f) * m_flipperLeft->scale.y), sin(Math::DegreeToRadian(rotationLeft - 90.f) * m_flipperLeft->scale.y), 0);
	m_goList.push_back(m_flipperLeftPillar);

	m_flipperRightPillar = new GameObject(GameObject::GO_FLIPPER_PILLAR);
	m_flipperRightPillar->active = true;
	m_flipperRightPillar->scale = Vector3(2, 2, 1);
	m_flipperRightPillar->isLeft = false;
	//m_flipperRightPillar->mass = 100;
	m_flipperRightPillar->pos = m_flipperRight->pos + Vector3(cos(Math::DegreeToRadian(rotationRight + 90.f) * m_flipperRight->scale.y), sin(Math::DegreeToRadian(rotationRight + 90.f) * m_flipperRight->scale.y), 0);
	m_goList.push_back(m_flipperRightPillar);

	m_spring = new GameObject(GameObject::GO_SPRING);
	m_spring->active = true;
	m_spring->scale = Vector3(5, 2, 1);
	m_spring->normal = Vector3(0, 1, 0);
	m_spring->pos = Vector3(m_worldWidth * 0.785, m_worldHeight * 0.1, 0);
	m_goList.push_back(m_spring);

	//starting ball
	GameObject* ball = FetchGO();
	ball->scale = Vector3(3, 3, 3);
	ball->mass = 2;
	ball->pos = Vector3(m_worldWidth * 0.785, m_worldHeight * 0.1, 0) + m_spring->normal * (m_spring->scale.x + ball->scale.x + 5);
	
	for (int i = 1; i < 5; i++)
	{
		CreateThickWall(Vector3(3, 1, 1), Vector3(m_worldWidth * 0.43f + (i * 10), m_worldHeight * 0.275f, 0), false);
	}
	CreateThickWall(Vector3(6, 6, 1), Vector3(m_worldWidth * 0.35f, m_worldHeight * 0.375f, 0), true);
	CreateThickWall(Vector3(6, 6, 1), Vector3(m_worldWidth * 0.65f, m_worldHeight * 0.375f, 0), true);

	//energy spheres
	for (int i = 1; i < 6; i++)
	{
		GameObject* sphere = FetchGO();
		sphere->type = GameObject::GO_SPHERE_ENERGY;
		sphere->scale.Set(5, 5, 5);
		sphere->pos = Vector3(m_worldWidth * 0.3f + (i * 25), m_worldHeight * 0.8f, 0);
	}

	//black hole
	GameObject* go = FetchGO();
	go->type = GameObject::GO_BLACKHOLE;
	go->scale.Set(10, 10, 1);
	go->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight * 0.65f, 0);

	//right walls
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->scale.Set(2.5, 80, 1);
	go->normal.Set(1, 0, 0);
	go->pos = Vector3(m_worldWidth * 0.75 + go->scale.x * go->scale.x, go->scale.y, 0);
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->scale.Set(2.5, 85, 1);
	go->normal.Set(1, 0, 0);
	go->pos = Vector3(m_worldWidth * 0.82 - go->scale.x * go->scale.x, go->scale.y, 0);
	
	GameObject* goo = FetchGO();
	goo->type = GameObject::GO_WALL_ONESIDED;
	goo->scale.Set(2.5, 5, 1);
	goo->normal.Set(0, -1, 0);
	goo->pos = Vector3(m_worldWidth * 0.8 - goo->scale.x * goo->scale.x, m_worldHeight * 0.7875, 0);

	//left wall
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->active = true;
	go->scale.Set(2.5, 42, 1);
	go->normal.Set(1, 0, 0);
	go->pos = Vector3(m_worldWidth * 0.25 - go->scale.x * go->scale.x, go->scale.y, 0);
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->active = true;
	go->scale.Set(2.5, 25.75, 1);
	go->normal.Set(1, 0, 0);
	go->pos = Vector3(m_worldWidth * 0.2475 - go->scale.x * go->scale.x, m_worldHeight * 0.725, 0);
	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->active = true;
	go->scale.Set(3.2, 3.2, 1);
	go->normal.Set(1, 0, 0);
	go->pos = Vector3(m_worldWidth * 0.229, m_worldHeight * 0.61, 0);

	//bottom left walls
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->active = true;
	go->scale.Set(9.6, 39.75, 1);
	go->normal = Vector3(0, 1, 0);
	go->pos = Vector3(m_worldWidth * 0.5 - go->scale.y - (m_flipperLeft->scale.y * 1.5), go->scale.x, 0);
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->active = true;
	go->scale.Set(10, 39.75, 1);
	go->normal = Vector3(cos(Math::DegreeToRadian(75.f)), sin(Math::DegreeToRadian(75.f)), 0);
	go->pos = Vector3(m_worldWidth * 0.5 - go->scale.y * 1.05 - (m_flipperLeft->scale.y * 1.5), go->scale.x * 2, 0);

	//bottom right walls
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->active = true;
	go->scale.Set(9.6, 39.75, 1);
	go->normal = Vector3(0, 1, 0);
	go->pos = Vector3(m_worldWidth * 0.5 + go->scale.y + (m_flipperLeft->scale.y * 1.5), go->scale.x, 0);
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->active = true;
	go->scale.Set(10, 39.75, 1);
	go->normal = Vector3(cos(Math::DegreeToRadian(105.f)), sin(Math::DegreeToRadian(105.f)), 0);
	go->pos = Vector3(m_worldWidth * 0.5 + go->scale.y * 1.05 + (m_flipperLeft->scale.y * 1.5), go->scale.x * 2, 0);

	//top walls
	go = FetchGO();
	go->type = GameObject::GO_WALL_BALLDROP;
	go->active = true;
	go->scale.Set(2.5, 85, 1);
	go->normal = Vector3(0, 1, 0);
	go->pos = Vector3(m_worldWidth * 0.5 + go->scale.x * 2, m_worldHeight * 0.945, 0);


	//top corner curved walls
	int n = 8;
	for (int i = 0; i < n; i++)
	{
		GameObject* go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->active = true;
		go->scale.Set(2.5, 3, 1);
		go->normal.Set(cos(Math::DegreeToRadian(100 * i / n)), sin(Math::DegreeToRadian(100 * i / n)), 0);
		go->pos = Vector3(m_worldWidth * 0.75715, m_worldHeight * 0.865, 0) + (go->normal * (2 * n));
	}

	//for (int i = 0; i < n; i++)
	//{
	//	GameObject* go = FetchGO();
	//	go->type = GameObject::GO_WALL;
	//	go->active = true;
	//	go->scale.Set(2.5, 3, 1);
	//	go->normal.Set(cos(Math::DegreeToRadian(92.25 + 100 * i / n)), sin(Math::DegreeToRadian(92.25 + 100 * i / n)), 0);
	//	go->pos = Vector3(m_worldWidth * 0.2775, m_worldHeight * 0.864, 0) + (go->normal * (2 * n));
	//}

	//left huge curved wall
	for (int i = 3; i < 12; i++)
	{
		GameObject* go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->active = true;
		go->scale.Set(2.5, 4, 1);
		go->normal.Set(cos(Math::DegreeToRadian(160 * i / n)), sin(Math::DegreeToRadian(160 * i / n)), 0);
		go->pos = Vector3(m_worldWidth * 0.2025, m_worldHeight * 0.781, 0) + (go->normal * (2 * n));
	}
	for (int i = 3; i < 12; i++)
	{
		GameObject* go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->active = true;
		go->scale.Set(2.5, 8, 1);
		go->normal.Set(cos(Math::DegreeToRadian(160 * i / n)), sin(Math::DegreeToRadian(160 * i / n)), 0);
		go->pos = Vector3(m_worldWidth * 0.2025, m_worldHeight * 0.79, 0) + (go->normal * (5 * n));
	}
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->scale.Set(2.5, 15, 1);
	go->normal = Vector3(cos(Math::DegreeToRadian(220)), sin(Math::DegreeToRadian(220)), 0);
	go->pos = Vector3(m_worldWidth * 0.2, m_worldHeight * 0.6625, 0);
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->scale.Set(2.5, 30, 1);
	go->normal = Vector3(cos(Math::DegreeToRadian(220)), sin(Math::DegreeToRadian(220)), 0);
	go->pos = Vector3(m_worldWidth * 0.18, m_worldHeight * 0.525, 0);
}

void SceneCollision::SpawnPowerup(double dt)
{
	powerUpSpawnTime += dt;
	if (powerUpSpawnTime > 10)
	{
		int random = Math::RandIntMinMax(1, 2);
		GameObject* go = FetchGO();
		switch (random)
		{
		case 1:
			go->type = GameObject::GO_BIG_BALL_POWERUP;
			break;
		case 2:
			go->type = GameObject::GO_SPAWN_BALLS_POWERUP;
			break;
		}
		go->active = true;
		go->scale.Set(2.5, 2.5, 1);
		go->normal.Set(cos(Math::DegreeToRadian(45)), sin(Math::DegreeToRadian(45)), 0);
		go->pos = Vector3(Math::RandFloatMinMax(m_worldWidth * 0.3, m_worldWidth * 0.6), m_worldWidth * 0.3, 0);
		powerUpSpawnTime = 0;
	}
}

void SceneCollision::Update(double dt)
{
	SceneBase::Update(dt);
	SpawnPowerup(dt);
	
	if(Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if(Application::IsKeyPressed('0'))
	{
		m_speed += 0.1f;
	}

	if (activateBallDrop)
	{
		ballDropTimer += dt;
		if (ballDropTimer > 2)
		{
			activateBallDrop = false;
			while (true)
			{
				break;
			}
			//spawn top balls
			for (int i = -2; i < 3; i++)
			{
				GameObject* go2 = FetchGO();
				go2->type = GameObject::GO_BALL;
				go2->scale.Set(3, 3, 3);
				go2->pos = Vector3(m_worldWidth * 0.5f + (i * 15), m_worldHeight * 0.99f, 0);
			}
		}
	}
	else
		ballDropTimer = 0;

	//Flippers
	if (Application::IsKeyPressed('A'))
	{
		//m_flipperLeft->angularVelocity += 2 * ((Math::PI) / 1.0f);
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

	//m_flipperLeft->angularVelocity = Math::Clamp(m_flipperLeft->angularVelocity, -5.0f, 5.0f);
	//rotationLeft = m_flipperLeft->angularVelocity * dt;
	//std::cout << rotationLeft << std::endl;
	//m_flipperLeft->normal.Set(m_flipperLeft->normal.x * cosf(rotationLeft) - m_flipperLeft->normal.y * sinf(rotationLeft),
	//	m_flipperLeft->normal.x * sinf(rotationLeft) + m_flipperLeft->normal.y * cosf(rotationLeft)); //turn a vector by angle

	//m_flipperLeft->normal.x = Math::Clamp(m_flipperLeft->normal.x, cos(Math::DegreeToRadian(FLIPPER_MIN_ROTATION)), cos(Math::DegreeToRadian(FLIPPER_MAX_ROTATION)));
	//m_flipperLeft->normal.y = Math::Clamp(m_flipperLeft->normal.y, sin(Math::DegreeToRadian(FLIPPER_MIN_ROTATION)), sin(Math::DegreeToRadian(FLIPPER_MAX_ROTATION)));
	
	std::cout << rotationLeft << std::endl;
	rotationLeft = Math::Clamp(rotationLeft, FLIPPER_MIN_ROTATION, FLIPPER_MAX_ROTATION);
	std::cout << rotationLeft << std::endl;
	rotationRight = Math::Clamp(rotationRight, FLIPPER_MIN_ROTATION, FLIPPER_MAX_ROTATION);	
	m_flipperLeft->normal = Vector3(cos(Math::DegreeToRadian(rotationLeft)), sin(Math::DegreeToRadian(rotationLeft)), 0);
	m_flipperRight->normal = Vector3(cos(Math::DegreeToRadian(rotationRight)), sin(Math::DegreeToRadian(rotationRight)), 0);
	m_flipperLeftPillar->pos = m_flipperLeft->pos + Vector3(cos(Math::DegreeToRadian(rotationLeft - 90)) * m_flipperLeft->scale.y, sin(Math::DegreeToRadian(rotationLeft - 90)) * m_flipperLeft->scale.y, 0);
	m_flipperRightPillar->pos = m_flipperRight->pos + Vector3(cos(Math::DegreeToRadian(rotationRight + 90)) * m_flipperRight->scale.y, sin(Math::DegreeToRadian(rotationRight + 90)) * m_flipperRight->scale.y, 0);

	//m_flipperLeft->vel = Vector3(0, 10, 0);
	
	//Spring
	if (Application::IsKeyPressed(VK_SPACE))
	{
		m_spring->pos.y -= .25f;
	}
	else
	{
		if (m_spring->pos.y >= m_worldHeight * 0.05f) {
			m_spring->vel.SetZero();
		}
		else {
			float pullDistance = m_worldHeight * 0.05f - m_spring->pos.y;
			m_spring->vel = Vector3(0, 50 * pullDistance, 0);
		}
	}
      
	m_spring->pos.y += m_spring->vel.y * dt * m_speed;
	m_spring->pos.y = Math::Clamp(m_spring->pos.y, m_spring->scale.x, m_worldHeight * 0.05f);

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
				if (go->vel.LengthSquared() >= BALL_MAX_SPEED * BALL_MAX_SPEED)
				{
					go->vel = go->vel.Normalized() * BALL_MAX_SPEED;
				}
				go->pos += go->vel * dt * m_speed;

				if (go->isSucked)
				{
					go->timer += dt;
					for (GameObject* blackHole : m_goList)
					{
						if (blackHole->type != GameObject::GO_BLACKHOLE)
							continue;
						
						Vector3 displacement = blackHole->pos - go->pos;
						float combinedRadii = go->scale.x + blackHole->scale.x / 3;
						if ((displacement.LengthSquared() < combinedRadii * combinedRadii))
						{
							go->pos = blackHole->pos;
						}
					}
				}
				else
				{
					for (GameObject* blackHole : m_goList)
					{
						if (blackHole->type != GameObject::GO_BLACKHOLE)
							continue;

						Vector3 displacement = blackHole->pos - go->pos;
						float combinedRadii = go->scale.x + blackHole->scale.x * 3;
						if ((displacement.LengthSquared() < combinedRadii * combinedRadii))
						{
							std::cout << "pulling" << std::endl;
							Vector3 pull = (blackHole->pos - go->pos).Normalized() * 3;
							go->vel += pull;
						}
					}
				}
			}

			//Exercise 7: handle out of bound game objects
			/*if (go->pos.x + go->scale.x > m_worldWidth && go->vel.x > 0 ||
				go->pos.x - go->scale.x < 0 && go->vel.x < 0) {
				go->vel.x *= -1;
			}
			if (go->pos.y + go->scale.y > m_worldHeight && go->vel.y > 0 ||
				go->pos.y - go->scale.y < 0 && go->vel.y < 0) {
				go->vel.y *= -1;
			}*/

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

			if (go->type == GameObject::GO_BLACKHOLE)
			{
				for (GameObject* go2 : m_goList)
				{
					if (go2->type == GameObject::GO_BALL && go2->timer > 2)
					{
						if (go->pos.x == go2->pos.x && go->pos.y == go2->pos.y)
						{
							go2->vel = Vector3(Math::RandFloatMinMax(-1, 1) * 2000, Math::RandFloatMinMax(-1, 1) * 2000, 0);
							go2->isSucked = false;
							go2->allowCollision = true;
							go2->timer = 0;
						}
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
		meshList[GEO_BALL]->material.kAmbient.Set(0.69f, 0.60f, 0.84f);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();
		//Exercise 11: think of a way to give balls different colors
		break;
	case GameObject::GO_WALL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_CUBE]->material.kAmbient.Set(0.46f, 0.61f, 0.79f);
		RenderMesh(meshList[GEO_CUBE], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_PILLAR:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_BALL]->material.kAmbient.Set(0.37f, 1.f, 0.71f);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_SPRING:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_CUBE]->material.kAmbient.Set(0.69f, 0.60f, 0.84f);
		RenderMesh(meshList[GEO_CUBE], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_FLIPPER:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, -1);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_CUBE]->material.kAmbient.Set(0.41f, 0.37f, 1.f);
		RenderMesh(meshList[GEO_CUBE], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_WALL_ENERGY:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_CUBE]->material.kAmbient.Set(0.37f, 1.f, 0.71f);
		RenderMesh(meshList[GEO_CUBE], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_SPHERE_ENERGY:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_BALL]->material.kAmbient.Set(1.f, 1.f, 0.56f);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_WALL_ONESIDED:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_CUBE]->material.kAmbient.Set(0.46f, 0.61f, 0.79f);
		RenderMesh(meshList[GEO_CUBE], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_BLACKHOLE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, 10);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_BALL]->material.kAmbient.Set(1.f, 0.58f, 0.67);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_BIG_BALL_POWERUP:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_CUBE]->material.kAmbient.Set(1.f, 0.41f, 0.37f);
		RenderMesh(meshList[GEO_CUBE], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_WALL_BALLDROP:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_CUBE]->material.kAmbient.Set(0.46f, 0.61f, 0.79f);
		RenderMesh(meshList[GEO_CUBE], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_SPAWN_BALLS_POWERUP:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_CUBE]->material.kAmbient.Set(0.37f, 0.96f, 1.f);
		RenderMesh(meshList[GEO_CUBE], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_FLIPPER_PILLAR:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);		
		meshList[GEO_BALL]->material.kAmbient.Set(0.41f, 0.37f, 1.f);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();
		break;
	}
}

void SceneCollision::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Calculating aspect ratio
	m_worldHeight = 200.f;
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
	ss2 << "Score: " << (int)score;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 6);
	
	std::ostringstream ss;
	ss.precision(5);
	ss << "FPS: " << fps;  
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);
	
	RenderTextOnScreen(meshList[GEO_TEXT], "Collision", Color(0, 1, 0), 3, 0, 0);
	
	ss.str("");
	ss << "Object Count: " << m_objectCount;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 9);
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
