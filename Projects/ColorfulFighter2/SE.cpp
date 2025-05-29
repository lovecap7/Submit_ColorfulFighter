#include "SE.h"
#include <DxLib.h>
SE::SE():
	m_isPlay(false),
	m_seHandle(-1)
{
}

SE::~SE()
{
	Stop();
}

void SE::SetSE(int seHandle)
{
	//�����T�E���h�Ȃ�ύX���Ȃ�
	if (m_seHandle != seHandle)
	{
		Stop();
		m_seHandle = seHandle;
	}
}

void SE::PlayOnce()
{
	//��x�����Ă�
	if (!m_isPlay)
	{
		PlaySoundMem(m_seHandle, DX_PLAYTYPE_BACK, true);
		m_isPlay = true;
	}
}

void SE::PlayLoop()
{
	//��x�����Ă�
	if (!m_isPlay)
	{
		PlaySoundMem(m_seHandle, DX_PLAYTYPE_LOOP, true);
		m_isPlay = true;
	}
}

void SE::Stop()
{
	StopSoundMem(m_seHandle);
	m_isPlay = false;
}

bool SE::CheckEndSE()
{
	bool isEnd = CheckSoundMem(m_seHandle) == 0;
	return isEnd;
}

void SE::Volume(int volume)
{
	ChangeVolumeSoundMem(volume, m_seHandle);
}
