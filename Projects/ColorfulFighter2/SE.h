#pragma once
class SE
{
private:
	//���݂�BGM
	int m_seHandle;
	//��x�����Ă΂ꂽ���̂�bool�^�ň�x�Ă΂ꂽ��true�ɂ��ČĂׂȂ�����
	bool m_isPlay;
public:
	SE();
	~SE();
	void SetSE(int seIndex);
	void PlayOnce();//�Đ�
	void PlayLoop();//Loop�Đ�
	void Stop();//��~
	bool CheckEndSE();//SE���I����������`�F�b�N
	void Volume(int volume);
};

