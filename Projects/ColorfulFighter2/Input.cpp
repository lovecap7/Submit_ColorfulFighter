#include "Dxlib.h"
#include "Input.h"
#include "game.h"
namespace
{
	constexpr float kTriggerPower = 128;

	//スティックの入力成立の大きさ
	constexpr int kLeftStickPowerX = 20;
	constexpr int kLeftStickPowerY = 20;

	//コマンド猶予フレーム
	//通常
	constexpr int kDirInputgraceFrame = 15;
	//タメ
	constexpr int kKeepInputgraceFrame = 25;
	//タメに必要なフレーム
	constexpr int kKeepFrame = 30;
}

void Input::DirInfoSave()
{
	//現在のスティックの方向を保存
//一旦現在のスティックの方向をNeutralで初期化
	StickDir nowDir = StickDir::Neutral;
	//右か左に入力が入っているとき
	if ((m_stickInfo.leftStickX < -kLeftStickPowerX) || (kLeftStickPowerX < m_stickInfo.leftStickX) || IsPress("Left") || IsPress("Right"))
	{
		//スティックが上(-500)か下(500)に入っているならtrueでNeutralならfalse
		bool isY = ((m_stickInfo.leftStickY < -kLeftStickPowerY) || (kLeftStickPowerY < m_stickInfo.leftStickY) || IsPress("Up") || IsPress("Down"));
		//スティックが右に入っているならtrueで左ならfalse
		bool isRight = ((m_stickInfo.leftStickX > kLeftStickPowerX) || IsPress("Right"));
		if (isY)
		{
			//下に入力が入っているならtrue
			bool isDown = ((m_stickInfo.leftStickY > kLeftStickPowerY) || IsPress("Down"));
			//下と右に入力が入っているなら右下
			if (isDown && isRight)			 nowDir = StickDir::RightDown;
			//下と左に入力が入っているなら左下
			else if (isDown && !isRight)	 nowDir = StickDir::LeftDown;
			//上と右に入力が入っているなら右上
			else if (!isDown && isRight)	 nowDir = StickDir::RightUp;
			//上と左に入力が入っているなら左上
			else							 nowDir = StickDir::LeftUp;
		}
		//上下どちらも入力が入っていないとき
		else
		{
			if (isRight)	nowDir = StickDir::Right;
			else			nowDir = StickDir::Left;
		}
	}
	//右左に入力がなく上か下に入力が入っているとき
	else if ((m_stickInfo.leftStickY < -kLeftStickPowerY) || (kLeftStickPowerY < m_stickInfo.leftStickY) || IsPress("Up") || IsPress("Down"))
	{
		//下に入力が入っているとき
		if ((m_stickInfo.leftStickY > kLeftStickPowerY) || IsPress("Down"))	nowDir = StickDir::Down;
		//上に入力が入っているとき
		else											nowDir = StickDir::Up;
	}

	//listの中身が空の時	(1回目)
	if (m_stickDirInfo.empty())
	{
		//listの先頭に現在のフレームの方向を入れる
		StickDirInfo info;
		info.frame = 1;
		info.dir = nowDir;
		m_stickDirInfo.push_front(info);
	}
	//2回目以降
	else
	{
		//ひとつ前のフレームの方向と現在の入力の方向が同じなら
		if (m_stickDirInfo.begin()->dir == nowDir)
		{
			//フレームだけ足していく
			m_stickDirInfo.begin()->frame++;
		}
		//ひとつ前のフレームの方向と現在の入力の方向が違うなら
		else
		{
			//listの先頭に現在のフレームの方向を入れる
			StickDirInfo info;
			info.frame = 1;
			info.dir = nowDir;
			m_stickDirInfo.push_front(info);
		}
	}



#if _DEBUG	
	//DrawFormatString(300, 32, 0xffff00, "dir:%d", m_stickDirInfo.begin()->dir);
	DxLib::DrawFormatString(580, 190, 0xffff00, "dir:%d", nowDir);

	DxLib::DrawFormatString(580, 168, 0x00ff00, "%d", StickDir::LeftDown);	//1
	DxLib::DrawFormatString(600, 168, 0x00ff00, "%d", StickDir::Down);		//2
	DxLib::DrawFormatString(620, 168, 0x00ff00, "%d", StickDir::RightDown);	//3
	DxLib::DrawFormatString(580, 142, 0x00ff00, "%d", StickDir::Left);		//4
	DxLib::DrawFormatString(600, 142, 0x00ff00, "%d", StickDir::Neutral);	//5
	DxLib::DrawFormatString(620, 142, 0x00ff00, "%d", StickDir::Right);		//6
	DxLib::DrawFormatString(580, 116, 0x00ff00, "%d", StickDir::LeftUp);	//7
	DxLib::DrawFormatString(600, 116, 0x00ff00, "%d", StickDir::Up);		//8
	DxLib::DrawFormatString(620, 116, 0x00ff00, "%d", StickDir::RightUp);	//9
#endif
}

Input::~Input()
{
}

void Input::Init()
{
	m_inputActionMap["Ok"] = { {InputType::kKeyboard,KEY_INPUT_RETURN},{InputType::kPad,PAD_INPUT_A} };
	m_inputActionMap["Pause"] = { {InputType::kKeyboard,KEY_INPUT_P}, {InputType::kPad,PAD_INPUT_START} };
	m_inputActionMap["Start"] = { {InputType::kKeyboard,KEY_INPUT_P}, {InputType::kPad,PAD_INPUT_8} };
	m_inputActionMap["Select"] = { {InputType::kKeyboard,KEY_INPUT_TAB}, {InputType::kPad,PAD_INPUT_7} };
	m_inputActionMap["Cancel"] = { {InputType::kKeyboard,KEY_INPUT_ESCAPE}, {InputType::kPad,PAD_INPUT_B} };
	m_inputActionMap["Up"] = { {InputType::kKeyboard,KEY_INPUT_W}, {InputType::kPad,PAD_INPUT_UP} };
	m_inputActionMap["Down"] = { {InputType::kKeyboard,KEY_INPUT_S}, {InputType::kPad,PAD_INPUT_DOWN} };
	m_inputActionMap["Left"] = { {InputType::kKeyboard,KEY_INPUT_A}, {InputType::kPad,PAD_INPUT_LEFT} };
	m_inputActionMap["Right"] = { {InputType::kKeyboard,KEY_INPUT_D}, {InputType::kPad,PAD_INPUT_RIGHT} };
	m_inputActionMap["LB"] = { {InputType::kKeyboard,KEY_INPUT_J}, {InputType::kPad,PAD_INPUT_5} };
	m_inputActionMap["RB"] = { {InputType::kKeyboard,KEY_INPUT_L}, {InputType::kPad,PAD_INPUT_6} };
	m_inputActionMap["A"] = { {InputType::kKeyboard,KEY_INPUT_U}, {InputType::kPad,PAD_INPUT_1} };
	m_inputActionMap["B"] = { {InputType::kKeyboard,KEY_INPUT_H}, {InputType::kPad,PAD_INPUT_2} };
	m_inputActionMap["X"] = { {InputType::kKeyboard,KEY_INPUT_G}, {InputType::kPad,PAD_INPUT_3} };
	m_inputActionMap["Y"] = { {InputType::kKeyboard,KEY_INPUT_Y}, {InputType::kPad,PAD_INPUT_4} };
	//コマンドリスト(技を打つキャラクターの向きが右を向いているとき)
	// 7 8 9
	// 4 5 6
	// 1 2 3
	

	
	// 入力が新しいほうからチェックしていくので成立コマンドは後ろから書く
	//コマンド
	// 波動拳
	m_commandList["236"] = { StickDir::Right,StickDir::RightDown ,StickDir::Down };	//右向き
	m_commandList["214"] = { StickDir::Left,StickDir::LeftDown ,StickDir::Down };	//左向き
	//昇竜拳
	m_commandList["623"] = { StickDir::RightDown,StickDir::Down, StickDir::Right };	//右向き
	m_commandList["421"] = { StickDir::LeftDown,StickDir::Down, StickDir::Left };//左向き
	//昇竜簡易コマンド
	m_commandList["323"] = { StickDir::RightDown,StickDir::Down, StickDir::RightDown };	//右向き
	m_commandList["121"] = { StickDir::LeftDown,StickDir::Down, StickDir::LeftDown };//左向き
	m_commandList["636"] = { StickDir::Right,StickDir::RightDown, StickDir::Right };	//右向き
	m_commandList["414"] = { StickDir::Left,StickDir::LeftDown, StickDir::Left };//左向き

	//タメコマンド(K = keep)
	//後ろタメ前
	// ソニックブーム
	m_commandList["4K6"] = { StickDir::Right ,StickDir::Left };	//右向き
	m_commandList["6K4"] = { StickDir::Left ,StickDir::Right };	//左向きソニックブーム
	//下タメ上
	m_commandList["2K8"] = { StickDir::Up ,StickDir::Down };	
	

	//スクリュー
	m_commandList["RightOneRevolution"] = { StickDir::LeftUp ,StickDir::Left,StickDir::Down, StickDir::Right };	//右から
	m_commandList["LeftOneRevolution"] = { StickDir::RightUp ,StickDir::Right,StickDir::Down, StickDir::Left };	//左から

	//半回転
	m_commandList["HalfTurnRightStart"] = { StickDir::Left ,StickDir::Down,StickDir::Right };	//右向き
	m_commandList["HalfTurnLeftStart"] = { StickDir::Right ,StickDir::Down,StickDir::Left };	//左向き

	//パッドの番号
	m_padIndex = 1;
}

void Input::PadInit(int padIndex)
{
	m_padIndex = padIndex;
}

void Input::Update()
{
	//前のフレームの入力情報を保存する
	m_lastInput = m_currentInput;

	//すべての入力を取得する
	char keyState[256] = {};
	int padState = {};
	int mouseState = {};
	GetHitKeyStateAll(keyState);
	if (m_padIndex == static_cast<int>(PlayerIndex::Player2))
	{
		//2P
		padState = GetJoypadInputState(DX_INPUT_PAD2);
	}
	else
	{
		//1P
		padState = GetJoypadInputState(DX_INPUT_PAD1);
	}
	mouseState = GetMouseInput();

	//アクション名に割り当てられているすべてのキーの入力をチェックする
	for (const auto& keyInfo : m_inputActionMap)
	{
		bool isPress = false;
		for (const auto& inputInfo : keyInfo.second)
		{
			//2Pのみキーボード対応
			if (m_padIndex == static_cast<int>(PlayerIndex::Player2))
			{
				//キーボードのチェック
				if (inputInfo.type == InputType::kKeyboard && keyState[inputInfo.buttonID])
				{
					isPress = true;
				}
				//マウスのチェック
				if (inputInfo.type == InputType::kMouse && mouseState & inputInfo.buttonID)
				{
					isPress = true;
				}
			}
			//パッドのチェック
			if (inputInfo.type == InputType::kPad && padState & inputInfo.buttonID)
			{
				isPress = true;
			}

			if (isPress)
			{
				break;
			}
		}
		//現在のフレームで押されていたかどうかを返す
		m_currentInput[keyInfo.first] = isPress;
	}

	//スティックの入力を初期化する
	m_stickInfo.leftStickX = 0;
	m_stickInfo.leftStickY = 0;
	m_stickInfo.rightStickX = 0;
	m_stickInfo.rightStickY = 0;

	//スティックの入力を取得する
	if (m_padIndex == static_cast<int>(PlayerIndex::Player2))
	{
		GetJoypadAnalogInput(&m_stickInfo.leftStickX, &m_stickInfo.leftStickY, DX_INPUT_PAD2);
		GetJoypadAnalogInputRight(&m_stickInfo.rightStickX, &m_stickInfo.rightStickY, DX_INPUT_PAD2);
		XINPUT_STATE* xInputState = new XINPUT_STATE;
		GetJoypadXInputState(DX_INPUT_PAD2, xInputState);
		m_triggerInfo.left = xInputState->LeftTrigger;
		m_triggerInfo.right = xInputState->RightTrigger;
	}
	else
	{
		GetJoypadAnalogInput(&m_stickInfo.leftStickX, &m_stickInfo.leftStickY, DX_INPUT_PAD1);
		GetJoypadAnalogInputRight(&m_stickInfo.rightStickX, &m_stickInfo.rightStickY, DX_INPUT_PAD1);
		XINPUT_STATE* xInputState = new XINPUT_STATE;
		GetJoypadXInputState(DX_INPUT_PAD1, xInputState);
		m_triggerInfo.left = xInputState->LeftTrigger;
		m_triggerInfo.right = xInputState->RightTrigger;
	}

	//方向キーを保存
	DirInfoSave();
	
}

bool Input::CheckDirCommand(std::string command)
{
	//フレームを数えるために用意
	int frame = 0;
	//コマンド成立の方向キーの最初を0とする
	int index = 0;
	//これまでの入力内容を見ていく
	for (auto& data : m_stickDirInfo)
	{
		//入力内容にコマンドの条件に当てはまるキーがあった時
		if (data.dir == m_commandList.at(command).at(index))
		{
			//auto check = m_commandList.at(command).at(index);
			//フレームをリセット
			//frame = 0;
			//次の方向キーをチェックするためにindexを1増やす
			index++;
			//コマンドの方向キーの条件をすべて満たしたらtrueを返す
			if (index == m_commandList.at(command).size())
			{
				return true;
			}
		}
		//フレームを足していく
		frame += data.frame;
		//コマンドの猶予フレームを超えたらfalse
		if (frame > kDirInputgraceFrame)return false;
	}

	return false;
}

bool Input::CheckKeepCommand(std::string command)
{
	//フレームを数えるために用意
	int frame = 0;
	//タメのフレームを数えるために用意
	int keepFrame = 0;
	//コマンド成立の方向キーの最初を0とする
	int index = 0;

	//これまでの入力内容を見ていく
	for (auto& data : m_stickDirInfo)
	{
		//タメの部分だけ斜めでも成立していることにしたい
		if (index == (m_commandList.at(command).size() - 1))
		{
			//元の方向を覚えておく
			StickDir originDir = data.dir;
			if ((data.dir == StickDir::RightUp) || (data.dir == StickDir::RightDown))
			{
				//右として扱う
				data.dir =  StickDir::Right ;
				//失敗してるならもとに戻す
				if (!(data.dir == m_commandList.at(command).at(index)))
				{
					data.dir = originDir;
				}
			}
			if ((data.dir == StickDir::LeftUp) || (data.dir == StickDir::LeftDown))
			{
				//左として扱う
				data.dir = StickDir::Left;
				//失敗してるならもとに戻す
				if (!(data.dir == m_commandList.at(command).at(index)))
				{
					data.dir = originDir;
				}
			}
			if ((data.dir == StickDir::RightDown) || (data.dir == StickDir::LeftDown))
			{
				//下として扱う
				data.dir = StickDir::Down;
				//失敗してるならもとに戻す
				if (!(data.dir == m_commandList.at(command).at(index)))
				{
					data.dir = originDir;
				}
			}
		}
		else//タメの部分以外の時
		{
			//下タメ上のコマンドの際[上]の入力を斜め上での成立するようにしたいので
			//[斜め上]を[上]として扱う
			if (data.dir == StickDir::RightUp || data.dir == StickDir::LeftUp)
			{
				data.dir = StickDir::Up;
			}
		}
		//入力内容にコマンドの条件に当てはまるキーがあった時
		if (data.dir == m_commandList.at(command).at(index))
		{
			//次の方向キーをチェックするためにindexを1増やす
			index++;
			//コマンドの方向キーのタメの部分をチェック
			if (index == m_commandList.at(command).size())
			{
				//成功しているならタメのフレームを足していく
				keepFrame += data.frame;
				//フレームが40フレームを超えてるか
				if (keepFrame > kKeepFrame)
				{
					//成立
					return true;
				}
				else
				{
					//いったん戻す
					index = 1;
					continue;
				}
			}
		}
		//フレームを足していく
		frame += data.frame;
		//コマンドの猶予フレームを超えたらfalse
		if (frame > kKeepInputgraceFrame)return false;
	}
	return false;
}






void Input::StopUpdate()
{
	for (auto& item : m_currentInput)
	{
		item.second = false;
	}
	for (auto& item : m_lastInput)
	{
		item.second = false;
	}

	m_stickInfo.leftStickX = 0;
	m_stickInfo.leftStickY = 0;
	m_stickInfo.rightStickX = 0;
	m_stickInfo.rightStickY = 0;

	m_triggerInfo.left = 0;
	m_triggerInfo.right = 0;
}

bool Input::IsPress(const std::string& action)const
{
	auto keyInfo = m_currentInput.find(action);

	//ボタン名が定義されていなかったらfalseを返す
	if (keyInfo == m_currentInput.end())
	{
		return false;
	}
	else
	{
		//見つかったらboolの値を返す
		return keyInfo->second;
	}
}

bool Input::IsTrigger(const std::string& action)const
{
	if (IsPress(action))
	{
		//前のフレームを参照
		auto last = m_lastInput.find(action);
		//未定義のボタン名が来たらfalseを返す
		if (last == m_lastInput.end())
		{
			return false;
		}
		else
		{
			//前のフレームでも押されていたら
			return !last->second;
		}
	}
	else
	{
		return false;
	}
}

bool Input::IsRelease(const std::string& action)const
{
	//まず押されているかを判定
	if (IsPress(action))
	{
		//押されていたらfalseを返す
		return false;
	}
	else
	{
		//前のフレームを参照
		auto last = m_lastInput.find(action);
		//押されていたらfalseを返す
		if (last == m_lastInput.end())
		{
			return false;
		}
		//押されていなければ
		else
		{
			return last->second;
		}
	}

	return false;
}

bool Input::IsPushTrigger(bool right, int power)
{
	//トリガーの状態取得
	auto trigger = GetTriggerInfo();

	//右側を判定する場合
	if (right)
	{
		//引数よりも強く押し込まれたら
		if (trigger.right >= power)
		{
			return true;
		}
	}
	//左側を判定する場合
	else
	{
		//引数よりも強く押し込まれたら
		if (trigger.left >= power)
		{
			return true;
		}
	}
	return false;
}

bool Input::IsPushTrigger(bool right)
{
	return IsPushTrigger(right, kTriggerPower);
}

//格ゲー用
