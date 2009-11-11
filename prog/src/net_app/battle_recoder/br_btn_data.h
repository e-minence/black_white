//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_btn_data.h
 *	@brief	�{�^�����
 *	@author	Toru=Nagihashi
 *	@date		2009.11.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�f�[�^�������Ă��Ȃ��l
//=====================================
#define BR_BTN_DATA_NONE	(0xFFFF)


#define BR_BTN_DATA_WIDTH	()

//-------------------------------------
///	���j���[�C���f�b�N�X
//		�����ł̃��j���[�Ƃ͏��ʂ��؂�ւ炸�A
//		���j���[�������Ă������ƂŐi��ł����ӏ��ł��B
//		�i�����͓���܂��j
//=====================================
typedef enum
{
	BR_BROWSE_MENUID_TOP,					//�u���E�U���[�h�@�������j���[

	BR_BROWSE_MENUID_BTLVIDEO,		//�u���E�U���[�h�@�o�g���r�f�I���j���[,

	BR_BROWSE_MENUID_OTHER_RECORD,//�u���E�U���[�h�@���ꂩ�̋L�^���j���[
	BR_BROWSE_MENUID_DELETE_RECORD,//�u���E�U���[�h�@�L�^���������j���[
	BR_BROWSE_MENUID_DELETE_OTHER,//�u���E�U���[�h�@�N���̋L�^���������j���[

	//BR_BROWSE_MENUID_BTLSUBWAY,		//�u���E�U���[�h�@�o�g���T�u�E�F�C���j���[	
	//BR_BROWSE_MENUID_RNDMATCH,		//�u���E�U���[�h�@�����_���}�b�`���j���[
	
	BR_BTLVIDEO_MENUID_TOP,					//�O���[�o���o�g���r�f�I���[�h�@�������j���[

	BR_BTLVIDEO_MENUID_LOOK,				//�O���[�o���o�g���r�f�I���[�h�@���郁�j���[
	BR_BTLVIDEO_MENUID_RANK,				//�O���[�o���o�g���r�f�I���[�h�@�����L���O�ŒT�����j���[

	BR_MUSICAL_MENUID_TOP,					//�O���[�o���~���[�W�J���V���b�g���[�h�@�������j���[

	BR_MENUID_YESNO,								//�ėp	�͂��A������
	BR_MENUID_MAX
} BR_MENUID;

//-------------------------------------
///	�{�^���̎��
//=====================================
typedef enum
{
	BR_BTN_TYPE_SELECT,				//�I��p�{�^��
	BR_BTN_TYPE_RETURN,				//1�O�̃��j���[�֖߂�p�{�^��
	BR_BTN_TYPE_EXIT,					//�o�g�����R�[�_�[�I���p�{�^��
	BR_BTN_TYPE_MYRECORD,			//�����̋L�^�{�^��
	BR_BTN_TYPE_OTHERRECORD,	//�N���̋L�^�{�^��
	BR_BTN_TYPE_DELETE_MY,		//�����̋L�^�������{�^��
	BR_BTN_TYPE_DELETE_OTHER,	//�N���̋L�^�������{�^��
	BR_BTN_TYPE_CHANGESEQ,		//�V�[�P���X�ύX�{�^��
} BR_BTN_TYPE;

//-------------------------------------
///	�{�^���f�[�^���J���C���f�b�N�X
//=====================================
typedef enum
{
	BR_BTN_DATA_PARAM_X,			//�ʒuX
	BR_BTN_DATA_PARAM_Y,			//�ʒuY
	BR_BTN_DATA_PARAM_MENUID,	//�����̑����Ă��郁�j���[ID
	BR_BTN_DATA_PARAM_MSGID,	//�����̃A�C�R���ɕ\�����郁�b�Z�[�WID
	BR_BTN_DATA_PARAM_ANMSEQ,	//�����̃A�C�R���̃A�j���V�[�P���X
	BR_BTN_DATA_PARAM_TYPE,		//�{�^���̎��
	BR_BTN_DATA_PARAM_DATA,		//��ޕʂ̃f�[�^

	BR_BTN_DATA_PARAM_MAX
} BR_BTN_DATA_PARAM;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�{�^�����Ǘ�
//=====================================
typedef struct _BR_BTN_DATA_SYS  BR_BTN_DATA_SYS;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�f�[�^�擾
//=====================================
extern const BR_BTN_DATA * BR_BTN_DATA_SYS_GetData( BR_MENUID menuID, u16 btnID );
extern u32 BR_BTN_DATA_SYS_GetDataNum( BR_MENUID menuID );
extern u32 BR_BTN_DATA_SYS_GetDataMax( void );
extern BR_MENUID BR_BTN_DATA_SYS_GetStartMenuID( BR_MODE mode );
//-------------------------------------
///	�������擾
//=====================================
extern u16 BR_BTN_DATA_GetParam( const BR_BTN_DATA *cp_data, BR_BTN_DATA_PARAM paramID );
