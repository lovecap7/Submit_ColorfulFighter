#pragma once
#include "Vector3.h"
#include "AttackInfo.h"
#include <memory>
class Player;
class Bullet;
class Camera;
class GameManager;
class SE;

class CollisionCheck
{
private:
	Vector3 m_lastVeloP1;
	Vector3 m_lastVeloP2;
	//�ǂɓ����������̏���
	Vector3 WallProcess(Player& player, Camera& camera);
	//�v���C���[�ƂԂ����Ă��邩
	bool ColCheckPlayer(Player& p1, Player& p2);
	//�ǂɓ������Ă邩
	bool ColCheckWall(Player& player, Camera& camera);
	//�E�̕ǂɓ������Ă��邩
	bool ColCheckRightWall(Player& player, Camera& camera);
	//���̕ǂɓ������Ă��邩
	bool ColCheckLeftWall(Player& player, Camera& camera);
	//�����������Ƃɂ�鏈��
	void MoveProcess(Player& p1, Player& p2, Camera& camera);//�ړ��̏���
	void AttackProcess(Player& p1, Player& p2, Bullet& bulletP1, Bullet& bulletP2, 
		Camera& camera,GameManager& gameManager);//�U���̏���
	//�U�����ʂ邩���`�F�b�N
	bool CheckSuccessAttack(Player& attacker, Player& defender);
	//�q�b�g�G�t�F�N�g�̏o���|�W�V�������v�Z
	Vector3 CreateHitEffectPosPtoP(Player& p1, Player& p2);
	Vector3 CreateHitEffectPosBtoP(Bullet& p1, Player& p2);
	//SE
	std::shared_ptr<SE> m_seP1;
	std::shared_ptr<SE> m_seP2;
	int m_hitSeHandle;
	int m_guardSeHandle;
public:
	CollisionCheck();
	~CollisionCheck();
	void Update(Player& p1, Player& p2, Bullet& bulletP1, Bullet& bulletP2,
		Camera& camera, GameManager& gameManager);
};

