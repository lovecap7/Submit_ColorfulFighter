#pragma once
//�X�}�[�g�|�C���^���g��
#include<memory>
#include "game.h"
class Input;
class CharaBase;
class SoundManager;
class SceneBase;

/// <summary>
/// �e�V�[�����Ǘ�����N���X
/// �Ǘ��͂��邪�A�@�ۂ��Ǘ��͂��Ȃ�(�@�ۂ̓N���X�ǂ������)
/// �C���^�[�t�F�C�X��񋟂���N���X
/// </summary>
class SceneController
{
private:
	//���ݎ��s���̃V�[��(���������͒m��Ȃ�)
	std::shared_ptr<SceneBase> m_scene;
	//�I�񂾃R�}���h�Z�̃C���f�b�N�X��ۑ�
	int m_selectCommandIndexP1[3];
	int m_selectCommandIndexP2[3];
	//�I�񂾃L�����N�^�[�̃J���[�����O�̕ۑ�
	CharaColorIndex m_charaColorIndexP1;
	CharaColorIndex m_charaColorIndexP2;
	//�������v���C���[�̏��
	PlayerIndex m_winPlayerIndex;
public:
	SceneController();
	/// <summary>
	/// Application����Ă΂��֐��B���݂̃V�[����Update�����̂܂܎��s�B
	/// </summary>
	/// <param name="input"></param>
	void Update(Input& input, Input& input2);
	/// <summary>
	/// Application����Ă΂��֐��B���݂̃V�[����Draw�����̂܂܎��s�B
	/// </summary>
	/// <param name="input"></param>
	void Draw();
	/// <summary>
	/// ���̏�Ԃ��Z�b�g����(�Z�b�g����̂͊e��Ԃ̖���)
	/// �e�V�[�����R���g���[���[�����Đ؂�ւ�������
	/// </summary>
	/// <param name="">���̏�Ԃ̃|�C���^</param>
	void ChangeScene(std::shared_ptr<SceneBase>);
	//���̃V�[���Ɉ���������������ۑ���������o�����肷��֐��B
	//�I�񂾋Z�̏��
	void SaveSelectCommandIndex(int indexP1[3], int indexP2[3]);//�ۑ�
	int* GetSelectCommandIndex(PlayerIndex playerIndex);
	//�I�񂾐F
	void SaveCharaColorIndexP1(CharaColorIndex colorIndex);
	void SaveCharaColorIndexP2(CharaColorIndex colorIndex);
	CharaColorIndex GetCharaColorIndexP1() { return m_charaColorIndexP1; }
	CharaColorIndex GetCharaColorIndexP2() { return m_charaColorIndexP2; }
	//�������v���C���[�̃C���f�b�N�X
	void SaveWinPlayerIndex(PlayerIndex winPlayerIndex) { m_winPlayerIndex = winPlayerIndex; }
	PlayerIndex GetWinPlayerIndex() { return m_winPlayerIndex; }
};

