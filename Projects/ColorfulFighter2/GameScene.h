#pragma once
#include "SceneBase.h"
#include <memory>
class Player;
class GameManager;
class UI;
class Camera;
class Bullet;
class CharaBase;
class BGM;

class GameScene :
    public SceneBase
{
private:
    //�|�C���^�[�ǂ�
    std::shared_ptr<Player> m_player1;
    std::shared_ptr<Player> m_player2;
    std::shared_ptr<GameManager> m_gameManager;
    std::shared_ptr<UI> m_ui;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Bullet> m_bullet1;
    std::shared_ptr<Bullet> m_bullet2;
    //�T�E���h
    std::shared_ptr<BGM> m_bgm;
    int m_bgmHandle;
    //�X�e�[�W�̉摜
    int m_floorBaseHandle;//��
    int m_backBaseHandle;//�w�i
    //8�F�̃n���h��
    int m_floorColorHandle[8];//���̐F
    int m_backColorHandle[8];//�w�i�̐F
    void CreateColorHandle();
    //�Q�[���̏�����
    void GameInit();
    //�X�e�[�W
    void DrawStage();
    void UpdateStageColor();
    unsigned int m_color;
    int m_colorAlphaCountFrame;
    int m_colorIndex;
public:
    GameScene(SceneController& controller);
    ~GameScene();
    /// <summary>
    /// �h���N���X�Ŏ���������
    /// </summary>
    /// <param name="input"></param>
    virtual void Update(Input& input, Input& input2) override;
    virtual void Draw() override;
};

