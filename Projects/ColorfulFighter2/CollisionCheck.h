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
	//壁に当たった時の処理
	Vector3 WallProcess(Player& player, Camera& camera);
	//プレイヤーとぶつかっているか
	bool ColCheckPlayer(Player& p1, Player& p2);
	//壁に当たってるか
	bool ColCheckWall(Player& player, Camera& camera);
	//右の壁に当たっているか
	bool ColCheckRightWall(Player& player, Camera& camera);
	//左の壁に当たっているか
	bool ColCheckLeftWall(Player& player, Camera& camera);
	//当たったことによる処理
	void MoveProcess(Player& p1, Player& p2, Camera& camera);//移動の処理
	void AttackProcess(Player& p1, Player& p2, Bullet& bulletP1, Bullet& bulletP2, 
		Camera& camera,GameManager& gameManager);//攻撃の処理
	//攻撃が通るかをチェック
	bool CheckSuccessAttack(Player& attacker, Player& defender);
	//ヒットエフェクトの出すポジションを計算
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

