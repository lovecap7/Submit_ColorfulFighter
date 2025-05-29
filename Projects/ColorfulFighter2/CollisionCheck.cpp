#include "CollisionCheck.h"
#include "Player.h"
#include "Bullet.h"
#include "Camera.h"
#include "GameManager.h"
#include "game.h"
#include "SE.h"
#include <DxLib.h>

namespace
{
	//攻撃したときに与える力
	constexpr float kGiveVeloPowerX = 20.0f;
	constexpr float kGiveVeloPowerY = -30.0f;

	//壁に当たってる敵を殴ったときのノックバック
	constexpr float kReflectionMagin = 10.0f;

	//削り軽減率
	constexpr float kScrapMagin = 0.25f;

	//seボリューム
	constexpr int kSeVolume = 150;

	//ガードしたときのけぞりが減るようにする
	constexpr float kKnockbackMagin = 0.25f;
}


Vector3 CollisionCheck::CreateHitEffectPosPtoP(Player& attacker, Player& defender)
{
	//ヒットエフェクトのポジション
	Vector3 hitEffectPos;
	
	//攻撃判定の中心を求める
	Vector3 attackPos;
	//向きによって位置が変わるので
	if (!attacker.GetDirState())
	{
		attackPos.x = (attacker.GetPos().x + attacker.GetHitBoxAttack().x2 + attacker.GetPos().x + attacker.GetHitBoxAttack().x1) / 2;
		attackPos.y = (attacker.GetPos().y + attacker.GetHitBoxAttack().y2 + attacker.GetPos().y + attacker.GetHitBoxAttack().y1) / 2;
	}
	else
	{
		//攻撃判定の中心を求める
		attackPos.x = (attacker.GetPos().x - attacker.GetHitBoxAttack().x2 + attacker.GetPos().x - attacker.GetHitBoxAttack().x1) / 2;
		attackPos.y = (attacker.GetPos().y + attacker.GetHitBoxAttack().y2 + attacker.GetPos().y + attacker.GetHitBoxAttack().y1) / 2;
	}
	
	//攻撃を食らう側のポジション
	Vector3 defenderPos = defender.GetPos();
	//攻撃と喰らう側の間の位置
	hitEffectPos.x = (attackPos.x + defenderPos.x) / 2;
	//高さはお互いが地上の場合は攻撃のY座標に合わせたほうが
	//違和感がなかったので地上か空中かで条件分岐
	if (attacker.IsGround())
	{
		//地上
		hitEffectPos.y = attackPos.y;
	}
	else
	{
		//空中
		hitEffectPos.y = (attackPos.y + defenderPos.y) / 2;
	}


	return hitEffectPos;
}

Vector3 CollisionCheck::CreateHitEffectPosBtoP(Bullet& bullet, Player& defender)
{
	//ヒットエフェクトのポジション
	Vector3 hitEffectPos;

	//弾の座標
	Vector3 attackPos = bullet.GetPos();
	//敵の座標
	Vector3 defenderPos = defender.GetPos();
	//攻撃と喰らう側の間の位置
	hitEffectPos.x = (attackPos.x + defenderPos.x) / 2;
	hitEffectPos.y = (attackPos.y + defenderPos.y) / 2;

	return hitEffectPos;
}

CollisionCheck::CollisionCheck():
	m_lastVeloP1(),
	m_lastVeloP2(),
	m_hitSeHandle(LoadSoundMem("./SE/Hit/Hit1.mp3")),
	m_guardSeHandle(LoadSoundMem("./SE/Hit/Guard.mp3"))
{
	//SE
	m_seP1 = std::make_shared<SE>();
	m_seP2 = std::make_shared<SE>();
}

CollisionCheck::~CollisionCheck()
{
	DeleteGraph(m_hitSeHandle);
	DeleteGraph(m_guardSeHandle);
}

void CollisionCheck::Update(Player& p1, Player& p2, Bullet& bulletP1, Bullet& bulletP2, 
	Camera& camera, GameManager& gameManager)
{
	if (gameManager.GetTimer() > 0)
	{
		AttackProcess(p1, p2, bulletP1, bulletP2, camera, gameManager);
	}
	MoveProcess(p1, p2, camera);
}

//押し合い判定
bool CollisionCheck::ColCheckPlayer(Player& p1, Player& p2)
{
	//次の移動先を取得
	Vector3 nextP1 = p1.GetPos() + p1.GetVelo();
	Vector3 nextP2 = p2.GetPos() + p2.GetVelo();

	//X、Y方向の離れている距離を絶対値化して取得
	float width = std::abs(nextP1.x - nextP2.x);
	float height = std::abs(nextP1.y - nextP2.y);

	//X方向の距離がキャラの幅より大きければ当たっていない
	if (width > p1.GetPushBox().x1 + p2.GetPushBox().x2)	return false;
	//Y方向の距離がキャラの高さより大きければ当たっていない
	if (height > p1.GetPushBox().y1 + p2.GetPushBox().y2)	return false;

	//ここまでくれば当たってる
	return true;
}

bool CollisionCheck::ColCheckWall(Player& player, Camera& camera)
{
	//次の移動先を取得
	Vector3 nextPos = player.GetPos() + player.GetVelo();
	float playerWidth = std::abs(player.GetPushBox().x1) + std::abs(player.GetPushBox().x2);

	//カメラの範囲内なら当たってない
	if ((camera.m_pos.x - Game::kScreenWidth / 2) <= nextPos.x - playerWidth / 2 &&
		(camera.m_pos.x + Game::kScreenWidth / 2) >= nextPos.x + playerWidth / 2)	return false;


	//ここまでくれば当たってる
	return true;
}

//右の壁
bool CollisionCheck::ColCheckRightWall(Player& player, Camera& camera)
{
	//次の移動先を取得
	Vector3 nextPos = player.GetPos() + player.GetVelo();
	float playerWidth = std::abs(player.GetPushBox().x1) + std::abs(player.GetPushBox().x2);

	
	if ((camera.m_pos.x + Game::kScreenWidth / 2) < nextPos.x + playerWidth / 2)	return true;


	//ここまでくれば当たってない
	return false;
}

bool CollisionCheck::ColCheckLeftWall(Player& player, Camera& camera)
{
	//次の移動先を取得
	Vector3 nextPos = player.GetPos() + player.GetVelo();
	float playerWidth = std::abs(player.GetPushBox().x1) + std::abs(player.GetPushBox().x2);


	if ((camera.m_pos.x - Game::kScreenWidth / 2) > nextPos.x - playerWidth / 2)	return true;


	//ここまでくれば当たってない
	return false;
}

//壁に当たった時の処理
Vector3 CollisionCheck::WallProcess(Player& player, Camera& camera)
{
	Vector3 nextPos = player.GetPos() + player.GetVelo();;
	DrawString(300, 500, "カベ", 0xffffff, true);
	float playerWidth = std::abs(player.GetPushBox().x1) + std::abs(player.GetPushBox().x2);
	//左の壁なら
	if (ColCheckLeftWall(player, camera))
	{
		nextPos.x = camera.m_pos.x - Game::kScreenWidth / 2 + playerWidth / 2;
	}
	//右の壁なら
	if (ColCheckRightWall(player, camera))
	{
		nextPos.x = camera.m_pos.x + Game::kScreenWidth / 2 - playerWidth / 2;
	}
	return nextPos;
}


void CollisionCheck::MoveProcess(Player& p1, Player& p2, Camera& camera)
{
	Player nextP1 = p1;
	Player nextP2 = p2;

	//つぎに移動する座標を出す
	Vector3 p1NextPos = nextP1.GetPos() + nextP1.GetVelo();
	Vector3 p2NextPos = nextP2.GetPos() + nextP2.GetVelo();
	//重なってる部分を出す
	float overlap = 0;

	m_lastVeloP1 = nextP1.GetVelo();
	m_lastVeloP2 = nextP2.GetVelo();

	//相手と当たっているかをチェック
	if (ColCheckPlayer(nextP1, nextP2))
	{
		//壁に当たってたら速度を0にする
		if (ColCheckWall(nextP1, camera))
		{
			nextP1.SetVeloX(0);
		}
		else if (ColCheckWall(nextP2, camera))
		{
			nextP2.SetVeloX(0);
		}


		//P1だけ動いているなら
		if (nextP1.GetVelo().Length() != 0 && nextP2.GetVelo().Length() == 0)
		{
			//左にずらす
			if (p1NextPos.x < p2NextPos.x)
			{
				if (!ColCheckWall(nextP1, camera))
				{
					nextP2.SetVeloX(nextP1.GetVelo().x / 2);//プレイヤーの速度の半分の力で押す
					p2NextPos = nextP2.GetPos() + nextP2.GetVelo();
				}


				overlap = (nextP1.GetPushBox().x1 + nextP2.GetPushBox().x2) - std::abs(p1NextPos.x - p2NextPos.x);
				nextP1.SetVeloX(nextP1.GetVelo().x - overlap);
				p1NextPos.x = nextP1.GetPos().x + nextP1.GetVelo().x - 0.000001f;
			}
			//右にずらす
			else if (p1NextPos.x > p2NextPos.x)
			{
				if (!ColCheckWall(nextP1, camera))
				{
					nextP2.SetVeloX(nextP1.GetVelo().x / 2);//プレイヤーの速度の半分の力で押す
					p2NextPos = nextP2.GetPos() + nextP2.GetVelo();
				}

				overlap = (nextP2.GetPushBox().x1 + nextP1.GetPushBox().x2) - std::abs(p1NextPos.x - p2NextPos.x);
				nextP1.SetVeloX(nextP1.GetVelo().x + overlap);
				p1NextPos.x = nextP1.GetPos().x + nextP1.GetVelo().x + 0.000001f;
			}

			//同じ位置の時
			if (p1NextPos.x == p2NextPos.x)
			{
				//左を向いているなら左にずれる
				if (nextP1.GetDirState())
				{
					p1NextPos.x = nextP1.GetPos().x - (nextP2.GetPushBox().x1 + nextP2.GetPushBox().x2);
				}
				//右なら右
				else
				{
					p1NextPos.x = nextP1.GetPos().x + (nextP2.GetPushBox().x1 + nextP2.GetPushBox().x2);
				}
			}
		}
		//P2だけ動いているなら
		else if (nextP1.GetVelo().Length() == 0 && nextP2.GetVelo().Length() != 0)
		{
			//左にずらす
			if (p2NextPos.x < p1NextPos.x)
			{
				if (!ColCheckWall(nextP2, camera))
				{
					nextP1.SetVeloX(nextP2.GetVelo().x / 2);//プレイヤーの速度の半分の力で押す
					p1NextPos = nextP1.GetPos() + nextP1.GetVelo();
				}
				overlap = (nextP1.GetPushBox().x1 + nextP2.GetPushBox().x2) - std::abs(p1NextPos.x - p2NextPos.x);
				nextP2.SetVeloX(nextP2.GetVelo().x - overlap);
				p2NextPos.x = nextP2.GetPos().x + nextP2.GetVelo().x - 0.000001f;

			}
			//右にずらす
			else if (p2NextPos.x > p1NextPos.x)
			{
				if (!ColCheckWall(nextP2, camera))
				{
					nextP1.SetVeloX(nextP2.GetVelo().x / 2);//プレイヤーの速度の半分の力で押す
					p1NextPos = nextP1.GetPos() + nextP1.GetVelo();
				}

				overlap = (nextP2.GetPushBox().x1 + nextP1.GetPushBox().x2) - std::abs(p1NextPos.x - p2NextPos.x);
				nextP2.SetVeloX(nextP2.GetVelo().x + overlap);
				p2NextPos.x = nextP2.GetPos().x + nextP2.GetVelo().x + 0.000001f;
			}

			//同じ位置の時
			if (p1NextPos.x == p2NextPos.x)
			{
				//左を向いているなら左にずれる
				if (nextP2.GetDirState())
				{
					p2NextPos.x = nextP2.GetPos().x - (nextP1.GetPushBox().x1 + nextP1.GetPushBox().x2);
				}
				//右なら右
				else
				{
					p2NextPos.x = nextP2.GetPos().x + (nextP1.GetPushBox().x1 + nextP1.GetPushBox().x2);
				}
			}
		}
		//両方が動いているときまたは動いていないとき
		else
		{
			//左
			if (p1NextPos.x < p2NextPos.x)
			{
				overlap = ((nextP1.GetPushBox().x1 + nextP2.GetPushBox().x2) - std::abs(p1NextPos.x - p2NextPos.x)) / 2;
				nextP1.SetVeloX(nextP1.GetVelo().x - overlap);
				p1NextPos.x = nextP1.GetPos().x + nextP1.GetVelo().x - 0.0000001f;
				nextP2.SetVeloX(nextP2.GetVelo().x + overlap);
				p2NextPos.x = nextP2.GetPos().x + nextP2.GetVelo().x + 0.0000001f;
			}
			//右
			else if (p1NextPos.x > p2NextPos.x)
			{
				overlap = ((nextP2.GetPushBox().x1 + nextP1.GetPushBox().x2) - std::abs(p1NextPos.x - p2NextPos.x)) / 2;
				nextP1.SetVeloX(nextP1.GetVelo().x + overlap);
				p1NextPos.x = nextP1.GetPos().x + nextP1.GetVelo().x + 0.0000001f;
				nextP2.SetVeloX(nextP2.GetVelo().x - overlap);
				p2NextPos.x = nextP2.GetPos().x + nextP2.GetVelo().x - 0.0000001f;
			}
		}
	}

	//どちらか一方でも壁に当たったかをチェック
	bool isHitWall = false;
	//プレイヤーが生きていたら壁との当たり判定をする
	if (p1.GetHp() > 0)
	{
		//壁との当たり判定
		if (ColCheckWall(nextP1, camera))
		{
			p1NextPos = WallProcess(nextP1, camera);
			isHitWall = true;

		}
	}
	if(p2.GetHp() > 0)
	{
		if (ColCheckWall(nextP2, camera))
		{
			p2NextPos = WallProcess(nextP2, camera);
			isHitWall = true;
		}
	}
	nextP1.SetPos(p1NextPos);
	nextP2.SetPos(p2NextPos);
	//もしも二人が壁に当たってぶつかったときにおなじX座標になった際の処理
	if (nextP1.GetPos().x == nextP2.GetPos().x && isHitWall && ColCheckPlayer(nextP1, nextP2))
	{
		//P1がP2の上にいる場合
		if (nextP1.GetPos().y < nextP2.GetPos().y)
		{
			//右の壁なら左にずらす
			if (ColCheckRightWall(nextP1, camera))
			{
				p1NextPos.x = nextP1.GetPos().x - (nextP2.GetPushBox().x1 + nextP2.GetPushBox().x2);
			}
			//左なら左に
			if (ColCheckLeftWall(nextP1, camera))
			{
				p1NextPos.x = nextP1.GetPos().x + (nextP2.GetPushBox().x1 + nextP2.GetPushBox().x2);
			}
		}
		//P2がP1の上にいるとき
		if (nextP1.GetPos().y > nextP2.GetPos().y)
		{
			//右の壁なら左にずらす
			if (ColCheckRightWall(nextP2, camera))
			{
				p2NextPos.x = nextP2.GetPos().x - (nextP1.GetPushBox().x1 + nextP1.GetPushBox().x2);
			}
			//左なら左に
			if (ColCheckLeftWall(nextP2, camera))
			{
				p2NextPos.x = nextP2.GetPos().x + (nextP1.GetPushBox().x1 + nextP1.GetPushBox().x2);
			}
		}
	}
	//移動
	p1.SetPos(p1NextPos);
	p2.SetPos(p2NextPos);
}

void CollisionCheck::AttackProcess(Player& p1, Player& p2, Bullet& bulletP1, Bullet& bulletP2 , 
	Camera& camera,GameManager& gameManager)
{
	if (gameManager.GetTimer() <= 0)
	{
		return;
	}
	
	//攻撃が当たっていればプレイヤーの移動量を初期化
	if (p1.IsHitAttack() || bulletP1.GetIsHitPlayer())
	{
		p2.SetVeloX(0);
		p2.SetVeloY(0);
		//描画を前に
		gameManager.SetIsDrawFrontP1(true);
	}
	if (p2.IsHitAttack() || bulletP2.GetIsHitPlayer())
	{
		p1.SetVeloX(0);
		p1.SetVeloY(0);
		//描画を後ろに
		gameManager.SetIsDrawFrontP1(false);
	}
	//攻撃による移動ベク
	Vector3 giveVeloP1(p2.GetGiveAttackVelo());
	Vector3 giveVeloP2(p1.GetGiveAttackVelo());

	//投げと攻撃が重なっていない場合投げは成立する
	if (!p1.IsHitAttack() && !bulletP1.GetIsHitPlayer() && !p2.IsHitAttack() && !bulletP2.GetIsHitPlayer())
	{
		//投げぬけ
		if (p1.IsHitGrasp() && p2.IsHitGrasp() || p1.IsThrowSuccess() && p2.IsThrowSuccess())
		{
			p1.LoadStateThrowEscape();
			p2.LoadStateThrowEscape();
			p1.OffIsHitGrasp();
			p2.OffIsHitGrasp();
		}
		//P1の投げが当たったとき
		else if (p1.IsHitGrasp() && !p2.IsHitGrasp())
		{
			//描画を後ろに
			//投げるときは後ろにしたほうが見栄えがいい
			gameManager.SetIsDrawFrontP1(false);
			//投げられ状態にする
			p2.LoadStateBeThrown();
			//投げる状態にする
			p1.OffIsHitGrasp();
			p1.OnIsThrownSuccess();;
		}
		//P2の投げが当たったとき
		else if (p2.IsHitGrasp() && !p1.IsHitGrasp())
		{
			//描画を前に
			gameManager.SetIsDrawFrontP1(true);
			p1.LoadStateBeThrown();
			p2.OffIsHitGrasp();
			p2.OnIsThrownSuccess();
		}
	}

	//弾による攻撃の処理
	//P1の弾がP2に当たった時
	if (bulletP1.GetIsHitPlayer())
	{
		//ヒットストップ
		gameManager.OnIsHitStop();
		giveVeloP2.x = bulletP1.GetGiveAttackVelo().x;
		giveVeloP2.y = 0;
		//ガードしてたら
		if (p2.IsGuard())
		{
			//相手が左側なら反転
			//プレイヤーの攻撃と同時に当たっている場合
			//最後の攻撃によってベクトルに向きを確定させてほしい
			if ((p2.GetPos().x < p1.GetPos().x) && !p1.IsHitAttack())
			{
				giveVeloP2.x *= -1;
			}
			p2.SetKnockback(giveVeloP2 * kKnockbackMagin);
			p2.SetVelo(giveVeloP2 * kKnockbackMagin);//相手をずらす
			p2.SetHp(p2.GetHp() - (bulletP1.GetGiveDamage() * kScrapMagin));//ダメージ
			p2.SetGuardFrame(bulletP1.GetGiveGuardFrame());//ガード硬直

			//立ちガード
			if (!p2.IsSquat())
			{
				p2.LoadStateGuardStand();//ガードモーション
			}
			//しゃがみガード
			else
			{
				p2.LoadStateGuardSquat();//ガードモーション
			}

			//ガードエフェクト
			gameManager.LoadScrapeGuardEffect(p1);

			//ガードの音
			m_seP1->Stop();
			m_seP1->SetSE(m_guardSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();
		}
		else
		{
			//カメラ揺らす
			gameManager.OnIsCameraShake();
			//空中の敵に攻撃を当てたら
			if (!p2.IsGround())
			{
				if (giveVeloP2.y == 0)
				{
					giveVeloP2.y = kGiveVeloPowerY;
				}
				p2.LoadStateDownAerial();//ダウンモーション
			}
			else
			{
				p2.LoadStateHit();//ヒットモーション
			}
			//相手が左側なら反転
			//プレイヤーの攻撃と同時に当たっている場合
			//最後の攻撃によってベクトルに向きを確定させてほしい
			if ((p2.GetPos().x < p1.GetPos().x) && !p1.IsHitAttack())
			{
				giveVeloP2.x *= -1;
			}
			p2.SetKnockback(giveVeloP2);
			p2.SetVelo(giveVeloP2);//相手をずらす
			p2.SetHp(p2.GetHp() - bulletP1.GetGiveDamage());//ダメージ
			p2.SetNoActFrame(bulletP1.GetGiveNoActFrame());//硬直差

			//ヒットエフェクト
			gameManager.LoadSpecialHitEffect(p1);

			//ヒットの音
			m_seP1->Stop();
			m_seP1->SetSE(m_hitSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();
			
		}
		//エフェクトの位置
		gameManager.SetHitEffectPosP1(CreateHitEffectPosBtoP(bulletP1, p2));

		//弾を削除}
		bulletP1.Disappear();
	}
	//P2の弾がP1に当たった時
	if (bulletP2.GetIsHitPlayer())
	{
		//ヒットストップ
		gameManager.OnIsHitStop();
		giveVeloP1.x = bulletP2.GetGiveAttackVelo().x;;
		giveVeloP1.y = 0;
		//ガードしてたら
		if (p1.IsGuard())
		{
			//相手が左側なら反転
			//プレイヤーの攻撃と同時に当たっている場合
			//最後の攻撃によってベクトルに向きを確定させてほしい GetIsHitAttack()
			if ((p1.GetPos().x < p2.GetPos().x) && !p2.IsHitAttack())
			{
				giveVeloP1.x *= -1;
			}
			p1.SetKnockback(giveVeloP1* kKnockbackMagin);
			p1.SetVelo(giveVeloP1* kKnockbackMagin);//相手をずらす
			p1.SetHp(p1.GetHp() - (bulletP2.GetGiveDamage() * kScrapMagin));//ダメージ
			p1.SetGuardFrame(bulletP2.GetGiveGuardFrame());//ガード硬直
			//立ちガード
			if (!p1.IsSquat())
			{
				p1.LoadStateGuardStand();//ガードモーション
			}
			//しゃがみガード
			else
			{
				p1.LoadStateGuardSquat();//ガードモーション
			}

			//ガードエフェクト
			gameManager.LoadScrapeGuardEffect(p2);

			//ガードの音
			m_seP2->Stop();
			m_seP2->SetSE(m_guardSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();
		}
		else
		{
			//カメラ揺らす
			gameManager.OnIsCameraShake();
			//空中の敵に攻撃を当てたら
			if (!p1.IsGround())
			{
				if (giveVeloP1.y == 0)
				{
					giveVeloP1.y = kGiveVeloPowerY;
				}
				p1.LoadStateDownAerial();//ダウンモーション
			}
			else
			{
				p1.LoadStateHit();//ヒットモーション
			}
			//相手が左側なら反転
			//プレイヤーの攻撃と同時に当たっている場合
			//最後の攻撃によってベクトルに向きを確定させてほしい GetIsHitAttack()
			if ((p1.GetPos().x < p2.GetPos().x) && !p2.IsHitAttack())
			{
				giveVeloP1.x *= -1;
			}
			p1.SetKnockback(giveVeloP1);
			p1.SetVelo(giveVeloP1);//相手をずらす
			p1.SetHp(p1.GetHp() - bulletP2.GetGiveDamage());//ダメージ
			p1.SetNoActFrame(bulletP2.GetGiveNoActFrame());//硬直差

			//ヒットエフェクト
			gameManager.LoadSpecialHitEffect(p2);

			//ヒットの音
			m_seP2->Stop();
			m_seP2->SetSE(m_hitSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();
		}

		//エフェクトの位置
		gameManager.SetHitEffectPosP2(CreateHitEffectPosBtoP(bulletP2, p1));
		
		//弾を削除}
		bulletP2.Disappear();
	}

	//同時に殴った時
	if (p1.IsHitAttack() && p2.IsHitAttack())
	{
		//カメラ揺らす
		gameManager.OnIsCameraShake();
		//空中の敵に攻撃を当てたら
		if (!p2.IsGround())
		{
			if (giveVeloP2.y == 0)
			{
				giveVeloP2.y = kGiveVeloPowerY;
			}
		}
		//相手が左側なら反転
		if ((p2.GetPos().x < p1.GetPos().x))
		{
			giveVeloP2.x *= -1;
		}
		//0の場合はダウン属性攻撃
		if (p1.GetGiveNoActFrame() == 0)
		{
			//とりま空中ダウンにしとけば地上なら地上ダウンになるようにしてるからOK
			p2.LoadStateDownAerial();
			
			if (giveVeloP2.y == 0)
			{
				giveVeloP2.y = kGiveVeloPowerY * 0.5f;
			}
		}
		else
		{
			p2.SetNoActFrame(p1.GetGiveNoActFrame());//硬直差
			p2.LoadStateHit();//ヒットモーション
		}

		p2.SetKnockback(giveVeloP2);
		p2.SetVelo(giveVeloP2);//相手をずらす


		//空中の敵に攻撃を当てたら
		if (!p1.IsGround())
		{
			if (giveVeloP1.y == 0)
			{
				giveVeloP1.y = kGiveVeloPowerY;
			}
			
		}
		//相手が左側なら反転
		if ((p1.GetPos().x < p2.GetPos().x))
		{
			giveVeloP1.x *= -1;
		}
		//0の場合はダウン属性攻撃
		if (p2.GetGiveNoActFrame() == 0)
		{
			p1.LoadStateDownAerial();
			if (giveVeloP1.y == 0)
			{
				giveVeloP1.y = kGiveVeloPowerY * 0.5f;
			}
			
		}
		else
		{
			p1.SetNoActFrame(p2.GetGiveNoActFrame());//硬直差
			p1.LoadStateHit();//ヒットモーション
		}
		p1.SetKnockback(giveVeloP1);
		p1.SetVelo(giveVeloP1);//相手をずらす


		//ダメージ
		p1.SetHp(p1.GetHp() - p2.GetGiveDamage());
		p2.SetHp(p2.GetHp() - p1.GetGiveDamage());

		//ヒットエフェクト
		gameManager.LoadNormalHitEffect(p1);
		gameManager.LoadNormalHitEffect(p2);
		//エフェクトの位置
		gameManager.SetHitEffectPosP1(CreateHitEffectPosPtoP(p1, p2));
		gameManager.SetHitEffectPosP2(CreateHitEffectPosPtoP(p2, p1));
		//攻撃判定を消す
		p1.ResetAttackBox();
		p2.ResetAttackBox();


		//ヒットの音
		m_seP1->Stop();
		m_seP1->SetSE(m_hitSeHandle);
		m_seP1->Volume(kSeVolume);
		m_seP1->PlayOnce();
		//ヒットの音
		m_seP2->Stop();
		m_seP2->SetSE(m_hitSeHandle);
		m_seP2->Volume(kSeVolume);
		m_seP2->PlayOnce();
	}
	//P1が攻撃を当てた時
	else if (p1.IsHitAttack() && !p2.IsHitAttack())
	{

		//攻撃が成功したら
		if (CheckSuccessAttack(p1,p2))
		{
			//カメラ揺らす
			gameManager.OnIsCameraShake();
			//ヒットエフェクト
			if (p1.IsCommand())
			{
				gameManager.LoadSpecialHitEffect(p1);
			}
			else
			{
				gameManager.LoadNormalHitEffect(p1);
			}

			//空中の敵に攻撃を当てたら
			if (!p2.IsGround())
			{
				if (giveVeloP2.y == 0)
				{
					giveVeloP2.y = kGiveVeloPowerY;
				}
				giveVeloP2.x = kGiveVeloPowerX;
			}
			//相手が左側なら反転
			//左の壁に当たってるかをチェックするのは左の壁に当たっている敵に
			//空中攻撃を当てると当たったプレイヤーが右に行ってしまい空いたスペースに
			//当てたプレイヤーが入ってしまうのでそれを防ぐために追加した条件
			if ((p2.GetPos().x < p1.GetPos().x) || ColCheckLeftWall(p1, camera))
			{
				giveVeloP2.x *= -1;
			}

			//0の場合はダウン属性攻撃
			if (p1.GetGiveNoActFrame() == 0)
			{
				p2.LoadStateDownAerial();
				if (giveVeloP2.y == 0)
				{
					giveVeloP2.y = kGiveVeloPowerY * 0.5f;
				}
			}
			else
			{
				p2.SetNoActFrame(p1.GetGiveNoActFrame());//硬直差
				p2.LoadStateHit();//ヒットモーション
			}
			p2.SetKnockback(giveVeloP2);
			p2.SetVelo(giveVeloP2);//相手をずらす
			p2.SetHp(p2.GetHp() - p1.GetGiveDamage());//ダメージ

			//ヒットの音
			m_seP2->Stop();
			m_seP2->SetSE(m_hitSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();
		}
		else//ガード
		{
			giveVeloP2.y = 0;
			//相手が左側なら反転
			if ((p2.GetPos().x < p1.GetPos().x) || ColCheckLeftWall(p1, camera))
			{
				giveVeloP2.x *= -1;
			}

			//この攻撃がコマンドの技なら削れる
			if (p1.IsCommand())
			{
				p2.SetHp(p2.GetHp() - (p1.GetGiveDamage() * kScrapMagin));//ダメージ
				//削り
				gameManager.LoadScrapeGuardEffect(p1);
			}
			else
			{
				//ガードエフェクト
				gameManager.LoadGuardEffect(p1);
			}

			p2.SetKnockback(giveVeloP2* kKnockbackMagin);//相手をずらす
			p2.SetVelo(giveVeloP2* kKnockbackMagin);
			p2.SetGuardFrame(p1.GetGiveGuardFrame());//ガード硬直

			//立ちガード
			if (!p2.IsSquat())
			{
				p2.LoadStateGuardStand();//ガードモーション
			}
			//しゃがみガード
			else
			{
				p2.LoadStateGuardSquat();//ガードモーション
			}

			//ヒットの音
			m_seP2->Stop();
			m_seP2->SetSE(m_guardSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();
		}
	

		//壁に当たってる敵を攻撃したとき
		if (ColCheckWall(p2, camera))
		{
			//相手じゃなくて自分がのけぞる
			giveVeloP2.y = 0;
			giveVeloP2.x *= -1;
			p1.SetKnockback(giveVeloP2);
			p1.SetVelo(giveVeloP2);
		}

		//エフェクトの位置
		gameManager.SetHitEffectPosP1(CreateHitEffectPosPtoP(p1, p2));
		p1.ResetAttackBox();	
	}
	//P2が攻撃を当てた時
	else if (!p1.IsHitAttack() && p2.IsHitAttack())
	{
		//攻撃が成功したら
		if (CheckSuccessAttack(p2, p1))
		{
			//カメラ揺らす
			gameManager.OnIsCameraShake();
			//ヒットエフェクト
			if (p2.IsCommand())
			{
				gameManager.LoadSpecialHitEffect(p2);
			}
			else
			{
				gameManager.LoadNormalHitEffect(p2);
			}
			//空中の敵に攻撃を当てたら
			if (!p1.IsGround())
			{
				if (giveVeloP1.y == 0)
				{
					giveVeloP1.y = kGiveVeloPowerY;
				}
				giveVeloP1.x = kGiveVeloPowerX;
			}
			//相手が左側なら反転
			if ((p1.GetPos().x < p2.GetPos().x) || ColCheckLeftWall(p2, camera))
			{
				giveVeloP1.x *= -1;
			}

			//0の場合はダウン属性攻撃
			if (p2.GetGiveNoActFrame() == 0)
			{
				p1.LoadStateDownAerial();
				if (giveVeloP1.y == 0)
				{
					giveVeloP1.y = kGiveVeloPowerY * 0.5f;
				}
			}
			else
			{
				p1.SetNoActFrame(p2.GetGiveNoActFrame());//硬直差
				p1.LoadStateHit();//ヒットモーション
			}

			p1.SetKnockback(giveVeloP1);
			p1.SetVelo(giveVeloP1);//相手をずらす
			p1.SetHp(p1.GetHp() - p2.GetGiveDamage());//ダメージ

			//ヒットの音
			m_seP1->Stop();
			m_seP1->SetSE(m_hitSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();
		
		}
		else//ガード
		{
			giveVeloP1.y = 0;
			//相手が左側なら反転
			if ((p1.GetPos().x < p2.GetPos().x) || ColCheckLeftWall(p2, camera))
			{
				giveVeloP1.x *= -1;
			}
			//この攻撃がコマンドの技なら削れる
			if (p2.IsCommand())
			{
				p1.SetHp(p1.GetHp() - (p2.GetGiveDamage() * kScrapMagin));//ダメージ
				//削り
				gameManager.LoadScrapeGuardEffect(p2);
			}
			else
			{
				//ガードエフェクト
				gameManager.LoadGuardEffect(p2);
			}
			p1.SetKnockback(giveVeloP1* kKnockbackMagin);
			p1.SetVelo(giveVeloP1* kKnockbackMagin);//相手をずらす
			p1.SetGuardFrame(p2.GetGiveGuardFrame());//ガード硬直
			//立ちガード
			if (!p1.IsSquat())
			{
				p1.LoadStateGuardStand();//ガードモーション
			}
			//しゃがみガード
			else
			{
				p1.LoadStateGuardSquat();//ガードモーション
			}
			
			//ガードの音
			m_seP1->Stop();
			m_seP1->SetSE(m_guardSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();
		
		}
		//壁に当たってる敵を攻撃したとき
		if (ColCheckWall(p1, camera))
		{
			//相手じゃなくて自分がのけぞる
			giveVeloP1.y = 0;
			giveVeloP1.x *= -1;
			p2.SetKnockback(giveVeloP1);
			p2.SetVelo(giveVeloP1);
		}

		//エフェクトの位置
		gameManager.SetHitEffectPosP2(CreateHitEffectPosPtoP(p2, p1));
		p2.ResetAttackBox();
	}
	
	//攻撃の処理が終わったら判定を消す
	if (p1.IsHitAttack() || p2.IsHitAttack())
	{
		//リセット
		p1.OffIsHitAttack();
		p2.OffIsHitAttack();
	}

	//プレイヤーのHPが0以下になった場合
	if (p1.GetHp() <= 0)
	{
		//カメラ揺らす
		gameManager.OnIsCameraShake();
		p1.SetHp(0);
		p1.LoadStateHit();//ヒットモーション
	}
	if (p2.GetHp() <= 0)
	{
		//カメラ揺らす
		gameManager.OnIsCameraShake();
		p2.SetHp(0);
		p2.LoadStateHit();//ヒットモーション
	}
	
}

//攻撃が通るか
bool CollisionCheck::CheckSuccessAttack(Player& attacker, Player& defender)
{
	//プレイヤーの攻撃の属性
	AttackAttributes currentAttributes = attacker.GetAttackAttributes();

	//上段攻撃をしたとき
	if (currentAttributes == AttackAttributes::Upper)
	{
		//ガードをしていたら
		if (defender.IsGuard())
		{
			return false;
		}
	}
	//下段攻撃をしたとき
	else if (currentAttributes == AttackAttributes::Lower)
	{
		//しゃがみガードをしていたら
		if (defender.IsGuard() && defender.IsSquat())
		{
			return false;
		}
	}
	//中段段攻撃をしたとき
	else if (currentAttributes == AttackAttributes::Middle)
	{
		//立ちガードをしていたら
		if (defender.IsGuard() && !defender.IsSquat())
		{
			return false;
		}
	}
	//ここまで来たら攻撃は成功
	return true;
}




