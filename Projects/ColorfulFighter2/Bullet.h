#pragma once
#include "Vector3.h"
#include "game.h"
#include "Box.h"

class Camera;
class Player;
class Bullet
{
private:
	Vector3 m_pos;
	Vector3 m_velocity;
	PlayerIndex m_playerIndex;
	Box m_hitBoxAttack;
	int m_giveNoActFrame;	//�������U�������Ƃ����肪�����Ȃ��t���[��
	int m_giveGuardFrame;	//�������U�������Ƃ����肪�K�[�h�����ē����Ȃ��t���[��
	float m_giveDamage;			//����ɗ^����_���[�W
	//�U���ɂ���đ���ɗ^����Velo
	Vector3 m_giveAttackVelo;
	void SetShotEffect(float damage, int giveNoActFrame, int giveGuardFrame);
	//true�Ȃ甭�˒�
	bool m_isShooting;
	//�U�����v���C���[�ɂ��������𔻒�
	bool m_isHitPlayer;
	//�v���C���[�ɓ���������
	bool CheckHitPlayer(Player& enemy);
	//�e�ɓ���������
	bool CheckHitBullet(Bullet& otherBullet);
	//�U������̏�����
	void InitHitBoxHadou();
	void InitHitBoxSonic();
	void InitHitBoxWave();
	void ResetAttackBox() {
		//�U���̔�������Z�b�g
		m_hitBoxAttack.x1 = 0;
		m_hitBoxAttack.y1 = 0;
		m_hitBoxAttack.x2 = 0;
		m_hitBoxAttack.y2 = 0;
	}
	//�e�̃n���h��
	int m_blueBulletHandle;//���e
	int m_yellowBulletHandle;//���F���e
	int m_purpleBulletHandle;//���̒e
	int m_animIndex;//���݂̉摜�̔ԍ�
	int m_startAnimIndex;//�ŏ��̉摜�̔ԍ�
	int m_finishAnimIndex;//�Ō�̉摜�̔ԍ�
	int m_animCountFrame;
	using UpdateFunc_t = void(Bullet::*)(Player& enemy, Bullet& otherBullet, Camera& camera);
	UpdateFunc_t m_update;
	//Draw
	using DrawFunc_t = void(Bullet::*)(Camera& camera);
	DrawFunc_t m_draw;
	void UpdateHadou(Player& enemy, Bullet& otherBullet, Camera& camera);
	void DrawHadou(Camera& camera);
	void UpdateSonic(Player& enemy, Bullet& otherBullet, Camera& camera);
	void DrawSonic(Camera& camera);
	void UpdateWave(Player& enemy, Bullet& otherBullet, Camera& camera);
	void DrawWave(Camera& camera);
public:
	Bullet(PlayerIndex playerIndex);
	~Bullet();
	void Init();
	void Update(Player& enemy, Bullet& otherBullet,Camera& camera);
	void Draw(Camera& camera);
	void Disappear();//�e��������
	//�Ȃ�P�Ȃ̂�
	PlayerIndex GetPlayerIndex() { return m_playerIndex; }
	//���ݔ��˒������擾
	bool GetIsShooting()const { return m_isShooting; }
	//���˒�
	void OnIsShooting() { m_isShooting = true; }
	void OffShooting() { m_isShooting = false; }
	//���W�̎擾
	Vector3 GetPos()const { return m_pos; }
	//�v���C���[�ɓ����������ǂ������擾
	bool GetIsHitPlayer() const{ return m_isHitPlayer; }
	//�U������
	Box GetHitBoxAttack()const { return m_hitBoxAttack; }
	//�_���[�W
	float GetGiveDamage()const { return m_giveDamage; }
	//�d��
	int GetGiveNoActFrame()const { return m_giveNoActFrame; }
	//�K�[�h�d��
	int GetGiveGuardFrame()const { return m_giveGuardFrame; }
	//�U���ɂ���đ���ɗ^����Velo
	Vector3 GetGiveAttackVelo() { return m_giveAttackVelo; }
	void SetGiveAttackVelo(Vector3 giveAttackVelo) { m_giveAttackVelo = giveAttackVelo; }
	//3��ނ̒e
	void LoadHadou(Player& player, float damage, int giveNoActFrame, int giveGuardFrame);
	void LoadSonic(Player& player, float damage, int giveNoActFrame, int giveGuardFrame);
	void LoadWave(Player& player, float damage, int giveNoActFrame, int giveGuardFrame);
};

