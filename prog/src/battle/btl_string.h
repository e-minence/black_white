//=============================================================================================
/**
 * @file	btl_string.h
 * @brief	�|�P����WB �o�g�� �����񐶐�����
 * @author	taya
 *
 * @date	2008.10.06	�쐬
 */
//=============================================================================================
#ifndef __BTL_STRING_H__
#define __BTL_STRING_H__

#include "btl_main.h"
#include "btl_client.h"
#include "btl_pokeparam.h"


extern void BTL_STR_InitSystem( const BTL_MAIN_MODULE* mainModule, const BTL_CLIENT* client, HEAPID heapID );
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

extern void BTL_STR_MakeStringStd( STRBUF* buf, BtlStrID_STD strID );

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
 * @param   strID		
 * @param   args		
 *
 */
//=============================================================================================
extern void BTL_STR_MakeStringWaza( STRBUF* buf, BtlPokePos pokePos, u16 waza );





extern void BTL_STR_GetUIString( STRBUF* dst, u16 strID );
extern void BTL_STR_MakeWazaUIString( STRBUF* dst, u16 wazaID, u8 wazaPP, u8 wazaPPMax );


extern void BTL_STR_MakeStatusWinStr( STRBUF* dst, const BTL_POKEPARAM* bpp, u16 hp );




//----------------

extern u16 BTL_STR_GetRankUpStrID( u8 statusType, u8 volume );
extern u16 BTL_STR_GetRankDownStrID( u8 statusType, u8 volume );


#endif

