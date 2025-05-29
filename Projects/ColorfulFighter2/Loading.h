#pragma once
#include "Vector3.h"
class Loading
{
private:
	int m_loadingHandle;
    int m_backgroundHandle;
    struct Actor//戦ってる人たちの描画に使う
    {
        int handle;
        int animIndex;	//アニメーションの番号を数える(1増える毎にアニメーションが進む)
        int animNum;		//アニメーションの数
        int oneAnimIntervalFrame;	//1枚のアニメーションにかかるフレーム
        /// <summary>
        /// 初期化
        /// </summary>
        /// <param name="handle">ハンドル</param>
        /// <param name="animNum">絵の枚数</param>
        Actor(int Handle, int AnimNum, int OneAnimIntervalFrame) :
            handle(Handle),
            animIndex(0),
            animNum(AnimNum),
            oneAnimIntervalFrame(OneAnimIntervalFrame)
        {
        }
    };
    //歩いてる人
    Actor m_actor1;
    Actor m_actor2;
    Actor m_actor3;
    Actor m_actor4;
    Actor m_actor5;
    Vector3 m_actor1Pos;
    Vector3 m_actor2Pos;
    Vector3 m_actor3Pos;
    Vector3 m_actor4Pos;
    Vector3 m_actor5Pos;
    //アニメーション関連
    int m_animCountFrame;//戦ってるキャラクターのアニメーションに使う
public:
	Loading();
	~Loading();
	void Update();
	void Draw();
};

