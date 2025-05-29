#pragma once
#include "DxLib.h"
#include "Box.h"
#include "Vector3.h"
#include <string>
#include "AttackInfo.h"
#include "game.h"
#include <memory>

class Input;
class Player;
class Bullet;
class SE;
class Chara
{	
private:
	//voice
	std::shared_ptr<SE> m_voice;
	int m_attack1VoiceHandle;
	int m_attack2VoiceHandle;
	int m_attack3VoiceHandle;
	int m_damage1VoiceHandle;
	int m_damage2VoiceHandle;
	int m_standUp1VoiceHandle;
	int m_standUp2VoiceHandle;
	void VoiceSe(int voiceHandle);
	//se
	std::shared_ptr<SE> m_se;
	int m_commandSeHandle[9];
	void CommandSe(int commandIndex);
	//選んだ技のインデックス
	int m_selectCommandIndex[3];
	bool CheckMyCommand();
	//色
	CharaColorIndex m_charaColorIndex;
	void InitColor(CharaColorIndex charaColorIndex);
	//コマンド技
	enum class Command
	{
		Null,		//0をコマンド失敗という意味にしたい
		Hadouken,	//波動拳
		Shouryuken,	//昇竜拳
		Sukuryuu,	//スクリュー
		Supairaru,	//スパイラルアロー
		Sonic,	//ソニックブーム
		Wave,//闇払い
		Rolling,//バーチカルローリング
		Tatumaki,//竜巻旋風脚
		Wildhant,//ワイルドハント
	};
	int m_commandIndex;//何のコマンドを呼ぶのかを記録
	//波動拳
	//コマンド技モーション取得
	void GetAnimCommand1(Player& player);
	//コマンド技モーションの当たり判定(右向き)
	void GetHitBoxCommand1(Player& player);
	//コマンド技が相手にもたらす効果
	void GetGiveEffectCommand1(Player& player);
	//コマンド技の内容（動き）
	void MovementCommand1(Player& player, Bullet& bullet, Player& enemy);

	//昇竜拳
	//コマンド技モーション取得
	void GetAnimCommand2(Player& player);
	//コマンド技モーションの当たり判定(右向き)
	void GetHitBoxCommand2(Player& player);
	//コマンド技が相手にもたらす効果
	void GetGiveEffectCommand2(Player& player);
	//コマンド技の内容（動き）
	void MovementCommand2(Player& player, Bullet& bullet, Player& enemy);

	//スクリューパイルドライバー
	//コマンド技モーション取得
	void GetAnimCommand3(Player& player);
	//コマンド技モーションの当たり判定(右向き)
	void GetHitBoxCommand3(Player& player);
	//コマンド技が相手にもたらす効果
	void GetGiveEffectCommand3(Player& player);
	//コマンド技の内容（動き）
	void MovementCommand3(Player& player, Bullet& bullet, Player& enemy);

	//スパイラルアロー
	//コマンド技モーション取得
	void GetAnimCommand4(Player& player);
	//コマンド技モーションの当たり判定(右向き)
	void GetHitBoxCommand4(Player& player);
	//コマンド技が相手にもたらす効果
	void GetGiveEffectCommand4(Player& player);
	//コマンド技の内容（動き）
	void MovementCommand4(Player& player, Bullet& bullet, Player& enemy);

	//ソニックブーム
	//コマンド技モーション取得
	void GetAnimCommand5(Player& player);
	//コマンド技モーションの当たり判定(右向き)
	void GetHitBoxCommand5(Player& player);
	//コマンド技が相手にもたらす効果
	void GetGiveEffectCommand5(Player& player);
	//コマンド技の内容（動き）
	void MovementCommand5(Player& player, Bullet& bullet, Player& enemy);

	//パワーウェイブ
	//コマンド技モーション取得
	void GetAnimCommand6(Player& player);
	//コマンド技モーションの当たり判定(右向き)
	void GetHitBoxCommand6(Player& player);
	//コマンド技が相手にもたらす効果
	void GetGiveEffectCommand6(Player& player);
	//コマンド技の内容（動き）
	void MovementCommand6(Player& player, Bullet& bullet, Player& enemy);

	//バーチカルローリング
	//コマンド技モーション取得
	void GetAnimCommand7(Player& player);
	//コマンド技モーションの当たり判定(右向き)
	void GetHitBoxCommand7(Player& player);
	//コマンド技が相手にもたらす効果
	void GetGiveEffectCommand7(Player& player);
	//コマンド技の内容（動き）
	void MovementCommand7(Player& player, Bullet& bullet, Player& enemy);

	//竜巻旋風脚
	//コマンド技モーション取得
	void GetAnimCommand8(Player& player);
	//コマンド技モーションの当たり判定(右向き)
	void GetHitBoxCommand8(Player& player);
	//コマンド技が相手にもたらす効果
	void GetGiveEffectCommand8(Player& player);
	//コマンド技の内容（動き）
	void MovementCommand8(Player& player, Bullet& bullet, Player& enemy);

	//ワイルドハント
	//コマンド技モーション取得
	void GetAnimCommand9(Player& player);
	//コマンド技モーションの当たり判定(右向き)
	void GetHitBoxCommand9(Player& player);
	//コマンド技が相手にもたらす効果
	void GetGiveEffectCommand9(Player& player);
	//コマンド技の内容（動き）
	void MovementCommand9(Player& player, Bullet& bullet, Player& enemy);

	//モーション
	int m_idleStandHandel;//待機
	int m_idleSquatHandel;//しゃがみ待機
	int m_walkFrontHandel;//前歩き
	int m_walkBackHandel;//後ろ歩き
	int m_jumpHandel;//ジャンプ
	//立ち
	int m_punchLightHandle;//弱パンチ
	int m_punchHighHandle;//強パンチ
	int m_kickLightHandle;//弱キック
	int m_kickHighHandle;//強キック
	//しゃがみ
	int m_punchLightSquatHandle;//しゃがみ弱パンチ
	int m_punchHighSquatHandle;//しゃがみ強パンチ
	int m_kickLightSquatHandle;//しゃがみ弱キック
	int m_kickHighSquatHandle;//しゃがみ強キック
	//空中
	int m_punchLightAerialHandle;//空中弱パンチ
	int m_punchHighAerialHandle;//空中強パンチ
	int m_kickLightAerialHandle;//空中弱キック
	int m_kickHighAerialHandle;//空中強キック
	//ガード
	int m_guardStandHandle;//立ちガード
	int m_guardSquatHandle;//しゃがみガード
	//投げ
	int m_graspHandle;//つかみ
	int m_throwFrontHandle;//前投げ
	int m_throwBackHandle;//後ろ投げ
	//ヒット時
	int m_damageStandHandle;//喰らい
	int m_damageAerialHandle;//喰らい
	int m_downHandle;//ダウン
	int m_downAerialHandle;//空中ダウン
	int m_standUpHandle;//起き上がり
	//コマンド技
	int m_commandHandle1;//コマンド技
	int m_commandHandle2;//コマンド技
	int m_commandHandle3;//コマンド技
	int m_commandHandle4;//コマンド技
	int m_commandHandle5;//コマンド技
	int m_commandHandle6;//コマンド技
	int m_commandHandle7;//コマンド技
	int m_commandHandle8;//コマンド技
	int m_commandHandle9;//コマンド技
	//勝利ポーズ
	int m_winPoseHandle;
	//攻撃の属性を覚えておく
	AttackTypes lastAttackType;
	//攻撃によって自分が動く際に使う
	Vector3 m_velocity;
	int m_handle;//画像
	int m_animNum;//アニメーションの数
	int m_oneAnimIntervalFrame;//アニメーション１枚にかかるフレーム
	int m_allFrame;//全体フレーム
	AttackAttributes m_attackAttributes;//攻撃属性
	int m_startAttackFrame;//攻撃の発生
	int m_finishAttackFrame;//攻撃持続終了
	int m_giveNoActFrame;//硬直
	int m_giveGuardFrame;//ガード硬直
	float m_giveDamage;//ダメージ
	//攻撃によって相手に与えるVelo
	Vector3 m_giveAttackVelo;
public:
	Chara(int* index,CharaColorIndex charaColorIndex);
	~Chara();
	//立ちモーション取得
	 void GetAnimIdleStand(Player& player) ;
	//やられ判定　投げやられ判定　攻撃判定
	//待機モーションの当たり判定(右向き)
	 void GetHitBoxIdleStand(Player& player) ;
	
	//しゃがみモーション取得
	void GetAnimIdleSquat(Player& player) ;
	//しゃがみモーションの当たり判定(右向き)
	void GetHitBoxIdleSquat(Player& player) ;

	//前歩きモーション取得
	void GetAnimWalkFront(Player& player) ;
	//前歩きモーションの当たり判定(右向き)
	void GetHitBoxWalkFront(Player& player) ;

	//後ろ歩きモーション取得
	void GetAnimWalkBack(Player& player) ;
	//後ろ歩きモーションの当たり判定(右向き)
	void GetHitBoxWalkBack(Player& player) ;

	//ジャンプモーション取得
	void GetAnimJump(Player& player) ;
	//ジャンプモーションの当たり判定(右向き)
	void GetHitBoxJump(Player& player) ;

	//立ち
	//弱パンチモーション取得
	void GetAnimPunchLight(Player& player);
	//やられ判定　投げやられ判定　攻撃判定
	//弱パンチモーションの当たり判定(右向き)
	void GetHitBoxPunchLight(Player& player) ;
	//弱パンチが相手にもたらす効果
	void GetGiveEffectPunchLight(Player& player) ;

	//強パンチモーション取得
	void GetAnimPunchHigh(Player& player) ;
	//強パンチモーションの当たり判定(右向き)
	void GetHitBoxPunchHigh(Player& player) ;
	//強パンチが相手にもたらす効果
	void GetGiveEffectPunchHigh(Player& player) ;

	//弱キックモーション取得
	void GetAnimKickLight(Player& player) ;
	//弱キックモーションの当たり判定(右向き)
	void GetHitBoxKickLight(Player& player) ;
	//弱キックが相手にもたらす効果
	void GetGiveEffectKickLight(Player& player);

	//強キックモーション取得
	void GetAnimKickHigh(Player& player) ;
	//強キックモーションの当たり判定(右向き)
	void GetHitBoxKickHigh(Player& player) ;
	//強キックが相手にもたらす効果
	void GetGiveEffectKickHigh(Player& player) ;

	//しゃがみ
	//しゃがみ弱パンチモーション取得
	void GetAnimPunchLightSquat(Player& player) ;
	//やられ判定　投げやられ判定　攻撃判定
	//しゃがみ弱パンチモーションの当たり判定(右向き)
	void GetHitBoxPunchLightSquat(Player& player) ;
	//しゃがみ弱パンチが相手にもたらす効果
	void GetGiveEffectPunchLightSquat(Player& player) ;

	//しゃがみ強パンチモーション取得
	void GetAnimPunchHighSquat(Player& player) ;
	//やられ判定　投げやられ判定　攻撃判定
	//しゃがみ強パンチモーションの当たり判定(右向き)
	void GetHitBoxPunchHighSquat(Player& player) ;
	//しゃがみ強パンチが相手にもたらす効果
	void GetGiveEffectPunchHighSquat(Player& player) ;

	//しゃがみ弱キックモーション取得
	void GetAnimKickLightSquat(Player& player) ;
	//やられ判定　投げやられ判定　攻撃判定
	//しゃがみ弱キックモーションの当たり判定(右向き)
	void GetHitBoxKickLightSquat(Player& player) ;
	//しゃがみ弱キックが相手にもたらす効果
	void GetGiveEffectKickLightSquat(Player& player) ;

	//しゃがみ強キックモーション取得
	void GetAnimKickHighSquat(Player& player) ;
	//やられ判定　投げやられ判定　攻撃判定
	//しゃがみ強キックモーションの当たり判定(右向き)
	void GetHitBoxKickHighSquat(Player& player) ;
	//しゃがみ強キックが相手にもたらす効果
	void GetGiveEffectKickHighSquat(Player& player) ;

	//空中弱パンチモーション取得
	void GetAnimPunchLightAerial(Player& player) ;
	//やられ判定　投げやられ判定　攻撃判定
	//空中弱パンチモーションの当たり判定(右向き)
	void GetHitBoxPunchLightAerial(Player& player) ;
	//空中弱パンチが相手にもたらす効果
	void GetGiveEffectPunchLightAerial(Player& player) ;

	//空中強パンチモーション取得
	void GetAnimPunchHighAerial(Player& player) ;
	//やられ判定　投げやられ判定　攻撃判定
	//空中強パンチモーションの当たり判定(右向き)
	void GetHitBoxPunchHighAerial(Player& player) ;
	//空中強パンチが相手にもたらす効果
	void GetGiveEffectPunchHighAerial(Player& player) ;

	//空中弱キックモーション取得
	void GetAnimKickLightAerial(Player& player);
	//やられ判定　投げやられ判定　攻撃判定
	//空中弱キックモーションの当たり判定(右向き)
	void GetHitBoxKickLightAerial(Player& player) ;
	//空中弱キックが相手にもたらす効果
	void GetGiveEffectKickLightAerial(Player& player) ;

	//空中強キックモーション取得
	void GetAnimKickHighAerial(Player& player) ;
	//やられ判定　投げやられ判定　攻撃判定
	//空中強キックモーションの当たり判定(右向き)
	void GetHitBoxKickHighAerial(Player& player) ;
	//空中強キックが相手にもたらす効果
	void GetGiveEffectKickHighAerial(Player& player) ;

	//攻撃後のやられ判定
	void  GetHitBoxAttackedAerial(Player& player);

	//立ちガードモーション取得
	void GetAnimGuardStand(Player& player) ;
	//立ちガードの当たり判定(右向き)
	void GetHitBoxGuardStand(Player& player);
	//しゃがみガードモーション取得
	void GetAnimGuardSquat(Player& player) ;
	//しゃがみガードの当たり判定(右向き)
	void GetHitBoxGuardSquat(Player& player);

	//つかみモーション取得
	void GetAnimGrasp(Player& player) ;
	//つかみの当たり判定(右向き)
	void GetHitBoxGrasp(Player& player) ;
	//つかみがもたらす効果
	void GetGiveEffectGrasp(Player& player);

	//投げられモーションはやられを使うので当たり判定だけ
	void GetHitBoxBeThrown(Player& player) ;

	//前投げのモーション
	void GetAnimThrowFront(Player& player) ;
	//前投げの当たり判定(右向き)
	void GetHitBoxThrowFront(Player& player) ;

	//後ろ投げのモーション
	void GetAnimThrowBack(Player& player) ;
	//後ろ投げの当たり判定(右向き)
	void GetHitBoxThrowBack(Player& player) ;

	//立ちやられモーション取得
	void GetAnimHitStand(Player& player) ;
	//空中やられモーション取得
	void GetAnimHitAerial(Player& player) ;
	//やられ判定
	void GetHitBoxHitStand(Player& player) ;

	//ダウンモーションモーション取得
	void GetAnimDown(Player& player) ;
	//ダウンモーションの当たり判定(右向き)
	void GetHitBoxDown(Player& player) ;
	//空中ダウンモーションモーション取得（着地後にダウンする）
	void GetAnimDownAerial(Player& player) ;
	//空中モーションの当たり判定(右向き)
	void GetHitBoxDownAerial(Player& player) ;

	//起き上がり
	void GetAnimStandUp(Player& player);

	//コマンド
	//コマンドが成立しているか
	bool SuccessCommandCheck(Input& input, Player& player) ;

	//コマンド技モーション取得
	void GetAnimCommand(Player& player) ;
	//コマンド技モーションの当たり判定(右向き)
	void GetHitBoxCommand(Player& player) ;
	//コマンド技が相手にもたらす効果
	void GetGiveEffectCommand(Player& player) ;
	//コマンド技の内容（動き）
	void MovementCommand(Player& player, Bullet& bullet, Player& enemy) ;

	//勝利ポーズ
	void GetAnimWinPose(Player& player) ;
};
