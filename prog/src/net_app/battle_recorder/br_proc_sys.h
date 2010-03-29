//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_proc.h
 *	@brief	�o�g�����R�[�_�[�v���Z�X�q����Ǘ�
 *	@author	Toru=Nagihashi
 *	@date		2009.11.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "system/main.h"  //HEAPID

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//��ԍŏ���BR_PROC_SYS_BEFORE_FUNCTION�֐��ł�preID���炱�̒l���A���Ă���
#define BR_PROC_SYS_NONE_ID	    (0xFFFFFFFF)
//���A���͈ȉ��̃p�����[�^��preID����A���Ă���
#define BR_PROC_SYS_RECOVERY_ID	(0xFFFFFFFE)

#define BR_PROC_SYS_STACK_MAX	  (5)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X�q����Ǘ����[�N
//=====================================
typedef struct _BR_PROC_SYS BR_PROC_SYS;

//-------------------------------------
///	�����쐬�A�j���֐�
//=====================================
typedef void (*BR_PROC_SYS_BEFORE_FUNCTION)( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
typedef void (*BR_PROC_SYS_AFTER_FUNCTION)( void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	�v���Z�X�e�[�u���쐬�\����
//=====================================
typedef struct 
{
	const GFL_PROC_DATA						*cp_proc_data;	//�v���Z�X�e�[�u��
	u32														param_size;			//�����̃T�C�Y
  FSOverlayID                   ov_id;          //�I�[�o�[���CID
	BR_PROC_SYS_BEFORE_FUNCTION		before_func;		//�v���Z�X�J�n�O�֐�
	BR_PROC_SYS_AFTER_FUNCTION		after_func;			//�v���Z�X�I����֐�
} BR_PROC_SYS_DATA;

//-------------------------------------
///	�v���Z�X���A�f�[�^
//=====================================
typedef struct 
{
  u32	  stackID[BR_PROC_SYS_STACK_MAX];
  u32		stack_num;
} BR_PROC_SYS_RECOVERY_DATA;


//=============================================================================
/**
 *					PUBLIC�֐�
*/
//=============================================================================
//-------------------------------------
///	�V�X�e��
//=====================================
extern BR_PROC_SYS * BR_PROC_SYS_Init( u16 startID, const BR_PROC_SYS_DATA *cp_procdata_tbl, u16 tbl_max, void *p_wk_adrs, BR_PROC_SYS_RECOVERY_DATA *p_recovery, HEAPID heapID );
extern void BR_PROC_SYS_Exit( BR_PROC_SYS *p_wk );
extern void BR_PROC_SYS_Main( BR_PROC_SYS *p_wk );
extern BOOL BR_PROC_SYS_IsEnd( const BR_PROC_SYS *cp_wk );

//-------------------------------------
///	PROC�̒��Ŏg���֐�
//		�ePROC�̈�����BR_PROC_SYS��n���A�ȉ��̂��g�����삵�Ă�������
//=====================================
extern void * BR_PROC_SYS_GetParam( const BR_PROC_SYS *cp_wk ); //�g��Ȃ��Ȃ���
extern HEAPID BR_PROC_SYS_GetHeapID( const BR_PROC_SYS *cp_wk );
extern void BR_PROC_SYS_Pop( BR_PROC_SYS *p_wk );
extern void BR_PROC_SYS_Push( BR_PROC_SYS *p_wk, u16 procID );

extern void BR_PROC_SYS_Interruput( BR_PROC_SYS *p_wk );
