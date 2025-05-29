#pragma once
#include "DxLib.h"
#include "Box.h"
#include "Vector3.h"
#include <string>
#include "AttackInfo.h"
#include "game.h"
#include <memory>

class Input;
class Player;
class Bullet;
class SE;
class Chara
{	
private:
	//voice
	std::shared_ptr<SE> m_voice;
	int m_attack1VoiceHandle;
	int m_attack2VoiceHandle;
	int m_attack3VoiceHandle;
	int m_damage1VoiceHandle;
	int m_damage2VoiceHandle;
	int m_standUp1VoiceHandle;
	int m_standUp2VoiceHandle;
	void VoiceSe(int voiceHandle);
	//se
	std::shared_ptr<SE> m_se;
	int m_commandSeHandle[9];
	void CommandSe(int commandIndex);
	//�I�񂾋Z�̃C���f�b�N�X
	int m_selectCommandIndex[3];
	bool CheckMyCommand();
	//�F
	CharaColorIndex m_charaColorIndex;
	void InitColor(CharaColorIndex charaColorIndex);
	//�R�}���h�Z
	enum class Command
	{
		Null,		//0���R�}���h���s�Ƃ����Ӗ��ɂ�����
		Hadouken,	//�g����
		Shouryuken,	//������
		Sukuryuu,	//�X�N�����[
		Supairaru,	//�X�p�C�����A���[
		Sonic,	//�\�j�b�N�u�[��
		Wave,//�ŕ���
		Rolling,//�o�[�`�J�����[�����O
		Tatumaki,//���������r
		Wildhant,//���C���h�n���g
	};
	int m_commandIndex;//���̃R�}���h���ĂԂ̂����L�^
	//�g����
	//�R�}���h�Z���[�V�����擾
	void GetAnimCommand1(Player& player);
	//�R�}���h�Z���[�V�����̓����蔻��(�E����)
	void GetHitBoxCommand1(Player& player);
	//�R�}���h�Z������ɂ����炷����
	void GetGiveEffectCommand1(Player& player);
	//�R�}���h�Z�̓��e�i�����j
	void MovementCommand1(Player& player, Bullet& bullet, Player& enemy);

	//������
	//�R�}���h�Z���[�V�����擾
	void GetAnimCommand2(Player& player);
	//�R�}���h�Z���[�V�����̓����蔻��(�E����)
	void GetHitBoxCommand2(Player& player);
	//�R�}���h�Z������ɂ����炷����
	void GetGiveEffectCommand2(Player& player);
	//�R�}���h�Z�̓��e�i�����j
	void MovementCommand2(Player& player, Bullet& bullet, Player& enemy);

	//�X�N�����[�p�C���h���C�o�[
	//�R�}���h�Z���[�V�����擾
	void GetAnimCommand3(Player& player);
	//�R�}���h�Z���[�V�����̓����蔻��(�E����)
	void GetHitBoxCommand3(Player& player);
	//�R�}���h�Z������ɂ����炷����
	void GetGiveEffectCommand3(Player& player);
	//�R�}���h�Z�̓��e�i�����j
	void MovementCommand3(Player& player, Bullet& bullet, Player& enemy);

	//�X�p�C�����A���[
	//�R�}���h�Z���[�V�����擾
	void GetAnimCommand4(Player& player);
	//�R�}���h�Z���[�V�����̓����蔻��(�E����)
	void GetHitBoxCommand4(Player& player);
	//�R�}���h�Z������ɂ����炷����
	void GetGiveEffectCommand4(Player& player);
	//�R�}���h�Z�̓��e�i�����j
	void MovementCommand4(Player& player, Bullet& bullet, Player& enemy);

	//�\�j�b�N�u�[��
	//�R�}���h�Z���[�V�����擾
	void GetAnimCommand5(Player& player);
	//�R�}���h�Z���[�V�����̓����蔻��(�E����)
	void GetHitBoxCommand5(Player& player);
	//�R�}���h�Z������ɂ����炷����
	void GetGiveEffectCommand5(Player& player);
	//�R�}���h�Z�̓��e�i�����j
	void MovementCommand5(Player& player, Bullet& bullet, Player& enemy);

	//�p���[�E�F�C�u
	//�R�}���h�Z���[�V�����擾
	void GetAnimCommand6(Player& player);
	//�R�}���h�Z���[�V�����̓����蔻��(�E����)
	void GetHitBoxCommand6(Player& player);
	//�R�}���h�Z������ɂ����炷����
	void GetGiveEffectCommand6(Player& player);
	//�R�}���h�Z�̓��e�i�����j
	void MovementCommand6(Player& player, Bullet& bullet, Player& enemy);

	//�o�[�`�J�����[�����O
	//�R�}���h�Z���[�V�����擾
	void GetAnimCommand7(Player& player);
	//�R�}���h�Z���[�V�����̓����蔻��(�E����)
	void GetHitBoxCommand7(Player& player);
	//�R�}���h�Z������ɂ����炷����
	void GetGiveEffectCommand7(Player& player);
	//�R�}���h�Z�̓��e�i�����j
	void MovementCommand7(Player& player, Bullet& bullet, Player& enemy);

	//���������r
	//�R�}���h�Z���[�V�����擾
	void GetAnimCommand8(Player& player);
	//�R�}���h�Z���[�V�����̓����蔻��(�E����)
	void GetHitBoxCommand8(Player& player);
	//�R�}���h�Z������ɂ����炷����
	void GetGiveEffectCommand8(Player& player);
	//�R�}���h�Z�̓��e�i�����j
	void MovementCommand8(Player& player, Bullet& bullet, Player& enemy);

	//���C���h�n���g
	//�R�}���h�Z���[�V�����擾
	void GetAnimCommand9(Player& player);
	//�R�}���h�Z���[�V�����̓����蔻��(�E����)
	void GetHitBoxCommand9(Player& player);
	//�R�}���h�Z������ɂ����炷����
	void GetGiveEffectCommand9(Player& player);
	//�R�}���h�Z�̓��e�i�����j
	void MovementCommand9(Player& player, Bullet& bullet, Player& enemy);

	//���[�V����
	int m_idleStandHandel;//�ҋ@
	int m_idleSquatHandel;//���Ⴊ�ݑҋ@
	int m_walkFrontHandel;//�O����
	int m_walkBackHandel;//������
	int m_jumpHandel;//�W�����v
	//����
	int m_punchLightHandle;//��p���`
	int m_punchHighHandle;//���p���`
	int m_kickLightHandle;//��L�b�N
	int m_kickHighHandle;//���L�b�N
	//���Ⴊ��
	int m_punchLightSquatHandle;//���Ⴊ�ݎ�p���`
	int m_punchHighSquatHandle;//���Ⴊ�݋��p���`
	int m_kickLightSquatHandle;//���Ⴊ�ݎ�L�b�N
	int m_kickHighSquatHandle;//���Ⴊ�݋��L�b�N
	//��
	int m_punchLightAerialHandle;//�󒆎�p���`
	int m_punchHighAerialHandle;//�󒆋��p���`
	int m_kickLightAerialHandle;//�󒆎�L�b�N
	int m_kickHighAerialHandle;//�󒆋��L�b�N
	//�K�[�h
	int m_guardStandHandle;//�����K�[�h
	int m_guardSquatHandle;//���Ⴊ�݃K�[�h
	//����
	int m_graspHandle;//����
	int m_throwFrontHandle;//�O����
	int m_throwBackHandle;//��듊��
	//�q�b�g��
	int m_damageStandHandle;//��炢
	int m_damageAerialHandle;//��炢
	int m_downHandle;//�_�E��
	int m_downAerialHandle;//�󒆃_�E��
	int m_standUpHandle;//�N���オ��
	//�R�}���h�Z
	int m_commandHandle1;//�R�}���h�Z
	int m_commandHandle2;//�R�}���h�Z
	int m_commandHandle3;//�R�}���h�Z
	int m_commandHandle4;//�R�}���h�Z
	int m_commandHandle5;//�R�}���h�Z
	int m_commandHandle6;//�R�}���h�Z
	int m_commandHandle7;//�R�}���h�Z
	int m_commandHandle8;//�R�}���h�Z
	int m_commandHandle9;//�R�}���h�Z
	//�����|�[�Y
	int m_winPoseHandle;
	//�U���̑������o���Ă���
	AttackTypes lastAttackType;
	//�U���ɂ���Ď����������ۂɎg��
	Vector3 m_velocity;
	int m_handle;//�摜
	int m_animNum;//�A�j���[�V�����̐�
	int m_oneAnimIntervalFrame;//�A�j���[�V�����P���ɂ�����t���[��
	int m_allFrame;//�S�̃t���[��
	AttackAttributes m_attackAttributes;//�U������
	int m_startAttackFrame;//�U���̔���
	int m_finishAttackFrame;//�U�������I��
	int m_giveNoActFrame;//�d��
	int m_giveGuardFrame;//�K�[�h�d��
	float m_giveDamage;//�_���[�W
	//�U���ɂ���đ���ɗ^����Velo
	Vector3 m_giveAttackVelo;
public:
	Chara(int* index,CharaColorIndex charaColorIndex);
	~Chara();
	//�������[�V�����擾
	 void GetAnimIdleStand(Player& player) ;
	//���ꔻ��@�������ꔻ��@�U������
	//�ҋ@���[�V�����̓����蔻��(�E����)
	 void GetHitBoxIdleStand(Player& player) ;
	
	//���Ⴊ�݃��[�V�����擾
	void GetAnimIdleSquat(Player& player) ;
	//���Ⴊ�݃��[�V�����̓����蔻��(�E����)
	void GetHitBoxIdleSquat(Player& player) ;

	//�O�������[�V�����擾
	void GetAnimWalkFront(Player& player) ;
	//�O�������[�V�����̓����蔻��(�E����)
	void GetHitBoxWalkFront(Player& player) ;

	//���������[�V�����擾
	void GetAnimWalkBack(Player& player) ;
	//���������[�V�����̓����蔻��(�E����)
	void GetHitBoxWalkBack(Player& player) ;

	//�W�����v���[�V�����擾
	void GetAnimJump(Player& player) ;
	//�W�����v���[�V�����̓����蔻��(�E����)
	void GetHitBoxJump(Player& player) ;

	//����
	//��p���`���[�V�����擾
	void GetAnimPunchLight(Player& player);
	//���ꔻ��@�������ꔻ��@�U������
	//��p���`���[�V�����̓����蔻��(�E����)
	void GetHitBoxPunchLight(Player& player) ;
	//��p���`������ɂ����炷����
	void GetGiveEffectPunchLight(Player& player) ;

	//���p���`���[�V�����擾
	void GetAnimPunchHigh(Player& player) ;
	//���p���`���[�V�����̓����蔻��(�E����)
	void GetHitBoxPunchHigh(Player& player) ;
	//���p���`������ɂ����炷����
	void GetGiveEffectPunchHigh(Player& player) ;

	//��L�b�N���[�V�����擾
	void GetAnimKickLight(Player& player) ;
	//��L�b�N���[�V�����̓����蔻��(�E����)
	void GetHitBoxKickLight(Player& player) ;
	//��L�b�N������ɂ����炷����
	void GetGiveEffectKickLight(Player& player);

	//���L�b�N���[�V�����擾
	void GetAnimKickHigh(Player& player) ;
	//���L�b�N���[�V�����̓����蔻��(�E����)
	void GetHitBoxKickHigh(Player& player) ;
	//���L�b�N������ɂ����炷����
	void GetGiveEffectKickHigh(Player& player) ;

	//���Ⴊ��
	//���Ⴊ�ݎ�p���`���[�V�����擾
	void GetAnimPunchLightSquat(Player& player) ;
	//���ꔻ��@�������ꔻ��@�U������
	//���Ⴊ�ݎ�p���`���[�V�����̓����蔻��(�E����)
	void GetHitBoxPunchLightSquat(Player& player) ;
	//���Ⴊ�ݎ�p���`������ɂ����炷����
	void GetGiveEffectPunchLightSquat(Player& player) ;

	//���Ⴊ�݋��p���`���[�V�����擾
	void GetAnimPunchHighSquat(Player& player) ;
	//���ꔻ��@�������ꔻ��@�U������
	//���Ⴊ�݋��p���`���[�V�����̓����蔻��(�E����)
	void GetHitBoxPunchHighSquat(Player& player) ;
	//���Ⴊ�݋��p���`������ɂ����炷����
	void GetGiveEffectPunchHighSquat(Player& player) ;

	//���Ⴊ�ݎ�L�b�N���[�V�����擾
	void GetAnimKickLightSquat(Player& player) ;
	//���ꔻ��@�������ꔻ��@�U������
	//���Ⴊ�ݎ�L�b�N���[�V�����̓����蔻��(�E����)
	void GetHitBoxKickLightSquat(Player& player) ;
	//���Ⴊ�ݎ�L�b�N������ɂ����炷����
	void GetGiveEffectKickLightSquat(Player& player) ;

	//���Ⴊ�݋��L�b�N���[�V�����擾
	void GetAnimKickHighSquat(Player& player) ;
	//���ꔻ��@�������ꔻ��@�U������
	//���Ⴊ�݋��L�b�N���[�V�����̓����蔻��(�E����)
	void GetHitBoxKickHighSquat(Player& player) ;
	//���Ⴊ�݋��L�b�N������ɂ����炷����
	void GetGiveEffectKickHighSquat(Player& player) ;

	//�󒆎�p���`���[�V�����擾
	void GetAnimPunchLightAerial(Player& player) ;
	//���ꔻ��@�������ꔻ��@�U������
	//�󒆎�p���`���[�V�����̓����蔻��(�E����)
	void GetHitBoxPunchLightAerial(Player& player) ;
	//�󒆎�p���`������ɂ����炷����
	void GetGiveEffectPunchLightAerial(Player& player) ;

	//�󒆋��p���`���[�V�����擾
	void GetAnimPunchHighAerial(Player& player) ;
	//���ꔻ��@�������ꔻ��@�U������
	//�󒆋��p���`���[�V�����̓����蔻��(�E����)
	void GetHitBoxPunchHighAerial(Player& player) ;
	//�󒆋��p���`������ɂ����炷����
	void GetGiveEffectPunchHighAerial(Player& player) ;

	//�󒆎�L�b�N���[�V�����擾
	void GetAnimKickLightAerial(Player& player);
	//���ꔻ��@�������ꔻ��@�U������
	//�󒆎�L�b�N���[�V�����̓����蔻��(�E����)
	void GetHitBoxKickLightAerial(Player& player) ;
	//�󒆎�L�b�N������ɂ����炷����
	void GetGiveEffectKickLightAerial(Player& player) ;

	//�󒆋��L�b�N���[�V�����擾
	void GetAnimKickHighAerial(Player& player) ;
	//���ꔻ��@�������ꔻ��@�U������
	//�󒆋��L�b�N���[�V�����̓����蔻��(�E����)
	void GetHitBoxKickHighAerial(Player& player) ;
	//�󒆋��L�b�N������ɂ����炷����
	void GetGiveEffectKickHighAerial(Player& player) ;

	//�U����̂��ꔻ��
	void  GetHitBoxAttackedAerial(Player& player);

	//�����K�[�h���[�V�����擾
	void GetAnimGuardStand(Player& player) ;
	//�����K�[�h�̓����蔻��(�E����)
	void GetHitBoxGuardStand(Player& player);
	//���Ⴊ�݃K�[�h���[�V�����擾
	void GetAnimGuardSquat(Player& player) ;
	//���Ⴊ�݃K�[�h�̓����蔻��(�E����)
	void GetHitBoxGuardSquat(Player& player);

	//���݃��[�V�����擾
	void GetAnimGrasp(Player& player) ;
	//���݂̓����蔻��(�E����)
	void GetHitBoxGrasp(Player& player) ;
	//���݂������炷����
	void GetGiveEffectGrasp(Player& player);

	//������ꃂ�[�V�����͂�����g���̂œ����蔻�肾��
	void GetHitBoxBeThrown(Player& player) ;

	//�O�����̃��[�V����
	void GetAnimThrowFront(Player& player) ;
	//�O�����̓����蔻��(�E����)
	void GetHitBoxThrowFront(Player& player) ;

	//��듊���̃��[�V����
	void GetAnimThrowBack(Player& player) ;
	//��듊���̓����蔻��(�E����)
	void GetHitBoxThrowBack(Player& player) ;

	//�������ꃂ�[�V�����擾
	void GetAnimHitStand(Player& player) ;
	//�󒆂��ꃂ�[�V�����擾
	void GetAnimHitAerial(Player& player) ;
	//���ꔻ��
	void GetHitBoxHitStand(Player& player) ;

	//�_�E�����[�V�������[�V�����擾
	void GetAnimDown(Player& player) ;
	//�_�E�����[�V�����̓����蔻��(�E����)
	void GetHitBoxDown(Player& player) ;
	//�󒆃_�E�����[�V�������[�V�����擾�i���n��Ƀ_�E������j
	void GetAnimDownAerial(Player& player) ;
	//�󒆃��[�V�����̓����蔻��(�E����)
	void GetHitBoxDownAerial(Player& player) ;

	//�N���オ��
	void GetAnimStandUp(Player& player);

	//�R�}���h
	//�R�}���h���������Ă��邩
	bool SuccessCommandCheck(Input& input, Player& player) ;

	//�R�}���h�Z���[�V�����擾
	void GetAnimCommand(Player& player) ;
	//�R�}���h�Z���[�V�����̓����蔻��(�E����)
	void GetHitBoxCommand(Player& player) ;
	//�R�}���h�Z������ɂ����炷����
	void GetGiveEffectCommand(Player& player) ;
	//�R�}���h�Z�̓��e�i�����j
	void MovementCommand(Player& player, Bullet& bullet, Player& enemy) ;

	//�����|�[�Y
	void GetAnimWinPose(Player& player) ;
};
