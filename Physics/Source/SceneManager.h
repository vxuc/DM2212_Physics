#pragma once
#include "SceneLevel1.h"
#include "SceneLevel2.h"
#include "SceneLevel3.h"
#include "SceneCollision.h"
#include "Application.h"
#include <vector>

enum SCENES
{
	S_LEVEL1 = 0,
	S_LEVEL2,
	S_LEVEL3,
	S_COUNT
};

class SceneManager : public SceneBase
{
private:
	int sceneIndex;
public:
	std::vector<Scene*> SceneList;
	static SCENES activeScene;
	static SCENES prevScene;

	SceneManager();
	~SceneManager();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void initScenes();
	void ReInitScenes();
	Scene* getScene();
};

