#pragma once
#include "SceneBase.h"
#include "Vector3.h"
#include <memory>

class BGM;
class SE;
class CharaBase;
class FadeManager;
class CommandSelectScene :
    public SceneBase
{
private:
    //�`��֘A
    //�R�}���h�̃A�C�R��
    int m_commandIconHandle[9];
    int m_selectCommandIconP1Handle[3];//P1���I�񂾃R�}���h��\��
    int m_selectCommandIconP2Handle[3];//P2���I�񂾃R�}���h��\��
    int m_nullCommandIconHandle;//�R�}���h�Z��I��ł��Ȃ����ɕ\������摜
    //�R�}���h�Z�̐���
    int m_commandInfoHandle[9];
    //�J�[�\��
    int m_cursorP1Handle; //P1�̃J�[�\��
    int m_cursorP2Handle;//P2�̃J�[�\��
    //�R�}���h�Z��3�I�яI�������ɏo�Ă���UI
    int m_gettingReadyHandle;//������
    int m_readyHandle;//��������
    int m_currentReadyP1Handle;//�����������ǂ�����\��
    int m_currentReadyP2Handle;//�����������ǂ�����\��
    //�L�����N�^�[�̃n���h��(����ɏ�Ԃɍ��킹���n���h��������)
    int m_charaP1Handle;
    int m_charaP2Handle;
    //�L�����N�^�[�̐F�Ə��
    int m_idlePoseHandle[5];
    int m_readyPoseHandle[5];
    //�w�i
    int m_backGroundHandle;
    //�e�L�X�g(�p��ŋZ��3�I�ׂ��ď����Ă�摜)
    int m_selectTextHandle;
    //����Ɋւ���UI(���ɏo�Ă���)
    int m_operationHandle;
    //�A�j���[�V�����֘A
    //�L�����N�^�[�A�j���[�V����
    int m_animIndexP1;
    int m_animIndexP2;
    int m_animCountFrame;
    //�J�[�\���̓_�łɎg��
    int m_blinkCountFrame;
    //�T�E���h
    std::shared_ptr<BGM> m_bgm;
    int m_bgmHandle;
    //SE
    std::shared_ptr<SE> m_seP1;
    std::shared_ptr<SE> m_seP2;
    int m_selectSeHandle;//���艹
    int m_readySeHandle;//����������
    int m_cursorMoveSeHandle;//�J�[�\���ړ���
    int m_cancelSeHandle;//�L�����Z����
    //�t�F�[�h�C�����邽�߂Ɏg��
    std::shared_ptr<FadeManager> m_fadeManager;
    bool m_isFadeOut;
    //�R�}���h�Z��I�ԏ����Ɏg��
     //���݂̋Z�̃Z���N�g�ԍ�
    int m_currentSelectCommandIndexP1;
    int m_currentSelectCommandIndexP2;
    //�I�񂾋Z
    int m_selectCommandIndexP1[3];
    int m_selectCommandIndexP2[3];
    //�Z��3���肵����true
    bool m_isSelectFinishP1;
    bool m_isSelectFinishP2;
    //���ׂĂ̏������I�������true
    bool m_isReadyP1;
    bool m_isReadyP2;
    //�L�����N�^�[�̐F
    //�I��ł���F
    int m_currentColorIndexP1;
    int m_currentColorIndexP2;
    //�R�}���h�Z�̐�����\�����邩�ǂ���
    bool m_isCommandInfoDisplayP1;
    bool m_isCommandInfoDisplayP2;
    //�֐��B
    //�R�}���h�Z��I�ԏ���(�����͓��������ǈ��������������G�ɂȂ����̂ŕ����������)
    void SelectCommandP1(Input& input);
    void SelectCommandP2(Input& input);
	//������\������
	void DrawCommandInfo();
    //�I�񂾃R�}���h�Z�̕`��
    void DrawSelectPlayerCommandIcon();
    //�F��I�ԏ���
    void SelectColorP1(Input& input);
    void SelectColorP2(Input& input);
    //�������ł��Ă��邩��\��
    void DrawReady();
    //9����ł�A�C�R���̕`��
    void DrawCommandIcon();
    //�J�[�\���̕`��
    void DrawCursor();
    //�Z��I��ł�������
	void DrawSelectText();
    //�L�����N�^�[�̃A�j���[�V�����ƕ`��
    void UpdateCharaAnim();
    void DrawCharacter();
    //�_��
    void UpdateBlink();
    //Loading�̎��ɏo���摜
    int m_loadingHandle;
public:
    CommandSelectScene(SceneController& controller);
    ~CommandSelectScene();
    //�h���N���X�Ŏ���������
    virtual void Update(Input& input, Input& input2) override;
    virtual void Draw() override;
};

