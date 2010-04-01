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
#include "system/main.h"  //HEAPID
#include "br_menu_proc.h"	//BR_MENUID
#include "br_inner.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�f�[�^�������Ă��Ȃ��l
//=====================================
#define BR_BTN_DATA_NONE	(0xFFFF)

//-------------------------------------
///	
//=====================================
#define BR_BTN_DATA_WIDTH	      	(20*8)
#define BR_BTN_DATA_SHORT_WIDTH		(12*8)
#define BR_BTN_DATA_HEIGHT	      (4*8)
#define BR_BTN_POS_X		    			(42)

//-------------------------------------
///	�{�^���̎��
//=====================================
typedef enum
{
	BR_BTN_TYPE_SELECT,				//�I��p�{�^��  data1��BR_MENUID
	BR_BTN_TYPE_SELECT_MSG,		//�I�� �{�@���b�Z�[�W�\���{�^�� data1��BR_MENUID data2�� ���b�Z�[�WID�@
	BR_BTN_TYPE_RETURN,				//1�O�̃��j���[�֖߂�p�{�^�� data1��BR_MENUID
	BR_BTN_TYPE_EXIT,					//�o�g�����R�[�_�[�I���p�{�^��
	BR_BTN_TYPE_CHANGESEQ,		//�V�[�P���X�ύX�{�^��    data1�Ɉړ���PROC�@data2��PROC���[�h

} BR_BTN_TYPE;

//-------------------------------------
///	�����Ƃ��̃{�^���̓���
//=====================================
typedef enum
{
	BR_BTN_NONEPROC_VANISH,	//������  
	BR_BTN_NONEPROC_NOPUSH,	//�����Ȃ��i���ʂɃ��b�Z�[�W�\�� data1�Ƀ��b�Z�[�WID�j
} BR_BTN_NONEPROC;

//-------------------------------------
///	����{�^��
//=====================================
typedef enum
{
  BR_BTN_UNIQUE_NONE,         //����{�^���ł͂Ȃ�
	BR_BTN_UNIQUE_MYRECORD,			//�����̋L�^�����邩
	BR_BTN_UNIQUE_OTHERRECORD1,	//�N���̋L�^�����邩
	BR_BTN_UNIQUE_OTHERRECORD2,	//�N���̋L�^�����邩
	BR_BTN_UNIQUE_OTHERRECORD3,	//�N���̋L�^�����邩
  BR_BTN_UNIQUE_MUSICALSHOT,	//�~���[�W�J���V���b�g�����邩
} BR_BTN_UNIQUE;

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
	BR_BTN_DATA_PARAM_TYPE,		//�{�^���̎��		BR_BTN_TYPE��
	BR_BTN_DATA_PARAM_DATA1,	//��ޕʂ̃f�[�^�P  BR_BTN_TYPE�񋓂̃R�����g�Q��
	BR_BTN_DATA_PARAM_DATA2,	//��ޕʂ̃f�[�^�Q  BR_BTN_TYPE�񋓂̃R�����g�Q��
	BR_BTN_DATA_PARAM_VALID,	//�L���ȃ{�^����	TRUE or FALSE
	BR_BTN_DATA_PARAM_NONE_PROC,	//�L���łȂ��Ƃ��̏���		BR_BTN_NONEPROC��
	BR_BTN_DATA_PARAM_NONE_DATA,	//�L���łȂ��Ƃ��̏����Ŏg���f�[�^
  BR_BTN_DATA_PARAM_UNIQUE,	    //����{�^��

	BR_BTN_DATA_PARAM_MAX
} BR_BTN_DATA_PARAM;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�{�^�����o�b�t�@
//=====================================
typedef struct _BR_BTN_DATA_SYS  BR_BTN_DATA_SYS;

//-------------------------------------
///	�{�^�����
//=====================================
typedef struct _BR_BTN_DATA  BR_BTN_DATA;

//-------------------------------------
///	�O������{�^���ɗ^������
//=====================================
#define BR_RECORD_NUM 4
typedef struct 
{
  const BR_SAVE_INFO  *cp_saveinfo;
} BR_BTN_DATA_SETUP;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
extern BR_BTN_DATA_SYS * BR_BTN_DATA_SYS_Init( const BR_BTN_DATA_SETUP *cp_setup, HEAPID heapID );
extern void BR_BTN_DATA_SYS_Exit( BR_BTN_DATA_SYS *p_wk );

//-------------------------------------
///	�f�[�^�擾
//=====================================
extern const BR_BTN_DATA * BR_BTN_DATA_SYS_GetData( const BR_BTN_DATA_SYS *cp_wk, BR_MENUID menuID, u16 btnID );
extern u32 BR_BTN_DATA_SYS_GetDataNum( const BR_BTN_DATA_SYS *cp_wk, BR_MENUID menuID );
extern u32 BR_BTN_DATA_SYS_GetDataMax( const BR_BTN_DATA_SYS *cp_wk );

extern void BR_BTN_DATA_SYS_GetLink( const BR_BTN_DATA_SYS *cp_wk, BR_MENUID menuID, u32 idx, BR_MENUID *p_preID, u32 *p_btnID );
extern u32 BR_BTN_DATA_SYS_GetLinkBtnID( const BR_BTN_DATA_SYS *cp_wk, BR_MENUID menuID, BR_MENUID preID );
extern u32 BR_BTN_DATA_SYS_GetLinkMax( const BR_BTN_DATA_SYS *cp_wk, BR_MENUID menuID );

//-------------------------------------
///	�������擾
//=====================================
extern u16 BR_BTN_DATA_GetParam( const BR_BTN_DATA *cp_data, BR_BTN_DATA_PARAM paramID );
extern STRBUF *BR_BTN_DATA_CreateString( const BR_BTN_DATA_SYS *cp_sys, const BR_BTN_DATA *cp_data, GFL_MSGDATA *p_msg, HEAPID heapID );
