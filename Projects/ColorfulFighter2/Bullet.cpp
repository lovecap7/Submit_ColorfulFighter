#include "Bullet.h"
#include <DxLib.h>
#include "Camera.h"
#include "Player.h"
#include "Camera.h"

namespace
{
	constexpr float kBulletOffsetPos = 90.0f;
	//弾の大きさ
	constexpr float kBulletScale = 1.0f;
	constexpr int kBulletWidth = 160;
	constexpr int kBulletHeight = 160;
	//弾のアニメーションの速さ
	constexpr int kBulletOneAnimFrame = 7;
	//消滅のアニメーションの速さ
	constexpr int kDisappearOneAnimFrame = 3;

	//波動拳
	//弾の判定
	constexpr float kHadouRadius = 40.0f;
	//弾の番号
	constexpr int kStartHadouAnimIndex = 211;
	constexpr int kFinishHadouAnimIndex = 214;
	//弾の位置の調整
	constexpr float kHadouOffsetPosX = 120.0f;
	//速度
	constexpr float kHadouVeloXLight = 15.0f;
	constexpr float kHadouVeloXHigh = 25.0f;
	
	//ソニックブーム
	//弾の判定
	constexpr float kSonicRadius = 40.0f;
	//弾の位置の調整
	constexpr float kSonicOffsetPosX = 120.0f;
	//弾の番号
	constexpr int kStartSonicAnimIndex = 231;
	constexpr int kFinishSonicAnimIndex = 234;
	//速度
	constexpr float kSonicVeloXLight = 25.0f;
	constexpr float kSonicVeloXHigh = 30.0f;

	//パワーウェイブ
	//弾の判定
	constexpr float kPowerWaveRadiusX = 50.0f;
	constexpr float kPowerWaveRadiusY = 30.0f;
	//弾の番号
	constexpr int kStartPowerWaveAnimIndex = 6;
	constexpr int kFinishPowerWaveAnimIndex = 9;
	//弾の位置の調整
	constexpr float kPowerWaveOffsetPosY = 120.0f;
	//速度
	constexpr float kWaveVeloXLight = 15.0f;
	constexpr float kWaveVeloXHigh = 30.0f;
	constexpr float kWaveVeloYHigh = -5.0f;

	//消滅
	constexpr int kStartDisappearAnimIndex = 202;
	constexpr int kFinishDisappearAnimIndex = 205;
}

void Bullet::UpdateHadou(Player& enemy, Bullet& otherBullet, Camera& camera)
{
	if (m_isShooting)
	{
		//移動
		m_pos += m_velocity;

		//敵に当たったかどうかをチェック
		//当たった後の処理はCollisionCheckがやる（1P側と2P側で処理の順で優劣がつかないようにするため）
		m_isHitPlayer = CheckHitPlayer(enemy);

		//弾同士で相殺
		if (CheckHitBullet(otherBullet) && otherBullet.GetIsShooting())
		{
			Disappear();
			otherBullet.Disappear();
		}
		//壁の外にいったら消す
		if (camera.GetCameraLeftWallPos() - m_hitBoxAttack.x2 > m_pos.x || camera.GetCameraRightWallPos() - m_hitBoxAttack.x1 < m_pos.x)
		{
			Disappear();
		}
		//アニメーションのフレームを数える
		if (m_animCountFrame % kBulletOneAnimFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//アニメーションの数が最大まで行ったとき
			if ((m_animIndex > m_finishAnimIndex))
			{
				m_animIndex = m_startAnimIndex;
			}
		}
	}
	else
	{
		//アニメーションのフレームを数える
		if (m_animCountFrame % kDisappearOneAnimFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//アニメーションの数が最大まで行ったとき
			if ((m_animIndex > m_finishAnimIndex))
			{
				m_animIndex = m_finishAnimIndex;
			}
		}
	}
}

void Bullet::DrawHadou(Camera& camera)
{
	//弾
	//左に進んでるなら左を向く
	bool isLeft = false;
	if (m_velocity.x < 0)
	{
		isLeft = true;
	}

	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_blueBulletHandle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kBulletWidth);//横
	int cutY = m_animIndex / (sizeX / kBulletHeight);//縦
	////メイン
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kBulletWidth * cutX,
		kBulletHeight * cutY,
		kBulletWidth, kBulletHeight,
		kBulletScale, 0.0f, m_blueBulletHandle, true, isLeft);
}

void Bullet::UpdateSonic(Player& enemy, Bullet& otherBullet, Camera& camera)
{
	if (m_isShooting)
	{

		//移動
		m_pos += m_velocity;

		//敵に当たったかどうかをチェック
		//当たった後の処理はCollisionCheckがやる（1P側と2P側で処理の順で優劣がつかないようにするため）
		m_isHitPlayer = CheckHitPlayer(enemy);

		//弾同士で相殺
		if (CheckHitBullet(otherBullet) && otherBullet.GetIsShooting())
		{
			Disappear();
			otherBullet.Disappear();
		}
		//壁の外にいったら消す
		if (camera.GetCameraLeftWallPos() - m_hitBoxAttack.x2 > m_pos.x || camera.GetCameraRightWallPos() - m_hitBoxAttack.x1 < m_pos.x)
		{
			Disappear();
		}
		//アニメーションのフレームを数える
		if (m_animCountFrame % kBulletOneAnimFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//アニメーションの数が最大まで行ったとき
			if ((m_animIndex > m_finishAnimIndex))
			{
				m_animIndex = m_startAnimIndex;
			}
		}
	}
	else
	{
		//アニメーションのフレームを数える
		if (m_animCountFrame % kDisappearOneAnimFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//アニメーションの数が最大まで行ったとき
			if ((m_animIndex > m_finishAnimIndex))
			{
				m_animIndex = m_finishAnimIndex;
			}
		}
	}
}

void Bullet::DrawSonic(Camera& camera)
{
	////弾
//左に進んでるなら左を向く
	bool isLeft = false;
	if (m_velocity.x < 0)
	{
		isLeft = true;
	}

	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_yellowBulletHandle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kBulletWidth);//横
	int cutY = m_animIndex / (sizeX / kBulletHeight);//縦
	////メイン
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kBulletWidth * cutX,
		kBulletHeight * cutY,
		kBulletWidth, kBulletHeight,
		kBulletScale, 0.0f, m_yellowBulletHandle, true, isLeft);
}

void Bullet::UpdateWave(Player& enemy, Bullet& otherBullet, Camera& camera)
{
	if (m_isShooting)
	{
		m_velocity.y *= 1.1f;
		//移動
		m_pos += m_velocity;

		//敵に当たったかどうかをチェック
		//当たった後の処理はCollisionCheckがやる（1P側と2P側で処理の順で優劣がつかないようにするため）
		m_isHitPlayer = CheckHitPlayer(enemy);

		//弾同士で相殺
		if (CheckHitBullet(otherBullet) && otherBullet.GetIsShooting())
		{
			Disappear();
			otherBullet.Disappear();
		}
		//壁の外にいったら消す
		if (camera.GetCameraLeftWallPos() - m_hitBoxAttack.x2 > m_pos.x || camera.GetCameraRightWallPos() - m_hitBoxAttack.x1 < m_pos.x)
		{
			Disappear();
		}
		//アニメーションのフレームを数える
		if (m_animCountFrame % kBulletOneAnimFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//アニメーションの数が最大まで行ったとき
			if ((m_animIndex > m_finishAnimIndex))
			{
				m_animIndex = m_startAnimIndex;
			}
		}
	}
	else
	{
		//アニメーションのフレームを数える
		if (m_animCountFrame % kDisappearOneAnimFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//アニメーションの数が最大まで行ったとき
			if ((m_animIndex > m_finishAnimIndex))
			{
				m_animIndex = m_finishAnimIndex;
			}
		}
	}
}

void Bullet::DrawWave(Camera& camera)
{
	////弾
	//左に進んでるなら左を向く
	bool isLeft = false;
	if (m_velocity.x < 0)
	{
		isLeft = true;
	}

	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_purpleBulletHandle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kBulletWidth);//横
	int cutY = m_animIndex / (sizeX / kBulletHeight);//縦
	////メイン
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kBulletWidth * cutX,
		kBulletHeight * cutY,
		kBulletWidth, kBulletHeight,
		kBulletScale, 0.0f, m_purpleBulletHandle, true, isLeft);
}

Bullet::Bullet(PlayerIndex playerIndex):
	m_pos(),
	m_velocity(),
	m_playerIndex(playerIndex),
	m_isShooting(false),
	m_hitBoxAttack(-kHadouRadius,-kHadouRadius, kHadouRadius, kHadouRadius),
	m_isHitPlayer(false),
	m_giveNoActFrame(0),
	m_giveGuardFrame(0),
	m_giveDamage(0.0f),
	m_giveAttackVelo(5,0,0),
	m_animIndex(kStartHadouAnimIndex),
	m_startAnimIndex(kStartHadouAnimIndex),
	m_finishAnimIndex(kFinishHadouAnimIndex),
	m_animCountFrame(0),
	m_update(&Bullet::UpdateHadou),
	m_draw(&Bullet::DrawHadou),
	m_blueBulletHandle(LoadGraph("./img/Bullet/BlueBullet160x160.png")),//青
	m_yellowBulletHandle(LoadGraph("./img/Bullet/YellowBullet160x160.png")),//黄色
	m_purpleBulletHandle(LoadGraph("./img/Bullet/PurpleBullet160x160.png"))//紫
{
}

Bullet::~Bullet()
{
	DeleteGraph(m_blueBulletHandle);
	DeleteGraph(m_yellowBulletHandle);
	DeleteGraph(m_purpleBulletHandle);
}

void Bullet::Init()
{
	m_pos.x = 0;
	m_pos.y = 0;
	m_velocity.x = 0;
	m_velocity.y = 0;
	m_isShooting = false;
	InitHitBoxHadou();
	m_isHitPlayer = false;
}

void Bullet::SetShotEffect(float damage, int giveNoActFrame, int giveGuardFrame)
{
	//相手に与える影響をセット
	m_giveDamage = damage;
	m_giveNoActFrame = giveNoActFrame;
	m_giveGuardFrame = giveGuardFrame;
}

void Bullet::Update(Player& enemy, Bullet& otherBullet, Camera& camera)
{
	m_animCountFrame++;
	(this->*m_update)(enemy,otherBullet,camera);
}

void Bullet::Draw(Camera& camera)
{
	(this->*m_draw)(camera);
#if _DEBUG
	//当たり判定
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	//攻撃判定
	DxLib::DrawBox(
		static_cast<int>(m_pos.x + m_hitBoxAttack.x1 + camera.m_drawOffset.x),
		static_cast<int>(m_pos.y + m_hitBoxAttack.y1 + camera.m_drawOffset.y),
		static_cast<int>(m_pos.x + m_hitBoxAttack.x2 + camera.m_drawOffset.x),
		static_cast<int>(m_pos.y + m_hitBoxAttack.y2 + camera.m_drawOffset.y),
		0xff0000, true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 100);
#endif
}

void Bullet::Disappear()
{
	m_isShooting = false;
	ResetAttackBox();
	m_isHitPlayer = false;
	m_startAnimIndex = kStartDisappearAnimIndex;
	m_finishAnimIndex = kFinishDisappearAnimIndex;
	m_animIndex = m_startAnimIndex;
	m_animCountFrame = 0;
}

//波動拳
void Bullet::LoadHadou(Player& player, float damage, int giveNoActFrame, int giveGuardFrame)
{
	//すでに弾が発射されているならやらない
	if (m_isShooting)return;
	//波動拳
	m_startAnimIndex = kStartHadouAnimIndex;
	m_finishAnimIndex = kFinishHadouAnimIndex;
	m_animIndex = m_startAnimIndex;
	m_animCountFrame = 0;
	//ポジションとスピードと向きをセット
	m_pos.y = player.GetPos().y;
	m_velocity.x = 0;
	m_velocity.y = 0;
	if (player.GetDirState())
	{
		m_pos.x = player.GetPos().x - kBulletOffsetPos;
		if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
		{
			m_velocity.x = -kHadouVeloXLight;
		}
		else if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
		{
			m_velocity.x = -kHadouVeloXHigh;
		}
	}
	else
	{
		m_pos.x = player.GetPos().x + kBulletOffsetPos;
		if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
		{
			m_velocity.x = kHadouVeloXLight;
		}
		else if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
		{
			m_velocity.x = kHadouVeloXHigh;
		}
	}
	//手から出したいので位置調整
	if (m_velocity.x < 0)
	{
		m_pos.x -= kHadouOffsetPosX;
	}
	else
	{
		m_pos.x += kHadouOffsetPosX;
	}
	//当たり判定初期化
	InitHitBoxHadou();
	//相手に与える効果
	SetShotEffect(damage, giveNoActFrame, giveGuardFrame);
	if (m_update != &Bullet::UpdateHadou || m_draw != &Bullet::DrawHadou)
	{
		m_update = &Bullet::UpdateHadou;
		m_draw = &Bullet::DrawHadou;
	}
}
//ソニックブーム
void Bullet::LoadSonic(Player& player, float damage, int giveNoActFrame, int giveGuardFrame)
{
	//すでに弾が発射されているならやらない
	if (m_isShooting)return;
	//ソニックブーム
	m_startAnimIndex = kStartSonicAnimIndex;
	m_finishAnimIndex = kFinishSonicAnimIndex;
	m_animIndex = m_startAnimIndex;
	m_animCountFrame = 0;
	//ポジションとスピードと向きをセット
	m_pos.y = player.GetPos().y;
	m_velocity.x = 0;
	m_velocity.y = 0;
	if (player.GetDirState())
	{
		m_pos.x = player.GetPos().x - kSonicOffsetPosX;
		if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
		{
			m_velocity.x = -kSonicVeloXLight;
		}
		else if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
		{
			m_velocity.x = -kSonicVeloXHigh;
		}
	}
	else
	{
		m_pos.x = player.GetPos().x + kSonicOffsetPosX;
		if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
		{
			m_velocity.x = kSonicVeloXLight;
		}
		else if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
		{
			m_velocity.x = kSonicVeloXHigh;
		}
	}
	//初期化
	InitHitBoxSonic();
	//相手に与える効果
	SetShotEffect(damage, giveNoActFrame, giveGuardFrame);
	if (m_update != &Bullet::UpdateSonic || m_draw != &Bullet::DrawSonic)
	{
		m_update = &Bullet::UpdateSonic;
		m_draw = &Bullet::DrawSonic;
	}
}
//闇払い
void Bullet::LoadWave(Player& player, float damage, int giveNoActFrame, int giveGuardFrame)
{
	//すでに弾が発射されているならやらない
	if (m_isShooting)return;
	//闇払い
	m_startAnimIndex = kStartPowerWaveAnimIndex;
	m_finishAnimIndex = kFinishPowerWaveAnimIndex;
	m_animIndex = m_startAnimIndex;
	m_animCountFrame = 0;
	//ポジションとスピードと向きをセット
	//地面を滑らせたいので位置調整
	m_pos.y = player.GetPos().y + kPowerWaveOffsetPosY;
	m_velocity.x = 0;
	m_velocity.y = 0;
	if (player.GetDirState())
	{
		m_pos.x = player.GetPos().x - kBulletOffsetPos;
		if (player.GetAttackAttackTypes() == AttackTypes::LightKick)
		{
			m_velocity.x = -kWaveVeloXLight;
		}
		else if (player.GetAttackAttackTypes() == AttackTypes::HighKick)
		{
			m_velocity.x = -kWaveVeloXHigh;
			m_velocity.y = kWaveVeloYHigh;
		}
	}
	else
	{
		m_pos.x = player.GetPos().x + kBulletOffsetPos;
		if (player.GetAttackAttackTypes() == AttackTypes::LightKick)
		{
			m_velocity.x = kWaveVeloXLight;
		}
		else if (player.GetAttackAttackTypes() == AttackTypes::HighKick)
		{
			m_velocity.x = kWaveVeloXHigh;
			m_velocity.y = kWaveVeloYHigh;
		}
	}
	//初期化
	InitHitBoxWave();
	//相手に与える効果
	SetShotEffect(damage, giveNoActFrame, giveGuardFrame);
	if (m_update != &Bullet::UpdateWave || m_draw != &Bullet::DrawWave)
	{
		m_update = &Bullet::UpdateWave;
		m_draw = &Bullet::DrawWave;
	}
}

bool Bullet::CheckHitPlayer(Player& enemy)
{
	//攻撃の判定がないならチェックしない
	if (m_hitBoxAttack.x1 == 0 &&
		m_hitBoxAttack.y1 == 0 &&
		m_hitBoxAttack.x2 == 0 &&
		m_hitBoxAttack.y2 == 0)	return false;



	float width;
	float height;
	bool isHit = false;
	if (enemy.GetDirState())
	{
		//攻撃判定の中心を求める
		Vector3 attackPos((m_pos.x + m_hitBoxAttack.x2 + m_pos.x + m_hitBoxAttack.x1) / 2,
			(m_pos.y + m_hitBoxAttack.y2 + m_pos.y + m_hitBoxAttack.y1) / 2, 0.0f);

		//当たり判定があるならチェック
		//頭
		if (enemy.GetHitBoxHead().x1 != 0 ||
			enemy.GetHitBoxHead().y1 != 0 ||
			enemy.GetHitBoxHead().x2 != 0 ||
			enemy.GetHitBoxHead().y2 != 0)
		{
			//敵の頭のやられ判定////////////////////////////////////////
			Vector3 enemyHead((enemy.GetPos().x - enemy.GetHitBoxHead().x2 + enemy.GetPos().x - enemy.GetHitBoxHead().x1) / 2,
				(enemy.GetPos().y + enemy.GetHitBoxHead().y2 + enemy.GetPos().y + enemy.GetHitBoxHead().y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyHead.x);
			height = std::abs(attackPos.y - enemyHead.y);

			//X方向の攻撃の幅と体の幅を足した値より小さいなら当たってる
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.GetHitBoxHead().x2 - enemy.GetHitBoxHead().x1) / 2) &&
				//Y方向攻撃の幅と体の幅を足した値より小さいなら当たってる
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.GetHitBoxHead().y2 - enemy.GetHitBoxHead().y1) / 2)) return true;
			/////////////////////////////////////////////////////////////
		}
	

		//当たり判定があるならチェック
		//体
		if (enemy.GetHitBoxBody().x1 != 0 ||
			enemy.GetHitBoxBody().y1 != 0 ||
			enemy.GetHitBoxBody().x2 != 0 ||
			enemy.GetHitBoxBody().y2 != 0)
		{
			//敵の体のやられ判定////////////////////////////////////////////
			Vector3 enemyBody((enemy.GetPos().x - enemy.GetHitBoxBody().x2 + enemy.GetPos().x - enemy.GetHitBoxBody().x1) / 2,
				(enemy.GetPos().y + enemy.GetHitBoxBody().y2 + enemy.GetPos().y + enemy.GetHitBoxBody().y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyBody.x);
			height = std::abs(attackPos.y - enemyBody.y);

			//X方向の攻撃の幅と体の幅を足した値より小さいなら当たってる
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.GetHitBoxBody().x2 - enemy.GetHitBoxBody().x1) / 2) &&
				//Y方向攻撃の幅と体の幅を足した値より小さいなら当たってる
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.GetHitBoxBody().y2 - enemy.GetHitBoxBody().y1) / 2)) return true;
			///////////////////////////////////////////////////////////////
		}
		
		//当たり判定があるならチェック
		//足
		if (enemy.GetHitBoxLeg().x1 != 0 ||
			enemy.GetHitBoxLeg().y1 != 0 ||
			enemy.GetHitBoxLeg().x2 != 0 ||
			enemy.GetHitBoxLeg().y2 != 0)
		{
			//敵の足のやられ判定
			Vector3 enemyLeg((enemy.GetPos().x - enemy.GetHitBoxLeg().x2 + enemy.GetPos().x - enemy.GetHitBoxLeg().x1) / 2,
				(enemy.GetPos().y + enemy.GetHitBoxLeg().y2 + enemy.GetPos().y + enemy.GetHitBoxLeg().y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyLeg.x);
			height = std::abs(attackPos.y - enemyLeg.y);

			//X方向の攻撃の幅と体の幅を足した値より小さいなら当たってる
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.GetHitBoxLeg().x2 - enemy.GetHitBoxLeg().x1) / 2) &&
				//Y方向攻撃の幅と体の幅を足した値より小さいなら当たってる
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.GetHitBoxLeg().y2 - enemy.GetHitBoxLeg().y1) / 2)) return true;
		}
		
	}
	else
	{
		//攻撃判定の中心を求める
		Vector3 attackPos((m_pos.x - m_hitBoxAttack.x2 + m_pos.x - m_hitBoxAttack.x1) / 2,
			(m_pos.y + m_hitBoxAttack.y2 + m_pos.y + m_hitBoxAttack.y1) / 2, 0.0f);

		//頭
		if (enemy.GetHitBoxHead().x1 != 0 ||
			enemy.GetHitBoxHead().y1 != 0 ||
			enemy.GetHitBoxHead().x2 != 0 ||
			enemy.GetHitBoxHead().y2 != 0)
		{
			//敵の頭のやられ判定////////////////////////////////////////
			Vector3 enemyHead((enemy.GetPos().x + enemy.GetHitBoxHead().x2 + enemy.GetPos().x + enemy.GetHitBoxHead().x1) / 2,
				(enemy.GetPos().y + enemy.GetHitBoxHead().y2 + enemy.GetPos().y + enemy.GetHitBoxHead().y1) / 2, 0.0f);

			width = std::abs(attackPos.x - enemyHead.x);
			height = std::abs(attackPos.y - enemyHead.y);

			//X方向の攻撃の幅と頭の幅を足した値より小さいなら当たってる
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.GetHitBoxHead().x2 - enemy.GetHitBoxHead().x1) / 2) &&
				//X方向の攻撃の幅と頭の幅を足した値より小さいなら当たってる
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.GetHitBoxHead().y2 - enemy.GetHitBoxHead().y1) / 2)) return true;
			////////////////////////////////////////////////////////////
		}
	

		//体
		if (enemy.GetHitBoxBody().x1 != 0 ||
			enemy.GetHitBoxBody().y1 != 0 ||
			enemy.GetHitBoxBody().x2 != 0 ||
			enemy.GetHitBoxBody().y2 != 0)
		{
			//敵の体のやられ判定///////////////////////////////////////
			Vector3 enemyBody((enemy.GetPos().x + enemy.GetHitBoxBody().x2 + enemy.GetPos().x + enemy.GetHitBoxBody().x1) / 2,
				(enemy.GetPos().y + enemy.GetHitBoxBody().y2 + enemy.GetPos().y + enemy.GetHitBoxBody().y1) / 2, 0.0f);

			width = std::abs(attackPos.x - enemyBody.x);
			height = std::abs(attackPos.y - enemyBody.y);

			//X方向の攻撃の幅と体の幅を足した値より小さいなら当たってる
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.GetHitBoxBody().x2 - enemy.GetHitBoxBody().x1) / 2) &&
				//Y方向攻撃の幅と体の幅を足した値より小さいなら当たってる
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.GetHitBoxBody().y2 - enemy.GetHitBoxBody().y1) / 2)) return true;
			/////////////////////////////////////////////////////////
		}
	


		//足
		if (enemy.GetHitBoxLeg().x1 != 0 ||
			enemy.GetHitBoxLeg().y1 != 0 ||
			enemy.GetHitBoxLeg().x2 != 0 ||
			enemy.GetHitBoxLeg().y2 != 0)
		{
			//敵の足のやられ判定
			Vector3 enemyLeg((enemy.GetPos().x + enemy.GetHitBoxLeg().x2 + enemy.GetPos().x + enemy.GetHitBoxLeg().x1) / 2,
				(enemy.GetPos().y + enemy.GetHitBoxLeg().y2 + enemy.GetPos().y + enemy.GetHitBoxLeg().y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyLeg.x);
			height = std::abs(attackPos.y - enemyLeg.y);

			//X方向の攻撃の幅と体の幅を足した値より小さいなら当たってる
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.GetHitBoxLeg().x2 - enemy.GetHitBoxLeg().x1) / 2) &&
				//Y方向攻撃の幅と体の幅を足した値より小さいなら当たってる
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.GetHitBoxLeg().y2 - enemy.GetHitBoxLeg().y1) / 2)) return true;
		}
	
	}



	return false;
}

bool Bullet::CheckHitBullet(Bullet& otherBullet)
{

	//X、Y方向の離れている距離を絶対値化して取得
	float width = std::abs(m_pos.x - otherBullet.GetPos().x);
	float height = std::abs(m_pos.y - otherBullet.GetPos().y);

	//X方向の距離がキャラの幅より大きければ当たっていない
	if (width > std::abs(GetHitBoxAttack().x1 - otherBullet.GetHitBoxAttack().x2))	return false;
	//Y方向の距離がキャラの高さより大きければ当たっていない
	if (height > std::abs(GetHitBoxAttack().y1 - otherBullet.GetHitBoxAttack().y2))	return false;

	//ここまでくれば当たってる
	return true;
}

void Bullet::InitHitBoxHadou()
{
	//攻撃範囲
	m_hitBoxAttack.x1 = -kHadouRadius;
	m_hitBoxAttack.y1 = -kHadouRadius;
	m_hitBoxAttack.x2 = kHadouRadius;
	m_hitBoxAttack.y2 = kHadouRadius;
}

void Bullet::InitHitBoxSonic()
{
	//攻撃範囲
	m_hitBoxAttack.x1 = -kSonicRadius;
	m_hitBoxAttack.y1 = -kSonicRadius;
	m_hitBoxAttack.x2 = kSonicRadius;
	m_hitBoxAttack.y2 = kSonicRadius;
}

void Bullet::InitHitBoxWave()
{
	//攻撃範囲
	m_hitBoxAttack.x1 = -kPowerWaveRadiusX;
	m_hitBoxAttack.y1 = -kPowerWaveRadiusY;
	m_hitBoxAttack.x2 = kPowerWaveRadiusX;
	m_hitBoxAttack.y2 = kPowerWaveRadiusY;
}

