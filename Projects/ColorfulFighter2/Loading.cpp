#include "Loading.h"
#include <DxLib.h>
#include"game.h"

namespace
{
	//�L�����̉摜�̑傫��
	constexpr int kCharaWidth = 512;
	constexpr int kCharaHeight = 512;
	//�����̃A�j���[�V�����̐�
	constexpr int kWalkAnimNum = 8;
	constexpr int kWalkOneAnimIntervalFrame = 1;
	//�����̃X�s�[�h
	constexpr float kActor1WalkSpeed = 15.0f;
	constexpr float kActor2WalkSpeed = 13.0f;
	constexpr float kActor3WalkSpeed = 10.0f;
	constexpr float kActor4WalkSpeed = 7.0f;
	constexpr float kActor5WalkSpeed = 5.0f;

	//NowLoading�̕����̈ʒu
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
	//�񓯊��œǂݍ���
	//SetUseASyncLoadFlag(true);
	m_actor1.handle = LoadGraph("./img/Chara/White/playerbase/walk_front_001.png");
	m_actor2.handle = LoadGraph("./img/Chara/Red/playerbase/walk_front_001.png");
	m_actor3.handle = LoadGraph("./img/Chara/Blue/playerbase/walk_front_001.png");
	m_actor4.handle = LoadGraph("./img/Chara/Yellow/playerbase/walk_front_001.png");
	m_actor5.handle = LoadGraph("./img/Chara/Green/playerbase/walk_front_001.png");
}

Loading::~Loading()
{
	//�񓯊��œǂݍ���
	SetUseASyncLoadFlag(false);
}

void Loading::Update()
{
	if (GetASyncLoadNum() > 0)
	{
		++m_animCountFrame;
		//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
		//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
		int animMaxNum = m_actor1.animNum - 1;
		//�A�j���[�V�����̃t���[���𐔂���
		if ((m_animCountFrame % kWalkAnimNum == 0) && m_animCountFrame != 0)
		{
			++m_actor1.animIndex;
			++m_actor2.animIndex;
			++m_actor3.animIndex;
			++m_actor4.animIndex;
			++m_actor5.animIndex;
			//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
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
	DrawFormatString(0, 30, 0xff00ff, "������ = %d", GetASyncLoadNum());
	printfDx("������ = %d\n", GetASyncLoadNum());
#endif
	if (GetASyncLoadNum() > 0)
	{
		//���������Â�����
		SetDrawBright(150, 150, 150);
		DrawGraph(0, 0, m_backgroundHandle, false);
		SetDrawBright(255, 255, 255);
		DrawGraph(kNowLoadingPosX, kNowLoadingPosY, m_loadingHandle, true);
		//�؂�����v�Z����
		int sizeX, sizeY;
		GetGraphSize(m_actor1.handle, &sizeX, &sizeY);//�摜�T�C�Y
		int cutX = m_actor1.animIndex % (sizeX / kCharaWidth);//��
		int cutY = m_actor1.animIndex / (sizeX / kCharaWidth);//�c
		//�`��
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
