#pragma once
#include "Vector3.h"
class GameManager;
class UI
{
private:
	//描画
	//プレイヤーアイコン
	int m_p1IconHandle;
	int m_p2IconHandle;
	//Hpバー
	float m_hpbarMaxP1;// 現在の体力/最大値　でHPの大きさを表示する際に使う(割合)
	float m_hpbarMaxP2;
	float m_hpbarP1;
	float m_hpbarP2;
	//ダメージ表記
	int m_damageDisplayCountFrameP1;
	int m_damageDisplayCountFrameP2;
	float m_damagebarP1;
	float m_damagebarP2;
	//Hpバーを点滅
	int m_blinkHpbarCountFrameP1;
	int m_blinkHpbarIntervalFrameP1;//点滅の間隔
	int m_blinkHpbarCountFrameP2;
	int m_blinkHpbarIntervalFrameP2;//点滅の間隔
	//HPの色
	int m_hpColorP1;
	int m_hpColorP2;
	//試合終了の際のUI
	int m_koHandle;
	int m_timeupHandle;
	int m_roundFinishHandle;//この変数をKOもしくはTimeUpに変えて表示する
	//試合結果の表示
	bool m_isResult;
	//結果の表示位置
	Vector3 m_resultUiPos;
	//ラウンド開始時のUI
	int m_startRoundCount;
	int m_roundTextHandle;
	int m_roundNumHandle;
	int m_round1Handle;
	int m_round2Handle;
	int m_round3Handle;
	int m_roundOverHandle;
	int m_fightHandle;
	//最初のラウンドの文字
	Vector3 m_roundUiPos;
	float m_roundUiScale;
	//試合結果
	int m_winnerHandle;
	int m_drawHandle;
	int m_resultHandle;
	//FIGHTの文字をだんだん大きくする
	float m_fightScale;
	//タイマーUI
	void TimerUI(GameManager& gameManager);
	int m_onesPlaceHandle;//1の位
	int m_tensPlaceHandle;//10の位
	//タイマーの数字
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
	//勝利数のUI
	int m_winRoundP1Handle;
	int m_winRoundP2Handle;
	//P1
	int m_winRound1P1Handle;
	int m_winRound2P1Handle;
	//P2
	int m_winRound1P2Handle;
	int m_winRound2P2Handle;
	//コマンドのアイコン
	int m_commandIconHandle[9];
	int m_selectCommandIconP1Handle[3];//P1が選んだコマンドを表示
	int m_selectCommandIconP2Handle[3];//P2が選んだコマンドを表示
	//1フレーム前のHPの値を保持
	float m_beforeHpP1;
	float m_beforeHpP2;
	//試合が終了したかどうか
	bool m_isFinishRound;
	//KOまたはTIMEUPの文字の表示時間
	int m_displayFinishRoundCountFrame;
	//スプライト
	int m_sprite1Handle;
	int m_sprite2Handle;
	int m_sprite3Handle;
	int m_spriteAlpha;
	Vector3 m_spriteVelo;
	//プレイヤーアイコン
	void DrawPlayerIcon();
	//ダメージ
	void UpdateDamage();
	//KOした時の演出
	void UpdateRoundFinish(GameManager& gameManager);//決着がついてからの処理
	void DrawFinishUI();//文字や白背景の表示
	void UpdateKoSpriteFade();//インクがだんだん消えていく
	void InitKoSpriteEffect();//インクの位置を初期化
	void DrawKoSprite();//KOした時のインクの表現
	//HPバーの処理
	void UpdateHp(float p1Hp, float p2Hp);
	void DrawHpbar();
	void UpdateBlinkHpbar();
	void CheckPinch();
	//勝ち星の表示
	void DrawWinNum();
	//選んだコマンド技の表示
	void DrawHaveCommand();
	//タイマー
	void DrawTimer();
	//ラウンドコール
	void RoundStartUI();
	//勝ったプレイヤーのほうにwinnerの文字を出すかdrow(引き分け)の文字を出す
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

