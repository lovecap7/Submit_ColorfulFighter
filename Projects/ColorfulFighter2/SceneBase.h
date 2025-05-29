#pragma once
class Input;
class SceneController;

/// <summary>
/// 各シーンの基底クラス
/// </summary>
class SceneBase
{
protected:
	//各シーンの状態を切り替えるため
	SceneController& m_controller;
public:
	SceneBase(SceneController& controller);
	/// <summary>
	/// 内部変数の更新
	/// </summary>
	/// <param name="input"></param>
	virtual void Update(Input& input, Input& input2) = 0;
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;
};

