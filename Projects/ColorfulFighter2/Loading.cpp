#include "Loading.h"
#include <DxLib.h>
#include"game.h"

namespace
{
	//キャラの画像の大きさ
	constexpr int kCharaWidth = 512;
	constexpr int kCharaHeight = 512;
	//歩きのアニメーションの数
	constexpr int kWalkAnimNum = 8;
	constexpr int kWalkOneAnimIntervalFrame = 1;
	//歩きのスピード
	constexpr float kActor1WalkSpeed = 15.0f;
	constexpr float kActor2WalkSpeed = 13.0f;
	constexpr float kActor3WalkSpeed = 10.0f;
	constexpr float kActor4WalkSpeed = 7.0f;
	constexpr float kActor5WalkSpeed = 5.0f;

	//NowLoadingの文字の位置
	constexpr int kNowLoadingPosX = 1000;
	constexpr int kNowLoadingPosY = 800;
}

Loading::Loading() :
	m_loadingHandle(LoadGraph("./img/Loading/NowLoading.png")),
	m_backgroundHandle(LoadGraph("./img/title/TitleBack.png")),
	m_animCountFrame(0),
	m_actor1{-1,kWalkAnimNum,kWalkOneAnimIntervalFrame },
	m_actor2{-1,kWalkAnimNum,kWalkOneAnimIntervalFrame },
	m_actor3{-1,kWalkAnimNum,kWalkOneAnimIntervalFrame },
	m_actor4{-1,kWalkAnimNum,kWalkOneAnimIntervalFrame },
	m_actor5{-1,kWalkAnimNum,kWalkOneAnimIntervalFrame },
	m_actor1Pos(0.0f, Game::kScreenHeight / 2,0),
	m_actor2Pos(0.0f, Game::kScreenHeight / 2,0),
	m_actor3Pos(0.0f, Game::kScreenHeight / 2,0),
	m_actor4Pos(0.0f, Game::kScreenHeight / 2,0),
	m_actor5Pos(0.0f, Game::kScreenHeight / 2,0)
{
	//非同期で読み込み
	//SetUseASyncLoadFlag(true);
	m_actor1.handle = LoadGraph("./img/Chara/White/playerbase/walk_front_001.png");
	m_actor2.handle = LoadGraph("./img/Chara/Red/playerbase/walk_front_001.png");
	m_actor3.handle = LoadGraph("./img/Chara/Blue/playerbase/walk_front_001.png");
	m_actor4.handle = LoadGraph("./img/Chara/Yellow/playerbase/walk_front_001.png");
	m_actor5.handle = LoadGraph("./img/Chara/Green/playerbase/walk_front_001.png");
}

Loading::~Loading()
{
	//非同期で読み込み
	SetUseASyncLoadFlag(false);
}

void Loading::Update()
{
	if (GetASyncLoadNum() > 0)
	{
		++m_animCountFrame;
		//アニメーションの1枚目を0番として数えるので
		//アニメーションの最大数から-1した値が最後のアニメーション
		int animMaxNum = m_actor1.animNum - 1;
		//アニメーションのフレームを数える
		if ((m_animCountFrame % kWalkAnimNum == 0) && m_animCountFrame != 0)
		{
			++m_actor1.animIndex;
			++m_actor2.animIndex;
			++m_actor3.animIndex;
			++m_actor4.animIndex;
			++m_actor5.animIndex;
			//アニメーションの数が最大まで行ったとき
			if ((m_actor1.animIndex > animMaxNum))
			{
				m_actor1.animIndex = 0;
				m_actor2.animIndex = 0;
				m_actor3.animIndex = 0;
				m_actor4.animIndex = 0;
				m_actor5.animIndex = 0;
			}
		}

		m_actor1Pos.x += kActor1WalkSpeed;
		m_actor2Pos.x += kActor2WalkSpeed;
		m_actor3Pos.x += kActor3WalkSpeed;
		m_actor4Pos.x += kActor4WalkSpeed;
		m_actor5Pos.x += kActor5WalkSpeed;
		if (m_actor1Pos.x > Game::kScreenWidth + kCharaWidth)
		{
			m_actor1Pos.x = -kCharaWidth;
		}
		if (m_actor2Pos.x > Game::kScreenWidth + kCharaWidth)
		{
			m_actor2Pos.x = -kCharaWidth;
		}
		if (m_actor3Pos.x > Game::kScreenWidth + kCharaWidth)
		{
			m_actor3Pos.x = -kCharaWidth;
		}
		if (m_actor4Pos.x > Game::kScreenWidth + kCharaWidth)
		{
			m_actor4Pos.x = -kCharaWidth;
		}
		if (m_actor5Pos.x > Game::kScreenWidth + kCharaWidth)
		{
			m_actor5Pos.x = -kCharaWidth;
		}
	}
}

void Loading::Draw()
{
#if _DEBUG
	DrawFormatString(0, 30, 0xff00ff, "処理数 = %d", GetASyncLoadNum());
	printfDx("処理数 = %d\n", GetASyncLoadNum());
#endif
	if (GetASyncLoadNum() > 0)
	{
		//少しだけ暗くする
		SetDrawBright(150, 150, 150);
		DrawGraph(0, 0, m_backgroundHandle, false);
		SetDrawBright(255, 255, 255);
		DrawGraph(kNowLoadingPosX, kNowLoadingPosY, m_loadingHandle, true);
		//切り取るを計算する
		int sizeX, sizeY;
		GetGraphSize(m_actor1.handle, &sizeX, &sizeY);//画像サイズ
		int cutX = m_actor1.animIndex % (sizeX / kCharaWidth);//横
		int cutY = m_actor1.animIndex / (sizeX / kCharaWidth);//縦
		//描画
		DrawRectRotaGraphFast(m_actor1Pos.x, m_actor1Pos.y,
			kCharaWidth * cutX,
			kCharaHeight * cutY,
			kCharaWidth, kCharaHeight,
			1.0f, 0.0f, m_actor1.handle, true, false);
		DrawRectRotaGraphFast(m_actor2Pos.x, m_actor2Pos.y,
			kCharaWidth * cutX,
			kCharaHeight * cutY,
			kCharaWidth, kCharaHeight,
			1.0f, 0.0f, m_actor2.handle, true, false);
		DrawRectRotaGraphFast(m_actor3Pos.x, m_actor3Pos.y,
			kCharaWidth * cutX,
			kCharaHeight * cutY,
			kCharaWidth, kCharaHeight,
			1.0f, 0.0f, m_actor3.handle, true, false);
		DrawRectRotaGraphFast(m_actor4Pos.x, m_actor4Pos.y,
			kCharaWidth * cutX,
			kCharaHeight * cutY,
			kCharaWidth, kCharaHeight,
			1.0f, 0.0f, m_actor4.handle, true, false);
		DrawRectRotaGraphFast(m_actor5Pos.x, m_actor5Pos.y,
			kCharaWidth * cutX,
			kCharaHeight * cutY,
			kCharaWidth, kCharaHeight,
			1.0f, 0.0f, m_actor5.handle, true, false);
	}
}
