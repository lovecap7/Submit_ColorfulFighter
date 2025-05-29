#pragma once
#include "Vector3.h"
#include <memory>
#include "Box.h"
#include "AttackInfo.h"
#include "game.h"
class Chara;
class Input;
class Camera;
class Bullet;
class GameManager;
class SE;

class Player
{
private:
	PlayerIndex m_playerIndex;	//自分がプレイヤー1なのか2なのか
	Vector3		m_pos;		//プレイヤーの座標
	Vector3	    m_velocity;	//プレイヤーの移動量
	float		m_angle;//回転
	float		m_hp;
	Vector3		m_jumpVelo;		//ジャンプの移動量
	Vector3		m_giveAttackVelo;//攻撃によって相手を動かす力
	Vector3		m_giveThrowVelo;//投げが与える力
	int			m_handle;		//プレイヤーの現在の画像
	//キャラクター
	std::shared_ptr<Chara> m_chara;
	//プレイヤーの状態管理
	bool m_isSameColor;		//相手と同じ色ならtrue
	bool m_isLeft;			//trueなら左向き
	bool m_isGround;		//trueなら地上にいる
	bool m_isHitAttack;		//trueなら攻撃を当てている
	bool m_isHitGrasp;		//trueならつかみを当てている
	bool m_isGuard;			//trueなら攻撃されたときガードをする
	bool m_isSquat;			//trueならしゃがみ
	bool m_isThrown;		//trueなら投げられている
	bool m_isThrowSuccess;	//trueなら投げ成功
	bool m_isCommand;		//trueならコマンド技を行っている
	bool m_isPossibleCancel;//硬直をキャンセルできる
	int	 m_cancelFrameCount;
	void Cancel(Input& input, Player enemy, std::shared_ptr<Bullet> myBullet, GameManager& gameManager);//キャンセル
	void InputDelay(Input& input, Player enemy, std::shared_ptr<Bullet> myBullet, GameManager& gameManager);//ボタンを押して技が確定するまでの処理
	bool CheckDead();//HPの状態を管理
	void CheckGround();//地上にいるかチェック
	void Gravity();//重力
	void LookDir(Player& enemy);//相手のほうを向く
	//選んだ技のインデックス
	int m_selectCommandIndex[3];
	//押し合い判定
	Box m_pushBox;
	//押し合い判定の初期化
	void InitPushBox();
	//やられ判定
	Box m_hitBoxHead;
	Box m_hitBoxBody;
	Box m_hitBoxLeg;
	//投げやられ
	Box m_hitBoxThrow;
	//つかみ判定
	Box m_hitBoxGrasp;
	//攻撃判定
	Box m_hitBoxAttack;
	//アニメーション関連
	int m_animCountFrame;	//フレームを数える
	int m_animIndex;	//アニメーションの番号を数える(1増える毎にアニメーションが進む)
	int m_animNum;		//アニメーションの数
	int m_oneAnimIntervalFrame;	//1枚のアニメーションにかかるフレーム
	//攻撃手段　パンチとかキックとか
	AttackTypes m_attackType;
	int		m_startAttackFrame;//攻撃の発生
	int		m_finishAttackFrame;//持続
	int		m_giveNoActFrame;	//自分が攻撃したとき相手が動けないフレーム
	int		m_giveGuardFrame;	//自分が攻撃したとき相手がガードをして動けないフレーム
	float	m_giveDamage;		//相手に与えるダメージ
	int	    m_guardFrame;		//自分がガードで動けないフレーム
	Vector3 m_knockback;		//自分が吹っ飛ばされる力
	//敵に攻撃を当てたかどうか
	bool CheckHit(Player& enemy);
	//敵につかみを当てたかどうか
	bool CheckHitGrasp(Player& enemy);
	//現在の攻撃の属性
	AttackAttributes m_attackAttributes;
	int m_noActFrame;		//自分が動けないフレーム
	int m_allNoActFrame;		//自分が動けない全体フレーム
	//SE
	std::shared_ptr<SE> m_se;
	//ハンドル
	int m_walkSeHandle;//足音
	int m_lightPunchSeHandle;
	int m_lightKickSeHandle;
	int m_highPunchSeHandle;
	int m_highKickSeHandle;
	int m_downSeHandle;//倒れる音
	int m_jumpSeHandle;//飛ぶ音
	int m_jumpedSeHandle;//着地したときの音
	int m_graspSeHandle;//つかむ音
	int m_throwSeHandle;//投げる
	int m_loseSeHandle;//うーわ
	int m_winSeHandle;//やった！
	void JumpSE();
	void JumpedSE();
	void AttackSE();//攻撃によって風を切る音
	void DownSE();
	void GraspSE();
	void ThrowSE();
	void LoseSE();
	void WinSE();
	//影
	void DrawShadow(const Camera& camera);
	//同じボタンを押し続けている場合20フレームだけ入力を取りたい
	struct PressBottun
	{
		bool isPress;
		int pressCountFrame;
		void Init(bool IsPress, int PressCountFrame)
		{
			isPress = IsPress;
			pressCountFrame = PressCountFrame;
		}
	};
	PressBottun m_lightPunchBottun;
	PressBottun m_highPunchBottun;
	PressBottun m_lightKickBottun;
	PressBottun m_highKickBottun;
	PressBottun m_graspBottun;
	//プレイヤー専用の入力
	void PlayerInput(Input& input);
	void CheckContinuePressBottun(PressBottun& bottun);

	//メンバ関数ポインタの準備
//Update
	using UpdateFunc_t = void(Player::*)(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	UpdateFunc_t m_update;
	//Draw
	using DrawFunc_t = void(Player::*)(const Camera& camera);
	DrawFunc_t m_draw;

	//立ち待機
	void IdleStandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void IdleStandDraw(const Camera& camera);

	//しゃがみ待機
	void IdleSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void IdleSquatDraw(const Camera& camera);

	//ジャンプ
	void JumpUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void JumpDraw(const Camera& camera);

	//立ち攻撃
	void AttackStandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void AttackStandDraw(const Camera& camera);

	//しゃがみ攻撃
	void AttackSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void AttackSquatDraw(const Camera& camera);

	//空中攻撃
	void AttackAerialUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void AttackAerialDraw(const Camera& camera);

	//空中攻撃後
	void AttackedAerialUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void AttackedAerialDraw(const Camera& camera);

	//コマンド1
	void CommandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void CommandDraw(const Camera& camera);

	//立ちガード
	void GuardStandUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void GuardStandDraw(const Camera& camera);

	//しゃがみガード
	void GuardSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void GuardSquatDraw(const Camera& camera);

	//つかみ -> 当たったら投げに移行
	void GraspUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void GraspDraw(const Camera& camera);

	//投げる
	void ThrowUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void ThrowDraw(const Camera& camera);

	//投げられる
	void BeThrownUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void BeThrownDraw(const Camera& camera);

	//投げぬけ
	void ThrowEscapeUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void ThrowEscapeDraw(const Camera& camera);

	//立ち喰らい
	void DamageUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void DamageDraw(const Camera& camera);

	//しゃがみ喰らい
	void DamageSquatUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void DamageSquatDraw(const Camera& camera);

	//ダウン
	void DownUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet,GameManager& gameManager);
	void DownDraw(const Camera& camera);
	//空中ダウン
	void DownAerialUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet,GameManager& gameManager);
	void DownAerialDraw(const Camera& camera);

	//起き上がり
	void StandUpUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void StandUpDraw(const Camera& camera);

	//勝敗ポーズ
	void ResultUpdate(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet,GameManager& gameManager);
	void ResultDraw(const Camera& camera);
#if _DEBUG
	//デバッグ用
	//アニメーションのフレームを保存する
	int debugJumpFrame = 0;
	int debugCommandFrame = 0;
	//当たり判定の表示
	void DrawHitBox(const Camera& camera);
	bool m_isCheckAttackBox = false;//攻撃判定の可視化
#endif
public:
	Player(PlayerIndex playeIndex, int* selectCommandIndex, CharaColorIndex charaColorIndex, bool isSameColor);
	~Player();
	void Init(float X, bool isLeft);
	void Update(Input& input, Player& enemy, std::shared_ptr<Bullet>& myBullet, GameManager& gameManager);
	void Draw(const Camera& camera);
	void End();
	//なんPなのか
	PlayerIndex GetPlayerIndex() { return m_playerIndex; }
	//硬直
	void SetNoActFrame(int noActFrame)
	{
		m_noActFrame = noActFrame;		//自分が動けないフレーム
		m_allNoActFrame = noActFrame;		//自分が動けない全体フレーム
	}
	void SetGuardFrame(int guardFrame) { m_guardFrame = guardFrame; }
	//ノックバック
	void SetKnockback(Vector3 knockback) { m_knockback = knockback; }
	//座標
	Vector3 GetPos() { return m_pos; }
	void SetPos(Vector3 vec) { m_pos = vec; }
	//速度（力）
	Vector3 GetVelo() { return m_velocity; }
	void SetVelo(Vector3 vec) { m_velocity = vec; }
	void AddVelo(Vector3 vec) { m_velocity += vec; }
	void SetVeloX(float x) { m_velocity.x = x; }
	void SetVeloY(float y) { m_velocity.y = y; }
	//bool
	bool GetDirState() const { return m_isLeft; }//向いてる方向
	bool IsGuard() const { return m_isGuard; }//ガードをしているか
	bool IsGround() const { return m_isGround; }//地上にいるか
	bool IsCommand() { return m_isCommand; }//コマンド技をしている
	bool IsSquat() const { return m_isSquat; }//しゃがんでるか
	bool IsThrowSuccess() const { return m_isThrowSuccess; }//つかみが成功したか
	//投げの処理で使う
	bool IsHitAttack() const { return m_isHitAttack; }//攻撃が当たったか
	bool IsHitGrasp() const { return m_isHitGrasp; }//つかみが当たったか
	//true投げぬけにする
	void OnIsThrownSuccess() { m_isThrowSuccess = true; }
	//投げられたらtrueにする
	void OnIsThrown() { m_isThrown = true; }
	//攻撃が当たりキャンセルができるようにする
	void OnIsPossibleCancel() { m_isPossibleCancel = true; }
	//投げの速度（力）
	void SetThrowVelo(Vector3 vec) { m_giveThrowVelo = vec; }
	//HP
	float GetHp() { return m_hp; }
	void SetHp(float hp) { m_hp = hp; }
	//キャラクターの技の情報をプレイヤーにセットするために使う関数
	void SetHandle(int handle) { m_handle = handle; }//画像
	void SetAnimNum(int animNum) { m_animNum = animNum; }//アニメーションの数
	void SetAnimIndex(int animIndex) { m_animIndex = animIndex; }//現在のアニメーションを特定のタイミングから始めたいときとかに使う
	int GetAnimIndex() { return m_animIndex; }//現在のアニメーションを特定のタイミングから始めたいときとかに使う
	void SetOneAnimFrame(int oneAnimFrame) { m_oneAnimIntervalFrame = oneAnimFrame; }//アニメーション１枚にかかるフレーム
	int GetAnimCountFrame() { return m_animCountFrame; }//アニメーションのフレーム
	void SetAttackAttributes(AttackAttributes attackAttributes) { m_attackAttributes = attackAttributes; }//攻撃属性
	void SetStartAttackFrame(int startAttackFrame) { m_startAttackFrame = startAttackFrame; }//攻撃の発生
	void SetFinishAttackFrame(int finishAttackFrame) { m_finishAttackFrame = finishAttackFrame; }//攻撃持続終了
	void SetGiveNoActFrame(int giveNoActFrame) { m_giveNoActFrame = giveNoActFrame; }//硬直
	void SetGiveGuardFrame(int giveGuardFrame) { m_giveGuardFrame = giveGuardFrame; }//ガード硬直
	void SetGiveDamage(float giveDamage) { m_giveDamage = giveDamage; }//ダメージ
	//攻撃によって相手に与えるVelo
	Vector3 GetGiveAttackVelo() { return m_giveAttackVelo; }
	void SetGiveAttackVelo(Vector3 giveAttackVelo) { m_giveAttackVelo = giveAttackVelo; }
	//プレイヤーの攻撃の手段
	AttackTypes GetAttackAttackTypes() { return m_attackType; }
	//プレイヤーの攻撃の属性取得
	AttackAttributes GetAttackAttributes() { return m_attackAttributes; }
	//押し合い判定
	Box GetPushBox() { return m_pushBox; }
	//やられ判定
	Box GetHitBoxHead() { return  m_hitBoxHead; }
	Box GetHitBoxBody() { return  m_hitBoxBody; }
	Box GetHitBoxLeg() { return  m_hitBoxLeg; }
	Box GetHitBoxAttack() { return  m_hitBoxAttack; }
	//当たり判定とか
	void ResetAttackBox() {
		//攻撃の判定をリセット
		m_hitBoxAttack.x1 = 0;
		m_hitBoxAttack.y1 = 0;
		m_hitBoxAttack.x2 = 0;
		m_hitBoxAttack.y2 = 0;
	}
	void ResetGraspBox() {
		//投げの判定をリセット
		m_hitBoxGrasp.x1 = 0;
		m_hitBoxGrasp.y1 = 0;
		m_hitBoxGrasp.x2 = 0;
		m_hitBoxGrasp.y2 = 0;
	}
	void ResetHitBox() {
		//やられ判定をリセット
		//頭
		m_hitBoxHead.x1 = 0;
		m_hitBoxHead.y1 = 0;
		m_hitBoxHead.x2 = 0;
		m_hitBoxHead.y2 = 0;
		//体
		m_hitBoxBody.x1 = 0;
		m_hitBoxBody.y1 = 0;
		m_hitBoxBody.x2 = 0;
		m_hitBoxBody.y2 = 0;
		//足
		m_hitBoxLeg.x1 = 0;
		m_hitBoxLeg.y1 = 0;
		m_hitBoxLeg.x2 = 0;
		m_hitBoxLeg.y2 = 0;
		//つかまれる判定
		m_hitBoxThrow.x1 = 0;
		m_hitBoxThrow.y1 = 0;
		m_hitBoxThrow.x2 = 0;
		m_hitBoxThrow.y2 = 0;
	}
	//押し合い判定を消す
	void ResetPushBox() {
		m_pushBox.x1 = 0;
		m_pushBox.y1 = 0;
		m_pushBox.x2 = 0;
		m_pushBox.y2 = 0;
	}
	//当たったことをリセット
	void OffIsHitAttack() { m_isHitAttack = false; }
	void OffIsHitGrasp() { m_isHitGrasp = false; }
	//やられ判定や攻撃判定のセット
	void SetHitBoxHead(Box hitBoxHead) { m_hitBoxHead = hitBoxHead; }//頭
	void SetHitBoxBody(Box hitBoxBody) { m_hitBoxBody = hitBoxBody; }//体
	void SetHitBoxLeg(Box hitBoxLeg) { m_hitBoxLeg = hitBoxLeg; }//足
	void SetHitBoxThrow(Box hitBoxThrow) { m_hitBoxThrow = hitBoxThrow; }//つかまれる判定
	void SetHitBoxAttack(Box hitBoxAttack) { m_hitBoxAttack = hitBoxAttack; }//攻撃判定
	void SetHitBoxGrasp(Box hitBoxGrasp) { m_hitBoxGrasp = hitBoxGrasp; }//つかみ判定
	float GetGiveDamage() const { return m_giveDamage; }//相手に与えるダメージの参照
	int  GetGiveNoActFrame() const { return m_giveNoActFrame; }//相手に与える硬直フレームの参照
	int  GetGiveGuardFrame() const { return m_giveGuardFrame; }//相手に与えるガード硬直フレームの参照
	//グラフィックを変化するタイミングをゲームマネージャーしてもらう
	void LoadStateHit();//やられ
	void LoadStateBeThrown();//投げられ
	void LoadStateThrowEscape();//投げぬけ
	void LoadStateGuardStand();//立ちガード
	void LoadStateGuardSquat();//しゃがみガード
	void LoadStateDown();//ダウン
	void LoadStateDownAerial();//空中ダウン
};

