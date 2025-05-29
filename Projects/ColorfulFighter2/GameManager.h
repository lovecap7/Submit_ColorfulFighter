#pragma once
#include "Vector3.h"
#include <memory>
#include <DxLib.h>
#include "game.h"
class Player;
class Bullet;
class Camera;
class CollisionCheck;
class UI;
class FadeManager;
class SE;

class GameManager
{
private:
	int m_hitStopFrameCount;//ヒットストップの時間を数える
	int m_changeRoundFrameCount;//ラウンドが切り替わるまでの時間
	//当たり判定に処理
	std::shared_ptr<CollisionCheck> m_collcheck;
	void CheckKO(Player& p1, Player& p2);
	//フェード
	std::shared_ptr<FadeManager> m_fadeManager;
	bool m_isFadeOut;
	//プレイヤーの勝利数
	int m_winNumP1;
	int m_winNumP2;
	//ラウンド
	int m_roundNumber;
	//ヒットストップ
	bool m_isHitStop;
	int m_hitStopFinishFrame;//何フレーム止めるか
	void HitStop();
	//ラウンドの切り替わり
	bool m_isChangeRound;
	//カメラ揺れる
	bool m_isCameraShake;
	//ゲーム終了
	bool m_isGameset;
	//KO
	bool m_isTimeUpOrKo;
	//ゲーム開始
	bool m_isStartRound;
	int m_startRoundCount;
	//タイマー
	void Timer();
	int m_time;//時間
	int m_countFrame;//フレーム
	//エフェクト
	Vector3 m_hitEffectPosP1;//プレイヤー1が敵を殴った時のエフェクト
	Vector3 m_hitEffectPosP2;//プレイヤー2が敵を殴った時のエフェクト
	int m_redEffectHandle;
	int m_greenEffectHandle;
	int m_purpleEffectHandle;
	int m_blueHandle;
	//再生するハンドル
	int m_hitEffectHandleP1;
	int m_hitEffectHandleP2;
	//ヒットエフェクトP1
	int m_hitAnimIndexP1;//再生している画像の現在の番号
	int m_hitAnimFinishIndexP1;//この番号まで画像が来たら終わり
	int m_hitAnimCountFrameP1;
	float m_effectSizeP1;
	float m_angleP1;
	//ヒットエフェクトP2
	int m_hitAnimIndexP2;//再生している画像の現在の番号
	int m_hitAnimFinishIndexP2;//この番号まで画像が来たら終わり
	int m_hitAnimCountFrameP2;
	float m_effectSizeP2;
	float m_angleP2;
	//SE
	std::shared_ptr<SE> m_se;
	int m_round1SeHandle;
	int m_round2SeHandle;
	int m_round3SeHandle;
	int m_roundOverSeHandle;
	int m_fightSeHandle;
	int m_koSeHandle;
	int m_timeUpSeHandle;
	//プレイヤー1が攻撃を当てた場合プレイヤー1を前に描画(true)
	//プレイヤー2が攻撃を当てた場合プレイヤー1を後ろに描画(false)
	bool m_isDrawFrontP1;
public:
	GameManager();
	~GameManager();
	void Init();
	void Update(Player& p1, Player& p2, Bullet& bulletP1, Bullet& bulletP2,
		Camera& camera,UI& ui);
	void NoStopUpdate(Player& p1, Player& p2);//止めないUpdate
	void Draw(Camera& camera);
	void DrawFade();
	void DrawHitEffect(Camera& camera);
	//ラウンド数
	int GetRoundNumber() { return m_roundNumber; }
	//ヒットストップ
	bool IsHitStop() { return m_isHitStop; }
	void OnIsHitStop() { m_isHitStop = true; }
	bool IsCameraShake() { return m_isCameraShake; }
	void OnIsCameraShake() { m_isCameraShake = true; }
	//ラウンドの切り替わり
	bool IsChangeRound() { return m_isChangeRound; }
	void OffIsChangeRound() { m_isChangeRound = false; }
	//ゲーム終了
	bool IsGameset() { return m_isGameset; }
	//KOかtimeupしたか
	bool IsTimeUpOrKo() { return m_isTimeUpOrKo; }
	//勝利数
	int GetWinNumP1() { return m_winNumP1; }
	int GetWinNumP2() { return m_winNumP2; }
	//ラウンドスタート
	bool IsStartRound() { return m_isStartRound; }
	int GetStartRoundCount() { return m_startRoundCount;}
	//タイマーの取得
	int GetTimer() { return m_time; }
	//エフェクトのポジション
	void SetHitEffectPosP1(Vector3 hitPos) { m_hitEffectPosP1 = hitPos; }
	void SetHitEffectPosP2(Vector3 hitPos) { m_hitEffectPosP2 = hitPos; }
	//ヒットエフェクト、ガードエフェクトを表示するための関数
	void LoadNormalHitEffect(Player& player);
	void LoadSpecialHitEffect(Player& player);
	void LoadGuardEffect(Player& player);
	void LoadScrapeGuardEffect(Player& player);
	//描画順を変えるために使う
	bool GetIsDrawFrontP1() { return m_isDrawFrontP1; }
	void SetIsDrawFrontP1(bool isDrawFront) { m_isDrawFrontP1 = isDrawFront; }
	//勝ったプレイヤーのインデックス
	PlayerIndex ResultWinPlayerIndex();
};

