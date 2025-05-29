#include "CommandSelectScene.h"
#include "Dxlib.h"
#include "Input.h"
#include "SceneController.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "game.h"
#include "Chara.h"
#include "BGM.h"
#include "SE.h"
#include "FadeManager.h"

namespace
{
	constexpr int kCenterX = Game::kScreenWidth / 2;
	constexpr int kCenterY = Game::kScreenHeight / 2;
	//�J�[�\��
	constexpr int kCursorOffset = 50;
	//�L�����̉摜�̑傫��
	constexpr int kCharaWidth = 512;
	constexpr int kCharaHeight = 512;
	//�L�����̕\���ʒu
	constexpr int kCharaPosXP1 = 200;
	constexpr int kCharaPosXP2 = Game::kScreenWidth - 200;
	constexpr int kCharaPosY = 250;
	//�J���o���̐�
	constexpr int kCharaColorNum = 5;

	//�A�j���[�V����
	constexpr int kAnimNum = 5;//�ҋ@��ԂƏ��������̃A�j���[�V�����̐��𓯂��ɂ���
	constexpr int kOneAnimFrame = 5;

	//�R�}���h�Z�̐�
	constexpr int kCommandIndexMin = 1;
	constexpr int kCommandIndexMax = 9;
	//�i
	constexpr int kOneRowCommandNum = 3;
	constexpr int kOneColumnCommandNum = 3;

	//�A�C�R���̔��a
	constexpr int kIconRadius = 50;
	constexpr int kIconPosOffset = 200;
	//�R�}���h�̐�
	constexpr int kCommandNum = 9;
	//�R�}���h�A�C�R���̑傫��
	constexpr float kCommandIconImageScale = 0.5f;
	constexpr int kCommandIconImageWidth = 200;
	constexpr int kCommandIconImageHight = 200;
	//�I�ׂ�R�}���h�Z�̐�
	constexpr int kSelectCommandNum = 3;
	//�I�񂾋Z�̕\���ʒu
	constexpr float kSelectedCommandOffsetPosX = kCommandIconImageWidth * kCommandIconImageScale;
	constexpr float kSelectedCommandPosY = Game::kScreenHeight / 2.0f + 100.0f;

	//Ready
	constexpr float kReadyPosXP1 = 50.0f;
	constexpr float kReadyPosXP2 = Game::kScreenWidth - 350.0f;
	constexpr float kReadyPosY = kSelectedCommandPosY + kIconRadius;

	//BGM�{�����[��
	constexpr int kBgmVolume = 120;
	//SE�{�����[��
	constexpr int kSeVolume = 150;
	constexpr int kOkSeVolume = 180;

	//�_�ł̃X�p��
	constexpr int kBlinkSpan = 50;

	//�R�}���h�̐ݒ�
	constexpr int kCommandInfoPosXP1 = kCenterX - 500;
	constexpr int kCommandInfoPosXP2 = kCenterX + 500;
	constexpr int kCommandInfoPosY = kCenterY - 200;
}

void CommandSelectScene::UpdateBlink()
{
	++m_blinkCountFrame;
	if (m_blinkCountFrame >= kBlinkSpan * 2)
	{
		m_blinkCountFrame = 0;
	}
}

void CommandSelectScene::UpdateCharaAnim()
{
	//�A�j���[�V����
	++m_animCountFrame;
	int animMaxNum = kAnimNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % kOneAnimFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndexP1++;
		m_animIndexP2++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if (m_animIndexP1 > animMaxNum)
		{
			if (m_isReadyP1)
			{
				m_animIndexP1 = animMaxNum;
			}
			else
			{
				m_animIndexP1 = 0;
			}
		}
		if (m_animIndexP2 > animMaxNum)
		{
			if (m_isReadyP2)
			{
				m_animIndexP2 = animMaxNum;
			}
			else
			{
				m_animIndexP2 = 0;
			}
		}
	}
}


void CommandSelectScene::SelectCommandP1(Input& input)
{
	//�����������݂��ɂ��Ă���ꍇ�͑���ł��Ȃ�
	if (m_isReadyP1 && m_isReadyP2)
	{
		return;
	}
	if (m_isSelectFinishP1)
	{
		//�������\��
		m_isCommandInfoDisplayP1 = false;

		if (input.IsTrigger("A") && !m_isReadyP1)
		{
			//����̉�
			m_seP1->Stop();
			m_seP1->SetSE(m_readySeHandle);
			m_seP1->Volume(kOkSeVolume);
			m_seP1->PlayOnce();
			//��������
			m_currentReadyP1Handle = m_readyHandle;
			m_isReadyP1 = true;
			m_animIndexP1 = 0;
		}
		//�L�����Z��
		if (input.IsTrigger("B"))
		{
			//������
			if (m_isReadyP1)
			{
				//�L�����Z���̉�
				m_seP1->Stop();
				m_seP1->SetSE(m_cancelSeHandle);
				m_seP1->Volume(kSeVolume);
				m_seP1->PlayOnce();
				//�m�F�ɕϊ�
				m_currentReadyP1Handle = m_gettingReadyHandle;
				m_isReadyP1 = false;
			}
			else//�܂��I�ђ���
			{
				//�L�����Z���̉�
				m_seP1->Stop();
				m_seP1->SetSE(m_cancelSeHandle);
				m_seP1->Volume(kSeVolume);
				m_seP1->PlayOnce();
				m_isSelectFinishP1 = false;
				for (int i = 0; i < kSelectCommandNum - 1; ++i)
				{
					//�Ō�ɑI�񂾏��ɏ���
					if (m_selectCommandIndexP1[(kSelectCommandNum - 1) - i] != 0)
					{
						m_selectCommandIndexP1[(kSelectCommandNum - 1) - i] = 0;
						m_selectCommandIconP1Handle[(kSelectCommandNum - 1) - i] = m_nullCommandIconHandle;
						break;
					}
				}
			}

		}
	}
	else
	{
		//�I��
		if (input.IsTrigger("Left"))
		{
			//�J�[�\���ړ��̉�
			m_seP1->Stop();
			m_seP1->SetSE(m_cursorMoveSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();

			m_currentSelectCommandIndexP1--;
			if (m_currentSelectCommandIndexP1 % 3 == 0)
			{
				m_currentSelectCommandIndexP1 += kOneRowCommandNum;
			}
		}
		if (input.IsTrigger("Right"))
		{
			//�J�[�\���ړ��̉�
			m_seP1->Stop();
			m_seP1->SetSE(m_cursorMoveSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();

			m_currentSelectCommandIndexP1++;
			if ((m_currentSelectCommandIndexP1 - 1) % kOneRowCommandNum == 0)
			{
				m_currentSelectCommandIndexP1 -= kOneRowCommandNum;
			}
		}
		if (input.IsTrigger("Up"))
		{
			//�J�[�\���ړ��̉�
			m_seP1->Stop();
			m_seP1->SetSE(m_cursorMoveSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();

			m_currentSelectCommandIndexP1 -= kOneColumnCommandNum;
			if (m_currentSelectCommandIndexP1 < kCommandIndexMin)
			{
				m_currentSelectCommandIndexP1 += (kOneColumnCommandNum * kOneRowCommandNum);
			}
		}
		if (input.IsTrigger("Down"))
		{
			//�J�[�\���ړ��̉�
			m_seP1->Stop();
			m_seP1->SetSE(m_cursorMoveSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();

			m_currentSelectCommandIndexP1 += kOneColumnCommandNum;
			if (m_currentSelectCommandIndexP1 > kCommandIndexMax)
			{
				m_currentSelectCommandIndexP1 -= (kOneColumnCommandNum * kOneRowCommandNum);
			}

		}
		if (input.IsTrigger("A"))
		{
			//����̉�
			m_seP1->Stop();
			m_seP1->SetSE(m_selectSeHandle);
			m_seP1->Volume(kOkSeVolume);
			m_seP1->PlayOnce();
			for (int i = 0; i < kSelectCommandNum; ++i)
			{
				//�����Z�͑I�ׂȂ�
				if (m_selectCommandIndexP1[i] == m_currentSelectCommandIndexP1)
				{
					break;
				}
				//��Ȃ�C���f�b�N�X������
				if (m_selectCommandIndexP1[i] == 0)
				{
					m_selectCommandIndexP1[i] = m_currentSelectCommandIndexP1;
					m_selectCommandIconP1Handle[i] = m_commandIconHandle[m_selectCommandIndexP1[i] - 1];//�A�C�R���\��
					break;
				}
			}
			//3�I�񂾂�J�[�\���𓮂����Ȃ�����
			if (m_selectCommandIndexP1[(kSelectCommandNum - 1)] != 0)
			{
				m_isSelectFinishP1 = true;
			}
		}
		//�I�тȂ���
		if (input.IsTrigger("B"))
		{
			//�L�����Z���̉�
			m_seP1->Stop();
			m_seP1->SetSE(m_cancelSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();
			m_isSelectFinishP1 = false;
			for (int i = 0; i < kSelectCommandNum; ++i)
			{
				//�Ō�ɑI�񂾏��ɏ���
				if (m_selectCommandIndexP1[(kSelectCommandNum - 1) - i] != 0)
				{
					m_selectCommandIndexP1[(kSelectCommandNum - 1) - i] = 0;
					m_selectCommandIconP1Handle[(kSelectCommandNum - 1) - i] = m_nullCommandIconHandle;
					break;
				}
			}
		}
		//�����̕\��
		if (input.IsTrigger("Y"))
		{
			m_isCommandInfoDisplayP1 = !m_isCommandInfoDisplayP1;
		}

		//�������ł��Ă��Ȃ����Ƃ�\��
		m_currentReadyP1Handle = m_gettingReadyHandle;
		m_isReadyP1 = false;
	}
}

void CommandSelectScene::SelectCommandP2(Input& input)
{
	//�����������݂��ɂ��Ă���ꍇ�͑���ł��Ȃ�
	if (m_isReadyP1 && m_isReadyP2)
	{
		return;
	}
	if (m_isSelectFinishP2)
	{
		//�������\��
		m_isCommandInfoDisplayP2 = false;

		if (input.IsTrigger("A") && !m_isReadyP2)
		{
			//����̉�
			m_seP2->Stop();
			m_seP2->SetSE(m_readySeHandle);
			m_seP2->Volume(kOkSeVolume);
			m_seP2->PlayOnce();
			//��������
			m_currentReadyP2Handle = m_readyHandle;
			m_isReadyP2 = true;
			m_animIndexP2 = 0;
		}
		//������
		if (input.IsTrigger("B"))
		{
			if (m_isReadyP2)
			{
				//�L�����Z���̉�
				m_seP2->Stop();
				m_seP2->SetSE(m_cancelSeHandle);
				m_seP2->Volume(kSeVolume);
				m_seP2->PlayOnce();
				//�m�F�ɕϊ�
				m_currentReadyP2Handle = m_gettingReadyHandle;
				m_isReadyP2 = false;
			}
			else//�܂��I�ђ���
			{
				//�L�����Z���̉�
				m_seP2->Stop();
				m_seP2->SetSE(m_cancelSeHandle);
				m_seP2->Volume(kSeVolume);
				m_seP2->PlayOnce();
				m_isSelectFinishP2 = false;
				//�Ō�ɑI�񂾏��ɏ���
				for (int i = 0; i < 3; ++i)
				{
					if (m_selectCommandIndexP2[(kSelectCommandNum - 1) - i] != 0)
					{
						m_selectCommandIndexP2[(kSelectCommandNum - 1) - i] = 0;
						m_selectCommandIconP2Handle[(kSelectCommandNum - 1) - i] = m_nullCommandIconHandle;
						break;
					}
				}
			}
		}
	}
	else
	{
		if (input.IsTrigger("Left"))
		{
			//�J�[�\���ړ��̉�
			m_seP2->Stop();
			m_seP2->SetSE(m_cursorMoveSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();

			m_currentSelectCommandIndexP2--;
			if (m_currentSelectCommandIndexP2 % 3 == 0)
			{
				m_currentSelectCommandIndexP2 += kOneRowCommandNum;
			}
		}
		if (input.IsTrigger("Right"))
		{
			//�J�[�\���ړ��̉�
			m_seP2->Stop();
			m_seP2->SetSE(m_cursorMoveSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();

			m_currentSelectCommandIndexP2++;
			if ((m_currentSelectCommandIndexP2 - 1) % kOneRowCommandNum == 0)
			{
				m_currentSelectCommandIndexP2 -= kOneRowCommandNum;
			}
		}
		if (input.IsTrigger("Up"))
		{
			//�J�[�\���ړ��̉�
			m_seP2->Stop();
			m_seP2->SetSE(m_cursorMoveSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();

			m_currentSelectCommandIndexP2 -= kOneColumnCommandNum;
			if (m_currentSelectCommandIndexP2 < kCommandIndexMin)
			{
				m_currentSelectCommandIndexP2 += (kOneColumnCommandNum * kOneRowCommandNum);
			}
		}
		if (input.IsTrigger("Down"))
		{
			//�J�[�\���ړ��̉�
			m_seP2->Stop();
			m_seP2->SetSE(m_cursorMoveSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();

			m_currentSelectCommandIndexP2 += kOneColumnCommandNum;
			if (m_currentSelectCommandIndexP2 > kCommandIndexMax)
			{
				m_currentSelectCommandIndexP2 -= (kOneColumnCommandNum * kOneRowCommandNum);
			}

		}
		//����
		if (input.IsTrigger("A"))
		{
			//����̉�
			m_seP2->Stop();
			m_seP2->SetSE(m_selectSeHandle);
			m_seP2->Volume(kOkSeVolume);
			m_seP2->PlayOnce();
			for (int i = 0; i < 3; ++i)
			{
				//�����Z�͑I�ׂȂ�
				if (m_selectCommandIndexP2[i] == m_currentSelectCommandIndexP2)
				{
					break;
				}
				//��Ȃ�C���f�b�N�X������
				if (m_selectCommandIndexP2[i] == 0)
				{
					m_selectCommandIndexP2[i] = m_currentSelectCommandIndexP2;
					m_selectCommandIconP2Handle[i] = m_commandIconHandle[m_selectCommandIndexP2[i] - 1];//�A�C�R���\��
					break;
				}
			}
			if (m_selectCommandIndexP2[(kSelectCommandNum - 1)] != 0)
			{
				m_isSelectFinishP2 = true;
			}
		}
		//�I�тȂ���
		if (input.IsTrigger("B"))
		{
			//�L�����Z���̉�
			m_seP2->Stop();
			m_seP2->SetSE(m_cancelSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();
			m_isSelectFinishP2 = false;
			//�Ō�ɑI�񂾏��ɏ���
			for (int i = 0; i < kSelectCommandNum; ++i)
			{
				if (m_selectCommandIndexP2[(kSelectCommandNum - 1) - i] != 0)
				{
					m_selectCommandIndexP2[(kSelectCommandNum - 1) - i] = 0;
					m_selectCommandIconP2Handle[(kSelectCommandNum - 1) - i] = m_nullCommandIconHandle;
					break;
				}
			}
		}
		//�Z�̐�����\��
		if (input.IsTrigger("Y"))
		{
			m_isCommandInfoDisplayP2 = !m_isCommandInfoDisplayP2;
		}

		//�������ł��Ă��Ȃ����Ƃ�\��
		m_currentReadyP2Handle = m_gettingReadyHandle;
		m_isReadyP2 = false;
	}
}


void CommandSelectScene::DrawCommandInfo()
{
	if (m_isCommandInfoDisplayP1)
	{
		//������\��
		DrawRotaGraph(kCommandInfoPosXP1, kCommandInfoPosY, 1.0, 0, m_commandInfoHandle[m_currentSelectCommandIndexP1 - 1], true, 0, 0);
	}
	if (m_isCommandInfoDisplayP2)
	{
		//������\��
		DrawRotaGraph(kCommandInfoPosXP2, kCommandInfoPosY, 1.0, 0, m_commandInfoHandle[m_currentSelectCommandIndexP2 - 1], true, 0, 0);
	}
}

void CommandSelectScene::SelectColorP1(Input& input)
{
	if (!m_isReadyP1)
	{
		if (input.IsTrigger("X"))
		{
			//����̉�
			m_seP1->Stop();
			m_seP1->SetSE(m_selectSeHandle);
			m_seP1->Volume(kOkSeVolume);
			m_seP1->PlayOnce();
			m_currentColorIndexP1++;
		}
		if (m_currentColorIndexP1 > (kCharaColorNum - 1))
		{
			m_currentColorIndexP1 = 0;
		}
		m_charaP1Handle = m_idlePoseHandle[m_currentColorIndexP1];
	}
	else
	{
		m_charaP1Handle = m_readyPoseHandle[m_currentColorIndexP1];
	}
}

void CommandSelectScene::SelectColorP2(Input& input2)
{
	if (!m_isReadyP2)
	{
		if (input2.IsTrigger("X"))
		{
			//����̉�
			m_seP2->Stop();
			m_seP2->SetSE(m_selectSeHandle);
			m_seP2->Volume(kOkSeVolume);
			m_seP2->PlayOnce();
			m_currentColorIndexP2++;
		}
		if (m_currentColorIndexP2 > (kCharaColorNum - 1))
		{
			m_currentColorIndexP2 = 0;
		}
		m_charaP2Handle = m_idlePoseHandle[m_currentColorIndexP2];
	}
	else
	{
		m_charaP2Handle = m_readyPoseHandle[m_currentColorIndexP2];
	}
}



void CommandSelectScene::DrawCharacter()
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_charaP1Handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndexP1 % (sizeX / kCharaWidth);//��
	int cutY = m_animIndexP1 / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(kCharaPosXP1, kCharaPosY,
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		1.0f, 0.0f, m_charaP1Handle, true, false);
	//�؂�����v�Z����
	GetGraphSize(m_charaP2Handle, &sizeX, &sizeY);//�摜�T�C�Y
	cutX = m_animIndexP2 % (sizeX / kCharaWidth);//��
	cutY = m_animIndexP2 / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(kCharaPosXP2, kCharaPosY,
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		1.0f, 0.0f, m_charaP2Handle, true, true);
}

void CommandSelectScene::DrawCommandIcon()
{
	//�Z1
	DrawCircle(kCenterX - kIconPosOffset, kCenterY - kIconPosOffset, kIconRadius, 0x22ff22, true, true);
	//�Z2
	DrawCircle(kCenterX, kCenterY - kIconPosOffset, kIconRadius, 0xff2222, true, true);
	//�Z3
	DrawCircle(kCenterX + kIconPosOffset, kCenterY - kIconPosOffset, kIconRadius, 0x2222ff, true, true);
	//�Z4
	DrawCircle(kCenterX - kIconPosOffset, kCenterY, kIconRadius, 0xff22ff, true, true);
	//�Z5
	DrawCircle(kCenterX, kCenterY, kIconRadius, 0x22ffff, true, true);
	//�Z6
	DrawCircle(kCenterX + kIconPosOffset, kCenterY, kIconRadius, 0xffff22, true, true);
	//�Z7
	DrawCircle(kCenterX - kIconPosOffset, kCenterY + kIconPosOffset, kIconRadius, 0xcff7700, true, true);
	//�Z8
	DrawCircle(kCenterX, kCenterY + kIconPosOffset, kIconRadius, 0x000000, true, true);
	//�Z9
	DrawCircle(kCenterX + kIconPosOffset, kCenterY + kIconPosOffset, kIconRadius, 0xffffff, true, true);

	for (int i = 0; i < kCommandNum; ++i)
	{
		switch (i)
		{
		case 0:
			//�Z1
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY - kIconPosOffset, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 1:
			//�Z2
			DxLib::DrawRotaGraph(kCenterX, kCenterY - kIconPosOffset, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 2:
			//�Z3
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY - kIconPosOffset, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 3:
			//�Z4
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 4:
			//�Z5
			DxLib::DrawRotaGraph(kCenterX, kCenterY, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 5:
			//�Z6
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 6:
			//�Z7
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY + kIconPosOffset, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 7:
			//�Z8
			DxLib::DrawRotaGraph(kCenterX, kCenterY + kIconPosOffset, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 8:
			//�Z9
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY + kIconPosOffset, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		default:
			break;
		}
	}
}

void CommandSelectScene::DrawCursor()
{
	//�I�яI��������\��
	if (!m_isSelectFinishP1)
	{
		if (m_blinkCountFrame <= kBlinkSpan)
		{
			//�J�[�\����������
			DxLib::SetDrawBright(200, 0, 0);
		}
		switch (m_currentSelectCommandIndexP1)
		{
		case 1:
			//�Z1
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY - kIconPosOffset, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 2:
			//�Z2
			DxLib::DrawRotaGraph(kCenterX, kCenterY - kIconPosOffset, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 3:
			//�Z3
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY - kIconPosOffset, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 4:
			//�Z4
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 5:
			//�Z5
			DxLib::DrawRotaGraph(kCenterX, kCenterY, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 6:
			//�Z6
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 7:
			//�Z7
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY + kIconPosOffset, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 8:
			//�Z8
			DxLib::DrawRotaGraph(kCenterX, kCenterY + kIconPosOffset, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 9:
			//�Z9
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY + kIconPosOffset, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		default:
			break;
		}
		DxLib::SetDrawBright(255, 255, 255);
	}
	if (!m_isSelectFinishP2)
	{
		//P2
		//��������
		if (m_blinkCountFrame <= kBlinkSpan)
		{
			//�J�[�\����������
			DxLib::SetDrawBright(0, 0, 200);
		}
		switch (m_currentSelectCommandIndexP2)
		{
		case 1:
			//�Z1
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY - kIconPosOffset, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 2:
			//�Z2
			DxLib::DrawRotaGraph(kCenterX, kCenterY - kIconPosOffset, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 3:
			//�Z3
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY - kIconPosOffset, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 4:
			//�Z4
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 5:
			//�Z5
			DxLib::DrawRotaGraph(kCenterX, kCenterY, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 6:
			//�Z6
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 7:
			//�Z7
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY + kIconPosOffset, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 8:
			//�Z8
			DxLib::DrawRotaGraph(kCenterX, kCenterY + kIconPosOffset, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 9:
			//�Z9
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY + kIconPosOffset, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		default:
			break;
		}
		DxLib::SetDrawBright(255, 255, 255);
	}
}

void CommandSelectScene::DrawSelectText()
{
	if (m_blinkCountFrame <= kBlinkSpan)
	{
		//�J�[�\����������
		SetDrawBlendMode(DX_BLENDMODE_INVSRC, 255);
	}
	DrawRotaGraph(kCenterX, 100, 1.0, 0, m_selectTextHandle, true, 0, 0);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}

void CommandSelectScene::DrawSelectPlayerCommandIcon()
{
	//�R�}���h
	for (int i = 0; i < kSelectCommandNum; ++i)
	{
		//�I�񂾋Z�̃A�C�R��
		DrawRectRotaGraphFast(
			static_cast<float>(kSelectedCommandOffsetPosX * (i + 1)),
			kSelectedCommandPosY,
			0, 0, kCommandIconImageWidth, kCommandIconImageHight,
			kCommandIconImageScale, 0.0f, m_selectCommandIconP1Handle[i], true);
		DrawRectRotaGraphFast(
			static_cast<float>(Game::kScreenWidth - kSelectedCommandOffsetPosX * (i + 1)),
			kSelectedCommandPosY,
			0, 0, kCommandIconImageWidth, kCommandIconImageHight,
			kCommandIconImageScale, 0.0f, m_selectCommandIconP2Handle[i], true);
	}
}

void CommandSelectScene::DrawReady()
{
	//�����ł������̊m�F
	if (m_isSelectFinishP1)
	{
		if ((m_blinkCountFrame <= kBlinkSpan) && !m_isReadyP1)
		{
			DxLib::SetDrawBright(150, 150, 150);
		}
		DxLib::DrawGraph(kReadyPosXP1, kReadyPosY, m_currentReadyP1Handle, true);//1P��Ready
		DxLib::SetDrawBright(255, 255, 255);
	}
	if (m_isSelectFinishP2)
	{
		if ((m_blinkCountFrame <= kBlinkSpan) && !m_isReadyP2)
		{
			DxLib::SetDrawBright(150, 150, 150);
		}
		DxLib::DrawGraph(kReadyPosXP2, kReadyPosY, m_currentReadyP2Handle, true);//2P��Ready
		DxLib::SetDrawBright(255, 255, 255);
	}
}

CommandSelectScene::CommandSelectScene(SceneController& controller) :
	SceneBase(controller),
	m_currentSelectCommandIndexP1(1),
	m_currentSelectCommandIndexP2(2),
	m_isSelectFinishP1(false),
	m_isSelectFinishP2(false),
	m_isReadyP1(false),
	m_isReadyP2(false),
	m_selectCommandIndexP1{ 0,0,0 },
	m_selectCommandIndexP2{ 0,0,0 },
	m_isFadeOut(false),
	m_blinkCountFrame(0),
	m_animCountFrame(0),
	m_animIndexP1(0),
	m_animIndexP2(0),
	//�J�[�\��
	m_cursorP1Handle(LoadGraph("img/CharacterSelect/Icon/SelectFrameP1.png")),
	m_cursorP2Handle(LoadGraph("img/CharacterSelect/Icon/SelectFrameP2.png")),
	//SE�̉���
	m_selectSeHandle(LoadSoundMem("./SE/Select/SelectSE.mp3")),
	m_readySeHandle(LoadSoundMem("./SE/Select/ReadySE.mp3")),
	m_cursorMoveSeHandle(LoadSoundMem("./SE/Select/CursorMoveSE.mp3")),
	m_cancelSeHandle(LoadSoundMem("./SE/Select/CancelSE.mp3")),
	//�w�i
	m_backGroundHandle(LoadGraph("img/CharacterSelect/CommandSelectBack.png")),
	//�e�L�X�g
	m_selectTextHandle(LoadGraph("img/CharacterSelect/SelectText.png")),
	//�F
	m_currentColorIndexP1(0),
	m_currentColorIndexP2(0),
	//����Ɋւ���UI
	m_operationHandle(LoadGraph("img/CharacterSelect/Operation.png")),
	//�R�}���h�Z�̐���
	m_isCommandInfoDisplayP1(false),
	m_isCommandInfoDisplayP2(false),
	m_commandInfoHandle{
		LoadGraph("./img/CharacterSelect/CommandInfo/Command1Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command2Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command3Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command4Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command5Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command6Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command7Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command8Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command9Info.png")
	},
	m_charaP1Handle(-1),
	m_charaP2Handle(-1),
	m_commandIconHandle{},
	m_idlePoseHandle{},
	m_readyPoseHandle{},
	m_gettingReadyHandle(LoadGraph("./img/CharacterSelect/Ready_Off.png")),//������
	m_readyHandle(LoadGraph("./img/CharacterSelect/Ready_On.png")),//��������
	m_currentReadyP1Handle(m_gettingReadyHandle),//�����������ǂ�����\��
	m_currentReadyP2Handle(m_gettingReadyHandle),
	m_loadingHandle(LoadGraph("./img/Loading/Operation.png"))
{
	//BGM
	m_bgm = std::make_shared<BGM>();
	m_bgmHandle = LoadSoundMem("./BGM/BGM_SelectScene.mp3");
	m_bgm->SetBGM(m_bgmHandle);
	m_bgm->Volume(kBgmVolume);
	m_bgm->PlayLoop();
	//SE
	m_seP1 = std::make_shared<SE>();
	m_seP2 = std::make_shared<SE>();

	for (int i = 0; i < kCharaColorNum; ++i)
	{
		switch (i)
		{
		case 0:
			m_idlePoseHandle[i] = LoadGraph("img/Chara/White/playerbase/idle_001.png");
			m_readyPoseHandle[i] = LoadGraph("img/Chara/White/playerbase/win_001.png");
			break;
		case 1:
			m_idlePoseHandle[i] = LoadGraph("img/Chara/Red/playerbase/idle_001.png");
			m_readyPoseHandle[i] = LoadGraph("img/Chara/Red/playerbase/win_001.png");
			break;
		case 2:
			m_idlePoseHandle[i] = LoadGraph("img/Chara/Blue/playerbase/idle_001.png");
			m_readyPoseHandle[i] = LoadGraph("img/Chara/Blue/playerbase/win_001.png");
			break;
		case 3:
			m_idlePoseHandle[i] = LoadGraph("img/Chara/Yellow/playerbase/idle_001.png");
			m_readyPoseHandle[i] = LoadGraph("img/Chara/Yellow/playerbase/win_001.png");
			break;
		case 4:
			m_idlePoseHandle[i] = LoadGraph("img/Chara/Green/playerbase/idle_001.png");
			m_readyPoseHandle[i] = LoadGraph("img/Chara/Green/playerbase/win_001.png");
			break;
		default:
			break;
		}
	}
	m_charaP1Handle = m_idlePoseHandle[0];
	m_charaP2Handle = m_idlePoseHandle[0];

	//�A�C�R��
	for (int i = 0; i < kCommandNum; ++i)
	{
		switch (i)
		{
		case 0:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command1.png");
			break;
		case 1:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command2.png");
			break;
		case 2:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command3.png");
			break;
		case 3:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command4.png");
			break;
		case 4:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command5.png");
			break;
		case 5:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command6.png");
			break;
		case 6:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command7.png");
			break;
		case 7:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command8.png");
			break;
		case 8:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command9.png");
			break;
		default:
			break;
		}
	}
	//��ł��邱�Ƃ�\��
	m_nullCommandIconHandle = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/SelectNull.png");
	for (int i = 0; i < kSelectCommandNum; ++i)
	{
		m_selectCommandIconP1Handle[i] = m_nullCommandIconHandle;
		m_selectCommandIconP2Handle[i] = m_nullCommandIconHandle;
	}
	//�t�F�[�h�C������Ƃ��Ɏg��
	m_fadeManager = std::make_shared<FadeManager>();
}

CommandSelectScene::~CommandSelectScene()
{
	//�R�}���h�Z�̃A�C�R��
	for (int i = 0; i < kCommandNum; ++i)
	{
		DeleteGraph(m_commandIconHandle[i]);
	}
	//�I�񂾃R�}���h�Z�̃A�C�R��
	for (int i = 0; i < kSelectCommandNum; ++i)
	{
		DeleteGraph(m_selectCommandIconP1Handle[i]);
		DeleteGraph(m_selectCommandIconP2Handle[i]);
	}
	DeleteGraph(m_bgmHandle);
	DeleteGraph(m_loadingHandle);
}

void CommandSelectScene::Update(Input& input, Input& input2)
{
	//�L�����̃A�j���[�V����
	UpdateCharaAnim();
	//���������Ɏg��
	UpdateBlink();
	//P1�̃Z���N�g
	SelectColorP1(input);
	SelectCommandP1(input);
	//P2�̃Z���N�g
	SelectColorP2(input2);
	SelectCommandP2(input2);
	//2�l����������������Q�[���V�[����
	if (m_isReadyP1 && m_isReadyP2)
	{
		m_isFadeOut = true;
		//�t�F�[�h�C�����Ă���V�[���ړ�
		if (m_fadeManager->IsFinishFadeOut())
		{
			//���̃V�[���ł�肽������
			//�L�����N�^�[�����肵���炻�̃L�����N�^�[��
			//�|�C���^�����̃V�[���ɓn������
			//�I�񂾃R�}���h�Z�̃C���f�b�N�X�����̃V�[���ɓn�����߂ɕۑ�
			m_controller.SaveSelectCommandIndex(m_selectCommandIndexP1, m_selectCommandIndexP2);
			//�F
			m_controller.SaveCharaColorIndexP1(static_cast<CharaColorIndex>(m_currentColorIndexP1));
			m_controller.SaveCharaColorIndexP2(static_cast<CharaColorIndex>(m_currentColorIndexP2));
			//�����ꂽ�玟�̏�Ԃɑ@��
			//���̏�Ԃ͂��̃N���X���o���Ă���
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));
			return;//�Y�ꂸreturn
		}
	}
	else
	{
		if (input.IsTrigger("Start") || input2.IsTrigger("Start"))
		{
			m_isFadeOut = true;
		}
		if (m_fadeManager->IsFinishFadeOut())
		{
			//�^�C�g����ʂɖ߂�
			m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
			return;
		}
	}
#if _DEBUG
	//�f�o�b�O�p
	if (input.IsTrigger("Select"))
	{
		////BGM���~�߂�
		//soundManager.StopBGM();

		for (int i = 0;i < 3;++i)
		{
			//��Ȃ�C���f�b�N�X������
			if (m_selectCommandIndexP1[i] == 0)
			{
				m_selectCommandIndexP1[i] = i + 1;
			}
			//��Ȃ�C���f�b�N�X������
			if (m_selectCommandIndexP2[i] == 0)
			{
				m_selectCommandIndexP2[i] = i + 1;
			}
		}

		//�I�񂾃R�}���h�Z�̃C���f�b�N�X�����̃V�[���ɓn�����߂ɕۑ�
		m_controller.SaveSelectCommandIndex(m_selectCommandIndexP1, m_selectCommandIndexP2);
		//�F
		m_controller.SaveCharaColorIndexP1(static_cast<CharaColorIndex>(m_currentColorIndexP1));
		m_controller.SaveCharaColorIndexP2(static_cast<CharaColorIndex>(m_currentColorIndexP2));
		//�����ꂽ�玟�̏�Ԃɑ@��
		//���̏�Ԃ͂��̃N���X���o���Ă���
		m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));
		return;//�Y�ꂸreturn
	}
#endif
}

void CommandSelectScene::Draw()
{
	//�w�i
	DxLib::DrawGraph(0, 0, m_backGroundHandle, true);

	//�L�����N�^�[�̃A�j���[�V����
	DrawCharacter();

	//Ready
	DrawReady();

	//�Z�̃A�C�R��
	DrawCommandIcon();

	//�J�[�\��
	DrawCursor();

	//�I�񂾃R�}���h�̕\��
	DrawSelectPlayerCommandIcon();
	//"�R�}���h�Z��3�I��ł�������"�̃e�L�X�g
	DrawSelectText();

	//�R�}���h�Z�̐���
	DrawCommandInfo();

	//�������
	DrawGraph(0, Game::kScreenHeight - 50, m_operationHandle, true);

	//�t�F�[�h�C��
	m_fadeManager->DrawWhiteFade(m_isFadeOut);
	if (m_fadeManager->IsFinishFadeOut())
	{
		DrawGraph(0, 0, m_loadingHandle, false);
	}

#if _DEBUG	
	DxLib::DrawString(10, 10, "CharacterselectScene", 0xffffff);
	for (int i = 0;i < 3;++i)
	{
		DrawFormatString(400, 800 + (20 * i), 0xff3333, "selectNumP1 = %d", m_selectCommandIndexP1[i]);
		DrawFormatString(1000, 800 + (20 * i), 0x0000ff, "selectNumP2 = %d", m_selectCommandIndexP2[i]);
		DrawFormatString(400, 750, 0xff3333, "currentColorIndexP1 = %d", m_currentColorIndexP1);
		DrawFormatString(1000, 750, 0x0000ff, "currentColorIndexP2 = %d", m_currentColorIndexP2);
	}
	//DrawFormatString(10, 20, 0xffffff, "������%d", GetASyncLoadNum());
#endif

}
