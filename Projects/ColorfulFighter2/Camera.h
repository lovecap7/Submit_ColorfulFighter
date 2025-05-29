#pragma once
#include "Vector3.h"
class Player;
class GameManager;
class Camera
{	
private:
	//揺れるカメラの移動量
	Vector3 m_shakeVelo;
	void ShakeCamera(Player& p1, Player& p2, GameManager& gameManager);
	bool m_isInitShakeVelo;
public:
	Camera();
	~Camera();
	void Init(Player& p1, Player& p2);
	void Update(Player& p1,Player& p2, GameManager& gameManager);
	////座標
	//Vector3 GetCameraPos() { return m_pos; }
	//Vector3 GetDrawOffset() { return m_drawOffset; }
	Vector3 m_pos;			//実際のカメラのポジション
	Vector3 m_drawOffset;	//補正
	//左右の壁の座標
	float GetCameraLeftWallPos();
	float GetCameraRightWallPos();
};

