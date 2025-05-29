#include "ResultScene.h"
#include "SceneController.h"
#include "Input.h"
#include "CommandSelectScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "FadeManager.h"
#include <DxLib.h>
#include "game.h"
#include "BGM.h"
#include "SE.h"

namespace
{
	//プレイヤーの画像
	constexpr int kP1ImagePosX = (Game::kScreenWidth / 2) - 300;
	constexpr int kP2ImagePosX = (Game::kScreenWidth / 2) + 300;
	constexpr int kPlayerImagePosY = (Game::kScreenHeight / 2) - 200;
	//セリフ数
	constexpr int kSerifNum = 5;
	constexpr int kSerifMaxIndex = kSerifNum - 1;
	constexpr int kSerifPosX = (Game::kScreenWidth / 2) - 300;
	constexpr int kSerifPosY = (Game::kScreenHeight / 2) + 50;
	//メニュー数
	constexpr int kMenuNum = 2;
	//BGMボリューム
	constexpr int kBgmVolume = 150;
	//SEボリューム
	constexpr int kSeVolume = 150;

	//カーソル
	constexpr int kMenuWidth = 375;
	constexpr int kCursorHeight = 100;
	constexpr int kCursorPosY = (Game::kScreenHeight / 2) + 120;

	//光る数
	constexpr int kBlinkNum = 10;
	//点滅の間隔
	constexpr int kBlinkFrame = 7;
	//ボタンの拡大率
	constexpr double kSelectBottunScale = 1.3;

	//プレイヤーのリアクション
	constexpr double kAddAngle = 0.01;
	constexpr double kMaxAngle = 0.1;
	constexpr double kMinAngle = -0.1;
	constexpr double kAddScale = 0.01;
	constexpr double kMaxScale = 1.0;
	constexpr double kMinScale = 0.9;

}

ResultScene::ResultScene(SceneController& controller) :
	SceneBase(controller),
	m_p1Handle(-1),
	m_p2Handle(-1),
	m_p1ImagePos(Game::kScreenWidth / 2, kPlayerImagePosY, 0),
	m_p2ImagePos(Game::kScreenWidth / 2, kPlayerImagePosY, 0),
	m_p1ImageAngle(0.0),
	m_p2ImageAngle(0.0),
	m_addAngle(kAddAngle),
	m_p1ImageScale(1.0),
	m_p2ImageScale(1.0),
	m_addScale(kAddScale),
	m_isFadeOut(false),
	m_selectMenuIndexP1(0),
	m_selectMenuIndexP2(0),
	m_rematchBottunHandle(LoadGraph("./img/Menu/Rematch.png")),
	m_reselectBottunHandle(LoadGraph("./img/Menu/CommandSelect.png")),
	m_p1ColorHandle(LoadGraph("./img/Menu/BlendP1.png")),
	m_p2ColorHandle(LoadGraph("./img/Menu/BlendP2.png")),
	m_selectBottunScaleP1(1.0),
	m_selectBottunScaleP2(1.0),
	m_isDecideP1(false),
	m_isDecideP2(false),
	m_isSelecting(false),
	m_blinkCountFrame(0),
	m_blinkCountP1(0),
	m_blinkCountP2(0),
	m_selectSeHandle(LoadSoundMem("./SE/Select/ReadySE.mp3")),
	m_cursorMoveSehandle(LoadSoundMem("./SE/Select/CursorMoveSE.mp3")),
	m_serifHandle(-1),
	m_backGroundHandle(LoadGraph("./img/Result/ResultBack.png"))
{
	int serif;
	switch (GetRand(kSerifMaxIndex))
	{
	case 0:
		serif = LoadGraph("./img/Result/Serif/Serif1.png");
		break;
	case 1:
		serif = LoadGraph("./img/Result/Serif/Serif2.png");
		break;
	case 2:
		serif = LoadGraph("./img/Result/Serif/Serif3.png");
		break;
	case 3:
		serif = LoadGraph("./img/Result/Serif/Serif4.png");
		break;
	case 4:
		serif = LoadGraph("./img/Result/Serif/Serif5.png");
		break;
	default:
		break;
	}
	m_serifHandle = serif;

	//1Pが勝ったなら
	if (m_controller.GetWinPlayerIndex() == PlayerIndex::Player1)
	{
		//P1の勝利画像
		switch (m_controller.GetCharaColorIndexP1())
		{
		case CharaColorIndex::White:
			m_p1Handle = LoadGraph("./img/Result/Face/White_Win.png");
			break;
		case CharaColorIndex::Red:
			m_p1Handle = LoadGraph("./img/Result/Face/Red_Win.png");
			break;
		case CharaColorIndex::Blue:
			m_p1Handle = LoadGraph("./img/Result/Face/Blue_Win.png");
			break;
		case CharaColorIndex::Green:
			m_p1Handle = LoadGraph("./img/Result/Face/Green_Win.png");
			break;
		case CharaColorIndex::Yellow:
			m_p1Handle = LoadGraph("./img/Result/Face/Yellow_Win.png");
			break;
		default:
			break;
		}
		//P2の敗北画像
		switch (m_controller.GetCharaColorIndexP2())
		{
		case CharaColorIndex::White:
			m_p2Handle = LoadGraph("./img/Result/Face/White_Lose.png");
			break;
		case CharaColorIndex::Red:
			m_p2Handle = LoadGraph("./img/Result/Face/Red_Lose.png");
			break;
		case CharaColorIndex::Blue:
			m_p2Handle = LoadGraph("./img/Result/Face/Blue_Lose.png");
			break;
		case CharaColorIndex::Green:
			m_p2Handle = LoadGraph("./img/Result/Face/Green_Lose.png");
			break;
		case CharaColorIndex::Yellow:
			m_p2Handle = LoadGraph("./img/Result/Face/Yellow_Lose.png");
			break;
		default:
			break;
		}
	}
	else if (m_controller.GetWinPlayerIndex() == PlayerIndex::Player2)
	{
		//P2の勝利画像
		switch (m_controller.GetCharaColorIndexP2())
		{
		case CharaColorIndex::White:
			m_p2Handle = LoadGraph("./img/Result/Face/White_Win.png");
			break;
		case CharaColorIndex::Red:
			m_p2Handle = LoadGraph("./img/Result/Face/Red_Win.png");
			break;
		case CharaColorIndex::Blue:
			m_p2Handle = LoadGraph("./img/Result/Face/Blue_Win.png");
			break;
		case CharaColorIndex::Green:
			m_p2Handle = LoadGraph("./img/Result/Face/Green_Win.png");
			break;
		case CharaColorIndex::Yellow:
			m_p2Handle = LoadGraph("./img/Result/Face/Yellow_Win.png");
			break;
		default:
			break;
		}
		//P1の敗北画像
		switch (m_controller.GetCharaColorIndexP1())
		{
		case CharaColorIndex::White:
			m_p1Handle = LoadGraph("./img/Result/Face/White_Lose.png");
			break;
		case CharaColorIndex::Red:
			m_p1Handle = LoadGraph("./img/Result/Face/Red_Lose.png");
			break;
		case CharaColorIndex::Blue:
			m_p1Handle = LoadGraph("./img/Result/Face/Blue_Lose.png");
			break;
		case CharaColorIndex::Green:
			m_p1Handle = LoadGraph("./img/Result/Face/Green_Lose.png");
			break;
		case CharaColorIndex::Yellow:
			m_p1Handle = LoadGraph("./img/Result/Face/Yellow_Lose.png");
			break;
		default:
			break;
		}
	}

	m_bgm = std::make_shared<BGM>();
	m_resultBgmHandle = LoadSoundMem("./BGM/BGM_Result.mp3");
	m_menuBgmHandle = LoadSoundMem("./BGM/BGM_Menu.mp3");
	m_bgm->SetBGM(m_resultBgmHandle);
	m_bgm->Volume(kBgmVolume);
	m_bgm->PlayOnce();

	m_seP1 = std::make_shared<SE>();
	m_seP2 = std::make_shared<SE>();
	

	//フェードするときに使う
	m_fadeManager = std::make_shared<FadeManager>();

	//三項演算子
	//条件 ? trueなら : falseなら;
	std::string loadingGraphPath = GetRand(1) ? "./img/Loading/kotu1.png" : "./img/Loading/kotu2.png";
	m_loadingHandle = LoadGraph(loadingGraphPath.c_str());
}

ResultScene::~ResultScene()
{
	//解放
	DeleteGraph(m_rematchBottunHandle);
	DeleteGraph(m_reselectBottunHandle);
	DeleteGraph(m_p1ColorHandle);
	DeleteGraph(m_p2ColorHandle);
	DeleteGraph(m_serifHandle);
	DeleteGraph(m_selectSeHandle);
	DeleteGraph(m_cursorMoveSehandle);
	DeleteGraph(m_backGroundHandle);
	DeleteGraph(m_p1Handle);
	DeleteGraph(m_p2Handle);
	DeleteGraph(m_menuBgmHandle);
	DeleteGraph(m_resultBgmHandle);
	DeleteGraph(m_loadingHandle);
}

void ResultScene::SelectMenu(Input& input,int& selectMenuIndex,bool& isDecide, std::shared_ptr<SE>& se)
{
	if (!isDecide)
	{
		if (input.IsTrigger("Up"))
		{
			selectMenuIndex--;
			//カーソル移動の音
			se->Stop();
			se->SetSE(m_cursorMoveSehandle);
			se->Volume(kSeVolume);
			se->PlayOnce();
		}
		if (input.IsTrigger("Down"))
		{
			selectMenuIndex++;
			//カーソル移動の音
			se->Stop();
			se->SetSE(m_cursorMoveSehandle);
			se->Volume(kSeVolume);
			se->PlayOnce();
		}
		if (selectMenuIndex < 0)
		{
			selectMenuIndex = (kMenuNum - 1);
		}
		if (selectMenuIndex > (kMenuNum - 1))
		{
			selectMenuIndex = 0;
		}
		//決定
		if (input.IsTrigger("A") && !isDecide)
		{
			isDecide = true;
			//決定の音
			se->Stop();
			se->SetSE(m_selectSeHandle);
			se->Volume(kSeVolume);
			se->PlayOnce();
		}
	}
}

void ResultScene::Rematch()
{
	//押されたら次の状態に繊維
	//次の状態はこのクラスが覚えておく
	m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));
}

void ResultScene::Reselect()
{
	//押されたら次の状態に繊維
	//次の状態はこのクラスが覚えておく
	m_controller.ChangeScene(std::make_shared<CommandSelectScene>(m_controller));
}

void ResultScene::ChangeBottunScale(bool& isDecide,double& selectBottunScale)
{
	if (!isDecide)
	{
		if (selectBottunScale > kSelectBottunScale)
		{
			selectBottunScale = 1.0;
		}
		selectBottunScale += 0.01;
	}
	else
	{
		selectBottunScale = kSelectBottunScale;
	}
}

void ResultScene::DrawMenuBottunP1()
{
	//透明度
	int alpha = 255;
	if (m_isDecideP1)
	{
		if (m_blinkCountP1 < kBlinkNum)
		{
			if (m_blinkCountFrame % kBlinkFrame == 0)
			{
				m_blinkCountP1++;
				alpha = 0;
			}
		}
	}
	//選んでいるボタンを拡大する
	if (m_selectMenuIndexP1 == 0)
	{
		DxLib::DrawRotaGraph(300, 550,
			m_selectBottunScaleP1, 0.0,
			m_rematchBottunHandle, true, false);
		//赤くする
		SetDrawBlendMode(DX_BLENDMODE_MULA, alpha);
		DxLib::DrawRotaGraph(300, 550,
			m_selectBottunScaleP1, 0.0,
			m_p1ColorHandle, true, false);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		
		DxLib::DrawRotaGraph(300, 700,
			1.0, 0.0,
			m_reselectBottunHandle, true, false);
	}
	else if (m_selectMenuIndexP1 == 1)
	{
		DxLib::DrawRotaGraph(300, 550,
			1.0, 0.0,
			m_rematchBottunHandle, true, false);
		DxLib::DrawRotaGraph(300, 700,
			m_selectBottunScaleP1, 0.0,
			m_reselectBottunHandle, true, false);
		//赤くする
		SetDrawBlendMode(DX_BLENDMODE_MULA, alpha);
		DxLib::DrawRotaGraph(300, 700,
			m_selectBottunScaleP1, 0.0,
			m_p1ColorHandle, true, false);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
}
void ResultScene::DrawMenuBottunP2()
{
	//透明度
	int alpha = 255;
	if (m_isDecideP2)
	{
		if (m_blinkCountP2 < kBlinkNum)
		{
			if (m_blinkCountFrame % kBlinkFrame == 0)
			{
				m_blinkCountP2++;
				alpha = 0;
			}
		}
	}
	//選んでいるボタンを拡大する
	if (m_selectMenuIndexP2 == 0)
	{
		DxLib::DrawRotaGraph(Game::kScreenWidth - 300, 550,
			m_selectBottunScaleP2, 0.0,
			m_rematchBottunHandle, true, false);
		//青くする
		SetDrawBlendMode(DX_BLENDMODE_MULA, alpha);
		DxLib::DrawRotaGraph(Game::kScreenWidth - 300, 550,
			m_selectBottunScaleP2, 0.0,
			m_p2ColorHandle, true, false);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

		DxLib::DrawRotaGraph(Game::kScreenWidth - 300, 700,
			1.0, 0.0,
			m_reselectBottunHandle, true, false);
	}
	else if (m_selectMenuIndexP2 == 1)
	{
		DxLib::DrawRotaGraph(Game::kScreenWidth - 300, 550,
			1.0, 0.0,
			m_rematchBottunHandle, true, false);
		DxLib::DrawRotaGraph(Game::kScreenWidth - 300, 700,
			m_selectBottunScaleP2, 0.0,
			m_reselectBottunHandle, true, false);
		//青くする
		SetDrawBlendMode(DX_BLENDMODE_MULA, alpha);
		DxLib::DrawRotaGraph(Game::kScreenWidth - 300, 700,
			m_selectBottunScaleP2, 0.0,
			m_p2ColorHandle, true, false);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
}

void ResultScene::Update(Input& input, Input& input2)
{
	//画面を黒で塗りつぶす
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x555555, true);

	//点滅のフレームをカウント
	m_blinkCountFrame++;

	if (m_isSelecting)
	{
		//メニューを選ぶ
		SelectMenu(input,m_selectMenuIndexP1,m_isDecideP1,m_seP1);
		SelectMenu(input2,m_selectMenuIndexP2, m_isDecideP2, m_seP2);
		if (CheckDecidedMenu())return;
		if (m_p1ImagePos.x > kP1ImagePosX)
		{
			m_p1ImagePos.x -= 5.0f;
		}
		if (m_p2ImagePos.x < kP2ImagePosX)
		{
			m_p2ImagePos.x += 5.0f;;
		}

		//1Pが勝ったなら
		if (m_controller.GetWinPlayerIndex() == PlayerIndex::Player1)
		{
			//ノリノリ
			m_p1ImageAngle += m_addAngle;
			if (m_p1ImageAngle > kMaxAngle || m_p1ImageAngle < kMinAngle)
			{
				m_addAngle *= -1.0;
			}
			//しょんぼり
			m_p2ImageScale += m_addScale;
			if (m_p2ImageScale > kMaxScale || m_p2ImageScale < kMinScale)
			{
				m_addScale *= -1.0;
			}
		}
		else
		{
			//ノリノリ
			m_p2ImageAngle += m_addAngle;
			if (m_p2ImageAngle > kMaxAngle || m_p2ImageAngle < kMinAngle)
			{
				m_addAngle *= -1.0;
			}
			//しょんぼり
			m_p1ImageScale += m_addScale;
			if (m_p1ImageScale > kMaxScale || m_p1ImageScale < kMinScale)
			{
				m_addScale *= -1.0;
			}
		}
	}
	//ボタンを押したらスキップ
	SkipResult(input, input2);
	//ボタンの大きさを変える
	ChangeBottunScale(m_isDecideP1, m_selectBottunScaleP1);
	ChangeBottunScale(m_isDecideP2, m_selectBottunScaleP2);
}

bool ResultScene::CheckDecidedMenu()
{
	//両方が選んだら
	if (m_isDecideP1 && m_isDecideP2)
	{
		//点滅が終わったら
		if (m_blinkCountP1 >= kBlinkNum && m_blinkCountP2 >= kBlinkNum)
		{
			m_isFadeOut = true;
		}
	}
	//フェードアウトしたら
	if (m_fadeManager->IsFinishFadeOut())
	{
		//番号にあった関数を呼ぶ
		if (m_selectMenuIndexP1 == 0 && m_selectMenuIndexP2 == 0)
		{
			Rematch();//再戦
		}
		else
		{
			Reselect();//コマンドセレクト
		}
		return true;
	}
	return false;
}

void ResultScene::SkipResult(Input& input, Input& input2)
{
	//BGMの再生が終わったら切り替え
	if (m_bgm->CheckEndBGM())
	{
		//メニューの表示とBGM切り替え
		m_isSelecting = true;
		m_bgm->SetBGM(m_menuBgmHandle);
		m_bgm->Volume(kBgmVolume);
		m_bgm->PlayLoop();
	}
	//スキップ
	if (input.IsTrigger("A") || input2.IsTrigger("A"))
	{
		m_isSelecting = true;
	}
}


void ResultScene::DrawPlayerImage()
{
	//リザルトの画像
	if (m_controller.GetWinPlayerIndex() == PlayerIndex::Player1)
	{
		DxLib::DrawRotaGraph(m_p2ImagePos.x, m_p2ImagePos.y,
			m_p2ImageScale, m_p2ImageAngle,
			m_p2Handle, true, true);
		DxLib::DrawRotaGraph(m_p1ImagePos.x, m_p1ImagePos.y,
			m_p1ImageScale, m_p1ImageAngle,
			m_p1Handle, true, false);
	}
	else
	{
		DxLib::DrawRotaGraph(m_p1ImagePos.x, m_p1ImagePos.y,
			m_p1ImageScale, m_p1ImageAngle,
			m_p1Handle, true, false);
		DxLib::DrawRotaGraph(m_p2ImagePos.x, m_p2ImagePos.y,
			m_p2ImageScale, m_p2ImageAngle,
			m_p2Handle, true, true);
	}
}

void ResultScene::DrawBackGround()
{
	DrawGraph(0, 0, m_backGroundHandle, true);
}

void ResultScene::DrawSerif()
{
	DxLib::DrawGraph(kSerifPosX, kSerifPosY, m_serifHandle, true);
}

void ResultScene::DrawMenu()
{
	//メニュー
	if (m_isSelecting)
	{
		//選んでいるボタンを拡大
		DrawMenuBottunP1();
		DrawMenuBottunP2();
	}
}

void ResultScene::Draw()
{
#if _DEBUG	
	DrawString(10, 10, "Result Scene", 0xffffff);
	//DrawLine(Game::kScreenWidth / 2, 0, Game::kScreenWidth / 2, Game::kScreenHeight, 0xffffff);
#endif
	//背景
	DrawBackGround();

	//プレイヤーの画像
	DrawPlayerImage();
	
	//セリフ
	DrawSerif();

	//メニュー
	DrawMenu();

	//フェード
	m_fadeManager->DrawBlackFade(m_isFadeOut);

	//Loading
	if (m_fadeManager->IsFinishFadeOut())
	{
		DrawGraph(0, 0, m_loadingHandle, false);
	}
}

