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
    //ポインターども
    std::shared_ptr<Player> m_player1;
    std::shared_ptr<Player> m_player2;
    std::shared_ptr<GameManager> m_gameManager;
    std::shared_ptr<UI> m_ui;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Bullet> m_bullet1;
    std::shared_ptr<Bullet> m_bullet2;
    //サウンド
    std::shared_ptr<BGM> m_bgm;
    int m_bgmHandle;
    //ステージの画像
    int m_floorBaseHandle;//床
    int m_backBaseHandle;//背景
    //8色のハンドル
    int m_floorColorHandle[8];//床の色
    int m_backColorHandle[8];//背景の色
    void CreateColorHandle();
    //ゲームの初期化
    void GameInit();
    //ステージ
    void DrawStage();
    void UpdateStageColor();
    unsigned int m_color;
    int m_colorAlphaCountFrame;
    int m_colorIndex;
public:
    GameScene(SceneController& controller);
    ~GameScene();
    /// <summary>
    /// 派生クラスで実装を実装
    /// </summary>
    /// <param name="input"></param>
    virtual void Update(Input& input, Input& input2) override;
    virtual void Draw() override;
};

