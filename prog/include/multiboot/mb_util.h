//======================================================================
/**
 * @file	mb_util_poke.h
 * @brief	�}���`�u�[�g ���[�e�B���e�B
 * @author	ariizumi
 * @data	09/12/06
 *
 * ���W���[�����FMB_UTIL
 */
//======================================================================
#pragma once

#include "multiboot/mb_data_def.h"
#include "poke_tool/poke_tool.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MUCPR_OK,
  MUCPR_EGG,    //�^�}�S�̓_��
  MUCPR_HIDEN,  //��`�������_��
  MUCPR_FUSEI,  //�s���I
  MUCPR_GIZAMIMI,  //�����݂݃s�`���[
}MB_UTIL_CHECK_PLAY_RET;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
extern const MB_UTIL_CHECK_PLAY_RET MB_UTIL_CheckPlay_PalGate( POKEMON_PASO_PARAM *ppp , const DLPLAY_CARD_TYPE cardType );
extern void MB_UTIL_ConvertPPP( const void *src , POKEMON_PASO_PARAM *dst , const DLPLAY_CARD_TYPE cardType );
extern void MB_UTIL_ConvertStr( const u16 *src , STRCODE *dst , const u8 dstLen , const DLPLAY_CARD_TYPE cardType );

