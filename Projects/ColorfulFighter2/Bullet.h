#pragma once
#include "Vector3.h"
#include "game.h"
#include "Box.h"

class Camera;
class Player;
class Bullet
{
private:
	Vector3 m_pos;
	Vector3 m_velocity;
	PlayerIndex m_playerIndex;
	Box m_hitBoxAttack;
	int m_giveNoActFrame;	//自分が攻撃したとき相手が動けないフレーム
	int m_giveGuardFrame;	//自分が攻撃したとき相手がガードをして動けないフレーム
	float m_giveDamage;			//相手に与えるダメージ
	//攻撃によって相手に与えるVelo
	Vector3 m_giveAttackVelo;
	void SetShotEffect(float damage, int giveNoActFrame, int giveGuardFrame);
	//trueなら発射中
	bool m_isShooting;
	//攻撃がプレイヤーにあったかを判定
	bool m_isHitPlayer;
	//プレイヤーに当たったか
	bool CheckHitPlayer(Player& enemy);
	//弾に当たったか
	bool CheckHitBullet(Bullet& otherBullet);
	//攻撃判定の初期化
	void InitHitBoxHadou();
	void InitHitBoxSonic();
	void InitHitBoxWave();
	void ResetAttackBox() {
		//攻撃の判定をリセット
		m_hitBoxAttack.x1 = 0;
		m_hitBoxAttack.y1 = 0;
		m_hitBoxAttack.x2 = 0;
		m_hitBoxAttack.y2 = 0;
	}
	//弾のハンドル
	int m_blueBulletHandle;//青い弾
	int m_yellowBulletHandle;//黄色い弾
	int m_purpleBulletHandle;//紫の弾
	int m_animIndex;//現在の画像の番号
	int m_startAnimIndex;//最初の画像の番号
	int m_finishAnimIndex;//最後の画像の番号
	int m_animCountFrame;
	using UpdateFunc_t = void(Bullet::*)(Player& enemy, Bullet& otherBullet, Camera& camera);
	UpdateFunc_t m_update;
	//Draw
	using DrawFunc_t = void(Bullet::*)(Camera& camera);
	DrawFunc_t m_draw;
	void UpdateHadou(Player& enemy, Bullet& otherBullet, Camera& camera);
	void DrawHadou(Camera& camera);
	void UpdateSonic(Player& enemy, Bullet& otherBullet, Camera& camera);
	void DrawSonic(Camera& camera);
	void UpdateWave(Player& enemy, Bullet& otherBullet, Camera& camera);
	void DrawWave(Camera& camera);
public:
	Bullet(PlayerIndex playerIndex);
	~Bullet();
	void Init();
	void Update(Player& enemy, Bullet& otherBullet,Camera& camera);
	void Draw(Camera& camera);
	void Disappear();//弾が消える
	//なんPなのか
	PlayerIndex GetPlayerIndex() { return m_playerIndex; }
	//現在発射中かを取得
	bool GetIsShooting()const { return m_isShooting; }
	//発射中
	void OnIsShooting() { m_isShooting = true; }
	void OffShooting() { m_isShooting = false; }
	//座標の取得
	Vector3 GetPos()const { return m_pos; }
	//プレイヤーに当たったかどうかを取得
	bool GetIsHitPlayer() const{ return m_isHitPlayer; }
	//攻撃判定
	Box GetHitBoxAttack()const { return m_hitBoxAttack; }
	//ダメージ
	float GetGiveDamage()const { return m_giveDamage; }
	//硬直
	int GetGiveNoActFrame()const { return m_giveNoActFrame; }
	//ガード硬直
	int GetGiveGuardFrame()const { return m_giveGuardFrame; }
	//攻撃によって相手に与えるVelo
	Vector3 GetGiveAttackVelo() { return m_giveAttackVelo; }
	void SetGiveAttackVelo(Vector3 giveAttackVelo) { m_giveAttackVelo = giveAttackVelo; }
	//3種類の弾
	void LoadHadou(Player& player, float damage, int giveNoActFrame, int giveGuardFrame);
	void LoadSonic(Player& player, float damage, int giveNoActFrame, int giveGuardFrame);
	void LoadWave(Player& player, float damage, int giveNoActFrame, int giveGuardFrame);
};

