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
	//�U�������Ƃ��ɗ^�����
	constexpr float kGiveVeloPowerX = 20.0f;
	constexpr float kGiveVeloPowerY = -30.0f;

	//�ǂɓ������Ă�G���������Ƃ��̃m�b�N�o�b�N
	constexpr float kReflectionMagin = 10.0f;

	//���y����
	constexpr float kScrapMagin = 0.25f;

	//se�{�����[��
	constexpr int kSeVolume = 150;

	//�K�[�h�����Ƃ��̂����肪����悤�ɂ���
	constexpr float kKnockbackMagin = 0.25f;
}


Vector3 CollisionCheck::CreateHitEffectPosPtoP(Player& attacker, Player& defender)
{
	//�q�b�g�G�t�F�N�g�̃|�W�V����
	Vector3 hitEffectPos;
	
	//�U������̒��S�����߂�
	Vector3 attackPos;
	//�����ɂ���Ĉʒu���ς��̂�
	if (!attacker.GetDirState())
	{
		attackPos.x = (attacker.GetPos().x + attacker.GetHitBoxAttack().x2 + attacker.GetPos().x + attacker.GetHitBoxAttack().x1) / 2;
		attackPos.y = (attacker.GetPos().y + attacker.GetHitBoxAttack().y2 + attacker.GetPos().y + attacker.GetHitBoxAttack().y1) / 2;
	}
	else
	{
		//�U������̒��S�����߂�
		attackPos.x = (attacker.GetPos().x - attacker.GetHitBoxAttack().x2 + attacker.GetPos().x - attacker.GetHitBoxAttack().x1) / 2;
		attackPos.y = (attacker.GetPos().y + attacker.GetHitBoxAttack().y2 + attacker.GetPos().y + attacker.GetHitBoxAttack().y1) / 2;
	}
	
	//�U����H�炤���̃|�W�V����
	Vector3 defenderPos = defender.GetPos();
	//�U���Ƌ�炤���̊Ԃ̈ʒu
	hitEffectPos.x = (attackPos.x + defenderPos.x) / 2;
	//�����͂��݂����n��̏ꍇ�͍U����Y���W�ɍ��킹���ق���
	//��a�����Ȃ������̂Œn�ォ�󒆂��ŏ�������
	if (attacker.IsGround())
	{
		//�n��
		hitEffectPos.y = attackPos.y;
	}
	else
	{
		//��
		hitEffectPos.y = (attackPos.y + defenderPos.y) / 2;
	}


	return hitEffectPos;
}

Vector3 CollisionCheck::CreateHitEffectPosBtoP(Bullet& bullet, Player& defender)
{
	//�q�b�g�G�t�F�N�g�̃|�W�V����
	Vector3 hitEffectPos;

	//�e�̍��W
	Vector3 attackPos = bullet.GetPos();
	//�G�̍��W
	Vector3 defenderPos = defender.GetPos();
	//�U���Ƌ�炤���̊Ԃ̈ʒu
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

//������������
bool CollisionCheck::ColCheckPlayer(Player& p1, Player& p2)
{
	//���̈ړ�����擾
	Vector3 nextP1 = p1.GetPos() + p1.GetVelo();
	Vector3 nextP2 = p2.GetPos() + p2.GetVelo();

	//X�AY�����̗���Ă��鋗�����Βl�����Ď擾
	float width = std::abs(nextP1.x - nextP2.x);
	float height = std::abs(nextP1.y - nextP2.y);

	//X�����̋������L�����̕����傫����Γ������Ă��Ȃ�
	if (width > p1.GetPushBox().x1 + p2.GetPushBox().x2)	return false;
	//Y�����̋������L�����̍������傫����Γ������Ă��Ȃ�
	if (height > p1.GetPushBox().y1 + p2.GetPushBox().y2)	return false;

	//�����܂ł���Γ������Ă�
	return true;
}

bool CollisionCheck::ColCheckWall(Player& player, Camera& camera)
{
	//���̈ړ�����擾
	Vector3 nextPos = player.GetPos() + player.GetVelo();
	float playerWidth = std::abs(player.GetPushBox().x1) + std::abs(player.GetPushBox().x2);

	//�J�����͈͓̔��Ȃ瓖�����ĂȂ�
	if ((camera.m_pos.x - Game::kScreenWidth / 2) <= nextPos.x - playerWidth / 2 &&
		(camera.m_pos.x + Game::kScreenWidth / 2) >= nextPos.x + playerWidth / 2)	return false;


	//�����܂ł���Γ������Ă�
	return true;
}

//�E�̕�
bool CollisionCheck::ColCheckRightWall(Player& player, Camera& camera)
{
	//���̈ړ�����擾
	Vector3 nextPos = player.GetPos() + player.GetVelo();
	float playerWidth = std::abs(player.GetPushBox().x1) + std::abs(player.GetPushBox().x2);

	
	if ((camera.m_pos.x + Game::kScreenWidth / 2) < nextPos.x + playerWidth / 2)	return true;


	//�����܂ł���Γ������ĂȂ�
	return false;
}

bool CollisionCheck::ColCheckLeftWall(Player& player, Camera& camera)
{
	//���̈ړ�����擾
	Vector3 nextPos = player.GetPos() + player.GetVelo();
	float playerWidth = std::abs(player.GetPushBox().x1) + std::abs(player.GetPushBox().x2);


	if ((camera.m_pos.x - Game::kScreenWidth / 2) > nextPos.x - playerWidth / 2)	return true;


	//�����܂ł���Γ������ĂȂ�
	return false;
}

//�ǂɓ����������̏���
Vector3 CollisionCheck::WallProcess(Player& player, Camera& camera)
{
	Vector3 nextPos = player.GetPos() + player.GetVelo();;
	DrawString(300, 500, "�J�x", 0xffffff, true);
	float playerWidth = std::abs(player.GetPushBox().x1) + std::abs(player.GetPushBox().x2);
	//���̕ǂȂ�
	if (ColCheckLeftWall(player, camera))
	{
		nextPos.x = camera.m_pos.x - Game::kScreenWidth / 2 + playerWidth / 2;
	}
	//�E�̕ǂȂ�
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

	//���Ɉړ�������W���o��
	Vector3 p1NextPos = nextP1.GetPos() + nextP1.GetVelo();
	Vector3 p2NextPos = nextP2.GetPos() + nextP2.GetVelo();
	//�d�Ȃ��Ă镔�����o��
	float overlap = 0;

	m_lastVeloP1 = nextP1.GetVelo();
	m_lastVeloP2 = nextP2.GetVelo();

	//����Ɠ������Ă��邩���`�F�b�N
	if (ColCheckPlayer(nextP1, nextP2))
	{
		//�ǂɓ������Ă��瑬�x��0�ɂ���
		if (ColCheckWall(nextP1, camera))
		{
			nextP1.SetVeloX(0);
		}
		else if (ColCheckWall(nextP2, camera))
		{
			nextP2.SetVeloX(0);
		}


		//P1���������Ă���Ȃ�
		if (nextP1.GetVelo().Length() != 0 && nextP2.GetVelo().Length() == 0)
		{
			//���ɂ��炷
			if (p1NextPos.x < p2NextPos.x)
			{
				if (!ColCheckWall(nextP1, camera))
				{
					nextP2.SetVeloX(nextP1.GetVelo().x / 2);//�v���C���[�̑��x�̔����̗͂ŉ���
					p2NextPos = nextP2.GetPos() + nextP2.GetVelo();
				}


				overlap = (nextP1.GetPushBox().x1 + nextP2.GetPushBox().x2) - std::abs(p1NextPos.x - p2NextPos.x);
				nextP1.SetVeloX(nextP1.GetVelo().x - overlap);
				p1NextPos.x = nextP1.GetPos().x + nextP1.GetVelo().x - 0.000001f;
			}
			//�E�ɂ��炷
			else if (p1NextPos.x > p2NextPos.x)
			{
				if (!ColCheckWall(nextP1, camera))
				{
					nextP2.SetVeloX(nextP1.GetVelo().x / 2);//�v���C���[�̑��x�̔����̗͂ŉ���
					p2NextPos = nextP2.GetPos() + nextP2.GetVelo();
				}

				overlap = (nextP2.GetPushBox().x1 + nextP1.GetPushBox().x2) - std::abs(p1NextPos.x - p2NextPos.x);
				nextP1.SetVeloX(nextP1.GetVelo().x + overlap);
				p1NextPos.x = nextP1.GetPos().x + nextP1.GetVelo().x + 0.000001f;
			}

			//�����ʒu�̎�
			if (p1NextPos.x == p2NextPos.x)
			{
				//���������Ă���Ȃ獶�ɂ����
				if (nextP1.GetDirState())
				{
					p1NextPos.x = nextP1.GetPos().x - (nextP2.GetPushBox().x1 + nextP2.GetPushBox().x2);
				}
				//�E�Ȃ�E
				else
				{
					p1NextPos.x = nextP1.GetPos().x + (nextP2.GetPushBox().x1 + nextP2.GetPushBox().x2);
				}
			}
		}
		//P2���������Ă���Ȃ�
		else if (nextP1.GetVelo().Length() == 0 && nextP2.GetVelo().Length() != 0)
		{
			//���ɂ��炷
			if (p2NextPos.x < p1NextPos.x)
			{
				if (!ColCheckWall(nextP2, camera))
				{
					nextP1.SetVeloX(nextP2.GetVelo().x / 2);//�v���C���[�̑��x�̔����̗͂ŉ���
					p1NextPos = nextP1.GetPos() + nextP1.GetVelo();
				}
				overlap = (nextP1.GetPushBox().x1 + nextP2.GetPushBox().x2) - std::abs(p1NextPos.x - p2NextPos.x);
				nextP2.SetVeloX(nextP2.GetVelo().x - overlap);
				p2NextPos.x = nextP2.GetPos().x + nextP2.GetVelo().x - 0.000001f;

			}
			//�E�ɂ��炷
			else if (p2NextPos.x > p1NextPos.x)
			{
				if (!ColCheckWall(nextP2, camera))
				{
					nextP1.SetVeloX(nextP2.GetVelo().x / 2);//�v���C���[�̑��x�̔����̗͂ŉ���
					p1NextPos = nextP1.GetPos() + nextP1.GetVelo();
				}

				overlap = (nextP2.GetPushBox().x1 + nextP1.GetPushBox().x2) - std::abs(p1NextPos.x - p2NextPos.x);
				nextP2.SetVeloX(nextP2.GetVelo().x + overlap);
				p2NextPos.x = nextP2.GetPos().x + nextP2.GetVelo().x + 0.000001f;
			}

			//�����ʒu�̎�
			if (p1NextPos.x == p2NextPos.x)
			{
				//���������Ă���Ȃ獶�ɂ����
				if (nextP2.GetDirState())
				{
					p2NextPos.x = nextP2.GetPos().x - (nextP1.GetPushBox().x1 + nextP1.GetPushBox().x2);
				}
				//�E�Ȃ�E
				else
				{
					p2NextPos.x = nextP2.GetPos().x + (nextP1.GetPushBox().x1 + nextP1.GetPushBox().x2);
				}
			}
		}
		//�����������Ă���Ƃ��܂��͓����Ă��Ȃ��Ƃ�
		else
		{
			//��
			if (p1NextPos.x < p2NextPos.x)
			{
				overlap = ((nextP1.GetPushBox().x1 + nextP2.GetPushBox().x2) - std::abs(p1NextPos.x - p2NextPos.x)) / 2;
				nextP1.SetVeloX(nextP1.GetVelo().x - overlap);
				p1NextPos.x = nextP1.GetPos().x + nextP1.GetVelo().x - 0.0000001f;
				nextP2.SetVeloX(nextP2.GetVelo().x + overlap);
				p2NextPos.x = nextP2.GetPos().x + nextP2.GetVelo().x + 0.0000001f;
			}
			//�E
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

	//�ǂ��炩����ł��ǂɓ������������`�F�b�N
	bool isHitWall = false;
	//�v���C���[�������Ă�����ǂƂ̓����蔻�������
	if (p1.GetHp() > 0)
	{
		//�ǂƂ̓����蔻��
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
	//��������l���ǂɓ������ĂԂ������Ƃ��ɂ��Ȃ�X���W�ɂȂ����ۂ̏���
	if (nextP1.GetPos().x == nextP2.GetPos().x && isHitWall && ColCheckPlayer(nextP1, nextP2))
	{
		//P1��P2�̏�ɂ���ꍇ
		if (nextP1.GetPos().y < nextP2.GetPos().y)
		{
			//�E�̕ǂȂ獶�ɂ��炷
			if (ColCheckRightWall(nextP1, camera))
			{
				p1NextPos.x = nextP1.GetPos().x - (nextP2.GetPushBox().x1 + nextP2.GetPushBox().x2);
			}
			//���Ȃ獶��
			if (ColCheckLeftWall(nextP1, camera))
			{
				p1NextPos.x = nextP1.GetPos().x + (nextP2.GetPushBox().x1 + nextP2.GetPushBox().x2);
			}
		}
		//P2��P1�̏�ɂ���Ƃ�
		if (nextP1.GetPos().y > nextP2.GetPos().y)
		{
			//�E�̕ǂȂ獶�ɂ��炷
			if (ColCheckRightWall(nextP2, camera))
			{
				p2NextPos.x = nextP2.GetPos().x - (nextP1.GetPushBox().x1 + nextP1.GetPushBox().x2);
			}
			//���Ȃ獶��
			if (ColCheckLeftWall(nextP2, camera))
			{
				p2NextPos.x = nextP2.GetPos().x + (nextP1.GetPushBox().x1 + nextP1.GetPushBox().x2);
			}
		}
	}
	//�ړ�
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
	
	//�U�����������Ă���΃v���C���[�̈ړ��ʂ�������
	if (p1.IsHitAttack() || bulletP1.GetIsHitPlayer())
	{
		p2.SetVeloX(0);
		p2.SetVeloY(0);
		//�`���O��
		gameManager.SetIsDrawFrontP1(true);
	}
	if (p2.IsHitAttack() || bulletP2.GetIsHitPlayer())
	{
		p1.SetVeloX(0);
		p1.SetVeloY(0);
		//�`�������
		gameManager.SetIsDrawFrontP1(false);
	}
	//�U���ɂ��ړ��x�N
	Vector3 giveVeloP1(p2.GetGiveAttackVelo());
	Vector3 giveVeloP2(p1.GetGiveAttackVelo());

	//�����ƍU�����d�Ȃ��Ă��Ȃ��ꍇ�����͐�������
	if (!p1.IsHitAttack() && !bulletP1.GetIsHitPlayer() && !p2.IsHitAttack() && !bulletP2.GetIsHitPlayer())
	{
		//�����ʂ�
		if (p1.IsHitGrasp() && p2.IsHitGrasp() || p1.IsThrowSuccess() && p2.IsThrowSuccess())
		{
			p1.LoadStateThrowEscape();
			p2.LoadStateThrowEscape();
			p1.OffIsHitGrasp();
			p2.OffIsHitGrasp();
		}
		//P1�̓��������������Ƃ�
		else if (p1.IsHitGrasp() && !p2.IsHitGrasp())
		{
			//�`�������
			//������Ƃ��͌��ɂ����ق������h��������
			gameManager.SetIsDrawFrontP1(false);
			//��������Ԃɂ���
			p2.LoadStateBeThrown();
			//�������Ԃɂ���
			p1.OffIsHitGrasp();
			p1.OnIsThrownSuccess();;
		}
		//P2�̓��������������Ƃ�
		else if (p2.IsHitGrasp() && !p1.IsHitGrasp())
		{
			//�`���O��
			gameManager.SetIsDrawFrontP1(true);
			p1.LoadStateBeThrown();
			p2.OffIsHitGrasp();
			p2.OnIsThrownSuccess();
		}
	}

	//�e�ɂ��U���̏���
	//P1�̒e��P2�ɓ���������
	if (bulletP1.GetIsHitPlayer())
	{
		//�q�b�g�X�g�b�v
		gameManager.OnIsHitStop();
		giveVeloP2.x = bulletP1.GetGiveAttackVelo().x;
		giveVeloP2.y = 0;
		//�K�[�h���Ă���
		if (p2.IsGuard())
		{
			//���肪�����Ȃ甽�]
			//�v���C���[�̍U���Ɠ����ɓ������Ă���ꍇ
			//�Ō�̍U���ɂ���ăx�N�g���Ɍ������m�肳���Ăق���
			if ((p2.GetPos().x < p1.GetPos().x) && !p1.IsHitAttack())
			{
				giveVeloP2.x *= -1;
			}
			p2.SetKnockback(giveVeloP2 * kKnockbackMagin);
			p2.SetVelo(giveVeloP2 * kKnockbackMagin);//��������炷
			p2.SetHp(p2.GetHp() - (bulletP1.GetGiveDamage() * kScrapMagin));//�_���[�W
			p2.SetGuardFrame(bulletP1.GetGiveGuardFrame());//�K�[�h�d��

			//�����K�[�h
			if (!p2.IsSquat())
			{
				p2.LoadStateGuardStand();//�K�[�h���[�V����
			}
			//���Ⴊ�݃K�[�h
			else
			{
				p2.LoadStateGuardSquat();//�K�[�h���[�V����
			}

			//�K�[�h�G�t�F�N�g
			gameManager.LoadScrapeGuardEffect(p1);

			//�K�[�h�̉�
			m_seP1->Stop();
			m_seP1->SetSE(m_guardSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();
		}
		else
		{
			//�J�����h�炷
			gameManager.OnIsCameraShake();
			//�󒆂̓G�ɍU���𓖂Ă���
			if (!p2.IsGround())
			{
				if (giveVeloP2.y == 0)
				{
					giveVeloP2.y = kGiveVeloPowerY;
				}
				p2.LoadStateDownAerial();//�_�E�����[�V����
			}
			else
			{
				p2.LoadStateHit();//�q�b�g���[�V����
			}
			//���肪�����Ȃ甽�]
			//�v���C���[�̍U���Ɠ����ɓ������Ă���ꍇ
			//�Ō�̍U���ɂ���ăx�N�g���Ɍ������m�肳���Ăق���
			if ((p2.GetPos().x < p1.GetPos().x) && !p1.IsHitAttack())
			{
				giveVeloP2.x *= -1;
			}
			p2.SetKnockback(giveVeloP2);
			p2.SetVelo(giveVeloP2);//��������炷
			p2.SetHp(p2.GetHp() - bulletP1.GetGiveDamage());//�_���[�W
			p2.SetNoActFrame(bulletP1.GetGiveNoActFrame());//�d����

			//�q�b�g�G�t�F�N�g
			gameManager.LoadSpecialHitEffect(p1);

			//�q�b�g�̉�
			m_seP1->Stop();
			m_seP1->SetSE(m_hitSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();
			
		}
		//�G�t�F�N�g�̈ʒu
		gameManager.SetHitEffectPosP1(CreateHitEffectPosBtoP(bulletP1, p2));

		//�e���폜}
		bulletP1.Disappear();
	}
	//P2�̒e��P1�ɓ���������
	if (bulletP2.GetIsHitPlayer())
	{
		//�q�b�g�X�g�b�v
		gameManager.OnIsHitStop();
		giveVeloP1.x = bulletP2.GetGiveAttackVelo().x;;
		giveVeloP1.y = 0;
		//�K�[�h���Ă���
		if (p1.IsGuard())
		{
			//���肪�����Ȃ甽�]
			//�v���C���[�̍U���Ɠ����ɓ������Ă���ꍇ
			//�Ō�̍U���ɂ���ăx�N�g���Ɍ������m�肳���Ăق��� GetIsHitAttack()
			if ((p1.GetPos().x < p2.GetPos().x) && !p2.IsHitAttack())
			{
				giveVeloP1.x *= -1;
			}
			p1.SetKnockback(giveVeloP1* kKnockbackMagin);
			p1.SetVelo(giveVeloP1* kKnockbackMagin);//��������炷
			p1.SetHp(p1.GetHp() - (bulletP2.GetGiveDamage() * kScrapMagin));//�_���[�W
			p1.SetGuardFrame(bulletP2.GetGiveGuardFrame());//�K�[�h�d��
			//�����K�[�h
			if (!p1.IsSquat())
			{
				p1.LoadStateGuardStand();//�K�[�h���[�V����
			}
			//���Ⴊ�݃K�[�h
			else
			{
				p1.LoadStateGuardSquat();//�K�[�h���[�V����
			}

			//�K�[�h�G�t�F�N�g
			gameManager.LoadScrapeGuardEffect(p2);

			//�K�[�h�̉�
			m_seP2->Stop();
			m_seP2->SetSE(m_guardSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();
		}
		else
		{
			//�J�����h�炷
			gameManager.OnIsCameraShake();
			//�󒆂̓G�ɍU���𓖂Ă���
			if (!p1.IsGround())
			{
				if (giveVeloP1.y == 0)
				{
					giveVeloP1.y = kGiveVeloPowerY;
				}
				p1.LoadStateDownAerial();//�_�E�����[�V����
			}
			else
			{
				p1.LoadStateHit();//�q�b�g���[�V����
			}
			//���肪�����Ȃ甽�]
			//�v���C���[�̍U���Ɠ����ɓ������Ă���ꍇ
			//�Ō�̍U���ɂ���ăx�N�g���Ɍ������m�肳���Ăق��� GetIsHitAttack()
			if ((p1.GetPos().x < p2.GetPos().x) && !p2.IsHitAttack())
			{
				giveVeloP1.x *= -1;
			}
			p1.SetKnockback(giveVeloP1);
			p1.SetVelo(giveVeloP1);//��������炷
			p1.SetHp(p1.GetHp() - bulletP2.GetGiveDamage());//�_���[�W
			p1.SetNoActFrame(bulletP2.GetGiveNoActFrame());//�d����

			//�q�b�g�G�t�F�N�g
			gameManager.LoadSpecialHitEffect(p2);

			//�q�b�g�̉�
			m_seP2->Stop();
			m_seP2->SetSE(m_hitSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();
		}

		//�G�t�F�N�g�̈ʒu
		gameManager.SetHitEffectPosP2(CreateHitEffectPosBtoP(bulletP2, p1));
		
		//�e���폜}
		bulletP2.Disappear();
	}

	//�����ɉ�������
	if (p1.IsHitAttack() && p2.IsHitAttack())
	{
		//�J�����h�炷
		gameManager.OnIsCameraShake();
		//�󒆂̓G�ɍU���𓖂Ă���
		if (!p2.IsGround())
		{
			if (giveVeloP2.y == 0)
			{
				giveVeloP2.y = kGiveVeloPowerY;
			}
		}
		//���肪�����Ȃ甽�]
		if ((p2.GetPos().x < p1.GetPos().x))
		{
			giveVeloP2.x *= -1;
		}
		//0�̏ꍇ�̓_�E�������U��
		if (p1.GetGiveNoActFrame() == 0)
		{
			//�Ƃ�܋󒆃_�E���ɂ��Ƃ��Βn��Ȃ�n��_�E���ɂȂ�悤�ɂ��Ă邩��OK
			p2.LoadStateDownAerial();
			
			if (giveVeloP2.y == 0)
			{
				giveVeloP2.y = kGiveVeloPowerY * 0.5f;
			}
		}
		else
		{
			p2.SetNoActFrame(p1.GetGiveNoActFrame());//�d����
			p2.LoadStateHit();//�q�b�g���[�V����
		}

		p2.SetKnockback(giveVeloP2);
		p2.SetVelo(giveVeloP2);//��������炷


		//�󒆂̓G�ɍU���𓖂Ă���
		if (!p1.IsGround())
		{
			if (giveVeloP1.y == 0)
			{
				giveVeloP1.y = kGiveVeloPowerY;
			}
			
		}
		//���肪�����Ȃ甽�]
		if ((p1.GetPos().x < p2.GetPos().x))
		{
			giveVeloP1.x *= -1;
		}
		//0�̏ꍇ�̓_�E�������U��
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
			p1.SetNoActFrame(p2.GetGiveNoActFrame());//�d����
			p1.LoadStateHit();//�q�b�g���[�V����
		}
		p1.SetKnockback(giveVeloP1);
		p1.SetVelo(giveVeloP1);//��������炷


		//�_���[�W
		p1.SetHp(p1.GetHp() - p2.GetGiveDamage());
		p2.SetHp(p2.GetHp() - p1.GetGiveDamage());

		//�q�b�g�G�t�F�N�g
		gameManager.LoadNormalHitEffect(p1);
		gameManager.LoadNormalHitEffect(p2);
		//�G�t�F�N�g�̈ʒu
		gameManager.SetHitEffectPosP1(CreateHitEffectPosPtoP(p1, p2));
		gameManager.SetHitEffectPosP2(CreateHitEffectPosPtoP(p2, p1));
		//�U�����������
		p1.ResetAttackBox();
		p2.ResetAttackBox();


		//�q�b�g�̉�
		m_seP1->Stop();
		m_seP1->SetSE(m_hitSeHandle);
		m_seP1->Volume(kSeVolume);
		m_seP1->PlayOnce();
		//�q�b�g�̉�
		m_seP2->Stop();
		m_seP2->SetSE(m_hitSeHandle);
		m_seP2->Volume(kSeVolume);
		m_seP2->PlayOnce();
	}
	//P1���U���𓖂Ă���
	else if (p1.IsHitAttack() && !p2.IsHitAttack())
	{

		//�U��������������
		if (CheckSuccessAttack(p1,p2))
		{
			//�J�����h�炷
			gameManager.OnIsCameraShake();
			//�q�b�g�G�t�F�N�g
			if (p1.IsCommand())
			{
				gameManager.LoadSpecialHitEffect(p1);
			}
			else
			{
				gameManager.LoadNormalHitEffect(p1);
			}

			//�󒆂̓G�ɍU���𓖂Ă���
			if (!p2.IsGround())
			{
				if (giveVeloP2.y == 0)
				{
					giveVeloP2.y = kGiveVeloPowerY;
				}
				giveVeloP2.x = kGiveVeloPowerX;
			}
			//���肪�����Ȃ甽�]
			//���̕ǂɓ������Ă邩���`�F�b�N����͍̂��̕ǂɓ������Ă���G��
			//�󒆍U���𓖂Ă�Ɠ��������v���C���[���E�ɍs���Ă��܂��󂢂��X�y�[�X��
			//���Ă��v���C���[�������Ă��܂��̂ł����h�����߂ɒǉ���������
			if ((p2.GetPos().x < p1.GetPos().x) || ColCheckLeftWall(p1, camera))
			{
				giveVeloP2.x *= -1;
			}

			//0�̏ꍇ�̓_�E�������U��
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
				p2.SetNoActFrame(p1.GetGiveNoActFrame());//�d����
				p2.LoadStateHit();//�q�b�g���[�V����
			}
			p2.SetKnockback(giveVeloP2);
			p2.SetVelo(giveVeloP2);//��������炷
			p2.SetHp(p2.GetHp() - p1.GetGiveDamage());//�_���[�W

			//�q�b�g�̉�
			m_seP2->Stop();
			m_seP2->SetSE(m_hitSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();
		}
		else//�K�[�h
		{
			giveVeloP2.y = 0;
			//���肪�����Ȃ甽�]
			if ((p2.GetPos().x < p1.GetPos().x) || ColCheckLeftWall(p1, camera))
			{
				giveVeloP2.x *= -1;
			}

			//���̍U�����R�}���h�̋Z�Ȃ����
			if (p1.IsCommand())
			{
				p2.SetHp(p2.GetHp() - (p1.GetGiveDamage() * kScrapMagin));//�_���[�W
				//���
				gameManager.LoadScrapeGuardEffect(p1);
			}
			else
			{
				//�K�[�h�G�t�F�N�g
				gameManager.LoadGuardEffect(p1);
			}

			p2.SetKnockback(giveVeloP2* kKnockbackMagin);//��������炷
			p2.SetVelo(giveVeloP2* kKnockbackMagin);
			p2.SetGuardFrame(p1.GetGiveGuardFrame());//�K�[�h�d��

			//�����K�[�h
			if (!p2.IsSquat())
			{
				p2.LoadStateGuardStand();//�K�[�h���[�V����
			}
			//���Ⴊ�݃K�[�h
			else
			{
				p2.LoadStateGuardSquat();//�K�[�h���[�V����
			}

			//�q�b�g�̉�
			m_seP2->Stop();
			m_seP2->SetSE(m_guardSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();
		}
	

		//�ǂɓ������Ă�G���U�������Ƃ�
		if (ColCheckWall(p2, camera))
		{
			//���肶��Ȃ��Ď������̂�����
			giveVeloP2.y = 0;
			giveVeloP2.x *= -1;
			p1.SetKnockback(giveVeloP2);
			p1.SetVelo(giveVeloP2);
		}

		//�G�t�F�N�g�̈ʒu
		gameManager.SetHitEffectPosP1(CreateHitEffectPosPtoP(p1, p2));
		p1.ResetAttackBox();	
	}
	//P2���U���𓖂Ă���
	else if (!p1.IsHitAttack() && p2.IsHitAttack())
	{
		//�U��������������
		if (CheckSuccessAttack(p2, p1))
		{
			//�J�����h�炷
			gameManager.OnIsCameraShake();
			//�q�b�g�G�t�F�N�g
			if (p2.IsCommand())
			{
				gameManager.LoadSpecialHitEffect(p2);
			}
			else
			{
				gameManager.LoadNormalHitEffect(p2);
			}
			//�󒆂̓G�ɍU���𓖂Ă���
			if (!p1.IsGround())
			{
				if (giveVeloP1.y == 0)
				{
					giveVeloP1.y = kGiveVeloPowerY;
				}
				giveVeloP1.x = kGiveVeloPowerX;
			}
			//���肪�����Ȃ甽�]
			if ((p1.GetPos().x < p2.GetPos().x) || ColCheckLeftWall(p2, camera))
			{
				giveVeloP1.x *= -1;
			}

			//0�̏ꍇ�̓_�E�������U��
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
				p1.SetNoActFrame(p2.GetGiveNoActFrame());//�d����
				p1.LoadStateHit();//�q�b�g���[�V����
			}

			p1.SetKnockback(giveVeloP1);
			p1.SetVelo(giveVeloP1);//��������炷
			p1.SetHp(p1.GetHp() - p2.GetGiveDamage());//�_���[�W

			//�q�b�g�̉�
			m_seP1->Stop();
			m_seP1->SetSE(m_hitSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();
		
		}
		else//�K�[�h
		{
			giveVeloP1.y = 0;
			//���肪�����Ȃ甽�]
			if ((p1.GetPos().x < p2.GetPos().x) || ColCheckLeftWall(p2, camera))
			{
				giveVeloP1.x *= -1;
			}
			//���̍U�����R�}���h�̋Z�Ȃ����
			if (p2.IsCommand())
			{
				p1.SetHp(p1.GetHp() - (p2.GetGiveDamage() * kScrapMagin));//�_���[�W
				//���
				gameManager.LoadScrapeGuardEffect(p2);
			}
			else
			{
				//�K�[�h�G�t�F�N�g
				gameManager.LoadGuardEffect(p2);
			}
			p1.SetKnockback(giveVeloP1* kKnockbackMagin);
			p1.SetVelo(giveVeloP1* kKnockbackMagin);//��������炷
			p1.SetGuardFrame(p2.GetGiveGuardFrame());//�K�[�h�d��
			//�����K�[�h
			if (!p1.IsSquat())
			{
				p1.LoadStateGuardStand();//�K�[�h���[�V����
			}
			//���Ⴊ�݃K�[�h
			else
			{
				p1.LoadStateGuardSquat();//�K�[�h���[�V����
			}
			
			//�K�[�h�̉�
			m_seP1->Stop();
			m_seP1->SetSE(m_guardSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();
		
		}
		//�ǂɓ������Ă�G���U�������Ƃ�
		if (ColCheckWall(p1, camera))
		{
			//���肶��Ȃ��Ď������̂�����
			giveVeloP1.y = 0;
			giveVeloP1.x *= -1;
			p2.SetKnockback(giveVeloP1);
			p2.SetVelo(giveVeloP1);
		}

		//�G�t�F�N�g�̈ʒu
		gameManager.SetHitEffectPosP2(CreateHitEffectPosPtoP(p2, p1));
		p2.ResetAttackBox();
	}
	
	//�U���̏������I������画�������
	if (p1.IsHitAttack() || p2.IsHitAttack())
	{
		//���Z�b�g
		p1.OffIsHitAttack();
		p2.OffIsHitAttack();
	}

	//�v���C���[��HP��0�ȉ��ɂȂ����ꍇ
	if (p1.GetHp() <= 0)
	{
		//�J�����h�炷
		gameManager.OnIsCameraShake();
		p1.SetHp(0);
		p1.LoadStateHit();//�q�b�g���[�V����
	}
	if (p2.GetHp() <= 0)
	{
		//�J�����h�炷
		gameManager.OnIsCameraShake();
		p2.SetHp(0);
		p2.LoadStateHit();//�q�b�g���[�V����
	}
	
}

//�U�����ʂ邩
bool CollisionCheck::CheckSuccessAttack(Player& attacker, Player& defender)
{
	//�v���C���[�̍U���̑���
	AttackAttributes currentAttributes = attacker.GetAttackAttributes();

	//��i�U���������Ƃ�
	if (currentAttributes == AttackAttributes::Upper)
	{
		//�K�[�h�����Ă�����
		if (defender.IsGuard())
		{
			return false;
		}
	}
	//���i�U���������Ƃ�
	else if (currentAttributes == AttackAttributes::Lower)
	{
		//���Ⴊ�݃K�[�h�����Ă�����
		if (defender.IsGuard() && defender.IsSquat())
		{
			return false;
		}
	}
	//���i�i�U���������Ƃ�
	else if (currentAttributes == AttackAttributes::Middle)
	{
		//�����K�[�h�����Ă�����
		if (defender.IsGuard() && !defender.IsSquat())
		{
			return false;
		}
	}
	//�����܂ŗ�����U���͐���
	return true;
}




