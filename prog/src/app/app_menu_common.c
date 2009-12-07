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

//�}���`�u�[�g�p���蕪��(MB���̂ݓǂݍ���
#ifdef MULTI_BOOT_MAKE  //DL�q�@������
#include "app_menu_common_dl.naix"
#endif //MULTI_BOOT_MAKE


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
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
	return NARC_app_menu_common_bar_button_NCLR;
#else                    //DL�q�@������
	return NARC_app_menu_common_dl_bar_button_NCLR;
#endif //MULTI_BOOT_MAKE
}
const u8  APP_COMMON_GetBarIconPltOffset( const APP_COMMON_BARICON type )
{	
	GF_ASSERT( type < APP_COMMON_BARICON_MAX );
	return BarIconPlttOffset[ type ];
}
const u32 APP_COMMON_GetBarIconCharArcIdx()
{	
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
	return NARC_app_menu_common_bar_button_128k_NCGR;
#else                    //DL�q�@������
	return NARC_app_menu_common_dl_bar_button_128k_NCGR;
#endif //MULTI_BOOT_MAKE
}
const u32 APP_COMMON_GetBarIconCellArcIdx( const APP_COMMON_MAPPING mapping )
{	
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
	return NARC_app_menu_common_bar_button_32k_NCER + mapping;
#else                    //DL�q�@������
	GF_ASSERT( mapping == APP_COMMON_MAPPING_128K );
	return NARC_app_menu_common_dl_bar_button_128k_NCER;
#endif //MULTI_BOOT_MAKE
}
const u32 APP_COMMON_GetBarIconAnimeArcIdx( const APP_COMMON_MAPPING mapping )
{	
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
	return NARC_app_menu_common_bar_button_32k_NANR + mapping;
#else                    //DL�q�@������
	GF_ASSERT( mapping == APP_COMMON_MAPPING_128K );
	return NARC_app_menu_common_dl_bar_button_128k_NANR;
#endif //MULTI_BOOT_MAKE
}
//--------------------------------------------------------------
//�����o�[�n�f�ގ擾
//--------------------------------------------------------------
const u32 APP_COMMON_GetBarPltArcIdx( void )
{	
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
	return NARC_app_menu_common_menu_bar_NCLR;
#else                    //DL�q�@������
	return NARC_app_menu_common_dl_menu_bar_NCLR;
#endif //MULTI_BOOT_MAKE
}
const u32 APP_COMMON_GetBarCharArcIdx( void )
{	
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
	return NARC_app_menu_common_menu_bar_NCGR;
#else                    //DL�q�@������
	return NARC_app_menu_common_dl_menu_bar_NCGR;
#endif //MULTI_BOOT_MAKE
}
const u32 APP_COMMON_GetBarScrnArcIdx( void )
{	
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
	return NARC_app_menu_common_menu_bar_NSCR;
#else                    //DL�q�@������
	return NARC_app_menu_common_dl_menu_bar_NSCR;
#endif //MULTI_BOOT_MAKE
}
const u32 APP_COMMON_GetBarScrn_512x256ArcIdx( void )
{	
	return NARC_app_menu_common_menu_bar_512x256_NSCR;
}

//--------------------------------------------------------------
// �A�C�e���A�C�R��(�|�P�A�C�R���p)
//--------------------------------------------------------------

const u32 APP_COMMON_GetPokeItemIconPltArcIdx( void )
{	
	return NARC_app_menu_common_item_icon_NCLR;
}
const u32 APP_COMMON_GetPokeItemIconCharArcIdx( void )
{	
	return NARC_app_menu_common_item_icon_NCGR;
}
const u32 APP_COMMON_GetPokeItemIconCellArcIdx( const APP_COMMON_MAPPING mapping )
{	
	return NARC_app_menu_common_item_icon_32k_NCER + mapping;
}
const u32 APP_COMMON_GetPokeItemIconAnimeArcIdx( const APP_COMMON_MAPPING mapping )
{	
	return NARC_app_menu_common_item_icon_32k_NANR + mapping;
}

//--------------------------------------------------------------
//4 x 4 ��f�[�^�擾�i�e�N�X�`���]���p�j
//--------------------------------------------------------------
const u32 APP_COMMON_GetNull4x4PltArcIdx( void )
{	
	return NARC_app_menu_common_null_4x4_NCLR;
}
const u32 APP_COMMON_GetNull4x4CharArcIdx( void )
{	
	return NARC_app_menu_common_null_4x4_128k_NCGR;
}
const u32 APP_COMMON_GetNull4x4CellArcIdx( const APP_COMMON_MAPPING mapping )
{	
  return NARC_app_menu_common_null_4x4_32k_NCER + mapping;
}
const u32 APP_COMMON_GetNull4x4AnimeArcIdx( const APP_COMMON_MAPPING mapping )
{	
  return NARC_app_menu_common_null_4x4_32k_NANR + mapping;
}

//--------------------------------------------------------------
//��Ԉُ�A�C�R���f�[�^�擾
//--------------------------------------------------------------
const u32 APP_COMMON_GetStatusIconPltArcIdx( void )
{
  return NARC_app_menu_common_p_st_ijou_NCLR;
}
const u32 APP_COMMON_GetStatusIconCharArcIdx( void )
{
  return NARC_app_menu_common_p_st_ijou_NCGR;
}
const u32 APP_COMMON_GetStatusIconCellArcIdx( const APP_COMMON_MAPPING mapping )
{
  return NARC_app_menu_common_p_st_ijou_32k_NCER + mapping;
}
const u32 APP_COMMON_GetStatusIconAnimeArcIdx( const APP_COMMON_MAPPING mapping )
{
  return NARC_app_menu_common_p_st_ijou_32k_NANR + mapping;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		��Ԉُ�A�C�R���̃A�j���ԍ����擾
 *
 * @param		pp		POKEMON_PARAM
 *
 * @return	�A�j���ԍ�
 *
 * @li	�|�P���X�̓`�F�b�N���Ă܂���
 */
//--------------------------------------------------------------------------------------------
u32 APP_COMMON_GetStatusIconAnime( POKEMON_PARAM * pp )
{
	// �m��
	if( PP_Get( pp, ID_PARA_hp, NULL ) == 0 ){
		return APP_COMMON_ST_ICON_HINSI;
	}else{
		u32	prm = PP_Get( pp, ID_PARA_condition, NULL );

		// ��
		if( ( prm & (PTL_CONDITION_DOKU|PTL_CONDITION_DOKUDOKU) ) != 0 ){
			return APP_COMMON_ST_ICON_DOKU;
		// ����
		}else if( ( prm & PTL_CONDITION_NEMURI ) != 0 ){
			return APP_COMMON_ST_ICON_NEMURI;
		// �Ώ�
		}else if( ( prm & PTL_CONDITION_YAKEDO ) != 0 ){
			return APP_COMMON_ST_ICON_YAKEDO;
		// �X
		}else if( ( prm & PTL_CONDITION_KOORI ) != 0 ){
			return APP_COMMON_ST_ICON_KOORI;
		// ���
		}else if( ( prm & PTL_CONDITION_MAHI ) != 0 ){
			return APP_COMMON_ST_ICON_MAHI;
		}
		// �Ȃ�
		return APP_COMMON_ST_ICON_NONE;
	}
}
//=============================================================================
/**
 *	�|�P�����GOBJ�̂��߂̃_�~�[�Z��
 */
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief	�_�~�[�Z���A�[�J�C�u�C���f�b�N�X
 *
 *	@param	const APP_COMMON_MAPPING mapping �}�b�s���O���[�h
 *
 *	@return	���\�[�X�ԍ�
 */
//-----------------------------------------------------------------------------
u32 APP_COMMON_GetDummyCellArcIdx( const APP_COMMON_MAPPING mapping )
{	
	return NARC_app_menu_common_poke2dgra_dummy_32k_NCER + mapping;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�_�~�[�A�j���A�[�J�C�u�C���f�b�N�X�擾
 *
 *	@param	const APP_COMMON_MAPPING mapping	�}�b�s���O���[�h
 *
 *	@return	���\�[�X�ԍ�
 */
//-----------------------------------------------------------------------------
u32 APP_COMMON_GetDummyAnimeArcIdx( const APP_COMMON_MAPPING mapping )
{	
	return NARC_app_menu_common_poke2dgra_dummy_32k_NANR + mapping;
}
//=============================================================================
/**
 *	�{�[���A�C�R��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g���\�[�X�ԍ��擾
 *
 *	@param	const BALL_ID ball					�{�[��ID
 *
 *	@return	�p���b�g�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 APP_COMMON_GetBallPltArcIdx( const BALL_ID ball )
{	
	GF_ASSERT_MSG( ball != BALLID_NULL, "�{�[��ID��NULL�ł�\n" );
	return NARC_app_menu_common_ball00_NCLR + ball - 1;	//BALLID��1�I���W��
}
//----------------------------------------------------------------------------
/**
 *	@brief	�L�������\�[�X�ԍ��擾
 *
 *	@param	const BALL_ID ball					�{�[��ID
 *
 *	@return	�L�����A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 APP_COMMON_GetBallCharArcIdx( const BALL_ID ball )
{	
	GF_ASSERT_MSG( ball != BALLID_NULL, "�{�[��ID��NULL�ł�\n" );
	return NARC_app_menu_common_ball00_NCGR + ball - 1;	//BALLID�͂P�I���W��
}
//----------------------------------------------------------------------------
/**
 *	@brief	�Z�����\�[�X�ԍ��擾
 *
 *	@param	const BALL_ID ball					�{�[��ID
 *	@param	APP_COMMON_MAPPING mapping	�}�b�s���O���[�h
 *
 *	@return	�Z���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 APP_COMMON_GetBallCellArcIdx( const BALL_ID ball, const APP_COMMON_MAPPING mapping )
{	
	GF_ASSERT_MSG( ball != BALLID_NULL, "�{�[��ID��NULL�ł�\n" );
	return NARC_app_menu_common_ball_32k_NCER + mapping;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j�����\�[�X�ԍ��擾
 *
 *	@param	const BALL_ID ball					�{�[��ID
 *	@param	APP_COMMON_MAPPING mapping	�}�b�s���O���[�h
 *
 *	@return	�p���b�g�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 APP_COMMON_GetBallAnimeArcIdx( const BALL_ID ball, const APP_COMMON_MAPPING mapping )
{	
	GF_ASSERT_MSG( ball != BALLID_NULL, "�{�[��ID��NULL�ł�\n" );
	return NARC_app_menu_common_ball_32k_NANR + mapping;
}
//=============================================================================
/**
 *	�}�[�N
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g���\�[�X�ԍ��擾
 *
 *	@return	�p���b�g�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 APP_COMMON_GetPokeMarkPltArcIdx( void )
{	
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
	return NARC_app_menu_common_poke_mark_NCLR;
#else                    //DL�q�@������
	return NARC_app_menu_common_dl_poke_mark_NCLR;
#endif //MULTI_BOOT_MAKE
}
//----------------------------------------------------------------------------
/**
 *	@brief	�L�������\�[�X�ԍ��擾
 *
 *	@param	APP_COMMON_MAPPING mapping	�}�b�s���O���[�h
 *
 *	@return	�L�����A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 APP_COMMON_GetPokeMarkCharArcIdx( const APP_COMMON_MAPPING mapping )
{	
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
	return NARC_app_menu_common_poke_mark_32k_NCGR + mapping;
#else                    //DL�q�@������
	GF_ASSERT( mapping == APP_COMMON_MAPPING_128K );
	return NARC_app_menu_common_dl_poke_mark_128k_NCGR;
#endif //MULTI_BOOT_MAKE
}
//----------------------------------------------------------------------------
/**
 *	@brief	�Z�����\�[�X�ԍ��擾
 *
 *	@param	APP_COMMON_MAPPING mapping	�}�b�s���O���[�h
 *
 *	@return	�Z���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 APP_COMMON_GetPokeMarkCellArcIdx( const APP_COMMON_MAPPING mapping )
{	
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
	return NARC_app_menu_common_poke_mark_32k_NCER + mapping;
#else                    //DL�q�@������
	GF_ASSERT( mapping == APP_COMMON_MAPPING_128K );
	return NARC_app_menu_common_dl_poke_mark_128k_NCER;
#endif //MULTI_BOOT_MAKE
}
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j�����\�[�X�ԍ��擾
 *
 *	@param	APP_COMMON_MAPPING mapping	�}�b�s���O���[�h
 *
 *	@return	�p���b�g�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 APP_COMMON_GetPokeMarkAnimeArcIdx( const APP_COMMON_MAPPING mapping )
{	
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
	return NARC_app_menu_common_poke_mark_32k_NANR + mapping;
#else                    //DL�q�@������
	GF_ASSERT( mapping == APP_COMMON_MAPPING_128K );
	return NARC_app_menu_common_dl_poke_mark_128k_NANR;
#endif //MULTI_BOOT_MAKE
}

//--------------------------------------------------------------
//HP�o�[�f�[�^�擾
//--------------------------------------------------------------
const u32 APP_COMMON_GetHPBarBasePltArcIdx( void )
{
  return NARC_app_menu_common_hp_bar_NCLR;
}
const u32 APP_COMMON_GetHPBarBaseCharArcIdx( const APP_COMMON_MAPPING mapping )
{
  return NARC_app_menu_common_hp_dodai_32k_NCGR + mapping;
}
const u32 APP_COMMON_GetHPBarBaseCellArcIdx( const APP_COMMON_MAPPING mapping )
{
  return NARC_app_menu_common_hp_dodai_32k_NCER + mapping;
}
const u32 APP_COMMON_GetHPBarBaseAnimeArcIdx( const APP_COMMON_MAPPING mapping )
{
  return NARC_app_menu_common_hp_dodai_32k_NANR + mapping;
}


//--------------------------------------------------------------
//�|�P���X
//--------------------------------------------------------------
u32 APP_COMMON_GetPokerusPltArcIdx( void )
{
  return NARC_app_menu_common_pokerus_NCLR;
}
u32 APP_COMMON_GetPokerusCharArcIdx( const APP_COMMON_MAPPING mapping )
{
  return NARC_app_menu_common_pokerus_32k_NCGR + mapping;
}
u32 APP_COMMON_GetPokerusCellArcIdx( const APP_COMMON_MAPPING mapping )
{
  return NARC_app_menu_common_pokerus_32k_NCER + mapping;
}
u32 APP_COMMON_GetPokerusAnimeArcIdx( const APP_COMMON_MAPPING mapping )
{
  return NARC_app_menu_common_pokerus_32k_NANR + mapping;
}

