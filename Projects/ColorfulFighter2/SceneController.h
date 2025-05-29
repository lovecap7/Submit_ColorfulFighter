#pragma once
//スマートポインタを使う
#include<memory>
#include "game.h"
class Input;
class CharaBase;
class SoundManager;
class SceneBase;

/// <summary>
/// 各シーンを管理するクラス
/// 管理はするが、繊維を管理はしない(繊維はクラスどもがやる)
/// インターフェイスを提供するクラス
/// </summary>
class SceneController
{
private:
	//現在実行中のシーン(中が何かは知らない)
	std::shared_ptr<SceneBase> m_scene;
	//選んだコマンド技のインデックスを保存
	int m_selectCommandIndexP1[3];
	int m_selectCommandIndexP2[3];
	//選んだキャラクターのカラーリングの保存
	CharaColorIndex m_charaColorIndexP1;
	CharaColorIndex m_charaColorIndexP2;
	//勝ったプレイヤーの情報
	PlayerIndex m_winPlayerIndex;
public:
	SceneController();
	/// <summary>
	/// Applicationから呼ばれる関数。現在のシーンのUpdateをそのまま実行。
	/// </summary>
	/// <param name="input"></param>
	void Update(Input& input, Input& input2);
	/// <summary>
	/// Applicationから呼ばれる関数。現在のシーンのDrawをそのまま実行。
	/// </summary>
	/// <param name="input"></param>
	void Draw();
	/// <summary>
	/// 次の状態をセットする(セットするのは各状態の役割)
	/// 各シーンがコントローラーを見て切り替えさせる
	/// </summary>
	/// <param name="">次の状態のポインタ</param>
	void ChangeScene(std::shared_ptr<SceneBase>);
	//次のシーンに引き続きたい情報を保存したり取り出したりする関数達
	//選んだ技の情報
	void SaveSelectCommandIndex(int indexP1[3], int indexP2[3]);//保存
	int* GetSelectCommandIndex(PlayerIndex playerIndex);
	//選んだ色
	void SaveCharaColorIndexP1(CharaColorIndex colorIndex);
	void SaveCharaColorIndexP2(CharaColorIndex colorIndex);
	CharaColorIndex GetCharaColorIndexP1() { return m_charaColorIndexP1; }
	CharaColorIndex GetCharaColorIndexP2() { return m_charaColorIndexP2; }
	//勝ったプレイヤーのインデックス
	void SaveWinPlayerIndex(PlayerIndex winPlayerIndex) { m_winPlayerIndex = winPlayerIndex; }
	PlayerIndex GetWinPlayerIndex() { return m_winPlayerIndex; }
};

