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
    //プレイヤーの画像
    int m_p1Handle;
    int m_p2Handle;
    Vector3 m_p1ImagePos;
    Vector3 m_p2ImagePos;
    //ノリノリな動きをさせる
    double m_p1ImageAngle;
    double m_p2ImageAngle;
    double m_addAngle;
    //小さくなったら大きくなったりさせる
    double m_p1ImageScale;
    double m_p2ImageScale;
    double m_addScale;
	void DrawPlayerImage();
    //背景
	int m_backGroundHandle;
	void DrawBackGround();
    //BGM
    std::shared_ptr<BGM> m_bgm;
    int m_resultBgmHandle;
    int m_menuBgmHandle;
    //SE
    std::shared_ptr<SE> m_seP1;
    std::shared_ptr<SE> m_seP2;
    int m_selectSeHandle;//決定音
    int m_cursorMoveSehandle;//カーソル移動音
    //セリフ
    int m_serifHandle;
	void DrawSerif();
    //メニュー
	bool m_isSelecting;
    int m_selectMenuIndexP1;
    int m_selectMenuIndexP2;
    void SelectMenu(Input& input,int& selectMenuIndex, bool& isDecide, std::shared_ptr<SE>& se);
    //決定した項目から実行する関数を決める
    bool CheckDecidedMenu();
    void Rematch();//再戦
    void Reselect();//コマンドの選び直し
	void DrawMenu();
	//ボタン
    int m_rematchBottunHandle;
    int m_reselectBottunHandle;
    //乗算用
	int m_p1ColorHandle;
	int m_p2ColorHandle;
    double m_selectBottunScaleP1;
    double m_selectBottunScaleP2;
	void ChangeBottunScale(bool& isSelecting, double& selectBottunScale);
    //リザルトをスキップしてメニューを表示
    void SkipResult(Input& input, Input& input2);
    //メニューボタンの表示
	void DrawMenuBottunP1();
	void DrawMenuBottunP2();
    //決定
	bool m_isDecideP1;
	bool m_isDecideP2;
    //点滅
	int m_blinkCountFrame;//フレームを数える
	int m_blinkCountP1;//点滅のカウント
	int m_blinkCountP2;//点滅のカウント
    //フェードインするために使う
    std::shared_ptr<FadeManager> m_fadeManager;
    bool m_isFadeOut;
    //Loadingの時に出す画像
    int m_loadingHandle;
public:
    ResultScene(SceneController& controller);
    ~ResultScene();
    //派生クラスで実装を実装
    virtual void Update(Input& input, Input& input2) override;
    virtual void Draw() override;
};

