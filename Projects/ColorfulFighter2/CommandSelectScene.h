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
    //描画関連
    //コマンドのアイコン
    int m_commandIconHandle[9];
    int m_selectCommandIconP1Handle[3];//P1が選んだコマンドを表示
    int m_selectCommandIconP2Handle[3];//P2が選んだコマンドを表示
    int m_nullCommandIconHandle;//コマンド技を選んでいない時に表示する画像
    //コマンド技の説明
    int m_commandInfoHandle[9];
    //カーソル
    int m_cursorP1Handle; //P1のカーソル
    int m_cursorP2Handle;//P2のカーソル
    //コマンド技を3つ選び終わった後に出てくるUI
    int m_gettingReadyHandle;//準備中
    int m_readyHandle;//準備完了
    int m_currentReadyP1Handle;//準備完了かどうかを表示
    int m_currentReadyP2Handle;//準備完了かどうかを表示
    //キャラクターのハンドル(これに状態に合わせたハンドルを入れる)
    int m_charaP1Handle;
    int m_charaP2Handle;
    //キャラクターの色と状態
    int m_idlePoseHandle[5];
    int m_readyPoseHandle[5];
    //背景
    int m_backGroundHandle;
    //テキスト(英語で技を3つ選べって書いてる画像)
    int m_selectTextHandle;
    //操作に関するUI(下に出てるやつ)
    int m_operationHandle;
    //アニメーション関連
    //キャラクターアニメーション
    int m_animIndexP1;
    int m_animIndexP2;
    int m_animCountFrame;
    //カーソルの点滅に使う
    int m_blinkCountFrame;
    //サウンド
    std::shared_ptr<BGM> m_bgm;
    int m_bgmHandle;
    //SE
    std::shared_ptr<SE> m_seP1;
    std::shared_ptr<SE> m_seP2;
    int m_selectSeHandle;//決定音
    int m_readySeHandle;//準備完了音
    int m_cursorMoveSeHandle;//カーソル移動音
    int m_cancelSeHandle;//キャンセル音
    //フェードインするために使う
    std::shared_ptr<FadeManager> m_fadeManager;
    bool m_isFadeOut;
    //コマンド技を選ぶ処理に使う
     //現在の技のセレクト番号
    int m_currentSelectCommandIndexP1;
    int m_currentSelectCommandIndexP2;
    //選んだ技
    int m_selectCommandIndexP1[3];
    int m_selectCommandIndexP2[3];
    //技を3つ決定したらtrue
    bool m_isSelectFinishP1;
    bool m_isSelectFinishP2;
    //すべての準備が終わったらtrue
    bool m_isReadyP1;
    bool m_isReadyP2;
    //キャラクターの色
    //選んでいる色
    int m_currentColorIndexP1;
    int m_currentColorIndexP2;
    //コマンド技の説明を表示するかどうか
    bool m_isCommandInfoDisplayP1;
    bool m_isCommandInfoDisplayP2;
    //関数達
    //コマンド技を選ぶ処理(処理は同じだけど引数が多すぎ複雑になったので分けちゃった)
    void SelectCommandP1(Input& input);
    void SelectCommandP2(Input& input);
	//説明を表示する
	void DrawCommandInfo();
    //選んだコマンド技の描画
    void DrawSelectPlayerCommandIcon();
    //色を選ぶ処理
    void SelectColorP1(Input& input);
    void SelectColorP2(Input& input);
    //準備ができているかを表示
    void DrawReady();
    //9個並んでるアイコンの描画
    void DrawCommandIcon();
    //カーソルの描画
    void DrawCursor();
    //技を選んでください
	void DrawSelectText();
    //キャラクターのアニメーションと描画
    void UpdateCharaAnim();
    void DrawCharacter();
    //点滅
    void UpdateBlink();
    //Loadingの時に出す画像
    int m_loadingHandle;
public:
    CommandSelectScene(SceneController& controller);
    ~CommandSelectScene();
    //派生クラスで実装を実装
    virtual void Update(Input& input, Input& input2) override;
    virtual void Draw() override;
};

