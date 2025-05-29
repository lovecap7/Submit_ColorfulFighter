#pragma once
#include <string>
class BGM
{
private:
	//現在のBGM
	int m_bgmHandle;
	//一度だけ呼ばれたいのでbool型で一度呼ばれたらtrueにして呼べなくする
	bool m_isPlay;
	//曲
	int m_bgmStage1;
	int m_bgmResult;
	int m_bgmTitleScene;
	int m_bgmSelectScene;
public:
	BGM();
	~BGM();
	void SetBGM(int bgmIndex);
	void PlayOnce();//再生
	void PlayLoop();//Loop再生
	void Stop();//停止
	bool CheckEndBGM();//BGMが終わったかをチェック
	void Volume(int volume);
};

