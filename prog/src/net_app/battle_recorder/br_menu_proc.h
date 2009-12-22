//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_menu_proc.h
 *	@brief	���j���[�v���Z�X
 *	@author	Toru=Nagihashi
 *	@date		2009.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "br_proc_sys.h"
#include "br_res.h"
#include "br_fade.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	���j���[�C���f�b�N�X
//		�����ł̃��j���[�Ƃ͏��ʂ��؂�ւ炸�A
//		�{�^���������Ă������ƂŐi��ł����ӏ��ł��B
//		�i�����͕ʉ�ʂ̂悤�Ɏv���܂�����L�̈Ӗ��Ŕ��e�ł��j
//=====================================
typedef enum
{
	BR_BROWSE_MENUID_TOP,						//�u���E�U���[�h�@�������j���[

	BR_BROWSE_MENUID_BTLVIDEO,			//�u���E�U���[�h�@�o�g���r�f�I���j���[,

	BR_BROWSE_MENUID_OTHER_RECORD,	//�u���E�U���[�h�@���ꂩ�̋L�^���j���[
	BR_BROWSE_MENUID_DELETE_RECORD,	//�u���E�U���[�h�@�L�^���������j���[
	BR_BROWSE_MENUID_DELETE_OTHER,	//�u���E�U���[�h�@�N���̋L�^���������j���[
	
	BR_BTLVIDEO_MENUID_TOP,					//�O���[�o���o�g���r�f�I���[�h�@�������j���[

	BR_BTLVIDEO_MENUID_LOOK,				//�O���[�o���o�g���r�f�I���[�h�@���郁�j���[
	BR_BTLVIDEO_MENUID_RANK,				//�O���[�o���o�g���r�f�I���[�h�@�����L���O�ŒT�����j���[

	BR_MUSICAL_MENUID_TOP,					//�O���[�o���~���[�W�J���V���b�g���[�h�@�������j���[

	BR_MENUID_YESNO,								//�ėp	�͂��A������
	BR_MENUID_MAX
} BR_MENUID;

//=============================================================================
/**
 *					�\����
*/
//=============================================================================
#define BR_BTLREC_DATA_NUM  4
//-------------------------------------
///	���j���[�Ŏg�p����^��f�[�^
//=====================================
typedef struct 
{
  BOOL    is_valid[BR_BTLREC_DATA_NUM];
  STRBUF  *p_name[BR_BTLREC_DATA_NUM];
  u32     sex[BR_BTLREC_DATA_NUM];
} BR_MENU_BTLREC_DATA;


//-------------------------------------
///	���j���[�v���Z�X����
//=====================================
typedef struct 
{
	BR_MENUID				menuID;				//[in/out]�v���Z�X�J�n���A�I�����̃��j���[
	u32							next_proc;		//[out]���ֈړ�����v���Z�X(���ۂ̈ړ���menu_proc��push)
	u32							next_mode;		//[out]���ֈړ�����v���Z�X�ɓn�����
	
  BR_FADE_WORK    *p_fade;      //[in]�t�F�[�h
	GFL_CLUNIT			*p_unit;			//[in]���j�b�g
	BR_RES_WORK			*p_res;				//[in]���\�[�X�Ǘ�
	BR_PROC_SYS			*p_procsys;		//[in]�v���Z�X�Ǘ�
  const BR_MENU_BTLREC_DATA *cp_btlrec; //[in]�^��f�[�^
} BR_MENU_PROC_PARAM;


//=============================================================================
/**
 *					�O�����J�֐�
*/
//=============================================================================
//-------------------------------------
///	���j���[�v���Z�X�f�[�^
//=====================================
extern const GFL_PROC_DATA	BR_MENU_ProcData;
