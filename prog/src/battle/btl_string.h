//=============================================================================================
/**
 * @file  btl_string.h
 * @brief �|�P����WB �o�g�� �����񐶐�����
 * @author  taya
 *
 * @date  2008.10.06  �쐬
 */
//=============================================================================================
#pragma once

#include "btl_main_def.h"
#include "btl_client.h"
#include "btl_pokeparam.h"

typedef enum {
  BTL_STRTYPE_NULL = 0,
  BTL_STRTYPE_STD,
  BTL_STRTYPE_SET,
  BTL_STRTYPE_UI,
  BTL_STRTYPE_WAZA,
  BTL_STRTYPE_WAZAOBOE,
  BTL_STRTYPE_YESNO,
}BtlStrType;

enum {
  BTL_STR_ARG_MAX = 8,
};


extern void BTL_STR_InitSystem( const BTL_MAIN_MODULE* mainModule, const BTL_CLIENT* client, const BTL_POKE_CONTAINER* pokeCon, HEAPID heapID );
extern void BTL_STR_QuitSystem( void );

//---------------------------------------------------------------------------------------
/**
 * �W�����b�Z�[�W�̐���
 *
 * �W�����b�Z�[�W�F�Ώێ҂Ȃ��A���邢�͑Ώۂ��N�ł����Ă����̃t�H�[�}�b�g�Ő�������镶����
 */
//---------------------------------------------------------------------------------------

#include "msg/msg_btl_std.h"

enum {
  BTL_STRID_STD_MAX = msg_btl_std_max,
};

typedef u16 BtlStrID_STD;

extern void BTL_STR_MakeStringStd( STRBUF* buf, BtlStrID_STD strID, u32 numArgs, ... );
extern void BTL_STR_MakeStringStdWithArgArray( STRBUF* buf, BtlStrID_STD strID, const int* array );


//---------------------------------------------------------------------------------------
/**
 * �Z�b�g���b�Z�[�W�̐���
 *
 * �Z�b�g���b�Z�[�W�F�������E�G���i�₹���́j�E�G���i�����Ắj���K���R�Z�b�g�ɂȂ��Ă��镶����
 * ID�́A�������̂��̂��w�肷��Ηǂ��B
 */
//---------------------------------------------------------------------------------------

#include "msg/msg_btl_set.h"

typedef u16 BtlStrID_SET;

extern void BTL_STR_MakeStringSet( STRBUF* buf, BtlStrID_SET strID, const int* args );


//=============================================================================================
/**
 * ���U���b�Z�[�W�̐���
 * �����U���b�Z�[�W�F�����́~�~���������I�Ƃ��B�Z�b�g���b�Z�[�W�Ɠ��l�A�K���R�Z�b�g�B
 *
 * @param   buf
 * @param   pokeID
 * @param   waza
 *
 * @retval  extern void
 */
//=============================================================================================
extern void BTL_STR_MakeStringWaza( STRBUF* buf, u8 pokeID, u16 waza );





#include "msg/msg_btl_ui.h"

extern void BTL_STR_GetUIString( STRBUF* dst, u16 strID );
extern void BTL_STR_MakeWazaUIString( STRBUF* dst, u16 wazaID, u8 wazaPP, u8 wazaPPMax );


//---------------------------------------------------------------------------------------
/**
 * �Z�o�����b�Z�[�W�̐���
 *
 * �W�����b�Z�[�W�F�Ώێ҂Ȃ��A���邢�͑Ώۂ��N�ł����Ă����̃t�H�[�}�b�g�Ő�������镶����
 */
//---------------------------------------------------------------------------------------

#include "msg/msg_waza_oboe.h"

enum {
  BTL_STRID_WAZAOBOE_MAX = msg_waza_oboe_max,
};

typedef u16 BtlStrID_WAZAOBOE;

extern void BTL_STR_MakeStringWazaOboeWithArgArray( STRBUF* buf, BtlStrID_WAZAOBOE strID, const int* array );


//---------------------------------------------------------------------------------------
/**
 * �͂��^���������b�Z�[�W�̐���
 *
 * �W�����b�Z�[�W�F�Ώێ҂Ȃ��A���邢�͑Ώۂ��N�ł����Ă����̃t�H�[�}�b�g�Ő�������镶����
 */
//---------------------------------------------------------------------------------------

#include "msg/msg_yesnomenu.h"

enum {
  BTL_STRID_YESNO_MAX = msg_yesnomenu_max,
  BTL_YESNO_MSG_LENGTH = 32,    //�͂��^���������b�Z�[�W�̒���
};

typedef u16 BtlStrID_YESNO;

extern void BTL_STR_MakeStringYesNoWithArgArray( STRBUF* buf, BtlStrID_YESNO strID, const int* array );

//----------------------------------------------------------------------
// ���x���A�b�v�E�B���h�E�e�L�X�g����
//----------------------------------------------------------------------
extern void BTL_STR_MakeString_LvupInfo_Diff( STRBUF* buf, u32 hp, u32 atk, u32 def, u32 sp_atk, u32 sp_def, u32 agi );
extern void BTL_STR_MakeString_LvupInfo_Param( STRBUF* buf, u32 hp, u32 atk, u32 def, u32 sp_atk, u32 sp_def, u32 agi );

