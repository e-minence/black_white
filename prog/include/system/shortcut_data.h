//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		shortcut_data.h
 *	@brief	Y�{�^���o�^
 *	@author	Toru=Nagihashi
 *	@date		2009.10.20
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
///	Y�{�^���o�^�ӏ�
//=====================================
typedef enum
{
	SHORTCUT_ID_ZITENSYA,			//���]��
	SHORTCUT_ID_TOWNMAP,			//�^�E���}�b�v
	SHORTCUT_ID_PALACEGO,			//�p���X�փS�[
	SHORTCUT_ID_BTLRECORDER,	//�o�g�����R�[�_�[
	SHORTCUT_ID_FRIENDNOTE,		//�F�B���
	SHORTCUT_ID_TURIZAO,			//�肴��

	SHORTCUT_ID_PSTATUS_STATUS,		//�|�P������񁄃X�e�[�^�X
	SHORTCUT_ID_PSTATUS_ABILITY,	//�|�P������񁄂̂���傭
	SHORTCUT_ID_PSTATUS_RIBBON,		//�|�P������񁄂��˂񃊃{��
	SHORTCUT_ID_BAG_ITEM,					//�o�b�O���ǂ���
	SHORTCUT_ID_BAG_RECOVERY,			//�o�b�O�������ӂ�
	SHORTCUT_ID_BAG_WAZAMACHINE,	//�o�b�O���Z�}�V��
	SHORTCUT_ID_BAG_NUTS,					//�o�b�O�����̂�
	SHORTCUT_ID_BAG_IMPORTANT,		//�o�b�O���������Ȃ���
	SHORTCUT_ID_ZUKAN_MENU,				//�����񁄃��j���[
	SHORTCUT_ID_ZUKAN_BUNPU,			//�����񁄕��z
	SHORTCUT_ID_ZUKAN_CRY,				//�����񁄂Ȃ�����
	SHORTCUT_ID_TRCARD_FRONT,			//�g���[�i�[�J�[�h������߂�
	SHORTCUT_ID_TRCARD_BACK,			//�g���[�i�[�J�[�h������߂�
	SHORTCUT_ID_CONFIG,						//�����Ă�


	SHORTCUT_ID_MAX,
	SHORTCUT_ID_NULL	= SHORTCUT_ID_MAX,	//�f�[�^�Ȃ�
} SHORTCUT_ID;
//=============================================================================
/**
 *					�\����
*/
//=============================================================================
//-------------------------------------
///	�V���[�g�J�b�g�̃J�[�\��
//=====================================
typedef struct 
{
	u16 list;
	u16 cursor;
} SHORTCUT_CURSOR;


//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//-------------------------------------
///	�J�[�\���\���̏�����
//=====================================
extern void SHORTCUT_CURSOR_Init( SHORTCUT_CURSOR *p_wk );

//-------------------------------------
///	UTIL
//=====================================
extern SHORTCUT_ID SHORTCUT_DATA_GetItemToShortcutID( u16 item );
