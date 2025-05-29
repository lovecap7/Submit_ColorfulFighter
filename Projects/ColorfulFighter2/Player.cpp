#include "Player.h"
#include "Input.h"
#include "Chara.h"
#include "Camera.h"
#include "GameManager.h"
#include <cassert>
#include "Bullet.h"
#include "SE.h"

namespace
{
	//地上の高さ
	constexpr int kGroundHeight = Game::kScreenHeight + 400;

	//HP
	constexpr float kHp = 100.0f;

	//重力
	constexpr float kGravityPower = 3.2f;

	//プレイヤーの画像の大きさ
	constexpr int kCharaWidth = 512;
	constexpr int kCharaHeight = 512;
	//画像の倍率
	constexpr float kCharaScale = 1.0f;

	//移動速度
	constexpr float kMoveSpeedFront = 12.0f;
	constexpr float kMoveSpeedBack = 9.0f;

	//ジャンプ力
	constexpr float kJumpPowerX = kMoveSpeedFront;
	constexpr float kJumpPowerY = -60.0f;
	//ジャンプ移行フレーム
	constexpr int kJumpTransFrame = 5;

	//押す力
	constexpr float kPushPower = 3.0f;

	//プレイヤーの押し合い判定のベース(右向きの時)
	constexpr int kBaseCollisionWidthFront = 80;
	constexpr int kBaseCollisionWidthBack = 80;
	constexpr int kBaseCollisionWidthTop = 60;
	constexpr int kBaseCollisionWidthBottom = 200;

	//ダウンしたときの画像の位置を補正
	constexpr int kDownDrawOffset = kCharaHeight/2 - 90;
	constexpr int kDownFrame = 30;//ダウンしてる時間

	//投げのダメージ
	constexpr float kThrowDamage = 12.0f;
	//投げで飛ぶ距離
	constexpr float kThrowVeloX = 15.0f;
	constexpr float kThrowVeloY = -40.0f;
	//後ろ投げの際に少しずれる
	constexpr float kBackThrowShiftVelo = 1.0f;

	//後ろ投げをした際の距離を前投げと同じくらいにするために1.5倍する
	constexpr float kThrowBackMargin = 1.5f;

	//投げぬけができるフレーム
	constexpr int kCanThrowEscapeFrame = 10;
	//投げ抜け成功時の硬直フレーム
	constexpr int kNoActThrowEscapeFrame = 20;


	//キャンセル可能猶予
	constexpr int kCanselGraceFrame = 20;
	//早めにボタンを押してしまってもコマンド成立可能猶予
	constexpr int kInputDelayFrame = 4;

	//SEの音量
	constexpr int kSeVolume = 150;

	//勝利ポーズをするまでにかかる時間
	constexpr int kChangeWinPoseFrame = 120;

	//影のY座標
	constexpr int kShadowPosY = 1510;
	//影の半径
	constexpr float kShadowRadiusX = 120.0f;
	constexpr float kShadowRadiusY = 30.0f;
	//ステージの大きさ
	constexpr int kStageWidth = 2600;

	//入力の残るフレーム
	constexpr int kRemainInputFrame = 7;
}

//プレイヤーの番号と選んだキャラを渡す
Player::Player(PlayerIndex playerIndex, int* selectCommandIndex,CharaColorIndex charaColorIndex,bool isSameColor):
	m_playerIndex(playerIndex),//P1かP2
	m_isSameColor(isSameColor),
	m_pos(500, kGroundHeight, 0),
	m_velocity(),
	m_angle(0.0f),
	m_jumpVelo(0, kJumpPowerY,0),
	m_giveThrowVelo(),
	m_isLeft(false),
	m_hp(kHp),
	m_isGround(true),
	m_isHitAttack(false),
	m_isHitGrasp(false),
	m_isSquat(false),
	m_isThrown(false),
	m_isThrowSuccess(false),
	m_isCommand(false),
	m_isPossibleCancel(false),
	m_cancelFrameCount(0),
	m_animCountFrame(0),
	m_animIndex(0),
	m_animNum(0),
	m_hitBoxGrasp(),
	m_oneAnimIntervalFrame(0),
	m_startAttackFrame(0),
	m_finishAttackFrame(0),
	m_noActFrame(0),
	m_allNoActFrame(0),
	m_giveDamage(0),
	m_knockback(0, 0, 0),
	m_giveNoActFrame(0),
	m_guardFrame(0),
	m_giveGuardFrame(0),
	m_attackAttributes(AttackAttributes::Null),
	m_attackType(AttackTypes::Null),
	m_update(&Player::IdleSquatUpdate),
	m_draw(&Player::IdleSquatDraw),
	m_lightPunchSeHandle(LoadSoundMem("./SE/PlayerBase/LightPunch.mp3")),
	m_lightKickSeHandle(LoadSoundMem("./SE/PlayerBase/LightKick.mp3")),
	m_highPunchSeHandle(LoadSoundMem("./SE/PlayerBase/HighPunch.mp3")),
	m_highKickSeHandle(LoadSoundMem("./SE/PlayerBase/HighKick.mp3")),
	m_downSeHandle(LoadSoundMem("./SE/PlayerBase/Down.mp3")),//倒れる音
	m_jumpSeHandle(LoadSoundMem("./SE/PlayerBase/Jumped.mp3")),//ジャンプの音
	m_jumpedSeHandle(LoadSoundMem("./SE/PlayerBase/Jumped.mp3")),//着地したときの音
	m_graspSeHandle(LoadSoundMem("./SE/PlayerBase/Grasp.mp3")),//つかむ音
	m_throwSeHandle(LoadSoundMem("./SE/PlayerBase/Throw.mp3")),//投げる
	m_loseSeHandle(LoadSoundMem("./SE/PlayerBase/voice/Lose.mp3")),//負け
	m_winSeHandle(LoadSoundMem("./SE/PlayerBase/voice/Win.mp3"))//勝ち
{
	for (int i = 0;i < 3;++i)
	{
		m_selectCommandIndex[i] = selectCommandIndex[i];
	}
	
	m_chara = std::make_shared<Chara>(m_selectCommandIndex, charaColorIndex);//キャラクターを確保
	m_se = std::make_shared<SE>();//SEを確保
	m_chara->GetAnimIdleStand(*this);//待機モーション
	assert(m_handle != -1);
	m_chara->GetHitBoxIdleStand(*this);//待機モーションの当たり判定を取得
	//押し合い判定の初期化
	InitPushBox();
}

Player::~Player()
{
}

//ラウンド切り替わりの際の初期化にも使う
void Player::Init(float X,bool isLeft)
{
	m_se->Stop();
	//入力リセット
	m_lightPunchBottun.Init(false, 0);
	m_highPunchBottun.Init(false, 0);
	m_lightKickBottun.Init(false, 0);
	m_highKickBottun.Init(false, 0);
	m_graspBottun.Init(false, 0);
	//初期位置
	Vector3 firstPos(X, kGroundHeight, 0);
	m_pos = firstPos;
	//速度の初期化
	m_velocity.x = 0;
	m_velocity.y = 0;
	m_giveThrowVelo.x = 0;
	m_giveThrowVelo.y = 0;
	m_knockback.x = 0;
	m_knockback.y = 0;
	//押し合い判定
	InitPushBox();
	//HPの初期化
	m_hp = kHp;
	//状態の回復
	m_isLeft = isLeft;
	m_isGround = true;
	m_isHitAttack = false;
	m_isHitGrasp = false;
	m_isSquat = false;
	m_isThrown = false;
	m_isThrowSuccess = false;
	//アニメーション関連の初期化
	m_animCountFrame = 0;
	m_animIndex = 0;
	m_animNum = 0;
	m_oneAnimIntervalFrame = 0;
	//攻撃のステータスの初期化
	m_startAttackFrame = 0;
	m_finishAttackFrame = 0;
	m_giveDamage = 0;
	ResetAttackBox();
	ResetGraspBox();
	m_attackAttributes = AttackAttributes::Null;
	m_attackType = AttackTypes::Null;
	//硬直時間
	m_noActFrame = 0;
	m_giveNoActFrame = 0;
	m_guardFrame = 0;
	m_giveGuardFrame = 0;
	//待機状態スタート
	m_chara->GetAnimIdleStand(*this);//待機モーション
	assert(m_handle != -1);
	m_chara->GetHitBoxIdleStand(*this);//待機モーションの当たり判定を取得
	//押し合い判定の初期化
	InitPushBox();
	//待機状態
	m_update = &Player::IdleStandUpdate;
	m_draw = &Player::IdleStandDraw;
}

void Player::Update(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//地面についてるかチェック
	CheckGround();
	//重力
	Gravity();
	//時間切れした場合はダメージを与えなくする
	if (gameManager.GetTimer() <= 0)
	{
		m_giveDamage = 0;
	}
	//体力があるなら行動できる
	if (!CheckDead())
	{
		PlayerInput(input);
		(this->*m_update)(input, enemy, myBullet, gameManager);
	}
}

void Player::Draw(const Camera& camera)
{
	//影
	DrawShadow(camera);
	(this->*m_draw)(camera);
	//2プレイヤーの色を少し暗く
	if ((m_playerIndex == PlayerIndex::Player2) && m_isSameColor)
	{
		SetDrawBlendMode(DX_BLENDMODE_SUB, 100);
		(this->*m_draw)(camera);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
#if _DEBUG
	//当たり判定表示
	DrawHitBox(camera);
	//フレーム
	if (m_playerIndex == PlayerIndex::Player1)
	{
		DrawFormatString(0, 400, 0xff4422, "ジャンプの全体フレーム = %d", debugJumpFrame);
		DrawFormatString(0, 450, 0xff4422, "コマンドの全体フレーム = %d", debugCommandFrame);
		DrawFormatString(0, 500, 0xff4422, "プレイヤーの位置(x=%2.0f,y=%2.0f)", m_pos.x, m_pos.y);
		for (int i = 0;i < 3;++i)
		{
			DrawFormatString(400, 50 + (20 * i), 0xff3333, "selectNumP1 = %d", m_selectCommandIndex[i]);
		}
	}
#endif
}

//デストラクタの呼ばれるタイミングを勘違いしてたのでこれで任意のタイミングで解放します。
void Player::End()
{
	DeleteGraph(m_lightPunchSeHandle);
	DeleteGraph(m_lightKickSeHandle);
	DeleteGraph(m_highPunchSeHandle);
	DeleteGraph(m_highKickSeHandle);
	DeleteGraph(m_downSeHandle);//倒れる音
	DeleteGraph(m_jumpSeHandle);//ジャンプの音
	DeleteGraph(m_jumpedSeHandle);//着地したときの音
	DeleteGraph(m_graspSeHandle);//つかむ音
	DeleteGraph(m_throwSeHandle);//投げる
	DeleteGraph(m_loseSeHandle);//負け
	DeleteGraph(m_winSeHandle);//勝ち
}

void Player::JumpSE()
{
	//ジャンプの音
	m_se->SetSE(m_jumpSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::JumpedSE()
{
	//着地後の音
	m_se->SetSE(m_jumpedSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::AttackSE()
{
	if (m_attackType == AttackTypes::LightPunch)
	{
		//弱パン
		m_se->SetSE(m_lightPunchSeHandle);
	}
	else if (m_attackType == AttackTypes::HighPunch)
	{
		//強パン
		m_se->SetSE(m_highPunchSeHandle);
	}
	else if (m_attackType == AttackTypes::LightKick)
	{
		//弱キック
		m_se->SetSE(m_lightKickSeHandle);
	}
	else if (m_attackType == AttackTypes::HighKick)
	{
		//強キック
		m_se->SetSE(m_highKickSeHandle);
	}
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::DownSE()
{
	//ダウンしたときの音
	m_se->SetSE(m_downSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::GraspSE()
{
	//つかみに成功した音
	m_se->SetSE(m_graspSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::ThrowSE()
{
	//投げる音
	m_se->SetSE(m_throwSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::LoseSE()
{
	//うーわ
	m_se->SetSE(m_loseSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::WinSE()
{
	//やった！
	m_se->SetSE(m_winSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}


//影
void Player::DrawShadow(const Camera& camera)
{
	//影
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	//楕円
	DrawOvalAA(m_pos.x + camera.m_drawOffset.x, 
		kShadowPosY + camera.m_drawOffset.y,
		kShadowRadiusX, kShadowRadiusY,20, 0x000000, true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void Player::PlayerInput(Input& input)
{
	m_lightPunchBottun.isPress = input.IsPress("X");
	m_highPunchBottun.isPress = input.IsPress("Y");
	m_lightKickBottun.isPress = input.IsPress("A");
	m_highKickBottun.isPress = input.IsPress("B");
	m_graspBottun.isPress = input.IsPress("RB");
	//指定した猶予フレームの間だけ押し続けても入力を残す
	CheckContinuePressBottun(m_lightPunchBottun);
	CheckContinuePressBottun(m_highPunchBottun);
	CheckContinuePressBottun(m_lightKickBottun);
	CheckContinuePressBottun(m_highKickBottun);
	CheckContinuePressBottun(m_graspBottun);
}

void Player::CheckContinuePressBottun(PressBottun& bottun)
{
	if (bottun.isPress)
	{
		++bottun.pressCountFrame;
		if (bottun.pressCountFrame > kRemainInputFrame)
		{
			bottun.isPress = false;
		}
	}
	else
	{
		bottun.pressCountFrame = 0;
	}
}

void Player::CheckGround()
{
	//地面の高さ
	if (m_pos.y >= kGroundHeight)
	{
		//位置を地面の高さに
		m_pos.y = kGroundHeight;
		m_isGround = true;
	}
	else
	{
		m_isGround = false;
	}
}

void Player::Gravity()
{
	//地上にいるときは重力をかけない
	if (m_isGround)
	{
		m_velocity.y = 0;
	}
	//空中にいるときは重力をかける
	else
	{
		m_velocity.y += kGravityPower;
	}
}

void Player::LookDir(Player& enemy)
{
	if (m_isGround)
	{
		if (m_pos.x > enemy.m_pos.x)
		{
			m_isLeft = true;
		}
		else if (m_pos.x < enemy.m_pos.x)
		{
			m_isLeft = false;
		}
	}
}

bool Player::CheckHit(Player& enemy)
{
	//攻撃の判定がないなら当たらない
	if (m_hitBoxAttack.x1 == 0 &&
		m_hitBoxAttack.y1 == 0 &&
		m_hitBoxAttack.x2 == 0 &&
		m_hitBoxAttack.y2 == 0)	return false;

	float width;
	float height;
	bool isHit = false;
	if (!m_isLeft)
	{
		//攻撃判定の中心を求める
		Vector3 attackPos((m_pos.x + m_hitBoxAttack.x2 + m_pos.x + m_hitBoxAttack.x1) / 2,
			(m_pos.y + m_hitBoxAttack.y2 + m_pos.y + m_hitBoxAttack.y1) / 2, 0.0f);

		//当たり判定があるならチェック
		//頭
		if (enemy.m_hitBoxHead.x1 != 0 ||
			enemy.m_hitBoxHead.y1 != 0 ||
			enemy.m_hitBoxHead.x2 != 0 ||
			enemy.m_hitBoxHead.y2 != 0)
		{
			//敵の頭のやられ判定////////////////////////////////////////
			Vector3 enemyHead((enemy.m_pos.x - enemy.m_hitBoxHead.x2 + enemy.m_pos.x - enemy.m_hitBoxHead.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxHead.y2 + enemy.m_pos.y + enemy.m_hitBoxHead.y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyHead.x);
			height = std::abs(attackPos.y - enemyHead.y);

			//X方向の攻撃の幅と体の幅を足した値より小さいなら当たってる
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.m_hitBoxHead.x2 - enemy.m_hitBoxHead.x1) / 2) &&
				//Y方向攻撃の幅と体の幅を足した値より小さいなら当たってる
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.m_hitBoxHead.y2 - enemy.m_hitBoxHead.y1) / 2)) return true;
			/////////////////////////////////////////////////////////////
		}
		

		//体
		if (enemy.m_hitBoxBody.x1 != 0 ||
			enemy.m_hitBoxBody.y1 != 0 ||
			enemy.m_hitBoxBody.x2 != 0 ||
			enemy.m_hitBoxBody.y2 != 0)
		{
			//敵の体のやられ判定////////////////////////////////////////////
			Vector3 enemyBody((enemy.m_pos.x - enemy.m_hitBoxBody.x2 + enemy.m_pos.x - enemy.m_hitBoxBody.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxBody.y2 + enemy.m_pos.y + enemy.m_hitBoxBody.y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyBody.x);
			height = std::abs(attackPos.y - enemyBody.y);

			//X方向の攻撃の幅と体の幅を足した値より小さいなら当たってる
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.m_hitBoxBody.x2 - enemy.m_hitBoxBody.x1) / 2) &&
				//Y方向攻撃の幅と体の幅を足した値より小さいなら当たってる
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.m_hitBoxBody.y2 - enemy.m_hitBoxBody.y1) / 2)) return true;
			///////////////////////////////////////////////////////////////
		}
		
		//足
		if (enemy.m_hitBoxLeg.x1 != 0 ||
			enemy.m_hitBoxLeg.y1 != 0 ||
			enemy.m_hitBoxLeg.x2 != 0 ||
			enemy.m_hitBoxLeg.y2 != 0)
		{
			//敵の足のやられ判定
			Vector3 enemyLeg((enemy.m_pos.x - enemy.m_hitBoxLeg.x2 + enemy.m_pos.x - enemy.m_hitBoxLeg.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxLeg.y2 + enemy.m_pos.y + enemy.m_hitBoxLeg.y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyLeg.x);
			height = std::abs(attackPos.y - enemyLeg.y);

			//X方向の攻撃の幅と体の幅を足した値より小さいなら当たってる
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.m_hitBoxLeg.x2 - enemy.m_hitBoxLeg.x1) / 2) &&
				//Y方向攻撃の幅と体の幅を足した値より小さいなら当たってる
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.m_hitBoxLeg.y2 - enemy.m_hitBoxLeg.y1) / 2)) return true;
		}
		
	}
	else
	{
		//攻撃判定の中心を求める
		Vector3 attackPos((m_pos.x - m_hitBoxAttack.x2 + m_pos.x - m_hitBoxAttack.x1) / 2,
			(m_pos.y + m_hitBoxAttack.y2 + m_pos.y + m_hitBoxAttack.y1) / 2, 0.0f);

		//当たり判定があるならチェック
		//頭
		if (enemy.m_hitBoxHead.x1 != 0 ||
			enemy.m_hitBoxHead.y1 != 0 ||
			enemy.m_hitBoxHead.x2 != 0 ||
			enemy.m_hitBoxHead.y2 != 0)
		{
			//敵の頭のやられ判定////////////////////////////////////////
			Vector3 enemyHead((enemy.m_pos.x + enemy.m_hitBoxHead.x2 + enemy.m_pos.x + enemy.m_hitBoxHead.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxHead.y2 + enemy.m_pos.y + enemy.m_hitBoxHead.y1) / 2, 0.0f);

			width = std::abs(attackPos.x - enemyHead.x);
			height = std::abs(attackPos.y - enemyHead.y);

			//X方向の攻撃の幅と頭の幅を足した値より小さいなら当たってる
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.m_hitBoxHead.x2 - enemy.m_hitBoxHead.x1) / 2) &&
				//X方向の攻撃の幅と頭の幅を足した値より小さいなら当たってる
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.m_hitBoxHead.y2 - enemy.m_hitBoxHead.y1) / 2)) return true;
			////////////////////////////////////////////////////////////
		}
	

		//体
		if (enemy.m_hitBoxBody.x1 != 0 ||
			enemy.m_hitBoxBody.y1 != 0 ||
			enemy.m_hitBoxBody.x2 != 0 ||
			enemy.m_hitBoxBody.y2 != 0)
		{
			//敵の頭のやられ判定///////////////////////////////////////
			Vector3 enemyBody((enemy.m_pos.x + enemy.m_hitBoxBody.x2 + enemy.m_pos.x + enemy.m_hitBoxBody.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxBody.y2 + enemy.m_pos.y + enemy.m_hitBoxBody.y1) / 2, 0.0f);

			width = std::abs(attackPos.x - enemyBody.x);
			height = std::abs(attackPos.y - enemyBody.y);

			//X方向の攻撃の幅と体の幅を足した値より小さいなら当たってる
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.m_hitBoxBody.x2 - enemy.m_hitBoxBody.x1) / 2) &&
				//Y方向攻撃の幅と体の幅を足した値より小さいなら当たってる
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.m_hitBoxBody.y2 - enemy.m_hitBoxBody.y1) / 2)) return true;
			/////////////////////////////////////////////////////////
		}
		
		//足
		if (enemy.m_hitBoxLeg.x1 != 0 ||
			enemy.m_hitBoxLeg.y1 != 0 ||
			enemy.m_hitBoxLeg.x2 != 0 ||
			enemy.m_hitBoxLeg.y2 != 0)
		{
			//敵の足のやられ判定
			Vector3 enemyLeg((enemy.m_pos.x + enemy.m_hitBoxLeg.x2 + enemy.m_pos.x + enemy.m_hitBoxLeg.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxLeg.y2 + enemy.m_pos.y + enemy.m_hitBoxLeg.y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyLeg.x);
			height = std::abs(attackPos.y - enemyLeg.y);

			//X方向の攻撃の幅と体の幅を足した値より小さいなら当たってる
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.m_hitBoxLeg.x2 - enemy.m_hitBoxLeg.x1) / 2) &&
				//Y方向攻撃の幅と体の幅を足した値より小さいなら当たってる
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.m_hitBoxLeg.y2 - enemy.m_hitBoxLeg.y1) / 2)) return true;
		}
	}


	//ここまで来たら当たってない
	return false;
}

bool Player::CheckHitGrasp(Player& enemy)
{
	//攻撃の判定がないなら当たらない
	if (m_hitBoxGrasp.x1 == 0 &&
		m_hitBoxGrasp.y1 == 0 &&
		m_hitBoxGrasp.x2 == 0 &&
		m_hitBoxGrasp.y2 == 0)	return false;

	float width;
	float height;
	bool isHit = false;

	
	if (!m_isLeft)
	{
		//攻撃判定の中心を求める
		Vector3 graspPos((m_pos.x + m_hitBoxGrasp.x2 + m_pos.x + m_hitBoxGrasp.x1) / 2,
			(m_pos.y + m_hitBoxGrasp.y2 + m_pos.y + m_hitBoxGrasp.y1) / 2, 0.0f);
		//投げやられ判定
		if (enemy.m_hitBoxThrow.x1 != 0 ||
			enemy.m_hitBoxThrow.y1 != 0 ||
			enemy.m_hitBoxThrow.x2 != 0 ||
			enemy.m_hitBoxThrow.y2 != 0)
		{
			//敵の投げのやられ判定
			Vector3 enemyThrow((enemy.m_pos.x - enemy.m_hitBoxThrow.x2 + enemy.m_pos.x - enemy.m_hitBoxThrow.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxThrow.y2 + enemy.m_pos.y + enemy.m_hitBoxThrow.y1) / 2, 0.0f);
			width = std::abs(graspPos.x - enemyThrow.x);
			height = std::abs(graspPos.y - enemyThrow.y);


			if (width < std::abs((m_hitBoxGrasp.x2 - m_hitBoxGrasp.x1) / 2) + std::abs((enemy.m_hitBoxThrow.x2 - enemy.m_hitBoxThrow.x1) / 2) &&
				height < std::abs((m_hitBoxGrasp.y2 - m_hitBoxGrasp.y1) / 2) + std::abs((enemy.m_hitBoxThrow.y2 - enemy.m_hitBoxThrow.y1) / 2)) return true;
		}
	}
	else
	{
		//攻撃判定の中心を求める
		Vector3 graspPos((m_pos.x - m_hitBoxGrasp.x2 + m_pos.x - m_hitBoxGrasp.x1) / 2,
			(m_pos.y + m_hitBoxGrasp.y2 + m_pos.y + m_hitBoxGrasp.y1) / 2, 0.0f);
		//投げやられ判定
		if (enemy.m_hitBoxThrow.x1 != 0 ||
			enemy.m_hitBoxThrow.y1 != 0 ||
			enemy.m_hitBoxThrow.x2 != 0 ||
			enemy.m_hitBoxThrow.y2 != 0)
		{
			//敵の投げのやられ判定
			Vector3 enemyThrow((enemy.m_pos.x + enemy.m_hitBoxThrow.x2 + enemy.m_pos.x + enemy.m_hitBoxThrow.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxThrow.y2 + enemy.m_pos.y + enemy.m_hitBoxThrow.y1) / 2, 0.0f);
			width = std::abs(graspPos.x - enemyThrow.x);
			height = std::abs(graspPos.y - enemyThrow.y);


			if (width < std::abs((m_hitBoxGrasp.x2 - m_hitBoxGrasp.x1) / 2) + std::abs((enemy.m_hitBoxThrow.x2 - enemy.m_hitBoxThrow.x1) / 2) &&
				height < std::abs((m_hitBoxGrasp.y2 - m_hitBoxGrasp.y1) / 2) + std::abs((enemy.m_hitBoxThrow.y2 - enemy.m_hitBoxThrow.y1) / 2)) return true;
		}
	}

	//ここまで来たら当たってない
	return false;
}

#if _DEBUG
//当たり判定を可視化
void Player::DrawHitBox(const Camera& camera)
{

	//当たり判定
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	if (!m_isLeft)
	{
		//押し合い判定
		DxLib::DrawBox(
			static_cast<int>(m_pos.x - m_pushBox.x1 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y - m_pushBox.y1 + camera.m_drawOffset.y),
			static_cast<int>(m_pos.x + m_pushBox.x2 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_pushBox.y2 + camera.m_drawOffset.y),
			0x00ff00, true);

		//頭
		DxLib::DrawBox(
			static_cast<int>(m_pos.x + m_hitBoxHead.x1 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxHead.y1 + camera.m_drawOffset.y),
			static_cast<int>(m_pos.x + m_hitBoxHead.x2 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxHead.y2 + camera.m_drawOffset.y),
			0x000fff, true);
		//胴体
		DxLib::DrawBox(
			static_cast<int>(m_pos.x + m_hitBoxBody.x1 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxBody.y1 + camera.m_drawOffset.y),
			static_cast<int>(m_pos.x + m_hitBoxBody.x2 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxBody.y2 + camera.m_drawOffset.y),
			0x0000ff, true);
		//足
		DxLib::DrawBox(
			static_cast<int>(m_pos.x + m_hitBoxLeg.x1 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxLeg.y1 + camera.m_drawOffset.y),
			static_cast<int>(m_pos.x + m_hitBoxLeg.x2 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxLeg.y2 + camera.m_drawOffset.y),
			0x0000ff, true);
		//投げつかみ判定 60x150
		DxLib::DrawBox(
			static_cast<int>(m_pos.x + m_hitBoxThrow.x1 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxThrow.y1 + camera.m_drawOffset.y),
			static_cast<int>(m_pos.x + m_hitBoxThrow.x2 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxThrow.y2 + camera.m_drawOffset.y),
			0xffffff, true);
		if (m_isCheckAttackBox)
		{
			//攻撃判定
			DxLib::DrawBox(
				static_cast<int>(m_pos.x + m_hitBoxAttack.x1 + camera.m_drawOffset.x),
				static_cast<int>(m_pos.y + m_hitBoxAttack.y1 + camera.m_drawOffset.y),
				static_cast<int>(m_pos.x + m_hitBoxAttack.x2 + camera.m_drawOffset.x),
				static_cast<int>(m_pos.y + m_hitBoxAttack.y2 + camera.m_drawOffset.y),
				0xff0000, true);
			//攻撃判定
			DxLib::DrawBox(
				static_cast<int>(m_pos.x + m_hitBoxGrasp.x1 + camera.m_drawOffset.x),
				static_cast<int>(m_pos.y + m_hitBoxGrasp.y1 + camera.m_drawOffset.y),
				static_cast<int>(m_pos.x + m_hitBoxGrasp.x2 + camera.m_drawOffset.x),
				static_cast<int>(m_pos.y + m_hitBoxGrasp.y2 + camera.m_drawOffset.y),
				0xffff00, true);
		}
	}
	else
	{
		//押し合い判定
		DxLib::DrawBox(
			(static_cast<int>(m_pos.x) - m_pushBox.x1) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) - m_pushBox.y1) + camera.m_drawOffset.y,
			(static_cast<int>(m_pos.x) + m_pushBox.x2) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_pushBox.y2) + camera.m_drawOffset.y,
			0x00ff00, true);

		//頭
		DxLib::DrawBox(
			(static_cast<int>(m_pos.x) - m_hitBoxHead.x1) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxHead.y1) + camera.m_drawOffset.y,
			(static_cast<int>(m_pos.x) - m_hitBoxHead.x2) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxHead.y2) + camera.m_drawOffset.y,
			0x000fff, true);
		//胴体
		DxLib::DrawBox(
			(static_cast<int>(m_pos.x) - m_hitBoxBody.x1) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxBody.y1) + camera.m_drawOffset.y,
			(static_cast<int>(m_pos.x) - m_hitBoxBody.x2) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxBody.y2) + camera.m_drawOffset.y,
			0x0000ff, true);
		//足
		DxLib::DrawBox(
			(static_cast<int>(m_pos.x) - m_hitBoxLeg.x1) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxLeg.y1) + camera.m_drawOffset.y,
			(static_cast<int>(m_pos.x) - m_hitBoxLeg.x2) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxLeg.y2) + camera.m_drawOffset.y,
			0x0000ff, true);
		//投げつかみ判定
		DxLib::DrawBox(
			(static_cast<int>(m_pos.x) - m_hitBoxThrow.x1) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxThrow.y1) + camera.m_drawOffset.y,
			(static_cast<int>(m_pos.x) - m_hitBoxThrow.x2) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxThrow.y2) + camera.m_drawOffset.y,
			0xffffff, true);
		if (m_isCheckAttackBox)
		{
			//攻撃判定
			DxLib::DrawBox(
				(static_cast<int>(m_pos.x) - m_hitBoxAttack.x1) + camera.m_drawOffset.x,
				(static_cast<int>(m_pos.y) + m_hitBoxAttack.y1) + camera.m_drawOffset.y,
				(static_cast<int>(m_pos.x) - m_hitBoxAttack.x2) + camera.m_drawOffset.x,
				(static_cast<int>(m_pos.y) + m_hitBoxAttack.y2) + camera.m_drawOffset.y,
				0xff0000, true);

			//攻撃判定
			DxLib::DrawBox(
				(static_cast<int>(m_pos.x) - m_hitBoxGrasp.x1) + camera.m_drawOffset.x,
				(static_cast<int>(m_pos.y) + m_hitBoxGrasp.y1) + camera.m_drawOffset.y,
				(static_cast<int>(m_pos.x) - m_hitBoxGrasp.x2) + camera.m_drawOffset.x,
				(static_cast<int>(m_pos.y) + m_hitBoxGrasp.y2) + camera.m_drawOffset.y,
				0xffff00, true);
		}
		
	}
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 150);
}
#endif

//立ちくらい
void Player::LoadStateHit()
{
	//やられモーション
	m_chara->GetAnimHitStand(*this);
	assert(m_handle != -1);
	//喰らい判定
	m_chara->GetHitBoxHitStand(*this);
	//喰らい
	m_update = &Player::DamageUpdate;
	m_draw = &Player::DamageDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
}
//投げられ
void Player::LoadStateBeThrown()
{
	//やられモーション
	m_chara->GetAnimHitStand(*this);
	assert(m_handle != -1);
	//投げられ中は当たり判定を消す
	m_chara->GetHitBoxBeThrown(*this);
	ResetPushBox();
	//喰らい
	m_update = &Player::BeThrownUpdate;
	m_draw = &Player::BeThrownDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
}

void Player::LoadStateThrowEscape()
{
	//やられモーション
	m_chara->GetAnimHitStand(*this);
	assert(m_handle != -1);
	//投げ抜け
	m_update = &Player::ThrowEscapeUpdate;
	m_draw = &Player::ThrowEscapeDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
	m_isThrowSuccess = false;
}

//立ちガード
void Player::LoadStateGuardStand()
{
	//ガードモーション
	m_chara->GetAnimGuardStand(*this);
	assert(m_handle != -1);
	//喰らい判定
	m_chara->GetHitBoxGuardStand(*this);
	//ガード
	m_update = &Player::GuardStandUpdate;
	m_draw = &Player::GuardStandDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
}
//しゃがみガード
void Player::LoadStateGuardSquat()
{
	//ガードモーション
	m_chara->GetAnimGuardSquat(*this);
	assert(m_handle != -1);
	//喰らい判定
	m_chara->GetHitBoxGuardSquat(*this);
	//ガード
	m_update = &Player::GuardSquatUpdate;
	m_draw = &Player::GuardSquatDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
}


//ダウン
void Player::LoadStateDown()
{
	//ダウンモーション
	m_chara->GetAnimDown(*this);
	assert(m_handle != -1);
	//ヒットボックス
	m_chara->GetHitBoxDown(*this);
	//ダウン
	m_update = &Player::DownUpdate;
	m_draw = &Player::DownDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
	m_noActFrame = kDownFrame;
}

void Player::LoadStateDownAerial()
{
	//ダウンモーション
	m_chara->GetAnimDownAerial(*this);
	assert(m_handle != -1);
	//ヒットボックス
	m_chara->GetHitBoxDownAerial(*this);
	//ダウン
	m_update = &Player::DownAerialUpdate;
	m_draw = &Player::DownAerialDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
	m_noActFrame = kDownFrame;
}


void Player::Cancel(Input& input, Player enemy, std::shared_ptr<Bullet> myBullet, GameManager& gameManager)
{
	//キャンセル可能になったら
	if (m_isPossibleCancel)
	{
		m_cancelFrameCount++;
		//キャンセル可能猶予以内で入力が入ったら
		if (m_cancelFrameCount <= kCanselGraceFrame)
		{
			////////////////////攻撃//////////////////////////////////////
			//弱パンチ
			if (m_lightPunchBottun.isPress)
			{
				m_attackType = AttackTypes::LightPunch;
			}
			//強パンチ
			if (m_highPunchBottun.isPress)
			{
				m_attackType = AttackTypes::HighPunch;
			}
			//弱キック
			if (m_lightKickBottun.isPress)
			{
				m_attackType = AttackTypes::LightKick;
			}
			//強キック
			if (m_highKickBottun.isPress)
			{
				m_attackType = AttackTypes::HighKick;
			}
			//なんか攻撃してたら
			if (m_attackType != AttackTypes::Null)
			{
				//コマンドをチェックして成立している技を取得
				if (m_chara->SuccessCommandCheck(input, *this))
				{
					m_isPossibleCancel = false;
					//コマンド技
					//アニメーションの取得
					m_chara->GetAnimCommand(*this);
					assert(m_handle != -1);
					//やられ判定
					m_chara->GetHitBoxCommand(*this);
					//コマンド技のダメージとふっとばしと硬直差を取得
					m_chara->GetGiveEffectCommand(*this);
					m_update = &Player::CommandUpdate;
					m_draw = &Player::CommandDraw;
					m_animIndex = 0;
					m_animCountFrame = 0;
					(this->*m_update)(input, enemy, myBullet, gameManager);
					return;
				}
				else
				{
					m_attackType = AttackTypes::Null;
				}
			}
		}
		else
		{
			m_isPossibleCancel = false;
		}
	}
	else
	{
		m_cancelFrameCount = 0;
	}
}

void Player::InputDelay(Input& input, Player enemy, std::shared_ptr<Bullet> myBullet, GameManager& gameManager)
{
	//通常攻撃の最初の数Fの間でコマンドが成立していたら上書きする
	if (m_animCountFrame <= kInputDelayFrame)
	{
		//コマンドをチェックして成立している技を取得
		if (m_chara->SuccessCommandCheck(input, *this))
		{
			m_isPossibleCancel = false;
			//コマンド技
			//アニメーションの取得
			m_chara->GetAnimCommand(*this);
			assert(m_handle != -1);
			//やられ判定
			m_chara->GetHitBoxCommand(*this);
			//コマンド技のダメージとふっとばしと硬直差を取得
			m_chara->GetGiveEffectCommand(*this);
			m_update = &Player::CommandUpdate;
			m_draw = &Player::CommandDraw;
			m_animIndex = 0;
			m_animCountFrame = 0;
			(this->*m_update)(input, enemy, myBullet, gameManager);
			return;
		}
	}
}

bool Player::CheckDead()
{
	//体力がなくなったら
	if (m_hp <= 0)
	{
		++m_angle;
		//うーわ
		LoseSE();
		//吹っ飛んでほしい
		//押し合い判定を消す(画面外に行くために)
		ResetPushBox();
		//画面外に吹っ飛ぶ
		if ((0 - kCharaWidth) < m_pos.x && m_pos.x < (kStageWidth + kCharaWidth))
		{
			m_velocity.x *= 1.05f;
			m_velocity.y -= 3.0f;
		}
		else
		{
			//画面外に行ったら0にする
			m_velocity.x = 0;
			m_velocity.y = 0;
		}
		//HPを0より低くしないために
		m_hp = 0;
		return true;
	}
	else
	{
		m_angle = 0.0f;
	}
	return false;
}

void Player::InitPushBox()
{
	if (m_isLeft) {
		m_pushBox.x1 = kBaseCollisionWidthFront;
		m_pushBox.y1 = kBaseCollisionWidthTop;
		m_pushBox.x2 = kBaseCollisionWidthBack;
		m_pushBox.y2 = kBaseCollisionWidthBottom;
	}
	else {
		m_pushBox.x1 = kBaseCollisionWidthBack;
		m_pushBox.y1 = kBaseCollisionWidthTop;
		m_pushBox.x2 = kBaseCollisionWidthFront;
		m_pushBox.y2 = kBaseCollisionWidthBottom;
	}
}



void Player::IdleStandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//攻撃判定リセット
	ResetAttackBox();
	ResetGraspBox();
	//相手の方向を向く
	LookDir(enemy);

	//移動量リセット
	m_velocity.x = 0;
	//リセット
	m_giveThrowVelo.x = 0;
	m_giveThrowVelo.y = 0;
	//攻撃の手段リセット
	m_attackType = AttackTypes::Null;
	//攻撃の属性リセット
	m_attackAttributes = AttackAttributes::Null;
	//直前のハンドルを覚える
	int lastHandle = m_handle;
	//しゃがんでいないのでfalse
	m_isSquat = false;
	//ガードリセット
	m_isGuard = false;
	//リセット
	InitPushBox();//押し合い判定を戻す
	m_isHitGrasp = false;
	m_isThrown = false;
	m_isThrowSuccess = false;
	//コマンド技
	m_isCommand = false;

	//相手の体力がなくなったらまたは時間切れ
	if ((enemy.GetHp() <= 0) && (m_hp > 0) || gameManager.GetTimer() <= 0)
	{
		m_update = &Player::ResultUpdate;
		m_draw = &Player::ResultDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet,gameManager);
		return;
	}

	//プレイヤーが相手の方向と反対にスティックを倒したときガード準備
	if (input.IsPress("Right") && m_isLeft || input.IsPress("Left") && !m_isLeft)
	{
		m_isGuard = true;
	}
	else
	{
		m_isGuard = false;
	}

	//下にスティックを倒したとき
	if (input.IsPress("Down"))
	{
		//しゃがみ
		m_update = &Player::IdleSquatUpdate;
		m_draw = &Player::IdleSquatDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}

	////////////////////攻撃//////////////////////////////////////
	//両方trueなら投げ
	bool isLightPunch = false;
	bool isLightKick = false;
		//弱パンチ
	if (m_lightPunchBottun.isPress)
	{
		m_attackType = AttackTypes::LightPunch;
		isLightPunch = true;
	}
	//強パンチ
	if (m_highPunchBottun.isPress)
	{
		m_attackType = AttackTypes::HighPunch;
	}
	//弱キック
	if (m_lightKickBottun.isPress)
	{
		m_attackType = AttackTypes::LightKick;
		isLightKick = true;
	}
	//強キック
	if (m_highKickBottun.isPress)
	{
		m_attackType = AttackTypes::HighKick;
	}
	//ワンボタン投げ
	if (m_graspBottun.isPress)
	{
		isLightPunch = true;
		isLightKick = true;
	}
	//弱パンチと弱キック同時押しで投げ
	if (isLightPunch && isLightKick)
	{
		m_update = &Player::GraspUpdate;
		m_draw = &Player::GraspDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet,gameManager);
		return;
	}


	//なんか攻撃してたら
	if (m_attackType != AttackTypes::Null)
	{
		//SEの再生ストップ
		m_se->Stop();
		//コマンドをチェックして成立している技を取得
		if (m_chara->SuccessCommandCheck(input, *this))
		{
			//コマンド技
			//アニメーションの取得
			m_chara->GetAnimCommand(*this);
			assert(m_handle != -1);
			//やられ判定
			m_chara->GetHitBoxCommand(*this);
			//コマンド技のダメージとふっとばしと硬直差を取得
			m_chara->GetGiveEffectCommand(*this);
			m_update = &Player::CommandUpdate;
			m_draw = &Player::CommandDraw;
			m_animIndex = 0;
			m_animCountFrame = 0;
			(this->*m_update)(input, enemy, myBullet, gameManager);
			return;
		}
		else
		{
			m_update = &Player::AttackStandUpdate;
			m_draw = &Player::AttackStandDraw;
			m_animIndex = 0;
			m_animCountFrame = 0;
			(this->*m_update)(input, enemy, myBullet,gameManager);
			return;
		}
	}


	////////////////////移動//////////////////////////////////////
	//右にスティックを倒したとき
	//if ((input.GetStickInfo().leftStickX > 300))
	if (input.IsPress("Right"))
	{
		//右を向いてるなら
		if (!m_isLeft)
		{
			//前歩き
			m_chara->GetAnimWalkFront(*this);
			assert(m_handle != -1);
			//当たり判定
			m_chara->GetHitBoxWalkFront(*this);
			//右に進む
			//スト2同様に相手に向かって進むスピードを大きくする
			m_velocity.x = kMoveSpeedFront;
		}
		else
		{
			//後ろ歩き
			m_chara->GetAnimWalkBack(*this);
			assert(m_handle != -1);
			//当たり判定
			m_chara->GetHitBoxWalkBack(*this);
			//右に進む
			//スト2同様に相手から離れるスピードを小さくする
			m_velocity.x = kMoveSpeedBack;
		}
	}
	//左にスティックを倒したとき	かつしゃがんでないとき
	else if (input.IsPress("Left")) {
		//右を向いてるなら
		if (!m_isLeft)
		{
			//後ろ歩き
			m_chara->GetAnimWalkBack(*this);
			assert(m_handle != -1);
			//当たり判定
			m_chara->GetHitBoxWalkBack(*this);
			//右に進む
			//スト2同様に相手から離れるスピードを小さくする
			m_velocity.x = -kMoveSpeedBack;
		}
		else
		{
			//前歩き
			m_chara->GetAnimWalkFront(*this);
			assert(m_handle != -1);
			//当たり判定
			m_chara->GetHitBoxWalkFront(*this);
			//右に進む
			//スト2同様に相手から離れるスピードを大きくする
			m_velocity.x = -kMoveSpeedFront;
		}
	}
	else
	{
		//立ち待機
		m_chara->GetAnimIdleStand(*this);
		assert(m_handle != -1);
		//当たり判定
		m_chara->GetHitBoxIdleStand(*this);
	}
	//切り替わったらリセット
	//直前のハンドルと切り替わった時にアニメーションを最初から再生する
	if (lastHandle != m_handle)
	{
		m_animIndex = 0;
		m_animCountFrame = 0;
	}

	//上にスティックを倒したとき
	if (input.IsPress("Up"))
	{
		//ジャンプの方向を決める
		if (m_velocity.x > 0)//右
		{
			m_jumpVelo.x = kJumpPowerX;
		}
		else if (m_velocity.x < 0)//左
		{
			m_jumpVelo.x = -kJumpPowerX;
		}
		else//真上
		{
			m_jumpVelo.x = 0;
		}
		
		//ジャンプ
		JumpSE();
		m_update = &Player::JumpUpdate;
		m_draw = &Player::JumpDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		return;
	}
	

	//戦闘開始前のUpdateを止めてるときにこのアニメーションだけは止めたくないのでここに書く
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = 0;
		}
	}
	m_animCountFrame++;
}

void Player::IdleStandDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::IdleSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//攻撃判定リセット
	ResetAttackBox();
	ResetGraspBox();
	//相手の方向を向く
	LookDir(enemy);
	//移動量リセット
	m_velocity.x = 0;
	//リセット
	m_giveThrowVelo.x = 0;
	m_giveThrowVelo.y = 0;
	//攻撃の手段リセット
	m_attackType = AttackTypes::Null;
	//攻撃の属性リセット
	m_attackAttributes = AttackAttributes::Null;
	//しゃがみ待機
	m_chara->GetAnimIdleSquat(*this);
	assert(m_handle != -1);
	//当たり判定
	m_chara->GetHitBoxIdleSquat(*this);
	//しゃがんでるのでtrue
	m_isSquat = true;
	//ガードリセット
	m_isGuard = false;
	//リセット
	InitPushBox();//押し合い判定を戻す
	m_isHitGrasp = false;
	m_isThrown = false;
	m_isThrowSuccess = false;
	//コマンド技
	m_isCommand = false;

	//相手の体力がなくなったらまたは時間切れ
	if ((enemy.GetHp() <= 0) && (m_hp > 0) || gameManager.GetTimer() <= 0)
	{
		m_update = &Player::ResultUpdate;
		m_draw = &Player::ResultDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}

	//プレイヤーが相手の方向と反対にスティックを倒したときガード準備
	if (input.IsPress("Right") && m_isLeft || input.IsPress("Left") && !m_isLeft)
	{
		m_isGuard = true;
	}
	else
	{
		m_isGuard = false;
	}

	/////攻撃////////

	//両方trueなら投げ
	bool isLightPunch = false;
	bool isLightKick = false;
	//弱パンチ
	if (m_lightPunchBottun.isPress)
	{
		m_attackType = AttackTypes::LightPunch;
		isLightPunch = true;
	}
	//強パンチ
	if (m_highPunchBottun.isPress)
	{
		m_attackType = AttackTypes::HighPunch;
	}
	//弱キック
	if (m_lightKickBottun.isPress)
	{
		m_attackType = AttackTypes::LightKick;
		isLightKick = true;
	}
	//強キック
	if (m_highKickBottun.isPress)
	{
		m_attackType = AttackTypes::HighKick;
	}
	//ワンボタン投げ
	if (m_graspBottun.isPress)
	{
		isLightPunch = true;
		isLightKick = true;
	}
	
	//弱パンチと弱キック同時押しで投げ
	if (isLightPunch && isLightKick)
	{
		m_update = &Player::GraspUpdate;
		m_draw = &Player::GraspDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet,gameManager);
		return;
	}

	//なんか攻撃してたら
	if (m_attackType != AttackTypes::Null)
	{
		//コマンドが成立しているか
		if (m_chara->SuccessCommandCheck(input, *this))
		{
			//コマンド技
			//アニメーションの取得
			m_chara->GetAnimCommand(*this);
			assert(m_handle != -1);
			//やられ判定
			m_chara->GetHitBoxCommand(*this);
			//コマンド技のダメージとふっとばしと硬直差を取得
			m_chara->GetGiveEffectCommand(*this);
			m_update = &Player::CommandUpdate;
			m_draw = &Player::CommandDraw;
			m_animIndex = 0;
			m_animCountFrame = 0;
			(this->*m_update)(input, enemy, myBullet, gameManager);
			return;
		}
		else
		{
			m_update = &Player::AttackSquatUpdate;
			m_draw = &Player::AttackSquatDraw;
			m_animIndex = 0;
			m_animCountFrame = 0;
			(this->*m_update)(input, enemy, myBullet, gameManager);
			return;
		}
	}
	////移動/////

		//上にスティックを倒したとき
	if (input.IsPress("Up"))
	{
		//ジャンプの方向を決める
		if (m_velocity.x > 0)//右
		{
			m_jumpVelo.x = kJumpPowerX;
		}
		else if (m_velocity.x < 0)//左
		{
			m_jumpVelo.x = -kJumpPowerX;
		}
		else//真上
		{
			m_jumpVelo.x = 0;
		}
		//ジャンプ
		m_update = &Player::JumpUpdate;
		m_draw = &Player::JumpDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		//ジャンプだけすぐ実行しない（バグるから）
		return;
	}
	//スティックを倒してないとき
	if (!input.IsPress("Down"))
	{
		//立ち
		m_update = &Player::IdleStandUpdate;
		m_draw = &Player::IdleStandDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		//状態が切り替わったらこのフレームでその状態を実行したい
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}


	m_animCountFrame++;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = 0;
		}
	}
}

void Player::IdleSquatDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::JumpUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//攻撃判定リセット
	ResetAttackBox();
	ResetGraspBox();
	//フレームを数える
	m_animCountFrame++;
	//ガードできない
	m_isGuard = false;
	//ジャンプ
	m_chara->GetAnimJump(*this);
	assert(m_handle != -1);
	//当たり判定
	m_chara->GetHitBoxJump(*this);
	//コマンド技
	m_isCommand = false;

	//最初の4フレームは地上
	if (m_animCountFrame < kJumpTransFrame)
	{
		m_velocity.x = 0;
		//弱パンチ
		if (m_lightPunchBottun.isPress)
		{
			m_attackType = AttackTypes::LightPunch;
		}
		//強パンチ
		if (m_highPunchBottun.isPress)
		{
			m_attackType = AttackTypes::HighPunch;
		}
		//弱キック
		if (m_lightKickBottun.isPress)
		{
			m_attackType = AttackTypes::LightKick;
		}
		//強キック
		if (m_highKickBottun.isPress)
		{
			m_attackType = AttackTypes::HighKick;
		}
		//なんか攻撃してたら
		if (m_attackType != AttackTypes::Null)
		{
			//コマンドをチェックして成立している技を取得
			if (m_chara->SuccessCommandCheck(input, *this))
			{
				//コマンド技
				//アニメーションの取得
				m_chara->GetAnimCommand(*this);
				assert(m_handle != -1);
				//やられ判定
				m_chara->GetHitBoxCommand(*this);
				//コマンド技のダメージとふっとばしと硬直差を取得
				m_chara->GetGiveEffectCommand(*this);
				//コマンド技
				m_update = &Player::CommandUpdate;
				m_draw = &Player::CommandDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy, myBullet, gameManager);
				return;
			}
			else
			{
				m_update = &Player::AttackStandUpdate;
				m_draw = &Player::AttackStandDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy, myBullet, gameManager);
				return;
			}
		}
	}
	//5フレームからジャンプ
	else if (m_animCountFrame == kJumpTransFrame)
	{
		m_velocity = m_jumpVelo;
	}
	else
	{
		//地上についたら
		if (m_isGround)
		{
			//着地後
			JumpedSE();
			m_update = &Player::IdleStandUpdate;
			m_draw = &Player::IdleStandDraw;
			m_animIndex = 0;
			m_animCountFrame = 0;
			//一フレーム遅らせる（実質後隙）
			return;
		}
		//弱パンチ
		if (m_lightPunchBottun.isPress)
		{
			m_attackType = AttackTypes::LightPunch;
		}
		//強パンチ
		if (m_highPunchBottun.isPress)
		{
			m_attackType = AttackTypes::HighPunch;
		}
		//弱キック
		if (m_lightKickBottun.isPress)
		{
			m_attackType = AttackTypes::LightKick;
		}
		//強キック
		if (m_highKickBottun.isPress)
		{
			m_attackType = AttackTypes::HighKick;
		}
		//なんか攻撃してたら
		if (m_attackType != AttackTypes::Null)
		{
			m_update = &Player::AttackAerialUpdate;
			m_draw = &Player::AttackAerialDraw;
			m_animIndex = 0;
			m_animCountFrame = 0;
			(this->*m_update)(input, enemy, myBullet, gameManager);
			return;
		}
	}


	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
#if _DEBUG
	//デバッグ
	debugJumpFrame = m_animCountFrame;
#endif

}

void Player::JumpDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::AttackStandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	
	//ガードできない
	m_isGuard = false;
	
	if (m_attackType == AttackTypes::LightPunch)
	{
		//左右の動きを止める
		m_velocity.x = 0;
		//弱パンチ
		m_chara->GetAnimPunchLight(*this);
		assert(m_handle != -1);
		//弱パンチモーションの当たり判定を取得
		m_chara->GetHitBoxPunchLight(*this);
		//弱パンチのダメージとふっとばしと硬直差を取得
		m_chara->GetGiveEffectPunchLight(*this);
	}
	else if (m_attackType == AttackTypes::HighPunch)
	{
		//少し前進する
		m_velocity.x = 1.0f;//右
		if (m_isLeft)
		{
			m_velocity.x = -1.0f;//左
		}
		
		//強パンチ
		m_chara->GetAnimPunchHigh(*this);
		assert(m_handle != -1);
		//強パンチモーションの当たり判定を取得
		m_chara->GetHitBoxPunchHigh(*this);
		//強パンチのダメージとふっとばしと硬直差を取得
		m_chara->GetGiveEffectPunchHigh(*this);
	}
	else if (m_attackType == AttackTypes::LightKick)
	{
		//左右の動きを止める
		m_velocity.x = 0;
		//弱キック
		m_chara->GetAnimKickLight(*this);
		assert(m_handle != -1);
		//弱キックモーションの当たり判定を取得
		m_chara->GetHitBoxKickLight(*this);
		//弱キックのダメージとふっとばしと硬直差を取得
		m_chara->GetGiveEffectKickLight(*this);
	}
	else if (m_attackType == AttackTypes::HighKick)
	{
		//少し前進する
		m_velocity.x = 1.0f;//右
		if (m_isLeft)
		{
			m_velocity.x = -1.0f;//左
		}
		//強キック
		m_chara->GetAnimKickHigh(*this);
		assert(m_handle != -1);
		//強キックモーションの当たり判定を取得
		m_chara->GetHitBoxKickHigh(*this);
		//強キックのダメージとふっとばしと硬直差を取得
		m_chara->GetGiveEffectKickHigh(*this);
	}
	//SE再生
	AttackSE();

	m_animCountFrame++;
	//最初の2フレームの間にコマンドが成立しているなら上書き
	InputDelay(input, enemy, myBullet, gameManager);

	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			//再生を止める
			m_se->Stop();
			//スティックを下に倒してる
			if (input.IsPress("Down"))
			{
				//しゃがみ
				m_update = &Player::IdleSquatUpdate;
				m_draw = &Player::IdleSquatDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy,myBullet, gameManager);
				return;
			}
			else
			{
				//立ち
				m_update = &Player::IdleStandUpdate;
				m_draw = &Player::IdleStandDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy,myBullet, gameManager);
				return;
			}
		}
	}
	
	//キャンセル
	Cancel(input, enemy, myBullet, gameManager);
#if _DEBUG
	m_isCheckAttackBox = false;
#endif
	//フレームが攻撃持続フレーム以内の時
	if (m_startAttackFrame <= m_animCountFrame && m_animCountFrame <= m_finishAttackFrame)
	{
#if _DEBUG
		m_isCheckAttackBox = true;
#endif
		//攻撃が当たったかどうか
		m_isHitAttack = CheckHit(enemy);
		if (m_isHitAttack)
		{
			//ヒットストップ
			gameManager.OnIsHitStop();
			//キャンセル可能にする
			m_isPossibleCancel = true;
			m_cancelFrameCount = 0;
#if _DEBUG
			DrawString(600, 200, "当たった\n", 0xff0000);
#endif
			//リセット
			m_attackType = AttackTypes::Null;
		}
	}

	//攻撃持続終わり
	if (m_animCountFrame > m_finishAttackFrame)
	{
		//当たり判定リセット
		m_attackType = AttackTypes::Null;
		ResetAttackBox();
	}


}

void Player::AttackStandDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::AttackSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//ガードできない
	m_isGuard = false;
	if (m_attackType == AttackTypes::LightPunch)
	{
		//左右の動きを止める
		m_velocity.x = 0;
		//しゃがみ弱パンチ
		m_chara->GetAnimPunchLightSquat(*this);
		assert(m_handle != -1);
		//しゃがみ弱パンチモーションの当たり判定を取得
		m_chara->GetHitBoxPunchLightSquat(*this);
		//しゃがみ弱パンチのダメージとふっとばしと硬直差を取得
		m_chara->GetGiveEffectPunchLightSquat(*this);
	}
	else if (m_attackType == AttackTypes::HighPunch)
	{
		//左右の動きを止める
		m_velocity.x = 0;
		//しゃがみ強パンチ
		m_chara->GetAnimPunchHighSquat(*this);
		assert(m_handle != -1);
		//しゃがみ強パンチモーションの当たり判定を取得
		m_chara->GetHitBoxPunchHighSquat(*this);
		//しゃがみ強パンチのダメージとふっとばしと硬直差を取得
		m_chara->GetGiveEffectPunchHighSquat(*this);
	}
	else if (m_attackType == AttackTypes::LightKick)
	{
		//左右の動きを止める
		m_velocity.x = 0;
		//しゃがみ弱キック
		m_chara->GetAnimKickLightSquat(*this);
		assert(m_handle != -1);
		//しゃがみ弱キックモーションの当たり判定を取得
		m_chara->GetHitBoxKickLightSquat(*this);
		// しゃがみ弱キックのダメージと硬直差を取得
		m_chara->GetGiveEffectKickLightSquat(*this);
	}
	else if (m_attackType == AttackTypes::HighKick)
	{
		//左右の動きを止める
		m_velocity.x = 0;
		//強キック
		m_chara->GetAnimKickHighSquat(*this);
		assert(m_handle != -1);
		//強キックモーションの当たり判定を取得
		m_chara->GetHitBoxKickHighSquat(*this);
		//強キックのダメージとふっとばしと硬直差を取得
		m_chara->GetGiveEffectKickHighSquat(*this);
	}
	//SE再生
	AttackSE();

	m_animCountFrame++;
	//最初の2フレームの間にコマンドが成立しているなら上書き
	InputDelay(input, enemy, myBullet, gameManager);
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			//再生を止める
			m_se->Stop();
			//スティックを下に倒してる
			if (input.IsPress("Down"))
			{
				//しゃがみ
				m_update = &Player::IdleSquatUpdate;
				m_draw = &Player::IdleSquatDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy,myBullet, gameManager);
				return;
			}
			else
			{
				//立ち
				m_update = &Player::IdleStandUpdate;
				m_draw = &Player::IdleStandDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy,myBullet, gameManager);
				return;
			}
		}
	}
	

	//キャンセル
	Cancel(input, enemy, myBullet, gameManager);

#if _DEBUG
	m_isCheckAttackBox = false;
#endif
	//フレームが攻撃持続フレーム以内の時
	if (m_startAttackFrame <= m_animCountFrame && m_animCountFrame <= m_finishAttackFrame)
	{
#if _DEBUG
		m_isCheckAttackBox = true;
#endif
		//攻撃が当たったかどうか
		m_isHitAttack = CheckHit(enemy);
		if (m_isHitAttack)
		{
			//ヒットストップ
			gameManager.OnIsHitStop();
			//キャンセル可能にする
			m_isPossibleCancel = true;
			m_cancelFrameCount = 0;

			//リセット
			m_attackType = AttackTypes::Null;
			//ResetAttackBox();
		}
	}
	//攻撃持続終わり
	if (m_animCountFrame > m_finishAttackFrame)
	{
		//当たり判定リセット
		m_attackType = AttackTypes::Null;
		ResetAttackBox();
	}
}

void Player::AttackSquatDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::AttackAerialUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//地面についたら
	if (m_isGround)
	{
		//着地後
		JumpedSE();
		m_attackType = AttackTypes::Null;
		ResetAttackBox();
		m_update = &Player::IdleStandUpdate;
		m_draw = &Player::IdleStandDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}

	//ガードできない
	m_isGuard = false;
	if (m_attackType == AttackTypes::LightPunch)
	{
		//空中弱パンチ
		m_chara->GetAnimPunchLightAerial(*this);
		assert(m_handle != -1);
		//空中弱パンチモーションの当たり判定を取得
		m_chara->GetHitBoxPunchLightAerial(*this);
		//空中弱パンチのダメージとふっとばしと硬直差を取得
		m_chara->GetGiveEffectPunchLightAerial(*this);
	}
	else if (m_attackType == AttackTypes::HighPunch)
	{
		//空中強パンチ
		m_chara->GetAnimPunchHighAerial(*this);
		assert(m_handle != -1);
		//空中強パンチモーションの当たり判定を取得
		m_chara->GetHitBoxPunchHighAerial(*this);
		//空中強パンチのダメージとふっとばしと硬直差を取得
		m_chara->GetGiveEffectPunchHighAerial(*this);
	}
	else if (m_attackType == AttackTypes::LightKick)
	{
		//空中弱キック
		m_chara->GetAnimKickLightAerial(*this);
		assert(m_handle != -1);
		//空中弱キックモーションの当たり判定を取得
		m_chara->GetHitBoxKickLightAerial(*this);
		//空中弱キックのダメージとふっとばしと硬直差を取得
		m_chara->GetGiveEffectKickLightAerial(*this);
	}
	else if (m_attackType == AttackTypes::HighKick)
	{
		//空中強キック
		m_chara->GetAnimKickHighAerial(*this);
		assert(m_handle != -1);
		//空中強キックモーションの当たり判定を取得
		m_chara->GetHitBoxKickHighAerial(*this);
		//空中強キックのダメージとふっとばしと硬直差を取得
		m_chara->GetGiveEffectKickHighAerial(*this);
	}

	m_animCountFrame++;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
			m_attackType = AttackTypes::Null;
			m_update = &Player::AttackedAerialUpdate;
			m_draw = &Player::AttackedAerialDraw;
			m_animIndex = 0;
			m_animCountFrame = 0;
			(this->*m_update)(input, enemy, myBullet, gameManager);
			return;
		}
	}

#if _DEBUG
	m_isCheckAttackBox = false;
#endif
	//フレームが攻撃持続フレーム以内の時
	if (m_startAttackFrame <= m_animCountFrame && m_animCountFrame <= m_finishAttackFrame)
	{
		m_animIndex = m_animNum/2;
#if _DEBUG
		m_isCheckAttackBox = true;
#endif
		//攻撃が当たったかどうか
		m_isHitAttack = CheckHit(enemy);
		if (m_isHitAttack)
		{
			//ヒットストップ
			gameManager.OnIsHitStop();
			//リセット
			m_attackType = AttackTypes::Null;
		}
	}
	//攻撃持続終わり
	if (m_animCountFrame > m_finishAttackFrame)
	{
		//当たり判定リセット
		m_attackType = AttackTypes::Null;
		ResetAttackBox();
	}


}

void Player::AttackAerialDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

//空中攻撃後
void Player::AttackedAerialUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
#if _DEBUG
	//コマンドの硬直まで数える
	debugCommandFrame++;
#endif
	m_chara->GetAnimJump(*this);
	m_chara->GetHitBoxAttackedAerial(*this);
	//ガードできない
	m_isGuard = false;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	m_animIndex = animMaxNum;
	if (m_isGround)
	{
		m_animCountFrame++;
		if (m_animCountFrame > 3)//着地硬直後
		{
			//着地後
			JumpedSE();
			m_attackType = AttackTypes::Null;
			m_update = &Player::IdleStandUpdate;
			m_draw = &Player::IdleStandDraw;
			m_animIndex = 0;
			m_animCountFrame = 0;
			(this->*m_update)(input, enemy, myBullet, gameManager);
			return;
		}
	}
	else
	{
		//着地硬直を数えるために空中にいる間は0
		m_animCountFrame = 0;
	}
}

void Player::AttackedAerialDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::CommandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//コマンド技
	m_isCommand = true;

	//ガードできない
	m_isGuard = false;
	
	//コマンド技は動きが特殊なものが多いのでCharaのほうで動きを作る
	m_chara->MovementCommand(*this, *myBullet, enemy);

	
	m_animCountFrame++;
#if _DEBUG
	//コマンド全体フレーム
	debugCommandFrame = m_animCountFrame;
#endif
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
	}
	//アニメーションの数が最大まで行ったとき
	if (m_animIndex > animMaxNum)
	{
		m_animIndex = animMaxNum;

		//アニメーションが攻撃の持続終了より先に終わってしまった場合は
		//アニメーションを最後のアニメーションで止める
		if (m_animCountFrame >= m_finishAttackFrame)
		{
			//移動量リセット
			m_velocity.x = 0;
			//リセット
			InitPushBox();//押し合い判定を戻す
			m_isHitGrasp = false;
			m_isThrown = false;
			m_isThrowSuccess = false;

			if (!m_isGround)
			{
				//リセット
				m_attackType = AttackTypes::Null;
				ResetAttackBox();
				m_animIndex = animMaxNum;
				m_update = &Player::AttackedAerialUpdate;
				m_draw = &Player::AttackedAerialDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy, myBullet, gameManager);
				return;
			}
			else
			{
				//スティックを下に倒してる
				if (input.IsPress("Down"))
				{
					//しゃがみ
					m_update = &Player::IdleSquatUpdate;
					m_draw = &Player::IdleSquatDraw;
					m_animIndex = 0;
					m_animCountFrame = 0;
					(this->*m_update)(input, enemy, myBullet, gameManager);
					return;
				}
				else
				{
					//立ち
					m_update = &Player::IdleStandUpdate;
					m_draw = &Player::IdleStandDraw;
					m_animIndex = 0;
					m_animCountFrame = 0;
					(this->*m_update)(input, enemy, myBullet, gameManager);
					return;
				}
			}

		}

	}

	//フレームが攻撃持続フレーム以内の時
	if (m_startAttackFrame <= m_animCountFrame && m_animCountFrame <= m_finishAttackFrame)
	{
#if _DEBUG
		m_isCheckAttackBox = true;
#endif
		//攻撃が当たったかどうか
		m_isHitAttack = CheckHit(enemy);
		if (m_isHitAttack)
		{
			//ヒットストップ
			gameManager.OnIsHitStop();
			//当たり判定リセット
			m_attackType = AttackTypes::Null;
		}

		//投げが当たったかどうか
		m_isHitGrasp = CheckHitGrasp(enemy);
		if (m_isHitGrasp)
		{
			//アニメーションをリセット
			m_animCountFrame = 0;
			m_animIndex = 0;

			//当たり判定リセット
			ResetGraspBox();
			//押し合い判定を消す
			ResetPushBox();
		}
	}
	
	//攻撃持続終わり
	if (m_animCountFrame > m_finishAttackFrame)
	{
		//当たり判定リセット
		m_attackType = AttackTypes::Null;
		ResetAttackBox();
	}
}



void Player::CommandDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}


void Player::GuardStandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//攻撃判定リセット
	ResetAttackBox();
	ResetGraspBox();
	//ガード硬直フレームがあるなら
	if (m_guardFrame <= 0)
	{
		m_guardFrame = 0;
		//立ち
		m_update = &Player::IdleStandUpdate;
		m_draw = &Player::IdleStandDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}
	else
	{
		m_guardFrame--;
		m_isGuard = true;
	}

	m_animCountFrame++;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;

	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
}

void Player::GuardStandDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::GuardSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//攻撃判定リセット
	ResetAttackBox();
	ResetGraspBox();
	//ガード硬直フレームがあるなら
	if (m_guardFrame <= 0)
	{
		m_guardFrame = 0;
		//立ち
		m_update = &Player::IdleStandUpdate;
		m_draw = &Player::IdleStandDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}
	else
	{
		m_guardFrame--;
		m_isGuard = true;
	}
	
	m_animCountFrame++;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
}

void Player::GuardSquatDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

//投げ
void Player::GraspUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//ガードできない
	m_isGuard = false;
	//移動量リセット
	m_velocity.x = 0;
	//投げ
	m_chara->GetAnimGrasp(*this);
	assert(m_handle != -1);
	//ヒットボックス
	m_chara->GetHitBoxGrasp(*this);

	m_animCountFrame++;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if (m_animIndex > animMaxNum)
		{
			//当たり判定リセット
			ResetGraspBox();
			//スティックを下に倒してる
			if (input.GetStickInfo().leftStickY > 300)
			{
				//しゃがみ
				m_update = &Player::IdleSquatUpdate;
				m_draw = &Player::IdleSquatDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy, myBullet, gameManager);
				return;
			}
			else
			{
				//立ち
				m_update = &Player::IdleStandUpdate;
				m_draw = &Player::IdleStandDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy, myBullet, gameManager);
				return;
			}
		}
	}
	


#if _DEBUG
	m_isCheckAttackBox = false;
#endif
	//フレームが攻撃持続フレーム以内の時
	if (m_startAttackFrame <= m_animCountFrame && m_animCountFrame <= m_finishAttackFrame)
	{
#if _DEBUG
		m_isCheckAttackBox = true;
#endif
		//攻撃が当たったかどうか
		m_isHitGrasp = CheckHitGrasp(enemy);
		//ゲームマネージャーの中で投げが通ったかをチェックして
		//成功している場合　m_isThrowSuccess　がtrueになる
	}
	//攻撃持続終わり
	if (m_animCountFrame > m_finishAttackFrame)
	{
		//当たり判定リセット
		ResetGraspBox();
	}

	//投げが通ったら
	if (m_isThrowSuccess)
	{
		//SE再生
		GraspSE();

		m_chara->GetGiveEffectGrasp(*this);
		//相手をふっとばす方向
		Vector3 throwVelo;
		throwVelo.x = kThrowVeloX;

		if (m_isLeft)//自分が左向きなら左に飛ばす
		{
			throwVelo.x *= -1;
		}
		throwVelo.y = kThrowVeloY;
		//向いてる向きと反対方向を押してるとき（後ろ投げ）
		if (input.IsPress("Left") && !m_isLeft || input.IsPress("Right") && m_isLeft)
		{
			//投げ
			m_chara->GetAnimThrowBack(*this);
			assert(m_handle != -1);
			//ヒットボックス
			m_chara->GetHitBoxThrowBack(*this);
			//後ろに投げるので反転
			throwVelo.x *= -kThrowBackMargin;

			//自分も若干動かす
			m_velocity.x = kBackThrowShiftVelo;
			if (m_isLeft)
			{
				m_velocity.x *= -1.0f;
			}
		}
		else
		{
			//移動量リセット
			m_velocity.x = 0;
			//投げ
			m_chara->GetAnimThrowFront(*this);
			assert(m_handle != -1);
			//ヒットボックス
			m_chara->GetHitBoxThrowFront(*this);
		}
		//セット
		enemy.SetThrowVelo(throwVelo);

		DrawString(600, 200, "投げが当たった\n", 0xff0000);
		//当たり判定リセット
		ResetGraspBox();

		//投げる相手の初期値
		Vector3 posOffset(10, 0, 0);
		if (m_isLeft)
		{
			posOffset.x *= -1;
		}
		enemy.SetPos(m_pos + posOffset);
		//投げに移行
		ResetPushBox();//押し合い判定を消す
		m_update = &Player::ThrowUpdate;
		m_draw = &Player::ThrowDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}
}

void Player::GraspDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

//投げる
void Player::ThrowUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//ガードできない
	m_isGuard = false;

	m_animCountFrame++;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;

		if (m_animIndex == animMaxNum / 2)
		{
			//SE再生
			ThrowSE();
			// 投げられ状態にする
			enemy.OnIsThrown();
		}

		//アニメーションの数が最大まで行ったとき
		if (m_animIndex > animMaxNum)
		{
			m_se->Stop();
			//移動量リセット
			m_velocity.x = 0;
			//リセット
			InitPushBox();//押し合い判定を戻す
			m_isHitGrasp = false;
			m_isThrown = false;
			m_isThrowSuccess = false;
			//スティックを下に倒してる
			if (input.IsPress("Down"))
			{
				//しゃがみ
				m_update = &Player::IdleSquatUpdate;
				m_draw = &Player::IdleSquatDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy, myBullet, gameManager);
				return;
			}
			else
			{
				//立ち
				m_update = &Player::IdleStandUpdate;
				m_draw = &Player::IdleStandDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy, myBullet, gameManager);
				return;
			}
		}
	}
	
}


void Player::ThrowDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}
//投げられる
void Player::BeThrownUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//移動量リセット
	m_velocity.x = 0;
	//攻撃判定リセット
	ResetAttackBox();
	ResetGraspBox();
	//ガードできない
	m_isGuard = false;

	//コマ投げは投げ抜けできない
	if (!enemy.IsCommand())
	{
		if (m_animCountFrame <= kCanThrowEscapeFrame)
		{
			//両方trueなら投げ
			bool isLightPunch = false;
			bool isLightKick = false;
			//弱パンチ
			if (m_lightPunchBottun.isPress)
			{
				isLightPunch = true;
			}
			//弱キック
			if (m_lightKickBottun.isPress)
			{
				isLightKick = true;
			}
			//ワンボタン投げ
			if (m_graspBottun.isPress)
			{
				isLightPunch = true;
				isLightKick = true;
			}
			//弱パンチと弱キック同時押しで投げ
			if (isLightPunch && isLightKick)
			{
				m_isThrowSuccess = true;
				//投げぬけ
				enemy.LoadStateThrowEscape();
				//立ち
				m_update = &Player::ThrowEscapeUpdate;
				m_draw = &Player::ThrowEscapeDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy, myBullet, gameManager);
				return;
			}
		}
	}


	if (m_isThrown)
	{
		//ヒットストップ
		gameManager.OnIsHitStop();
		//カメラを揺らす
		gameManager.OnIsCameraShake();
		//ガードできない
		m_isGround = false;
		//ふっとばされる
		m_velocity = m_giveThrowVelo;
		//衝突判定復活
		InitPushBox();
		//投げのダメージ
		m_hp -= enemy.GetGiveDamage();

		m_update = &Player::DownAerialUpdate;
		m_draw = &Player::DownAerialDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		//リセット
		m_isHitGrasp = false;
		m_isThrown = false;
		m_isThrowSuccess = false;
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}

	m_animCountFrame++;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
}

void Player::BeThrownDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, m_angle, m_handle, true, m_isLeft);
}

//投げ抜け
void Player::ThrowEscapeUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//攻撃判定リセット
	ResetAttackBox();
	ResetGraspBox();
	//衝突判定復活
	InitPushBox();
	//ガードできない
	m_isGuard = false;

	//硬直終了時
	if (m_animCountFrame > kNoActThrowEscapeFrame)
	{
		//リセット
		m_isHitGrasp = false;
		m_isThrowSuccess = false;
		//衝突判定復活
		InitPushBox();
		//立ち
		m_update = &Player::IdleStandUpdate;
		m_draw = &Player::IdleStandDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}
	if (m_isLeft)
	{
		m_velocity.x = 10.0f;
	}
	else
	{
		m_velocity.x = -10.0f;
	}

	m_animCountFrame++;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
}

void Player::ThrowEscapeDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::DamageUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//やられ判定
	m_chara->GetHitBoxHitStand(*this);
	//攻撃判定リセット
	ResetAttackBox();
	ResetGraspBox();
	//投げリセット
	m_isHitGrasp = false;
	m_isThrown = false;
	m_isThrowSuccess = false;
	//ガードできない
	m_isGuard = false;
	//硬直フレーム
	if (m_noActFrame <= 0)
	{
		m_noActFrame = 0;
		
		if (m_isGround)
		{
			//立ち
			m_update = &Player::IdleStandUpdate;
			m_draw = &Player::IdleStandDraw;
			m_animIndex = 0;
			m_animCountFrame = 0;
			(this->*m_update)(input, enemy, myBullet, gameManager);
			return;
		}
	}
	else
	{
		if (!m_isGround)
		{
			//空中
			m_chara->GetAnimHitAerial(*this);
		}
		m_noActFrame--;
		m_velocity.x = m_knockback.x * static_cast<float>(m_noActFrame) / m_allNoActFrame;
	}

	m_animCountFrame++;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
}

void Player::DamageDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, m_angle, m_handle, true, m_isLeft);
}

void Player::DamageSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{

}

void Player::DamageSquatDraw(const Camera& camera)
{
}


//ダウン
void Player::DownUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//攻撃判定リセット
	ResetAttackBox();
	ResetGraspBox();
	//投げリセット
	m_isHitGrasp = false;
	m_isThrown = false;
	m_isThrowSuccess = false;
	//ガードできない
	m_isGuard = false;
	//SE
	DownSE();

	//地面について数秒立ったら起き上がる
	//硬直フレーム
	if ((m_noActFrame <= 0) && m_hp > 0)
	{
		m_noActFrame = 0;
		m_se->Stop();
		//起き上がり
		m_chara->GetAnimStandUp(*this);
		m_update = &Player::StandUpUpdate;
		m_draw = &Player::StandUpDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}
	else
	{
		//地上にいるときはカウントする
		if (m_isGround)
		{
			m_noActFrame--;
			m_velocity.x = 0;
		}
	}

	m_animCountFrame++;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
}

void Player::DownDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y) + kDownDrawOffset,
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, m_angle, m_handle, true, m_isLeft);
}

void Player::DownAerialUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//攻撃判定リセット
	ResetAttackBox();
	ResetGraspBox();
	//投げリセット
	m_isHitGrasp = false;
	m_isThrown = false;
	m_isThrowSuccess = false;
	//ガードできない
	m_isGuard = false;
	//地面についたらダウン
	if (m_isGround)
	{
		//ダウン
		LoadStateDown();
		m_animIndex = 4;//途中から始めたい
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}

	m_animCountFrame++;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
}

void Player::DownAerialDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, m_angle, m_handle, true, m_isLeft);
}

//起き上がり
void Player::StandUpUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	m_animCountFrame++;
	//ガードできない
	m_isGuard = false;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//アニメーションの数が最大まで行ったとき
		if ((m_animIndex > animMaxNum))
		{
			//スティックを下に倒してる
			if (input.IsPress("Down"))
			{
				//しゃがみ
				m_update = &Player::IdleSquatUpdate;
				m_draw = &Player::IdleSquatDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy, myBullet, gameManager);
				return;
			}
			else
			{
				//立ち
				m_update = &Player::IdleStandUpdate;
				m_draw = &Player::IdleStandDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy, myBullet, gameManager);
				return;
			}
		}
	}
}

void Player::StandUpDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::ResultUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//判定を消す
	ResetAttackBox();
	OffIsHitGrasp();
	m_chara->GetHitBoxDown(*this);

	m_velocity.x = 0;
	//アニメーションの1枚目を0番として数えるので
	//アニメーションの最大数から-1した値が最後のアニメーション
	int animMaxNum = m_animNum - 1;

	m_animCountFrame++;

	
	//勝ってたら勝利ポーズ
	if (m_animCountFrame >= kChangeWinPoseFrame && m_hp > enemy.GetHp())
	{
		//リセットする
		if (m_animCountFrame == kChangeWinPoseFrame && m_hp > enemy.GetHp())
		{
			m_animIndex = 0;
		}
		m_chara->GetAnimWinPose(*this);

		//アニメーションのフレームを数える
		if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//アニメーションの数が最大まで行ったとき
			if ((m_animIndex > animMaxNum))
			{
				WinSE();
				m_animIndex = animMaxNum;
			}
		}
	}
	else
	{
		m_chara->GetAnimIdleStand(*this);

		//アニメーションのフレームを数える
		if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//アニメーションの数が最大まで行ったとき
			if ((m_animIndex > animMaxNum))
			{
				m_animIndex = 0;
			}
		}
	}
}

void Player::ResultDraw(const Camera& camera)
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndex % (sizeX / kCharaWidth);//横
	int cutY = m_animIndex / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}
