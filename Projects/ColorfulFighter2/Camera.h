#pragma once
#include "Vector3.h"
class Player;
class GameManager;
class Camera
{	
private:
	//�h���J�����̈ړ���
	Vector3 m_shakeVelo;
	void ShakeCamera(Player& p1, Player& p2, GameManager& gameManager);
	bool m_isInitShakeVelo;
public:
	Camera();
	~Camera();
	void Init(Player& p1, Player& p2);
	void Update(Player& p1,Player& p2, GameManager& gameManager);
	////���W
	//Vector3 GetCameraPos() { return m_pos; }
	//Vector3 GetDrawOffset() { return m_drawOffset; }
	Vector3 m_pos;			//���ۂ̃J�����̃|�W�V����
	Vector3 m_drawOffset;	//�␳
	//���E�̕ǂ̍��W
	float GetCameraLeftWallPos();
	float GetCameraRightWallPos();
};

