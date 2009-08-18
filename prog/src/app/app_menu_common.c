//======================================================================
/**
 * @file	app_menu_common.c
 * @brief	���ʑf�ފ֌W�\�[�X
 * @author	ariizumi
 * @data	09/07/14
 *
 * ���W���[�����FAPP_COMMON
 */
//======================================================================
#include <gflib.h>

#include "arc_def.h"
#include "app_menu_common.naix"

#include "app/app_menu_common.h"

///�Z�^�C�v���̃p���b�g�I�t�Z�b�g�ԍ�
ALIGN4 static const u8 PokeTypePlttOffset[] = {
	0,	//POKETYPE_NORMAL
	0,	//POKETYPE_BATTLE
	1,	//POKETYPE_HIKOU
	1,	//POKETYPE_POISON
	0,	//POKETYPE_JIMEN
	0,	//POKETYPE_IWA
	2,	//POKETYPE_MUSHI
	1,	//POKETYPE_GHOST
	0,	//POKETYPE_METAL
	2,	//POKETYPE_HATE
	0,	//POKETYPE_FIRE
	1,	//POKETYPE_WATER
	2,	//POKETYPE_KUSA
	0,	//POKETYPE_ELECTRIC
	1,	//POKETYPE_SP
	1,	//POKETYPE_KOORI
	2,	//POKETYPE_DRAGON
	0,	//POKETYPE_AKU
	0,	//ICONTYPE_STYLE,			///<�i�D�悳
	1,	//ICONTYPE_BEAUTIFUL,		///<������
	1,	//ICONTYPE_CUTE,			///<����
	2,	//ICONTYPE_INTELLI,		///<����
	0,	//ICONTYPE_STRONG,		///<痂���
};

///���ޖ��̃p���b�g�I�t�Z�b�g�ԍ�
ALIGN4 static const u8 WazaKindPlttOffset[] = {
  0,    //KIND_HENNKA
  0,    //KIND_BUTSURI
  1,    //KIND_TOKUSYU
};

//�o�[�{�^���p�p���b�g�I�t�Z�b�g
ALIGN4 static const u8 BarIconPlttOffset[]	= {	
	0,//  APP_COMMON_BARICON_EXIT,
	0,//  APP_COMMON_BARICON_RETURN,
	1,//  APP_COMMON_BARICON_CURSOR_DOWN,
	1,//  APP_COMMON_BARICON_CURSOR_UP,
	1,//  APP_COMMON_BARICON_CURSOR_LEFT,
	1,//  APP_COMMON_BARICON_CURSOR_RIGHT,
	1,//  APP_COMMON_BARICON_CHECK_ON,
	1,//  APP_COMMON_BARICON_CHECK_OFF,
  
};

//--------------------------------------------------------------
//	ARCID�̎擾
//--------------------------------------------------------------
const u32 APP_COMMON_GetArcId(void)
{
  return ARCID_APP_MENU_COMMON;
}

//--------------------------------------------------------------
//	�^�C�v�n�f�ގ擾
//--------------------------------------------------------------
const u32 APP_COMMON_GetPokeTypePltArcIdx( void )
{
  return NARC_app_menu_common_p_st_type_NCLR;
}

const u8  APP_COMMON_GetPokeTypePltOffset( const PokeType type )
{
  return PokeTypePlttOffset[type];
}

const u32 APP_COMMON_GetPokeTypeCharArcIdx( const PokeType type )
{
  return NARC_app_menu_common_p_st_type_normal_NCGR + type;
}

const u32 APP_COMMON_GetPokeTypeCellArcIdx( const APP_COMMON_MAPPING mapping )
{
  return NARC_app_menu_common_p_st_type_32k_NCER + mapping;
}

const u32 APP_COMMON_GetPokeTypeAnimeArcIdx( const APP_COMMON_MAPPING mapping )
{
  return NARC_app_menu_common_p_st_type_32k_NANR + mapping;
}

//--------------------------------------------------------------
//�Z���ތn�f�ގ擾(�p���b�g�E�Z���E�A�j���̓^�C�v�Ƌ���
//--------------------------------------------------------------
const u8  APP_COMMON_GetWazaKindPltOffset( const PokeType type )
{
  return WazaKindPlttOffset[type];
}

const u32 APP_COMMON_GetWazaKindCharArcIdx( const PokeType type )
{
  return NARC_app_menu_common_p_st_bunrui_henka_NCGR + type;
}

//--------------------------------------------------------------
//�����o�[�{�^���n�f�ގ擾
//--------------------------------------------------------------
const u32 APP_COMMON_GetBarIconPltArcIdx( void )
{	
	return NARC_app_menu_common_bar_button_NCLR;
}
const u8  APP_COMMON_GetBarIconPltOffset( const APP_COMMON_BARICON type )
{	
	GF_ASSERT( type < APP_COMMON_BARICON_MAX );
	return BarIconPlttOffset[ type ];
}
const u32 APP_COMMON_GetBarIconCharArcIdx()
{	
	return NARC_app_menu_common_bar_button_128k_NCGR;
}
const u32 APP_COMMON_GetBarIconCellArcIdx( const APP_COMMON_MAPPING mapping )
{	
	return NARC_app_menu_common_bar_button_32k_NCER + mapping;
}
const u32 APP_COMMON_GetBarIconAnimeArcIdx( const APP_COMMON_MAPPING mapping )
{	
	return NARC_app_menu_common_bar_button_32k_NANR + mapping;
}
//--------------------------------------------------------------
//�����o�[�n�f�ގ擾
//--------------------------------------------------------------
const u32 APP_COMMON_GetBarPltArcIdx( void )
{	
	return NARC_app_menu_common_menu_bar_NCLR;
}
const u32 APP_COMMON_GetBarCharArcIdx( void )
{	
	return NARC_app_menu_common_menu_bar_NCGR;
}
const u32 APP_COMMON_GetBarScrnArcIdx( void )
{	
	return NARC_app_menu_common_menu_bar_NSCR;
}
