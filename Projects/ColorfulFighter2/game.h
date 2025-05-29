#pragma once

//基本16:9 (4:3もOK)
//このサイズ以外は特別な理由がない限り基本ダメ
namespace Game//属性
{
	//画面の大きさを定数定義	16:9
	constexpr int kScreenWidth = 1600;
	constexpr int kScreenHeight = 900;
	constexpr int kColorBitNum = 32;

	constexpr bool kDefaultWindowMode = true;
}

enum class PlayerIndex
{
	Index,
	Player1,
	Player2,
	Player3,
	Player4
};

enum class CharaColorIndex
{
	White,
	Red,
	Blue,
	Yellow,
	Green
};