#pragma once
#include "Vector3.h"
class Loading
{
private:
	int m_loadingHandle;
    int m_backgroundHandle;
    struct Actor//����Ă�l�����̕`��Ɏg��
    {
        int handle;
        int animIndex;	//�A�j���[�V�����̔ԍ��𐔂���(1�����閈�ɃA�j���[�V�������i��)
        int animNum;		//�A�j���[�V�����̐�
        int oneAnimIntervalFrame;	//1���̃A�j���[�V�����ɂ�����t���[��
        /// <summary>
        /// ������
        /// </summary>
        /// <param name="handle">�n���h��</param>
        /// <param name="animNum">�G�̖���</param>
        Actor(int Handle, int AnimNum, int OneAnimIntervalFrame) :
            handle(Handle),
            animIndex(0),
            animNum(AnimNum),
            oneAnimIntervalFrame(OneAnimIntervalFrame)
        {
        }
    };
    //�����Ă�l
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
    //�A�j���[�V�����֘A
    int m_animCountFrame;//����Ă�L�����N�^�[�̃A�j���[�V�����Ɏg��
public:
	Loading();
	~Loading();
	void Update();
	void Draw();
};

