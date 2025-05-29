#include "FadeManager.h"
#include "game.h"
#include <DxLib.h>

namespace
{
	constexpr int kFadeSpeed = 5;
	//���邳
	constexpr int kMaxAlpha = 255;
}

FadeManager::FadeManager():
	m_fadeFrameCount(kMaxAlpha),
	m_isFinishFadeIn(false),
	m_isFinishFadeOut(false)
{
}

FadeManager::~FadeManager()
{
}

void FadeManager::DrawBlackFade(bool isFadeIn)
{
	//�t�F�[�h�C��
	if (isFadeIn)
	{
		m_fadeFrameCount += kFadeSpeed;
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeFrameCount);
		DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x111111, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	//�t�F�[�h�A�E�g
	else
	{
		m_fadeFrameCount -= kFadeSpeed;
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeFrameCount);
		DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x111111, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	//�ő�l�ɍs�����Ƃ�
	if (m_fadeFrameCount >= kMaxAlpha)
	{
		m_isFinishFadeOut = true;
		m_fadeFrameCount = kMaxAlpha;
	}
	else
	{
		m_isFinishFadeOut = false;
	}
	//�ŏ��l�ɍs�����Ƃ�
	if (m_fadeFrameCount <= 0)
	{
		m_isFinishFadeIn = true;
		m_fadeFrameCount = 0;
	}
	else
	{
		m_isFinishFadeIn = false;
	}
}

void FadeManager::DrawWhiteFade(bool isFadeOut)
{
	//�t�F�[�h�A�E�g
	if (isFadeOut)
	{
		m_fadeFrameCount += kFadeSpeed;
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeFrameCount);
		DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0xeeeeee, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	//�t�F�[�h�C��
	else
	{
		m_fadeFrameCount -= kFadeSpeed;
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeFrameCount);
		DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0xeeeeee, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	//�ő�l�ɍs�����Ƃ�
	if (m_fadeFrameCount >= kMaxAlpha)
	{
		m_isFinishFadeOut = true;
		m_fadeFrameCount = kMaxAlpha;
	}
	else
	{
		m_isFinishFadeOut = false;
	}
	//�ŏ��l�ɍs�����Ƃ�
	if (m_fadeFrameCount <= 0)
	{
		m_isFinishFadeIn = true;
		m_fadeFrameCount = 0;
	}
	else
	{
		m_isFinishFadeIn = false;
	}
}

