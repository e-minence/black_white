//======================================================================
/**
 * @file	app_menu_common.h
 * @brief	���ʑf�ފ֌W�\�[�X
 * @author	ariizumi
 * @data	09/07/14
 *
 * ���W���[�����FAPP_COMMON
 */
//======================================================================

#pragma once

#include "poke_tool/poketype.h"

//==============================================================================
//	�萔��`
//==============================================================================
//�o�[�̃{�^���̃A�j���C���f�b�N�X
enum
{
  APP_COMMON_BARICON_CLOSE,
  APP_COMMON_BARICON_RETURN,
  APP_COMMON_BARICON_CURSOR_DOWN,
  APP_COMMON_BARICON_CURSOR_UP,
  APP_COMMON_BARICON_CURSOR_LEFT,
  APP_COMMON_BARICON_CURSOR_RIGHT,
  APP_COMMON_BARICON_CHECK_ON,
  APP_COMMON_BARICON_CHECK_OFF,
  
  APP_COMMON_BARICON_MAX,
};


///�A�C�R���^�C�v(�Z�^�C�v�ȊO�̃^�C�v)
enum{
  ICONTYPE_STYLE = POKETYPE_MAX,  ///<�i�D�悳
  ICONTYPE_BEAUTIFUL,             ///<������
  ICONTYPE_CUTE,                  ///<����
  ICONTYPE_INTELLI,               ///<����
  ICONTYPE_STRONG,                ///<痂���
};

//�|�P�����^�C�v�̃p���b�g�̖{��
#define APP_COMMON_POKETYPE_PLT_NUM (3)

//�Z���̓ǂ݂킯�Ɏg���}�b�s���O���[�h��`
typedef enum
{
  APP_COMMON_MAPPING_32K,
  APP_COMMON_MAPPING_64K,
  APP_COMMON_MAPPING_128K,
}APP_COMMON_MAPPING;

//ARCID�̎擾
extern const u32 APP_COMMON_GetArcId(void);

//�^�C�v�n�f�ގ擾
extern const u32 APP_COMMON_GetPokeTypePltArcIdx( void );
extern const u8  APP_COMMON_GetPokeTypePltOffset( const PokeType type );
extern const u32 APP_COMMON_GetPokeTypeCharArcIdx( const PokeType type );
extern const u32 APP_COMMON_GetPokeTypeCellArcIdx( const APP_COMMON_MAPPING mapping );
extern const u32 APP_COMMON_GetPokeTypeAnimeArcIdx( const APP_COMMON_MAPPING mapping );

//�Z���ތn�f�ގ擾(�p���b�g�E�Z���E�A�j���̓^�C�v�Ƌ���
extern const u8  APP_COMMON_GetWazaKindPltOffset( const PokeType type );
extern const u32 APP_COMMON_GetWazaKindCharArcIdx( const PokeType type );

