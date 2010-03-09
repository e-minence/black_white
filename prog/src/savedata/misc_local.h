
#include "pm_define.h"

///����Ⴂ�F���ȏЉ�b�Z�[�W��
#define SAVE_SURETIGAI_SELFINTRODUCTION_LEN   (8 + 1)  //EOM����
///����Ⴂ�F���烁�b�Z�[�W��
#define SAVE_SURETIGAI_THANKYOU_LEN           (8 + 1)  //EOM����

#define GYM_MAX     (8)


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�����ރZ�[�u�f�[�^
//	�E�ǂ��ɕ��ނ���ׂ������f�ɖ������́B
//	�E�K�͂����������̂��߂Ƀu���b�N�m�ۂ�����̂����������Ȃ�����
//=====================================
struct _MISC
{	
  u32 gold;         //����
  u32 badge;     //�o�b�`BIT

  u16 favorite_monsno;		//���C�ɓ���|�P����
	u8  favorite_form_no:7;		//���C�ɓ���|�P�����̃t�H�����ԍ�
	u8  favorite_egg_flag:1;	//���C�ɓ���|�P�����̃^�}�S�t���O
	u8	namein_mode[NAMEIN_SAVEMODE_MAX];	//8��

	//�p���p�[�N
	u32 palpark_highscore:28;
	u32 palpark_finish_state:4;

	//����Ⴂ
  u32 thanks_recv_count;         ///<������󂯂���
  u32 suretigai_count;           ///<����Ⴂ�l��
  STRCODE self_introduction[SAVE_SURETIGAI_SELFINTRODUCTION_LEN]; ///<���ȏЉ�
  STRCODE thankyou_message[SAVE_SURETIGAI_THANKYOU_LEN];  ///<���烁�b�Z�[�W
  u8 research_team_rank;         ///<���������������N(RESEARCH_TEAM_RANK_xxx)

	//�^�C�g�����j���[
	u8	start_menu_open;		///<���j���[�\��

  // ����Ⴂ������
  u8  research_request_id;  // �󂯂Ă��钲���˗�ID
  u16 research_start_count; // �����˗����󂯂����̉񓚐l��
  s64 research_start_time;  // �����˗����󂯂����̎���[�b]

  u8 padding[3];

  //�W�����[�_�[�폟�����̎莝���L�^
  u16 gym_win_monsno[GYM_MAX][TEMOTI_POKEMAX];
};

