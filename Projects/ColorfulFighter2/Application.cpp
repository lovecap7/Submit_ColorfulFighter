#include "Application.h"
#include <DxLib.h>
#include <cassert>
#include "game.h"
#include "SceneController.h"
#include "Input.h"
#include "Loading.h"
#include <memory>
#include "resource.h"

Application& Application::GetInstance()
{
    //この時点でメモリが確保されてアプリ終了まで残る
    static Application app;
    return app;
}

bool Application::Init()
{
    //フルスクリーンでなく、ウィンドウモードで開くようにする
    //こういった関数はウィンドウが開く前に(Dxlib.Init()の前)に処理しておく必要がある
	ChangeWindowMode(Game::kDefaultWindowMode);
	//画面サイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);
	//ゲームタイトル
	SetWindowText("ColorfulFighter");
	//ゲームアイコン
	SetWindowIconID(IDI_ICON1);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return false;			// エラーが起きたら直ちに終了
	}

	//ここに書くと一瞬フルスクリーンになるので書かない
	//ChangeWindowMode(true);
	//SetGraphMode(1280, 720, 32);

	//描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

    return true;
}

void Application::Run()
{
	//アプリケーション以外はここで宣言と初期化
	SceneController sceneController;

	//プレイヤー1
	Input input;
	input.Init();
	//プレイヤー2
	Input input2;
	input2.Init();
	input2.PadInit(2);
	////ロード画面
	//Loading loading;

	//ゲームループ
	while (ProcessMessage() == 0) // Windowsが行う処理を待つ
	{
		//今回のループが始まった時間を覚えておく
		LONGLONG time = GetNowHiPerformanceCount();

		//画面全体をクリア
		ClearDrawScreen();

		////ここにゲームの処理を書く
		input.Update();
		input2.Update();
		sceneController.Update(input,input2);
		sceneController.Draw();
		/*loading.Update();
		loading.Draw();*/

		//画面の切り替わりを待つ必要がある
		ScreenFlip();//1/60秒経過するまで待つ

		//FPSを60に固定
		while (GetNowHiPerformanceCount() - time < 16667);
		{

		}
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}
	}
}

void Application::Terminate()
{
	DxLib_End();				// ＤＸライブラリ使用の終了処理
}

