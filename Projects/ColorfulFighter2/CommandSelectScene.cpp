#include "CommandSelectScene.h"
#include "Dxlib.h"
#include "Input.h"
#include "SceneController.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "game.h"
#include "Chara.h"
#include "BGM.h"
#include "SE.h"
#include "FadeManager.h"

namespace
{
	constexpr int kCenterX = Game::kScreenWidth / 2;
	constexpr int kCenterY = Game::kScreenHeight / 2;
	//カーソル
	constexpr int kCursorOffset = 50;
	//キャラの画像の大きさ
	constexpr int kCharaWidth = 512;
	constexpr int kCharaHeight = 512;
	//キャラの表示位置
	constexpr int kCharaPosXP1 = 200;
	constexpr int kCharaPosXP2 = Game::kScreenWidth - 200;
	constexpr int kCharaPosY = 250;
	//カラバリの数
	constexpr int kCharaColorNum = 5;

	//アニメーション
	constexpr int kAnimNum = 5;//待機状態と準備完了のアニメーションの数を同じにする
	constexpr int kOneAnimFrame = 5;

	//コマンド技の数
	constexpr int kCommandIndexMin = 1;
	constexpr int kCommandIndexMax = 9;
	//段
	constexpr int kOneRowCommandNum = 3;
	constexpr int kOneColumnCommandNum = 3;

	//アイコンの半径
	constexpr int kIconRadius = 50;
	constexpr int kIconPosOffset = 200;
	//コマンドの数
	constexpr int kCommandNum = 9;
	//コマンドアイコンの大きさ
	constexpr float kCommandIconImageScale = 0.5f;
	constexpr int kCommandIconImageWidth = 200;
	constexpr int kCommandIconImageHight = 200;
	//選べるコマンド技の数
	constexpr int kSelectCommandNum = 3;
	//選んだ技の表示位置
	constexpr float kSelectedCommandOffsetPosX = kCommandIconImageWidth * kCommandIconImageScale;
	constexpr float kSelectedCommandPosY = Game::kScreenHeight / 2.0f + 100.0f;

	//Ready
	constexpr float kReadyPosXP1 = 50.0f;
	constexpr float kReadyPosXP2 = Game::kScreenWidth - 350.0f;
	constexpr float kReadyPosY = kSelectedCommandPosY + kIconRadius;

	//BGMボリューム
	constexpr int kBgmVolume = 120;
	//SEボリューム
	constexpr int kSeVolume = 150;
	constexpr int kOkSeVolume = 180;

	//点滅のスパン
	constexpr int kBlinkSpan = 50;

	//コマンドの設定
	constexpr int kCommandInfoPosXP1 = kCenterX - 500;
	constexpr int kCommandInfoPosXP2 = kCenterX + 500;
	constexpr int kCommandInfoPosY = kCenterY - 200;
}

void CommandSelectScene::UpdateBlink()
{
	++m_blinkCountFrame;
	if (m_blinkCountFrame >= kBlinkSpan * 2)
	{
		m_blinkCountFrame = 0;
	}
}

void CommandSelectScene::UpdateCharaAnim()
{
	//アニメーション
	++m_animCountFrame;
	int animMaxNum = kAnimNum - 1;
	//アニメーションのフレームを数える
	if (m_animCountFrame % kOneAnimFrame == 0 && m_animCountFrame != 0)
	{
		m_animIndexP1++;
		m_animIndexP2++;
		//アニメーションの数が最大まで行ったとき
		if (m_animIndexP1 > animMaxNum)
		{
			if (m_isReadyP1)
			{
				m_animIndexP1 = animMaxNum;
			}
			else
			{
				m_animIndexP1 = 0;
			}
		}
		if (m_animIndexP2 > animMaxNum)
		{
			if (m_isReadyP2)
			{
				m_animIndexP2 = animMaxNum;
			}
			else
			{
				m_animIndexP2 = 0;
			}
		}
	}
}


void CommandSelectScene::SelectCommandP1(Input& input)
{
	//準備完了を互いにしている場合は操作できない
	if (m_isReadyP1 && m_isReadyP2)
	{
		return;
	}
	if (m_isSelectFinishP1)
	{
		//説明を非表示
		m_isCommandInfoDisplayP1 = false;

		if (input.IsTrigger("A") && !m_isReadyP1)
		{
			//決定の音
			m_seP1->Stop();
			m_seP1->SetSE(m_readySeHandle);
			m_seP1->Volume(kOkSeVolume);
			m_seP1->PlayOnce();
			//準備完了
			m_currentReadyP1Handle = m_readyHandle;
			m_isReadyP1 = true;
			m_animIndexP1 = 0;
		}
		//キャンセル
		if (input.IsTrigger("B"))
		{
			//取り消す
			if (m_isReadyP1)
			{
				//キャンセルの音
				m_seP1->Stop();
				m_seP1->SetSE(m_cancelSeHandle);
				m_seP1->Volume(kSeVolume);
				m_seP1->PlayOnce();
				//確認に変換
				m_currentReadyP1Handle = m_gettingReadyHandle;
				m_isReadyP1 = false;
			}
			else//また選び直す
			{
				//キャンセルの音
				m_seP1->Stop();
				m_seP1->SetSE(m_cancelSeHandle);
				m_seP1->Volume(kSeVolume);
				m_seP1->PlayOnce();
				m_isSelectFinishP1 = false;
				for (int i = 0; i < kSelectCommandNum - 1; ++i)
				{
					//最後に選んだ順に消す
					if (m_selectCommandIndexP1[(kSelectCommandNum - 1) - i] != 0)
					{
						m_selectCommandIndexP1[(kSelectCommandNum - 1) - i] = 0;
						m_selectCommandIconP1Handle[(kSelectCommandNum - 1) - i] = m_nullCommandIconHandle;
						break;
					}
				}
			}

		}
	}
	else
	{
		//選ぶ
		if (input.IsTrigger("Left"))
		{
			//カーソル移動の音
			m_seP1->Stop();
			m_seP1->SetSE(m_cursorMoveSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();

			m_currentSelectCommandIndexP1--;
			if (m_currentSelectCommandIndexP1 % 3 == 0)
			{
				m_currentSelectCommandIndexP1 += kOneRowCommandNum;
			}
		}
		if (input.IsTrigger("Right"))
		{
			//カーソル移動の音
			m_seP1->Stop();
			m_seP1->SetSE(m_cursorMoveSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();

			m_currentSelectCommandIndexP1++;
			if ((m_currentSelectCommandIndexP1 - 1) % kOneRowCommandNum == 0)
			{
				m_currentSelectCommandIndexP1 -= kOneRowCommandNum;
			}
		}
		if (input.IsTrigger("Up"))
		{
			//カーソル移動の音
			m_seP1->Stop();
			m_seP1->SetSE(m_cursorMoveSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();

			m_currentSelectCommandIndexP1 -= kOneColumnCommandNum;
			if (m_currentSelectCommandIndexP1 < kCommandIndexMin)
			{
				m_currentSelectCommandIndexP1 += (kOneColumnCommandNum * kOneRowCommandNum);
			}
		}
		if (input.IsTrigger("Down"))
		{
			//カーソル移動の音
			m_seP1->Stop();
			m_seP1->SetSE(m_cursorMoveSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();

			m_currentSelectCommandIndexP1 += kOneColumnCommandNum;
			if (m_currentSelectCommandIndexP1 > kCommandIndexMax)
			{
				m_currentSelectCommandIndexP1 -= (kOneColumnCommandNum * kOneRowCommandNum);
			}

		}
		if (input.IsTrigger("A"))
		{
			//決定の音
			m_seP1->Stop();
			m_seP1->SetSE(m_selectSeHandle);
			m_seP1->Volume(kOkSeVolume);
			m_seP1->PlayOnce();
			for (int i = 0; i < kSelectCommandNum; ++i)
			{
				//同じ技は選べない
				if (m_selectCommandIndexP1[i] == m_currentSelectCommandIndexP1)
				{
					break;
				}
				//空ならインデックスを入れる
				if (m_selectCommandIndexP1[i] == 0)
				{
					m_selectCommandIndexP1[i] = m_currentSelectCommandIndexP1;
					m_selectCommandIconP1Handle[i] = m_commandIconHandle[m_selectCommandIndexP1[i] - 1];//アイコン表示
					break;
				}
			}
			//3つ選んだらカーソルを動かせなくする
			if (m_selectCommandIndexP1[(kSelectCommandNum - 1)] != 0)
			{
				m_isSelectFinishP1 = true;
			}
		}
		//選びなおす
		if (input.IsTrigger("B"))
		{
			//キャンセルの音
			m_seP1->Stop();
			m_seP1->SetSE(m_cancelSeHandle);
			m_seP1->Volume(kSeVolume);
			m_seP1->PlayOnce();
			m_isSelectFinishP1 = false;
			for (int i = 0; i < kSelectCommandNum; ++i)
			{
				//最後に選んだ順に消す
				if (m_selectCommandIndexP1[(kSelectCommandNum - 1) - i] != 0)
				{
					m_selectCommandIndexP1[(kSelectCommandNum - 1) - i] = 0;
					m_selectCommandIconP1Handle[(kSelectCommandNum - 1) - i] = m_nullCommandIconHandle;
					break;
				}
			}
		}
		//説明の表示
		if (input.IsTrigger("Y"))
		{
			m_isCommandInfoDisplayP1 = !m_isCommandInfoDisplayP1;
		}

		//準備ができていないことを表示
		m_currentReadyP1Handle = m_gettingReadyHandle;
		m_isReadyP1 = false;
	}
}

void CommandSelectScene::SelectCommandP2(Input& input)
{
	//準備完了を互いにしている場合は操作できない
	if (m_isReadyP1 && m_isReadyP2)
	{
		return;
	}
	if (m_isSelectFinishP2)
	{
		//説明を非表示
		m_isCommandInfoDisplayP2 = false;

		if (input.IsTrigger("A") && !m_isReadyP2)
		{
			//決定の音
			m_seP2->Stop();
			m_seP2->SetSE(m_readySeHandle);
			m_seP2->Volume(kOkSeVolume);
			m_seP2->PlayOnce();
			//準備完了
			m_currentReadyP2Handle = m_readyHandle;
			m_isReadyP2 = true;
			m_animIndexP2 = 0;
		}
		//取り消す
		if (input.IsTrigger("B"))
		{
			if (m_isReadyP2)
			{
				//キャンセルの音
				m_seP2->Stop();
				m_seP2->SetSE(m_cancelSeHandle);
				m_seP2->Volume(kSeVolume);
				m_seP2->PlayOnce();
				//確認に変換
				m_currentReadyP2Handle = m_gettingReadyHandle;
				m_isReadyP2 = false;
			}
			else//また選び直す
			{
				//キャンセルの音
				m_seP2->Stop();
				m_seP2->SetSE(m_cancelSeHandle);
				m_seP2->Volume(kSeVolume);
				m_seP2->PlayOnce();
				m_isSelectFinishP2 = false;
				//最後に選んだ順に消す
				for (int i = 0; i < 3; ++i)
				{
					if (m_selectCommandIndexP2[(kSelectCommandNum - 1) - i] != 0)
					{
						m_selectCommandIndexP2[(kSelectCommandNum - 1) - i] = 0;
						m_selectCommandIconP2Handle[(kSelectCommandNum - 1) - i] = m_nullCommandIconHandle;
						break;
					}
				}
			}
		}
	}
	else
	{
		if (input.IsTrigger("Left"))
		{
			//カーソル移動の音
			m_seP2->Stop();
			m_seP2->SetSE(m_cursorMoveSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();

			m_currentSelectCommandIndexP2--;
			if (m_currentSelectCommandIndexP2 % 3 == 0)
			{
				m_currentSelectCommandIndexP2 += kOneRowCommandNum;
			}
		}
		if (input.IsTrigger("Right"))
		{
			//カーソル移動の音
			m_seP2->Stop();
			m_seP2->SetSE(m_cursorMoveSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();

			m_currentSelectCommandIndexP2++;
			if ((m_currentSelectCommandIndexP2 - 1) % kOneRowCommandNum == 0)
			{
				m_currentSelectCommandIndexP2 -= kOneRowCommandNum;
			}
		}
		if (input.IsTrigger("Up"))
		{
			//カーソル移動の音
			m_seP2->Stop();
			m_seP2->SetSE(m_cursorMoveSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();

			m_currentSelectCommandIndexP2 -= kOneColumnCommandNum;
			if (m_currentSelectCommandIndexP2 < kCommandIndexMin)
			{
				m_currentSelectCommandIndexP2 += (kOneColumnCommandNum * kOneRowCommandNum);
			}
		}
		if (input.IsTrigger("Down"))
		{
			//カーソル移動の音
			m_seP2->Stop();
			m_seP2->SetSE(m_cursorMoveSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();

			m_currentSelectCommandIndexP2 += kOneColumnCommandNum;
			if (m_currentSelectCommandIndexP2 > kCommandIndexMax)
			{
				m_currentSelectCommandIndexP2 -= (kOneColumnCommandNum * kOneRowCommandNum);
			}

		}
		//決定
		if (input.IsTrigger("A"))
		{
			//決定の音
			m_seP2->Stop();
			m_seP2->SetSE(m_selectSeHandle);
			m_seP2->Volume(kOkSeVolume);
			m_seP2->PlayOnce();
			for (int i = 0; i < 3; ++i)
			{
				//同じ技は選べない
				if (m_selectCommandIndexP2[i] == m_currentSelectCommandIndexP2)
				{
					break;
				}
				//空ならインデックスを入れる
				if (m_selectCommandIndexP2[i] == 0)
				{
					m_selectCommandIndexP2[i] = m_currentSelectCommandIndexP2;
					m_selectCommandIconP2Handle[i] = m_commandIconHandle[m_selectCommandIndexP2[i] - 1];//アイコン表示
					break;
				}
			}
			if (m_selectCommandIndexP2[(kSelectCommandNum - 1)] != 0)
			{
				m_isSelectFinishP2 = true;
			}
		}
		//選びなおす
		if (input.IsTrigger("B"))
		{
			//キャンセルの音
			m_seP2->Stop();
			m_seP2->SetSE(m_cancelSeHandle);
			m_seP2->Volume(kSeVolume);
			m_seP2->PlayOnce();
			m_isSelectFinishP2 = false;
			//最後に選んだ順に消す
			for (int i = 0; i < kSelectCommandNum; ++i)
			{
				if (m_selectCommandIndexP2[(kSelectCommandNum - 1) - i] != 0)
				{
					m_selectCommandIndexP2[(kSelectCommandNum - 1) - i] = 0;
					m_selectCommandIconP2Handle[(kSelectCommandNum - 1) - i] = m_nullCommandIconHandle;
					break;
				}
			}
		}
		//技の説明を表示
		if (input.IsTrigger("Y"))
		{
			m_isCommandInfoDisplayP2 = !m_isCommandInfoDisplayP2;
		}

		//準備ができていないことを表示
		m_currentReadyP2Handle = m_gettingReadyHandle;
		m_isReadyP2 = false;
	}
}


void CommandSelectScene::DrawCommandInfo()
{
	if (m_isCommandInfoDisplayP1)
	{
		//説明を表示
		DrawRotaGraph(kCommandInfoPosXP1, kCommandInfoPosY, 1.0, 0, m_commandInfoHandle[m_currentSelectCommandIndexP1 - 1], true, 0, 0);
	}
	if (m_isCommandInfoDisplayP2)
	{
		//説明を表示
		DrawRotaGraph(kCommandInfoPosXP2, kCommandInfoPosY, 1.0, 0, m_commandInfoHandle[m_currentSelectCommandIndexP2 - 1], true, 0, 0);
	}
}

void CommandSelectScene::SelectColorP1(Input& input)
{
	if (!m_isReadyP1)
	{
		if (input.IsTrigger("X"))
		{
			//決定の音
			m_seP1->Stop();
			m_seP1->SetSE(m_selectSeHandle);
			m_seP1->Volume(kOkSeVolume);
			m_seP1->PlayOnce();
			m_currentColorIndexP1++;
		}
		if (m_currentColorIndexP1 > (kCharaColorNum - 1))
		{
			m_currentColorIndexP1 = 0;
		}
		m_charaP1Handle = m_idlePoseHandle[m_currentColorIndexP1];
	}
	else
	{
		m_charaP1Handle = m_readyPoseHandle[m_currentColorIndexP1];
	}
}

void CommandSelectScene::SelectColorP2(Input& input2)
{
	if (!m_isReadyP2)
	{
		if (input2.IsTrigger("X"))
		{
			//決定の音
			m_seP2->Stop();
			m_seP2->SetSE(m_selectSeHandle);
			m_seP2->Volume(kOkSeVolume);
			m_seP2->PlayOnce();
			m_currentColorIndexP2++;
		}
		if (m_currentColorIndexP2 > (kCharaColorNum - 1))
		{
			m_currentColorIndexP2 = 0;
		}
		m_charaP2Handle = m_idlePoseHandle[m_currentColorIndexP2];
	}
	else
	{
		m_charaP2Handle = m_readyPoseHandle[m_currentColorIndexP2];
	}
}



void CommandSelectScene::DrawCharacter()
{
	//切り取るを計算する
	int sizeX, sizeY;
	GetGraphSize(m_charaP1Handle, &sizeX, &sizeY);//画像サイズ
	int cutX = m_animIndexP1 % (sizeX / kCharaWidth);//横
	int cutY = m_animIndexP1 / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(kCharaPosXP1, kCharaPosY,
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		1.0f, 0.0f, m_charaP1Handle, true, false);
	//切り取るを計算する
	GetGraphSize(m_charaP2Handle, &sizeX, &sizeY);//画像サイズ
	cutX = m_animIndexP2 % (sizeX / kCharaWidth);//横
	cutY = m_animIndexP2 / (sizeX / kCharaWidth);//縦
	//描画
	DrawRectRotaGraphFast(kCharaPosXP2, kCharaPosY,
		kCharaWidth * cutX,
		kCharaHeight * cutY,
		kCharaWidth, kCharaHeight,
		1.0f, 0.0f, m_charaP2Handle, true, true);
}

void CommandSelectScene::DrawCommandIcon()
{
	//技1
	DrawCircle(kCenterX - kIconPosOffset, kCenterY - kIconPosOffset, kIconRadius, 0x22ff22, true, true);
	//技2
	DrawCircle(kCenterX, kCenterY - kIconPosOffset, kIconRadius, 0xff2222, true, true);
	//技3
	DrawCircle(kCenterX + kIconPosOffset, kCenterY - kIconPosOffset, kIconRadius, 0x2222ff, true, true);
	//技4
	DrawCircle(kCenterX - kIconPosOffset, kCenterY, kIconRadius, 0xff22ff, true, true);
	//技5
	DrawCircle(kCenterX, kCenterY, kIconRadius, 0x22ffff, true, true);
	//技6
	DrawCircle(kCenterX + kIconPosOffset, kCenterY, kIconRadius, 0xffff22, true, true);
	//技7
	DrawCircle(kCenterX - kIconPosOffset, kCenterY + kIconPosOffset, kIconRadius, 0xcff7700, true, true);
	//技8
	DrawCircle(kCenterX, kCenterY + kIconPosOffset, kIconRadius, 0x000000, true, true);
	//技9
	DrawCircle(kCenterX + kIconPosOffset, kCenterY + kIconPosOffset, kIconRadius, 0xffffff, true, true);

	for (int i = 0; i < kCommandNum; ++i)
	{
		switch (i)
		{
		case 0:
			//技1
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY - kIconPosOffset, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 1:
			//技2
			DxLib::DrawRotaGraph(kCenterX, kCenterY - kIconPosOffset, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 2:
			//技3
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY - kIconPosOffset, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 3:
			//技4
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 4:
			//技5
			DxLib::DrawRotaGraph(kCenterX, kCenterY, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 5:
			//技6
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 6:
			//技7
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY + kIconPosOffset, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 7:
			//技8
			DxLib::DrawRotaGraph(kCenterX, kCenterY + kIconPosOffset, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		case 8:
			//技9
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY + kIconPosOffset, 1.0, 0, m_commandIconHandle[i], true, 0, 0);
			break;
		default:
			break;
		}
	}
}

void CommandSelectScene::DrawCursor()
{
	//選び終わったら非表示
	if (!m_isSelectFinishP1)
	{
		if (m_blinkCountFrame <= kBlinkSpan)
		{
			//カーソルちかちか
			DxLib::SetDrawBright(200, 0, 0);
		}
		switch (m_currentSelectCommandIndexP1)
		{
		case 1:
			//技1
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY - kIconPosOffset, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 2:
			//技2
			DxLib::DrawRotaGraph(kCenterX, kCenterY - kIconPosOffset, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 3:
			//技3
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY - kIconPosOffset, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 4:
			//技4
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 5:
			//技5
			DxLib::DrawRotaGraph(kCenterX, kCenterY, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 6:
			//技6
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 7:
			//技7
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY + kIconPosOffset, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 8:
			//技8
			DxLib::DrawRotaGraph(kCenterX, kCenterY + kIconPosOffset, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		case 9:
			//技9
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY + kIconPosOffset, 1.0, 0, m_cursorP1Handle, true, 0, 0);
			break;
		default:
			break;
		}
		DxLib::SetDrawBright(255, 255, 255);
	}
	if (!m_isSelectFinishP2)
	{
		//P2
		//ちかちか
		if (m_blinkCountFrame <= kBlinkSpan)
		{
			//カーソルちかちか
			DxLib::SetDrawBright(0, 0, 200);
		}
		switch (m_currentSelectCommandIndexP2)
		{
		case 1:
			//技1
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY - kIconPosOffset, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 2:
			//技2
			DxLib::DrawRotaGraph(kCenterX, kCenterY - kIconPosOffset, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 3:
			//技3
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY - kIconPosOffset, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 4:
			//技4
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 5:
			//技5
			DxLib::DrawRotaGraph(kCenterX, kCenterY, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 6:
			//技6
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 7:
			//技7
			DxLib::DrawRotaGraph(kCenterX - kIconPosOffset, kCenterY + kIconPosOffset, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 8:
			//技8
			DxLib::DrawRotaGraph(kCenterX, kCenterY + kIconPosOffset, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		case 9:
			//技9
			DxLib::DrawRotaGraph(kCenterX + kIconPosOffset, kCenterY + kIconPosOffset, 1.0, 0, m_cursorP2Handle, true, 0, 0);
			break;
		default:
			break;
		}
		DxLib::SetDrawBright(255, 255, 255);
	}
}

void CommandSelectScene::DrawSelectText()
{
	if (m_blinkCountFrame <= kBlinkSpan)
	{
		//カーソルちかちか
		SetDrawBlendMode(DX_BLENDMODE_INVSRC, 255);
	}
	DrawRotaGraph(kCenterX, 100, 1.0, 0, m_selectTextHandle, true, 0, 0);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}

void CommandSelectScene::DrawSelectPlayerCommandIcon()
{
	//コマンド
	for (int i = 0; i < kSelectCommandNum; ++i)
	{
		//選んだ技のアイコン
		DrawRectRotaGraphFast(
			static_cast<float>(kSelectedCommandOffsetPosX * (i + 1)),
			kSelectedCommandPosY,
			0, 0, kCommandIconImageWidth, kCommandIconImageHight,
			kCommandIconImageScale, 0.0f, m_selectCommandIconP1Handle[i], true);
		DrawRectRotaGraphFast(
			static_cast<float>(Game::kScreenWidth - kSelectedCommandOffsetPosX * (i + 1)),
			kSelectedCommandPosY,
			0, 0, kCommandIconImageWidth, kCommandIconImageHight,
			kCommandIconImageScale, 0.0f, m_selectCommandIconP2Handle[i], true);
	}
}

void CommandSelectScene::DrawReady()
{
	//準備できたかの確認
	if (m_isSelectFinishP1)
	{
		if ((m_blinkCountFrame <= kBlinkSpan) && !m_isReadyP1)
		{
			DxLib::SetDrawBright(150, 150, 150);
		}
		DxLib::DrawGraph(kReadyPosXP1, kReadyPosY, m_currentReadyP1Handle, true);//1PのReady
		DxLib::SetDrawBright(255, 255, 255);
	}
	if (m_isSelectFinishP2)
	{
		if ((m_blinkCountFrame <= kBlinkSpan) && !m_isReadyP2)
		{
			DxLib::SetDrawBright(150, 150, 150);
		}
		DxLib::DrawGraph(kReadyPosXP2, kReadyPosY, m_currentReadyP2Handle, true);//2PのReady
		DxLib::SetDrawBright(255, 255, 255);
	}
}

CommandSelectScene::CommandSelectScene(SceneController& controller) :
	SceneBase(controller),
	m_currentSelectCommandIndexP1(1),
	m_currentSelectCommandIndexP2(2),
	m_isSelectFinishP1(false),
	m_isSelectFinishP2(false),
	m_isReadyP1(false),
	m_isReadyP2(false),
	m_selectCommandIndexP1{ 0,0,0 },
	m_selectCommandIndexP2{ 0,0,0 },
	m_isFadeOut(false),
	m_blinkCountFrame(0),
	m_animCountFrame(0),
	m_animIndexP1(0),
	m_animIndexP2(0),
	//カーソル
	m_cursorP1Handle(LoadGraph("img/CharacterSelect/Icon/SelectFrameP1.png")),
	m_cursorP2Handle(LoadGraph("img/CharacterSelect/Icon/SelectFrameP2.png")),
	//SEの音源
	m_selectSeHandle(LoadSoundMem("./SE/Select/SelectSE.mp3")),
	m_readySeHandle(LoadSoundMem("./SE/Select/ReadySE.mp3")),
	m_cursorMoveSeHandle(LoadSoundMem("./SE/Select/CursorMoveSE.mp3")),
	m_cancelSeHandle(LoadSoundMem("./SE/Select/CancelSE.mp3")),
	//背景
	m_backGroundHandle(LoadGraph("img/CharacterSelect/CommandSelectBack.png")),
	//テキスト
	m_selectTextHandle(LoadGraph("img/CharacterSelect/SelectText.png")),
	//色
	m_currentColorIndexP1(0),
	m_currentColorIndexP2(0),
	//操作に関するUI
	m_operationHandle(LoadGraph("img/CharacterSelect/Operation.png")),
	//コマンド技の説明
	m_isCommandInfoDisplayP1(false),
	m_isCommandInfoDisplayP2(false),
	m_commandInfoHandle{
		LoadGraph("./img/CharacterSelect/CommandInfo/Command1Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command2Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command3Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command4Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command5Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command6Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command7Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command8Info.png"),
		LoadGraph("./img/CharacterSelect/CommandInfo/Command9Info.png")
	},
	m_charaP1Handle(-1),
	m_charaP2Handle(-1),
	m_commandIconHandle{},
	m_idlePoseHandle{},
	m_readyPoseHandle{},
	m_gettingReadyHandle(LoadGraph("./img/CharacterSelect/Ready_Off.png")),//準備中
	m_readyHandle(LoadGraph("./img/CharacterSelect/Ready_On.png")),//準備完了
	m_currentReadyP1Handle(m_gettingReadyHandle),//準備完了かどうかを表示
	m_currentReadyP2Handle(m_gettingReadyHandle),
	m_loadingHandle(LoadGraph("./img/Loading/Operation.png"))
{
	//BGM
	m_bgm = std::make_shared<BGM>();
	m_bgmHandle = LoadSoundMem("./BGM/BGM_SelectScene.mp3");
	m_bgm->SetBGM(m_bgmHandle);
	m_bgm->Volume(kBgmVolume);
	m_bgm->PlayLoop();
	//SE
	m_seP1 = std::make_shared<SE>();
	m_seP2 = std::make_shared<SE>();

	for (int i = 0; i < kCharaColorNum; ++i)
	{
		switch (i)
		{
		case 0:
			m_idlePoseHandle[i] = LoadGraph("img/Chara/White/playerbase/idle_001.png");
			m_readyPoseHandle[i] = LoadGraph("img/Chara/White/playerbase/win_001.png");
			break;
		case 1:
			m_idlePoseHandle[i] = LoadGraph("img/Chara/Red/playerbase/idle_001.png");
			m_readyPoseHandle[i] = LoadGraph("img/Chara/Red/playerbase/win_001.png");
			break;
		case 2:
			m_idlePoseHandle[i] = LoadGraph("img/Chara/Blue/playerbase/idle_001.png");
			m_readyPoseHandle[i] = LoadGraph("img/Chara/Blue/playerbase/win_001.png");
			break;
		case 3:
			m_idlePoseHandle[i] = LoadGraph("img/Chara/Yellow/playerbase/idle_001.png");
			m_readyPoseHandle[i] = LoadGraph("img/Chara/Yellow/playerbase/win_001.png");
			break;
		case 4:
			m_idlePoseHandle[i] = LoadGraph("img/Chara/Green/playerbase/idle_001.png");
			m_readyPoseHandle[i] = LoadGraph("img/Chara/Green/playerbase/win_001.png");
			break;
		default:
			break;
		}
	}
	m_charaP1Handle = m_idlePoseHandle[0];
	m_charaP2Handle = m_idlePoseHandle[0];

	//アイコン
	for (int i = 0; i < kCommandNum; ++i)
	{
		switch (i)
		{
		case 0:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command1.png");
			break;
		case 1:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command2.png");
			break;
		case 2:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command3.png");
			break;
		case 3:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command4.png");
			break;
		case 4:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command5.png");
			break;
		case 5:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command6.png");
			break;
		case 6:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command7.png");
			break;
		case 7:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command8.png");
			break;
		case 8:
			m_commandIconHandle[i] = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/Command9.png");
			break;
		default:
			break;
		}
	}
	//空であることを表示
	m_nullCommandIconHandle = LoadGraph("./img/CharacterSelect/Icon/CommandIcon/SelectNull.png");
	for (int i = 0; i < kSelectCommandNum; ++i)
	{
		m_selectCommandIconP1Handle[i] = m_nullCommandIconHandle;
		m_selectCommandIconP2Handle[i] = m_nullCommandIconHandle;
	}
	//フェードインするときに使う
	m_fadeManager = std::make_shared<FadeManager>();
}

CommandSelectScene::~CommandSelectScene()
{
	//コマンド技のアイコン
	for (int i = 0; i < kCommandNum; ++i)
	{
		DeleteGraph(m_commandIconHandle[i]);
	}
	//選んだコマンド技のアイコン
	for (int i = 0; i < kSelectCommandNum; ++i)
	{
		DeleteGraph(m_selectCommandIconP1Handle[i]);
		DeleteGraph(m_selectCommandIconP2Handle[i]);
	}
	DeleteGraph(m_bgmHandle);
	DeleteGraph(m_loadingHandle);
}

void CommandSelectScene::Update(Input& input, Input& input2)
{
	//キャラのアニメーション
	UpdateCharaAnim();
	//ちかちかに使う
	UpdateBlink();
	//P1のセレクト
	SelectColorP1(input);
	SelectCommandP1(input);
	//P2のセレクト
	SelectColorP2(input2);
	SelectCommandP2(input2);
	//2人が準備完了したらゲームシーンへ
	if (m_isReadyP1 && m_isReadyP2)
	{
		m_isFadeOut = true;
		//フェードインしてからシーン移動
		if (m_fadeManager->IsFinishFadeOut())
		{
			//このシーンでやりたいこと
			//キャラクターを決定したらそのキャラクターの
			//ポインタを次のシーンに渡したい
			//選んだコマンド技のインデックスを次のシーンに渡すために保存
			m_controller.SaveSelectCommandIndex(m_selectCommandIndexP1, m_selectCommandIndexP2);
			//色
			m_controller.SaveCharaColorIndexP1(static_cast<CharaColorIndex>(m_currentColorIndexP1));
			m_controller.SaveCharaColorIndexP2(static_cast<CharaColorIndex>(m_currentColorIndexP2));
			//押されたら次の状態に繊維
			//次の状態はこのクラスが覚えておく
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));
			return;//忘れずreturn
		}
	}
	else
	{
		if (input.IsTrigger("Start") || input2.IsTrigger("Start"))
		{
			m_isFadeOut = true;
		}
		if (m_fadeManager->IsFinishFadeOut())
		{
			//タイトル画面に戻る
			m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
			return;
		}
	}
#if _DEBUG
	//デバッグ用
	if (input.IsTrigger("Select"))
	{
		////BGMを止める
		//soundManager.StopBGM();

		for (int i = 0;i < 3;++i)
		{
			//空ならインデックスを入れる
			if (m_selectCommandIndexP1[i] == 0)
			{
				m_selectCommandIndexP1[i] = i + 1;
			}
			//空ならインデックスを入れる
			if (m_selectCommandIndexP2[i] == 0)
			{
				m_selectCommandIndexP2[i] = i + 1;
			}
		}

		//選んだコマンド技のインデックスを次のシーンに渡すために保存
		m_controller.SaveSelectCommandIndex(m_selectCommandIndexP1, m_selectCommandIndexP2);
		//色
		m_controller.SaveCharaColorIndexP1(static_cast<CharaColorIndex>(m_currentColorIndexP1));
		m_controller.SaveCharaColorIndexP2(static_cast<CharaColorIndex>(m_currentColorIndexP2));
		//押されたら次の状態に繊維
		//次の状態はこのクラスが覚えておく
		m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));
		return;//忘れずreturn
	}
#endif
}

void CommandSelectScene::Draw()
{
	//背景
	DxLib::DrawGraph(0, 0, m_backGroundHandle, true);

	//キャラクターのアニメーション
	DrawCharacter();

	//Ready
	DrawReady();

	//技のアイコン
	DrawCommandIcon();

	//カーソル
	DrawCursor();

	//選んだコマンドの表示
	DrawSelectPlayerCommandIcon();
	//"コマンド技を3つ選んでください"のテキスト
	DrawSelectText();

	//コマンド技の説明
	DrawCommandInfo();

	//操作説明
	DrawGraph(0, Game::kScreenHeight - 50, m_operationHandle, true);

	//フェードイン
	m_fadeManager->DrawWhiteFade(m_isFadeOut);
	if (m_fadeManager->IsFinishFadeOut())
	{
		DrawGraph(0, 0, m_loadingHandle, false);
	}

#if _DEBUG	
	DxLib::DrawString(10, 10, "CharacterselectScene", 0xffffff);
	for (int i = 0;i < 3;++i)
	{
		DrawFormatString(400, 800 + (20 * i), 0xff3333, "selectNumP1 = %d", m_selectCommandIndexP1[i]);
		DrawFormatString(1000, 800 + (20 * i), 0x0000ff, "selectNumP2 = %d", m_selectCommandIndexP2[i]);
		DrawFormatString(400, 750, 0xff3333, "currentColorIndexP1 = %d", m_currentColorIndexP1);
		DrawFormatString(1000, 750, 0x0000ff, "currentColorIndexP2 = %d", m_currentColorIndexP2);
	}
	//DrawFormatString(10, 20, 0xffffff, "処理数%d", GetASyncLoadNum());
#endif

}
