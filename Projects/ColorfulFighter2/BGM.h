#pragma once
#include <string>
class BGM
{
private:
	//���݂�BGM
	int m_bgmHandle;
	//��x�����Ă΂ꂽ���̂�bool�^�ň�x�Ă΂ꂽ��true�ɂ��ČĂׂȂ�����
	bool m_isPlay;
	//��
	int m_bgmStage1;
	int m_bgmResult;
	int m_bgmTitleScene;
	int m_bgmSelectScene;
public:
	BGM();
	~BGM();
	void SetBGM(int bgmIndex);
	void PlayOnce();//�Đ�
	void PlayLoop();//Loop�Đ�
	void Stop();//��~
	bool CheckEndBGM();//BGM���I����������`�F�b�N
	void Volume(int volume);
};

