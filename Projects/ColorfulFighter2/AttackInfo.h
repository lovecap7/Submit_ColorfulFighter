#pragma once

//攻撃の手段
enum class AttackTypes
{
	Null,		//何もしてない
	LightPunch,	//弱パンチ
	HighPunch,	//強パンチ
	LightKick,	//弱キック
	HighKick,	//強キック
	Bullet		//弾
};

//攻撃属性
enum class AttackAttributes
{
	Null,	//何もない
	Upper,	//上段
	Lower,	//下段
	Middle  //中段
};

