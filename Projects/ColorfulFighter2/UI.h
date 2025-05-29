#pragma once
#include "Vector3.h"
class GameManager;
class UI
{
private:
	//�`��
	//�v���C���[�A�C�R��
	int m_p1IconHandle;
	int m_p2IconHandle;
	//Hp�o�[
	float m_hpbarMaxP1;// ���݂̗̑�/�ő�l�@��HP�̑傫����\������ۂɎg��(����)
	float m_hpbarMaxP2;
	float m_hpbarP1;
	float m_hpbarP2;
	//�_���[�W�\�L
	int m_damageDisplayCountFrameP1;
	int m_damageDisplayCountFrameP2;
	float m_damagebarP1;
	float m_damagebarP2;
	//Hp�o�[��_��
	int m_blinkHpbarCountFrameP1;
	int m_blinkHpbarIntervalFrameP1;//�_�ł̊Ԋu
	int m_blinkHpbarCountFrameP2;
	int m_blinkHpbarIntervalFrameP2;//�_�ł̊Ԋu
	//HP�̐F
	int m_hpColorP1;
	int m_hpColorP2;
	//�����I���̍ۂ�UI
	int m_koHandle;
	int m_timeupHandle;
	int m_roundFinishHandle;//���̕ϐ���KO��������TimeUp�ɕς��ĕ\������
	//�������ʂ̕\��
	bool m_isResult;
	//���ʂ̕\���ʒu
	Vector3 m_resultUiPos;
	//���E���h�J�n����UI
	int m_startRoundCount;
	int m_roundTextHandle;
	int m_roundNumHandle;
	int m_round1Handle;
	int m_round2Handle;
	int m_round3Handle;
	int m_roundOverHandle;
	int m_fightHandle;
	//�ŏ��̃��E���h�̕���
	Vector3 m_roundUiPos;
	float m_roundUiScale;
	//��������
	int m_winnerHandle;
	int m_drawHandle;
	int m_resultHandle;
	//FIGHT�̕��������񂾂�傫������
	float m_fightScale;
	//�^�C�}�[UI
	void TimerUI(GameManager& gameManager);
	int m_onesPlaceHandle;//1�̈�
	int m_tensPlaceHandle;//10�̈�
	//�^�C�}�[�̐���
	void SetTimeNumHandle(int num,int& handle);
	int m_timeZero;		//0
	int m_timeOne;		//1
	int m_timeTwo;		//2
	int m_timeThree;	//3
	int m_timeFour;		//4
	int m_timeFive;		//5
	int m_timeSix;		//6
	int m_timeSeven;	//7
	int m_timeEight;	//8
	int m_timeNine;		//9
	//HP
	int m_hpFrameHandle;
	//��������UI
	int m_winRoundP1Handle;
	int m_winRoundP2Handle;
	//P1
	int m_winRound1P1Handle;
	int m_winRound2P1Handle;
	//P2
	int m_winRound1P2Handle;
	int m_winRound2P2Handle;
	//�R�}���h�̃A�C�R��
	int m_commandIconHandle[9];
	int m_selectCommandIconP1Handle[3];//P1���I�񂾃R�}���h��\��
	int m_selectCommandIconP2Handle[3];//P2���I�񂾃R�}���h��\��
	//1�t���[���O��HP�̒l��ێ�
	float m_beforeHpP1;
	float m_beforeHpP2;
	//�������I���������ǂ���
	bool m_isFinishRound;
	//KO�܂���TIMEUP�̕����̕\������
	int m_displayFinishRoundCountFrame;
	//�X�v���C�g
	int m_sprite1Handle;
	int m_sprite2Handle;
	int m_sprite3Handle;
	int m_spriteAlpha;
	Vector3 m_spriteVelo;
	//�v���C���[�A�C�R��
	void DrawPlayerIcon();
	//�_���[�W
	void UpdateDamage();
	//KO�������̉��o
	void UpdateRoundFinish(GameManager& gameManager);//���������Ă���̏���
	void DrawFinishUI();//�����┒�w�i�̕\��
	void UpdateKoSpriteFade();//�C���N�����񂾂�����Ă���
	void InitKoSpriteEffect();//�C���N�̈ʒu��������
	void DrawKoSprite();//KO�������̃C���N�̕\��
	//HP�o�[�̏���
	void UpdateHp(float p1Hp, float p2Hp);
	void DrawHpbar();
	void UpdateBlinkHpbar();
	void CheckPinch();
	//�������̕\��
	void DrawWinNum();
	//�I�񂾃R�}���h�Z�̕\��
	void DrawHaveCommand();
	//�^�C�}�[
	void DrawTimer();
	//���E���h�R�[��
	void RoundStartUI();
	//�������v���C���[�̂ق���winner�̕������o����drow(��������)�̕������o��
	void ResultUI();
public:
	UI(int* selectCommandIndexP1, int* selectCommandIndexP2);
	~UI();
	void Init(float p1Hp, float p2Hp, GameManager& gameManager);
	void Update(float p1Hp, float p2Hp, GameManager& gameManager);
	//UI
	void DrawFront();
	void DrawBack();
};

