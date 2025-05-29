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
    //描画関連
    int m_backgroundHandle;//背景
	int m_titleHandle;
    int m_textHandle;//press any bottun
    struct Actor//戦ってる人たちの描画に使う
    {
        int handle;
        int animIndex;	//アニメーションの番号を数える(1増える毎にアニメーションが進む)
        int animNum;		//アニメーションの数
        int oneAnimIntervalFrame;	//1枚のアニメーションにかかるフレーム
        /// <summary>
        /// 初期化
        /// </summary>
        /// <param name="handle">ハンドル</param>
        /// <param name="animNum">絵の枚数</param>
        Actor(int Handle, int AnimNum, int OneAnimIntervalFrame) :
            handle(Handle),
            animIndex(0),
            animNum(AnimNum),
            oneAnimIntervalFrame(OneAnimIntervalFrame)
        {
        }
    };
    //裏で戦っているキャラクター
    Actor m_actor1; 
    Actor m_actor2;
    int m_idleHandle;
    int m_punchHandle;
    int m_kickHandle;
    int m_guardHandle;
    int m_walkHandle;
    //キャラクターを動かす
    Vector3 m_actor1Pos;
    Vector3 m_actor2Pos;
    Vector3 m_actor1Velo;
    Vector3 m_actor2Velo;
    //デモ映像
    int m_demoMovieHandle;
    //アニメーション関連
    int m_animCountFrame;//戦ってるキャラクターのアニメーションに使う
    int m_textBlinkFrame;//文字の点滅に使う
    int m_titleFadeCountFrame;//タイトルがだんだん出てくる演出に使う
    //音
    std::shared_ptr<BGM> m_bgm;
    int m_bgmHandle;
    std::shared_ptr<SE> m_se;
	int m_startSeHandle;//始まるときの音
    int m_citySeHandle;//町の騒音
    //フェード
    std::shared_ptr<FadeManager> m_fadeManager;
    bool m_isFadeOut;
    //関数
    void DrawBlinkingText(); //テキストが出たり消えたりする
    void DrawActor();
    //stateパターン
    using UpdateFunc_t = void(TitleScene::*)(Input& input, Input& input2);
    UpdateFunc_t m_update;
    //Draw
    using DrawFunc_t = void(TitleScene::*)();
    DrawFunc_t m_draw;
    //ボタンを押したらゲームが開始する状態
	void UpdateNormal(Input& input, Input& input2);
	void DrawNormal();
    //2人のキャラクターが出会うオープニングの状態
	void UpdateOpening(Input& input, Input& input2);
	void DrawOpening();
    //デモ映像
	void UpdateDemo(Input& input, Input& input2);
	void DrawDemo();
	//白色のフェードインが入る状態
    void UpdateGameStart(Input& input, Input& input2);
    void DrawGameStart();
public:
    TitleScene(SceneController& contoller);
    ~TitleScene();
    /// <summary>
    /// 派生クラスで実装を実装
    /// virtual はこれを継承するかもしれないから
    /// overrideは明示的にオーバーライドをエディタに示すため
    /// </summary>
    virtual void Update(Input& input, Input& input2) override;
    virtual void Draw() override;
};

