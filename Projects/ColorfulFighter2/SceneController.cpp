#include "SceneController.h"
#include <memory>
//継承元の関数も認識してくれる
#include "TitleScene.h"

SceneController::SceneController()
{
	//一番最初のシーンだけは割り当てる
	//自分自身のインスタンスを渡してあげる
	m_scene = std::make_shared<TitleScene>(*this);
}

void SceneController::Update(Input& input, Input& input2)
{
	m_scene->Update(input,input2);
}

void SceneController::Draw()
{
	m_scene->Draw();
}

void SceneController::ChangeScene(std::shared_ptr<SceneBase> scene)
{
	m_scene = scene;
}

void SceneController::SaveSelectCommandIndex(int indexP1[3], int indexP2[3])
{
	for (int i = 0;i < 3;++i)
	{
		m_selectCommandIndexP1[i] = indexP1[i];
		m_selectCommandIndexP2[i] = indexP2[i];
	}
}

int* SceneController::GetSelectCommandIndex(PlayerIndex playerIndex)
{
	if (playerIndex == PlayerIndex::Player1)
	{
		return m_selectCommandIndexP1;
	}
	else if (playerIndex == PlayerIndex::Player2)
	{
		return m_selectCommandIndexP2;
	}
}

void SceneController::SaveCharaColorIndexP1(CharaColorIndex colorIndex)
{
	m_charaColorIndexP1 = colorIndex;
}

void SceneController::SaveCharaColorIndexP2(CharaColorIndex colorIndex)
{
	m_charaColorIndexP2 = colorIndex;
}
