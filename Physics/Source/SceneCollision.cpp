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

	int n = 8;
	for (int i = 0; i < n; i++)
	{
		GameObject* go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->active = true;
		go->scale.Set(2, 12, 1);
		go->normal.Set(cos(Math::DegreeToRadian(i * 360.f / n)), sin(Math::DegreeToRadian(i * 360.f / n)), 0);
		go->pos = Vector3(m_worldWidth * 0.5, m_worldHeight * 0.5, 0) + (go->normal * (3.375 * n));
	}
	GameObject* go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->active = true;
	go->scale.Set(2, 12, 1);
	go->normal.Set(0, 1, 0);
	go->pos = Vector3(m_worldWidth * 0.5, m_worldHeight * 0.5, 0);
	for (int i = -1; i < 2; i += 2)
	{
		GameObject* go = FetchGO();
		go->type = GameObject::GO_PILLAR;
		go->active = true;
		go->scale.Set(.2, .2, .2);
		go->pos = Vector3(m_worldWidth * 0.5 + (i * 15), m_worldHeight * 0.5, 0);
	}
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
	if (go->type == GameObject::GO_BALL && go2->type == GameObject::GO_BALL)
	{
		Vector3 displacement = go2->pos - go->pos;
		float combinedRadii = go->scale.x + go2->scale.x;
		if ((displacement.LengthSquared() < combinedRadii * combinedRadii) && (go2->vel - go->vel).Dot(go2->pos - go->pos) < 0)
			return true;
	}
	else if (go->type == GameObject::GO_BALL && go2->type == GameObject::GO_WALL)
	{
		Vector3 N = go2->normal;
		Vector3 NP = Vector3(N.y, -N.x, 0);
		Vector3 w0_b1 = go2->pos - go->pos;
		float r = go->scale.x;
		float h_2 = go2->scale.x;
		float l_2 = go2->scale.y;

		if ((abs((w0_b1).Dot(N)) < r + h_2) &&
			abs((w0_b1).Dot(NP) < r + l_2))
		{
			std::cout << "COLLIDED" << std::endl;
			return true;
		}
	}
	else if (go->type == GameObject::GO_BALL && go2->type == GameObject::GO_PILLAR)
	{
		Vector3 u = go->vel;
		Vector3 p2_p1 = go2->pos - go->pos;
		float r1 = go->scale.x;
		float r2 = go2->scale.x;

		if ((abs(p2_p1.Length()) < r1 + r2) && ((p2_p1.Dot(u)) > 0))
		{
			std::cout << "COLLIDED" << std::endl;
			return true;
		}
	}
	return false;
}

void SceneCollision::CollisionResponse(GameObject* go, GameObject* go2)
{
	if (go->type == GameObject::GO_BALL && go2->type == GameObject::GO_BALL)
	{
		m1 = go->mass;
		m2 = go2->mass;
		u1 = go->vel;
		u2 = go2->vel;

		Vector3 N = (go->pos - go2->pos).Normalized();
		float mass = (2 * m2 / (m1 + m2));

		v1 = u1 + mass * ((u2 - u1).Dot(N) * N);
		v2 = u2 + mass * ((u1 - u2).Dot(N) * N);

		go->vel = v1; //d
		go2->vel = v2;
	}
	else if (go->type == GameObject::GO_BALL && go2->type == GameObject::GO_WALL)
	{
		Vector3 N = go2->normal;
		Vector3 u = go->vel;
		go->vel = u - (2 * u.Dot(N)) * N;
	}
	else if (go->type == GameObject::GO_BALL && go2->type == GameObject::GO_PILLAR)
	{
		Vector3 N = go2->normal;
		Vector3 u = go->vel;
		go->vel = u - (2 * u.Dot(N)) * N;
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
			go->pos += go->vel * dt * m_speed;

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
					if (CheckCollision(ball, other, dt))
					{
						CollisionResponse(ball, other);
						break;
					}
					//Exercise 8b: store values in auditing variables
				}
			}
			
			//Exercise 10: handle collision using momentum swap instead

			//Exercise 12: improve inner loop to prevent double collision

			//Exercise 13: improve collision detection algorithm [solution to be given later]
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
		modelStack.Rotate(90, 1, 0, 0);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CYLINDER], false);
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
