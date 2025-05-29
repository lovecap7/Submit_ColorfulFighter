#pragma once
#include "Vector3.h"
#include <memory>
#include "Box.h"
#include "AttackInfo.h"
#include "game.h"
class Chara;
class Input;
class Camera;
class Bullet;
class GameManager;
class SE;

class Player
{
private:
	PlayerIndex m_playerIndex;	//�������v���C���[1�Ȃ̂�2�Ȃ̂�
	Vector3		m_pos;		//�v���C���[�̍��W
	Vector3	    m_velocity;	//�v���C���[�̈ړ���
	float		m_angle;//��]
	float		m_hp;
	Vector3		m_jumpVelo;		//�W�����v�̈ړ���
	Vector3		m_giveAttackVelo;//�U���ɂ���đ���𓮂�����
	Vector3		m_giveThrowVelo;//�������^�����
	int			m_handle;		//�v���C���[�̌��݂̉摜
	//�L�����N�^�[
	std::shared_ptr<Chara> m_chara;
	//�v���C���[�̏�ԊǗ�
	bool m_isSameColor;		//����Ɠ����F�Ȃ�true
	bool m_isLeft;			//true�Ȃ獶����
	bool m_isGround;		//true�Ȃ�n��ɂ���
	bool m_isHitAttack;		//true�Ȃ�U���𓖂ĂĂ���
	bool m_isHitGrasp;		//true�Ȃ���݂𓖂ĂĂ���
	bool m_isGuard;			//true�Ȃ�U�����ꂽ�Ƃ��K�[�h������
	bool m_isSquat;			//true�Ȃ炵�Ⴊ��
	bool m_isThrown;		//true�Ȃ瓊�����Ă���
	bool m_isThrowSuccess;	//true�Ȃ瓊������
	bool m_isCommand;		//true�Ȃ�R�}���h�Z���s���Ă���
	bool m_isPossibleCancel;//�d�����L�����Z���ł���
	int	 m_cancelFrameCount;
	void Cancel(Input& input, Player enemy, std::shared_ptr<Bullet> myBullet, GameManager& gameManager);//�L�����Z��
	void InputDelay(Input& input, Player enemy, std::shared_ptr<Bullet> myBullet, GameManager& gameManager);//�{�^���������ċZ���m�肷��܂ł̏���
	bool CheckDead();//HP�̏�Ԃ��Ǘ�
	void CheckGround();//�n��ɂ��邩�`�F�b�N
	void Gravity();//�d��
	void LookDir(Player& enemy);//����̂ق�������
	//�I�񂾋Z�̃C���f�b�N�X
	int m_selectCommandIndex[3];
	//������������
	Box m_pushBox;
	//������������̏�����
	void InitPushBox();
	//���ꔻ��
	Box m_hitBoxHead;
	Box m_hitBoxBody;
	Box m_hitBoxLeg;
	//��������
	Box m_hitBoxThrow;
	//���ݔ���
	Box m_hitBoxGrasp;
	//�U������
	Box m_hitBoxAttack;
	//�A�j���[�V�����֘A
	int m_animCountFrame;	//�t���[���𐔂���
	int m_animIndex;	//�A�j���[�V�����̔ԍ��𐔂���(1�����閈�ɃA�j���[�V�������i��)
	int m_animNum;		//�A�j���[�V�����̐�
	int m_oneAnimIntervalFrame;	//1���̃A�j���[�V�����ɂ�����t���[��
	//�U����i�@�p���`�Ƃ��L�b�N�Ƃ�
	AttackTypes m_attackType;
	int		m_startAttackFrame;//�U���̔���
	int		m_finishAttackFrame;//����
	int		m_giveNoActFrame;	//�������U�������Ƃ����肪�����Ȃ��t���[��
	int		m_giveGuardFrame;	//�������U�������Ƃ����肪�K�[�h�����ē����Ȃ��t���[��
	float	m_giveDamage;		//����ɗ^����_���[�W
	int	    m_guardFrame;		//�������K�[�h�œ����Ȃ��t���[��
	Vector3 m_knockback;		//������������΂�����
	//�G�ɍU���𓖂Ă����ǂ���
	bool CheckHit(Player& enemy);
	//�G�ɂ��݂𓖂Ă����ǂ���
	bool CheckHitGrasp(Player& enemy);
	//���݂̍U���̑���
	AttackAttributes m_attackAttributes;
	int m_noActFrame;		//�����������Ȃ��t���[��
	int m_allNoActFrame;		//�����������Ȃ��S�̃t���[��
	//SE
	std::shared_ptr<SE> m_se;
	//�n���h��
	int m_walkSeHandle;//����
	int m_lightPunchSeHandle;
	int m_lightKickSeHandle;
	int m_highPunchSeHandle;
	int m_highKickSeHandle;
	int m_downSeHandle;//�|��鉹
	int m_jumpSeHandle;//��ԉ�
	int m_jumpedSeHandle;//���n�����Ƃ��̉�
	int m_graspSeHandle;//���މ�
	int m_throwSeHandle;//������
	int m_loseSeHandle;//���[��
	int m_winSeHandle;//������I
	void JumpSE();
	void JumpedSE();
	void AttackSE();//�U���ɂ���ĕ���؂鉹
	void DownSE();
	void GraspSE();
	void ThrowSE();
	void LoseSE();
	void WinSE();
	//�e
	void DrawShadow(const Camera& camera);
	//�����{�^�������������Ă���ꍇ20�t���[���������͂���肽��
	struct PressBottun
	{
		bool isPress;
		int pressCountFrame;
		void Init(bool IsPress, int PressCountFrame)
		{
			isPress = IsPress;
			pressCountFrame = PressCountFrame;
		}
	};
	PressBottun m_lightPunchBottun;
	PressBottun m_highPunchBottun;
	PressBottun m_lightKickBottun;
	PressBottun m_highKickBottun;
	PressBottun m_graspBottun;
	//�v���C���[��p�̓���
	void PlayerInput(Input& input);
	void CheckContinuePressBottun(PressBottun& bottun);

	//�����o�֐��|�C���^�̏���
//Update
	using UpdateFunc_t = void(Player::*)(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	UpdateFunc_t m_update;
	//Draw
	using DrawFunc_t = void(Player::*)(const Camera& camera);
	DrawFunc_t m_draw;

	//�����ҋ@
	void IdleStandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void IdleStandDraw(const Camera& camera);

	//���Ⴊ�ݑҋ@
	void IdleSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void IdleSquatDraw(const Camera& camera);

	//�W�����v
	void JumpUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void JumpDraw(const Camera& camera);

	//�����U��
	void AttackStandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void AttackStandDraw(const Camera& camera);

	//���Ⴊ�ݍU��
	void AttackSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void AttackSquatDraw(const Camera& camera);

	//�󒆍U��
	void AttackAerialUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void AttackAerialDraw(const Camera& camera);

	//�󒆍U����
	void AttackedAerialUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void AttackedAerialDraw(const Camera& camera);

	//�R�}���h1
	void CommandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void CommandDraw(const Camera& camera);

	//�����K�[�h
	void GuardStandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void GuardStandDraw(const Camera& camera);

	//���Ⴊ�݃K�[�h
	void GuardSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void GuardSquatDraw(const Camera& camera);

	//���� -> ���������瓊���Ɉڍs
	void GraspUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void GraspDraw(const Camera& camera);

	//������
	void ThrowUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void ThrowDraw(const Camera& camera);

	//��������
	void BeThrownUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void BeThrownDraw(const Camera& camera);

	//�����ʂ�
	void ThrowEscapeUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void ThrowEscapeDraw(const Camera& camera);

	//������炢
	void DamageUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void DamageDraw(const Camera& camera);

	//���Ⴊ�݋�炢
	void DamageSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void DamageSquatDraw(const Camera& camera);

	//�_�E��
	void DownUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet,GameManager& gameManager);
	void DownDraw(const Camera& camera);
	//�󒆃_�E��
	void DownAerialUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet,GameManager& gameManager);
	void DownAerialDraw(const Camera& camera);

	//�N���オ��
	void StandUpUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void StandUpDraw(const Camera& camera);

	//���s�|�[�Y
	void ResultUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet,GameManager& gameManager);
	void ResultDraw(const Camera& camera);
#if _DEBUG
	//�f�o�b�O�p
	//�A�j���[�V�����̃t���[����ۑ�����
	int debugJumpFrame = 0;
	int debugCommandFrame = 0;
	//�����蔻��̕\��
	void DrawHitBox(const Camera& camera);
	bool m_isCheckAttackBox = false;//�U������̉���
#endif
public:
	Player(PlayerIndex playeIndex, int* selectCommandIndex, CharaColorIndex charaColorIndex, bool isSameColor);
	~Player();
	void Init(float X, bool isLeft);
	void Update(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void Draw(const Camera& camera);
	void End();
	//�Ȃ�P�Ȃ̂�
	PlayerIndex GetPlayerIndex() { return m_playerIndex; }
	//�d��
	void SetNoActFrame(int noActFrame)
	{
		m_noActFrame = noActFrame;		//�����������Ȃ��t���[��
		m_allNoActFrame = noActFrame;		//�����������Ȃ��S�̃t���[��
	}
	void SetGuardFrame(int guardFrame) { m_guardFrame = guardFrame; }
	//�m�b�N�o�b�N
	void SetKnockback(Vector3 knockback) { m_knockback = knockback; }
	//���W
	Vector3 GetPos() { return m_pos; }
	void SetPos(Vector3 vec) { m_pos = vec; }
	//���x�i�́j
	Vector3 GetVelo() { return m_velocity; }
	void SetVelo(Vector3 vec) { m_velocity = vec; }
	void AddVelo(Vector3 vec) { m_velocity += vec; }
	void SetVeloX(float x) { m_velocity.x = x; }
	void SetVeloY(float y) { m_velocity.y = y; }
	//bool
	bool GetDirState() const { return m_isLeft; }//�����Ă����
	bool IsGuard() const { return m_isGuard; }//�K�[�h�����Ă��邩
	bool IsGround() const { return m_isGround; }//�n��ɂ��邩
	bool IsCommand() { return m_isCommand; }//�R�}���h�Z�����Ă���
	bool IsSquat() const { return m_isSquat; }//���Ⴊ��ł邩
	bool IsThrowSuccess() const { return m_isThrowSuccess; }//���݂�����������
	//�����̏����Ŏg��
	bool IsHitAttack() const { return m_isHitAttack; }//�U��������������
	bool IsHitGrasp() const { return m_isHitGrasp; }//���݂�����������
	//true�����ʂ��ɂ���
	void OnIsThrownSuccess() { m_isThrowSuccess = true; }
	//������ꂽ��true�ɂ���
	void OnIsThrown() { m_isThrown = true; }
	//�U����������L�����Z�����ł���悤�ɂ���
	void OnIsPossibleCancel() { m_isPossibleCancel = true; }
	//�����̑��x�i�́j
	void SetThrowVelo(Vector3 vec) { m_giveThrowVelo = vec; }
	//HP
	float GetHp() { return m_hp; }
	void SetHp(float hp) { m_hp = hp; }
	//�L�����N�^�[�̋Z�̏����v���C���[�ɃZ�b�g���邽�߂Ɏg���֐�
	void SetHandle(int handle) { m_handle = handle; }//�摜
	void SetAnimNum(int animNum) { m_animNum = animNum; }//�A�j���[�V�����̐�
	void SetAnimIndex(int animIndex) { m_animIndex = animIndex; }//���݂̃A�j���[�V���������̃^�C�~���O����n�߂����Ƃ��Ƃ��Ɏg��
	int GetAnimIndex() { return m_animIndex; }//���݂̃A�j���[�V���������̃^�C�~���O����n�߂����Ƃ��Ƃ��Ɏg��
	void SetOneAnimFrame(int oneAnimFrame) { m_oneAnimIntervalFrame = oneAnimFrame; }//�A�j���[�V�����P���ɂ�����t���[��
	int GetAnimCountFrame() { return m_animCountFrame; }//�A�j���[�V�����̃t���[��
	void SetAttackAttributes(AttackAttributes attackAttributes) { m_attackAttributes = attackAttributes; }//�U������
	void SetStartAttackFrame(int startAttackFrame) { m_startAttackFrame = startAttackFrame; }//�U���̔���
	void SetFinishAttackFrame(int finishAttackFrame) { m_finishAttackFrame = finishAttackFrame; }//�U�������I��
	void SetGiveNoActFrame(int giveNoActFrame) { m_giveNoActFrame = giveNoActFrame; }//�d��
	void SetGiveGuardFrame(int giveGuardFrame) { m_giveGuardFrame = giveGuardFrame; }//�K�[�h�d��
	void SetGiveDamage(float giveDamage) { m_giveDamage = giveDamage; }//�_���[�W
	//�U���ɂ���đ���ɗ^����Velo
	Vector3 GetGiveAttackVelo() { return m_giveAttackVelo; }
	void SetGiveAttackVelo(Vector3 giveAttackVelo) { m_giveAttackVelo = giveAttackVelo; }
	//�v���C���[�̍U���̎�i
	AttackTypes GetAttackAttackTypes() { return m_attackType; }
	//�v���C���[�̍U���̑����擾
	AttackAttributes GetAttackAttributes() { return m_attackAttributes; }
	//������������
	Box GetPushBox() { return m_pushBox; }
	//���ꔻ��
	Box GetHitBoxHead() { return  m_hitBoxHead; }
	Box GetHitBoxBody() { return  m_hitBoxBody; }
	Box GetHitBoxLeg() { return  m_hitBoxLeg; }
	Box GetHitBoxAttack() { return  m_hitBoxAttack; }
	//�����蔻��Ƃ�
	void ResetAttackBox() {
		//�U���̔�������Z�b�g
		m_hitBoxAttack.x1 = 0;
		m_hitBoxAttack.y1 = 0;
		m_hitBoxAttack.x2 = 0;
		m_hitBoxAttack.y2 = 0;
	}
	void ResetGraspBox() {
		//�����̔�������Z�b�g
		m_hitBoxGrasp.x1 = 0;
		m_hitBoxGrasp.y1 = 0;
		m_hitBoxGrasp.x2 = 0;
		m_hitBoxGrasp.y2 = 0;
	}
	void ResetHitBox() {
		//���ꔻ������Z�b�g
		//��
		m_hitBoxHead.x1 = 0;
		m_hitBoxHead.y1 = 0;
		m_hitBoxHead.x2 = 0;
		m_hitBoxHead.y2 = 0;
		//��
		m_hitBoxBody.x1 = 0;
		m_hitBoxBody.y1 = 0;
		m_hitBoxBody.x2 = 0;
		m_hitBoxBody.y2 = 0;
		//��
		m_hitBoxLeg.x1 = 0;
		m_hitBoxLeg.y1 = 0;
		m_hitBoxLeg.x2 = 0;
		m_hitBoxLeg.y2 = 0;
		//���܂�锻��
		m_hitBoxThrow.x1 = 0;
		m_hitBoxThrow.y1 = 0;
		m_hitBoxThrow.x2 = 0;
		m_hitBoxThrow.y2 = 0;
	}
	//�����������������
	void ResetPushBox() {
		m_pushBox.x1 = 0;
		m_pushBox.y1 = 0;
		m_pushBox.x2 = 0;
		m_pushBox.y2 = 0;
	}
	//�����������Ƃ����Z�b�g
	void OffIsHitAttack() { m_isHitAttack = false; }
	void OffIsHitGrasp() { m_isHitGrasp = false; }
	//���ꔻ���U������̃Z�b�g
	void SetHitBoxHead(Box hitBoxHead) { m_hitBoxHead = hitBoxHead; }//��
	void SetHitBoxBody(Box hitBoxBody) { m_hitBoxBody = hitBoxBody; }//��
	void SetHitBoxLeg(Box hitBoxLeg) { m_hitBoxLeg = hitBoxLeg; }//��
	void SetHitBoxThrow(Box hitBoxThrow) { m_hitBoxThrow = hitBoxThrow; }//���܂�锻��
	void SetHitBoxAttack(Box hitBoxAttack) { m_hitBoxAttack = hitBoxAttack; }//�U������
	void SetHitBoxGrasp(Box hitBoxGrasp) { m_hitBoxGrasp = hitBoxGrasp; }//���ݔ���
	float GetGiveDamage() const { return m_giveDamage; }//����ɗ^����_���[�W�̎Q��
	int  GetGiveNoActFrame() const { return m_giveNoActFrame; }//����ɗ^����d���t���[���̎Q��
	int  GetGiveGuardFrame() const { return m_giveGuardFrame; }//����ɗ^����K�[�h�d���t���[���̎Q��
	//�O���t�B�b�N��ω�����^�C�~���O���Q�[���}�l�[�W���[���Ă��炤
	void LoadStateHit();//����
	void LoadStateBeThrown();//�������
	void LoadStateThrowEscape();//�����ʂ�
	void LoadStateGuardStand();//�����K�[�h
	void LoadStateGuardSquat();//���Ⴊ�݃K�[�h
	void LoadStateDown();//�_�E��
	void LoadStateDownAerial();//�󒆃_�E��
};

