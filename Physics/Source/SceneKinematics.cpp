#include "SceneKinematics.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneKinematics::SceneKinematics()
{
}

SceneKinematics::~SceneKinematics()
{
}

void SceneKinematics::Init()
{
	SceneBase::Init();
	srand(time(NULL));
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;
	
	m_gravity.Set(0, -9.8f, 0); //init gravity as 9.8ms-2 downwards
	Math::InitRNG();

	m_ghost = new GameObject(GameObject::GO_BALL);

	/*for (int i = 0; i < 10; i++)
	{
		GameObject* newGO = new GameObject(GameObject::GO_BALL);
		m_goList.push_back(newGO);
	}*/
}

void SceneKinematics::Update(double dt)
{
	SceneBase::Update(dt);
	//Converting Viewport space to UI space
	double x, y;
	Application::GetCursorPos(&x, &y);
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();
	float posX = (x / w) * 133; //convert (0,800) to (0,80)
	float posY = ((h - y) / h * 100); //convert (600,0) to (0,60)

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Keyboard Section
	if(Application::IsKeyPressed('1'))
		glEnable(GL_CULL_FACE);
	if(Application::IsKeyPressed('2'))
		glDisable(GL_CULL_FACE);
	if(Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	if(Application::IsKeyPressed(VK_OEM_PLUS))
	{		
		//Exercise 6: adjust simulation speed
		m_speed += 10.f * dt;
	}
	if(Application::IsKeyPressed(VK_OEM_MINUS))
	{
		//Exercise 6: adjust simulation speed
		m_speed -= 10.f * dt;
		if (m_speed < 0)
			m_speed = 0;
	}
	if(Application::IsKeyPressed('C'))
	{
		//Exercise 9: clear screen
		for (auto go : m_goList)
		{
			go->active = false;
		}
	}
	if(Application::IsKeyPressed(VK_SPACE))
	{
		//Exercise 9: spawn balls
		for (int i = 0; i < 100; ++i)
		{
			SpawnBall(Vector3(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0.f, m_worldHeight), 0), Vector3(Math::RandFloatMinMax(-100.f, 100.f), Math::RandFloatMinMax(-100.f, 100.f), 0));
		}
	}
	if(Application::IsKeyPressed('V'))
	{
		//Exercise 9: spawn obstacles
		for (int i = 0; i < 100; ++i)
		{
			SpawnQuad(Vector3(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0.f, m_worldHeight), 0));
		}
	}

	static bool keyState = false;
	if (!keyState && Application::IsKeyPressed('Q'))
	{
		keyState = true;
		//Exercise 9: random spawn
		int random = rand() % 2;
		switch (random)
		{
		case 0:
			{
				GameObject* newGO = new GameObject(GameObject::GO_BALL);
				m_goList.push_back(newGO);
				newGO->active = true;
				newGO->type = GameObject::GO_BALL;
				newGO->pos = Vector3(posX, posY, 0);
				newGO->vel = Vector3(20, 20, 0);
				break;
			}
		case 1:
			{
				GameObject* newGO = new GameObject(GameObject::GO_CUBE);
				m_goList.push_back(newGO);
				newGO->active = true;
				newGO->type = GameObject::GO_CUBE;
				newGO->pos = Vector3(posX, posY, 0);
				newGO->vel = Vector3(20, 20, 0);
				break;
			}
		}
	}
	else if (keyState && !Application::IsKeyPressed('Q'))
	{
		keyState = false;
	}

	//Mouse Section
	static bool bLButtonState = false;
	//Exercise 10: ghost code here
	if(!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;

		//Exercise 10: spawn ghost ball
		if (!m_ghost->active)
		{
			m_ghost->active = true;

			m_ghost->pos = Vector3(x / w * m_worldWidth, (h - y) / h * m_worldHeight, 0);
		}
	}
	else if(bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;

		////Exercise 4: spawn ball
		//SpawnBall(Vector3(posX, posY, 0), Vector3(20, 20, 0));

		//Exercise 10: replace Exercise 4 code and use ghost to determine ball velocity
		GameObject* newGO = new GameObject(GameObject::GO_BALL);
		m_goList.push_back(newGO);
		newGO->active = true;
		newGO->type = GameObject::GO_BALL;
		newGO->scale = m_ghost->scale;
		newGO->pos = m_ghost->pos;
		newGO->vel = newGO->pos - Vector3(x / w * m_worldWidth, (h - y) / h * m_worldHeight, 0);
		m_ghost->vel = newGO->vel;
		m_ghost->active = false;
		m_timeGO = newGO;

		//Exercise 11: kinematics equation
		//v = u + a * t
		//t = (v - u ) / a

		//v * v = u * u + 2 * a * s
		//s = - (u * u) / (2 * a)
						
		//s = u * t + 0.5 * a * t * t
		//(0.5 * a) * t * t + (u) * t + (-s) = 0
	}
	
	static bool bRButtonState = false;
	if(!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;

		//Exercise 7: spawn obstacles using GO_CUBE
		SpawnQuad(Vector3(posX, posY, 0));
	}
	else if(bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;
	}

	//Physics Simulation Section
	fps = (float)(1.f / dt);

	//Exercise 11: update kinematics information
	for (auto& go : m_goList) 
	{
		if(go->active && go->type == GameObject::GO_BALL)
		{
			int w = Application::GetWindowWidth();
			int h = Application::GetWindowHeight();

			//Exercise 2: implement equation 1 & 2
			go->vel.y += m_gravity.y * dt * m_speed;
			go->pos += go->vel * dt * m_speed;

			//Exercise 12: replace Exercise 2 code and use average speed instead

			//Exercise 8: check collision with GO_CUBE
			for (auto& go2 : m_goList)
			{
				if (go2->active && go2->type == GameObject::GO_CUBE)
				{
					Vector3 displacement = go2->pos - go->pos;
					float combinedRadii = go->scale.x + go2->scale.x;
					if (displacement.LengthSquared() < combinedRadii * combinedRadii)
					{
						go2->active = false;
						go->active = false;
						break;
					}
				}
			}

			//Exercise 5: unspawn ball when outside window
			if ((go->pos.x > m_worldWidth + go->scale.x * 0.5f|| go->pos.x < 0 - go->scale.x * 0.5f) ||
				(go->pos.y > m_worldHeight + go->scale.y * 0.5f || go->pos.y < 0 - go->scale.y * 0.5f))
			{
				go->active = false;
			}
		}
	}
}

void SceneKinematics::RenderGO(GameObject *go)
{
	switch(go->type)
	{
	case GameObject::GO_BALL:
		//Exercise 3: render a sphere with radius 1
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_CUBE:
		//Exercise 7: render a cube with length 2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();
		break;
	}
}

void SceneKinematics::SpawnBall(Vector3 pos, Vector3 vel)
{
	GameObject* newGO = new GameObject(GameObject::GO_BALL);
	m_goList.push_back(newGO);
	newGO->active = true;
	newGO->type = GameObject::GO_BALL;
	newGO->pos = pos;
	newGO->vel = vel;
}

void SceneKinematics::SpawnQuad(Vector3 pos)
{
	GameObject* newGO = new GameObject(GameObject::GO_CUBE);
	m_goList.push_back(newGO);
	newGO->active = true;
	newGO->type = GameObject::GO_CUBE;
	newGO->pos = pos;
}

void SceneKinematics::Render()
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

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
		}
	}
	if(m_ghost->active)
	{
		RenderGO(m_ghost);
	}

	//On screen text
	std::ostringstream ss;
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);
	
	//Exercise 6: print simulation speed

	std::ostringstream sss;
	sss.precision(5);
	sss << "Speed: " << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], sss.str(), Color(0, 1, 0), 3, 0, 6);

	//Exercise 10: print m_ghost position and velocity information
	ss.str("");
	ss.precision(4);
	ss << "Gpos: " << m_ghost->pos;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 9);

	ss.str("");
	ss.precision(4);
	ss << "Gvel: " << m_ghost->vel;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 12);

	//Exercise 11: print kinematics information

	RenderTextOnScreen(meshList[GEO_TEXT], "Kinematics", Color(0, 1, 0), 3, 0, 0);
}

void SceneKinematics::Exit()
{
	// Cleanup VBO
	for(int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if(meshList[i])
			delete meshList[i];
	}
	glDeleteProgram(m_programID);
	glDeleteVertexArrays(1, &m_vertexArrayID);
	
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
