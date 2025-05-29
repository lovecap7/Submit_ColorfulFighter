#pragma once
#include "SceneBase.h"
#include "game.h"
#include "Vector3.h"
#include <memory>

class Player;
class Input;
class BGM;
class SE;
class FadeManager;
class ResultScene :
    public SceneBase
{
private:
    //�v���C���[�̉摜
    int m_p1Handle;
    int m_p2Handle;
    Vector3 m_p1ImagePos;
    Vector3 m_p2ImagePos;
    //�m���m���ȓ�����������
    double m_p1ImageAngle;
    double m_p2ImageAngle;
    double m_addAngle;
    //�������Ȃ�����傫���Ȃ����肳����
    double m_p1ImageScale;
    double m_p2ImageScale;
    double m_addScale;
	void DrawPlayerImage();
    //�w�i
	int m_backGroundHandle;
	void DrawBackGround();
    //BGM
    std::shared_ptr<BGM> m_bgm;
    int m_resultBgmHandle;
    int m_menuBgmHandle;
    //SE
    std::shared_ptr<SE> m_seP1;
    std::shared_ptr<SE> m_seP2;
    int m_selectSeHandle;//���艹
    int m_cursorMoveSehandle;//�J�[�\���ړ���
    //�Z���t
    int m_serifHandle;
	void DrawSerif();
    //���j���[
	bool m_isSelecting;
    int m_selectMenuIndexP1;
    int m_selectMenuIndexP2;
    void SelectMenu(Input& input,int& selectMenuIndex, bool& isDecide, std::shared_ptr<SE>& se);
    //���肵�����ڂ�����s����֐������߂�
    bool CheckDecidedMenu();
    void Rematch();//�Đ�
    void Reselect();//�R�}���h�̑I�ђ���
	void DrawMenu();
	//�{�^��
    int m_rematchBottunHandle;
    int m_reselectBottunHandle;
    //��Z�p
	int m_p1ColorHandle;
	int m_p2ColorHandle;
    double m_selectBottunScaleP1;
    double m_selectBottunScaleP2;
	void ChangeBottunScale(bool& isSelecting, double& selectBottunScale);
    //���U���g���X�L�b�v���ă��j���[��\��
    void SkipResult(Input& input, Input& input2);
    //���j���[�{�^���̕\��
	void DrawMenuBottunP1();
	void DrawMenuBottunP2();
    //����
	bool m_isDecideP1;
	bool m_isDecideP2;
    //�_��
	int m_blinkCountFrame;//�t���[���𐔂���
	int m_blinkCountP1;//�_�ł̃J�E���g
	int m_blinkCountP2;//�_�ł̃J�E���g
    //�t�F�[�h�C�����邽�߂Ɏg��
    std::shared_ptr<FadeManager> m_fadeManager;
    bool m_isFadeOut;
    //Loading�̎��ɏo���摜
    int m_loadingHandle;
public:
    ResultScene(SceneController& controller);
    ~ResultScene();
    //�h���N���X�Ŏ���������
    virtual void Update(Input& input, Input& input2) override;
    virtual void Draw() override;
};

