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
	//�n��̍���
	constexpr int kGroundHeight = Game::kScreenHeight + 400;

	//HP
	constexpr float kHp = 100.0f;

	//�d��
	constexpr float kGravityPower = 3.2f;

	//�v���C���[�̉摜�̑傫��
	constexpr int kCharaWidth = 512;
	constexpr int kCharaHeight = 512;
	//�摜�̔{��
	constexpr float kCharaScale = 1.0f;

	//�ړ����x
	constexpr float kMoveSpeedFront = 12.0f;
	constexpr float kMoveSpeedBack = 9.0f;

	//�W�����v��
	constexpr float kJumpPowerX = kMoveSpeedFront;
	constexpr float kJumpPowerY = -60.0f;
	//�W�����v�ڍs�t���[��
	constexpr int kJumpTransFrame = 5;

	//������
	constexpr float kPushPower = 3.0f;

	//�v���C���[�̉�����������̃x�[�X(�E�����̎�)
	constexpr int kBaseCollisionWidthFront = 80;
	constexpr int kBaseCollisionWidthBack = 80;
	constexpr int kBaseCollisionWidthTop = 60;
	constexpr int kBaseCollisionWidthBottom = 200;

	//�_�E�������Ƃ��̉摜�̈ʒu��␳
	constexpr int kDownDrawOffset = kCharaHeight/2 - 90;
	constexpr int kDownFrame = 30;//�_�E�����Ă鎞��

	//�����̃_���[�W
	constexpr float kThrowDamage = 12.0f;
	//�����Ŕ�ԋ���
	constexpr float kThrowVeloX = 15.0f;
	constexpr float kThrowVeloY = -40.0f;
	//��듊���̍ۂɏ��������
	constexpr float kBackThrowShiftVelo = 1.0f;

	//��듊���������ۂ̋�����O�����Ɠ������炢�ɂ��邽�߂�1.5�{����
	constexpr float kThrowBackMargin = 1.5f;

	//�����ʂ����ł���t���[��
	constexpr int kCanThrowEscapeFrame = 10;
	//���������������̍d���t���[��
	constexpr int kNoActThrowEscapeFrame = 20;


	//�L�����Z���\�P�\
	constexpr int kCanselGraceFrame = 20;
	//���߂Ƀ{�^���������Ă��܂��Ă��R�}���h�����\�P�\
	constexpr int kInputDelayFrame = 4;

	//SE�̉���
	constexpr int kSeVolume = 150;

	//�����|�[�Y������܂łɂ����鎞��
	constexpr int kChangeWinPoseFrame = 120;

	//�e��Y���W
	constexpr int kShadowPosY = 1510;
	//�e�̔��a
	constexpr float kShadowRadiusX = 120.0f;
	constexpr float kShadowRadiusY = 30.0f;
	//�X�e�[�W�̑傫��
	constexpr int kStageWidth = 2600;

	//���͂̎c��t���[��
	constexpr int kRemainInputFrame = 7;
}

//�v���C���[�̔ԍ��ƑI�񂾃L������n��
Player::Player(PlayerIndex playerIndex, int* selectCommandIndex,CharaColorIndex charaColorIndex,bool isSameColor):
	m_playerIndex(playerIndex),//P1��P2
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
	m_downSeHandle(LoadSoundMem("./SE/PlayerBase/Down.mp3")),//�|��鉹
	m_jumpSeHandle(LoadSoundMem("./SE/PlayerBase/Jumped.mp3")),//�W�����v�̉�
	m_jumpedSeHandle(LoadSoundMem("./SE/PlayerBase/Jumped.mp3")),//���n�����Ƃ��̉�
	m_graspSeHandle(LoadSoundMem("./SE/PlayerBase/Grasp.mp3")),//���މ�
	m_throwSeHandle(LoadSoundMem("./SE/PlayerBase/Throw.mp3")),//������
	m_loseSeHandle(LoadSoundMem("./SE/PlayerBase/voice/Lose.mp3")),//����
	m_winSeHandle(LoadSoundMem("./SE/PlayerBase/voice/Win.mp3"))//����
{
	for (int i = 0;i < 3;++i)
	{
		m_selectCommandIndex[i] = selectCommandIndex[i];
	}
	
	m_chara = std::make_shared<Chara>(m_selectCommandIndex, charaColorIndex);//�L�����N�^�[���m��
	m_se = std::make_shared<SE>();//SE���m��
	m_chara->GetAnimIdleStand(*this);//�ҋ@���[�V����
	assert(m_handle != -1);
	m_chara->GetHitBoxIdleStand(*this);//�ҋ@���[�V�����̓����蔻����擾
	//������������̏�����
	InitPushBox();
}

Player::~Player()
{
}

//���E���h�؂�ւ��̍ۂ̏������ɂ��g��
void Player::Init(float X,bool isLeft)
{
	m_se->Stop();
	//���̓��Z�b�g
	m_lightPunchBottun.Init(false, 0);
	m_highPunchBottun.Init(false, 0);
	m_lightKickBottun.Init(false, 0);
	m_highKickBottun.Init(false, 0);
	m_graspBottun.Init(false, 0);
	//�����ʒu
	Vector3 firstPos(X, kGroundHeight, 0);
	m_pos = firstPos;
	//���x�̏�����
	m_velocity.x = 0;
	m_velocity.y = 0;
	m_giveThrowVelo.x = 0;
	m_giveThrowVelo.y = 0;
	m_knockback.x = 0;
	m_knockback.y = 0;
	//������������
	InitPushBox();
	//HP�̏�����
	m_hp = kHp;
	//��Ԃ̉�
	m_isLeft = isLeft;
	m_isGround = true;
	m_isHitAttack = false;
	m_isHitGrasp = false;
	m_isSquat = false;
	m_isThrown = false;
	m_isThrowSuccess = false;
	//�A�j���[�V�����֘A�̏�����
	m_animCountFrame = 0;
	m_animIndex = 0;
	m_animNum = 0;
	m_oneAnimIntervalFrame = 0;
	//�U���̃X�e�[�^�X�̏�����
	m_startAttackFrame = 0;
	m_finishAttackFrame = 0;
	m_giveDamage = 0;
	ResetAttackBox();
	ResetGraspBox();
	m_attackAttributes = AttackAttributes::Null;
	m_attackType = AttackTypes::Null;
	//�d������
	m_noActFrame = 0;
	m_giveNoActFrame = 0;
	m_guardFrame = 0;
	m_giveGuardFrame = 0;
	//�ҋ@��ԃX�^�[�g
	m_chara->GetAnimIdleStand(*this);//�ҋ@���[�V����
	assert(m_handle != -1);
	m_chara->GetHitBoxIdleStand(*this);//�ҋ@���[�V�����̓����蔻����擾
	//������������̏�����
	InitPushBox();
	//�ҋ@���
	m_update = &Player::IdleStandUpdate;
	m_draw = &Player::IdleStandDraw;
}

void Player::Update(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�n�ʂɂ��Ă邩�`�F�b�N
	CheckGround();
	//�d��
	Gravity();
	//���Ԑ؂ꂵ���ꍇ�̓_���[�W��^���Ȃ�����
	if (gameManager.GetTimer() <= 0)
	{
		m_giveDamage = 0;
	}
	//�̗͂�����Ȃ�s���ł���
	if (!CheckDead())
	{
		PlayerInput(input);
		(this->*m_update)(input, enemy, myBullet, gameManager);
	}
}

void Player::Draw(const Camera& camera)
{
	//�e
	DrawShadow(camera);
	(this->*m_draw)(camera);
	//2�v���C���[�̐F�������Â�
	if ((m_playerIndex == PlayerIndex::Player2) && m_isSameColor)
	{
		SetDrawBlendMode(DX_BLENDMODE_SUB, 100);
		(this->*m_draw)(camera);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
#if _DEBUG
	//�����蔻��\��
	DrawHitBox(camera);
	//�t���[��
	if (m_playerIndex == PlayerIndex::Player1)
	{
		DrawFormatString(0, 400, 0xff4422, "�W�����v�̑S�̃t���[�� = %d", debugJumpFrame);
		DrawFormatString(0, 450, 0xff4422, "�R�}���h�̑S�̃t���[�� = %d", debugCommandFrame);
		DrawFormatString(0, 500, 0xff4422, "�v���C���[�̈ʒu(x=%2.0f,y=%2.0f)", m_pos.x, m_pos.y);
		for (int i = 0;i < 3;++i)
		{
			DrawFormatString(400, 50 + (20 * i), 0xff3333, "selectNumP1 = %d", m_selectCommandIndex[i]);
		}
	}
#endif
}

//�f�X�g���N�^�̌Ă΂��^�C�~���O�����Ⴂ���Ă��̂ł���ŔC�ӂ̃^�C�~���O�ŉ�����܂��B
void Player::End()
{
	DeleteGraph(m_lightPunchSeHandle);
	DeleteGraph(m_lightKickSeHandle);
	DeleteGraph(m_highPunchSeHandle);
	DeleteGraph(m_highKickSeHandle);
	DeleteGraph(m_downSeHandle);//�|��鉹
	DeleteGraph(m_jumpSeHandle);//�W�����v�̉�
	DeleteGraph(m_jumpedSeHandle);//���n�����Ƃ��̉�
	DeleteGraph(m_graspSeHandle);//���މ�
	DeleteGraph(m_throwSeHandle);//������
	DeleteGraph(m_loseSeHandle);//����
	DeleteGraph(m_winSeHandle);//����
}

void Player::JumpSE()
{
	//�W�����v�̉�
	m_se->SetSE(m_jumpSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::JumpedSE()
{
	//���n��̉�
	m_se->SetSE(m_jumpedSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::AttackSE()
{
	if (m_attackType == AttackTypes::LightPunch)
	{
		//��p��
		m_se->SetSE(m_lightPunchSeHandle);
	}
	else if (m_attackType == AttackTypes::HighPunch)
	{
		//���p��
		m_se->SetSE(m_highPunchSeHandle);
	}
	else if (m_attackType == AttackTypes::LightKick)
	{
		//��L�b�N
		m_se->SetSE(m_lightKickSeHandle);
	}
	else if (m_attackType == AttackTypes::HighKick)
	{
		//���L�b�N
		m_se->SetSE(m_highKickSeHandle);
	}
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::DownSE()
{
	//�_�E�������Ƃ��̉�
	m_se->SetSE(m_downSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::GraspSE()
{
	//���݂ɐ���������
	m_se->SetSE(m_graspSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::ThrowSE()
{
	//�����鉹
	m_se->SetSE(m_throwSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::LoseSE()
{
	//���[��
	m_se->SetSE(m_loseSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

void Player::WinSE()
{
	//������I
	m_se->SetSE(m_winSeHandle);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}


//�e
void Player::DrawShadow(const Camera& camera)
{
	//�e
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	//�ȉ~
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
	//�w�肵���P�\�t���[���̊Ԃ������������Ă����͂��c��
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
	//�n�ʂ̍���
	if (m_pos.y >= kGroundHeight)
	{
		//�ʒu��n�ʂ̍�����
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
	//�n��ɂ���Ƃ��͏d�͂������Ȃ�
	if (m_isGround)
	{
		m_velocity.y = 0;
	}
	//�󒆂ɂ���Ƃ��͏d�͂�������
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
	//�U���̔��肪�Ȃ��Ȃ瓖����Ȃ�
	if (m_hitBoxAttack.x1 == 0 &&
		m_hitBoxAttack.y1 == 0 &&
		m_hitBoxAttack.x2 == 0 &&
		m_hitBoxAttack.y2 == 0)	return false;

	float width;
	float height;
	bool isHit = false;
	if (!m_isLeft)
	{
		//�U������̒��S�����߂�
		Vector3 attackPos((m_pos.x + m_hitBoxAttack.x2 + m_pos.x + m_hitBoxAttack.x1) / 2,
			(m_pos.y + m_hitBoxAttack.y2 + m_pos.y + m_hitBoxAttack.y1) / 2, 0.0f);

		//�����蔻�肪����Ȃ�`�F�b�N
		//��
		if (enemy.m_hitBoxHead.x1 != 0 ||
			enemy.m_hitBoxHead.y1 != 0 ||
			enemy.m_hitBoxHead.x2 != 0 ||
			enemy.m_hitBoxHead.y2 != 0)
		{
			//�G�̓��̂��ꔻ��////////////////////////////////////////
			Vector3 enemyHead((enemy.m_pos.x - enemy.m_hitBoxHead.x2 + enemy.m_pos.x - enemy.m_hitBoxHead.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxHead.y2 + enemy.m_pos.y + enemy.m_hitBoxHead.y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyHead.x);
			height = std::abs(attackPos.y - enemyHead.y);

			//X�����̍U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.m_hitBoxHead.x2 - enemy.m_hitBoxHead.x1) / 2) &&
				//Y�����U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.m_hitBoxHead.y2 - enemy.m_hitBoxHead.y1) / 2)) return true;
			/////////////////////////////////////////////////////////////
		}
		

		//��
		if (enemy.m_hitBoxBody.x1 != 0 ||
			enemy.m_hitBoxBody.y1 != 0 ||
			enemy.m_hitBoxBody.x2 != 0 ||
			enemy.m_hitBoxBody.y2 != 0)
		{
			//�G�̑̂̂��ꔻ��////////////////////////////////////////////
			Vector3 enemyBody((enemy.m_pos.x - enemy.m_hitBoxBody.x2 + enemy.m_pos.x - enemy.m_hitBoxBody.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxBody.y2 + enemy.m_pos.y + enemy.m_hitBoxBody.y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyBody.x);
			height = std::abs(attackPos.y - enemyBody.y);

			//X�����̍U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.m_hitBoxBody.x2 - enemy.m_hitBoxBody.x1) / 2) &&
				//Y�����U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.m_hitBoxBody.y2 - enemy.m_hitBoxBody.y1) / 2)) return true;
			///////////////////////////////////////////////////////////////
		}
		
		//��
		if (enemy.m_hitBoxLeg.x1 != 0 ||
			enemy.m_hitBoxLeg.y1 != 0 ||
			enemy.m_hitBoxLeg.x2 != 0 ||
			enemy.m_hitBoxLeg.y2 != 0)
		{
			//�G�̑��̂��ꔻ��
			Vector3 enemyLeg((enemy.m_pos.x - enemy.m_hitBoxLeg.x2 + enemy.m_pos.x - enemy.m_hitBoxLeg.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxLeg.y2 + enemy.m_pos.y + enemy.m_hitBoxLeg.y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyLeg.x);
			height = std::abs(attackPos.y - enemyLeg.y);

			//X�����̍U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.m_hitBoxLeg.x2 - enemy.m_hitBoxLeg.x1) / 2) &&
				//Y�����U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.m_hitBoxLeg.y2 - enemy.m_hitBoxLeg.y1) / 2)) return true;
		}
		
	}
	else
	{
		//�U������̒��S�����߂�
		Vector3 attackPos((m_pos.x - m_hitBoxAttack.x2 + m_pos.x - m_hitBoxAttack.x1) / 2,
			(m_pos.y + m_hitBoxAttack.y2 + m_pos.y + m_hitBoxAttack.y1) / 2, 0.0f);

		//�����蔻�肪����Ȃ�`�F�b�N
		//��
		if (enemy.m_hitBoxHead.x1 != 0 ||
			enemy.m_hitBoxHead.y1 != 0 ||
			enemy.m_hitBoxHead.x2 != 0 ||
			enemy.m_hitBoxHead.y2 != 0)
		{
			//�G�̓��̂��ꔻ��////////////////////////////////////////
			Vector3 enemyHead((enemy.m_pos.x + enemy.m_hitBoxHead.x2 + enemy.m_pos.x + enemy.m_hitBoxHead.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxHead.y2 + enemy.m_pos.y + enemy.m_hitBoxHead.y1) / 2, 0.0f);

			width = std::abs(attackPos.x - enemyHead.x);
			height = std::abs(attackPos.y - enemyHead.y);

			//X�����̍U���̕��Ɠ��̕��𑫂����l��菬�����Ȃ瓖�����Ă�
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.m_hitBoxHead.x2 - enemy.m_hitBoxHead.x1) / 2) &&
				//X�����̍U���̕��Ɠ��̕��𑫂����l��菬�����Ȃ瓖�����Ă�
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.m_hitBoxHead.y2 - enemy.m_hitBoxHead.y1) / 2)) return true;
			////////////////////////////////////////////////////////////
		}
	

		//��
		if (enemy.m_hitBoxBody.x1 != 0 ||
			enemy.m_hitBoxBody.y1 != 0 ||
			enemy.m_hitBoxBody.x2 != 0 ||
			enemy.m_hitBoxBody.y2 != 0)
		{
			//�G�̓��̂��ꔻ��///////////////////////////////////////
			Vector3 enemyBody((enemy.m_pos.x + enemy.m_hitBoxBody.x2 + enemy.m_pos.x + enemy.m_hitBoxBody.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxBody.y2 + enemy.m_pos.y + enemy.m_hitBoxBody.y1) / 2, 0.0f);

			width = std::abs(attackPos.x - enemyBody.x);
			height = std::abs(attackPos.y - enemyBody.y);

			//X�����̍U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.m_hitBoxBody.x2 - enemy.m_hitBoxBody.x1) / 2) &&
				//Y�����U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.m_hitBoxBody.y2 - enemy.m_hitBoxBody.y1) / 2)) return true;
			/////////////////////////////////////////////////////////
		}
		
		//��
		if (enemy.m_hitBoxLeg.x1 != 0 ||
			enemy.m_hitBoxLeg.y1 != 0 ||
			enemy.m_hitBoxLeg.x2 != 0 ||
			enemy.m_hitBoxLeg.y2 != 0)
		{
			//�G�̑��̂��ꔻ��
			Vector3 enemyLeg((enemy.m_pos.x + enemy.m_hitBoxLeg.x2 + enemy.m_pos.x + enemy.m_hitBoxLeg.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxLeg.y2 + enemy.m_pos.y + enemy.m_hitBoxLeg.y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyLeg.x);
			height = std::abs(attackPos.y - enemyLeg.y);

			//X�����̍U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.m_hitBoxLeg.x2 - enemy.m_hitBoxLeg.x1) / 2) &&
				//Y�����U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.m_hitBoxLeg.y2 - enemy.m_hitBoxLeg.y1) / 2)) return true;
		}
	}


	//�����܂ŗ����瓖�����ĂȂ�
	return false;
}

bool Player::CheckHitGrasp(Player& enemy)
{
	//�U���̔��肪�Ȃ��Ȃ瓖����Ȃ�
	if (m_hitBoxGrasp.x1 == 0 &&
		m_hitBoxGrasp.y1 == 0 &&
		m_hitBoxGrasp.x2 == 0 &&
		m_hitBoxGrasp.y2 == 0)	return false;

	float width;
	float height;
	bool isHit = false;

	
	if (!m_isLeft)
	{
		//�U������̒��S�����߂�
		Vector3 graspPos((m_pos.x + m_hitBoxGrasp.x2 + m_pos.x + m_hitBoxGrasp.x1) / 2,
			(m_pos.y + m_hitBoxGrasp.y2 + m_pos.y + m_hitBoxGrasp.y1) / 2, 0.0f);
		//�������ꔻ��
		if (enemy.m_hitBoxThrow.x1 != 0 ||
			enemy.m_hitBoxThrow.y1 != 0 ||
			enemy.m_hitBoxThrow.x2 != 0 ||
			enemy.m_hitBoxThrow.y2 != 0)
		{
			//�G�̓����̂��ꔻ��
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
		//�U������̒��S�����߂�
		Vector3 graspPos((m_pos.x - m_hitBoxGrasp.x2 + m_pos.x - m_hitBoxGrasp.x1) / 2,
			(m_pos.y + m_hitBoxGrasp.y2 + m_pos.y + m_hitBoxGrasp.y1) / 2, 0.0f);
		//�������ꔻ��
		if (enemy.m_hitBoxThrow.x1 != 0 ||
			enemy.m_hitBoxThrow.y1 != 0 ||
			enemy.m_hitBoxThrow.x2 != 0 ||
			enemy.m_hitBoxThrow.y2 != 0)
		{
			//�G�̓����̂��ꔻ��
			Vector3 enemyThrow((enemy.m_pos.x + enemy.m_hitBoxThrow.x2 + enemy.m_pos.x + enemy.m_hitBoxThrow.x1) / 2,
				(enemy.m_pos.y + enemy.m_hitBoxThrow.y2 + enemy.m_pos.y + enemy.m_hitBoxThrow.y1) / 2, 0.0f);
			width = std::abs(graspPos.x - enemyThrow.x);
			height = std::abs(graspPos.y - enemyThrow.y);


			if (width < std::abs((m_hitBoxGrasp.x2 - m_hitBoxGrasp.x1) / 2) + std::abs((enemy.m_hitBoxThrow.x2 - enemy.m_hitBoxThrow.x1) / 2) &&
				height < std::abs((m_hitBoxGrasp.y2 - m_hitBoxGrasp.y1) / 2) + std::abs((enemy.m_hitBoxThrow.y2 - enemy.m_hitBoxThrow.y1) / 2)) return true;
		}
	}

	//�����܂ŗ����瓖�����ĂȂ�
	return false;
}

#if _DEBUG
//�����蔻�������
void Player::DrawHitBox(const Camera& camera)
{

	//�����蔻��
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	if (!m_isLeft)
	{
		//������������
		DxLib::DrawBox(
			static_cast<int>(m_pos.x - m_pushBox.x1 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y - m_pushBox.y1 + camera.m_drawOffset.y),
			static_cast<int>(m_pos.x + m_pushBox.x2 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_pushBox.y2 + camera.m_drawOffset.y),
			0x00ff00, true);

		//��
		DxLib::DrawBox(
			static_cast<int>(m_pos.x + m_hitBoxHead.x1 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxHead.y1 + camera.m_drawOffset.y),
			static_cast<int>(m_pos.x + m_hitBoxHead.x2 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxHead.y2 + camera.m_drawOffset.y),
			0x000fff, true);
		//����
		DxLib::DrawBox(
			static_cast<int>(m_pos.x + m_hitBoxBody.x1 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxBody.y1 + camera.m_drawOffset.y),
			static_cast<int>(m_pos.x + m_hitBoxBody.x2 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxBody.y2 + camera.m_drawOffset.y),
			0x0000ff, true);
		//��
		DxLib::DrawBox(
			static_cast<int>(m_pos.x + m_hitBoxLeg.x1 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxLeg.y1 + camera.m_drawOffset.y),
			static_cast<int>(m_pos.x + m_hitBoxLeg.x2 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxLeg.y2 + camera.m_drawOffset.y),
			0x0000ff, true);
		//�������ݔ��� 60x150
		DxLib::DrawBox(
			static_cast<int>(m_pos.x + m_hitBoxThrow.x1 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxThrow.y1 + camera.m_drawOffset.y),
			static_cast<int>(m_pos.x + m_hitBoxThrow.x2 + camera.m_drawOffset.x),
			static_cast<int>(m_pos.y + m_hitBoxThrow.y2 + camera.m_drawOffset.y),
			0xffffff, true);
		if (m_isCheckAttackBox)
		{
			//�U������
			DxLib::DrawBox(
				static_cast<int>(m_pos.x + m_hitBoxAttack.x1 + camera.m_drawOffset.x),
				static_cast<int>(m_pos.y + m_hitBoxAttack.y1 + camera.m_drawOffset.y),
				static_cast<int>(m_pos.x + m_hitBoxAttack.x2 + camera.m_drawOffset.x),
				static_cast<int>(m_pos.y + m_hitBoxAttack.y2 + camera.m_drawOffset.y),
				0xff0000, true);
			//�U������
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
		//������������
		DxLib::DrawBox(
			(static_cast<int>(m_pos.x) - m_pushBox.x1) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) - m_pushBox.y1) + camera.m_drawOffset.y,
			(static_cast<int>(m_pos.x) + m_pushBox.x2) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_pushBox.y2) + camera.m_drawOffset.y,
			0x00ff00, true);

		//��
		DxLib::DrawBox(
			(static_cast<int>(m_pos.x) - m_hitBoxHead.x1) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxHead.y1) + camera.m_drawOffset.y,
			(static_cast<int>(m_pos.x) - m_hitBoxHead.x2) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxHead.y2) + camera.m_drawOffset.y,
			0x000fff, true);
		//����
		DxLib::DrawBox(
			(static_cast<int>(m_pos.x) - m_hitBoxBody.x1) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxBody.y1) + camera.m_drawOffset.y,
			(static_cast<int>(m_pos.x) - m_hitBoxBody.x2) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxBody.y2) + camera.m_drawOffset.y,
			0x0000ff, true);
		//��
		DxLib::DrawBox(
			(static_cast<int>(m_pos.x) - m_hitBoxLeg.x1) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxLeg.y1) + camera.m_drawOffset.y,
			(static_cast<int>(m_pos.x) - m_hitBoxLeg.x2) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxLeg.y2) + camera.m_drawOffset.y,
			0x0000ff, true);
		//�������ݔ���
		DxLib::DrawBox(
			(static_cast<int>(m_pos.x) - m_hitBoxThrow.x1) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxThrow.y1) + camera.m_drawOffset.y,
			(static_cast<int>(m_pos.x) - m_hitBoxThrow.x2) + camera.m_drawOffset.x,
			(static_cast<int>(m_pos.y) + m_hitBoxThrow.y2) + camera.m_drawOffset.y,
			0xffffff, true);
		if (m_isCheckAttackBox)
		{
			//�U������
			DxLib::DrawBox(
				(static_cast<int>(m_pos.x) - m_hitBoxAttack.x1) + camera.m_drawOffset.x,
				(static_cast<int>(m_pos.y) + m_hitBoxAttack.y1) + camera.m_drawOffset.y,
				(static_cast<int>(m_pos.x) - m_hitBoxAttack.x2) + camera.m_drawOffset.x,
				(static_cast<int>(m_pos.y) + m_hitBoxAttack.y2) + camera.m_drawOffset.y,
				0xff0000, true);

			//�U������
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

//�������炢
void Player::LoadStateHit()
{
	//���ꃂ�[�V����
	m_chara->GetAnimHitStand(*this);
	assert(m_handle != -1);
	//��炢����
	m_chara->GetHitBoxHitStand(*this);
	//��炢
	m_update = &Player::DamageUpdate;
	m_draw = &Player::DamageDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
}
//�������
void Player::LoadStateBeThrown()
{
	//���ꃂ�[�V����
	m_chara->GetAnimHitStand(*this);
	assert(m_handle != -1);
	//������ꒆ�͓����蔻�������
	m_chara->GetHitBoxBeThrown(*this);
	ResetPushBox();
	//��炢
	m_update = &Player::BeThrownUpdate;
	m_draw = &Player::BeThrownDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
}

void Player::LoadStateThrowEscape()
{
	//���ꃂ�[�V����
	m_chara->GetAnimHitStand(*this);
	assert(m_handle != -1);
	//��������
	m_update = &Player::ThrowEscapeUpdate;
	m_draw = &Player::ThrowEscapeDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
	m_isThrowSuccess = false;
}

//�����K�[�h
void Player::LoadStateGuardStand()
{
	//�K�[�h���[�V����
	m_chara->GetAnimGuardStand(*this);
	assert(m_handle != -1);
	//��炢����
	m_chara->GetHitBoxGuardStand(*this);
	//�K�[�h
	m_update = &Player::GuardStandUpdate;
	m_draw = &Player::GuardStandDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
}
//���Ⴊ�݃K�[�h
void Player::LoadStateGuardSquat()
{
	//�K�[�h���[�V����
	m_chara->GetAnimGuardSquat(*this);
	assert(m_handle != -1);
	//��炢����
	m_chara->GetHitBoxGuardSquat(*this);
	//�K�[�h
	m_update = &Player::GuardSquatUpdate;
	m_draw = &Player::GuardSquatDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
}


//�_�E��
void Player::LoadStateDown()
{
	//�_�E�����[�V����
	m_chara->GetAnimDown(*this);
	assert(m_handle != -1);
	//�q�b�g�{�b�N�X
	m_chara->GetHitBoxDown(*this);
	//�_�E��
	m_update = &Player::DownUpdate;
	m_draw = &Player::DownDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
	m_noActFrame = kDownFrame;
}

void Player::LoadStateDownAerial()
{
	//�_�E�����[�V����
	m_chara->GetAnimDownAerial(*this);
	assert(m_handle != -1);
	//�q�b�g�{�b�N�X
	m_chara->GetHitBoxDownAerial(*this);
	//�_�E��
	m_update = &Player::DownAerialUpdate;
	m_draw = &Player::DownAerialDraw;
	m_animIndex = 0;
	m_animCountFrame = 0;
	m_noActFrame = kDownFrame;
}


void Player::Cancel(Input& input, Player enemy, std::shared_ptr<Bullet> myBullet, GameManager& gameManager)
{
	//�L�����Z���\�ɂȂ�����
	if (m_isPossibleCancel)
	{
		m_cancelFrameCount++;
		//�L�����Z���\�P�\�ȓ��œ��͂���������
		if (m_cancelFrameCount <= kCanselGraceFrame)
		{
			////////////////////�U��//////////////////////////////////////
			//��p���`
			if (m_lightPunchBottun.isPress)
			{
				m_attackType = AttackTypes::LightPunch;
			}
			//���p���`
			if (m_highPunchBottun.isPress)
			{
				m_attackType = AttackTypes::HighPunch;
			}
			//��L�b�N
			if (m_lightKickBottun.isPress)
			{
				m_attackType = AttackTypes::LightKick;
			}
			//���L�b�N
			if (m_highKickBottun.isPress)
			{
				m_attackType = AttackTypes::HighKick;
			}
			//�Ȃ񂩍U�����Ă���
			if (m_attackType != AttackTypes::Null)
			{
				//�R�}���h���`�F�b�N���Đ������Ă���Z���擾
				if (m_chara->SuccessCommandCheck(input, *this))
				{
					m_isPossibleCancel = false;
					//�R�}���h�Z
					//�A�j���[�V�����̎擾
					m_chara->GetAnimCommand(*this);
					assert(m_handle != -1);
					//���ꔻ��
					m_chara->GetHitBoxCommand(*this);
					//�R�}���h�Z�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
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
	//�ʏ�U���̍ŏ��̐�F�̊ԂŃR�}���h���������Ă�����㏑������
	if (m_animCountFrame <= kInputDelayFrame)
	{
		//�R�}���h���`�F�b�N���Đ������Ă���Z���擾
		if (m_chara->SuccessCommandCheck(input, *this))
		{
			m_isPossibleCancel = false;
			//�R�}���h�Z
			//�A�j���[�V�����̎擾
			m_chara->GetAnimCommand(*this);
			assert(m_handle != -1);
			//���ꔻ��
			m_chara->GetHitBoxCommand(*this);
			//�R�}���h�Z�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
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
	//�̗͂��Ȃ��Ȃ�����
	if (m_hp <= 0)
	{
		++m_angle;
		//���[��
		LoseSE();
		//�������łق���
		//�����������������(��ʊO�ɍs�����߂�)
		ResetPushBox();
		//��ʊO�ɐ������
		if ((0 - kCharaWidth) < m_pos.x && m_pos.x < (kStageWidth + kCharaWidth))
		{
			m_velocity.x *= 1.05f;
			m_velocity.y -= 3.0f;
		}
		else
		{
			//��ʊO�ɍs������0�ɂ���
			m_velocity.x = 0;
			m_velocity.y = 0;
		}
		//HP��0���Ⴍ���Ȃ����߂�
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
	//�U�����胊�Z�b�g
	ResetAttackBox();
	ResetGraspBox();
	//����̕���������
	LookDir(enemy);

	//�ړ��ʃ��Z�b�g
	m_velocity.x = 0;
	//���Z�b�g
	m_giveThrowVelo.x = 0;
	m_giveThrowVelo.y = 0;
	//�U���̎�i���Z�b�g
	m_attackType = AttackTypes::Null;
	//�U���̑������Z�b�g
	m_attackAttributes = AttackAttributes::Null;
	//���O�̃n���h�����o����
	int lastHandle = m_handle;
	//���Ⴊ��ł��Ȃ��̂�false
	m_isSquat = false;
	//�K�[�h���Z�b�g
	m_isGuard = false;
	//���Z�b�g
	InitPushBox();//�������������߂�
	m_isHitGrasp = false;
	m_isThrown = false;
	m_isThrowSuccess = false;
	//�R�}���h�Z
	m_isCommand = false;

	//����̗̑͂��Ȃ��Ȃ�����܂��͎��Ԑ؂�
	if ((enemy.GetHp() <= 0) && (m_hp > 0) || gameManager.GetTimer() <= 0)
	{
		m_update = &Player::ResultUpdate;
		m_draw = &Player::ResultDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet,gameManager);
		return;
	}

	//�v���C���[������̕����Ɣ��΂ɃX�e�B�b�N��|�����Ƃ��K�[�h����
	if (input.IsPress("Right") && m_isLeft || input.IsPress("Left") && !m_isLeft)
	{
		m_isGuard = true;
	}
	else
	{
		m_isGuard = false;
	}

	//���ɃX�e�B�b�N��|�����Ƃ�
	if (input.IsPress("Down"))
	{
		//���Ⴊ��
		m_update = &Player::IdleSquatUpdate;
		m_draw = &Player::IdleSquatDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}

	////////////////////�U��//////////////////////////////////////
	//����true�Ȃ瓊��
	bool isLightPunch = false;
	bool isLightKick = false;
		//��p���`
	if (m_lightPunchBottun.isPress)
	{
		m_attackType = AttackTypes::LightPunch;
		isLightPunch = true;
	}
	//���p���`
	if (m_highPunchBottun.isPress)
	{
		m_attackType = AttackTypes::HighPunch;
	}
	//��L�b�N
	if (m_lightKickBottun.isPress)
	{
		m_attackType = AttackTypes::LightKick;
		isLightKick = true;
	}
	//���L�b�N
	if (m_highKickBottun.isPress)
	{
		m_attackType = AttackTypes::HighKick;
	}
	//�����{�^������
	if (m_graspBottun.isPress)
	{
		isLightPunch = true;
		isLightKick = true;
	}
	//��p���`�Ǝ�L�b�N���������œ���
	if (isLightPunch && isLightKick)
	{
		m_update = &Player::GraspUpdate;
		m_draw = &Player::GraspDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet,gameManager);
		return;
	}


	//�Ȃ񂩍U�����Ă���
	if (m_attackType != AttackTypes::Null)
	{
		//SE�̍Đ��X�g�b�v
		m_se->Stop();
		//�R�}���h���`�F�b�N���Đ������Ă���Z���擾
		if (m_chara->SuccessCommandCheck(input, *this))
		{
			//�R�}���h�Z
			//�A�j���[�V�����̎擾
			m_chara->GetAnimCommand(*this);
			assert(m_handle != -1);
			//���ꔻ��
			m_chara->GetHitBoxCommand(*this);
			//�R�}���h�Z�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
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


	////////////////////�ړ�//////////////////////////////////////
	//�E�ɃX�e�B�b�N��|�����Ƃ�
	//if ((input.GetStickInfo().leftStickX > 300))
	if (input.IsPress("Right"))
	{
		//�E�������Ă�Ȃ�
		if (!m_isLeft)
		{
			//�O����
			m_chara->GetAnimWalkFront(*this);
			assert(m_handle != -1);
			//�����蔻��
			m_chara->GetHitBoxWalkFront(*this);
			//�E�ɐi��
			//�X�g2���l�ɑ���Ɍ������Đi�ރX�s�[�h��傫������
			m_velocity.x = kMoveSpeedFront;
		}
		else
		{
			//������
			m_chara->GetAnimWalkBack(*this);
			assert(m_handle != -1);
			//�����蔻��
			m_chara->GetHitBoxWalkBack(*this);
			//�E�ɐi��
			//�X�g2���l�ɑ��肩�痣���X�s�[�h������������
			m_velocity.x = kMoveSpeedBack;
		}
	}
	//���ɃX�e�B�b�N��|�����Ƃ�	�����Ⴊ��łȂ��Ƃ�
	else if (input.IsPress("Left")) {
		//�E�������Ă�Ȃ�
		if (!m_isLeft)
		{
			//������
			m_chara->GetAnimWalkBack(*this);
			assert(m_handle != -1);
			//�����蔻��
			m_chara->GetHitBoxWalkBack(*this);
			//�E�ɐi��
			//�X�g2���l�ɑ��肩�痣���X�s�[�h������������
			m_velocity.x = -kMoveSpeedBack;
		}
		else
		{
			//�O����
			m_chara->GetAnimWalkFront(*this);
			assert(m_handle != -1);
			//�����蔻��
			m_chara->GetHitBoxWalkFront(*this);
			//�E�ɐi��
			//�X�g2���l�ɑ��肩�痣���X�s�[�h��傫������
			m_velocity.x = -kMoveSpeedFront;
		}
	}
	else
	{
		//�����ҋ@
		m_chara->GetAnimIdleStand(*this);
		assert(m_handle != -1);
		//�����蔻��
		m_chara->GetHitBoxIdleStand(*this);
	}
	//�؂�ւ�����烊�Z�b�g
	//���O�̃n���h���Ɛ؂�ւ�������ɃA�j���[�V�������ŏ�����Đ�����
	if (lastHandle != m_handle)
	{
		m_animIndex = 0;
		m_animCountFrame = 0;
	}

	//��ɃX�e�B�b�N��|�����Ƃ�
	if (input.IsPress("Up"))
	{
		//�W�����v�̕��������߂�
		if (m_velocity.x > 0)//�E
		{
			m_jumpVelo.x = kJumpPowerX;
		}
		else if (m_velocity.x < 0)//��
		{
			m_jumpVelo.x = -kJumpPowerX;
		}
		else//�^��
		{
			m_jumpVelo.x = 0;
		}
		
		//�W�����v
		JumpSE();
		m_update = &Player::JumpUpdate;
		m_draw = &Player::JumpDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		return;
	}
	

	//�퓬�J�n�O��Update���~�߂Ă�Ƃ��ɂ��̃A�j���[�V���������͎~�߂����Ȃ��̂ł����ɏ���
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = 0;
		}
	}
	m_animCountFrame++;
}

void Player::IdleStandDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::IdleSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�U�����胊�Z�b�g
	ResetAttackBox();
	ResetGraspBox();
	//����̕���������
	LookDir(enemy);
	//�ړ��ʃ��Z�b�g
	m_velocity.x = 0;
	//���Z�b�g
	m_giveThrowVelo.x = 0;
	m_giveThrowVelo.y = 0;
	//�U���̎�i���Z�b�g
	m_attackType = AttackTypes::Null;
	//�U���̑������Z�b�g
	m_attackAttributes = AttackAttributes::Null;
	//���Ⴊ�ݑҋ@
	m_chara->GetAnimIdleSquat(*this);
	assert(m_handle != -1);
	//�����蔻��
	m_chara->GetHitBoxIdleSquat(*this);
	//���Ⴊ��ł�̂�true
	m_isSquat = true;
	//�K�[�h���Z�b�g
	m_isGuard = false;
	//���Z�b�g
	InitPushBox();//�������������߂�
	m_isHitGrasp = false;
	m_isThrown = false;
	m_isThrowSuccess = false;
	//�R�}���h�Z
	m_isCommand = false;

	//����̗̑͂��Ȃ��Ȃ�����܂��͎��Ԑ؂�
	if ((enemy.GetHp() <= 0) && (m_hp > 0) || gameManager.GetTimer() <= 0)
	{
		m_update = &Player::ResultUpdate;
		m_draw = &Player::ResultDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}

	//�v���C���[������̕����Ɣ��΂ɃX�e�B�b�N��|�����Ƃ��K�[�h����
	if (input.IsPress("Right") && m_isLeft || input.IsPress("Left") && !m_isLeft)
	{
		m_isGuard = true;
	}
	else
	{
		m_isGuard = false;
	}

	/////�U��////////

	//����true�Ȃ瓊��
	bool isLightPunch = false;
	bool isLightKick = false;
	//��p���`
	if (m_lightPunchBottun.isPress)
	{
		m_attackType = AttackTypes::LightPunch;
		isLightPunch = true;
	}
	//���p���`
	if (m_highPunchBottun.isPress)
	{
		m_attackType = AttackTypes::HighPunch;
	}
	//��L�b�N
	if (m_lightKickBottun.isPress)
	{
		m_attackType = AttackTypes::LightKick;
		isLightKick = true;
	}
	//���L�b�N
	if (m_highKickBottun.isPress)
	{
		m_attackType = AttackTypes::HighKick;
	}
	//�����{�^������
	if (m_graspBottun.isPress)
	{
		isLightPunch = true;
		isLightKick = true;
	}
	
	//��p���`�Ǝ�L�b�N���������œ���
	if (isLightPunch && isLightKick)
	{
		m_update = &Player::GraspUpdate;
		m_draw = &Player::GraspDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		(this->*m_update)(input, enemy, myBullet,gameManager);
		return;
	}

	//�Ȃ񂩍U�����Ă���
	if (m_attackType != AttackTypes::Null)
	{
		//�R�}���h���������Ă��邩
		if (m_chara->SuccessCommandCheck(input, *this))
		{
			//�R�}���h�Z
			//�A�j���[�V�����̎擾
			m_chara->GetAnimCommand(*this);
			assert(m_handle != -1);
			//���ꔻ��
			m_chara->GetHitBoxCommand(*this);
			//�R�}���h�Z�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
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
	////�ړ�/////

		//��ɃX�e�B�b�N��|�����Ƃ�
	if (input.IsPress("Up"))
	{
		//�W�����v�̕��������߂�
		if (m_velocity.x > 0)//�E
		{
			m_jumpVelo.x = kJumpPowerX;
		}
		else if (m_velocity.x < 0)//��
		{
			m_jumpVelo.x = -kJumpPowerX;
		}
		else//�^��
		{
			m_jumpVelo.x = 0;
		}
		//�W�����v
		m_update = &Player::JumpUpdate;
		m_draw = &Player::JumpDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		//�W�����v�����������s���Ȃ��i�o�O�邩��j
		return;
	}
	//�X�e�B�b�N��|���ĂȂ��Ƃ�
	if (!input.IsPress("Down"))
	{
		//����
		m_update = &Player::IdleStandUpdate;
		m_draw = &Player::IdleStandDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		//��Ԃ��؂�ւ�����炱�̃t���[���ł��̏�Ԃ����s������
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}


	m_animCountFrame++;
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = 0;
		}
	}
}

void Player::IdleSquatDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::JumpUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�U�����胊�Z�b�g
	ResetAttackBox();
	ResetGraspBox();
	//�t���[���𐔂���
	m_animCountFrame++;
	//�K�[�h�ł��Ȃ�
	m_isGuard = false;
	//�W�����v
	m_chara->GetAnimJump(*this);
	assert(m_handle != -1);
	//�����蔻��
	m_chara->GetHitBoxJump(*this);
	//�R�}���h�Z
	m_isCommand = false;

	//�ŏ���4�t���[���͒n��
	if (m_animCountFrame < kJumpTransFrame)
	{
		m_velocity.x = 0;
		//��p���`
		if (m_lightPunchBottun.isPress)
		{
			m_attackType = AttackTypes::LightPunch;
		}
		//���p���`
		if (m_highPunchBottun.isPress)
		{
			m_attackType = AttackTypes::HighPunch;
		}
		//��L�b�N
		if (m_lightKickBottun.isPress)
		{
			m_attackType = AttackTypes::LightKick;
		}
		//���L�b�N
		if (m_highKickBottun.isPress)
		{
			m_attackType = AttackTypes::HighKick;
		}
		//�Ȃ񂩍U�����Ă���
		if (m_attackType != AttackTypes::Null)
		{
			//�R�}���h���`�F�b�N���Đ������Ă���Z���擾
			if (m_chara->SuccessCommandCheck(input, *this))
			{
				//�R�}���h�Z
				//�A�j���[�V�����̎擾
				m_chara->GetAnimCommand(*this);
				assert(m_handle != -1);
				//���ꔻ��
				m_chara->GetHitBoxCommand(*this);
				//�R�}���h�Z�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
				m_chara->GetGiveEffectCommand(*this);
				//�R�}���h�Z
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
	//5�t���[������W�����v
	else if (m_animCountFrame == kJumpTransFrame)
	{
		m_velocity = m_jumpVelo;
	}
	else
	{
		//�n��ɂ�����
		if (m_isGround)
		{
			//���n��
			JumpedSE();
			m_update = &Player::IdleStandUpdate;
			m_draw = &Player::IdleStandDraw;
			m_animIndex = 0;
			m_animCountFrame = 0;
			//��t���[���x�点��i�����㌄�j
			return;
		}
		//��p���`
		if (m_lightPunchBottun.isPress)
		{
			m_attackType = AttackTypes::LightPunch;
		}
		//���p���`
		if (m_highPunchBottun.isPress)
		{
			m_attackType = AttackTypes::HighPunch;
		}
		//��L�b�N
		if (m_lightKickBottun.isPress)
		{
			m_attackType = AttackTypes::LightKick;
		}
		//���L�b�N
		if (m_highKickBottun.isPress)
		{
			m_attackType = AttackTypes::HighKick;
		}
		//�Ȃ񂩍U�����Ă���
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


	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
#if _DEBUG
	//�f�o�b�O
	debugJumpFrame = m_animCountFrame;
#endif

}

void Player::JumpDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::AttackStandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	
	//�K�[�h�ł��Ȃ�
	m_isGuard = false;
	
	if (m_attackType == AttackTypes::LightPunch)
	{
		//���E�̓������~�߂�
		m_velocity.x = 0;
		//��p���`
		m_chara->GetAnimPunchLight(*this);
		assert(m_handle != -1);
		//��p���`���[�V�����̓����蔻����擾
		m_chara->GetHitBoxPunchLight(*this);
		//��p���`�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
		m_chara->GetGiveEffectPunchLight(*this);
	}
	else if (m_attackType == AttackTypes::HighPunch)
	{
		//�����O�i����
		m_velocity.x = 1.0f;//�E
		if (m_isLeft)
		{
			m_velocity.x = -1.0f;//��
		}
		
		//���p���`
		m_chara->GetAnimPunchHigh(*this);
		assert(m_handle != -1);
		//���p���`���[�V�����̓����蔻����擾
		m_chara->GetHitBoxPunchHigh(*this);
		//���p���`�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
		m_chara->GetGiveEffectPunchHigh(*this);
	}
	else if (m_attackType == AttackTypes::LightKick)
	{
		//���E�̓������~�߂�
		m_velocity.x = 0;
		//��L�b�N
		m_chara->GetAnimKickLight(*this);
		assert(m_handle != -1);
		//��L�b�N���[�V�����̓����蔻����擾
		m_chara->GetHitBoxKickLight(*this);
		//��L�b�N�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
		m_chara->GetGiveEffectKickLight(*this);
	}
	else if (m_attackType == AttackTypes::HighKick)
	{
		//�����O�i����
		m_velocity.x = 1.0f;//�E
		if (m_isLeft)
		{
			m_velocity.x = -1.0f;//��
		}
		//���L�b�N
		m_chara->GetAnimKickHigh(*this);
		assert(m_handle != -1);
		//���L�b�N���[�V�����̓����蔻����擾
		m_chara->GetHitBoxKickHigh(*this);
		//���L�b�N�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
		m_chara->GetGiveEffectKickHigh(*this);
	}
	//SE�Đ�
	AttackSE();

	m_animCountFrame++;
	//�ŏ���2�t���[���̊ԂɃR�}���h���������Ă���Ȃ�㏑��
	InputDelay(input, enemy, myBullet, gameManager);

	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if ((m_animIndex > animMaxNum))
		{
			//�Đ����~�߂�
			m_se->Stop();
			//�X�e�B�b�N�����ɓ|���Ă�
			if (input.IsPress("Down"))
			{
				//���Ⴊ��
				m_update = &Player::IdleSquatUpdate;
				m_draw = &Player::IdleSquatDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy,myBullet, gameManager);
				return;
			}
			else
			{
				//����
				m_update = &Player::IdleStandUpdate;
				m_draw = &Player::IdleStandDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy,myBullet, gameManager);
				return;
			}
		}
	}
	
	//�L�����Z��
	Cancel(input, enemy, myBullet, gameManager);
#if _DEBUG
	m_isCheckAttackBox = false;
#endif
	//�t���[�����U�������t���[���ȓ��̎�
	if (m_startAttackFrame <= m_animCountFrame && m_animCountFrame <= m_finishAttackFrame)
	{
#if _DEBUG
		m_isCheckAttackBox = true;
#endif
		//�U���������������ǂ���
		m_isHitAttack = CheckHit(enemy);
		if (m_isHitAttack)
		{
			//�q�b�g�X�g�b�v
			gameManager.OnIsHitStop();
			//�L�����Z���\�ɂ���
			m_isPossibleCancel = true;
			m_cancelFrameCount = 0;
#if _DEBUG
			DrawString(600, 200, "��������\n", 0xff0000);
#endif
			//���Z�b�g
			m_attackType = AttackTypes::Null;
		}
	}

	//�U�������I���
	if (m_animCountFrame > m_finishAttackFrame)
	{
		//�����蔻�胊�Z�b�g
		m_attackType = AttackTypes::Null;
		ResetAttackBox();
	}


}

void Player::AttackStandDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::AttackSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�K�[�h�ł��Ȃ�
	m_isGuard = false;
	if (m_attackType == AttackTypes::LightPunch)
	{
		//���E�̓������~�߂�
		m_velocity.x = 0;
		//���Ⴊ�ݎ�p���`
		m_chara->GetAnimPunchLightSquat(*this);
		assert(m_handle != -1);
		//���Ⴊ�ݎ�p���`���[�V�����̓����蔻����擾
		m_chara->GetHitBoxPunchLightSquat(*this);
		//���Ⴊ�ݎ�p���`�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
		m_chara->GetGiveEffectPunchLightSquat(*this);
	}
	else if (m_attackType == AttackTypes::HighPunch)
	{
		//���E�̓������~�߂�
		m_velocity.x = 0;
		//���Ⴊ�݋��p���`
		m_chara->GetAnimPunchHighSquat(*this);
		assert(m_handle != -1);
		//���Ⴊ�݋��p���`���[�V�����̓����蔻����擾
		m_chara->GetHitBoxPunchHighSquat(*this);
		//���Ⴊ�݋��p���`�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
		m_chara->GetGiveEffectPunchHighSquat(*this);
	}
	else if (m_attackType == AttackTypes::LightKick)
	{
		//���E�̓������~�߂�
		m_velocity.x = 0;
		//���Ⴊ�ݎ�L�b�N
		m_chara->GetAnimKickLightSquat(*this);
		assert(m_handle != -1);
		//���Ⴊ�ݎ�L�b�N���[�V�����̓����蔻����擾
		m_chara->GetHitBoxKickLightSquat(*this);
		// ���Ⴊ�ݎ�L�b�N�̃_���[�W�ƍd�������擾
		m_chara->GetGiveEffectKickLightSquat(*this);
	}
	else if (m_attackType == AttackTypes::HighKick)
	{
		//���E�̓������~�߂�
		m_velocity.x = 0;
		//���L�b�N
		m_chara->GetAnimKickHighSquat(*this);
		assert(m_handle != -1);
		//���L�b�N���[�V�����̓����蔻����擾
		m_chara->GetHitBoxKickHighSquat(*this);
		//���L�b�N�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
		m_chara->GetGiveEffectKickHighSquat(*this);
	}
	//SE�Đ�
	AttackSE();

	m_animCountFrame++;
	//�ŏ���2�t���[���̊ԂɃR�}���h���������Ă���Ȃ�㏑��
	InputDelay(input, enemy, myBullet, gameManager);
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if ((m_animIndex > animMaxNum))
		{
			//�Đ����~�߂�
			m_se->Stop();
			//�X�e�B�b�N�����ɓ|���Ă�
			if (input.IsPress("Down"))
			{
				//���Ⴊ��
				m_update = &Player::IdleSquatUpdate;
				m_draw = &Player::IdleSquatDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy,myBullet, gameManager);
				return;
			}
			else
			{
				//����
				m_update = &Player::IdleStandUpdate;
				m_draw = &Player::IdleStandDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy,myBullet, gameManager);
				return;
			}
		}
	}
	

	//�L�����Z��
	Cancel(input, enemy, myBullet, gameManager);

#if _DEBUG
	m_isCheckAttackBox = false;
#endif
	//�t���[�����U�������t���[���ȓ��̎�
	if (m_startAttackFrame <= m_animCountFrame && m_animCountFrame <= m_finishAttackFrame)
	{
#if _DEBUG
		m_isCheckAttackBox = true;
#endif
		//�U���������������ǂ���
		m_isHitAttack = CheckHit(enemy);
		if (m_isHitAttack)
		{
			//�q�b�g�X�g�b�v
			gameManager.OnIsHitStop();
			//�L�����Z���\�ɂ���
			m_isPossibleCancel = true;
			m_cancelFrameCount = 0;

			//���Z�b�g
			m_attackType = AttackTypes::Null;
			//ResetAttackBox();
		}
	}
	//�U�������I���
	if (m_animCountFrame > m_finishAttackFrame)
	{
		//�����蔻�胊�Z�b�g
		m_attackType = AttackTypes::Null;
		ResetAttackBox();
	}
}

void Player::AttackSquatDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::AttackAerialUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�n�ʂɂ�����
	if (m_isGround)
	{
		//���n��
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

	//�K�[�h�ł��Ȃ�
	m_isGuard = false;
	if (m_attackType == AttackTypes::LightPunch)
	{
		//�󒆎�p���`
		m_chara->GetAnimPunchLightAerial(*this);
		assert(m_handle != -1);
		//�󒆎�p���`���[�V�����̓����蔻����擾
		m_chara->GetHitBoxPunchLightAerial(*this);
		//�󒆎�p���`�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
		m_chara->GetGiveEffectPunchLightAerial(*this);
	}
	else if (m_attackType == AttackTypes::HighPunch)
	{
		//�󒆋��p���`
		m_chara->GetAnimPunchHighAerial(*this);
		assert(m_handle != -1);
		//�󒆋��p���`���[�V�����̓����蔻����擾
		m_chara->GetHitBoxPunchHighAerial(*this);
		//�󒆋��p���`�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
		m_chara->GetGiveEffectPunchHighAerial(*this);
	}
	else if (m_attackType == AttackTypes::LightKick)
	{
		//�󒆎�L�b�N
		m_chara->GetAnimKickLightAerial(*this);
		assert(m_handle != -1);
		//�󒆎�L�b�N���[�V�����̓����蔻����擾
		m_chara->GetHitBoxKickLightAerial(*this);
		//�󒆎�L�b�N�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
		m_chara->GetGiveEffectKickLightAerial(*this);
	}
	else if (m_attackType == AttackTypes::HighKick)
	{
		//�󒆋��L�b�N
		m_chara->GetAnimKickHighAerial(*this);
		assert(m_handle != -1);
		//�󒆋��L�b�N���[�V�����̓����蔻����擾
		m_chara->GetHitBoxKickHighAerial(*this);
		//�󒆋��L�b�N�̃_���[�W�Ƃӂ��Ƃ΂��ƍd�������擾
		m_chara->GetGiveEffectKickHighAerial(*this);
	}

	m_animCountFrame++;
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
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
	//�t���[�����U�������t���[���ȓ��̎�
	if (m_startAttackFrame <= m_animCountFrame && m_animCountFrame <= m_finishAttackFrame)
	{
		m_animIndex = m_animNum/2;
#if _DEBUG
		m_isCheckAttackBox = true;
#endif
		//�U���������������ǂ���
		m_isHitAttack = CheckHit(enemy);
		if (m_isHitAttack)
		{
			//�q�b�g�X�g�b�v
			gameManager.OnIsHitStop();
			//���Z�b�g
			m_attackType = AttackTypes::Null;
		}
	}
	//�U�������I���
	if (m_animCountFrame > m_finishAttackFrame)
	{
		//�����蔻�胊�Z�b�g
		m_attackType = AttackTypes::Null;
		ResetAttackBox();
	}


}

void Player::AttackAerialDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

//�󒆍U����
void Player::AttackedAerialUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
#if _DEBUG
	//�R�}���h�̍d���܂Ő�����
	debugCommandFrame++;
#endif
	m_chara->GetAnimJump(*this);
	m_chara->GetHitBoxAttackedAerial(*this);
	//�K�[�h�ł��Ȃ�
	m_isGuard = false;
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	m_animIndex = animMaxNum;
	if (m_isGround)
	{
		m_animCountFrame++;
		if (m_animCountFrame > 3)//���n�d����
		{
			//���n��
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
		//���n�d���𐔂��邽�߂ɋ󒆂ɂ���Ԃ�0
		m_animCountFrame = 0;
	}
}

void Player::AttackedAerialDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::CommandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�R�}���h�Z
	m_isCommand = true;

	//�K�[�h�ł��Ȃ�
	m_isGuard = false;
	
	//�R�}���h�Z�͓���������Ȃ��̂������̂�Chara�̂ق��œ��������
	m_chara->MovementCommand(*this, *myBullet, enemy);

	
	m_animCountFrame++;
#if _DEBUG
	//�R�}���h�S�̃t���[��
	debugCommandFrame = m_animCountFrame;
#endif
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
	}
	//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
	if (m_animIndex > animMaxNum)
	{
		m_animIndex = animMaxNum;

		//�A�j���[�V�������U���̎����I������ɏI����Ă��܂����ꍇ��
		//�A�j���[�V�������Ō�̃A�j���[�V�����Ŏ~�߂�
		if (m_animCountFrame >= m_finishAttackFrame)
		{
			//�ړ��ʃ��Z�b�g
			m_velocity.x = 0;
			//���Z�b�g
			InitPushBox();//�������������߂�
			m_isHitGrasp = false;
			m_isThrown = false;
			m_isThrowSuccess = false;

			if (!m_isGround)
			{
				//���Z�b�g
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
				//�X�e�B�b�N�����ɓ|���Ă�
				if (input.IsPress("Down"))
				{
					//���Ⴊ��
					m_update = &Player::IdleSquatUpdate;
					m_draw = &Player::IdleSquatDraw;
					m_animIndex = 0;
					m_animCountFrame = 0;
					(this->*m_update)(input, enemy, myBullet, gameManager);
					return;
				}
				else
				{
					//����
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

	//�t���[�����U�������t���[���ȓ��̎�
	if (m_startAttackFrame <= m_animCountFrame && m_animCountFrame <= m_finishAttackFrame)
	{
#if _DEBUG
		m_isCheckAttackBox = true;
#endif
		//�U���������������ǂ���
		m_isHitAttack = CheckHit(enemy);
		if (m_isHitAttack)
		{
			//�q�b�g�X�g�b�v
			gameManager.OnIsHitStop();
			//�����蔻�胊�Z�b�g
			m_attackType = AttackTypes::Null;
		}

		//�����������������ǂ���
		m_isHitGrasp = CheckHitGrasp(enemy);
		if (m_isHitGrasp)
		{
			//�A�j���[�V���������Z�b�g
			m_animCountFrame = 0;
			m_animIndex = 0;

			//�����蔻�胊�Z�b�g
			ResetGraspBox();
			//�����������������
			ResetPushBox();
		}
	}
	
	//�U�������I���
	if (m_animCountFrame > m_finishAttackFrame)
	{
		//�����蔻�胊�Z�b�g
		m_attackType = AttackTypes::Null;
		ResetAttackBox();
	}
}



void Player::CommandDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}


void Player::GuardStandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�U�����胊�Z�b�g
	ResetAttackBox();
	ResetGraspBox();
	//�K�[�h�d���t���[��������Ȃ�
	if (m_guardFrame <= 0)
	{
		m_guardFrame = 0;
		//����
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
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;

	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
}

void Player::GuardStandDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::GuardSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�U�����胊�Z�b�g
	ResetAttackBox();
	ResetGraspBox();
	//�K�[�h�d���t���[��������Ȃ�
	if (m_guardFrame <= 0)
	{
		m_guardFrame = 0;
		//����
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
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
}

void Player::GuardSquatDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

//����
void Player::GraspUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�K�[�h�ł��Ȃ�
	m_isGuard = false;
	//�ړ��ʃ��Z�b�g
	m_velocity.x = 0;
	//����
	m_chara->GetAnimGrasp(*this);
	assert(m_handle != -1);
	//�q�b�g�{�b�N�X
	m_chara->GetHitBoxGrasp(*this);

	m_animCountFrame++;
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if (m_animIndex > animMaxNum)
		{
			//�����蔻�胊�Z�b�g
			ResetGraspBox();
			//�X�e�B�b�N�����ɓ|���Ă�
			if (input.GetStickInfo().leftStickY > 300)
			{
				//���Ⴊ��
				m_update = &Player::IdleSquatUpdate;
				m_draw = &Player::IdleSquatDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy, myBullet, gameManager);
				return;
			}
			else
			{
				//����
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
	//�t���[�����U�������t���[���ȓ��̎�
	if (m_startAttackFrame <= m_animCountFrame && m_animCountFrame <= m_finishAttackFrame)
	{
#if _DEBUG
		m_isCheckAttackBox = true;
#endif
		//�U���������������ǂ���
		m_isHitGrasp = CheckHitGrasp(enemy);
		//�Q�[���}�l�[�W���[�̒��œ������ʂ��������`�F�b�N����
		//�������Ă���ꍇ�@m_isThrowSuccess�@��true�ɂȂ�
	}
	//�U�������I���
	if (m_animCountFrame > m_finishAttackFrame)
	{
		//�����蔻�胊�Z�b�g
		ResetGraspBox();
	}

	//�������ʂ�����
	if (m_isThrowSuccess)
	{
		//SE�Đ�
		GraspSE();

		m_chara->GetGiveEffectGrasp(*this);
		//������ӂ��Ƃ΂�����
		Vector3 throwVelo;
		throwVelo.x = kThrowVeloX;

		if (m_isLeft)//�������������Ȃ獶�ɔ�΂�
		{
			throwVelo.x *= -1;
		}
		throwVelo.y = kThrowVeloY;
		//�����Ă�����Ɣ��Ε����������Ă�Ƃ��i��듊���j
		if (input.IsPress("Left") && !m_isLeft || input.IsPress("Right") && m_isLeft)
		{
			//����
			m_chara->GetAnimThrowBack(*this);
			assert(m_handle != -1);
			//�q�b�g�{�b�N�X
			m_chara->GetHitBoxThrowBack(*this);
			//���ɓ�����̂Ŕ��]
			throwVelo.x *= -kThrowBackMargin;

			//�������኱������
			m_velocity.x = kBackThrowShiftVelo;
			if (m_isLeft)
			{
				m_velocity.x *= -1.0f;
			}
		}
		else
		{
			//�ړ��ʃ��Z�b�g
			m_velocity.x = 0;
			//����
			m_chara->GetAnimThrowFront(*this);
			assert(m_handle != -1);
			//�q�b�g�{�b�N�X
			m_chara->GetHitBoxThrowFront(*this);
		}
		//�Z�b�g
		enemy.SetThrowVelo(throwVelo);

		DrawString(600, 200, "��������������\n", 0xff0000);
		//�����蔻�胊�Z�b�g
		ResetGraspBox();

		//�����鑊��̏����l
		Vector3 posOffset(10, 0, 0);
		if (m_isLeft)
		{
			posOffset.x *= -1;
		}
		enemy.SetPos(m_pos + posOffset);
		//�����Ɉڍs
		ResetPushBox();//�����������������
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
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

//������
void Player::ThrowUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�K�[�h�ł��Ȃ�
	m_isGuard = false;

	m_animCountFrame++;
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;

		if (m_animIndex == animMaxNum / 2)
		{
			//SE�Đ�
			ThrowSE();
			// ��������Ԃɂ���
			enemy.OnIsThrown();
		}

		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if (m_animIndex > animMaxNum)
		{
			m_se->Stop();
			//�ړ��ʃ��Z�b�g
			m_velocity.x = 0;
			//���Z�b�g
			InitPushBox();//�������������߂�
			m_isHitGrasp = false;
			m_isThrown = false;
			m_isThrowSuccess = false;
			//�X�e�B�b�N�����ɓ|���Ă�
			if (input.IsPress("Down"))
			{
				//���Ⴊ��
				m_update = &Player::IdleSquatUpdate;
				m_draw = &Player::IdleSquatDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy, myBullet, gameManager);
				return;
			}
			else
			{
				//����
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
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}
//��������
void Player::BeThrownUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�ړ��ʃ��Z�b�g
	m_velocity.x = 0;
	//�U�����胊�Z�b�g
	ResetAttackBox();
	ResetGraspBox();
	//�K�[�h�ł��Ȃ�
	m_isGuard = false;

	//�R�}�����͓��������ł��Ȃ�
	if (!enemy.IsCommand())
	{
		if (m_animCountFrame <= kCanThrowEscapeFrame)
		{
			//����true�Ȃ瓊��
			bool isLightPunch = false;
			bool isLightKick = false;
			//��p���`
			if (m_lightPunchBottun.isPress)
			{
				isLightPunch = true;
			}
			//��L�b�N
			if (m_lightKickBottun.isPress)
			{
				isLightKick = true;
			}
			//�����{�^������
			if (m_graspBottun.isPress)
			{
				isLightPunch = true;
				isLightKick = true;
			}
			//��p���`�Ǝ�L�b�N���������œ���
			if (isLightPunch && isLightKick)
			{
				m_isThrowSuccess = true;
				//�����ʂ�
				enemy.LoadStateThrowEscape();
				//����
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
		//�q�b�g�X�g�b�v
		gameManager.OnIsHitStop();
		//�J������h�炷
		gameManager.OnIsCameraShake();
		//�K�[�h�ł��Ȃ�
		m_isGround = false;
		//�ӂ��Ƃ΂����
		m_velocity = m_giveThrowVelo;
		//�Փ˔��蕜��
		InitPushBox();
		//�����̃_���[�W
		m_hp -= enemy.GetGiveDamage();

		m_update = &Player::DownAerialUpdate;
		m_draw = &Player::DownAerialDraw;
		m_animIndex = 0;
		m_animCountFrame = 0;
		//���Z�b�g
		m_isHitGrasp = false;
		m_isThrown = false;
		m_isThrowSuccess = false;
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}

	m_animCountFrame++;
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
}

void Player::BeThrownDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, m_angle, m_handle, true, m_isLeft);
}

//��������
void Player::ThrowEscapeUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�U�����胊�Z�b�g
	ResetAttackBox();
	ResetGraspBox();
	//�Փ˔��蕜��
	InitPushBox();
	//�K�[�h�ł��Ȃ�
	m_isGuard = false;

	//�d���I����
	if (m_animCountFrame > kNoActThrowEscapeFrame)
	{
		//���Z�b�g
		m_isHitGrasp = false;
		m_isThrowSuccess = false;
		//�Փ˔��蕜��
		InitPushBox();
		//����
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
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
}

void Player::ThrowEscapeDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::DamageUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//���ꔻ��
	m_chara->GetHitBoxHitStand(*this);
	//�U�����胊�Z�b�g
	ResetAttackBox();
	ResetGraspBox();
	//�������Z�b�g
	m_isHitGrasp = false;
	m_isThrown = false;
	m_isThrowSuccess = false;
	//�K�[�h�ł��Ȃ�
	m_isGuard = false;
	//�d���t���[��
	if (m_noActFrame <= 0)
	{
		m_noActFrame = 0;
		
		if (m_isGround)
		{
			//����
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
			//��
			m_chara->GetAnimHitAerial(*this);
		}
		m_noActFrame--;
		m_velocity.x = m_knockback.x * static_cast<float>(m_noActFrame) / m_allNoActFrame;
	}

	m_animCountFrame++;
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
}

void Player::DamageDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
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


//�_�E��
void Player::DownUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�U�����胊�Z�b�g
	ResetAttackBox();
	ResetGraspBox();
	//�������Z�b�g
	m_isHitGrasp = false;
	m_isThrown = false;
	m_isThrowSuccess = false;
	//�K�[�h�ł��Ȃ�
	m_isGuard = false;
	//SE
	DownSE();

	//�n�ʂɂ��Đ��b��������N���オ��
	//�d���t���[��
	if ((m_noActFrame <= 0) && m_hp > 0)
	{
		m_noActFrame = 0;
		m_se->Stop();
		//�N���オ��
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
		//�n��ɂ���Ƃ��̓J�E���g����
		if (m_isGround)
		{
			m_noActFrame--;
			m_velocity.x = 0;
		}
	}

	m_animCountFrame++;
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
}

void Player::DownDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y) + kDownDrawOffset,
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, m_angle, m_handle, true, m_isLeft);
}

void Player::DownAerialUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//�U�����胊�Z�b�g
	ResetAttackBox();
	ResetGraspBox();
	//�������Z�b�g
	m_isHitGrasp = false;
	m_isThrown = false;
	m_isThrowSuccess = false;
	//�K�[�h�ł��Ȃ�
	m_isGuard = false;
	//�n�ʂɂ�����_�E��
	if (m_isGround)
	{
		//�_�E��
		LoadStateDown();
		m_animIndex = 4;//�r������n�߂���
		(this->*m_update)(input, enemy, myBullet, gameManager);
		return;
	}

	m_animCountFrame++;
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if ((m_animIndex > animMaxNum))
		{
			m_animIndex = animMaxNum;
		}
	}
	
}

void Player::DownAerialDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, m_angle, m_handle, true, m_isLeft);
}

//�N���オ��
void Player::StandUpUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	m_animCountFrame++;
	//�K�[�h�ł��Ȃ�
	m_isGuard = false;
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;
	//�A�j���[�V�����̃t���[���𐔂���
	if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndex++;
		//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
		if ((m_animIndex > animMaxNum))
		{
			//�X�e�B�b�N�����ɓ|���Ă�
			if (input.IsPress("Down"))
			{
				//���Ⴊ��
				m_update = &Player::IdleSquatUpdate;
				m_draw = &Player::IdleSquatDraw;
				m_animIndex = 0;
				m_animCountFrame = 0;
				(this->*m_update)(input, enemy, myBullet, gameManager);
				return;
			}
			else
			{
				//����
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
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}

void Player::ResultUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager)
{
	//���������
	ResetAttackBox();
	OffIsHitGrasp();
	m_chara->GetHitBoxDown(*this);

	m_velocity.x = 0;
	//�A�j���[�V������1���ڂ�0�ԂƂ��Đ�����̂�
	//�A�j���[�V�����̍ő吔����-1�����l���Ō�̃A�j���[�V����
	int animMaxNum = m_animNum - 1;

	m_animCountFrame++;

	
	//�����Ă��珟���|�[�Y
	if (m_animCountFrame >= kChangeWinPoseFrame && m_hp > enemy.GetHp())
	{
		//���Z�b�g����
		if (m_animCountFrame == kChangeWinPoseFrame && m_hp > enemy.GetHp())
		{
			m_animIndex = 0;
		}
		m_chara->GetAnimWinPose(*this);

		//�A�j���[�V�����̃t���[���𐔂���
		if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
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

		//�A�j���[�V�����̃t���[���𐔂���
		if (m_animCountFrame % m_oneAnimIntervalFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
			if ((m_animIndex > animMaxNum))
			{
				m_animIndex = 0;
			}
		}
	}
}

void Player::ResultDraw(const Camera& camera)
{
	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_handle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kCharaWidth);//��
	int cutY = m_animIndex / (sizeX / kCharaWidth);//�c
	//�`��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		kCharaScale, 0.0f, m_handle, true, m_isLeft);
}
