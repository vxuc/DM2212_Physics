#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#include "Scene.h"
#include "Mtx44.h"
#include "Camera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "GameObject.h"
#include <vector>

enum MISSION
{
	DESTROY_ENEMYSTATION_LEVEL1 = 0,
	ELIMINATE_REMAINING_ENEMIES_LEVEL1,
	COMPLETED_LEVEL1,
	DESTROY_ENEMYSTATION_TURRET_LEVEL2,
	ELIMINATE_REMAINING_ENEMIES_LEVEL2,
	COMPLETED_LEVEL2,
	DESTROY_BOSS_TURRET_LEVEL3,
	ELIMINATE_REMAINING_ENEMIES_LEVEL3,
	COMPLETED_LEVEL3,
	TOTAL_MISSION
};

enum PLAYER_SHIP_COLOUR
{
	SHIP_BLUE = 0,
	SHIP_GREEN,
	TOTAL_COLOUR
};

enum MENU_TYPE
{
	M_NONE = 0,
	M_MAIN,
	M_CUSTOMISE,
	M_PAUSE,
	M_GAMEOVER,
	M_WIN,
	M_TOTAL
};

class SceneBase : public Scene
{
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHTENABLED,
		U_NUMLIGHTS,
		U_LIGHT0_TYPE,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_TEXT,
		GEO_BALL,
		GEO_CUBE,
		GEO_CYLINDER,
		GEO_HEALTHBAR,
		GEO_ENEMY_HEALTHBAR,
		GEO_HEALTHBAR_BOX,
		GEO_NITROBAR,
		GEO_MANABAR,

		GEO_SHIP_BLUE,
		GEO_SHIP_GREEN,
		GEO_ENEMYSHIP,
		GEO_FIGHTER_ENEMY,
		GEO_ENEMY_BULLET,
		GEO_ASTEROID,
		GEO_BULLET_BLUE,
		GEO_BULLET_GREEN,
		GEO_LASER,
		GEO_MISSILE,
		GEO_HEALTH_POWERUP,
		GEO_SLOWMO_POWERUP,
		GEO_FIRERATE_POWERUP,
		GEO_BUTTON_START,
		GEO_ASTEROIDLIGHT,
		GEO_MANA,
		GEO_ALLY_SHIP,
		GEO_SHIELD,
		GEO_SHIELD_POWERUP,


		GEO_BACKGROUND,
		GEO_PLAY_BUTTON,
		GEO_CUSTOMISE_BUTTON,
		GEO_MAIN_MENU,
		GEO_SELECTOR,
		GEO_CUSTOMISATION_MENU,
		GEO_PAUSE_MENU,
		GEO_GAMEOVER_MENU,
		GEO_PLAYER_SHIP_GREEN_UI,
		GEO_PLAYER_SHIP_BLUE_UI,
		GEO_WIN_MENU,
		GEO_LOGO,
		GEO_WIN_TITLE,
		GEO_GAMEOVER_TITLE,
		GEO_CUSTOMISATION_TITLE,

		GEO_ENEMYSPACESTATION,
		GEO_ENEMY_BOSS,
		GEO_ENEMY_TURRET,
		NUM_GEOMETRY,
	};

public:

	static MISSION missionType;
	static PLAYER_SHIP_COLOUR shipColor;
	static MENU_TYPE menuType;
	static bool restartGame;
	static bool resetGame;
	static int score;

	SceneBase();
	~SceneBase();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderMesh(Mesh *mesh, bool enableLight);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey, float angle = 0);
	void RenderGO(GameObject *go);
	void RenderMissionOnScreen(std::string text, std::string text2, Color color = Color(1,1,1));
	bool IsCollided(GameObject* go, GameObject* go2);
	void RenderMainMenu();
	void RenderCustomisationMenu();
	void RenderPauseMenu();
	void RenderGameOverMenu();
	void RenderWinMenu();

	void UpdateMainMenu(float& m_speed);
	void UpdateCustomisationMenu();
	void UpdatePauseMenu(float& m_speed);
	void UpdateGameOverMenu();
	void UpdateWinMenu();

	void MomentumCollision(GameObject* go, GameObject* go2);

	GameObject* FetchGO();
protected:
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	Camera camera;
	Camera camera2;

	MS modelStack;
	MS viewStack;
	MS projectionStack;

	Light lights[1];

	bool bLightEnabled;

	bool cam2;

	float fps;

	static float flashingTime;

	int selectorIndex;
	int colourIndex;
};

#endif