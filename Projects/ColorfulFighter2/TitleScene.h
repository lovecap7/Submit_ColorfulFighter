#pragma once
#include "SceneBase.h"
#include <memory>
#include "Vector3.h"
class BGM;
class SE;
class FadeManager;
class TitleScene :
    public SceneBase
{
private:
    //�`��֘A
    int m_backgroundHandle;//�w�i
	int m_titleHandle;
    int m_textHandle;//press any bottun
    struct Actor//����Ă�l�����̕`��Ɏg��
    {
        int handle;
        int animIndex;	//�A�j���[�V�����̔ԍ��𐔂���(1�����閈�ɃA�j���[�V�������i��)
        int animNum;		//�A�j���[�V�����̐�
        int oneAnimIntervalFrame;	//1���̃A�j���[�V�����ɂ�����t���[��
        /// <summary>
        /// ������
        /// </summary>
        /// <param name="handle">�n���h��</param>
        /// <param name="animNum">�G�̖���</param>
        Actor(int Handle, int AnimNum, int OneAnimIntervalFrame) :
            handle(Handle),
            animIndex(0),
            animNum(AnimNum),
            oneAnimIntervalFrame(OneAnimIntervalFrame)
        {
        }
    };
    //���Ő���Ă���L�����N�^�[
    Actor m_actor1; 
    Actor m_actor2;
    int m_idleHandle;
    int m_punchHandle;
    int m_kickHandle;
    int m_guardHandle;
    int m_walkHandle;
    //�L�����N�^�[�𓮂���
    Vector3 m_actor1Pos;
    Vector3 m_actor2Pos;
    Vector3 m_actor1Velo;
    Vector3 m_actor2Velo;
    //�f���f��
    int m_demoMovieHandle;
    //�A�j���[�V�����֘A
    int m_animCountFrame;//����Ă�L�����N�^�[�̃A�j���[�V�����Ɏg��
    int m_textBlinkFrame;//�����̓_�łɎg��
    int m_titleFadeCountFrame;//�^�C�g�������񂾂�o�Ă��鉉�o�Ɏg��
    //��
    std::shared_ptr<BGM> m_bgm;
    int m_bgmHandle;
    std::shared_ptr<SE> m_se;
	int m_startSeHandle;//�n�܂�Ƃ��̉�
    int m_citySeHandle;//���̑���
    //�t�F�[�h
    std::shared_ptr<FadeManager> m_fadeManager;
    bool m_isFadeOut;
    //�֐�
    void DrawBlinkingText(); //�e�L�X�g���o����������肷��
    void DrawActor();
    //state�p�^�[��
    using UpdateFunc_t = void(TitleScene::*)(Input& input, Input& input2);
    UpdateFunc_t m_update;
    //Draw
    using DrawFunc_t = void(TitleScene::*)();
    DrawFunc_t m_draw;
    //�{�^������������Q�[�����J�n������
	void UpdateNormal(Input& input, Input& input2);
	void DrawNormal();
    //2�l�̃L�����N�^�[���o��I�[�v�j���O�̏��
	void UpdateOpening(Input& input, Input& input2);
	void DrawOpening();
    //�f���f��
	void UpdateDemo(Input& input, Input& input2);
	void DrawDemo();
	//���F�̃t�F�[�h�C����������
    void UpdateGameStart(Input& input, Input& input2);
    void DrawGameStart();
public:
    TitleScene(SceneController& contoller);
    ~TitleScene();
    /// <summary>
    /// �h���N���X�Ŏ���������
    /// virtual �͂�����p�����邩������Ȃ�����
    /// override�͖����I�ɃI�[�o�[���C�h���G�f�B�^�Ɏ�������
    /// </summary>
    virtual void Update(Input& input, Input& input2) override;
    virtual void Draw() override;
};

