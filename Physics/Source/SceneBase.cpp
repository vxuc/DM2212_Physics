#include "SceneBase.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>
float SceneBase::flashingTime = 0;
bool SceneBase::restartGame = false;
bool SceneBase::resetGame = false;
int SceneBase::score = 0;
MISSION SceneBase::missionType;
PLAYER_SHIP_COLOUR SceneBase::shipColor = SHIP_BLUE;
MENU_TYPE SceneBase::menuType = M_MAIN;

SceneBase::SceneBase()
{
}

SceneBase::~SceneBase()
{
}

void SceneBase::Init()
{
	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 
	
	glEnable(GL_CULL_FACE);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	m_programID = LoadShaders( "Shader//comg.vertexshader", "Shader//comg.fragmentshader" );
	
	// Get a handle for our uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	//m_parameters[U_MODEL] = glGetUniformLocation(m_programID, "M");
	//m_parameters[U_VIEW] = glGetUniformLocation(m_programID, "V");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	
	// Use our shader
	glUseProgram(m_programID);

	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].position.Set(0, 20, 0);
	lights[0].color.Set(1, 1, 1);
	lights[0].power = 1;
	lights[0].kC = 1.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0].cosInner = cos(Math::DegreeToRadian(30));
	lights[0].exponent = 3.f;
	lights[0].spotDirection.Set(0.f, 1.f, 0.f);
	
	glUniform1i(m_parameters[U_NUMLIGHTS], 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], lights[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &lights[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], lights[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], lights[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], lights[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], lights[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], lights[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], lights[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], lights[0].exponent);

	camera.Init(Vector3(0, 0, 1), Vector3(0, 0, 0), Vector3(0, 1, 0));
	camera2.Init(Vector3(0, 0, 1), Vector3(0, 0, 0), Vector3(0, 1, 0));
	cam2 = false;

	for(int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);
	meshList[GEO_BALL] = MeshBuilder::GenerateSphere("ball", Color(1, 1, 1), 10, 10, 1.f);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", Color(1, 1, 1), 2.f);
	meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("cylinder", Color(1, 0, 0), 36, 12, 1);
	meshList[GEO_HEALTHBAR] = MeshBuilder::GenerateCube("healthbar", Color(0, 1, 0), 1.f);
	meshList[GEO_ENEMY_HEALTHBAR] = MeshBuilder::GenerateCube("healthbar", Color(1, 0, 0), 1.f);
	meshList[GEO_NITROBAR] = MeshBuilder::GenerateCube("healthbar", Color(1, 1, 0), 1.f);
	meshList[GEO_MANABAR] = MeshBuilder::GenerateCube("healthbar", Color(1, 0, 1), 1.f);

	meshList[GEO_HEALTHBAR_BOX] = MeshBuilder::GenerateQuad("bg", Color(1, 1, 1), 2.f);
	meshList[GEO_BACKGROUND] = MeshBuilder::GenerateQuad("bg", Color(1, 1, 1), 200.f);
	meshList[GEO_SHIP_BLUE] = MeshBuilder::GenerateQuad("ship", Color(1, 1, 1), 2.f);
	meshList[GEO_SHIP_GREEN] = MeshBuilder::GenerateQuad("ship", Color(1, 1, 1), 2.f);
	meshList[GEO_ENEMYSHIP] = MeshBuilder::GenerateQuad("Eship", Color(1, 1, 1), 2.f);
	meshList[GEO_FIGHTER_ENEMY] = MeshBuilder::GenerateQuad("Eship", Color(1, 1, 1), 2.f);
	meshList[GEO_ENEMY_BULLET] = MeshBuilder::GenerateQuad("asteroid", Color(1, 1, 1), 2.f);
	meshList[GEO_ASTEROID] = MeshBuilder::GenerateQuad("asteroid", Color(1, 1, 1), 2.f);
	meshList[GEO_ASTEROIDLIGHT] = MeshBuilder::GenerateQuad("asteroid", Color(1, 1, 1), 2.f);
	meshList[GEO_BULLET_BLUE] = MeshBuilder::GenerateQuad("asteroid", Color(1, 1, 1), 2.f);
	meshList[GEO_BULLET_GREEN] = MeshBuilder::GenerateQuad("asteroid", Color(1, 1, 1), 2.f);
	meshList[GEO_LASER] = MeshBuilder::GenerateQuad("asteroid", Color(1, 1, 1), 2.f);
	meshList[GEO_MISSILE] = MeshBuilder::GenerateQuad("asteroid", Color(1, 1, 1), 2.f);
	meshList[GEO_SHIELD] = MeshBuilder::GenerateQuad("shield", Color(1, 1, 1), 2.f);
	meshList[GEO_SHIELD_POWERUP] = MeshBuilder::GenerateQuad("shield", Color(1, 1, 1), 2.f);

	//powerup
	meshList[GEO_HEALTH_POWERUP] = MeshBuilder::GenerateQuad("asteroid", Color(1, 1, 1), 2.f);
	meshList[GEO_SLOWMO_POWERUP] = MeshBuilder::GenerateQuad("asteroid", Color(1, 1, 1), 2.f);
	meshList[GEO_FIRERATE_POWERUP] = MeshBuilder::GenerateQuad("asteroid", Color(1, 1, 1), 2.f);
	meshList[GEO_MANA] = MeshBuilder::GenerateQuad("mana", Color(1, 1, 1), 2.f);
	meshList[GEO_ALLY_SHIP] = MeshBuilder::GenerateQuad("ally", Color(1, 1, 1), 2.f);

	//ui
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_BUTTON_START] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_PLAY_BUTTON] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_CUSTOMISE_BUTTON] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_SELECTOR] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_MAIN_MENU] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_CUSTOMISATION_MENU] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_PAUSE_MENU] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_GAMEOVER_MENU] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_PLAYER_SHIP_BLUE_UI] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_PLAYER_SHIP_GREEN_UI] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_WIN_MENU] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_LOGO] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_WIN_TITLE] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_GAMEOVER_TITLE] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);
	meshList[GEO_CUSTOMISATION_TITLE] = MeshBuilder::GenerateQuad("button", Color(1, 1, 0), 1.f);

	//enemy
	meshList[GEO_ENEMYSPACESTATION] = MeshBuilder::GenerateQuad("sapcestation", Color(1, 1, 1), 2.f);
	meshList[GEO_ENEMY_BOSS] = MeshBuilder::GenerateQuad("boss", Color(1, 1, 1), 2.f);
	meshList[GEO_ENEMY_TURRET] = MeshBuilder::GenerateQuad("turret", Color(1, 1, 1), 2.f);


	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");
	meshList[GEO_TEXT]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_BACKGROUND]->textureID = LoadTGA("Image//background.tga");
	meshList[GEO_BACKGROUND]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_SHIP_BLUE]->textureID = LoadTGA("Image//player_ship_blue.tga");
	meshList[GEO_SHIP_BLUE]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_SHIP_GREEN]->textureID = LoadTGA("Image//player_ship_green.tga");
	meshList[GEO_SHIP_GREEN]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_ENEMYSHIP]->textureID = LoadTGA("Image//enemy_basic_ship_red.tga");
	meshList[GEO_ENEMYSHIP]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_FIGHTER_ENEMY]->textureID = LoadTGA("Image//enemy_fighter_ship_red.tga");
	meshList[GEO_FIGHTER_ENEMY]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_ENEMY_BULLET]->textureID = LoadTGA("Image//ship_bullet_red.tga");
	meshList[GEO_ENEMY_BULLET]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_ASTEROID]->textureID = LoadTGA("Image//asteroid.tga");
	meshList[GEO_ASTEROID]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_ASTEROIDLIGHT]->textureID = LoadTGA("Image//asteroid_grey2.tga");
	meshList[GEO_ASTEROIDLIGHT]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_BULLET_BLUE]->textureID = LoadTGA("Image//ship_bullet_blue.tga");
	meshList[GEO_BULLET_BLUE]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_BULLET_GREEN]->textureID = LoadTGA("Image//ship_bullet_green.tga");
	meshList[GEO_BULLET_GREEN]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_LASER]->textureID = LoadTGA("Image//ship_laser.tga");
	meshList[GEO_LASER]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_MISSILE]->textureID = LoadTGA("Image//Enemy//enemyMissile.tga");
	meshList[GEO_MISSILE]->material.kAmbient.Set(1, 0, 0);

	//powerup
	meshList[GEO_HEALTH_POWERUP]->textureID = LoadTGA("Image//Powerup//heal.tga");
	meshList[GEO_HEALTH_POWERUP]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_SLOWMO_POWERUP]->textureID = LoadTGA("Image//Powerup//slowmo.tga");
	meshList[GEO_SLOWMO_POWERUP]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_FIRERATE_POWERUP]->textureID = LoadTGA("Image//Powerup//firerate.tga");
	meshList[GEO_FIRERATE_POWERUP]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_MANA]->textureID = LoadTGA("Image//Powerup//mana.tga");
	meshList[GEO_MANA]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_ALLY_SHIP]->textureID = LoadTGA("Image//player_ally_ship.tga");
	meshList[GEO_ALLY_SHIP]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_SHIELD]->textureID = LoadTGA("Image//Powerup//shield.tga");
	meshList[GEO_SHIELD]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_SHIELD_POWERUP]->textureID = LoadTGA("Image//Powerup//shieldItem.tga");
	meshList[GEO_SHIELD_POWERUP]->material.kAmbient.Set(1, 0, 0);

	//ui
	meshList[GEO_BUTTON_START]->textureID = LoadTGA("Image//UI//buttonStart.tga");
	meshList[GEO_BUTTON_START]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_PLAY_BUTTON]->textureID = LoadTGA("Image//UI//playButton.tga");
	meshList[GEO_PLAY_BUTTON]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_CUSTOMISE_BUTTON]->textureID = LoadTGA("Image//UI//customiseButton.tga");
	meshList[GEO_CUSTOMISE_BUTTON]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_MAIN_MENU]->textureID = LoadTGA("Image//UI//menu.tga");
	meshList[GEO_MAIN_MENU]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_SELECTOR]->textureID = LoadTGA("Image//UI//selector.tga");
	meshList[GEO_SELECTOR]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_CUSTOMISATION_MENU]->textureID = LoadTGA("Image//UI//customisationMenu.tga");
	meshList[GEO_CUSTOMISATION_MENU]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_PAUSE_MENU]->textureID = LoadTGA("Image//UI//pauseMenu.tga");
	meshList[GEO_PAUSE_MENU]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_GAMEOVER_MENU]->textureID = LoadTGA("Image//UI//gameoverMenu.tga");
	meshList[GEO_GAMEOVER_MENU]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_PLAYER_SHIP_BLUE_UI]->textureID = LoadTGA("Image//UI//player_ship_blue_ui.tga");
	meshList[GEO_PLAYER_SHIP_BLUE_UI]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_PLAYER_SHIP_GREEN_UI]->textureID = LoadTGA("Image//UI//player_ship_green_ui.tga");
	meshList[GEO_PLAYER_SHIP_GREEN_UI]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_WIN_MENU]->textureID = LoadTGA("Image//UI//winScreen.tga");
	meshList[GEO_WIN_MENU]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_LOGO]->textureID = LoadTGA("Image//UI//logo.tga");
	meshList[GEO_LOGO]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_WIN_TITLE]->textureID = LoadTGA("Image//UI//winTitle.tga");
	meshList[GEO_WIN_TITLE]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_GAMEOVER_TITLE]->textureID = LoadTGA("Image//UI//gameoverTitle.tga");
	meshList[GEO_GAMEOVER_TITLE]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_CUSTOMISATION_TITLE]->textureID = LoadTGA("Image//UI//customisationTitle.tga");
	meshList[GEO_CUSTOMISATION_TITLE]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_HEALTHBAR_BOX]->textureID = LoadTGA("Image//UI//healthBarBox.tga");
	meshList[GEO_HEALTHBAR_BOX]->material.kAmbient.Set(0, 1, 0);

	//enemy
	meshList[GEO_ENEMYSPACESTATION]->textureID = LoadTGA("Image//Enemy//enemyStation.tga");
	meshList[GEO_ENEMYSPACESTATION]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_ENEMY_BOSS]->textureID = LoadTGA("Image//Enemy//enemyStation2.tga");
	meshList[GEO_ENEMY_BOSS]->material.kAmbient.Set(0, 1, 0);

	meshList[GEO_ENEMY_TURRET]->textureID = LoadTGA("Image//Enemy//enemyTurret.tga");
	meshList[GEO_ENEMY_TURRET]->material.kAmbient.Set(0, 1, 0);

	bLightEnabled = true;
}

void SceneBase::Update(double dt)
{
	//Keyboard Section
	if(Application::IsKeyPressed('1'))
		glEnable(GL_CULL_FACE);
	if(Application::IsKeyPressed('2'))
		glDisable(GL_CULL_FACE);
	if(Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	fps = (float)(1.f / dt);
	flashingTime += dt;
}

void SceneBase::RenderText(Mesh* mesh, std::string text, Color color)
{
	if(!mesh || mesh->textureID <= 0)
		return;
	
	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for(unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}

void SceneBase::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if(!mesh || mesh->textureID <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, Application::GetWindowWidth() / 10, 0, Application::GetWindowHeight() / 10, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for(unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.f + 0.5f, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneBase::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;
	
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	if(enableLight && bLightEnabled)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView = viewStack.Top() * modelStack.Top();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView.a[0]);
		
		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{	
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	if(mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render();
	if(mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void SceneBase::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey, float angle)
{
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, Application::GetWindowWidth() / 10, 0, Application::GetWindowHeight() / 10, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity();

	modelStack.Translate((float)x, (float)y, 0);
	modelStack.Rotate(angle, 0, 0, 1);
	modelStack.Scale((float)sizex, (float)sizey, 1);

	RenderMesh(mesh, false); //UI should not have light
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);

	//glDisable(GL_DEPTH_TEST);
	//Mtx44 ortho;
	//ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	//projectionStack.PushMatrix();
	//projectionStack.LoadMatrix(ortho);
	//viewStack.PushMatrix();
	//viewStack.LoadIdentity(); //No need camera for ortho mode
	//modelStack.PushMatrix();
	//modelStack.LoadIdentity();

	//modelStack.Translate(x, y, 0);
	//modelStack.Rotate(90, 1, 0, 0);
	//modelStack.Scale(sizex, sizey, 1);

	//RenderMesh(mesh, false); //UI should not have light
	//projectionStack.PopMatrix();
	//viewStack.PopMatrix();
	//modelStack.PopMatrix();
	//glEnable(GL_DEPTH_TEST);
}

void SceneBase::RenderMissionOnScreen(std::string text, std::string text2, Color color)
{
	if (flashingTime >= 0 && flashingTime < 1)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "", color, 3, 0, 75);
		RenderTextOnScreen(meshList[GEO_TEXT], "", color, 3, 0, 70);
	}
	else if (flashingTime > 1 && flashingTime < 2)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], text, Color(1, 0, .5), 3, 0, 75);
		RenderTextOnScreen(meshList[GEO_TEXT], text2, Color(1, 0, .5), 3, 0, 70);
	}
	else if (flashingTime > 2 && flashingTime < 3)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "", color, 3, 0, 75);
		RenderTextOnScreen(meshList[GEO_TEXT], "", color, 3, 0, 70);
	}
	else if (flashingTime > 3 && flashingTime < 4)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], text, Color(1, 0, .5), 3, 0, 75);
		RenderTextOnScreen(meshList[GEO_TEXT], text2, Color(1, 0, .5), 3, 0, 70);
	}
	else
	{
		RenderTextOnScreen(meshList[GEO_TEXT], text, color, 3, 0, 75);
		RenderTextOnScreen(meshList[GEO_TEXT], text2, color, 3, 0, 70);
	}
}

bool SceneBase::IsCollided(GameObject* go, GameObject* go2)
{
	Vector3 displacement = go2->pos - go->pos;
	float combinedRadii = go->scale.x + go2->scale.x;
	if (displacement.LengthSquared() < combinedRadii * combinedRadii)
	{
		return true;
	}
	return false;
}

void SceneBase::RenderMainMenu()
{
	RenderMeshOnScreen(meshList[GEO_LOGO], 96, 90, 90, 45);
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

void SceneBase::RenderCustomisationMenu()
{
	RenderMeshOnScreen(meshList[GEO_CUSTOMISATION_TITLE], 96, 90, 90, 45);
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

void SceneBase::RenderPauseMenu()
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

void SceneBase::RenderGameOverMenu()
{
	RenderMeshOnScreen(meshList[GEO_GAMEOVER_TITLE], 96, 80, 90, 45);
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

void SceneBase::RenderWinMenu()
{
	RenderMeshOnScreen(meshList[GEO_WIN_TITLE], 96, 80, 90, 45);
	RenderMeshOnScreen(meshList[GEO_WIN_MENU], 96, 30, 45, 45);

	std::ostringstream ss;
	ss.precision(3);
	ss << "YOUR SCORE: " + SceneBase::score;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 4, 65, 50);

	switch (selectorIndex)
	{
	case 0:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 15, 45, 45);
		break;
	}

	selectorIndex = Math::Clamp(selectorIndex, 0, 0);
}

void SceneBase::UpdateMainMenu(float& m_speed)
{
	SceneBase::score = 0;

	switch (selectorIndex)
	{
	case 0:
		menuType = M_NONE;
		m_speed = 1;
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

void SceneBase::UpdateCustomisationMenu()
{
	switch (selectorIndex)
	{
	case 0:
		break;
	case 1:
		selectorIndex = 0;
		menuType = M_MAIN;
		break;
	}
}

void SceneBase::UpdatePauseMenu(float& m_speed)
{
	switch (selectorIndex)
	{
	case 0:
		menuType = M_NONE;
		m_speed = 1;
		break;
	case 1:
		selectorIndex = 0;
		resetGame = true;
		menuType = M_MAIN;
		break;
	}
}

void SceneBase::UpdateGameOverMenu()
{
	SceneBase::score = 0;

	switch (selectorIndex)
	{
	case 0:
		menuType = M_NONE;
		restartGame = true;
		break;
	case 1:
		selectorIndex = 0;
		resetGame = true;
		menuType = M_MAIN;
		break;
	}
}

void SceneBase::MomentumCollision(GameObject* go, GameObject* go2)
{
	Vector3 finalGOvel, finalGO2vel;
	Vector3 totalMomentum = (go->mass * go->vel) + (go2->mass * go2->vel);
	Vector3 middle = (go2->vel - go->vel) * go->mass;
	finalGO2vel = (totalMomentum + middle) * (1 / (go->mass + go2->mass));
	finalGOvel = finalGO2vel + (go2->vel - go->vel);

	go->vel = finalGOvel;
	go2->vel = finalGO2vel;

	//go->vel + finalGOvel = go2->vel + finalGO2vel;
	//Vector3 totalFinalMomentum = (go->mass * finalGOvel) + (go2->mass * finalGO2vel);
	//finalGOvel = finalGO2vel + (go2->vel - go->vel);
	//totalMomentum = (go->mass * finalGOvel) + (go2->mass * finalGO2vel);
	//totalMomentum = (go->mass * (finalGO2vel + (go2->vel - go->vel)) + (go2->mass * finalGO2vel));
	//Vector3 middle2 = ((go->mass * finalGO2vel) - middle) + (go2->mass * finalGO2vel);
	//Vector3 middle3 = (go->mass + go2->mass) * finalGO2vel - middle;
	//totalMomentum + middle = (go->mass + go2->mass) * finalGO2vel;
}

void SceneBase::UpdateWinMenu()
{
	switch (selectorIndex)
	{
	case 0:
		selectorIndex = 0;
		resetGame = true;
		menuType = M_MAIN;
		break;
	}
}

void SceneBase::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SceneBase::Exit()
{
	// Cleanup VBO
	for(int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if(meshList[i])
			delete meshList[i];
	}
	glDeleteProgram(m_programID);
	glDeleteVertexArrays(1, &m_vertexArrayID);
}
