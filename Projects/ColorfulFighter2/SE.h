#pragma once
class SE
{
private:
	//現在のBGM
	int m_seHandle;
	//一度だけ呼ばれたいのでbool型で一度呼ばれたらtrueにして呼べなくする
	bool m_isPlay;
public:
	SE();
	~SE();
	void SetSE(int seIndex);
	void PlayOnce();//再生
	void PlayLoop();//Loop再生
	void Stop();//停止
	bool CheckEndSE();//SEが終わったかをチェック
	void Volume(int volume);
};

