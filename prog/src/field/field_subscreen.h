//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   field_subscreen.h
 *  @brief    InforBer�̏������E�j���@
 *  @date   2009.03.26  fieldmap����ڐA
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "gamesystem/gamesystem.h"
#include "field/fieldmap_proc.h"  //FIELDMAP_WORK�Q��
#include "field/field_menu_item.h"     //FIELD_MENU_ITEM_TYPE�Q��

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *          �萔�錾
*/
//-----------------------------------------------------------------------------

typedef void (STARTUP_ENDCALLBACK) (void* pWork);


//BG�ʂƃp���b�g�ԍ�(���ݒ�
#define FIELD_SUBSCREEN_BGPLANE (GFL_BG_FRAME0_S)
#define FIELD_SUBSCREEN_PALLET  (0xE)

///����ʂ��A�N�e�B�u�ɂ����ꍇ�̊�ƂȂ�P�x
#define FIELD_NONE_ACTIVE_EVY   (8)

///����ʂ̃��[�h�w��i�f�o�b�O�ł�������Ȃ��͂��j
typedef enum {  
  FIELD_SUBSCREEN_NORMAL = 0,         // C�M�A
  FIELD_SUBSCREEN_TOPMENU,            // �t�B�[���h���j���[
  FIELD_SUBSCREEN_UNION,              // ���j�I�������
  FIELD_SUBSCREEN_INTRUDE,            // �N��
  FIELD_SUBSCREEN_BEACON_VIEW,        // ����Ⴂ���ʉ��
  FIELD_SUBSCREEN_NOGEAR,             // C�M�A�̖��擾��
  FIELD_SUBSCREEN_DOWSING,            // �_�E�W���O
  FIELD_SUBSCREEN_REPORT,             // ���|�[�g���
  FIELD_SUBSCREEN_CGEARFIRST,         // CGEAR�N�����
#if PM_DEBUG
  FIELD_SUBSCREEN_DEBUG_LIGHT,        // �f�o�b�O���C�g����p�l��
  FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA,  // �f�o�b�O�J����
  FIELD_SUBSCREEN_DEBUG_SOUNDVIEWER,  // �f�o�b�O�T�E���h
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
  FIELD_SUBSCREEN_ACTION_PALACE_WARP,       ///<�p���X�փ��[�v
  FIELD_SUBSCREEN_ACTION_GSYNC,  ///< GAMESYNC
  FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_BEACON_VIEW, ///< �T�u�X�N���[�����r�[�R���Q�Ɖ�ʂɕύX
  FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_CGEAR, ///< �T�u�X�N���[����CGEAR��ʂɕύX
  FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_INTRUDE,     ///< �T�u�X�N���[��
  FIELD_SUBSCREEN_ACTION_BEACON_DETAIL,  ///<�r�[�R���ڍ׉�ʌĂяo��
  FIELD_SUBSCREEN_ACTION_DOWSING,         ///< �_�E�W���O
  FIELD_SUBSCREEN_ACTION_SCANRADAR,     ///< �������[�_�[
  FIELD_SUBSCREEN_ACTION_CGEAR_HELP,    ///< CGEARHELP
  
  FIELD_SUBSCREEN_ACTION_MAX,

}FIELD_SUBSCREEN_ACTION;

//-----------------------------------------------------------------------------
/**
 *          �\���̐錾
*/
//-----------------------------------------------------------------------------

typedef struct _FIELD_SUBSCREEN_WORK FIELD_SUBSCREEN_WORK;

//-----------------------------------------------------------------------------
/**
 *          �v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
extern FIELD_SUBSCREEN_WORK* FIELD_SUBSCREEN_Init( u32 heapID,
    FIELDMAP_WORK * fieldmap, FIELD_SUBSCREEN_MODE mode );
extern u8 FIELD_SUBSCREEN_Exit( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_Main( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_Draw( FIELD_SUBSCREEN_WORK* pWork );
extern GMEVENT* FIELD_SUBSCREEN_EventCheck( FIELD_SUBSCREEN_WORK* pWork, BOOL bEvReq );
extern const BOOL FIELD_SUBSCREEN_CanChange( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_Change( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode);
extern void FIELD_SUBSCREEN_ChangeForce( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode);
extern void FIELD_SUBSCREEN_ChangeFromWithin( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode, BOOL bFade);
extern void FIELD_SUBSCREEN_CgearFirst( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode,STARTUP_ENDCALLBACK* pCall,void* pWork2);

extern FIELD_SUBSCREEN_MODE FIELD_SUBSCREEN_GetMode(const FIELD_SUBSCREEN_WORK * pWork);

extern FIELD_SUBSCREEN_ACTION FIELD_SUBSCREEN_GetAction( FIELD_SUBSCREEN_WORK* pWork);
extern void FIELD_SUBSCREEN_ResetAction( FIELD_SUBSCREEN_WORK* pWork);
extern void FIELD_SUBSCREEN_MainDispBrightnessOff( HEAPID heapId );
extern void FIELD_SUBSCREEN_SetAction( FIELD_SUBSCREEN_WORK* pWork , FIELD_SUBSCREEN_ACTION actionno);
extern void FIELD_SUBSCREEN_GrantPermission( FIELD_SUBSCREEN_WORK* pWork);

//�t�B�[���h���j���[�ƌ��荀�ڔԍ��̂����
extern const FIELD_MENU_ITEM_TYPE FIELD_SUBSCREEN_GetTopMenuItemNo( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_SetTopMenuItemNo( FIELD_SUBSCREEN_WORK* pWork , const FIELD_MENU_ITEM_TYPE itemType );

#if 0
extern BOOL FIELD_SUBSCREEN_CheckReportInit( FIELD_SUBSCREEN_WORK * pWork );
#endif
extern void FIELD_SUBSCREEN_SetReportSize( FIELD_SUBSCREEN_WORK * pWork );
extern void FIELD_SUBSCREEN_SetReportStart( FIELD_SUBSCREEN_WORK * pWork );
extern BOOL FIELD_SUBSCREEN_SetReportEnd( FIELD_SUBSCREEN_WORK * pWork );
extern void FIELD_SUBSCREEN_SetReportBreak( FIELD_SUBSCREEN_WORK * pWork );
extern BOOL FIELD_SUBSCREEN_CheckReportType( FIELD_SUBSCREEN_WORK * pWork );

extern BOOL FIELD_SUBSCREEN_EnablePalaceUse( FIELD_SUBSCREEN_WORK* pWork );


#ifdef  PM_DEBUG
extern void * FIELD_SUBSCREEN_DEBUG_GetControl(FIELD_SUBSCREEN_WORK * pWork);
#endif  //PM_DEBUG

#ifdef _cplusplus
} // extern "C"{
#endif



