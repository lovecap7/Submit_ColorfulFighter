#include "Bullet.h"
#include <DxLib.h>
#include "Camera.h"
#include "Player.h"
#include "Camera.h"

namespace
{
	constexpr float kBulletOffsetPos = 90.0f;
	//�e�̑傫��
	constexpr float kBulletScale = 1.0f;
	constexpr int kBulletWidth = 160;
	constexpr int kBulletHeight = 160;
	//�e�̃A�j���[�V�����̑���
	constexpr int kBulletOneAnimFrame = 7;
	//���ł̃A�j���[�V�����̑���
	constexpr int kDisappearOneAnimFrame = 3;

	//�g����
	//�e�̔���
	constexpr float kHadouRadius = 40.0f;
	//�e�̔ԍ�
	constexpr int kStartHadouAnimIndex = 211;
	constexpr int kFinishHadouAnimIndex = 214;
	//�e�̈ʒu�̒���
	constexpr float kHadouOffsetPosX = 120.0f;
	//���x
	constexpr float kHadouVeloXLight = 15.0f;
	constexpr float kHadouVeloXHigh = 25.0f;
	
	//�\�j�b�N�u�[��
	//�e�̔���
	constexpr float kSonicRadius = 40.0f;
	//�e�̈ʒu�̒���
	constexpr float kSonicOffsetPosX = 120.0f;
	//�e�̔ԍ�
	constexpr int kStartSonicAnimIndex = 231;
	constexpr int kFinishSonicAnimIndex = 234;
	//���x
	constexpr float kSonicVeloXLight = 25.0f;
	constexpr float kSonicVeloXHigh = 30.0f;

	//�p���[�E�F�C�u
	//�e�̔���
	constexpr float kPowerWaveRadiusX = 50.0f;
	constexpr float kPowerWaveRadiusY = 30.0f;
	//�e�̔ԍ�
	constexpr int kStartPowerWaveAnimIndex = 6;
	constexpr int kFinishPowerWaveAnimIndex = 9;
	//�e�̈ʒu�̒���
	constexpr float kPowerWaveOffsetPosY = 120.0f;
	//���x
	constexpr float kWaveVeloXLight = 15.0f;
	constexpr float kWaveVeloXHigh = 30.0f;
	constexpr float kWaveVeloYHigh = -5.0f;

	//����
	constexpr int kStartDisappearAnimIndex = 202;
	constexpr int kFinishDisappearAnimIndex = 205;
}

void Bullet::UpdateHadou(Player& enemy, Bullet& otherBullet, Camera& camera)
{
	if (m_isShooting)
	{
		//�ړ�
		m_pos += m_velocity;

		//�G�ɓ����������ǂ������`�F�b�N
		//����������̏�����CollisionCheck�����i1P����2P���ŏ����̏��ŗD�򂪂��Ȃ��悤�ɂ��邽�߁j
		m_isHitPlayer = CheckHitPlayer(enemy);

		//�e���m�ő��E
		if (CheckHitBullet(otherBullet) && otherBullet.GetIsShooting())
		{
			Disappear();
			otherBullet.Disappear();
		}
		//�ǂ̊O�ɂ����������
		if (camera.GetCameraLeftWallPos() - m_hitBoxAttack.x2 > m_pos.x || camera.GetCameraRightWallPos() - m_hitBoxAttack.x1 < m_pos.x)
		{
			Disappear();
		}
		//�A�j���[�V�����̃t���[���𐔂���
		if (m_animCountFrame % kBulletOneAnimFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
			if ((m_animIndex > m_finishAnimIndex))
			{
				m_animIndex = m_startAnimIndex;
			}
		}
	}
	else
	{
		//�A�j���[�V�����̃t���[���𐔂���
		if (m_animCountFrame % kDisappearOneAnimFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
			if ((m_animIndex > m_finishAnimIndex))
			{
				m_animIndex = m_finishAnimIndex;
			}
		}
	}
}

void Bullet::DrawHadou(Camera& camera)
{
	//�e
	//���ɐi��ł�Ȃ獶������
	bool isLeft = false;
	if (m_velocity.x < 0)
	{
		isLeft = true;
	}

	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_blueBulletHandle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kBulletWidth);//��
	int cutY = m_animIndex / (sizeX / kBulletHeight);//�c
	////���C��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kBulletWidth * cutX,
		kBulletHeight * cutY,
		kBulletWidth, kBulletHeight,
		kBulletScale, 0.0f, m_blueBulletHandle, true, isLeft);
}

void Bullet::UpdateSonic(Player& enemy, Bullet& otherBullet, Camera& camera)
{
	if (m_isShooting)
	{

		//�ړ�
		m_pos += m_velocity;

		//�G�ɓ����������ǂ������`�F�b�N
		//����������̏�����CollisionCheck�����i1P����2P���ŏ����̏��ŗD�򂪂��Ȃ��悤�ɂ��邽�߁j
		m_isHitPlayer = CheckHitPlayer(enemy);

		//�e���m�ő��E
		if (CheckHitBullet(otherBullet) && otherBullet.GetIsShooting())
		{
			Disappear();
			otherBullet.Disappear();
		}
		//�ǂ̊O�ɂ����������
		if (camera.GetCameraLeftWallPos() - m_hitBoxAttack.x2 > m_pos.x || camera.GetCameraRightWallPos() - m_hitBoxAttack.x1 < m_pos.x)
		{
			Disappear();
		}
		//�A�j���[�V�����̃t���[���𐔂���
		if (m_animCountFrame % kBulletOneAnimFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
			if ((m_animIndex > m_finishAnimIndex))
			{
				m_animIndex = m_startAnimIndex;
			}
		}
	}
	else
	{
		//�A�j���[�V�����̃t���[���𐔂���
		if (m_animCountFrame % kDisappearOneAnimFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
			if ((m_animIndex > m_finishAnimIndex))
			{
				m_animIndex = m_finishAnimIndex;
			}
		}
	}
}

void Bullet::DrawSonic(Camera& camera)
{
	////�e
//���ɐi��ł�Ȃ獶������
	bool isLeft = false;
	if (m_velocity.x < 0)
	{
		isLeft = true;
	}

	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_yellowBulletHandle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kBulletWidth);//��
	int cutY = m_animIndex / (sizeX / kBulletHeight);//�c
	////���C��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kBulletWidth * cutX,
		kBulletHeight * cutY,
		kBulletWidth, kBulletHeight,
		kBulletScale, 0.0f, m_yellowBulletHandle, true, isLeft);
}

void Bullet::UpdateWave(Player& enemy, Bullet& otherBullet, Camera& camera)
{
	if (m_isShooting)
	{
		m_velocity.y *= 1.1f;
		//�ړ�
		m_pos += m_velocity;

		//�G�ɓ����������ǂ������`�F�b�N
		//����������̏�����CollisionCheck�����i1P����2P���ŏ����̏��ŗD�򂪂��Ȃ��悤�ɂ��邽�߁j
		m_isHitPlayer = CheckHitPlayer(enemy);

		//�e���m�ő��E
		if (CheckHitBullet(otherBullet) && otherBullet.GetIsShooting())
		{
			Disappear();
			otherBullet.Disappear();
		}
		//�ǂ̊O�ɂ����������
		if (camera.GetCameraLeftWallPos() - m_hitBoxAttack.x2 > m_pos.x || camera.GetCameraRightWallPos() - m_hitBoxAttack.x1 < m_pos.x)
		{
			Disappear();
		}
		//�A�j���[�V�����̃t���[���𐔂���
		if (m_animCountFrame % kBulletOneAnimFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
			if ((m_animIndex > m_finishAnimIndex))
			{
				m_animIndex = m_startAnimIndex;
			}
		}
	}
	else
	{
		//�A�j���[�V�����̃t���[���𐔂���
		if (m_animCountFrame % kDisappearOneAnimFrame == 0 && m_animCountFrame != 0)
		{
			m_animIndex++;
			//�A�j���[�V�����̐����ő�܂ōs�����Ƃ�
			if ((m_animIndex > m_finishAnimIndex))
			{
				m_animIndex = m_finishAnimIndex;
			}
		}
	}
}

void Bullet::DrawWave(Camera& camera)
{
	////�e
	//���ɐi��ł�Ȃ獶������
	bool isLeft = false;
	if (m_velocity.x < 0)
	{
		isLeft = true;
	}

	//�؂�����v�Z����
	int sizeX, sizeY;
	GetGraphSize(m_purpleBulletHandle, &sizeX, &sizeY);//�摜�T�C�Y
	int cutX = m_animIndex % (sizeX / kBulletWidth);//��
	int cutY = m_animIndex / (sizeX / kBulletHeight);//�c
	////���C��
	DrawRectRotaGraphFast(static_cast<int>(m_pos.x) + static_cast<int>(camera.m_drawOffset.x),
		static_cast<int>(m_pos.y) + static_cast<int>(camera.m_drawOffset.y),
		kBulletWidth * cutX,
		kBulletHeight * cutY,
		kBulletWidth, kBulletHeight,
		kBulletScale, 0.0f, m_purpleBulletHandle, true, isLeft);
}

Bullet::Bullet(PlayerIndex playerIndex):
	m_pos(),
	m_velocity(),
	m_playerIndex(playerIndex),
	m_isShooting(false),
	m_hitBoxAttack(-kHadouRadius,-kHadouRadius, kHadouRadius, kHadouRadius),
	m_isHitPlayer(false),
	m_giveNoActFrame(0),
	m_giveGuardFrame(0),
	m_giveDamage(0.0f),
	m_giveAttackVelo(5,0,0),
	m_animIndex(kStartHadouAnimIndex),
	m_startAnimIndex(kStartHadouAnimIndex),
	m_finishAnimIndex(kFinishHadouAnimIndex),
	m_animCountFrame(0),
	m_update(&Bullet::UpdateHadou),
	m_draw(&Bullet::DrawHadou),
	m_blueBulletHandle(LoadGraph("./img/Bullet/BlueBullet160x160.png")),//��
	m_yellowBulletHandle(LoadGraph("./img/Bullet/YellowBullet160x160.png")),//���F
	m_purpleBulletHandle(LoadGraph("./img/Bullet/PurpleBullet160x160.png"))//��
{
}

Bullet::~Bullet()
{
	DeleteGraph(m_blueBulletHandle);
	DeleteGraph(m_yellowBulletHandle);
	DeleteGraph(m_purpleBulletHandle);
}

void Bullet::Init()
{
	m_pos.x = 0;
	m_pos.y = 0;
	m_velocity.x = 0;
	m_velocity.y = 0;
	m_isShooting = false;
	InitHitBoxHadou();
	m_isHitPlayer = false;
}

void Bullet::SetShotEffect(float damage, int giveNoActFrame, int giveGuardFrame)
{
	//����ɗ^����e�����Z�b�g
	m_giveDamage = damage;
	m_giveNoActFrame = giveNoActFrame;
	m_giveGuardFrame = giveGuardFrame;
}

void Bullet::Update(Player& enemy, Bullet& otherBullet, Camera& camera)
{
	m_animCountFrame++;
	(this->*m_update)(enemy,otherBullet,camera);
}

void Bullet::Draw(Camera& camera)
{
	(this->*m_draw)(camera);
#if _DEBUG
	//�����蔻��
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	//�U������
	DxLib::DrawBox(
		static_cast<int>(m_pos.x + m_hitBoxAttack.x1 + camera.m_drawOffset.x),
		static_cast<int>(m_pos.y + m_hitBoxAttack.y1 + camera.m_drawOffset.y),
		static_cast<int>(m_pos.x + m_hitBoxAttack.x2 + camera.m_drawOffset.x),
		static_cast<int>(m_pos.y + m_hitBoxAttack.y2 + camera.m_drawOffset.y),
		0xff0000, true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 100);
#endif
}

void Bullet::Disappear()
{
	m_isShooting = false;
	ResetAttackBox();
	m_isHitPlayer = false;
	m_startAnimIndex = kStartDisappearAnimIndex;
	m_finishAnimIndex = kFinishDisappearAnimIndex;
	m_animIndex = m_startAnimIndex;
	m_animCountFrame = 0;
}

//�g����
void Bullet::LoadHadou(Player& player, float damage, int giveNoActFrame, int giveGuardFrame)
{
	//���łɒe�����˂���Ă���Ȃ���Ȃ�
	if (m_isShooting)return;
	//�g����
	m_startAnimIndex = kStartHadouAnimIndex;
	m_finishAnimIndex = kFinishHadouAnimIndex;
	m_animIndex = m_startAnimIndex;
	m_animCountFrame = 0;
	//�|�W�V�����ƃX�s�[�h�ƌ������Z�b�g
	m_pos.y = player.GetPos().y;
	m_velocity.x = 0;
	m_velocity.y = 0;
	if (player.GetDirState())
	{
		m_pos.x = player.GetPos().x - kBulletOffsetPos;
		if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
		{
			m_velocity.x = -kHadouVeloXLight;
		}
		else if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
		{
			m_velocity.x = -kHadouVeloXHigh;
		}
	}
	else
	{
		m_pos.x = player.GetPos().x + kBulletOffsetPos;
		if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
		{
			m_velocity.x = kHadouVeloXLight;
		}
		else if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
		{
			m_velocity.x = kHadouVeloXHigh;
		}
	}
	//�肩��o�������̂ňʒu����
	if (m_velocity.x < 0)
	{
		m_pos.x -= kHadouOffsetPosX;
	}
	else
	{
		m_pos.x += kHadouOffsetPosX;
	}
	//�����蔻�菉����
	InitHitBoxHadou();
	//����ɗ^�������
	SetShotEffect(damage, giveNoActFrame, giveGuardFrame);
	if (m_update != &Bullet::UpdateHadou || m_draw != &Bullet::DrawHadou)
	{
		m_update = &Bullet::UpdateHadou;
		m_draw = &Bullet::DrawHadou;
	}
}
//�\�j�b�N�u�[��
void Bullet::LoadSonic(Player& player, float damage, int giveNoActFrame, int giveGuardFrame)
{
	//���łɒe�����˂���Ă���Ȃ���Ȃ�
	if (m_isShooting)return;
	//�\�j�b�N�u�[��
	m_startAnimIndex = kStartSonicAnimIndex;
	m_finishAnimIndex = kFinishSonicAnimIndex;
	m_animIndex = m_startAnimIndex;
	m_animCountFrame = 0;
	//�|�W�V�����ƃX�s�[�h�ƌ������Z�b�g
	m_pos.y = player.GetPos().y;
	m_velocity.x = 0;
	m_velocity.y = 0;
	if (player.GetDirState())
	{
		m_pos.x = player.GetPos().x - kSonicOffsetPosX;
		if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
		{
			m_velocity.x = -kSonicVeloXLight;
		}
		else if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
		{
			m_velocity.x = -kSonicVeloXHigh;
		}
	}
	else
	{
		m_pos.x = player.GetPos().x + kSonicOffsetPosX;
		if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
		{
			m_velocity.x = kSonicVeloXLight;
		}
		else if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
		{
			m_velocity.x = kSonicVeloXHigh;
		}
	}
	//������
	InitHitBoxSonic();
	//����ɗ^�������
	SetShotEffect(damage, giveNoActFrame, giveGuardFrame);
	if (m_update != &Bullet::UpdateSonic || m_draw != &Bullet::DrawSonic)
	{
		m_update = &Bullet::UpdateSonic;
		m_draw = &Bullet::DrawSonic;
	}
}
//�ŕ���
void Bullet::LoadWave(Player& player, float damage, int giveNoActFrame, int giveGuardFrame)
{
	//���łɒe�����˂���Ă���Ȃ���Ȃ�
	if (m_isShooting)return;
	//�ŕ���
	m_startAnimIndex = kStartPowerWaveAnimIndex;
	m_finishAnimIndex = kFinishPowerWaveAnimIndex;
	m_animIndex = m_startAnimIndex;
	m_animCountFrame = 0;
	//�|�W�V�����ƃX�s�[�h�ƌ������Z�b�g
	//�n�ʂ����点�����̂ňʒu����
	m_pos.y = player.GetPos().y + kPowerWaveOffsetPosY;
	m_velocity.x = 0;
	m_velocity.y = 0;
	if (player.GetDirState())
	{
		m_pos.x = player.GetPos().x - kBulletOffsetPos;
		if (player.GetAttackAttackTypes() == AttackTypes::LightKick)
		{
			m_velocity.x = -kWaveVeloXLight;
		}
		else if (player.GetAttackAttackTypes() == AttackTypes::HighKick)
		{
			m_velocity.x = -kWaveVeloXHigh;
			m_velocity.y = kWaveVeloYHigh;
		}
	}
	else
	{
		m_pos.x = player.GetPos().x + kBulletOffsetPos;
		if (player.GetAttackAttackTypes() == AttackTypes::LightKick)
		{
			m_velocity.x = kWaveVeloXLight;
		}
		else if (player.GetAttackAttackTypes() == AttackTypes::HighKick)
		{
			m_velocity.x = kWaveVeloXHigh;
			m_velocity.y = kWaveVeloYHigh;
		}
	}
	//������
	InitHitBoxWave();
	//����ɗ^�������
	SetShotEffect(damage, giveNoActFrame, giveGuardFrame);
	if (m_update != &Bullet::UpdateWave || m_draw != &Bullet::DrawWave)
	{
		m_update = &Bullet::UpdateWave;
		m_draw = &Bullet::DrawWave;
	}
}

bool Bullet::CheckHitPlayer(Player& enemy)
{
	//�U���̔��肪�Ȃ��Ȃ�`�F�b�N���Ȃ�
	if (m_hitBoxAttack.x1 == 0 &&
		m_hitBoxAttack.y1 == 0 &&
		m_hitBoxAttack.x2 == 0 &&
		m_hitBoxAttack.y2 == 0)	return false;



	float width;
	float height;
	bool isHit = false;
	if (enemy.GetDirState())
	{
		//�U������̒��S�����߂�
		Vector3 attackPos((m_pos.x + m_hitBoxAttack.x2 + m_pos.x + m_hitBoxAttack.x1) / 2,
			(m_pos.y + m_hitBoxAttack.y2 + m_pos.y + m_hitBoxAttack.y1) / 2, 0.0f);

		//�����蔻�肪����Ȃ�`�F�b�N
		//��
		if (enemy.GetHitBoxHead().x1 != 0 ||
			enemy.GetHitBoxHead().y1 != 0 ||
			enemy.GetHitBoxHead().x2 != 0 ||
			enemy.GetHitBoxHead().y2 != 0)
		{
			//�G�̓��̂��ꔻ��////////////////////////////////////////
			Vector3 enemyHead((enemy.GetPos().x - enemy.GetHitBoxHead().x2 + enemy.GetPos().x - enemy.GetHitBoxHead().x1) / 2,
				(enemy.GetPos().y + enemy.GetHitBoxHead().y2 + enemy.GetPos().y + enemy.GetHitBoxHead().y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyHead.x);
			height = std::abs(attackPos.y - enemyHead.y);

			//X�����̍U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.GetHitBoxHead().x2 - enemy.GetHitBoxHead().x1) / 2) &&
				//Y�����U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.GetHitBoxHead().y2 - enemy.GetHitBoxHead().y1) / 2)) return true;
			/////////////////////////////////////////////////////////////
		}
	

		//�����蔻�肪����Ȃ�`�F�b�N
		//��
		if (enemy.GetHitBoxBody().x1 != 0 ||
			enemy.GetHitBoxBody().y1 != 0 ||
			enemy.GetHitBoxBody().x2 != 0 ||
			enemy.GetHitBoxBody().y2 != 0)
		{
			//�G�̑̂̂��ꔻ��////////////////////////////////////////////
			Vector3 enemyBody((enemy.GetPos().x - enemy.GetHitBoxBody().x2 + enemy.GetPos().x - enemy.GetHitBoxBody().x1) / 2,
				(enemy.GetPos().y + enemy.GetHitBoxBody().y2 + enemy.GetPos().y + enemy.GetHitBoxBody().y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyBody.x);
			height = std::abs(attackPos.y - enemyBody.y);

			//X�����̍U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.GetHitBoxBody().x2 - enemy.GetHitBoxBody().x1) / 2) &&
				//Y�����U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.GetHitBoxBody().y2 - enemy.GetHitBoxBody().y1) / 2)) return true;
			///////////////////////////////////////////////////////////////
		}
		
		//�����蔻�肪����Ȃ�`�F�b�N
		//��
		if (enemy.GetHitBoxLeg().x1 != 0 ||
			enemy.GetHitBoxLeg().y1 != 0 ||
			enemy.GetHitBoxLeg().x2 != 0 ||
			enemy.GetHitBoxLeg().y2 != 0)
		{
			//�G�̑��̂��ꔻ��
			Vector3 enemyLeg((enemy.GetPos().x - enemy.GetHitBoxLeg().x2 + enemy.GetPos().x - enemy.GetHitBoxLeg().x1) / 2,
				(enemy.GetPos().y + enemy.GetHitBoxLeg().y2 + enemy.GetPos().y + enemy.GetHitBoxLeg().y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyLeg.x);
			height = std::abs(attackPos.y - enemyLeg.y);

			//X�����̍U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.GetHitBoxLeg().x2 - enemy.GetHitBoxLeg().x1) / 2) &&
				//Y�����U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.GetHitBoxLeg().y2 - enemy.GetHitBoxLeg().y1) / 2)) return true;
		}
		
	}
	else
	{
		//�U������̒��S�����߂�
		Vector3 attackPos((m_pos.x - m_hitBoxAttack.x2 + m_pos.x - m_hitBoxAttack.x1) / 2,
			(m_pos.y + m_hitBoxAttack.y2 + m_pos.y + m_hitBoxAttack.y1) / 2, 0.0f);

		//��
		if (enemy.GetHitBoxHead().x1 != 0 ||
			enemy.GetHitBoxHead().y1 != 0 ||
			enemy.GetHitBoxHead().x2 != 0 ||
			enemy.GetHitBoxHead().y2 != 0)
		{
			//�G�̓��̂��ꔻ��////////////////////////////////////////
			Vector3 enemyHead((enemy.GetPos().x + enemy.GetHitBoxHead().x2 + enemy.GetPos().x + enemy.GetHitBoxHead().x1) / 2,
				(enemy.GetPos().y + enemy.GetHitBoxHead().y2 + enemy.GetPos().y + enemy.GetHitBoxHead().y1) / 2, 0.0f);

			width = std::abs(attackPos.x - enemyHead.x);
			height = std::abs(attackPos.y - enemyHead.y);

			//X�����̍U���̕��Ɠ��̕��𑫂����l��菬�����Ȃ瓖�����Ă�
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.GetHitBoxHead().x2 - enemy.GetHitBoxHead().x1) / 2) &&
				//X�����̍U���̕��Ɠ��̕��𑫂����l��菬�����Ȃ瓖�����Ă�
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.GetHitBoxHead().y2 - enemy.GetHitBoxHead().y1) / 2)) return true;
			////////////////////////////////////////////////////////////
		}
	

		//��
		if (enemy.GetHitBoxBody().x1 != 0 ||
			enemy.GetHitBoxBody().y1 != 0 ||
			enemy.GetHitBoxBody().x2 != 0 ||
			enemy.GetHitBoxBody().y2 != 0)
		{
			//�G�̑̂̂��ꔻ��///////////////////////////////////////
			Vector3 enemyBody((enemy.GetPos().x + enemy.GetHitBoxBody().x2 + enemy.GetPos().x + enemy.GetHitBoxBody().x1) / 2,
				(enemy.GetPos().y + enemy.GetHitBoxBody().y2 + enemy.GetPos().y + enemy.GetHitBoxBody().y1) / 2, 0.0f);

			width = std::abs(attackPos.x - enemyBody.x);
			height = std::abs(attackPos.y - enemyBody.y);

			//X�����̍U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.GetHitBoxBody().x2 - enemy.GetHitBoxBody().x1) / 2) &&
				//Y�����U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.GetHitBoxBody().y2 - enemy.GetHitBoxBody().y1) / 2)) return true;
			/////////////////////////////////////////////////////////
		}
	


		//��
		if (enemy.GetHitBoxLeg().x1 != 0 ||
			enemy.GetHitBoxLeg().y1 != 0 ||
			enemy.GetHitBoxLeg().x2 != 0 ||
			enemy.GetHitBoxLeg().y2 != 0)
		{
			//�G�̑��̂��ꔻ��
			Vector3 enemyLeg((enemy.GetPos().x + enemy.GetHitBoxLeg().x2 + enemy.GetPos().x + enemy.GetHitBoxLeg().x1) / 2,
				(enemy.GetPos().y + enemy.GetHitBoxLeg().y2 + enemy.GetPos().y + enemy.GetHitBoxLeg().y1) / 2, 0.0f);
			width = std::abs(attackPos.x - enemyLeg.x);
			height = std::abs(attackPos.y - enemyLeg.y);

			//X�����̍U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
			if (width < std::abs((m_hitBoxAttack.x2 - m_hitBoxAttack.x1) / 2) + std::abs((enemy.GetHitBoxLeg().x2 - enemy.GetHitBoxLeg().x1) / 2) &&
				//Y�����U���̕��Ƒ̂̕��𑫂����l��菬�����Ȃ瓖�����Ă�
				height < std::abs((m_hitBoxAttack.y2 - m_hitBoxAttack.y1) / 2) + std::abs((enemy.GetHitBoxLeg().y2 - enemy.GetHitBoxLeg().y1) / 2)) return true;
		}
	
	}



	return false;
}

bool Bullet::CheckHitBullet(Bullet& otherBullet)
{

	//X�AY�����̗���Ă��鋗�����Βl�����Ď擾
	float width = std::abs(m_pos.x - otherBullet.GetPos().x);
	float height = std::abs(m_pos.y - otherBullet.GetPos().y);

	//X�����̋������L�����̕����傫����Γ������Ă��Ȃ�
	if (width > std::abs(GetHitBoxAttack().x1 - otherBullet.GetHitBoxAttack().x2))	return false;
	//Y�����̋������L�����̍������傫����Γ������Ă��Ȃ�
	if (height > std::abs(GetHitBoxAttack().y1 - otherBullet.GetHitBoxAttack().y2))	return false;

	//�����܂ł���Γ������Ă�
	return true;
}

void Bullet::InitHitBoxHadou()
{
	//�U���͈�
	m_hitBoxAttack.x1 = -kHadouRadius;
	m_hitBoxAttack.y1 = -kHadouRadius;
	m_hitBoxAttack.x2 = kHadouRadius;
	m_hitBoxAttack.y2 = kHadouRadius;
}

void Bullet::InitHitBoxSonic()
{
	//�U���͈�
	m_hitBoxAttack.x1 = -kSonicRadius;
	m_hitBoxAttack.y1 = -kSonicRadius;
	m_hitBoxAttack.x2 = kSonicRadius;
	m_hitBoxAttack.y2 = kSonicRadius;
}

void Bullet::InitHitBoxWave()
{
	//�U���͈�
	m_hitBoxAttack.x1 = -kPowerWaveRadiusX;
	m_hitBoxAttack.y1 = -kPowerWaveRadiusY;
	m_hitBoxAttack.x2 = kPowerWaveRadiusX;
	m_hitBoxAttack.y2 = kPowerWaveRadiusY;
}

