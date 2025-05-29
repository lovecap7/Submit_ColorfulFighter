#pragma once
#include "Vector3.h"
#include <memory>
#include <DxLib.h>
#include "game.h"
class Player;
class Bullet;
class Camera;
class CollisionCheck;
class UI;
class FadeManager;
class SE;

class GameManager
{
private:
	int m_hitStopFrameCount;//�q�b�g�X�g�b�v�̎��Ԃ𐔂���
	int m_changeRoundFrameCount;//���E���h���؂�ւ��܂ł̎���
	//�����蔻��ɏ���
	std::shared_ptr<CollisionCheck> m_collcheck;
	void CheckKO(Player& p1, Player& p2);
	//�t�F�[�h
	std::shared_ptr<FadeManager> m_fadeManager;
	bool m_isFadeOut;
	//�v���C���[�̏�����
	int m_winNumP1;
	int m_winNumP2;
	//���E���h
	int m_roundNumber;
	//�q�b�g�X�g�b�v
	bool m_isHitStop;
	int m_hitStopFinishFrame;//���t���[���~�߂邩
	void HitStop();
	//���E���h�̐؂�ւ��
	bool m_isChangeRound;
	//�J�����h���
	bool m_isCameraShake;
	//�Q�[���I��
	bool m_isGameset;
	//KO
	bool m_isTimeUpOrKo;
	//�Q�[���J�n
	bool m_isStartRound;
	int m_startRoundCount;
	//�^�C�}�[
	void Timer();
	int m_time;//����
	int m_countFrame;//�t���[��
	//�G�t�F�N�g
	Vector3 m_hitEffectPosP1;//�v���C���[1���G�����������̃G�t�F�N�g
	Vector3 m_hitEffectPosP2;//�v���C���[2���G�����������̃G�t�F�N�g
	int m_redEffectHandle;
	int m_greenEffectHandle;
	int m_purpleEffectHandle;
	int m_blueHandle;
	//�Đ�����n���h��
	int m_hitEffectHandleP1;
	int m_hitEffectHandleP2;
	//�q�b�g�G�t�F�N�gP1
	int m_hitAnimIndexP1;//�Đ����Ă���摜�̌��݂̔ԍ�
	int m_hitAnimFinishIndexP1;//���̔ԍ��܂ŉ摜��������I���
	int m_hitAnimCountFrameP1;
	float m_effectSizeP1;
	float m_angleP1;
	//�q�b�g�G�t�F�N�gP2
	int m_hitAnimIndexP2;//�Đ����Ă���摜�̌��݂̔ԍ�
	int m_hitAnimFinishIndexP2;//���̔ԍ��܂ŉ摜��������I���
	int m_hitAnimCountFrameP2;
	float m_effectSizeP2;
	float m_angleP2;
	//SE
	std::shared_ptr<SE> m_se;
	int m_round1SeHandle;
	int m_round2SeHandle;
	int m_round3SeHandle;
	int m_roundOverSeHandle;
	int m_fightSeHandle;
	int m_koSeHandle;
	int m_timeUpSeHandle;
	//�v���C���[1���U���𓖂Ă��ꍇ�v���C���[1��O�ɕ`��(true)
	//�v���C���[2���U���𓖂Ă��ꍇ�v���C���[1�����ɕ`��(false)
	bool m_isDrawFrontP1;
public:
	GameManager();
	~GameManager();
	void Init();
	void Update(Player& p1, Player& p2, Bullet& bulletP1, Bullet& bulletP2,
		Camera& camera,UI& ui);
	void NoStopUpdate(Player& p1, Player& p2);//�~�߂Ȃ�Update
	void Draw(Camera& camera);
	void DrawFade();
	void DrawHitEffect(Camera& camera);
	//���E���h��
	int GetRoundNumber() { return m_roundNumber; }
	//�q�b�g�X�g�b�v
	bool IsHitStop() { return m_isHitStop; }
	void OnIsHitStop() { m_isHitStop = true; }
	bool IsCameraShake() { return m_isCameraShake; }
	void OnIsCameraShake() { m_isCameraShake = true; }
	//���E���h�̐؂�ւ��
	bool IsChangeRound() { return m_isChangeRound; }
	void OffIsChangeRound() { m_isChangeRound = false; }
	//�Q�[���I��
	bool IsGameset() { return m_isGameset; }
	//KO��timeup������
	bool IsTimeUpOrKo() { return m_isTimeUpOrKo; }
	//������
	int GetWinNumP1() { return m_winNumP1; }
	int GetWinNumP2() { return m_winNumP2; }
	//���E���h�X�^�[�g
	bool IsStartRound() { return m_isStartRound; }
	int GetStartRoundCount() { return m_startRoundCount;}
	//�^�C�}�[�̎擾
	int GetTimer() { return m_time; }
	//�G�t�F�N�g�̃|�W�V����
	void SetHitEffectPosP1(Vector3 hitPos) { m_hitEffectPosP1 = hitPos; }
	void SetHitEffectPosP2(Vector3 hitPos) { m_hitEffectPosP2 = hitPos; }
	//�q�b�g�G�t�F�N�g�A�K�[�h�G�t�F�N�g��\�����邽�߂̊֐�
	void LoadNormalHitEffect(Player& player);
	void LoadSpecialHitEffect(Player& player);
	void LoadGuardEffect(Player& player);
	void LoadScrapeGuardEffect(Player& player);
	//�`�揇��ς��邽�߂Ɏg��
	bool GetIsDrawFrontP1() { return m_isDrawFrontP1; }
	void SetIsDrawFrontP1(bool isDrawFront) { m_isDrawFrontP1 = isDrawFront; }
	//�������v���C���[�̃C���f�b�N�X
	PlayerIndex ResultWinPlayerIndex();
};

