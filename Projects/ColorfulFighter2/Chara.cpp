#include "Chara.h"
#include "Input.h"
#include "Player.h"
#include "Bullet.h"
#include "SE.h"
#include <cassert>
namespace
{
	//�R�}���h�Z�̐�
	constexpr int kSelectCommandNum = 3;

	//�d���t���[����0�̋Z��Down�ɂ���
	constexpr int kDown = 0;
	//SE�̃{�����[��
	constexpr int kSeVolume = 150;

	//�S�̃t���[��
	constexpr int kAllFramePunchLight = 14;//��p���`
	constexpr int kStartFramePunchLight = 4;//��p���`����
	constexpr int kFinishFramePunchLight = 6;//��p���`�����I��

	constexpr int kAllFramePunchHigh = 22;//���p���`
	constexpr int kStartFramePunchHigh = 13;//���p���`����

	constexpr int  kAllFrameKickLight = 18;//��L�b�N
	constexpr int  kStartFrameKickLight = 5;//��L�b�N����

	constexpr int  kAllFrameKickHigh = 33;//���L�b�N
	constexpr int  kStartFrameKickHigh = 12;//���L�b�N����

	constexpr int kAllFramePunchLightSquat = 8;//���Ⴊ�ݎ�p���`
	constexpr int kStartFramePunchLightSquat = 4;//���Ⴊ�ݎ�p���`����

	constexpr int kAllFramePunchHighSquat = 26;//���Ⴊ�݋��p���`
	constexpr int kStartFramePunchHighSquat = 9;//���Ⴊ�݋��p���`����

	constexpr int kAllFrameKickLightSquat = 14;//���Ⴊ�ݎ�L�b�N
	constexpr int kStartFrameKickLightSquat = 5;//���Ⴊ�ݎ�L�b�N����

	constexpr int kAllFrameKickHighSquat = 14;//���Ⴊ�݋��L�b�N
	constexpr int kStartFrameKickHighSquat = 9;//���Ⴊ�݋��L�b�N

	//�g����
	constexpr int kAllFrameCommand1 = 48;
	constexpr int kStartFrameCommand1Light = 16;//��g��������
	constexpr int kStartFrameCommand1High = 12;//���g��������

	//�������̑S�̃t���[��
	constexpr int kAllFrameCommand2 = 33;
	constexpr int kStartFrameCommand2 = 5;

	//�X�p�C�����A���[�̑S�̃t���[��
	constexpr int kAllFrameCommand4Light = 42;
	constexpr int kStartFrameCommand4Light = 9;
	constexpr int kAllFrameCommand4High = 49;
	constexpr int kStartFrameCommand4High = 15;
	constexpr float kCommand4LightVeloX = 70.0f;
	constexpr float kCommand4HighVeloX = 70.0f;
	constexpr float kDeceleration = 2.0f;

	//�\�j�b�N�u�[��
	constexpr int kAllFrameCommand5 = 40;
	constexpr int kStartFrameCommand5 = 10;//����
	
	//���̂��
	constexpr int kAllFrameCommand6Light = 46;
	constexpr int kAllFrameCommand6High = 52;
	constexpr int kStartFrameCommand6Light = 16;//����
	constexpr int kStartFrameCommand6High = 13;//����

	//�o�[�`�J�����[�����O�̑S�̃t���[��
	constexpr int kAllFrameCommand7 = 48;
	constexpr int kStartFrameCommand7 = 8;

	//���������r
	constexpr int kAllFrameCommand8Light = 48;
	constexpr int kAllFrameCommand8High = 61;
	
}

Chara::Chara(int* selectCommandIndex, CharaColorIndex charaColorIndex) :
	//���[�V����
	m_velocity(),
	m_handle(-1),//�摜
	m_animNum(0),//�A�j���[�V�����̐�
	m_oneAnimIntervalFrame(0),//�A�j���[�V�����P���ɂ�����t���[��
	m_attackAttributes(AttackAttributes::Null),//�U������
	m_startAttackFrame(0),//�U���̔���
	m_finishAttackFrame(0),//�U�������I��
	m_giveNoActFrame(0),//�d��
	m_giveGuardFrame(0),//�K�[�h�d��
	m_giveDamage(0),//�_���[�W
	m_giveAttackVelo(),//�U���ɂ���đ���ɗ^����Velo
	m_commandIndex(0),
	lastAttackType(AttackTypes::Null),
	m_commandSeHandle{
		LoadSoundMem("./SE/waza/hadouSe.mp3"),
		LoadSoundMem("./SE/waza/shouryuuSe.mp3"),
		LoadSoundMem("./SE/waza/sukuryuuSe.mp3"),
		LoadSoundMem("./SE/waza/supairaruSe.mp3"),
		LoadSoundMem("./SE/waza/sonicSe.mp3"),
		LoadSoundMem("./SE/waza/waveSe.mp3"),
		LoadSoundMem("./SE/waza/rollingSe.mp3"),
		LoadSoundMem("./SE/waza/tatumakiSe.mp3"),
		LoadSoundMem("./SE/waza/wildSe.mp3")
	},
	m_attack1VoiceHandle(LoadSoundMem("./SE/PlayerBase/voice/Attack1.mp3")),
	m_attack2VoiceHandle(LoadSoundMem("./SE/PlayerBase/voice/Attack2.mp3")),
	m_attack3VoiceHandle(LoadSoundMem("./SE/PlayerBase/voice/Attack3.mp3")),
	m_damage1VoiceHandle(LoadSoundMem("./SE/PlayerBase/voice/Damage1.mp3")),
	m_damage2VoiceHandle(LoadSoundMem("./SE/PlayerBase/voice/Damage2.mp3")),
	m_standUp1VoiceHandle(LoadSoundMem("./SE/PlayerBase/voice/StandUp1.mp3")),
	m_standUp2VoiceHandle(LoadSoundMem("./SE/PlayerBase/voice/StandUp2.mp3"))
{
	m_se = std::make_shared<SE>();//�{�C�X���m��
	//�F
	InitColor(charaColorIndex);
	m_voice = std::make_shared<SE>();//�{�C�X���m��
	for (int i = 0; i < kSelectCommandNum;++i)
	{
		m_selectCommandIndex[i] = selectCommandIndex[i];
	}
}

Chara::~Chara()
{
	DeleteGraph(m_idleStandHandel);//�ҋ@
	DeleteGraph(m_idleSquatHandel);//���Ⴊ�ݑҋ@
	DeleteGraph(m_walkFrontHandel);//�O����
	DeleteGraph(m_walkBackHandel);//������
	DeleteGraph(m_jumpHandel);//�W�����v
	//����
	DeleteGraph(m_punchLightHandle);//��p���`
	DeleteGraph(m_punchHighHandle);//���p���`
	DeleteGraph(m_kickLightHandle);//��L�b�N
	DeleteGraph(m_kickHighHandle);//���L�b�N
	//���Ⴊ��
	DeleteGraph(m_punchLightSquatHandle);//���Ⴊ�ݎ�p���`
	DeleteGraph(m_punchHighSquatHandle);//���Ⴊ�݋��p���`
	DeleteGraph(m_kickLightSquatHandle);//���Ⴊ�ݎ�L�b�N
	DeleteGraph(m_kickHighSquatHandle);//���Ⴊ�݋��L�b�N
	//��
	DeleteGraph(m_punchLightAerialHandle);//�󒆎�p���`
	DeleteGraph(m_punchHighAerialHandle);//�󒆋��p���`
	DeleteGraph(m_kickLightAerialHandle);//�󒆎�L�b�N
	DeleteGraph(m_kickHighAerialHandle);//�󒆋��L�b�N
	//�K�[�h
	DeleteGraph(m_guardStandHandle);//�����K�[�h
	DeleteGraph(m_guardSquatHandle);//���Ⴊ�݃K�[�h
	//����
	DeleteGraph(m_graspHandle);//����
	DeleteGraph(m_throwFrontHandle);//�O����
	DeleteGraph(m_throwBackHandle);//��듊��
	//����
	DeleteGraph(m_damageStandHandle);//��炢
	DeleteGraph(m_damageAerialHandle);//��炢
	DeleteGraph(m_downHandle);//�_�E��
	DeleteGraph(m_downAerialHandle);//�󒆃_�E��
	DeleteGraph(m_standUpHandle);//�N���オ��

	DeleteGraph(m_commandHandle1);//�R�}���h�Z
	DeleteGraph(m_commandHandle2);//�R�}���h�Z
	DeleteGraph(m_commandHandle3);//�R�}���h�Z
	DeleteGraph(m_commandHandle4);//�R�}���h�Z
	DeleteGraph(m_commandHandle5);//�R�}���h�Z
	DeleteGraph(m_commandHandle6);//�R�}���h�Z
	DeleteGraph(m_commandHandle7);//�R�}���h�Z
	DeleteGraph(m_commandHandle8);//�R�}���h�Z
	DeleteGraph(m_commandHandle9);//�R�}���h�Z
	DeleteGraph(m_winPoseHandle);//�����|�[�Y
	for (auto& handle : m_commandSeHandle)
	{
		DeleteGraph(handle);
	}
	DeleteGraph(m_attack1VoiceHandle);
	DeleteGraph(m_attack2VoiceHandle);
	DeleteGraph(m_attack3VoiceHandle);
	DeleteGraph(m_damage1VoiceHandle);
	DeleteGraph(m_damage2VoiceHandle);
	DeleteGraph(m_standUp1VoiceHandle);
	DeleteGraph(m_standUp2VoiceHandle);
}


void Chara::InitColor(CharaColorIndex charaColorIndex)
{
	switch (charaColorIndex)
	{
	case CharaColorIndex::White://��
		m_idleStandHandel = LoadGraph("./img/Chara/White/playerbase/idle_001.png");//�ҋ@
		m_idleSquatHandel = LoadGraph("./img/Chara/White/playerbase/idle_squat_001.png");//���Ⴊ�ݑҋ@
		m_walkFrontHandel = LoadGraph("./img/Chara/White/playerbase/walk_front_001.png");//�O����
		m_walkBackHandel = LoadGraph("./img/Chara/White/playerbase/walk_back_001.png");//������
		m_jumpHandel = LoadGraph("./img/Chara/White/playerbase/jump_vertical_001.png");//�W�����v
		//����
		m_punchLightHandle = LoadGraph("./img/Chara/White/punch/punch_stand_002.png");//��p���`
		m_punchHighHandle = LoadGraph("./img/Chara/White/punch/punch_stand_001.png");//���p���`
		m_kickLightHandle = LoadGraph("./img/Chara/White/kick/kick_stand_001.png");//��L�b�N
		m_kickHighHandle = LoadGraph("./img/Chara/White/kick/kick_stand_002.png");//���L�b�N
		//���Ⴊ��
		m_punchLightSquatHandle = LoadGraph("./img/Chara/White/punch/punch_squat_001.png");//���Ⴊ�ݎ�p���`
		m_punchHighSquatHandle = LoadGraph("./img/Chara/White/punch/punch_squat_003.png");//���Ⴊ�݋��p���`
		m_kickLightSquatHandle = LoadGraph("./img/Chara/White/kick/kick_squat_001.png");//���Ⴊ�ݎ�L�b�N
		m_kickHighSquatHandle = LoadGraph("./img/Chara/White/kick/kick_squat_003.png");//���Ⴊ�݋��L�b�N
		//��
		m_punchLightAerialHandle = LoadGraph("./img/Chara/White/punch/punch_aerial_001.png");//�󒆎�p���`
		m_punchHighAerialHandle = LoadGraph("./img/Chara/White/punch/punch_aerial_003.png");//�󒆋��p���`
		m_kickLightAerialHandle = LoadGraph("./img/Chara/White/kick/kick_aerial_002.png");//�󒆎�L�b�N
		m_kickHighAerialHandle = LoadGraph("./img/Chara/White/kick/kick_aerial_001.png");//�󒆋��L�b�N

		m_guardStandHandle = LoadGraph("./img/Chara/White/guard/guard_stand_001.png");//�����K�[�h
		m_guardSquatHandle = LoadGraph("./img/Chara/White/guard/guard_squat_002.png");//���Ⴊ�݃K�[�h

		//����
		m_graspHandle = LoadGraph("./img/Chara/White/throw/grasp_001.png");//����
		m_throwFrontHandle = LoadGraph("./img/Chara/White/throw/throw_001.png");//�O����
		m_throwBackHandle = LoadGraph("./img/Chara/White/throw/throw_002.png");//��듊��


		m_damageStandHandle = LoadGraph("./img/Chara/White/hit/hit_stand_001.png");//��炢
		m_damageAerialHandle = LoadGraph("./img/Chara/White/hit/hit_aerial_001.png");//��炢
		m_downHandle = LoadGraph("./img/Chara/White/playerbase/down_001.png");//�_�E��
		m_downAerialHandle = LoadGraph("./img/Chara/White/playerbase/down_aerial_001.png");//�󒆃_�E��
		m_standUpHandle = LoadGraph("./img/Chara/White/playerbase/standUp.png");//�N���オ��

		m_commandHandle1 = LoadGraph("./img/Chara/White/waza/hadou_001.png");//�R�}���h�Z
		m_commandHandle2 = LoadGraph("./img/Chara/White/waza/shouryuu.png");//�R�}���h�Z
		m_commandHandle3 = LoadGraph("./img/Chara/White/waza/sukuryuu.png");//�R�}���h�Z
		m_commandHandle4 = LoadGraph("./img/Chara/White/waza/spairaruaro.png");//�R�}���h�Z
		m_commandHandle5 = LoadGraph("./img/Chara/White/waza/sonic.png");//�R�}���h�Z
		m_commandHandle6 = LoadGraph("./img/Chara/White/waza/wave.png");//�R�}���h�Z
		m_commandHandle7 = LoadGraph("./img/Chara/White/waza/rolling.png");//�R�}���h�Z
		m_commandHandle8 = LoadGraph("./img/Chara/White/waza/tatumaki.png");//�R�}���h�Z
		m_commandHandle9 = LoadGraph("./img/Chara/White/waza/wildhant.png");//�R�}���h�Z
		m_winPoseHandle = LoadGraph("./img/Chara/White/playerbase/win_001.png");//�����|�[�Y}
		break;
	case CharaColorIndex::Red://��
		m_idleStandHandel = LoadGraph("./img/Chara/Red/playerbase/idle_001.png");//�ҋ@
		m_idleSquatHandel = LoadGraph("./img/Chara/Red/playerbase/idle_squat_001.png");//���Ⴊ�ݑҋ@
		m_walkFrontHandel = LoadGraph("./img/Chara/Red/playerbase/walk_front_001.png");//�O����
		m_walkBackHandel = LoadGraph("./img/Chara/Red/playerbase/walk_back_001.png");//������
		m_jumpHandel = LoadGraph("./img/Chara/Red/playerbase/jump_vertical_001.png");//�W�����v
		//����
		m_punchLightHandle = LoadGraph("./img/Chara/Red/punch/punch_stand_002.png");//��p���`
		m_punchHighHandle = LoadGraph("./img/Chara/Red/punch/punch_stand_001.png");//���p���`
		m_kickLightHandle = LoadGraph("./img/Chara/Red/kick/kick_stand_001.png");//��L�b�N
		m_kickHighHandle = LoadGraph("./img/Chara/Red/kick/kick_stand_002.png");//���L�b�N
		//���Ⴊ��
		m_punchLightSquatHandle = LoadGraph("./img/Chara/Red/punch/punch_squat_001.png");//���Ⴊ�ݎ�p���`
		m_punchHighSquatHandle = LoadGraph("./img/Chara/Red/punch/punch_squat_003.png");//���Ⴊ�݋��p���`
		m_kickLightSquatHandle = LoadGraph("./img/Chara/Red/kick/kick_squat_001.png");//���Ⴊ�ݎ�L�b�N
		m_kickHighSquatHandle = LoadGraph("./img/Chara/Red/kick/kick_squat_003.png");//���Ⴊ�݋��L�b�N
		//��
		m_punchLightAerialHandle = LoadGraph("./img/Chara/Red/punch/punch_aerial_001.png");//�󒆎�p���`
		m_punchHighAerialHandle = LoadGraph("./img/Chara/Red/punch/punch_aerial_003.png");//�󒆋��p���`
		m_kickLightAerialHandle = LoadGraph("./img/Chara/Red/kick/kick_aerial_002.png");//�󒆎�L�b�N
		m_kickHighAerialHandle = LoadGraph("./img/Chara/Red/kick/kick_aerial_001.png");//�󒆋��L�b�N

		m_guardStandHandle = LoadGraph("./img/Chara/Red/guard/guard_stand_001.png");//�����K�[�h
		m_guardSquatHandle = LoadGraph("./img/Chara/Red/guard/guard_squat_002.png");//���Ⴊ�݃K�[�h

		//����
		m_graspHandle = LoadGraph("./img/Chara/Red/throw/grasp_001.png");//����
		m_throwFrontHandle = LoadGraph("./img/Chara/Red/throw/throw_001.png");//�O����
		m_throwBackHandle = LoadGraph("./img/Chara/Red/throw/throw_002.png");//��듊��


		m_damageStandHandle = LoadGraph("./img/Chara/Red/hit/hit_stand_001.png");//��炢
		m_damageAerialHandle = LoadGraph("./img/Chara/Red/hit/hit_aerial_001.png");//��炢
		m_downHandle = LoadGraph("./img/Chara/Red/playerbase/down_001.png");//�_�E��
		m_downAerialHandle = LoadGraph("./img/Chara/Red/playerbase/down_aerial_001.png");//�󒆃_�E��
		m_standUpHandle = LoadGraph("./img/Chara/Red/playerbase/standUp.png");//�N���オ��

		m_commandHandle1 = LoadGraph("./img/Chara/Red/waza/hadou_001.png");//�R�}���h�Z
		m_commandHandle2 = LoadGraph("./img/Chara/Red/waza/shouryuu.png");//�R�}���h�Z
		m_commandHandle3 = LoadGraph("./img/Chara/Red/waza/sukuryuu.png");//�R�}���h�Z
		m_commandHandle4 = LoadGraph("./img/Chara/Red/waza/spairaruaro.png");//�R�}���h�Z
		m_commandHandle5 = LoadGraph("./img/Chara/Red/waza/sonic.png");//�R�}���h�Z
		m_commandHandle6 = LoadGraph("./img/Chara/Red/waza/wave.png");//�R�}���h�Z
		m_commandHandle7 = LoadGraph("./img/Chara/Red/waza/rolling.png");//�R�}���h�Z
		m_commandHandle8 = LoadGraph("./img/Chara/Red/waza/tatumaki.png");//�R�}���h�Z
		m_commandHandle9 = LoadGraph("./img/Chara/Red/waza/wildhant.png");//�R�}���h�Z
		m_winPoseHandle = LoadGraph("./img/Chara/Red/playerbase/win_001.png");//�����|�[�Y
		break;
	case CharaColorIndex::Blue://��
		m_idleStandHandel = LoadGraph("./img/Chara/Blue/playerbase/idle_001.png");//�ҋ@
		m_idleSquatHandel = LoadGraph("./img/Chara/Blue/playerbase/idle_squat_001.png");//���Ⴊ�ݑҋ@
		m_walkFrontHandel = LoadGraph("./img/Chara/Blue/playerbase/walk_front_001.png");//�O����
		m_walkBackHandel = LoadGraph("./img/Chara/Blue/playerbase/walk_back_001.png");//������
		m_jumpHandel = LoadGraph("./img/Chara/Blue/playerbase/jump_vertical_001.png");//�W�����v
		//����
		m_punchLightHandle = LoadGraph("./img/Chara/Blue/punch/punch_stand_002.png");//��p���`
		m_punchHighHandle = LoadGraph("./img/Chara/Blue/punch/punch_stand_001.png");//���p���`
		m_kickLightHandle = LoadGraph("./img/Chara/Blue/kick/kick_stand_001.png");//��L�b�N
		m_kickHighHandle = LoadGraph("./img/Chara/Blue/kick/kick_stand_002.png");//���L�b�N
		//���Ⴊ��
		m_punchLightSquatHandle = LoadGraph("./img/Chara/Blue/punch/punch_squat_001.png");//���Ⴊ�ݎ�p���`
		m_punchHighSquatHandle = LoadGraph("./img/Chara/Blue/punch/punch_squat_003.png");//���Ⴊ�݋��p���`
		m_kickLightSquatHandle = LoadGraph("./img/Chara/Blue/kick/kick_squat_001.png");//���Ⴊ�ݎ�L�b�N
		m_kickHighSquatHandle = LoadGraph("./img/Chara/Blue/kick/kick_squat_003.png");//���Ⴊ�݋��L�b�N
		//��
		m_punchLightAerialHandle = LoadGraph("./img/Chara/Blue/punch/punch_aerial_001.png");//�󒆎�p���`
		m_punchHighAerialHandle = LoadGraph("./img/Chara/Blue/punch/punch_aerial_003.png");//�󒆋��p���`
		m_kickLightAerialHandle = LoadGraph("./img/Chara/Blue/kick/kick_aerial_002.png");//�󒆎�L�b�N
		m_kickHighAerialHandle = LoadGraph("./img/Chara/Blue/kick/kick_aerial_001.png");//�󒆋��L�b�N

		m_guardStandHandle = LoadGraph("./img/Chara/Blue/guard/guard_stand_001.png");//�����K�[�h
		m_guardSquatHandle = LoadGraph("./img/Chara/Blue/guard/guard_squat_002.png");//���Ⴊ�݃K�[�h

		//����
		m_graspHandle = LoadGraph("./img/Chara/Blue/throw/grasp_001.png");//����
		m_throwFrontHandle = LoadGraph("./img/Chara/Blue/throw/throw_001.png");//�O����
		m_throwBackHandle = LoadGraph("./img/Chara/Blue/throw/throw_002.png");//��듊��


		m_damageStandHandle = LoadGraph("./img/Chara/Blue/hit/hit_stand_001.png");//��炢
		m_damageAerialHandle = LoadGraph("./img/Chara/Blue/hit/hit_aerial_001.png");//��炢
		m_downHandle = LoadGraph("./img/Chara/Blue/playerbase/down_001.png");//�_�E��
		m_downAerialHandle = LoadGraph("./img/Chara/Blue/playerbase/down_aerial_001.png");//�󒆃_�E��
		m_standUpHandle = LoadGraph("./img/Chara/Blue/playerbase/standUp.png");//�N���オ��

		m_commandHandle1 = LoadGraph("./img/Chara/Blue/waza/hadou_001.png");//�R�}���h�Z
		m_commandHandle2 = LoadGraph("./img/Chara/Blue/waza/shouryuu.png");//�R�}���h�Z
		m_commandHandle3 = LoadGraph("./img/Chara/Blue/waza/sukuryuu.png");//�R�}���h�Z
		m_commandHandle4 = LoadGraph("./img/Chara/Blue/waza/spairaruaro.png");//�R�}���h�Z
		m_commandHandle5 = LoadGraph("./img/Chara/Blue/waza/sonic.png");//�R�}���h�Z
		m_commandHandle6 = LoadGraph("./img/Chara/Blue/waza/wave.png");//�R�}���h�Z
		m_commandHandle7 = LoadGraph("./img/Chara/Blue/waza/rolling.png");//�R�}���h�Z
		m_commandHandle8 = LoadGraph("./img/Chara/Blue/waza/tatumaki.png");//�R�}���h�Z
		m_commandHandle9 = LoadGraph("./img/Chara/Blue/waza/wildhant.png");//�R�}���h�Z
		m_winPoseHandle = LoadGraph("./img/Chara/Blue/playerbase/win_001.png");//�����|�[�Y
		break;
	case CharaColorIndex::Yellow://���F
		m_idleStandHandel = LoadGraph("./img/Chara/Yellow/playerbase/idle_001.png");//�ҋ@
		m_idleSquatHandel = LoadGraph("./img/Chara/Yellow/playerbase/idle_squat_001.png");//���Ⴊ�ݑҋ@
		m_walkFrontHandel = LoadGraph("./img/Chara/Yellow/playerbase/walk_front_001.png");//�O����
		m_walkBackHandel = LoadGraph("./img/Chara/Yellow/playerbase/walk_back_001.png");//������
		m_jumpHandel = LoadGraph("./img/Chara/Yellow/playerbase/jump_vertical_001.png");//�W�����v
		//����
		m_punchLightHandle = LoadGraph("./img/Chara/Yellow/punch/punch_stand_002.png");//��p���`
		m_punchHighHandle = LoadGraph("./img/Chara/Yellow/punch/punch_stand_001.png");//���p���`
		m_kickLightHandle = LoadGraph("./img/Chara/Yellow/kick/kick_stand_001.png");//��L�b�N
		m_kickHighHandle = LoadGraph("./img/Chara/Yellow/kick/kick_stand_002.png");//���L�b�N
		//���Ⴊ��
		m_punchLightSquatHandle = LoadGraph("./img/Chara/Yellow/punch/punch_squat_001.png");//���Ⴊ�ݎ�p���`
		m_punchHighSquatHandle = LoadGraph("./img/Chara/Yellow/punch/punch_squat_003.png");//���Ⴊ�݋��p���`
		m_kickLightSquatHandle = LoadGraph("./img/Chara/Yellow/kick/kick_squat_001.png");//���Ⴊ�ݎ�L�b�N
		m_kickHighSquatHandle = LoadGraph("./img/Chara/Yellow/kick/kick_squat_003.png");//���Ⴊ�݋��L�b�N
		//��
		m_punchLightAerialHandle = LoadGraph("./img/Chara/Yellow/punch/punch_aerial_001.png");//�󒆎�p���`
		m_punchHighAerialHandle = LoadGraph("./img/Chara/Yellow/punch/punch_aerial_003.png");//�󒆋��p���`
		m_kickLightAerialHandle = LoadGraph("./img/Chara/Yellow/kick/kick_aerial_002.png");//�󒆎�L�b�N
		m_kickHighAerialHandle = LoadGraph("./img/Chara/Yellow/kick/kick_aerial_001.png");//�󒆋��L�b�N

		m_guardStandHandle = LoadGraph("./img/Chara/Yellow/guard/guard_stand_001.png");//�����K�[�h
		m_guardSquatHandle = LoadGraph("./img/Chara/Yellow/guard/guard_squat_002.png");//���Ⴊ�݃K�[�h

		//����
		m_graspHandle = LoadGraph("./img/Chara/Yellow/throw/grasp_001.png");//����
		m_throwFrontHandle = LoadGraph("./img/Chara/Yellow/throw/throw_001.png");//�O����
		m_throwBackHandle = LoadGraph("./img/Chara/Yellow/throw/throw_002.png");//��듊��


		m_damageStandHandle = LoadGraph("./img/Chara/Yellow/hit/hit_stand_001.png");//��炢
		m_damageAerialHandle = LoadGraph("./img/Chara/Yellow/hit/hit_aerial_001.png");//��炢
		m_downHandle = LoadGraph("./img/Chara/Yellow/playerbase/down_001.png");//�_�E��
		m_downAerialHandle = LoadGraph("./img/Chara/Yellow/playerbase/down_aerial_001.png");//�󒆃_�E��
		m_standUpHandle = LoadGraph("./img/Chara/Yellow/playerbase/standUp.png");//�N���オ��

		m_commandHandle1 = LoadGraph("./img/Chara/Yellow/waza/hadou_001.png");//�R�}���h�Z
		m_commandHandle2 = LoadGraph("./img/Chara/Yellow/waza/shouryuu.png");//�R�}���h�Z
		m_commandHandle3 = LoadGraph("./img/Chara/Yellow/waza/sukuryuu.png");//�R�}���h�Z
		m_commandHandle4 = LoadGraph("./img/Chara/Yellow/waza/spairaruaro.png");//�R�}���h�Z
		m_commandHandle5 = LoadGraph("./img/Chara/Yellow/waza/sonic.png");//�R�}���h�Z
		m_commandHandle6 = LoadGraph("./img/Chara/Yellow/waza/wave.png");//�R�}���h�Z
		m_commandHandle7 = LoadGraph("./img/Chara/Yellow/waza/rolling.png");//�R�}���h�Z
		m_commandHandle8 = LoadGraph("./img/Chara/Yellow/waza/tatumaki.png");//�R�}���h�Z
		m_commandHandle9 = LoadGraph("./img/Chara/Yellow/waza/wildhant.png");//�R�}���h�Z
		m_winPoseHandle = LoadGraph("./img/Chara/Yellow/playerbase/win_001.png");//�����|�[�Y
		break;
	case CharaColorIndex::Green://��
		m_idleStandHandel = LoadGraph("./img/Chara/Green/playerbase/idle_001.png");//�ҋ@
		m_idleSquatHandel = LoadGraph("./img/Chara/Green/playerbase/idle_squat_001.png");//���Ⴊ�ݑҋ@
		m_walkFrontHandel = LoadGraph("./img/Chara/Green/playerbase/walk_front_001.png");//�O����
		m_walkBackHandel = LoadGraph("./img/Chara/Green/playerbase/walk_back_001.png");//������
		m_jumpHandel = LoadGraph("./img/Chara/Green/playerbase/jump_vertical_001.png");//�W�����v
		//����
		m_punchLightHandle = LoadGraph("./img/Chara/Green/punch/punch_stand_002.png");//��p���`
		m_punchHighHandle = LoadGraph("./img/Chara/Green/punch/punch_stand_001.png");//���p���`
		m_kickLightHandle = LoadGraph("./img/Chara/Green/kick/kick_stand_001.png");//��L�b�N
		m_kickHighHandle = LoadGraph("./img/Chara/Green/kick/kick_stand_002.png");//���L�b�N
		//���Ⴊ��
		m_punchLightSquatHandle = LoadGraph("./img/Chara/Green/punch/punch_squat_001.png");//���Ⴊ�ݎ�p���`
		m_punchHighSquatHandle = LoadGraph("./img/Chara/Green/punch/punch_squat_003.png");//���Ⴊ�݋��p���`
		m_kickLightSquatHandle = LoadGraph("./img/Chara/Green/kick/kick_squat_001.png");//���Ⴊ�ݎ�L�b�N
		m_kickHighSquatHandle = LoadGraph("./img/Chara/Green/kick/kick_squat_003.png");//���Ⴊ�݋��L�b�N
		//��
		m_punchLightAerialHandle = LoadGraph("./img/Chara/Green/punch/punch_aerial_001.png");//�󒆎�p���`
		m_punchHighAerialHandle = LoadGraph("./img/Chara/Green/punch/punch_aerial_003.png");//�󒆋��p���`
		m_kickLightAerialHandle = LoadGraph("./img/Chara/Green/kick/kick_aerial_002.png");//�󒆎�L�b�N
		m_kickHighAerialHandle = LoadGraph("./img/Chara/Green/kick/kick_aerial_001.png");//�󒆋��L�b�N

		m_guardStandHandle = LoadGraph("./img/Chara/Green/guard/guard_stand_001.png");//�����K�[�h
		m_guardSquatHandle = LoadGraph("./img/Chara/Green/guard/guard_squat_002.png");//���Ⴊ�݃K�[�h

		//����
		m_graspHandle = LoadGraph("./img/Chara/Green/throw/grasp_001.png");//����
		m_throwFrontHandle = LoadGraph("./img/Chara/Green/throw/throw_001.png");//�O����
		m_throwBackHandle = LoadGraph("./img/Chara/Green/throw/throw_002.png");//��듊��


		m_damageStandHandle = LoadGraph("./img/Chara/Green/hit/hit_stand_001.png");//��炢
		m_damageAerialHandle = LoadGraph("./img/Chara/Green/hit/hit_aerial_001.png");//��炢
		m_downHandle = LoadGraph("./img/Chara/Green/playerbase/down_001.png");//�_�E��
		m_downAerialHandle = LoadGraph("./img/Chara/Green/playerbase/down_aerial_001.png");//�󒆃_�E��
		m_standUpHandle = LoadGraph("./img/Chara/Green/playerbase/standUp.png");//�N���オ��

		m_commandHandle1 = LoadGraph("./img/Chara/Green/waza/hadou_001.png");//�R�}���h�Z
		m_commandHandle2 = LoadGraph("./img/Chara/Green/waza/shouryuu.png");//�R�}���h�Z
		m_commandHandle3 = LoadGraph("./img/Chara/Green/waza/sukuryuu.png");//�R�}���h�Z
		m_commandHandle4 = LoadGraph("./img/Chara/Green/waza/spairaruaro.png");//�R�}���h�Z
		m_commandHandle5 = LoadGraph("./img/Chara/Green/waza/sonic.png");//�R�}���h�Z
		m_commandHandle6 = LoadGraph("./img/Chara/Green/waza/wave.png");//�R�}���h�Z
		m_commandHandle7 = LoadGraph("./img/Chara/Green/waza/rolling.png");//�R�}���h�Z
		m_commandHandle8 = LoadGraph("./img/Chara/Green/waza/tatumaki.png");//�R�}���h�Z
		m_commandHandle9 = LoadGraph("./img/Chara/Green/waza/wildhant.png");//�R�}���h�Z
		m_winPoseHandle = LoadGraph("./img/Chara/Green/playerbase/win_001.png");//�����|�[�Y
		break;
	default:
		m_idleStandHandel = LoadGraph("./img/Chara/White/playerbase/idle_001.png");//�ҋ@
		m_idleSquatHandel = LoadGraph("./img/Chara/White/playerbase/idle_squat_001.png");//���Ⴊ�ݑҋ@
		m_walkFrontHandel = LoadGraph("./img/Chara/White/playerbase/walk_front_001.png");//�O����
		m_walkBackHandel = LoadGraph("./img/Chara/White/playerbase/walk_back_001.png");//������
		m_jumpHandel = LoadGraph("./img/Chara/White/playerbase/jump_vertical_001.png");//�W�����v
		//����
		m_punchLightHandle = LoadGraph("./img/Chara/White/punch/punch_stand_002.png");//��p���`
		m_punchHighHandle = LoadGraph("./img/Chara/White/punch/punch_stand_001.png");//���p���`
		m_kickLightHandle = LoadGraph("./img/Chara/White/kick/kick_stand_001.png");//��L�b�N
		m_kickHighHandle = LoadGraph("./img/Chara/White/kick/kick_stand_002.png");//���L�b�N
		//���Ⴊ��
		m_punchLightSquatHandle = LoadGraph("./img/Chara/White/punch/punch_squat_001.png");//���Ⴊ�ݎ�p���`
		m_punchHighSquatHandle = LoadGraph("./img/Chara/White/punch/punch_squat_003.png");//���Ⴊ�݋��p���`
		m_kickLightSquatHandle = LoadGraph("./img/Chara/White/kick/kick_squat_001.png");//���Ⴊ�ݎ�L�b�N
		m_kickHighSquatHandle = LoadGraph("./img/Chara/White/kick/kick_squat_003.png");//���Ⴊ�݋��L�b�N
		//��
		m_punchLightAerialHandle = LoadGraph("./img/Chara/White/punch/punch_aerial_001.png");//�󒆎�p���`
		m_punchHighAerialHandle = LoadGraph("./img/Chara/White/punch/punch_aerial_003.png");//�󒆋��p���`
		m_kickLightAerialHandle = LoadGraph("./img/Chara/White/kick/kick_aerial_002.png");//�󒆎�L�b�N
		m_kickHighAerialHandle = LoadGraph("./img/Chara/White/kick/kick_aerial_001.png");//�󒆋��L�b�N

		m_guardStandHandle = LoadGraph("./img/Chara/White/guard/guard_stand_001.png");//�����K�[�h
		m_guardSquatHandle = LoadGraph("./img/Chara/White/guard/guard_squat_002.png");//���Ⴊ�݃K�[�h

		//����
		m_graspHandle = LoadGraph("./img/Chara/White/throw/grasp_001.png");//����
		m_throwFrontHandle = LoadGraph("./img/Chara/White/throw/throw_001.png");//�O����
		m_throwBackHandle = LoadGraph("./img/Chara/White/throw/throw_002.png");//��듊��


		m_damageStandHandle = LoadGraph("./img/Chara/White/hit/hit_stand_001.png");//��炢
		m_damageAerialHandle = LoadGraph("./img/Chara/White/hit/hit_aerial_001.png");//��炢
		m_downHandle = LoadGraph("./img/Chara/White/playerbase/down_001.png");//�_�E��
		m_downAerialHandle = LoadGraph("./img/Chara/White/playerbase/down_aerial_001.png");//�󒆃_�E��
		m_standUpHandle = LoadGraph("./img/Chara/White/playerbase/standUp.png");//�N���オ��

		m_commandHandle1 = LoadGraph("./img/Chara/White/waza/hadou_001.png");//�R�}���h�Z
		m_commandHandle2 = LoadGraph("./img/Chara/White/waza/shouryuu.png");//�R�}���h�Z
		m_commandHandle3 = LoadGraph("./img/Chara/White/waza/sukuryuu.png");//�R�}���h�Z
		m_commandHandle4 = LoadGraph("./img/Chara/White/waza/spairaruaro.png");//�R�}���h�Z
		m_commandHandle5 = LoadGraph("./img/Chara/White/waza/sonic.png");//�R�}���h�Z
		m_commandHandle6 = LoadGraph("./img/Chara/White/waza/wave.png");//�R�}���h�Z
		m_commandHandle7 = LoadGraph("./img/Chara/White/waza/rolling.png");//�R�}���h�Z
		m_commandHandle8 = LoadGraph("./img/Chara/White/waza/tatumaki.png");//�R�}���h�Z
		m_commandHandle9 = LoadGraph("./img/Chara/White/waza/wildhant.png");//�R�}���h�Z
		m_winPoseHandle = LoadGraph("./img/Chara/White/playerbase/win_001.png");//�����|�[�Y}
		break;
	}

}

//�{�C�X
void Chara::VoiceSe(int voiceHandle)
{
	if (m_voice->CheckEndSE())
	{
		m_voice->Stop();
	}
	m_voice->SetSE(voiceHandle);
	m_voice->Volume(kSeVolume);
	m_voice->PlayOnce();
}

bool Chara::CheckMyCommand()
{
	for (int i = 0;i < 3;++i)
	{
		//�����̑I�񂾃R�}���h�̃C���f�b�N�X�ƈ�v����Ȃ�true
		if (m_commandIndex == m_selectCommandIndex[i])
		{
			return true;
		}
	}
	//���s
	m_commandIndex = 0;
	return false;
}


bool Chara::SuccessCommandCheck(Input& input, Player& player)
{
	//���������ɂ���
	m_commandIndex = static_cast<int>(Command::Null);
	if ((player.GetAttackAttackTypes() == AttackTypes::LightPunch) ||
		(player.GetAttackAttackTypes() == AttackTypes::HighPunch))
	{
		if (input.CheckDirCommand("RightOneRevolution") ||
			input.CheckDirCommand("LeftOneRevolution"))
		{
			m_commandIndex = static_cast<int>(Command::Sukuryuu);
			//�������Ă�Ȃ烊�^�[����Ԃ�
			if (CheckMyCommand())
			{
				return true;
			}
		}
		//������
		if ((input.CheckDirCommand("623") && !player.GetDirState()) ||//�E����
			(input.CheckDirCommand("421") && player.GetDirState()) ||//������
			(input.CheckDirCommand("323") && !player.GetDirState()) ||
			(input.CheckDirCommand("121") && player.GetDirState()) ||
			(input.CheckDirCommand("636") && !player.GetDirState()) ||
			(input.CheckDirCommand("414") && player.GetDirState()))
		{
			m_commandIndex = static_cast<int>(Command::Shouryuken);
			if (CheckMyCommand())
			{
				return true;
			}
		}
		//�g����
		if ((input.CheckDirCommand("236") && !player.GetDirState()) ||
			(input.CheckDirCommand("214") && player.GetDirState()))
		{
			//�g����
			m_commandIndex = static_cast<int>(Command::Hadouken);
			if (CheckMyCommand())
			{
				return true;
			}
		}
		if ((input.CheckDirCommand("HalfTurnRightStart") && !player.GetDirState()) ||
			(input.CheckDirCommand("HalfTurnLeftStart") && player.GetDirState()))
		{
			//���C���h�n���g
			m_commandIndex = static_cast<int>(Command::Wildhant);
			if (CheckMyCommand())
			{
				return true;
			}
		}

		
		if ((input.CheckKeepCommand("4K6") && !player.GetDirState()) ||
			(input.CheckKeepCommand("6K4") && player.GetDirState()))
		{
			m_commandIndex = static_cast<int>(Command::Sonic);
			if (CheckMyCommand())
			{
				return true;
			}
		}
	}

	if ((player.GetAttackAttackTypes() == AttackTypes::LightKick) ||
		(player.GetAttackAttackTypes() == AttackTypes::HighKick))
	{
		//����
		if (input.CheckDirCommand("214"))
		{
			if (!player.GetDirState())
			{
				m_commandIndex = static_cast<int>(Command::Tatumaki);
				if (CheckMyCommand())
				{
					return true;
				}
			}
			else
			{
				m_commandIndex = static_cast<int>(Command::Wave);
				if (CheckMyCommand())
				{
					return true;
				}
			}
		}
		if (input.CheckDirCommand("236"))
		{
			if (!player.GetDirState())
			{
				m_commandIndex = static_cast<int>(Command::Wave);
				if (CheckMyCommand())
				{
					return true;
				}
			}
			else
			{
				m_commandIndex = static_cast<int>(Command::Tatumaki);
				if (CheckMyCommand())
				{
					return true;
				}
			}
		}
	
		//��뗭��
		if ((input.CheckKeepCommand("4K6") && !player.GetDirState()) ||
			(input.CheckKeepCommand("6K4") && player.GetDirState()))
		{
			m_commandIndex = static_cast<int>(Command::Supairaru);
			if (CheckMyCommand())
			{
				return true;
			}
		}//���^����v�f(�T�}�[�\���g)
		if (input.CheckKeepCommand("2K8"))
		{
			m_commandIndex = static_cast<int>(Command::Rolling);
			if (CheckMyCommand())
			{
				return true;
			}
		}
	}

	return false;
}


//�R�}���h�Z���[�V�����擾
void Chara::GetAnimCommand(Player& player)
{
	if (m_commandIndex == static_cast<int>(Command::Hadouken))
	{
		GetAnimCommand1(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Shouryuken))
	{
		GetAnimCommand2(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Sukuryuu))
	{
		GetAnimCommand3(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Supairaru))
	{
		GetAnimCommand4(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Sonic))
	{
		GetAnimCommand5(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Wave))
	{
		GetAnimCommand6(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Rolling))
	{
		GetAnimCommand7(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Tatumaki))
	{
		GetAnimCommand8(player);
	}
}
//�R�}���h�Z���[�V�����̓����蔻��(�E����)
void Chara::GetHitBoxCommand(Player& player)
{
	if (m_commandIndex == static_cast<int>(Command::Hadouken))
	{
		GetHitBoxCommand1(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Shouryuken))
	{
		GetHitBoxCommand2(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Sukuryuu))
	{
		GetHitBoxCommand3(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Supairaru))
	{
		GetHitBoxCommand4(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Sonic))
	{
		GetHitBoxCommand5(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Wave))
	{
		GetHitBoxCommand6(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Rolling))
	{
		GetHitBoxCommand7(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Tatumaki))
	{
		GetHitBoxCommand8(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Wildhant))
	{
		GetHitBoxCommand9(player);
	}
}
//�R�}���h�Z������ɂ����炷����
void Chara::GetGiveEffectCommand(Player& player)
{
	if (m_commandIndex == static_cast<int>(Command::Hadouken))
	{
		GetGiveEffectCommand1(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Shouryuken))
	{
		GetGiveEffectCommand2(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Sukuryuu))
	{
		GetGiveEffectCommand3(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Supairaru))
	{
		GetGiveEffectCommand4(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Sonic))
	{
		GetGiveEffectCommand5(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Wave))
	{
		GetGiveEffectCommand6(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Rolling))
	{
		GetGiveEffectCommand7(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Tatumaki))
	{
		GetGiveEffectCommand8(player);
	}
	else if (m_commandIndex == static_cast<int>(Command::Wildhant))
	{
		GetGiveEffectCommand9(player);
	}
}
//�R�}���h�Z�̓��e�i�����j
void Chara::MovementCommand(Player& player, Bullet& bullet, Player& enemy)
{
	//�ړ��ʃ��Z�b�g
	player.SetVeloX(0);
	if (m_commandIndex == static_cast<int>(Command::Hadouken))
	{
		MovementCommand1(player, bullet, enemy);
	}
	else if (m_commandIndex == static_cast<int>(Command::Shouryuken))
	{
		MovementCommand2(player, bullet, enemy);
	}
	else if (m_commandIndex == static_cast<int>(Command::Sukuryuu))
	{
		MovementCommand3(player, bullet, enemy);
	}
	else if (m_commandIndex == static_cast<int>(Command::Supairaru))
	{
		MovementCommand4(player, bullet, enemy);
	}
	else if (m_commandIndex == static_cast<int>(Command::Sonic))
	{
		MovementCommand5(player, bullet, enemy);
	}
	else if (m_commandIndex == static_cast<int>(Command::Wave))
	{
		MovementCommand6(player, bullet, enemy);
	}
	else if (m_commandIndex == static_cast<int>(Command::Rolling))
	{
		MovementCommand7(player, bullet, enemy);
	}
	else if (m_commandIndex == static_cast<int>(Command::Tatumaki))
	{
		MovementCommand8(player, bullet, enemy);
	}
	else if (m_commandIndex == static_cast<int>(Command::Wildhant))
	{
		MovementCommand9(player, bullet, enemy);
	}
}

//�����ҋ@���[�V����
void Chara::GetAnimIdleStand(Player& player)
{
	/// <summary>
	/// �摜�̃f�[�^
	/// </summary>
	m_handle = m_idleStandHandel;//�ҋ@
	/// <summary>
	/// �A�j���[�V�����̖���
	/// </summary>
	m_animNum = 6;
	/// <summary>
	/// �P�̃A�j���[�V�����ɂ�����t���[��
	/// </summary>
	m_oneAnimIntervalFrame = 4;
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
//�ҋ@���[�V�����̓����蔻��(�E����)
void Chara::GetHitBoxIdleStand(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -40;
	headBox.y1 = -170;
	headBox.x2 = 70;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -70;
	bodyBox.y1 = -60;
	bodyBox.x2 = 80;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -80;
	legBox.y1 = 60;
	legBox.x2 = 70;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}

;
//���Ⴊ�ݑҋ@���[�V����
void Chara::GetAnimIdleSquat(Player& player)
{
	m_handle = m_idleSquatHandel;//���Ⴊ�ݑҋ@
	m_animNum = 6;
	m_oneAnimIntervalFrame = 4;
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
void Chara::GetHitBoxIdleSquat(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -60;
	headBox.y1 = -60;
	headBox.x2 = 90;
	headBox.y2 = 50;
	//��	
	bodyBox.x1 = -70;
	bodyBox.y1 = 50;
	bodyBox.x2 = 60;
	bodyBox.y2 = 120;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 120;
	legBox.x2 = 60;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
;
//�O�������[�V����
void Chara::GetAnimWalkFront(Player& player)
{
	m_handle = m_walkFrontHandel;//����
	m_animNum = 8;
	m_oneAnimIntervalFrame = 4;
	player.SetHandle(m_handle);//������
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
void Chara::GetHitBoxWalkFront(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -40;
	headBox.y1 = -170;
	headBox.x2 = 70;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -70;
	bodyBox.y1 = -60;
	bodyBox.x2 = 80;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -80;
	legBox.y1 = 60;
	legBox.x2 = 70;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
;
//���������[�V����
void Chara::GetAnimWalkBack(Player& player)
{
	m_handle = m_walkBackHandel;//����
	m_animNum = 8;
	m_oneAnimIntervalFrame = 4;
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
void Chara::GetHitBoxWalkBack(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -40;
	headBox.y1 = -170;
	headBox.x2 = 70;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -70;
	bodyBox.y1 = -60;
	bodyBox.x2 = 80;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -80;
	legBox.y1 = 60;
	legBox.x2 = 70;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
;
//�W�����v���[�V����
void Chara::GetAnimJump(Player& player)
{
	m_handle = m_jumpHandel;//�W�����v
	m_animNum = 10;
	m_oneAnimIntervalFrame = 4;
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
void Chara::GetHitBoxJump(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -10;
	headBox.y1 = -150;
	headBox.x2 = 60;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 70;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 60;
	legBox.y2 = 130;
	//�������� 
	throwBox.x1 = 0;
	throwBox.y1 = 0;
	throwBox.x2 = 0;
	throwBox.y2 = 0;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
;
//��p���`���[�V����
void Chara::GetAnimPunchLight(Player& player)
{
	m_handle = m_punchLightHandle;//��p���`
	//�S�̃t���[��14
	m_animNum = 7;
	m_oneAnimIntervalFrame = 2;
	m_startAttackFrame = kStartFramePunchLight;//�U������
	m_finishAttackFrame = kFinishFramePunchLight;//�����I��

	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}
//��p���`���[�V�����̓����蔻��(�E����)
void Chara::GetHitBoxPunchLight(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -40;
	headBox.y1 = -170;
	headBox.x2 = 70;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 80;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 100;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 60;
	attackBox.y1 = -50;
	attackBox.x2 = 200;
	attackBox.y2 = 20;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
void Chara::GetGiveEffectPunchLight(Player& player)
{
	//�{�C�X
	VoiceSe(m_attack1VoiceHandle);
	m_giveDamage = 3.0f;
	m_giveNoActFrame = kAllFramePunchLight - kStartFramePunchLight + 4;
	m_giveGuardFrame = kAllFramePunchLight - kStartFramePunchLight - 1;
	m_attackAttributes = AttackAttributes::Upper;//��i
	player.SetGiveDamage(m_giveDamage);
	player.SetGiveNoActFrame(m_giveNoActFrame);
	player.SetGiveGuardFrame(m_giveGuardFrame);
	player.SetAttackAttributes(m_attackAttributes);
	//����𓮂���
	m_giveAttackVelo.x = 5;
	m_giveAttackVelo.y = 0;
	player.SetGiveAttackVelo(m_giveAttackVelo);
}
;
//���p���`���[�V����
void Chara::GetAnimPunchHigh(Player& player)
{
	m_handle = m_punchHighHandle;//���p���`
	//�S�̃t���[��22
	m_animNum = 11;
	m_oneAnimIntervalFrame = 2;
	m_startAttackFrame = kStartFramePunchHigh;//�U������
	m_finishAttackFrame = 16;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}
void Chara::GetHitBoxPunchHigh(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -40;
	headBox.y1 = -170;
	headBox.x2 = 80;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 130;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 130;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 80;
	attackBox.y1 = -40;
	attackBox.x2 = 220;
	attackBox.y2 = 60;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
void Chara::GetGiveEffectPunchHigh(Player& player)
{
	//�{�C�X
	VoiceSe(m_attack2VoiceHandle);
	m_giveDamage = 8.0f;
	m_giveNoActFrame = kAllFramePunchHigh - kStartFramePunchHigh + 4;
	m_giveGuardFrame = kAllFramePunchHigh - kStartFramePunchHigh - 2;
	m_attackAttributes = AttackAttributes::Upper;//��i
	player.SetGiveDamage(m_giveDamage);
	player.SetGiveNoActFrame(m_giveNoActFrame);
	player.SetGiveGuardFrame(m_giveGuardFrame);
	player.SetAttackAttributes(m_attackAttributes);
	//����𓮂���
	m_giveAttackVelo.x = 10;
	m_giveAttackVelo.y = 0;
	player.SetGiveAttackVelo(m_giveAttackVelo);
};


//��L�b�N���[�V����
void Chara::GetAnimKickLight(Player& player)
{
	//�S�̃t���[��18
	m_handle = m_kickLightHandle;//��L�b�N
	m_animNum = 9;
	m_oneAnimIntervalFrame = 2;
	m_startAttackFrame = kStartFrameKickLight;//�U������
	m_finishAttackFrame = 7;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}
void Chara::GetHitBoxKickLight(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -40;
	headBox.y1 = -170;
	headBox.x2 = 70;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 70;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 120;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 40;
	attackBox.y1 = 20;
	attackBox.x2 = 210;
	attackBox.y2 = 80;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
void Chara::GetGiveEffectKickLight(Player& player)
{
	//�{�C�X
	VoiceSe(m_attack1VoiceHandle);
	m_giveDamage = 3.0f;
	m_giveNoActFrame = kAllFrameKickLight - kStartFrameKickLight + 2;
	m_giveGuardFrame = kAllFrameKickLight - kStartFrameKickLight - 4;
	m_attackAttributes = AttackAttributes::Upper;//��i
	player.SetGiveDamage(m_giveDamage);
	player.SetGiveNoActFrame(m_giveNoActFrame);
	player.SetGiveGuardFrame(m_giveGuardFrame);
	player.SetAttackAttributes(m_attackAttributes);
	//����𓮂���
	m_giveAttackVelo.x = 7;
	m_giveAttackVelo.y = 0;
	player.SetGiveAttackVelo(m_giveAttackVelo);
}



;
//���L�b�N���[�V����
void Chara::GetAnimKickHigh(Player& player)
{
	//�S�̃t���[��33
	m_handle = m_kickHighHandle;//���L�b�N
	m_animNum = 11;
	m_oneAnimIntervalFrame = 3;
	m_startAttackFrame = kStartFrameKickHigh;//�U������
	m_finishAttackFrame = 15;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}
void Chara::GetHitBoxKickHigh(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -100;
	headBox.y1 = -150;
	headBox.x2 = 60;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 150;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 170;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 100;
	attackBox.y1 = -120;
	attackBox.x2 = 280;
	attackBox.y2 = -20;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
void Chara::GetGiveEffectKickHigh(Player& player)
{
	//�{�C�X
	VoiceSe(m_attack3VoiceHandle);
	m_giveDamage = 9.0f;
	m_giveNoActFrame = kAllFrameKickHigh - kStartFrameKickHigh + 9;
	m_giveGuardFrame = kAllFrameKickHigh - kStartFrameKickHigh + 1;
	m_attackAttributes = AttackAttributes::Upper;//��i
	player.SetGiveDamage(m_giveDamage);
	player.SetGiveNoActFrame(m_giveNoActFrame);
	player.SetGiveGuardFrame(m_giveGuardFrame);
	player.SetAttackAttributes(m_attackAttributes);
	//����𓮂���
	m_giveAttackVelo.x = 7;
	m_giveAttackVelo.y = 0;
	player.SetGiveAttackVelo(m_giveAttackVelo);
}

//���Ⴊ�ݎ�p���`
void Chara::GetAnimPunchLightSquat(Player& player)
{
	//�S�̃t���[��8
	m_handle = m_punchLightSquatHandle;//���Ⴊ�ݎ�p���`
	m_animNum = 8;
	m_oneAnimIntervalFrame = 1;
	m_startAttackFrame = kStartFramePunchLightSquat;//�U������
	m_finishAttackFrame = 5;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}
void Chara::GetHitBoxPunchLightSquat(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -60;
	headBox.y1 = -60;
	headBox.x2 = 100;
	headBox.y2 = 50;
	//��	
	bodyBox.x1 = -70;
	bodyBox.y1 = 50;
	bodyBox.x2 = 100;
	bodyBox.y2 = 120;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 120;
	legBox.x2 = 60;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 100;
	attackBox.y1 = 40;
	attackBox.x2 = 200;
	attackBox.y2 = 120;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
void Chara::GetGiveEffectPunchLightSquat(Player& player)
{
	//�{�C�X
	VoiceSe(m_attack1VoiceHandle);
	m_giveDamage = 2.0f;
	m_giveNoActFrame = kAllFramePunchLightSquat - kStartFramePunchLightSquat + 4;
	m_giveGuardFrame = kAllFramePunchLightSquat - kStartFramePunchLightSquat - 1;
	m_attackAttributes = AttackAttributes::Upper;//��i
	player.SetGiveDamage(m_giveDamage);
	player.SetGiveNoActFrame(m_giveNoActFrame);
	player.SetGiveGuardFrame(m_giveGuardFrame);
	player.SetAttackAttributes(m_attackAttributes);
	//����𓮂���
	m_giveAttackVelo.x = 10;
	m_giveAttackVelo.y = 0;
	player.SetGiveAttackVelo(m_giveAttackVelo);
}

//���Ⴊ�݋��p���`
void Chara::GetAnimPunchHighSquat(Player& player)
{
	//�S�̃t���[��26
	m_handle = m_punchHighSquatHandle;//���Ⴊ�ݎ�p���`
	m_animNum = 13;
	m_oneAnimIntervalFrame = 2;
	m_startAttackFrame = kStartFramePunchHighSquat;//�U������
	m_finishAttackFrame = 14;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}
void Chara::GetHitBoxPunchHighSquat(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -60;
	headBox.y1 = -60;
	headBox.x2 = 90;
	headBox.y2 = 50;
	//��	
	bodyBox.x1 = -70;
	bodyBox.y1 = 50;
	bodyBox.x2 = 90;
	bodyBox.y2 = 120;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 120;
	legBox.x2 = 90;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 20;
	attackBox.y1 = -240;
	attackBox.x2 = 130;
	attackBox.y2 = 20;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
void Chara::GetGiveEffectPunchHighSquat(Player& player)
{
	//�{�C�X
	VoiceSe(m_attack1VoiceHandle);
	m_giveDamage = 8.0f;
	m_giveNoActFrame = kAllFramePunchHighSquat - kStartFramePunchHighSquat + 1;
	m_giveGuardFrame = kAllFramePunchHighSquat - kStartFramePunchHighSquat - 7;
	m_attackAttributes = AttackAttributes::Upper;//��i
	player.SetGiveDamage(m_giveDamage);
	player.SetGiveNoActFrame(m_giveNoActFrame);
	player.SetGiveGuardFrame(m_giveGuardFrame);
	player.SetAttackAttributes(m_attackAttributes);
	//����𓮂���
	m_giveAttackVelo.x = 10;
	m_giveAttackVelo.y = 10;
	player.SetGiveAttackVelo(m_giveAttackVelo);
}

//���Ⴊ�ݎ�L�b�N
void Chara::GetAnimKickLightSquat(Player& player)
{
	//�S�̃t���[��14
	m_handle = m_kickLightSquatHandle;//���Ⴊ�ݎ�p���`
	m_animNum = 7;
	m_oneAnimIntervalFrame = 2;
	m_startAttackFrame = kStartFrameKickLightSquat;//�U������
	m_finishAttackFrame = 6;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}
void Chara::GetHitBoxKickLightSquat(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -60;
	headBox.y1 = -60;
	headBox.x2 = 90;
	headBox.y2 = 50;
	//��	
	bodyBox.x1 = -70;
	bodyBox.y1 = 50;
	bodyBox.x2 = 60;
	bodyBox.y2 = 120;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 120;
	legBox.x2 = 60;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 60;
	attackBox.y1 = 120;
	attackBox.x2 = 150;
	attackBox.y2 = 180;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
void Chara::GetGiveEffectKickLightSquat(Player& player)
{
	//�{�C�X
	VoiceSe(m_attack1VoiceHandle);
	m_giveDamage = 2.0f;
	m_giveNoActFrame = kAllFrameKickLightSquat - kStartFrameKickLightSquat + 3;
	m_giveGuardFrame = kAllFrameKickLightSquat - kStartFrameKickLightSquat - 3;
	m_attackAttributes = AttackAttributes::Lower;//���i
	player.SetGiveDamage(m_giveDamage);
	player.SetGiveNoActFrame(m_giveNoActFrame);
	player.SetGiveGuardFrame(m_giveGuardFrame);
	player.SetAttackAttributes(m_attackAttributes);
	//����𓮂���
	m_giveAttackVelo.x = 5;
	m_giveAttackVelo.y = 0;
	player.SetGiveAttackVelo(m_giveAttackVelo);
}


//���Ⴊ�݋��L�b�N
void Chara::GetAnimKickHighSquat(Player& player)
{
	//�S�̃t���[��32
	m_handle = m_kickHighSquatHandle;//���Ⴊ�݋��L�b�N
	m_animNum = 8;
	m_oneAnimIntervalFrame = 4;
	m_startAttackFrame = kStartFrameKickHighSquat;//�U������
	m_finishAttackFrame = 11;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}
void Chara::GetHitBoxKickHighSquat(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -60;
	headBox.y1 = -60;
	headBox.x2 = 90;
	headBox.y2 = 50;
	//��	
	bodyBox.x1 = -70;
	bodyBox.y1 = 50;
	bodyBox.x2 = 60;
	bodyBox.y2 = 120;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 120;
	legBox.x2 = 200;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 140;
	attackBox.y1 = 120;
	attackBox.x2 = 250;
	attackBox.y2 = 180;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
void Chara::GetGiveEffectKickHighSquat(Player& player)
{
	//�{�C�X
	VoiceSe(m_attack3VoiceHandle);
	m_giveDamage = 2.0f;
	m_giveNoActFrame = kDown;
	m_giveGuardFrame = kAllFrameKickHighSquat - kStartFrameKickHighSquat - 12;
	m_attackAttributes = AttackAttributes::Lower;//���i
	player.SetGiveDamage(m_giveDamage);
	player.SetGiveNoActFrame(m_giveNoActFrame);
	player.SetGiveGuardFrame(m_giveGuardFrame);
	player.SetAttackAttributes(m_attackAttributes);
	//����𓮂���
	m_giveAttackVelo.x = 2;
	m_giveAttackVelo.y = 0;
	player.SetGiveAttackVelo(m_giveAttackVelo);
}

//�󒆎�p���`
void Chara::GetAnimPunchLightAerial(Player& player)
{
	m_handle = m_punchLightAerialHandle;//�󒆎�p���`
	//�S�̃t���[��16
	m_animNum = 8;
	m_oneAnimIntervalFrame = 1;
	m_startAttackFrame = 4;//�U������
	m_finishAttackFrame = 13;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}
void Chara::GetHitBoxPunchLightAerial(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -10;
	headBox.y1 = -100;
	headBox.x2 = 200;
	headBox.y2 = -10;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 150;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 120;
	legBox.y2 = 130;
	//�������� 
	throwBox.x1 = 0;
	throwBox.y1 = 0;
	throwBox.x2 = 0;
	throwBox.y2 = 0;
	//�U��
	attackBox.x1 = 100;
	attackBox.y1 = 0;
	attackBox.x2 = 200;
	attackBox.y2 = 200;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
void Chara::GetGiveEffectPunchLightAerial(Player& player)
{
	//�{�C�X
	VoiceSe(m_attack1VoiceHandle);
	m_giveDamage = 3.0f;
	m_giveNoActFrame = 13;
	m_giveGuardFrame = 9;
	m_attackAttributes = AttackAttributes::Middle;//���i
	player.SetGiveDamage(m_giveDamage);
	player.SetGiveNoActFrame(m_giveNoActFrame);
	player.SetGiveGuardFrame(m_giveGuardFrame);
	player.SetAttackAttributes(m_attackAttributes);
	//����𓮂���
	m_giveAttackVelo.x = 3;
	m_giveAttackVelo.y = 0;
	player.SetGiveAttackVelo(m_giveAttackVelo);
}
;

//�󒆋��p���`
void Chara::GetAnimPunchHighAerial(Player& player)
{
	m_handle = m_punchHighAerialHandle;//�󒆎�p���`
	//�S�̃t���[��16
	m_animNum = 10;
	m_oneAnimIntervalFrame = 2;
	m_startAttackFrame = 9;//�U������
	m_finishAttackFrame = 14;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}
void Chara::GetHitBoxPunchHighAerial(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -10;
	headBox.y1 = -50;
	headBox.x2 = 200;
	headBox.y2 = 100;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 70;
	bodyBox.y2 = 220;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 60;
	legBox.y2 = 130;
	//�������� 
	throwBox.x1 = 0;
	throwBox.y1 = 0;
	throwBox.x2 = 0;
	throwBox.y2 = 0;
	//�U��
	attackBox.x1 = 50;
	attackBox.y1 = 50;
	attackBox.x2 = 200;
	attackBox.y2 = 250;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
void Chara::GetGiveEffectPunchHighAerial(Player& player)
{
	//�{�C�X
	VoiceSe(m_attack2VoiceHandle);
	m_giveDamage = 8.0f;
	m_giveNoActFrame = 19;
	m_giveGuardFrame = 15;
	m_attackAttributes = AttackAttributes::Middle;//���i
	player.SetGiveDamage(m_giveDamage);
	player.SetGiveNoActFrame(m_giveNoActFrame);
	player.SetGiveGuardFrame(m_giveGuardFrame);
	player.SetAttackAttributes(m_attackAttributes);
	//����𓮂���
	m_giveAttackVelo.x = 5;
	m_giveAttackVelo.y = 0;
	player.SetGiveAttackVelo(m_giveAttackVelo);
}

//�󒆎�L�b�N
void Chara::GetAnimKickLightAerial(Player& player)
{
	m_handle = m_kickLightAerialHandle;//�󒆎�L�b�N
	//�S�̃t���[��16
	m_animNum = 10;
	m_oneAnimIntervalFrame = 2;
	m_startAttackFrame = 5;//�U������
	m_finishAttackFrame = 6;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}

void Chara::GetHitBoxKickLightAerial(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -10;
	headBox.y1 = -150;
	headBox.x2 = 60;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 70;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 150;
	legBox.y2 = 130;
	//�������� 
	throwBox.x1 = 0;
	throwBox.y1 = 0;
	throwBox.x2 = 0;
	throwBox.y2 = 0;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 100;
	attackBox.x2 = 220;
	attackBox.y2 = 200;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}

void Chara::GetGiveEffectKickLightAerial(Player& player)
{
	//�{�C�X
	VoiceSe(m_attack1VoiceHandle);
	m_giveDamage = 2.0f;
	m_giveNoActFrame = 14;
	m_giveGuardFrame = 10;
	m_attackAttributes = AttackAttributes::Middle;//���i
	player.SetGiveDamage(m_giveDamage);
	player.SetGiveNoActFrame(m_giveNoActFrame);
	player.SetGiveGuardFrame(m_giveGuardFrame);
	player.SetAttackAttributes(m_attackAttributes);
	//����𓮂���
	m_giveAttackVelo.x = 3;
	m_giveAttackVelo.y = 0;
	player.SetGiveAttackVelo(m_giveAttackVelo);
}


//�󒆋��L�b�N
void Chara::GetAnimKickHighAerial(Player& player)
{
	m_handle = m_kickHighAerialHandle;//�󒆎�L�b�N
	//�S�̃t���[��16
	m_animNum = 11;
	m_oneAnimIntervalFrame = 2;
	m_startAttackFrame = 12;//�U������
	m_finishAttackFrame = 19;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}

void Chara::GetHitBoxKickHighAerial(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -10;
	headBox.y1 = -150;
	headBox.x2 = 60;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 70;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 170;
	legBox.y2 = 170;
	//�������� 
	throwBox.x1 = 0;
	throwBox.y1 = 0;
	throwBox.x2 = 0;
	throwBox.y2 = 0;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 100;
	attackBox.x2 = 220;
	attackBox.y2 = 220;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);

}

void Chara::GetGiveEffectKickHighAerial(Player& player)
{
	if (player.GetHp() > 0.0f)
	{
		//�{�C�X
		VoiceSe(m_attack3VoiceHandle);
	}
	m_giveDamage = 8.0f;
	m_giveNoActFrame = 19;
	m_giveGuardFrame = 15;
	m_attackAttributes = AttackAttributes::Middle;//���i
	player.SetGiveDamage(m_giveDamage);
	player.SetGiveNoActFrame(m_giveNoActFrame);
	player.SetGiveGuardFrame(m_giveGuardFrame);
	player.SetAttackAttributes(m_attackAttributes);
	//����𓮂���
	m_giveAttackVelo.x = 5;
	m_giveAttackVelo.y = 0;
	player.SetGiveAttackVelo(m_giveAttackVelo);
}

//�󒆍U����̂��ꔻ��(�s���`�ɂ������̂ő傫�߂ɐݒ�)
void Chara::GetHitBoxAttackedAerial(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -10;
	headBox.y1 = -150;
	headBox.x2 = 100;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 170;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -60;
	legBox.y1 = 60;
	legBox.x2 = 170;
	legBox.y2 = 170;
	//�������� 
	throwBox.x1 = -150;
	throwBox.y1 = 0;
	throwBox.x2 = 150;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
}




//�K�[�h���[�V����
void Chara::GetAnimGuardStand(Player& player)
{
	m_handle = m_guardStandHandle;//�K�[�h
	m_animNum = 6;
	m_oneAnimIntervalFrame =1;
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
void Chara::GetHitBoxGuardStand(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -40;
	headBox.y1 = -170;
	headBox.x2 = 70;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -70;
	bodyBox.y1 = -60;
	bodyBox.x2 = 80;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -80;
	legBox.y1 = 60;
	legBox.x2 = 70;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
}
void Chara::GetAnimGuardSquat(Player& player)
{
	m_handle = m_guardSquatHandle;//�K�[�h
	m_animNum = 8;
	m_oneAnimIntervalFrame = 1;
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
void Chara::GetHitBoxGuardSquat(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -60;
	headBox.y1 = -60;
	headBox.x2 = 90;
	headBox.y2 = 50;
	//��	
	bodyBox.x1 = -70;
	bodyBox.y1 = 50;
	bodyBox.x2 = 60;
	bodyBox.y2 = 120;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 120;
	legBox.x2 = 60;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
}
//���݃��[�V����
void Chara::GetAnimGrasp(Player& player)
{
	//�S�̃t���[��30
	m_handle = m_graspHandle;//�K�[�h
	m_animNum = 6;
	m_oneAnimIntervalFrame = 5;
	m_startAttackFrame = 5;//�U������
	m_finishAttackFrame = 7;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}
void Chara::GetHitBoxGrasp(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box graspBox;
	//��	
	headBox.x1 = -10;
	headBox.y1 = -150;
	headBox.x2 = 80;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 90;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 70;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//��������
	graspBox.x1 = 10;
	graspBox.y1 = -100;
	graspBox.x2 = 110;
	graspBox.y2 = 150;

	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxGrasp(graspBox);
}
void Chara::GetGiveEffectGrasp(Player& player)
{
	//�_���[�W
	m_giveDamage = 12.0f;
	player.SetGiveDamage(m_giveDamage);
}
//������ꓖ���蔻��
void Chara::GetHitBoxBeThrown(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	
	//��	
	headBox.x1 = 0;
	headBox.y1 = 0;
	headBox.x2 = 0;
	headBox.y2 = 0;
	//��	
	bodyBox.x1 = 0;
	bodyBox.y1 = 0;
	bodyBox.x2 = 0;
	bodyBox.y2 = 0;
	//��	
	legBox.x1 = 0;
	legBox.y1 = 0;
	legBox.x2 = 0;
	legBox.y2 = 0;
	//�������� 
	throwBox.x1 = 0;
	throwBox.y1 = 0;
	throwBox.x2 = 0;
	throwBox.y2 = 0;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	
}

//�O����
void Chara::GetAnimThrowFront(Player& player)
{
	m_handle = m_throwFrontHandle;
	m_animNum = 12;
	m_oneAnimIntervalFrame = 6;player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
void Chara::GetHitBoxThrowFront(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	//��	50x50
	headBox.x1 = 0;
	headBox.y1 = 0;
	headBox.x2 = 0;
	headBox.y2 = 0;
	//��	70x70
	bodyBox.x1 = 0;
	bodyBox.y1 = 0;
	bodyBox.x2 = 0;
	bodyBox.y2 = 0;
	//��	70x70
	legBox.x1 = 0;
	legBox.y1 = 0;
	legBox.x2 = 0;
	legBox.y2 = 0;
	//�������� 100x110
	throwBox.x1 = 0;
	throwBox.y1 = 0;
	throwBox.x2 = 0;
	throwBox.y2 = 0;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	
}

//��듊��
void Chara::GetAnimThrowBack(Player& player)
{
	m_handle = m_throwBackHandle;
	m_animNum = 9;
	m_oneAnimIntervalFrame = 6;
	player.SetHandle(m_handle);//������
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
void Chara::GetHitBoxThrowBack(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	//��	50x50
	headBox.x1 = 0;
	headBox.y1 = 0;
	headBox.x2 = 0;
	headBox.y2 = 0;
	//��	70x70
	bodyBox.x1 = 0;
	bodyBox.y1 = 0;
	bodyBox.x2 = 0;
	bodyBox.y2 = 0;
	//��	70x70
	legBox.x1 = 0;
	legBox.y1 = 0;
	legBox.x2 = 0;
	legBox.y2 = 0;
	//�������� 100x110
	throwBox.x1 = 0;
	throwBox.y1 = 0;
	throwBox.x2 = 0;
	throwBox.y2 = 0;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
}
;
//��炢(����)���[�V����
//��������
void Chara::GetAnimHitStand(Player& player)
{
	if (player.GetHp() > 0.0f)
	{
		//�{�C�X
		VoiceSe(m_damage1VoiceHandle);
	}
	m_handle = m_damageStandHandle;//����
	m_animNum = 5;
	m_oneAnimIntervalFrame = 5;
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
//�󒆂���
void Chara::GetAnimHitAerial(Player& player)
{
	if (player.GetHp() > 0.0f)
	{
		//�{�C�X
		VoiceSe(m_damage1VoiceHandle);
	}
	m_handle = m_damageAerialHandle;//����
	m_animNum = 7;
	m_oneAnimIntervalFrame = 4;
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
//���ꃂ�[�V�����̓����蔻��(�E����)
void Chara::GetHitBoxHitStand(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	
	headBox.x1 = -10;
	headBox.y1 = -150;
	headBox.x2 = 60;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 70;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 60;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = 0;
	throwBox.y1 = 0;
	throwBox.x2 = 0;
	throwBox.y2 = 0;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;
	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
//�_�E�����[�V����
void Chara::GetAnimDown(Player& player)
{
	m_handle = m_downHandle;
	m_animNum = 13 ;
	m_oneAnimIntervalFrame = 3;
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
void Chara::GetHitBoxDown(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	50x50
	headBox.x1 = 0;
	headBox.y1 = 0;
	headBox.x2 = 0;
	headBox.y2 = 0;
	//��	70x70
	bodyBox.x1 = 0;
	bodyBox.y1 = 0;
	bodyBox.x2 = 0;
	bodyBox.y2 = 0;
	//��	70x70
	legBox.x1 = 0;
	legBox.y1 = 0;
	legBox.x2 = 0;
	legBox.y2 = 0;
	//�������� 100x110
	throwBox.x1 = 0;
	throwBox.y1 = 0;
	throwBox.x2 = 0;
	throwBox.y2 = 0;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;

	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
void Chara::GetAnimDownAerial(Player& player)
{
	if (player.GetHp() > 0.0f)
	{
		//�{�C�X
		VoiceSe(m_damage2VoiceHandle);
	}
	m_handle = m_downAerialHandle;
	m_animNum = 8;
	m_oneAnimIntervalFrame = 3;
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
void Chara::GetHitBoxDownAerial(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	//��	50x50
	headBox.x1 = 0;
	headBox.y1 = 0;
	headBox.x2 = 0;
	headBox.y2 = 0;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 70;
	bodyBox.y2 = 60;
	//��	70x70
	legBox.x1 = 0;
	legBox.y1 = 0;
	legBox.x2 = 0;
	legBox.y2 = 0;
	//�������� 100x110
	throwBox.x1 = 0;
	throwBox.y1 = 0;
	throwBox.x2 = 0;
	throwBox.y2 = 0;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;

	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	
}
//�N���オ��
void Chara::GetAnimStandUp(Player& player)
{
	if (player.GetHp() > 0.0f)
	{
		if (GetRand(1) != 0)
		{
			//�{�C�X
			VoiceSe(m_standUp1VoiceHandle);
		}
		else
		{
			//�{�C�X
			VoiceSe(m_standUp2VoiceHandle);
		}
	}
	m_handle = m_standUpHandle;
	m_animNum = 5;
	m_oneAnimIntervalFrame = 3;
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}
;
//�����|�[�Y
void Chara::GetAnimWinPose(Player& player)
{
	m_handle = m_winPoseHandle;
	m_animNum = 5;
	m_oneAnimIntervalFrame = 5;
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
}


//�R�}���h�Z��SE�̍Đ�
void Chara::CommandSe(int commandIndex)
{
	m_se->SetSE(m_commandSeHandle[commandIndex - 1]);
	m_se->Volume(kSeVolume);
	m_se->PlayOnce();
}

//�g�������[�V����
void Chara::GetAnimCommand1(Player& player)
{
	m_handle = m_commandHandle1;//�g����
	//�S�̃t���[��47
	m_animNum = 7;
	m_oneAnimIntervalFrame = 3;
	//�U������
	if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
	{
		m_oneAnimIntervalFrame = 2;
		//���͔���12�t���[��
		m_startAttackFrame = kStartFrameCommand1High;
	}
	else if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
	{
		//��͔���16�t���[��
		m_startAttackFrame = kStartFrameCommand1Light;
	}
	m_finishAttackFrame = 47;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}
void Chara::GetHitBoxCommand1(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	Box graspBox;
	//��	
	headBox.x1 = -20;
	headBox.y1 = -150;
	headBox.x2 = 190;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 190;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 120;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;
	//����
	graspBox.x1 = 0;
	graspBox.y1 = 0;
	graspBox.x2 = 0;
	graspBox.y2 = 0;

	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	player.SetHitBoxGrasp(graspBox);
}

void Chara::GetGiveEffectCommand1(Player& player)
{
	//��U�Đ����~�߂�
	m_se->Stop();
	return;
}

void Chara::MovementCommand1(Player& player,Bullet& bullet, Player& enemy)
{
	if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
	{
		//���͔���12�t���[��
		if (player.GetAnimCountFrame() == kStartFrameCommand1High)
		{
			//�Đ�
			CommandSe(static_cast<int>(Command::Hadouken));
			//�e�i�g�������o���j
			bullet.LoadHadou(player,
				6.0f,		//�_���[�W
				kAllFrameCommand1 - kStartFrameCommand1High - 3,//�q�b�g���̍d��
				kAllFrameCommand1 - kStartFrameCommand1High - 9);//�K�[�h���Ă���G�ɓ����������̍d��
			bullet.OnIsShooting();
		}
	}
	else if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
	{
		//��͔���16�t���[��
		if (player.GetAnimCountFrame() == kStartFrameCommand1Light)
		{
			//�Đ�
			CommandSe(static_cast<int>(Command::Hadouken));
			//�e�i�g�������o���j
			bullet.LoadHadou(player,6.0f,
				kAllFrameCommand1 - kStartFrameCommand1Light + 1,
				kAllFrameCommand1 - kStartFrameCommand1Light - 5);
			bullet.OnIsShooting();
		}
	}

}
;
//���������[�V����
void Chara::GetAnimCommand2(Player& player)
{
	//����
	if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
	{
		//�S�̃t���[��60
		m_animNum = 4;
		m_oneAnimIntervalFrame = 3;
		m_startAttackFrame = kStartFrameCommand2 + 2;
		m_finishAttackFrame = 14 + 2;
	}
	//���
	else if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
	{
		//�S�̃t���[��47
		m_animNum = 4;
		m_oneAnimIntervalFrame = 2;
		m_startAttackFrame = kStartFrameCommand2;//�U������
		m_finishAttackFrame = 14;//�����I��
	}
	m_handle = m_commandHandle2;//������
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}
void Chara::GetHitBoxCommand2(Player& player)
{	
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	Box graspBox;
	//��	
	headBox.x1 = 0;
	headBox.y1 = 0;
	headBox.x2 = 0;
	headBox.y2 = 0;
	//��	
	bodyBox.x1 = 0;
	bodyBox.y1 = 0;
	bodyBox.x2 = 0;
	bodyBox.y2 = 0;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 20;
	legBox.x2 = 60;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = 0;
	throwBox.y1 = 0;
	throwBox.x2 = 0;
	throwBox.y2 = 0;
	//�U��
	attackBox.x1 = 60;
	attackBox.y1 = -280;
	attackBox.x2 = 200;
	attackBox.y2 = 50;
	//����
	graspBox.x1 = 0;
	graspBox.y1 = 0;
	graspBox.x2 = 0;
	graspBox.y2 = 0;

	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	player.SetHitBoxGrasp(graspBox);
}
void Chara::GetGiveEffectCommand2(Player& player)
{
	//��U�Đ����~�߂�
	m_se->Stop();
	//����
	if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
	{
		//�S�̃t���[��60
		//���G
		player.ResetHitBox();
		m_velocity.y = -46.0f;
		player.SetGiveDamage(12.0f);
		player.SetGiveNoActFrame(kDown);
		player.SetGiveGuardFrame(kAllFrameCommand2 - kStartFrameCommand2 - 39);
		player.SetAttackAttributes(AttackAttributes::Upper);//��i
		m_giveAttackVelo.x = 5;
		m_giveAttackVelo.y = -70;
		player.SetGiveAttackVelo(m_giveAttackVelo);
	}
	//���
	else if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
	{
		//�S�̃t���[��47
		m_velocity.y = -34.0f;
		player.SetGiveDamage(8.0f);
		player.SetGiveNoActFrame(kDown);
		player.SetGiveGuardFrame(kAllFrameCommand2 - kStartFrameCommand2 - 23);
		player.SetAttackAttributes(AttackAttributes::Upper);//��i
		m_giveAttackVelo.x = 5;
		m_giveAttackVelo.y = -50;
		player.SetGiveAttackVelo(m_giveAttackVelo);
	}
}
void Chara::MovementCommand2(Player& player, Bullet& bullet, Player& enemy)
{
	//�Đ�
	CommandSe(static_cast<int>(Command::Shouryuken));
	//���񂾂�㏸���Ă���
	m_velocity.y -= 0.2f;
	//�������̓���
	m_velocity.x = 10;

	if (player.GetDirState())
	{
		m_velocity.x *= -1;	
	}
	
	//�U�������I��
	if (player.GetAnimCountFrame() > m_finishAttackFrame)
	{
		m_velocity.x = 0;
		m_velocity.y += 0.5f;
	}
	//�U�����肪�o�Ă���o��n�߂�
	if (player.GetAnimCountFrame() > (m_startAttackFrame + 2))
	{
		player.SetVelo(m_velocity);
	}
}

void Chara::GetAnimCommand3(Player& player)
{
	if (player.IsThrowSuccess())
	{
		m_handle = m_commandHandle3;//�X�N�����[
		//�S�̃t���[��120
		m_animNum = 8;
		m_oneAnimIntervalFrame = 15;
		player.SetHandle(m_handle);
		player.SetAnimNum(m_animNum);
		player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	}
	else
	{
		//�����̃A�j���[�V����
		//�����̂��݃��[�V������30�t���[��(6��*5����)�Ȃ̂�
		//2�{�ɂ���60�t���[���ɂ��Ƃ�
		GetAnimGrasp(player);
		player.SetOneAnimFrame(10);
	}
}

void Chara::GetHitBoxCommand3(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	Box graspBox;
	//��	
	headBox.x1 = -10;
	headBox.y1 = -150;
	headBox.x2 = 80;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 90;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 70;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U������
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;
	//���
	//����
	graspBox.x1 = 0;
	graspBox.y1 = 0;
	graspBox.x2 = 170;
	graspBox.y2 = 100;

	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	player.SetHitBoxGrasp(graspBox);
}

void Chara::GetGiveEffectCommand3(Player& player)
{
	//��U�Đ����~�߂�
	m_se->Stop();
	m_velocity.x = 0.0f;
	m_velocity.y = -25.0f;
	//����
	if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
	{
		//����
		//����
		Box graspBox;
		graspBox.x1 = 0;
		graspBox.y1 = 0;
		graspBox.x2 = 130;
		graspBox.y2 = 100;
		m_velocity.y = -30.0f;
		player.SetHitBoxGrasp(graspBox);
		player.SetGiveDamage(33.0f);
		m_giveAttackVelo.x = 10;
		m_giveAttackVelo.y = -60;
	}
	//���
	else if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
	{
		player.SetGiveDamage(25.0f);
		m_giveAttackVelo.x = 20;
		m_giveAttackVelo.y = -40;
	}
}

void Chara::MovementCommand3(Player& player, Bullet& bullet, Player& enemy)
{
	GetAnimCommand3(player);
	//����������������
	if (player.IsThrowSuccess())
	{
		//�������[�V����
		GetAnimCommand3(player);
		//�������[�V�������͖��G
		player.ResetHitBox();

		//�㏸
		if (player.GetAnimCountFrame() < 30)
		{
			player.SetVelo(m_velocity);
			enemy.SetPos(player.GetPos());
		}
		else
		{
			//���������v���C���[�Ɠ������W
			if (!player.IsGround())
			{
				enemy.SetPos(player.GetPos());
			}
			else	//�n�ʂɂ�����
			{
				//�Đ�
				CommandSe(static_cast<int>(Command::Sukuryuu));
				//������
				//������ӂ��Ƃ΂�����
				Vector3 throwVelo;
				throwVelo.x = m_giveAttackVelo.x;
				throwVelo.y = m_giveAttackVelo.y;
				if (player.GetDirState())//�������������Ȃ獶�ɔ�΂�
				{
					throwVelo.x *= -1;
				}

				//�Z�b�g
				enemy.SetThrowVelo(throwVelo);
				// ��������Ԃɂ���
				enemy.OnIsThrown();

				//�����̌㑊��Ƃ̋����𗣂�
				//�������������Ȃ�E�ɓ���
				//�������E�����Ȃ獶�ɓ���
				m_velocity.y = 0;
				m_velocity.x = 5.0f;
				if (!player.GetDirState())
				{
					m_velocity.x *= -1;
				}
				player.SetVelo(m_velocity);
				//���Z�b�g
				m_velocity.x = 0;
			}
		}
	}
}

//�X�p�C�����A���[
void Chara::GetAnimCommand4(Player& player)
{
	//����
	if (player.GetAttackAttackTypes() == AttackTypes::HighKick)
	{
		//�S�̃t���[��49
		m_animNum = 7;
		m_oneAnimIntervalFrame = 7;
		m_startAttackFrame = kStartFrameCommand4High;//�U������
		m_finishAttackFrame = 30;//�����I��
	}
	//���
	else if (player.GetAttackAttackTypes() == AttackTypes::LightKick)
	{
		//�S�̃t���[��42
		m_animNum = 7;
		m_oneAnimIntervalFrame = 6;
		m_startAttackFrame = kStartFrameCommand4Light;//�U������
		m_finishAttackFrame = 21;//�����I��
	}
	m_handle = m_commandHandle4;//�X�p�C�����A���[
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}

void Chara::GetHitBoxCommand4(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	Box graspBox;
	//��	
	headBox.x1 = -200;
	headBox.y1 = 0;
	headBox.x2 = 80;
	headBox.y2 = 100;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = 0;
	bodyBox.x2 = 90;
	bodyBox.y2 = 100;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 0;
	legBox.x2 = 200;
	legBox.y2 = 200;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U������
	attackBox.x1 = 0;
	attackBox.y1 = -20;
	attackBox.x2 = 250;
	attackBox.y2 = 100;

	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
}

void Chara::GetGiveEffectCommand4(Player& player)
{	
	//��U�Đ����~�߂�
	m_se->Stop();
	//�ړ���
	m_velocity.y = 0.0f;
	//����
	if (player.GetAttackAttackTypes() == AttackTypes::HighKick)
	{
		m_velocity.x = kCommand4HighVeloX;
		if (player.GetDirState())
		{
			m_velocity.x *= -1;
		}
		player.SetGiveDamage(8.0f);
		player.SetGiveNoActFrame(kDown);
		player.SetGiveGuardFrame(kAllFrameCommand4High - kStartFrameCommand4High - 20);
		player.SetAttackAttributes(AttackAttributes::Upper);//��i
		m_giveAttackVelo.x = 2.0f;
		m_giveAttackVelo.y = -60.0f;
		player.SetGiveAttackVelo(m_giveAttackVelo);
	}
	//���
	else if (player.GetAttackAttackTypes() == AttackTypes::LightKick)
	{
		m_velocity.x = kCommand4LightVeloX;
		if (player.GetDirState())
		{
			m_velocity.x *= -1;
		}
		player.SetGiveDamage(8.0f);
		player.SetGiveNoActFrame(kDown);
		player.SetGiveGuardFrame(kAllFrameCommand4Light - kStartFrameCommand4Light - 20);
		player.SetAttackAttributes(AttackAttributes::Upper);//��i
		m_giveAttackVelo.x = 20.0f;
		m_giveAttackVelo.y = -20.0f;
		player.SetGiveAttackVelo(m_giveAttackVelo);
	}

}

void Chara::MovementCommand4(Player& player, Bullet& bullet, Player& enemy)
{
	//�Đ�
	CommandSe(static_cast<int>(Command::Supairaru));
	//�U�����肪�o�鏭���O���瓮���n�߂�
	if (player.GetAnimCountFrame() > (m_startAttackFrame * 0.5f))
	{
		player.SetVelo(m_velocity);
		if (player.GetDirState())
		{
			m_velocity.x+= kDeceleration;
			if (m_velocity.x > 0.0f)
			{
				m_velocity.x = -1;
			}
		}
		else
		{
			m_velocity.x-= kDeceleration;
			if (m_velocity.x < 0.0f)
			{
				m_velocity.x = 1;
			}
		}
	}
}

void Chara::GetAnimCommand5(Player& player)
{
	m_handle = m_commandHandle5;//�\�j�b�N�u�[��
	//�S�̃t���[��40
	m_animNum = 7;
	m_oneAnimIntervalFrame = 6;
	//�U������
	if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
	{
		m_oneAnimIntervalFrame = 5;
		//���͔���12�t���[��
		m_startAttackFrame = 10;
	}
	else if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
	{
		//��͔���16�t���[��
		m_startAttackFrame = 10;
	}
	m_finishAttackFrame = 40;//�����I��
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}

void Chara::GetHitBoxCommand5(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	Box graspBox;
	//��	
	headBox.x1 = -20;
	headBox.y1 = -150;
	headBox.x2 = 190;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 190;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 120;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;
	//����
	graspBox.x1 = 0;
	graspBox.y1 = 0;
	graspBox.x2 = 0;
	graspBox.y2 = 0;

	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	player.SetHitBoxGrasp(graspBox);
}

void Chara::GetGiveEffectCommand5(Player& player)
{
	//��U�Đ����~�߂�
	m_se->Stop();
	return;
}

void Chara::MovementCommand5(Player& player, Bullet& bullet, Player& enemy)
{
	if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
	{
		//���͔���12�t���[��
		if (player.GetAnimCountFrame() == kStartFrameCommand1High)
		{
			//�Đ�
			CommandSe(static_cast<int>(Command::Sonic));
			//�e�i�g�������o���j
			bullet.LoadSonic(player,
				5.5f,		//�_���[�W
				kAllFrameCommand1 - kStartFrameCommand5 - 2,//�q�b�g���̍d��
				kAllFrameCommand1 - kStartFrameCommand5 - 10);//�K�[�h���Ă���G�ɓ����������̍d��
			bullet.OnIsShooting();
		}
	}
	else if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
	{
		//��͔���16�t���[��
		if (player.GetAnimCountFrame() == kStartFrameCommand1Light)
		{
			//�Đ�
			CommandSe(static_cast<int>(Command::Sonic));
			//�e�i�g�������o���j
			bullet.LoadSonic(player,
				5.5f,
				kAllFrameCommand1 - kStartFrameCommand5 - 2,
				kAllFrameCommand1 - kStartFrameCommand5 - 10);
			bullet.OnIsShooting();
		}
	}
}

void Chara::GetAnimCommand6(Player& player)
{
	m_handle = m_commandHandle6;
	//�S�̃t���[��45
	m_animNum = 10;
	m_oneAnimIntervalFrame = 4;
	//�U������
	if (player.GetAttackAttackTypes() == AttackTypes::HighKick)
	{
		m_oneAnimIntervalFrame = 3;
		//���͔���16�t���[��
		m_startAttackFrame = kStartFrameCommand6High;
		m_finishAttackFrame = kAllFrameCommand6High;//�����I�� 52F
	}
	else if (player.GetAttackAttackTypes() == AttackTypes::LightKick)
	{
		//��͔���16�t���[��
		m_startAttackFrame = kStartFrameCommand6Light;
		m_finishAttackFrame = kAllFrameCommand6Light;//�����I�� 46F
	}
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}

void Chara::GetHitBoxCommand6(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	Box graspBox;
	//��	
	headBox.x1 = -20;
	headBox.y1 = -80;
	headBox.x2 = 100;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 120;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 120;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;
	//����
	graspBox.x1 = 0;
	graspBox.y1 = 0;
	graspBox.x2 = 0;
	graspBox.y2 = 0;

	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	player.SetHitBoxGrasp(graspBox);
}

void Chara::GetGiveEffectCommand6(Player& player)
{
	//��U�Đ����~�߂�
	m_se->Stop();
	return;
}

void Chara::MovementCommand6(Player& player, Bullet& bullet, Player& enemy)
{
	if (player.GetAttackAttackTypes() == AttackTypes::HighKick)
	{
		//���͔���12�t���[��
		if (player.GetAnimCountFrame() == kStartFrameCommand1High)
		{
			//�Đ�
			CommandSe(static_cast<int>(Command::Wave));
			//�e�i�g�������o���j
			bullet.LoadWave(player,
				4.0f,		//�_���[�W
				kAllFrameCommand1 - kStartFrameCommand6High + 3,//�q�b�g���̍d��
				kAllFrameCommand1 - kStartFrameCommand6High - 3);//�K�[�h���Ă���G�ɓ����������̍d��);
			bullet.OnIsShooting();
		}
	}
	else if (player.GetAttackAttackTypes() == AttackTypes::LightKick)
	{
		//��͔���16�t���[��
		if (player.GetAnimCountFrame() == kStartFrameCommand1Light)
		{
			//�Đ�
			CommandSe(static_cast<int>(Command::Wave));
			//�e�i�g�������o���j
			bullet.LoadWave(player,
				4.0f,
				kAllFrameCommand1 - kStartFrameCommand6Light + 3,
				kAllFrameCommand1 - kStartFrameCommand6Light - 3);
			bullet.OnIsShooting();
		}
	}
}

void Chara::GetAnimCommand7(Player& player)
{
	//�S�̃t���[��48
	m_animNum = 6;
	m_oneAnimIntervalFrame = 4;
	m_startAttackFrame = kStartFrameCommand7;//�U������
	m_finishAttackFrame = 27;//�����I��
	m_handle = m_commandHandle7;//�o�[�`�J�����[�����O
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}

void Chara::GetHitBoxCommand7(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	Box graspBox;
	//��	
	headBox.x1 = 0;
	headBox.y1 = 0;
	headBox.x2 = 0;
	headBox.y2 = 0;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = 30;
	bodyBox.x2 = 70;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 60;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = 0;
	throwBox.y1 = 0;
	throwBox.x2 = 0;
	throwBox.y2 = 0;
	//�U��
	attackBox.x1 = -50;
	attackBox.y1 = -120;
	attackBox.x2 = 150;
	attackBox.y2 = 100;
	//����
	graspBox.x1 = 0;
	graspBox.y1 = 0;
	graspBox.x2 = 0;
	graspBox.y2 = 0;

	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	player.SetHitBoxGrasp(graspBox);
}

void Chara::GetGiveEffectCommand7(Player& player)
{
	//��U�Đ����~�߂�
	m_se->Stop();
	//����
	if (player.GetAttackAttackTypes() == AttackTypes::HighKick)
	{
		//�S�̃t���[��57
		//���G
		player.ResetHitBox();
		m_velocity.y = -20.0f;
		m_velocity.x = 0.0f;
		player.SetGiveDamage(12.0f);
		player.SetGiveNoActFrame(kDown);
		player.SetGiveGuardFrame(kAllFrameCommand7 - kStartFrameCommand7 - 27);
		player.SetAttackAttributes(AttackAttributes::Upper);//��i
		m_giveAttackVelo.x = 5;
		m_giveAttackVelo.y = -40;
		player.SetGiveAttackVelo(m_giveAttackVelo);
	}
	//���
	else if (player.GetAttackAttackTypes() == AttackTypes::LightKick)
	{
		//�S�̃t���[��48
		m_velocity.y = -10.0f;
		m_velocity.x = 20.0f;
		if (player.GetDirState())
		{
			m_velocity.x *= -1;
		}
		player.SetGiveDamage(8.0f);
		player.SetGiveNoActFrame(kDown);
		player.SetGiveGuardFrame(kAllFrameCommand7 - kStartFrameCommand7 - 27);
		player.SetAttackAttributes(AttackAttributes::Upper);//��i
		m_giveAttackVelo.x = 5;
		m_giveAttackVelo.y = -30;
		player.SetGiveAttackVelo(m_giveAttackVelo);
	}
}

void Chara::MovementCommand7(Player& player, Bullet& bullet, Player& enemy)
{
	//�Đ�
	CommandSe(static_cast<int>(Command::Rolling));
	////�U�������I���܂ŏ�葱����
	if (player.GetAnimCountFrame() > m_finishAttackFrame)
	{
		m_velocity.x = 0;
	}
	//�U�����肪�o�Ă���o��n�߂�
	if (player.GetAnimCountFrame() > (m_startAttackFrame * 0.5f))
	{
		player.SetVelo(m_velocity);
	}
}

void Chara::GetAnimCommand8(Player& player)
{
	m_animNum = 7;
	m_oneAnimIntervalFrame = 5;
	m_startAttackFrame = 4;//�U������
	m_finishAttackFrame = 5;//�����I��
	m_handle = m_commandHandle8;//���������r
	player.SetHandle(m_handle);
	player.SetAnimNum(m_animNum);
	player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	player.SetStartAttackFrame(m_startAttackFrame);//�U������
	player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
}

void Chara::GetHitBoxCommand8(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	Box graspBox;
	//��	
	headBox.x1 = -10;
	headBox.y1 = -150;
	headBox.x2 = 120;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 180;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 180;
	legBox.y2 = 180;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U��
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 200;
	attackBox.y2 = 100;
	//����
	graspBox.x1 = 0;
	graspBox.y1 = 0;
	graspBox.x2 = 0;
	graspBox.y2 = 0;

	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	player.SetHitBoxGrasp(graspBox);
}

void Chara::GetGiveEffectCommand8(Player& player)
{
	//��U�Đ����~�߂�
	m_se->Stop();
	m_velocity.x = 0.0f;
	m_velocity.y = 0.0f;
	//����
	if (player.GetAttackAttackTypes() == AttackTypes::HighKick)
	{
		//�S�̃t���[��61
		m_allFrame = kAllFrameCommand8High;
		m_velocity.x = 15.0f;
		m_velocity.y = -5;
		//���ł͎�������
		m_startAttackFrame = 14;//�U������
		m_finishAttackFrame = 15;//�����I��
		player.SetStartAttackFrame(m_startAttackFrame);//�U������
		player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��

		player.SetGiveDamage(5.0f);
		player.SetGiveNoActFrame(m_allFrame - 27);
		player.SetGiveGuardFrame(m_allFrame - 12);
		player.SetAttackAttributes(AttackAttributes::Upper);//��i
		m_giveAttackVelo.x = 2;
		m_giveAttackVelo.y = -20;
		player.SetGiveAttackVelo(m_giveAttackVelo);
	}
	//���
	else if (player.GetAttackAttackTypes() == AttackTypes::LightKick)
	{
		//�S�̃t���[��48
		m_allFrame = kAllFrameCommand8Light;
		m_velocity.x = 10.0f;
		player.SetGiveDamage(2.5f);
		player.SetGiveNoActFrame(m_allFrame - 27);
		player.SetGiveGuardFrame(m_allFrame - 14);
		player.SetAttackAttributes(AttackAttributes::Upper);//��i
		m_giveAttackVelo.x = 2;
		m_giveAttackVelo.y = -30;
		player.SetGiveAttackVelo(m_giveAttackVelo);
	}
	if (player.GetDirState())
	{
		m_velocity.x *= -1;
	}
}

void Chara::MovementCommand8(Player& player, Bullet& bullet, Player& enemy)
{
	//�Đ�
	CommandSe(static_cast<int>(Command::Tatumaki));
	player.SetVelo(m_velocity);
	if (player.GetAnimCountFrame() < m_allFrame)
	{
		//�����I���^�C�~���O
		if (player.GetAnimCountFrame() == m_finishAttackFrame)
		{
			if (player.GetAnimCountFrame() < (m_allFrame * 0.5f))
			{
				m_startAttackFrame += 10;
				m_finishAttackFrame += 10;
				player.SetStartAttackFrame(m_startAttackFrame);//�U������
				player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
				//�U��
				GetHitBoxCommand8(player);
				player.SetGiveNoActFrame(kDown);
				player.SetGiveGuardFrame(m_allFrame - m_startAttackFrame - 14);
			}
		}
		if (player.GetAnimIndex() >= (m_animNum - 1))
		{
			player.SetAnimIndex(0);
		}
	}
	else
	{
		player.SetAnimIndex(m_animNum);
	}
}

void Chara::GetAnimCommand9(Player& player)
{
	if (player.IsThrowSuccess())
	{
		m_handle = m_commandHandle3;//�X�N�����[
		//�S�̃t���[��40
		m_animNum = 8;
		m_oneAnimIntervalFrame = 5;
		player.SetHandle(m_handle);
		player.SetAnimNum(m_animNum);
		player.SetOneAnimFrame(m_oneAnimIntervalFrame);
	}
	else
	{
		m_handle = m_commandHandle9;//�Ƃт�����
		m_animNum = 8;
		
		if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
		{
			m_oneAnimIntervalFrame = 7;
			m_startAttackFrame = 43;//�U������
			m_finishAttackFrame = 45;//�����I��
		}
		else if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
		{	
			m_oneAnimIntervalFrame = 6;
			m_startAttackFrame = 34;//�U������
			m_finishAttackFrame = 36;//�����I��
		}
		player.SetHandle(m_handle);//������
		player.SetAnimNum(m_animNum);
		player.SetOneAnimFrame(m_oneAnimIntervalFrame);
		player.SetStartAttackFrame(m_startAttackFrame);//�U������
		player.SetFinishAttackFrame(m_finishAttackFrame);//�����I��
	}
}

void Chara::GetHitBoxCommand9(Player& player)
{
	Box headBox;
	Box bodyBox;
	Box legBox;
	Box throwBox;
	Box attackBox;
	Box graspBox;
	//��	
	headBox.x1 = -10;
	headBox.y1 = -150;
	headBox.x2 = 150;
	headBox.y2 = -60;
	//��	
	bodyBox.x1 = -60;
	bodyBox.y1 = -60;
	bodyBox.x2 = 150;
	bodyBox.y2 = 60;
	//��	
	legBox.x1 = -70;
	legBox.y1 = 60;
	legBox.x2 = 70;
	legBox.y2 = 80;
	//�������� 
	throwBox.x1 = -100;
	throwBox.y1 = 0;
	throwBox.x2 = 100;
	throwBox.y2 = 200;
	//�U������
	attackBox.x1 = 0;
	attackBox.y1 = 0;
	attackBox.x2 = 0;
	attackBox.y2 = 0;

	//����
	graspBox.x1 = 0;
	graspBox.y1 = 0;
	graspBox.x2 = 130;
	graspBox.y2 = 100;

	//�����蔻��Z�b�g
	player.SetHitBoxHead(headBox);
	player.SetHitBoxBody(bodyBox);
	player.SetHitBoxLeg(legBox);
	player.SetHitBoxThrow(throwBox);
	player.SetHitBoxAttack(attackBox);
	player.SetHitBoxGrasp(graspBox);
}

void Chara::GetGiveEffectCommand9(Player& player)
{
	//��U�Đ����~�߂�
	m_se->Stop();
	//����
	if (player.GetAttackAttackTypes() == AttackTypes::HighPunch)
	{
		m_velocity.x = 15.0f;
		m_velocity.y = -55.0f;
		player.SetGiveDamage(18.0f);
	}
	//���
	else if (player.GetAttackAttackTypes() == AttackTypes::LightPunch)
	{
		m_velocity.x = 5.0f;
		m_velocity.y = -50.0f;
		player.SetGiveDamage(16.0f);
	}
	if (player.GetDirState())
	{
		m_velocity.x *= -1;
	}
	player.SetVelo(m_velocity);
}

void Chara::MovementCommand9(Player& player, Bullet& bullet, Player& enemy)
{
	GetAnimCommand9(player);
	if (player.GetAnimCountFrame() > 0)
	{
		if (player.IsGround())
		{
			m_velocity.x = 0;
			m_velocity.y = 0;
		}
	}
	//����������������
	if (player.IsThrowSuccess())
	{
		m_velocity.x = 0;
		m_velocity.y = 0;
		if (player.GetAnimIndex() > 3)
		{
			//�Đ�
			CommandSe(static_cast<int>(Command::Wildhant));
			//������
			//������ӂ��Ƃ΂�����
			Vector3 throwVelo;
			throwVelo.x = 20;

			if (player.GetDirState())//�������������Ȃ獶�ɔ�΂�
			{
				throwVelo.x *= -1;
			}

			throwVelo.y = -30;

			//�Z�b�g
			enemy.SetThrowVelo(throwVelo);
			// ��������Ԃɂ���
			enemy.OnIsThrown();

			//�����̌㑊��Ƃ̋����𗣂�
			//�������������Ȃ�E�ɓ���
			//�������E�����Ȃ獶�ɓ���
			m_velocity.y = 0;
			m_velocity.x = 5.0f;
			if (!player.GetDirState())
			{
				m_velocity.x *= -1;
			}
			player.SetVelo(m_velocity);
			//���Z�b�g
			m_velocity.x = 0;
		}
		else
		{
			enemy.SetPos(player.GetPos());
		}
	}
	player.SetVeloX(m_velocity.x);
}

