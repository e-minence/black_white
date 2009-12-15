//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_subscreen.h
 *	@brief		InforBer�̏������E�j���@
 *	@date		2009.03.26	fieldmap����ڐA
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "field/fieldmap_proc.h"  //FIELDMAP_WORK�Q��
#include "field/field_menu_item.h"     //FIELD_MENU_ITEM_TYPE�Q��

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//BG�ʂƃp���b�g�ԍ�(���ݒ�
#define FIELD_SUBSCREEN_BGPLANE	(GFL_BG_FRAME0_S)
#define FIELD_SUBSCREEN_PALLET	(0xE)

///����ʂ̃��[�h�w��i�f�o�b�O�ł�������Ȃ��͂��j
typedef enum {	
	FIELD_SUBSCREEN_NORMAL = 0,
  FIELD_SUBSCREEN_TOPMENU,
  FIELD_SUBSCREEN_UNION,
	FIELD_SUBSCREEN_INTRUDE,
	FIELD_SUBSCREEN_BEACON_VIEW,
  FIELD_SUBSCREEN_NOGEAR,
#if PM_DEBUG
	FIELD_SUBSCREEN_DEBUG_LIGHT,
	FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA,
	FIELD_SUBSCREEN_DEBUG_SOUNDVIEWER,
#endif
	FIELD_SUBSCREEN_MODE_MAX,

    //�T�u���j���[�̏������ɓn���O�񃂁[�h�ŋN�����ɌĂ΂ꂽ�ꍇ
  FIELD_SUBSCREEN_FIRST_CALL,
}FIELD_SUBSCREEN_MODE;


///����ʂ̎󂯓n���l
typedef enum {
  FIELD_SUBSCREEN_ACTION_NONE,
  FIELD_SUBSCREEN_ACTION_IRC,  ///< IRC�A�v���N��
  FIELD_SUBSCREEN_ACTION_TOPMENU,   ///< TOPMENU�N��
  FIELD_SUBSCREEN_ACTION_TOPMENU_DECIDE,  ///< FieldMenu���ڌ���
  FIELD_SUBSCREEN_ACTION_TOPMENU_EXIT,    ///< FieldMenu�I��
  FIELD_SUBSCREEN_ACTION_WIRELESS,  ///<���C�����X
  FIELD_SUBSCREEN_ACTION_UNION_CHAT,        ///<���j�I���F�`���b�g��ʃV���[�g�J�b�g
  FIELD_SUBSCREEN_ACTION_INTRUDE_TOWN_WARP, ///<�N���F�X���[�v
  FIELD_SUBSCREEN_ACTION_INTRUDE_MISSION_PUT, ///<�N���F�~�b�V�����̕\��
  FIELD_SUBSCREEN_ACTION_GSYNC,  ///< GAMESYNC
  FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_BEACON_VIEW, ///< �T�u�X�N���[�����r�[�R���Q�Ɖ�ʂɕύX
  FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_CGEAR, ///< �T�u�X�N���[����CGEAR��ʂɕύX
  
  FIELD_SUBSCREEN_ACTION_MAX,

}FIELD_SUBSCREEN_ACTION;

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

typedef struct _FIELD_SUBSCREEN_WORK FIELD_SUBSCREEN_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
extern FIELD_SUBSCREEN_WORK* FIELD_SUBSCREEN_Init( u32 heapID,
    FIELDMAP_WORK * fieldmap, FIELD_SUBSCREEN_MODE mode );
extern u8 FIELD_SUBSCREEN_Exit( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_Main( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_Draw( FIELD_SUBSCREEN_WORK* pWork );
extern const BOOL FIELD_SUBSCREEN_CanChange( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_Change( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode);
extern void FIELD_SUBSCREEN_ChangeForce( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode);
extern void FIELD_SUBSCREEN_ChangeFromWithin( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode, BOOL bFade);

extern FIELD_SUBSCREEN_MODE FIELD_SUBSCREEN_GetMode(const FIELD_SUBSCREEN_WORK * pWork);

extern FIELD_SUBSCREEN_ACTION FIELD_SUBSCREEN_GetAction( FIELD_SUBSCREEN_WORK* pWork);
extern void FIELD_SUBSCREEN_ResetAction( FIELD_SUBSCREEN_WORK* pWork);
extern void FIELD_SUBSCREEN_SetAction( FIELD_SUBSCREEN_WORK* pWork , FIELD_SUBSCREEN_ACTION actionno);
extern void FIELD_SUBSCREEN_GrantPermission( FIELD_SUBSCREEN_WORK* pWork);

//�t�B�[���h���j���[�ƌ��荀�ڔԍ��̂����
extern const FIELD_MENU_ITEM_TYPE FIELD_SUBSCREEN_GetTopMenuItemNo( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_SetTopMenuItemNo( FIELD_SUBSCREEN_WORK* pWork , const FIELD_MENU_ITEM_TYPE itemType );


#ifdef	PM_DEBUG
extern void * FIELD_SUBSCREEN_DEBUG_GetControl(FIELD_SUBSCREEN_WORK * pWork);
#endif	//PM_DEBUG

#ifdef _cplusplus
}	// extern "C"{
#endif



