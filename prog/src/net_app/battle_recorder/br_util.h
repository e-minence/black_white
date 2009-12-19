//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_util.h
 *	@brief  �ePROC�Ŏg�����ʃ��W���[���n
 *	@author	Toru=Nagihashi
 *	@date		2009.11.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "print/gf_font.h"
#include "print/printsys.h"
#include "system/bmp_menuwork.h"

#include "br_res.h"

#include "savedata/gds_profile.h"

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					BMPWIN���b�Z�[�W�`��\����
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
typedef struct _BR_MSGWIN_WORK BR_MSGWIN_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern BR_MSGWIN_WORK * BR_MSGWIN_Init( u16 frm, u8 x, u8 y, u8 w, u8 h, u8 plt, PRINT_QUE *p_que, HEAPID heapID );
extern void BR_MSGWIN_Exit( BR_MSGWIN_WORK* p_wk );
extern void BR_MSGWIN_Print( BR_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font );
extern void BR_MSGWIN_PrintBuf( BR_MSGWIN_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font );
extern void BR_MSGWIN_PrintColor( BR_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font, PRINTSYS_LSB lsb );
void BR_MSGWIN_PrintBufColor( BR_MSGWIN_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font, PRINTSYS_LSB lsb );
extern BOOL BR_MSGWIN_PrintMain( BR_MSGWIN_WORK* p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					  ���X�g�\���\����
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	SELECT�̕Ԃ�l
//=====================================
#define BR_LIST_SELECT_NONE (0xFFFFFFFF)

//-------------------------------------
///	���X�g�^�C�v
//=====================================
typedef enum
{
  BR_LIST_TYPE_TOUCH,  //�^�b�`���X�g
  BR_LIST_TYPE_CURSOR, //�J�[�\�����X�g
} BR_LIST_TYPE;

//-------------------------------------
///	���X�g�ݒ���
//=====================================
typedef struct 
{
  const BMP_MENULIST_DATA *cp_list;
  u32                     list_max;
  u8  x;
  u8  y;
  u8  w;
  u8  h;
  u8  plt;
  u8  frm;
  u8  str_line; //���s�̕�����(���L�����g����)
  BR_LIST_TYPE  type;
  BR_RES_WORK   *p_res;
  GFL_CLUNIT    *p_unit;
} BR_LIST_PARAM;

//-------------------------------------
///	���X�g
//=====================================
typedef struct _BR_LIST_WORK BR_LIST_WORK;

//-------------------------------------
///	�p�u���b�N
//=====================================
extern BR_LIST_WORK * BR_LIST_Init( const BR_LIST_PARAM *cp_param, HEAPID heapID );
extern void BR_LIST_Exit( BR_LIST_WORK* p_wk );
extern void BR_LIST_Main( BR_LIST_WORK* p_wk );

extern BOOL BR_LIST_IsMoveEnable( const BR_LIST_WORK* cp_wk );
extern u32 BR_LIST_GetSelect( const BR_LIST_WORK* cp_wk );

//-------------------------------------
///	���X�g�g��
//=====================================
//  �ʏ�͏�L�֐��݂̂œ��삷�邪�A�O���������ȕ������������݂����Ƃ��́A
//  BMP�ɕ��������������ŁA���L�֐��ɓn�����ƂŁA�X�N���[���ɑΉ��ł���
//  ���̍ہABMP_MENULIST_DATA��str��NULL�ɂ��邱�ƁB
extern void BR_LIST_SetBmp( BR_LIST_WORK* p_wk, u16 idx, GFL_BMP_DATA *p_src );
extern GFL_BMP_DATA *BR_LIST_GetBmp( const BR_LIST_WORK* cp_wk, u16 idx );

typedef enum
{
  BR_LIST_PARAM_IDX_CURSOR_POS,
  BR_LIST_PARAM_IDX_LIST_POS,
  BR_LIST_PARAM_IDX_MOVE_TIMING,
  BR_LIST_PARAM_IDX_LIST_MAX,

} BR_LIST_PARAM_IDX;
extern u32 BR_LIST_GetParam( const BR_LIST_WORK* cp_wk, BR_LIST_PARAM_IDX param );
extern void BR_LIST_Write( BR_LIST_WORK *p_wk );
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					  ���ʃe�L�X�g
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�e�L�X�g���[�N
//    �����I�ɂ�MSGWIN�Ɠ���
//=====================================
typedef BR_MSGWIN_WORK BR_TEXT_WORK;

//-------------------------------------
///	�p�u���b�N
//=====================================
extern BR_TEXT_WORK * BR_TEXT_Init( BR_RES_WORK *p_res, PRINT_QUE *p_que, HEAPID heapID );
extern void BR_TEXT_Exit( BR_TEXT_WORK *p_wk, BR_RES_WORK *p_res );
extern void BR_TEXT_Print( BR_TEXT_WORK* p_wk, const BR_RES_WORK *cp_res, u32 strID );
extern BOOL BR_TEXT_PrintMain( BR_TEXT_WORK* p_wk );


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					  �v���t�B�[����ʍ쐬
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�v���t�B�[�����[�N
//=====================================
typedef struct _BR_PROFILE_WORK BR_PROFILE_WORK;

//-------------------------------------
///	�p�u���b�N
//=====================================
extern BR_PROFILE_WORK * BR_PROFILE_CreateMainDisplay( const GDS_PROFILE_PTR cp_profile, BR_RES_WORK *p_res, GFL_CLUNIT *p_unit, PRINT_QUE *p_que, HEAPID heapID );
extern void BR_PROFILE_DeleteMainDisplay( BR_PROFILE_WORK *p_wk );
extern BOOL BR_PROFILE_PrintMain( BR_PROFILE_WORK *p_wk );




//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					  �f�o�b�O�p
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
#ifdef PM_DEBUG
#include "musical/mus_shot_photo.h"
#include "poke_tool/monsno_def.h" //debug�p
//----------------------------------------------------------------------------
/**
 *	@brief  �~���[�W�J���V���b�g�f�[�^�쐬
 *
 *	@param	MUSICAL_SHOT_DATA *p_data   �󂯎�胏�[�N
 */
//-----------------------------------------------------------------------------
static inline void DEBUG_Br_MusicalLook_GetPhotData( MUSICAL_SHOT_DATA *shotData )
{ 
  u8 i;
  RTCDate date;
  GFL_RTC_GetDate( &date );
  shotData->bgNo = 1;
  shotData->spotBit = 2;
  shotData->year = date.year;
  shotData->month = date.month;
  shotData->day = date.day;
  shotData->title[0] = L'�|';
  shotData->title[1] = L'�P';
  shotData->title[2] = L'�b';
  shotData->title[3] = L'�^';
  shotData->title[4] = L'�[';
  shotData->title[5] = L'��';
  shotData->title[6] = L'��';
  shotData->title[7] = L'��';
  shotData->title[8] = L'�X';
  shotData->title[9] = L'�^';
  shotData->title[10] = L'�[';
  shotData->title[11] = L'��';
  shotData->title[12] = GFL_STR_GetEOMCode();

  shotData->shotPoke[0].monsno = MONSNO_PIKATYUU;
  shotData->shotPoke[1].monsno = MONSNO_PIKUSII;
  shotData->shotPoke[2].monsno = MONSNO_PURUNSU;
  shotData->shotPoke[3].monsno = MONSNO_ONOKKUSU;

  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    u8 j;
    shotData->shotPoke[i].trainerName[0] = L'�g';
    shotData->shotPoke[i].trainerName[1] = L'��';
    shotData->shotPoke[i].trainerName[2] = L'�[';
    shotData->shotPoke[i].trainerName[3] = L'�i';
    shotData->shotPoke[i].trainerName[4] = L'�P'+i;
    shotData->shotPoke[i].trainerName[5] = 0;

    for( j=0;j<MUSICAL_ITEM_EQUIP_MAX;j++ )
    {
      shotData->shotPoke[i].equip[j].itemNo = MUSICAL_ITEM_INVALID;
      shotData->shotPoke[i].equip[j].angle = 0;
      shotData->shotPoke[i].equip[j].equipPos = MUS_POKE_EQU_INVALID;
    }
    shotData->shotPoke[i].equip[0].itemNo = 0;
    shotData->shotPoke[i].equip[0].angle = 0;
    shotData->shotPoke[i].equip[0].equipPos = MUS_POKE_EQU_HAND_R;
  }
}

#endif //PM_DEBUG
