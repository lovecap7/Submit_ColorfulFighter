#include "GameScene.h"
#include <DxLib.h>
#include "game.h"
#include "Input.h"
#include "SceneController.h"
#include "CommandSelectScene.h"
#include "ResultScene.h"
#include "Player.h"
#include "UI.h"
#include "GameManager.h"
#include "Camera.h"
#include "Bullet.h"
#include "BGM.h"

namespace
{
	constexpr float kPlayerFirstPosX = 200.0f;
	constexpr int kStageWidth = 2600;
	constexpr int kColorChangeSpeed = 5;
	constexpr int kColorNum = 8;

	//BGMボリューム
	constexpr int kBgmVolume = 120;
	//SEボリューム
	constexpr int kSeVolume = 150;
	//ステージ
	constexpr float kStageBackPosX = 0.0f;
	constexpr float kStageBackPosY = 700.0f;
	constexpr float kStageFloorPosX = 0.0f;
	constexpr float kStageFloorPosY = 1250.0f;
}

void GameScene::CreateColorHandle()
{
	for (int i = 0;i < kColorNum;++i)
	{
		switch (i)
		{
		case 0:
			m_floorColorHandle[i] = LoadGraph("./img/stage/floor/red.png");
			m_backColorHandle[i] = LoadGraph("./img/stage/back/red.png");
			break;
		case 1:
			m_floorColorHandle[i] = LoadGraph("./img/stage/floor/orange.png");
			m_backColorHandle[i] = LoadGraph("./img/stage/back/orange.png");
			break;
		case 2:
			m_floorColorHandle[i] = LoadGraph("./img/stage/floor/yellow.png");
			m_backColorHandle[i] = LoadGraph("./img/stage/back/yellow.png");
			break;
		case 3:
			m_floorColorHandle[i] = LoadGraph("./img/stage/floor/green.png");
			m_backColorHandle[i] = LoadGraph("./img/stage/back/green.png");
			break;
		case 4:
			m_floorColorHandle[i] = LoadGraph("./img/stage/floor/cyan.png");
			m_backColorHandle[i] = LoadGraph("./img/stage/back/cyan.png");
			break;
		case 5:
			m_floorColorHandle[i] = LoadGraph("./img/stage/floor/blue.png");
			m_backColorHandle[i] = LoadGraph("./img/stage/back/blue.png");
			break;
		case 6:
			m_floorColorHandle[i] = LoadGraph("./img/stage/floor/purple.png");
			m_backColorHandle[i] = LoadGraph("./img/stage/back/purple.png");
			break;
		case 7:
			m_floorColorHandle[i] = LoadGraph("./img/stage/floor/pink.png");
			m_backColorHandle[i] = LoadGraph("./img/stage/back/pink.png");
			break;
		default:
			break;
		}
	}
}


void GameScene::UpdateStageColor()
{
	//色を変えていく
	m_colorAlphaCountFrame += kColorChangeSpeed;
	if (m_colorAlphaCountFrame > 255)
	{
		m_colorIndex++;
		m_colorAlphaCountFrame = 0;
	}
	if (m_colorIndex >= kColorNum)
	{
		m_colorIndex = 0;
	}
}


void GameScene::GameInit()
{
	m_player1->Init((kStageWidth / 2) - kPlayerFirstPosX, false);
	m_player2->Init((kStageWidth / 2) + kPlayerFirstPosX, true);
	m_bullet1->Init();
	m_bullet2->Init();
	m_gameManager->Init();
	m_ui->Init(m_player1->GetHp(), m_player2->GetHp(),*m_gameManager);
	m_camera->Init(*m_player1, *m_player2);
}
void GameScene::DrawStage()
{
	DxLib::DrawGraph(kStageBackPosX + static_cast<int>(m_camera->m_drawOffset.x),
		kStageBackPosY + static_cast<int>(m_camera->m_drawOffset.y), 
		m_backBaseHandle, true);
	DxLib::DrawGraph(kStageFloorPosX + static_cast<int>(m_camera->m_drawOffset.x),
		kStageFloorPosY + static_cast<int>(m_camera->m_drawOffset.y), 
		m_floorBaseHandle, true);
	//色
	//色が強いから少し透明にしてる
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - m_colorAlphaCountFrame);
	//壁
	DxLib::DrawGraph(kStageBackPosX + static_cast<int>(m_camera->m_drawOffset.x),
		kStageBackPosY + static_cast<int>(m_camera->m_drawOffset.y), 
		m_backColorHandle[m_colorIndex], true);
	//床
	DxLib::DrawGraph(kStageFloorPosX + static_cast<int>(m_camera->m_drawOffset.x),
		kStageFloorPosY + static_cast<int>(m_camera->m_drawOffset.y), 
		m_floorColorHandle[m_colorIndex], true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_colorAlphaCountFrame);
	//次の色を表示
	int nextColorIndex = m_colorIndex + 1;
	if (nextColorIndex >= kColorNum)
	{
		nextColorIndex = 0;
	}
	//壁
	DxLib::DrawGraph(kStageBackPosX + static_cast<int>(m_camera->m_drawOffset.x),
		kStageBackPosY + static_cast<int>(m_camera->m_drawOffset.y),
		m_backColorHandle[nextColorIndex], true);
	//床
	DxLib::DrawGraph(kStageFloorPosX + static_cast<int>(m_camera->m_drawOffset.x),
		kStageFloorPosY + static_cast<int>(m_camera->m_drawOffset.y),
		m_floorColorHandle[nextColorIndex], true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

GameScene::GameScene(SceneController& controller):
	SceneBase(controller),
	m_color(0x000000),
	m_colorAlphaCountFrame(0),
	m_colorIndex(0),
	//ステージ
	m_backColorHandle{},
	m_floorColorHandle{},
	m_backBaseHandle(LoadGraph("./img/stage/back/NewBase.png")),//背景
	m_floorBaseHandle(LoadGraph("./img/stage/floor/NewBase.png"))//床
{
	bool isSameColor = false;
	//プレイヤーのカラーが同じかどうかをチェック
	if (m_controller.GetCharaColorIndexP1() == m_controller.GetCharaColorIndexP2())
	{
		isSameColor = true;
	}
	//メモリ確保
	m_player1 = std::make_shared<Player>(PlayerIndex::Player1, m_controller.GetSelectCommandIndex(PlayerIndex::Player1), m_controller.GetCharaColorIndexP1(), isSameColor);
	m_player2 = std::make_shared<Player>(PlayerIndex::Player2, m_controller.GetSelectCommandIndex(PlayerIndex::Player2), m_controller.GetCharaColorIndexP2(), isSameColor);
	//ゲームマネージャー
	m_gameManager = std::make_shared<GameManager>();
	//UI
	m_ui = std::make_shared<UI>(m_controller.GetSelectCommandIndex(PlayerIndex::Player1), m_controller.GetSelectCommandIndex(PlayerIndex::Player2));
	//弾
	m_bullet1 = std::make_shared<Bullet>(PlayerIndex::Player1);
	m_bullet2 = std::make_shared<Bullet>(PlayerIndex::Player2);
	//8色に光らせる準備
	CreateColorHandle();
	//カメラ
	m_camera = std::make_shared<Camera>();
	//BGM
	m_bgm = std::make_shared<BGM>();
	m_bgmHandle = LoadSoundMem("./BGM/BGM_Stage2.mp3");
	m_bgm->SetBGM(m_bgmHandle);
	m_bgm->Volume(kBgmVolume);
	m_bgm->PlayLoop();

	//初期化
	GameInit();
}

GameScene::~GameScene()
{
	for (int i = 0; i < kColorNum; ++i)
	{
		DeleteGraph(m_backColorHandle[i]);
		DeleteGraph(m_floorColorHandle[i]);
	}
	DeleteGraph(m_backBaseHandle);
	DeleteGraph(m_floorBaseHandle);
	DeleteGraph(m_bgmHandle);
	m_player1->End();
	m_player2->End();
}

void GameScene::Update(Input& input, Input& input2)
{

#if _DEBUG
	if (input.IsTrigger("Select"))
	{
		//押されたら次の状態に繊維
		//次の状態はこのクラスが覚えておく
		m_controller.ChangeScene(std::make_shared<CommandSelectScene>(m_controller));
		return;//忘れずreturn
	}
	if (input.IsTrigger("Start"))
	{
		//勝ったプレイヤーのインデックスをセット
		m_controller.SaveWinPlayerIndex(PlayerIndex::Player1);
		m_controller.ChangeScene(std::make_shared<ResultScene>(m_controller));
		return;//忘れずreturn
	}
#endif
	m_camera->Update(*m_player1, *m_player2, *m_gameManager);
	//戦闘前はUpdateを止める
	//ヒットストップ中はUpdateを止める
	if (!m_gameManager->IsHitStop() && m_gameManager->IsStartRound())
	{
		m_player1->Update(input, *m_player2, m_bullet1, *m_gameManager);
		m_player2->Update(input2, *m_player1, m_bullet2, *m_gameManager);
		m_bullet1->Update(*m_player2, *m_bullet2, *m_camera);
		m_bullet2->Update(*m_player1, *m_bullet1, *m_camera);
		m_gameManager->Update(*m_player1, *m_player2, *m_bullet1, *m_bullet2,
			*m_camera, *m_ui);
	}
	m_gameManager->NoStopUpdate(*m_player1, *m_player2);
	m_ui->Update(m_player1->GetHp(), m_player2->GetHp(), *m_gameManager);

	//2本取ったらゲーム終了
	if (m_gameManager->IsGameset())
	{
		////BGMを止める
		//soundManager.StopBGM();

		//勝ったプレイヤーのインデックスをセット
		m_controller.SaveWinPlayerIndex(m_gameManager->ResultWinPlayerIndex());
		m_controller.ChangeScene(std::make_shared<ResultScene>(m_controller));
		return;//忘れずreturn
	}
	//ラウンドの切り替わり
	if (m_gameManager->IsChangeRound())
	{
		//リセット
		GameInit();
		m_gameManager->OffIsChangeRound();
	}

	UpdateStageColor();
}

void GameScene::Draw()
{
	//ステージ
	DrawStage();
	m_ui->DrawBack();
	//攻撃したほうを前に描画
	if (m_gameManager->GetIsDrawFrontP1())
	{
		m_player2->Draw(*m_camera);
		m_player1->Draw(*m_camera);
	}
	else
	{
		m_player1->Draw(*m_camera);
		m_player2->Draw(*m_camera);
	}
	m_bullet1->Draw(*m_camera);
	m_bullet2->Draw(*m_camera);
	m_ui->DrawFront();
	//フェードインアウト
	m_gameManager->Draw(*m_camera);
#if _DEBUG	
	DrawString(10, 10, "GameScene", 0xffffff);
	DrawCircleAA(m_camera->m_drawOffset.x, m_camera->m_drawOffset.y, 50, 0xff0000, true);
	DrawCircleAA(m_camera->m_pos.x, m_camera->m_pos.y, 40, 0xff0000, true);
#endif
}

