#include "UI.h"
#include <DxLib.h>
#include "game.h"
#include "GameManager.h"

namespace
{
	//�A�C�R���̈ʒu
	constexpr float kIconPosXP1 = 70;
	constexpr float kIconPosXP2 = Game::kScreenWidth - 100 - 70;
	constexpr float kIconPosY = 30;
	//HP�o�[�̍��W
	constexpr float kHpPosXP1 = (Game::kScreenWidth / 2) - 120.0f;//X���W�̋N�_
	constexpr float kHpPosXP2 = (Game::kScreenWidth / 2) + 120.0f;//X���W�̋N�_
	constexpr float kHpPosY = 60.0;
	constexpr float kHpFrameOffset = 10.0f;
	//HP�o�[�̉����Əc��
	constexpr float kHpWidth = 500.0f;
	constexpr float kHpHeight = 50.0f;
	//HP�o�[�̓_��
	constexpr int kBlinkHpbarNormalFrame = 60;
	constexpr int kBlinkHpbarPinchFrame = 10;
	//�s���`�̊���
	constexpr float kPinchRatio = 3.0f;
	//�v���C���[��HP�̐F
	constexpr unsigned int kHpColorNormal = 0x33ff33;
	constexpr unsigned int kHpColorPinch = 0xffff33;
	//KO
	constexpr float kCenterPosX = Game::kScreenWidth / 2;
	constexpr float kCenterPosY = Game::kScreenHeight / 2;
	//KO��size
	constexpr int kKoWidth = 256;
	constexpr int kKoHeight = 256;
	//KO�̕������o�Ă鎞��
	constexpr int kDisplayFinishRoundFrame = 180;
	//�R�}���h�̐�
	constexpr int kCommandNum = 9;
	//�R�}���h�A�C�R���̑傫��
	constexpr float kCommandIconImageScale = 0.5f;
	constexpr int kCommandIconImageWidth = 200;
	constexpr int kCommandIconImageHight = 200;
	//�_���[�W�\�L���c��t���[��
	constexpr int kDamageDisplayFrame = 60;
	//���U���g�̕\���ʒu
	constexpr int kResultP1WinPosX = kCenterPosX - 500;
	constexpr int kResultP2WinPosX = kCenterPosX + 500;
	//KO�C���N�̏����鑬��
	constexpr int kSpriteAlphaSpeed = 5;
	//���������킩��UI�̈ʒu
	constexpr float kRound1WinP1PosX = (Game::kScreenWidth / 2) - 200;
	constexpr float kRound2WinP1PosX = (Game::kScreenWidth / 2) - 150;
	constexpr float kRound1WinP2PosX = (Game::kScreenWidth / 2) + 160;
	constexpr float kRound2WinP2PosX = (Game::kScreenWidth / 2) + 110;
}

void UI::TimerUI(GameManager& gameManager)
{
	//10�̈ʂ�\��
	int tensPlace = gameManager.GetTimer() / 10;
	SetTimeNumHandle(tensPlace, m_tensPlaceHandle);//10�̈ʂ̐����̉摜���Z�b�g
	
	//1�̈ʂ�\��
	int onesPlace = gameManager.GetTimer() - (tensPlace * 10);
	SetTimeNumHandle(onesPlace, m_onesPlaceHandle);//1�̈ʂ̐����̉摜���Z�b�g
}

void UI::SetTimeNumHandle(int num, int& handle)
{
	switch (num)
	{
	case 0:
		handle = m_timeZero;
		break;
	case 1:
		handle = m_timeOne;
		break;
	case 2:
		handle = m_timeTwo;
		break;
	case 3:
		handle = m_timeThree;
		break;
	case 4:
		handle = m_timeFour;
		break;
	case 5:
		handle = m_timeFive;
		break;
	case 6:
		handle = m_timeSix;
		break;
	case 7:
		handle = m_timeSeven;
		break;
	case 8:
		handle = m_timeEight;
		break;
	case 9:
		handle = m_timeNine;
		break;
	default:
		break;
	}
}

void UI::UpdateHp(float p1Hp, float p2Hp)
{
	//HP�̍X�V
	m_hpbarP1 = p1Hp;
	m_hpbarP2 = p2Hp;
	UpdateDamage();
	//���݂�HP��ۑ�
	m_beforeHpP1 = m_hpbarP1;
	m_beforeHpP2 = m_hpbarP2;
}

void UI::UpdateRoundFinish(GameManager& gameManager)
{
	//�ǂ��炩�����񂾂�KO�Ŏ��Ԑ؂�Ȃ�TimeUp�̕\��
	if (((m_hpbarP1 <= 0 || m_hpbarP2 <= 0) || gameManager.GetTimer() <= 0) && m_displayFinishRoundCountFrame < kDisplayFinishRoundFrame)
	{
		//KO������
		m_roundFinishHandle = m_koHandle;
		if (gameManager.GetTimer() <= 0)
		{
			//�^�C�}�[��0�Ȃ�TimeUp�ƕ\��
			m_roundFinishHandle = m_timeupHandle;
		}
		//���E���h���I��������Ƃ��t���O�ŕ\��
		m_isFinishRound = true;
		//3�b���炢KO�̕�����\������
		m_displayFinishRoundCountFrame++;
		//�C���N�𓮂����Ȃ����߂ɌĂяo��
		InitKoSpriteEffect();
	}
	else
	{
		m_isFinishRound = false;
		//KO�����瓧���ɂ�������
		UpdateKoSpriteFade();
	}
	//�������ق���Winner�̕���
	if (((m_hpbarP1 <= 0 || m_hpbarP2 <= 0) || gameManager.GetTimer() <= 0) && !m_isFinishRound)
	{
		if (m_hpbarP1 == m_hpbarP2)
		{
			//��������
			m_resultUiPos.x = kCenterPosX;
			m_resultHandle = m_drawHandle;
			m_isResult = true;
		}
		else if (m_hpbarP1 > m_hpbarP2)
		{
			//P1win
			m_resultHandle = m_winnerHandle;
			m_resultUiPos.x = kResultP1WinPosX;
			m_isResult = true;
		}
		else if (m_hpbarP1 < m_hpbarP2)
		{
			//P2win
			m_resultHandle = m_winnerHandle;
			m_resultUiPos.x = kResultP2WinPosX;
			m_isResult = true;
		}
	}


	//P1�̏�����
	if (gameManager.GetWinNumP1() > 0)
	{
		//���E���h���P�{����Ă���摜��ύX
		m_winRound1P1Handle = m_winRoundP1Handle;
		if (gameManager.GetWinNumP1() > 1)
		{
			//���E���h���Q�{����Ă���摜��ύX
			m_winRound2P1Handle = m_winRoundP1Handle;
		}
	}
	//P2�̏�����
	if (gameManager.GetWinNumP2() > 0)
	{
		//���E���h���P�{����Ă���摜��ύX
		m_winRound1P2Handle = m_winRoundP2Handle;
		if (gameManager.GetWinNumP2() > 1)
		{
			//���E���h���Q�{����Ă���摜��ύX
			m_winRound2P2Handle = m_winRoundP2Handle;
		}
	}
}

void UI::UpdateKoSpriteFade()
{
	m_spriteAlpha -= kSpriteAlphaSpeed;
	++m_spriteVelo.y;
	if (m_spriteAlpha < 0)
	{
		m_spriteAlpha = 0;
	}
}

void UI::InitKoSpriteEffect()
{
	//�����Ă��C���N�̕\���Ɏg��
	m_spriteAlpha = 255;
	m_spriteVelo.y = 0;
}

void UI::DrawPlayerIcon()
{
	//HP�o�[�̉��ɕ`��
	DrawGraph(kIconPosXP1, kIconPosY, m_p1IconHandle, true);
	DrawGraph(kIconPosXP2, kIconPosY, m_p2IconHandle, true);
}

void UI::UpdateDamage()
{
	//HP���ω�������J�E���g�J�n
	if (m_beforeHpP1 != m_hpbarP1)
	{
		m_damageDisplayCountFrameP1 = kDamageDisplayFrame;
	}
	if (m_beforeHpP2 != m_hpbarP2)
	{
		m_damageDisplayCountFrameP2 = kDamageDisplayFrame;
	}

	//0�ɂȂ�܂ŃJ�E���g�����炷
	//0�ɂȂ����猻�݂�HP�o�[�ɍ��킹��
	if (m_damageDisplayCountFrameP1 <= 0)
	{
		m_damagebarP1 = m_hpbarP1;
		m_damageDisplayCountFrameP1 = 0;
	}
	else
	{
		--m_damageDisplayCountFrameP1;
	}
	if (m_damageDisplayCountFrameP2 <= 0)
	{
		m_damagebarP2 = m_hpbarP2;
		m_damageDisplayCountFrameP2 = 0;
	}
	else
	{
		--m_damageDisplayCountFrameP2;
	}
}

void UI::UpdateBlinkHpbar()
{
	//P1��HP�̓_��
	if (m_blinkHpbarCountFrameP1 >= (m_blinkHpbarIntervalFrameP1 * 2))
	{
		m_blinkHpbarCountFrameP1 = 0;
	}
	++m_blinkHpbarCountFrameP1;
	//P2��HP�̓_��
	if (m_blinkHpbarCountFrameP2 >= (m_blinkHpbarIntervalFrameP2 * 2))
	{
		m_blinkHpbarCountFrameP2 = 0;
	}
	++m_blinkHpbarCountFrameP2;
}

void UI::CheckPinch()
{
	//P1���s���`�Ȃ�
	if (m_hpbarMaxP1 / kPinchRatio >= m_hpbarP1)
	{
		//�_�ő��x��HP�̐F��ς���
		m_blinkHpbarIntervalFrameP1 = kBlinkHpbarPinchFrame;
		m_hpColorP1 = kHpColorPinch;
	}
	//P2���s���`�Ȃ�
	if (m_hpbarMaxP2 / kPinchRatio >= m_hpbarP2)
	{
		//�_�ő��x��HP�̐F��ς���
		m_blinkHpbarIntervalFrameP2 = kBlinkHpbarPinchFrame;
		m_hpColorP2 = kHpColorPinch;
	}
}

void UI::DrawWinNum()
{
	//���������E���h�̐�UI
	//���������\��
	//P1
	DrawGraph(kRound1WinP1PosX, 0, m_winRound1P1Handle, true);
	DrawGraph(kRound2WinP1PosX, 0, m_winRound2P1Handle, true);
	//P2
	DrawGraph(kRound2WinP2PosX, 0, m_winRound2P2Handle, true);
	DrawGraph(kRound1WinP2PosX, 0, m_winRound1P2Handle, true);
}

void UI::DrawHaveCommand()
{
	//�R�}���h
	for (int i = 0; i < 3; ++i)
	{
		DrawRectRotaGraphFast(
			(kCommandIconImageWidth * kCommandIconImageScale) * (i + 1),
			(Game::kScreenHeight - (kCommandIconImageHight * kCommandIconImageScale * kCommandIconImageScale)),
			0, 0, kCommandIconImageWidth, kCommandIconImageHight,
			kCommandIconImageScale, 0.0f, m_selectCommandIconP1Handle[i], true);
		DrawRectRotaGraphFast(
			Game::kScreenWidth - ((kCommandIconImageWidth * kCommandIconImageScale) * (i + 1)),
			(Game::kScreenHeight - (kCommandIconImageHight * kCommandIconImageScale * kCommandIconImageScale)),
			0, 0, kCommandIconImageWidth, kCommandIconImageHight,
			kCommandIconImageScale, 0.0f, m_selectCommandIconP2Handle[i], true);
	}
}

void UI::DrawTimer()
{
	//�^�C�}�[
	//10�̈�
	DrawRectRotaGraphFast(
		Game::kScreenWidth / 2 - 40, 80,
		0, 0, 512, 512,
		0.2f, 0.0f, m_tensPlaceHandle, true, false);
	//1�̈�
	DrawRectRotaGraphFast(
		Game::kScreenWidth / 2 + 40, 80,
		0, 0, 512, 512,
		0.2f, 0.0f, m_onesPlaceHandle, true, false);
}

void UI::DrawHpbar()
{
	////P1��HP�̃o�b�N
	DrawBoxAA(kHpPosXP1, kHpPosY,
		kHpPosXP1 - kHpWidth, kHpPosY + kHpHeight,
		0x777777, true);
	////P2��HP�̃o�b�N
	DrawBoxAA(kHpPosXP2, kHpPosY,
		kHpPosXP2 + kHpWidth, kHpPosY + kHpHeight,
		0x777777, true);

	////P1�̃_���[�W
	DrawBoxAA(kHpPosXP1, kHpPosY,
		kHpPosXP1 - kHpWidth * (m_damagebarP1 / m_hpbarMaxP1), kHpPosY + kHpHeight,
		0xff3333, true);
	////P2�̃_���[�W
	DrawBoxAA(kHpPosXP2, kHpPosY,
		kHpPosXP2 + kHpWidth * (m_damagebarP2 / m_hpbarMaxP2), kHpPosY + kHpHeight,
		0xff3333, true);

	//�_��P1
	if (m_blinkHpbarCountFrameP1 >= m_blinkHpbarIntervalFrameP1)
	{
		SetDrawBright(200, 200, 200);
	}
	////P1��HP
	DrawBoxAA(kHpPosXP1, kHpPosY,
		kHpPosXP1 - kHpWidth * (m_hpbarP1 / m_hpbarMaxP1), kHpPosY + kHpHeight,
		m_hpColorP1, true);
	SetDrawBright(255, 255, 255);//���ɖ߂�
	//�_��P2
	if (m_blinkHpbarCountFrameP2 >= m_blinkHpbarIntervalFrameP2)
	{
		SetDrawBright(200, 200, 200);
	}
	////P2��HP
	DrawBoxAA(kHpPosXP2, kHpPosY,
		kHpPosXP2 + kHpWidth * (m_hpbarP2 / m_hpbarMaxP2), kHpPosY + kHpHeight,
		m_hpColorP2, true);
	SetDrawBright(255, 255, 255);//���ɖ߂�
	//�t���[��1P
	DrawGraph(kHpPosXP1 - kHpWidth - kHpFrameOffset, kHpPosY - kHpFrameOffset, m_hpFrameHandle, true);
	//�t���[��2P
	DrawGraph(kHpPosXP2 - kHpFrameOffset, kHpPosY - kHpFrameOffset, m_hpFrameHandle, true);
}

void UI::DrawFinishUI()
{
	//KO�����Ȃ�
	if ((m_roundFinishHandle == m_koHandle) && m_isFinishRound)
	{
		//���𔒂�����
		DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0xffffff, true);
	}
	//�����������ۂ̕����iKO�Ƃ�TIMEUP�Ƃ�)
	if (m_isFinishRound)
	{
		DrawRectRotaGraphFast(
			kCenterPosX, kCenterPosY,
			0, 0, 512, 512,
			2.0f, 0.0f, m_roundFinishHandle, true, false);
	}
}

void UI::RoundStartUI()
{
	//���E���h���߂̕���
	if (m_startRoundCount >= 120 && m_startRoundCount < 180)
	{
		m_fightScale += 0.02f;
		//�X�v���C�g
		DrawRectRotaGraphFast(
			kCenterPosX - 300, kCenterPosY,
			0, 0, 512, 512,
			2.0f, 0.0f, m_sprite3Handle, true, false);
		DrawRectRotaGraphFast(
			kCenterPosX + 300, kCenterPosY,
			0, 0, 512, 512,
			2.0f, 0.0f, m_sprite2Handle, true, false);
		//FIGHT
		DrawRectRotaGraphFast(
			kCenterPosX, kCenterPosY,
			0, 0, 512, 512,
			m_fightScale, 0.0f, m_fightHandle, true, false);
	}
	else if (m_startRoundCount < 120)
	{
		if (m_startRoundCount > 40)
		{
			//�X�v���C�g
			DrawRectRotaGraphFast(
				kCenterPosX + 400, kCenterPosY,
				0, 0, 512, 512,
				2.0f, 0.0f, m_sprite1Handle, true, false);
		}
		if (m_startRoundCount > 60)
		{
			//�X�v���C�g
			DrawRectRotaGraphFast(
				kCenterPosX + 400, kCenterPosY,
				0, 0, 512, 512,
				1.5f, 0.0f, m_sprite2Handle, true, false);
		}
		//���E���h
		DrawRectRotaGraphFast(
			m_roundUiPos.x, m_roundUiPos.y,
			0, 0, 512, 512,
			m_roundUiScale, 0.0f, m_roundTextHandle, true, false);
		//���E���h����
		DrawRectRotaGraphFast(
			kCenterPosX + 350, kCenterPosY,
			0, 0, 512, 512,
			1.0f, 0.0f, m_roundNumHandle, true, false);
	}
}

void UI::ResultUI()
{
	//��������
	if (m_isResult)
	{
		DrawRectRotaGraphFast(
			m_resultUiPos.x, m_resultUiPos.y,
			0, 0, 512, 512,
			1.0f, 0.0f, m_resultHandle, true, false);
	}
}

void UI::DrawKoSprite()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_spriteAlpha);
	if (m_displayFinishRoundCountFrame > 20)
	{
		//�X�v���C�g
		DrawRectRotaGraphFast(
			50, 0 + m_spriteVelo.y,
			0, 0, 512, 512,
			2.0f, 2.0f, m_sprite1Handle, true, false);
		//�X�v���C�g
		DrawRectRotaGraphFast(
			Game::kScreenWidth - 50, 0 + m_spriteVelo.y,
			0, 0, 512, 512,
			2.0f, 2.0f, m_sprite3Handle, true, false);
		//�X�v���C�g
		DrawRectRotaGraphFast(
			50, Game::kScreenHeight + m_spriteVelo.y,
			0, 0, 512, 512,
			2.0f, 2.0f, m_sprite1Handle, true, false);
		//�X�v���C�g
		DrawRectRotaGraphFast(
			Game::kScreenWidth - 50, Game::kScreenHeight + m_spriteVelo.y,
			0, 0, 512, 512,
			2.0f, 2.0f, m_sprite2Handle, true, false);
	}
	if (m_displayFinishRoundCountFrame > 40)
	{
		//�X�v���C�g
		DrawRectRotaGraphFast(
			0, 0 + m_spriteVelo.y,
			0, 0, 512, 512,
			2.0f, 0.0f, m_sprite2Handle, true, false);
		//�X�v���C�g
		DrawRectRotaGraphFast(
			Game::kScreenWidth, 0 + m_spriteVelo.y,
			0, 0, 512, 512,
			2.0f, 0.0f, m_sprite1Handle, true, false);
		//�X�v���C�g
		DrawRectRotaGraphFast(
			0, Game::kScreenHeight + m_spriteVelo.y,
			0, 0, 512, 512,
			2.0f, 0.0f, m_sprite3Handle, true, false);
		//�X�v���C�g
		DrawRectRotaGraphFast(
			Game::kScreenWidth, Game::kScreenHeight + m_spriteVelo.y,
			0, 0, 512, 512,
			2.0f, 0.0f, m_sprite1Handle, true, false);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

UI::UI(int* selectCommandIndexP1, int* selectCommandIndexP2) :
	m_p1IconHandle(LoadGraph("./img/UI/PlayerIcon/P1Icon.png")),
	m_p2IconHandle(LoadGraph("./img/UI/PlayerIcon/P2Icon.png")),
	m_hpbarP1(0.0f),
	m_hpbarMaxP1(0.0f),
	m_hpbarP2(0.0f),
	m_hpbarMaxP2(0.0f),
	m_damagebarP1(0.0f),
	m_damagebarP2(0.0f),
	m_isFinishRound(false),
	m_isResult(false),
	m_resultUiPos(kCenterPosX, kCenterPosY - 200, 0),
	m_displayFinishRoundCountFrame(0),
	m_startRoundCount(0),
	m_roundNumHandle(-1),
	m_resultHandle(-1),
	m_roundFinishHandle(-1),
	m_fightScale(1.0f),
	m_roundUiPos(kCenterPosX - 220, kCenterPosY + 50, 0),
	m_roundUiScale(2.0f),
	m_spriteAlpha(255),
	m_spriteVelo(),
	m_blinkHpbarCountFrameP1(0),
	m_blinkHpbarCountFrameP2(0),
	m_blinkHpbarIntervalFrameP1(kBlinkHpbarNormalFrame),
	m_blinkHpbarIntervalFrameP2(kBlinkHpbarNormalFrame),
	m_hpColorP1(kHpColorNormal),
	m_hpColorP2(kHpColorNormal),
	m_beforeHpP1(0),
	m_beforeHpP2(0),
	m_commandIconHandle{},
	m_damageDisplayCountFrameP1(0),
	m_damageDisplayCountFrameP2(0),
	m_fightHandle(LoadGraph("./img/UI/RoundText/FIGHT.png")),//FIGHT
	m_winnerHandle(LoadGraph("./img/UI/RoundText/WINNER.png")),//Winner
	m_drawHandle(LoadGraph("./img/UI/RoundText/DRAW.png")),//��������
	m_koHandle(LoadGraph("./img/UI/RoundText/KO.png")),//KO
	m_timeupHandle(LoadGraph("./img/UI/RoundText/TIMEUP.png")),//TimeUp
	//���E���h�R�[��
	m_roundTextHandle(LoadGraph("./img/UI/RoundText/ROUND.png")),
	m_round1Handle(LoadGraph("./img/UI/RoundText/1_ROUND.png")),
	m_round2Handle(LoadGraph("./img/UI/RoundText/2_ROUND.png")),
	m_round3Handle(LoadGraph("./img/UI/RoundText/3_ROUND.png")),
	m_roundOverHandle(LoadGraph("./img/UI/RoundText/OVERTIME.png")),
	//�^�C�}�[�̐���
	m_timeZero(LoadGraph("./img/UI/Timer/0_Timer.png")),	//0
	m_timeOne(LoadGraph("./img/UI/Timer/1_Timer.png")),	//1
	m_timeTwo(LoadGraph("./img/UI/Timer/2_Timer.png")),	//2
	m_timeThree(LoadGraph("./img/UI/Timer/3_Timer.png")),	//3
	m_timeFour(LoadGraph("./img/UI/Timer/4_Timer.png")),	//4
	m_timeFive(LoadGraph("./img/UI/Timer/5_Timer.png")),	//5
	m_timeSix(LoadGraph("./img/UI/Timer/6_Timer.png")),	//6
	m_timeSeven(LoadGraph("./img/UI/Timer/7_Timer.png")),	//7
	m_timeEight(LoadGraph("./img/UI/Timer/8_Timer.png")),	//8
	m_timeNine(LoadGraph("./img/UI/Timer/9_Timer.png")),	//9
	//HP
	m_hpFrameHandle(LoadGraph("./img/UI/Always/HpFrame.png")),	//�t���[��
	//������
	m_winRoundP1Handle(LoadGraph("./img/UI/Always/1PWinNumIcon.png")),
	m_winRoundP2Handle(LoadGraph("./img/UI/Always/2PWinNumIcon.png")),
	m_onesPlaceHandle(0),
	m_tensPlaceHandle(0),
	//�X�v���C�g
	m_sprite1Handle(LoadGraph("./img/UI/splash/splash.png")),
	m_sprite2Handle(LoadGraph("./img/UI/splash/splash2.png")),
	m_sprite3Handle(LoadGraph("./img/UI/splash/splash3.png"))
{
	int noWinRoundHandle = LoadGraph("./img/UI/Always/WinNullIcon.png");
	m_winRound1P1Handle = noWinRoundHandle;
	m_winRound1P2Handle = noWinRoundHandle;
	m_winRound2P1Handle = noWinRoundHandle;
	m_winRound2P2Handle = noWinRoundHandle;

	//�I�񂾃R�}���h��\�����鏀��
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
	//���ꂼ��̑I�񂾃R�}���h�̉摜���擾
	for (int i = 0; i < 3; ++i)
	{
		//1P
		m_selectCommandIconP1Handle[i] = m_commandIconHandle[selectCommandIndexP1[i] - 1];//�Z���N�g�ԍ���1~9�Ȃ̂Ŕz��ɍ��킹��-1����
		//2P
		m_selectCommandIconP2Handle[i] = m_commandIconHandle[selectCommandIndexP2[i] - 1];
	}
}

UI::~UI()
{
	
	DeleteGraph(m_fightHandle);//FIGHT
	DeleteGraph(m_winnerHandle);//Winner
	DeleteGraph(m_drawHandle);//��������
	DeleteGraph(m_koHandle);//KO
	DeleteGraph(m_timeupHandle);//TimeUp
	//���E���h�R�[��
	DeleteGraph(m_roundTextHandle);
	DeleteGraph(m_round1Handle);
	DeleteGraph(m_round2Handle);
	DeleteGraph(m_round3Handle);
	DeleteGraph(m_roundOverHandle);
	//�^�C�}�[�̐���
	DeleteGraph(m_timeZero);//0
	DeleteGraph(m_timeOne);	//1
	DeleteGraph(m_timeTwo);	//2
	DeleteGraph(m_timeThree);//3
	DeleteGraph(m_timeFour);//4
	DeleteGraph(m_timeFive);//5
	DeleteGraph(m_timeSix);//6
	DeleteGraph(m_timeSeven);//7
	DeleteGraph(m_timeEight);//8
	DeleteGraph(m_timeNine);//9
	//HP
	DeleteGraph(m_hpFrameHandle);//�t���[��
	//������
	DeleteGraph(m_winRoundP1Handle);
	DeleteGraph(m_winRoundP2Handle);
	DeleteGraph(m_winRound1P1Handle);
	DeleteGraph(m_winRound1P2Handle);
	DeleteGraph(m_winRound2P1Handle);
	DeleteGraph(m_winRound2P2Handle);

	//�I�񂾃R�}���h
	for (int i = 0; i < kCommandNum; ++i)
	{
		DeleteGraph(m_commandIconHandle[i]);
	}
	//���ꂼ��̑I�񂾃R�}���h�̉摜���擾
	for (int i = 0; i < 3; ++i)
	{
		//1P
		DeleteGraph(m_selectCommandIconP1Handle[i]);//�Z���N�g�ԍ���1~9�Ȃ̂Ŕz��ɍ��킹��-1����
		//2P
		DeleteGraph(m_selectCommandIconP2Handle[i]);
	}

	//�X�v���C�g
	DeleteGraph(m_sprite1Handle);
	DeleteGraph(m_sprite2Handle);
	DeleteGraph(m_sprite3Handle);
}

void UI::Init(float p1Hp, float p2Hp, GameManager& gameManager)
{
	//���Z�b�g
	m_isFinishRound = false;
	m_isResult = false;
	m_hpbarMaxP1 = p1Hp;
	m_hpbarMaxP2 = p2Hp;
	m_hpbarP1 = p1Hp;
	m_hpbarP2 = p2Hp;
	m_displayFinishRoundCountFrame = 0;
	m_fightScale = 1.0f;
	m_beforeHpP1 = m_hpbarP1;
	m_beforeHpP2 = m_hpbarP2;
	m_damagebarP1 = m_hpbarP1;
	m_damagebarP2 = m_hpbarP2;
	m_damageDisplayCountFrameP1 = 0;
	m_damageDisplayCountFrameP2 = 0;
	m_blinkHpbarCountFrameP1 = 0;
	m_blinkHpbarCountFrameP2 = 0;
	m_blinkHpbarIntervalFrameP1 = kBlinkHpbarNormalFrame;
	m_blinkHpbarIntervalFrameP2 = kBlinkHpbarNormalFrame;
	m_hpColorP1 = kHpColorNormal;
	m_hpColorP2 = kHpColorNormal;
	//���E���h����
	if (gameManager.GetRoundNumber() == 1)
	{
		m_roundNumHandle = m_round1Handle;
	}
	else if (gameManager.GetRoundNumber() == 2)
	{
		m_roundNumHandle = m_round2Handle;
	}
	else if (gameManager.GetRoundNumber() == 3)
	{
		m_roundNumHandle = m_round3Handle;
	}
	else//�I�[�o�[�^�C��
	{
		m_roundUiPos.x = kCenterPosX;
		m_roundUiScale = 3.0f;
		m_roundTextHandle = m_roundOverHandle;
		m_roundNumHandle = -1;
	}
	//����
	TimerUI(gameManager);
	//�C���N�̕\���̏�����
	InitKoSpriteEffect();
}


void UI::Update(float p1Hp, float p2Hp, GameManager& gameManager)
{
	//�^�C�}�[
	TimerUI(gameManager);
	//�Q�[���J�n���̉��o�̂��߂̃J�E���g
	m_startRoundCount = gameManager.GetStartRoundCount();
	//Hp
	UpdateHp(p1Hp, p2Hp);
	//�̗͂��s���`���ǂ���
	CheckPinch();
	//Hp�o�[�̓_��
	UpdateBlinkHpbar();
	//���E���h�I�����̏���
	UpdateRoundFinish(gameManager);
}

//�O�ɕ`��
void UI::DrawFront()
{
	//�����҂܂��͈��������̕\��
	ResultUI();
	//���E���h�R�[��
	RoundStartUI();
	//KO�������̃C���N
	DrawKoSprite();
}
//���ɕ`��
void UI::DrawBack()
{
	//�^�C�}�[��0�ɂȂ����ۂ̏���
	DrawFinishUI();
	//Hp�o�[
	DrawHpbar();
	//�v���C���[�A�C�R��
	DrawPlayerIcon();
	//�^�C�}�[
	DrawTimer();
	//�������Ă���R�}���h�Z�̕\��
	DrawHaveCommand();
	//���������E���h�̐�
	DrawWinNum();
}
