//======================================================================
/**
 * @file	field_comm_def.h
 * @brief	�t�B�[���h�ʐM�p��`�Q
 * @author	ariizumi
 * @data	08/11/27
 */
//======================================================================

#ifndef FIELD_COMM_DEF_H__
#define FIELD_COMM_DEF_H__

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#define FIELD_COMM_MEMBER_MAX (4)
#define FCM_BGPLANE_YESNO_WINDOW (GFL_BG_FRAME1_M)	//�g�p�t���[��(DEBUG_BGFRAME_MENU 
#define FCM_BGPLANE_MSG_WINDOW (GFL_BG_FRAME2_M)	//�g�p�t���[��(DEBUG_BGFRAME_MENU 

//���L�����̃f�[�^�p�C���f�b�N�X
#define FCD_SELF_INDEX (0xFF)
//======================================================================
//	enum
//======================================================================
//�L�����̏��
typedef enum
{
	FCCS_NONE,		//���݂Ȃ�
	FCCS_CONNECT,	//�ڑ�����
	FCCS_REQ_DATA,	//�L�����ʃf�[�^��M��
	FCCS_EXIST_DATA,	//�L�����ʃf�[�^��M��
	FCCS_FIELD,		//�t�B�[���h��(��b���܂�
}F_COMM_CHARA_STATE;

//�L�����̉�b�Ǘ��p
typedef enum
{
	FCTS_NONE,
	FCTS_REPLY_TALK,	//��b�̕Ԏ�
	FCTS_WAIT_TALK,		//��b����̒�~�҂�
	FCTS_UNPOSSIBLE,	//���̕Ԏ�(����
	FCTS_ACCEPT,		//���̕Ԏ�(OK
	FCTS_RESERVE_TALK,	//��b�\��
	FCTS_TALKING,		//��b��
	FCTS_WAIT_END,		//��b�I���҂�
}F_COMM_TALK_STATE;

//�t�B�[���h��b�łł���s�����X�g
typedef enum
{
	FCAL_TRAINERCARD,	//�g���[�i�[�J�[�h
	FCAL_BATTLE,		//�o�g��
	FCAL_END,			//�I��
	FCAL_MAX,

	FCAL_SELECT,		//�I���s���ł͂Ȃ����A�I�𒆗p
}F_COMM_ACTION_LIST;

//�ėp�t���O�̑��M
typedef enum
{
	FCCF_TALK_REQUEST,		//��b�v��
	FCCF_TALK_UNPOSSIBLE,	//�v���ɑ΂��Ė���
	FCCF_TALK_ACCEPT,		//�v���ɑ΂��ċ���

	FCCF_ACTION_SELECT,		//�e���I�񂾍s��
	FCCF_ACTION_RETURN,		//�s���ɑ΂���Ԏ�

	FCCF_SYNC_TYPE,			//���������p�֐�

	FCCF_TEST,				//�����p
	
	FCCF_FLG_MAX,			//�s���l�Ƃ��Ďg��
}F_COMM_COMMON_FLG;

//�����R�}���h���
typedef enum
{
	FCST_START_INIT_ACTION, //�s���O�̏�������
	FCST_START_CHANGE_PROC,	//�p�[�g�J�ڂ̃^�C�~���O
	FCST_WAIT_RETURN_PROC,	//�p�[�g�J�ڌ�̑҂�

	FCST_MAX,
}F_COMM_SYNC_TYPE;

//FIELD_COMM_DATA�Ŋm�ۂł��郆�[�U�[�f�[�^�̎��
typedef enum
{
	FCUT_TRAINERCARD,	//�g���[�i�[�J�[�h
	FCUT_BATTLE,		//�o�g��

	FCUT_MAX,
	FCUT_NO_INIT,	//�������O

}F_COMM_USERDATA_TYPE;

//======================================================================
//	typedef struct
//======================================================================
typedef struct _FIELD_COMM_MAIN FIELD_COMM_MAIN;
typedef struct _FIELD_COMM_EVENT FIELD_COMM_EVENT;
typedef struct _FIELD_COMM_MENU FIELD_COMM_MENU;
typedef struct _FIELD_COMM_FUNC FIELD_COMM_FUNC;
typedef struct _FIELD_COMM_DEBUG_WORK FIELD_COMM_DEBUG_WORK;

//�ʐM���̃��[�U�[�f�[�^
typedef struct
{
	char	name_[128];
	char	id_[128];
	char	pad_[768];
}FIELD_COMM_USERDATA_TRAINERCARD;

//======================================================================
//	proto
//======================================================================

#endif //FIELD_COMM_DEF_H__

