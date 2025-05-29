#include "GameManager.h"
#include "CollisionCheck.h"
#include "Player.h"
#include "Camera.h"
#include "UI.h"
#include "FadeManager.h"
#include "game.h"
#include "SE.h"
#include <cmath>
#include <DxLib.h>

namespace
{
	//ヒットストップの時間
	constexpr int kHitStopFinishFrame = 10;

	//2本先に取ったら勝ち
	constexpr int kGamesetWinNum = 2;

	//ラウンド切り替わりの時間
	constexpr int kChangeRoundFrame = 300;//5秒	

	//ラウンド開始の時間
	constexpr int kStartRoundFrame = 180;//2秒

	//1ラウンドの時間
	constexpr int kOneFrameTime = 60;
	constexpr int kRoundTime = 99;//99秒

	//ヒットエフェクトのアニメーション
	constexpr int kHitAnimFirstIndex = 36;
	constexpr int kHitAnimFinishIndex = 41;

	//ガードエフェクトのアニメーション
	constexpr int kGuardAnimFirstIndex = 12;
	constexpr int kGuardAnimFinishIndex = 17;

	constexpr int kEffectWidth = 160;
	constexpr int kEffectHeight = 160;
	
	
	//seボリューム
	constexpr int kSeVolume = 150;

	constexpr int kKoStopFinishFrame = 50;
}

GameManager::GameManager():
	m_hitStopFrameCount(0),
	m_changeRoundFrameCount(0),
	m_isHitStop(false),
	m_hitStopFinishFrame(kHitStopFinishFrame),
	m_isChangeRound(false),
	m_isGameset(false),
	m_isTimeUpOrKo(false),
	m_isStartRound(false),
	m_startRoundCount(0),
	m_winNumP1(0),
	m_winNumP2(0),
	m_roundNumber(1),
	m_isFadeOut(false),
	m_time(kRoundTime),
	m_countFrame(0),
	m_isDrawFrontP1(false),
	m_isCameraShake(false),
	//ヒットエフェクト
	//P1
	m_hitEffectPosP1(),
	m_hitAnimCountFrameP1(0),
	m_hitAnimIndexP1(kHitAnimFinishIndex),
	m_hitAnimFinishIndexP1(kHitAnimFinishIndex),
	m_effectSizeP1(1.0f),
	m_angleP1(1.0f),
	//P2
	m_hitEffectPosP2(),
	m_hitAnimCountFrameP2(0),
	m_hitAnimIndexP2(kHitAnimFinishIndex),
	m_hitAnimFinishIndexP2(kHitAnimFinishIndex),
	m_effectSizeP2(1.0f),
	m_angleP2(1.0f),
	//エフェクト
	m_redEffectHandle(LoadGraph("./img/HitEffect/Retro Impact Effect Pack 3 A.png")),
	m_purpleEffectHandle(LoadGraph("./img/HitEffect/Retro Impact Effect Pack 3 B.png")),
	m_greenEffectHandle(LoadGraph("./img/HitEffect/Retro Impact Effect Pack 3 D.png")),
	m_blueHandle(LoadGraph("./img/HitEffect/Retro Impact Effect Pack 3 C.png")),
	m_hitEffectHandleP1(m_redEffectHandle),
	m_hitEffectHandleP2(m_redEffectHandle),
	//SE
	m_round1SeHandle(LoadSoundMem("./SE/Round/Round1.mp3")),
	m_round2SeHandle(LoadSoundMem("./SE/Round/Round2.mp3")),
	m_round3SeHandle(LoadSoundMem("./SE/Round/Round3.mp3")),
	m_roundOverSeHandle(LoadSoundMem("./SE/Round/RoundOver.mp3")),
	m_fightSeHandle(LoadSoundMem("./SE/Round/Fight.mp3")),
	m_koSeHandle(LoadSoundMem("./SE/Round/Ko.mp3")),
	m_timeUpSeHandle(LoadSoundMem("./SE/Round/TimeUp.mp3"))
{
	m_collcheck = std::make_shared< CollisionCheck>();
	m_fadeManager = std::make_shared<FadeManager>();
	m_se = std::make_shared<SE>();
}

GameManager::~GameManager()
{
	//エフェクト
	DeleteGraph(m_redEffectHandle);
	DeleteGraph(m_purpleEffectHandle);
	DeleteGraph(m_greenEffectHandle);
	DeleteGraph(m_blueHandle);
	DeleteGraph(m_hitEffectHandleP1);
	DeleteGraph(m_hitEffectHandleP2);
	//SE
	DeleteGraph(m_round1SeHandle);
	DeleteGraph(m_round2SeHandle);
	DeleteGraph(m_round3SeHandle);
	DeleteGraph(m_roundOverSeHandle);
	DeleteGraph(m_fightSeHandle);
	DeleteGraph(m_koSeHandle);
	DeleteGraph(m_timeUpSeHandle);
}

void GameManager::Init()
{
	m_hitStopFrameCount = 0;//ヒットストップのフレームを数える
	m_hitStopFinishFrame = kHitStopFinishFrame;
	m_changeRoundFrameCount = 0;//ラウンドの切り替わりのフレーム
	m_isHitStop = false;
	m_isChangeRound = false;
	m_isTimeUpOrKo = false;
	m_isStartRound = false;
	m_isCameraShake = false;
	//フェードアウト
	m_isFadeOut = false;
	//タイマー
	m_time = kRoundTime;
	m_countFrame = 0;

	//P1
	m_hitAnimCountFrameP1 = 0;
	m_hitAnimIndexP1 = kHitAnimFinishIndex;
	m_hitAnimFinishIndexP1 = kHitAnimFinishIndex;
	m_effectSizeP1 = 1.0f;
	m_angleP1 = 1.0f;
		//P2
	m_hitAnimCountFrameP2 = 0;
	m_hitAnimIndexP2 = kHitAnimFinishIndex;
	m_hitAnimFinishIndexP2 = kHitAnimFinishIndex;
	m_effectSizeP2 = 1.0f;
	m_angleP2 = 1.0f;
}

void GameManager::Update(Player& p1, Player& p2, Bullet& bulletP1, Bullet& bulletP2, 
	Camera& camera, UI& ui)
{
	m_collcheck->Update(p1, p2,bulletP1,bulletP2, camera,*this);
	//ヒットストップがかかっていないときはfalse
	if (!m_isHitStop)
	{
		m_isCameraShake = false;
	}
#if _DEBUG
	DrawFormatString(0, 400, 0xffffff, "p1(%2.0f,%2.0f) p2(%2.0f,%2.0f)", p1.GetPos().x, p1.GetPos().y, p2.GetPos().x, p2.GetPos().y);
#endif
}

void GameManager::NoStopUpdate(Player& p1, Player& p2)
{
	//KOしてるかチェック
	CheckKO(p1, p2);

	//試合が開始したらカウントを進める
	if (m_isStartRound)
	{
		//試合時間
		Timer();
	}

	//フェードインしたらゲーム開始の準備
	if (m_fadeManager->IsFinishFadeIn())
	{
		//ゲーム開始時
		m_startRoundCount++;
		//ラウンドコールSE
		if (m_startRoundCount >= 120 && m_startRoundCount < 180)
		{
			//Fight!
			m_se->SetSE(m_fightSeHandle);
			m_se->Volume(kSeVolume);
			m_se->PlayOnce();
		}
		else if (m_startRoundCount < 120)
		{
			//ラウンド1
			if (m_roundNumber == 1)
			{
				m_se->SetSE(m_round1SeHandle);
			}
			//ラウンド2
			else if (m_roundNumber == 2)
			{
				m_se->SetSE(m_round2SeHandle);
			}
			//ラウンド3
			else if (m_roundNumber == 3)
			{
				m_se->SetSE(m_round3SeHandle);
			}
			//ラウンドOver
			else
			{
				m_se->SetSE(m_roundOverSeHandle);
			}
			//Fight!
			m_se->Volume(kSeVolume);
			m_se->PlayOnce();
		}
	
		//ラウンドスタート
		if ((m_startRoundCount > kStartRoundFrame) && !m_isStartRound)
		{
			m_isStartRound = true;
		}
	}
	//ヒットストップ
	HitStop();

	//ヒットエフェクトのアニメーション
	//P1
	m_hitAnimCountFrameP1++;
	//アニメーションのフレームを数える
	if (m_hitAnimCountFrameP1 % 4 == 0 && m_hitAnimCountFrameP1 != 0)
	{
		m_hitAnimIndexP1++;
		//アニメーションの数が最大まで行ったとき
		if ((m_hitAnimIndexP1 >= m_hitAnimFinishIndexP1))
		{
			m_hitAnimIndexP1 = m_hitAnimFinishIndexP1;
		}
	}
	//P2
	m_hitAnimCountFrameP2++;
	//アニメーションのフレームを数える
	if (m_hitAnimCountFrameP2 % 4 == 0 && m_hitAnimCountFrameP2 != 0)
	{
		m_hitAnimIndexP2++;
		//アニメーションの数が最大まで行ったとき
		if ((m_hitAnimIndexP2 >= m_hitAnimFinishIndexP2))
		{
			m_hitAnimIndexP2 = m_hitAnimFinishIndexP2;
		}
	}
}

void GameManager::Draw(Camera& camera)
{
	DrawFade();//フェード
	//ヒットエフェクト
	DrawHitEffect(camera);
}

void GameManager::DrawFade()
{
	//フェードインアウト
	m_fadeManager->DrawBlackFade(m_isFadeOut);
}

void GameManager::DrawHitEffect(Camera& camera)
{
	//描画
	//P1
	//切り取るを計算する
	int sizeX, sizeY;
	int cutX, cutY;
	//エフェクトを派手にしたいので下に別のエフェクトを重ねておく
	//青
	GetGraphSize(m_blueHandle, &sizeX, &sizeY);//画像サイズ
	cutX = m_hitAnimIndexP1 % (sizeX / kEffectWidth);//横
	cutY = m_hitAnimIndexP1 / (sizeX / kEffectHeight);//縦
	DrawRectRotaGraphFast(static_cast<int>(m_hitEffectPosP1.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_hitEffectPosP1.y) + static_cast<int>(camera.m_drawOffset.y),
		kEffectWidth * cutX,
		kEffectHeight * cutY,
		kEffectWidth, kEffectHeight,
		m_effectSizeP1 - 0.5f, 1.0f, m_blueHandle, true, false);

	cutX = m_hitAnimIndexP2 % (sizeX / kEffectWidth);//横
	cutY = m_hitAnimIndexP2 / (sizeX / kEffectHeight);//縦
	DrawRectRotaGraphFast(static_cast<int>(m_hitEffectPosP2.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_hitEffectPosP2.y) + static_cast<int>(camera.m_drawOffset.y),
		kEffectWidth * cutX,
		kEffectHeight * cutY,
		kEffectWidth, kEffectHeight,
		m_effectSizeP2 - 0.5f, 1.0f, m_blueHandle, true, false);

	//メイン
	//P1
	GetGraphSize(m_hitEffectHandleP1, &sizeX, &sizeY);//画像サイズ
	cutX = m_hitAnimIndexP1 % (sizeX / kEffectWidth);//横
	cutY = m_hitAnimIndexP1 / (sizeX / kEffectHeight);//縦
	DrawRectRotaGraphFast(static_cast<int>(m_hitEffectPosP1.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_hitEffectPosP1.y) + static_cast<int>(camera.m_drawOffset.y),
		kEffectWidth * cutX,
		kEffectHeight * cutY,
		kEffectWidth, kEffectHeight,
		m_effectSizeP1, m_angleP1, m_hitEffectHandleP1, true, false);
	//P2
	GetGraphSize(m_hitEffectHandleP2, &sizeX, &sizeY);//画像サイズ
	cutX = m_hitAnimIndexP2 % (sizeX / kEffectWidth);//横
	cutY = m_hitAnimIndexP2 / (sizeX / kEffectHeight);//縦
	DrawRectRotaGraphFast(static_cast<int>(m_hitEffectPosP2.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_hitEffectPosP2.y) + static_cast<int>(camera.m_drawOffset.y),
		kEffectWidth * cutX,
		kEffectHeight * cutY,
		kEffectWidth, kEffectHeight,
		m_effectSizeP2, m_angleP2, m_hitEffectHandleP2, true, false);
	//P1
	GetGraphSize(m_hitEffectHandleP1, &sizeX, &sizeY);//画像サイズ
	cutX = m_hitAnimIndexP1 % (sizeX / kEffectWidth);//横
	cutY = m_hitAnimIndexP1 / (sizeX / kEffectHeight);//縦
	DrawRectRotaGraphFast(static_cast<int>(m_hitEffectPosP1.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_hitEffectPosP1.y) + static_cast<int>(camera.m_drawOffset.y),
		kEffectWidth * cutX,
		kEffectHeight * cutY,
		kEffectWidth, kEffectHeight,
		m_effectSizeP1, m_angleP1 + 2.0f, m_hitEffectHandleP1, true, false);
	//P2
	GetGraphSize(m_hitEffectHandleP2, &sizeX, &sizeY);//画像サイズ
	cutX = m_hitAnimIndexP2 % (sizeX / kEffectWidth);//横
	cutY = m_hitAnimIndexP2 / (sizeX / kEffectHeight);//縦
	DrawRectRotaGraphFast(static_cast<int>(m_hitEffectPosP2.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_hitEffectPosP2.y) + static_cast<int>(camera.m_drawOffset.y),
		kEffectWidth * cutX,
		kEffectHeight * cutY,
		kEffectWidth, kEffectHeight,
		m_effectSizeP2, m_angleP2 + 2.0f, m_hitEffectHandleP2, true, false);

#if _DEBUG
	DrawFormatString(0, 700, 0xffff00, "P1のヒットエフェクト(%2.0f,%2.0f)", m_hitEffectPosP1.x, m_hitEffectPosP1.y);
	DrawFormatString(0, 720, 0xffff00, "P2のヒットエフェクト(%2.0f,%2.0f)", m_hitEffectPosP2.x, m_hitEffectPosP2.y);
#endif
}

void GameManager::LoadNormalHitEffect(Player& player)
{
	if (player.GetPlayerIndex() == PlayerIndex::Player1)
	{
		//ヒットハンドルをセット
		m_hitAnimCountFrameP1 = 0;
		m_hitAnimIndexP1 = kHitAnimFirstIndex;
		m_hitAnimFinishIndexP1 = kHitAnimFinishIndex;
		m_angleP1 *= 2.0f;
		m_hitEffectHandleP1 = m_greenEffectHandle;
		m_effectSizeP1 = 1.0f;
	}
	else if (player.GetPlayerIndex() == PlayerIndex::Player2)
	{
		//ヒットハンドルをセット
		m_hitAnimCountFrameP2 = 0;
		m_hitAnimIndexP2 = kHitAnimFirstIndex;
		m_hitAnimFinishIndexP2 = kHitAnimFinishIndex;
		m_angleP2 *= 2.0f;
		m_hitEffectHandleP2 = m_greenEffectHandle;
		m_effectSizeP2 = 1.0f;
	}
}

void GameManager::LoadSpecialHitEffect(Player& player)
{
	if (player.GetPlayerIndex() == PlayerIndex::Player1)
	{
		//ヒットハンドルをセット
		m_hitAnimCountFrameP1 = 0;
		m_hitAnimIndexP1 = kHitAnimFirstIndex;
		m_hitAnimFinishIndexP1 = kHitAnimFinishIndex;
		m_angleP1 *= 2.0f;
		m_hitEffectHandleP1 = m_redEffectHandle;
		m_effectSizeP1 = 1.3f;
	}
	else if (player.GetPlayerIndex() == PlayerIndex::Player2)
	{
		//ヒットハンドルをセット
		m_hitAnimCountFrameP2 = 0;
		m_hitAnimIndexP2 = kHitAnimFirstIndex;
		m_hitAnimFinishIndexP2 = kHitAnimFinishIndex;
		m_angleP2 *= 2.0f;
		m_hitEffectHandleP2 = m_redEffectHandle;
		m_effectSizeP2 = 1.3f;
	}
}

void GameManager::LoadGuardEffect(Player& player)
{
	if (player.GetPlayerIndex() == PlayerIndex::Player1)
	{
		//ヒットハンドルをセット
		m_hitEffectHandleP1 = m_blueHandle;
		m_hitAnimCountFrameP1 = 0;
		m_hitAnimIndexP1 = kGuardAnimFirstIndex;
		m_hitAnimFinishIndexP1 = kGuardAnimFinishIndex;
	}
	else if (player.GetPlayerIndex() == PlayerIndex::Player2)
	{
		//ヒットハンドルをセット
		m_hitEffectHandleP2 = m_blueHandle;
		m_hitAnimCountFrameP2 = 0;
		m_hitAnimIndexP2 = kGuardAnimFirstIndex;
		m_hitAnimFinishIndexP2 = kGuardAnimFinishIndex;
	}
}

void GameManager::LoadScrapeGuardEffect(Player& player)
{
	if (player.GetPlayerIndex() == PlayerIndex::Player1)
	{
		//ヒットハンドルをセット
		m_hitEffectHandleP1 = m_purpleEffectHandle;
		m_hitAnimCountFrameP1 = 0;
		m_hitAnimIndexP1 = kGuardAnimFirstIndex;
		m_hitAnimFinishIndexP1 = kGuardAnimFinishIndex;
	}
	else if (player.GetPlayerIndex() == PlayerIndex::Player2)
	{
		//ヒットハンドルをセット
		m_hitEffectHandleP2 = m_purpleEffectHandle;
		m_hitAnimCountFrameP2 = 0;
		m_hitAnimIndexP2 = kGuardAnimFirstIndex;
		m_hitAnimFinishIndexP2 = kGuardAnimFinishIndex;
	}
}

PlayerIndex GameManager::ResultWinPlayerIndex()
{
	if (m_winNumP1 > m_winNumP2)
	{
		return PlayerIndex::Player1;
	}
	else if (m_winNumP1 < m_winNumP2)
	{
		return PlayerIndex::Player2;
	}
}

//試合時間
void GameManager::Timer()
{
	m_countFrame++;
	if ((m_countFrame >= kOneFrameTime) && !m_isTimeUpOrKo)
	{
		m_time--;
		m_countFrame = 0;
	}
	
	if (m_time <= 0)
	{
		m_time = 0;
	}
}

void GameManager::CheckKO(Player& p1, Player& p2)
{
	//ラウンドをゲームシーンで切り替え
	if (m_changeRoundFrameCount > kChangeRoundFrame && !m_isGameset)
	{
		//フェードアウトしきったらラウンドを切り替え
		m_isFadeOut = true;
		if (m_fadeManager->IsFinishFadeOut())
		{
			//2本とったらゲーム終了
			if (m_winNumP1 >= kGamesetWinNum)
			{
				m_isGameset = true;
				return;
			}
			else if (m_winNumP2 >= kGamesetWinNum)
			{
				m_isGameset = true;
				return;
			}
			//次のラウンド
			m_isChangeRound = true;
			m_isTimeUpOrKo = false;
			m_isStartRound = false;
			m_startRoundCount = 0;
			m_roundNumber++;
			return;
		}
	}
	//どちらかのHPが尽きたらまたは時間が0になったら
	if (p1.GetHp() <= 0 || p2.GetHp() <= 0 || m_time <= 0)
	{
		//次のラウンドに切り替えるまでのフレームを数える
		m_changeRoundFrameCount++;
		//長めにヒットストップをかける
		m_hitStopFinishFrame = kKoStopFinishFrame;
		//KOかTimeup
		if (m_time <= 0)
		{
			m_se->SetSE(m_timeUpSeHandle);
		}
		else
		{
			m_se->SetSE(m_koSeHandle);
		}
		m_se->Volume(kSeVolume);
		m_se->PlayOnce();
		//相打ち
		if ((p1.GetHp() <= 0 && p2.GetHp() <= 0) || (p1.GetHp() == p2.GetHp() && m_time <= 0))
		{
			//引き分けはそのままラウンドチェンジ
			m_isTimeUpOrKo = true;
			return;
		}
		//P1の勝ち
		else if (((p1.GetHp() > 0 && p2.GetHp() <= 0) || (p1.GetHp() > p2.GetHp() && m_time <= 0)) && !m_isTimeUpOrKo)
		{
			m_isTimeUpOrKo = true;
			m_winNumP1++;
		}
		//P2の勝ち
		else if (((p1.GetHp() <= 0 && p2.GetHp() > 0) || (p1.GetHp() < p2.GetHp() && m_time <= 0)) && !m_isTimeUpOrKo)
		{
			m_isTimeUpOrKo = true;
			m_winNumP2++;
		}
	}
}

void GameManager::HitStop()
{
	//ヒットストップ
	if (m_isHitStop)
	{
		//動き出す
		if (m_hitStopFrameCount >= m_hitStopFinishFrame)
		{
			m_hitStopFrameCount = 0;
			m_isHitStop = false;
		}
		m_hitStopFrameCount++;
	}
}






