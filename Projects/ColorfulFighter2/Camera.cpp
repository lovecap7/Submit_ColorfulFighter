#include "Camera.h"
#include "Player.h"
#include "GameManager.h"
#include "game.h"
#include <cmath>
#include <DxLib.h>

namespace
{
	constexpr float kShakeVeloX = 20.0f;
	constexpr float kShakeVeloY = 10.0f;
	constexpr int kStageWidth = 2600;
}

Camera::Camera():
	m_pos(),
	m_drawOffset(),
	m_shakeVelo(),
	m_isInitShakeVelo(false)
{
}

Camera::~Camera()
{
}

void Camera::Init(Player& p1, Player& p2)
{
	m_pos.x = std::abs(p1.GetPos().x + p2.GetPos().x) / 2;
	m_pos.y = (std::abs(p1.GetPos().y + p2.GetPos().y) / 2) - 100;
}

void Camera::Update(Player& p1,Player& p2, GameManager& gameManager)
{
	//�v���C���[����l�Ƃ������Ă�����J������ǂ�
	if ((p1.GetHp() > 0) && (p2.GetHp() > 0))
	{
		if ((std::abs(p1.GetPos().x + p2.GetPos().x) / 2) < (Game::kScreenWidth / 2))
		{
			m_pos.x = Game::kScreenWidth / 2;
		}
		else if (std::abs(p1.GetPos().x + p2.GetPos().x) / 2 > (kStageWidth - (Game::kScreenWidth / 2)))
		{
			m_pos.x = (kStageWidth - (Game::kScreenWidth / 2));
		}
		else
		{
			//�J������X���W���v���C���[�̈ʒu�̔����̂Ƃ���ɂ���
			m_pos.x = std::abs(p1.GetPos().x + p2.GetPos().x) / 2;
		}
	}


	//�J�����̃|�W�V�����𓮂����Ƃ������o�������߂�
	//-1��������i��������Ȃ��ƃJ������X���W�����Z�����Ƃ����ɍs���Ă���悤�Ɍ�����j
	m_drawOffset.x = m_pos.x * -1;
	m_drawOffset.y = m_pos.y * -1;

	//�J��������ʂ̒����ɂ���
	m_drawOffset.x = m_drawOffset.x + (Game::kScreenWidth / 2);
	m_drawOffset.y = m_drawOffset.y + (Game::kScreenHeight / 2);
	
	//�q�b�g�X�g�b�v���h���
	ShakeCamera(p1, p2, gameManager);

#if _DEBUG
	DrawFormatString(300, 0, 0xffffff, "cameraPos(%2.0f,%2.0f)", m_pos.x, m_pos.y);
	DrawFormatString(300, 16, 0xffffff, "drawOffset(%2.0f,%2.0f)", m_drawOffset.x, m_drawOffset.y);
#endif
}

float Camera::GetCameraLeftWallPos()
{
	return m_pos.x - (Game::kScreenWidth / 2);
}

float Camera::GetCameraRightWallPos()
{
	return m_pos.x + (Game::kScreenWidth / 2);
}

void Camera::ShakeCamera(Player& p1, Player& p2, GameManager& gameManager)
{
	//�q�b�g�X�g�b�v���ɗh���
	if (gameManager.IsCameraShake())
	{
		//��x��������������
		if (!m_isInitShakeVelo)
		{
			if (!gameManager.IsTimeUpOrKo())
			{
				//�c�h�ꂾ��
				m_shakeVelo.x = 0;
				m_shakeVelo.y = kShakeVeloY;
			}
			else
			{
				//KO�����牡�h��������
				m_shakeVelo.x = kShakeVeloX;
				m_shakeVelo.y = kShakeVeloY;
			}
			m_isInitShakeVelo = true;
		}

		m_drawOffset.x += m_shakeVelo.x;
		m_drawOffset.y += m_shakeVelo.y;
		m_shakeVelo.x *= -1;
		m_shakeVelo.y *= -1;
		if (m_shakeVelo.x > 0)
		{
			m_shakeVelo.x--;
		}
		if (m_shakeVelo.y > 0)
		{
			m_shakeVelo.y--;
		}
	}
	else
	{
		m_isInitShakeVelo = false;
	}
}
