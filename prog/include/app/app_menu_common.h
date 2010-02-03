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

#include "poke_tool/poke_tool.h"
#include "poke_tool/poketype.h"
#include "item/item.h"

//==============================================================================
//	�萔��`
//==============================================================================
//------------------------------
//  �o�[�f��
//------------------------------
//�o�[�̃{�^���̃A�j���C���f�b�N�X
typedef enum
{
  APP_COMMON_BARICON_EXIT,
  APP_COMMON_BARICON_RETURN,
  APP_COMMON_BARICON_CURSOR_DOWN,
  APP_COMMON_BARICON_CURSOR_UP,
  APP_COMMON_BARICON_CURSOR_LEFT,
  APP_COMMON_BARICON_CURSOR_RIGHT,
  APP_COMMON_BARICON_CHECK_OFF,
  APP_COMMON_BARICON_CHECK_ON,

  APP_COMMON_BARICON_EXIT_ON,
  APP_COMMON_BARICON_RETURN_ON,
  APP_COMMON_BARICON_CURSOR_DOWN_ON,
  APP_COMMON_BARICON_CURSOR_UP_ON,
  APP_COMMON_BARICON_CURSOR_LEFT_ON,
  APP_COMMON_BARICON_CURSOR_RIGHT_ON,
  
  APP_COMMON_BARICON_EXIT_OFF,
  APP_COMMON_BARICON_RETURN_OFF,
  APP_COMMON_BARICON_CURSOR_DOWN_OFF,
  APP_COMMON_BARICON_CURSOR_UP_OFF,
  APP_COMMON_BARICON_CURSOR_LEFT_OFF,
  APP_COMMON_BARICON_CURSOR_RIGHT_OFF,
	APP_COMMON_BARICON_CHECK_NONE,

  APP_COMMON_BARICON_MAX,
}APP_COMMON_BARICON;

//�o�[�̃{�^���̃p���b�g�{��
#define APP_COMMON_BARICON_PLT_NUM	(3)

//�o�[�̃p���b�g�{��
#define APP_COMMON_BAR_PLT_NUM	(1)

//------------------------------
//  �A�C�e���A�C�R��
//------------------------------
#define APP_COMMON_ITEMICON_PLT_NUM	(1)
///�A�C�e���A�C�R���A�j���C���f�b�N�X
enum{
  APP_COMMON_ITEMICON_ITEM,   //�A�C�e���A�C�R��
  APP_COMMON_ITEMICON_MAIL,   //���[���A�C�R��
  APP_COMMON_ITEMICON_CUSTOMBALL,    //�J�X�^���{�[���A�C�R��
};


//�{�[���A�C�R���̃p���b�g�̖{��
#define APP_COMMON_BALL_PLT_NUM (1)

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

//�{�[���A�C�R���̃p���b�g�̖{��
#define APP_COMMON_BALL_PLT_NUM (1)

//�Z���̓ǂ݂킯�Ɏg���}�b�s���O���[�h��`
typedef enum
{
  APP_COMMON_MAPPING_32K,
  APP_COMMON_MAPPING_64K,
  APP_COMMON_MAPPING_128K,
}APP_COMMON_MAPPING;

// ��Ԉُ�A�C�R���̃A�j���ԍ�
enum {
	APP_COMMON_ST_ICON_POKERUS = 0,	// �|�P���X
	APP_COMMON_ST_ICON_MAHI,				// ���
	APP_COMMON_ST_ICON_KOORI,				// �X
	APP_COMMON_ST_ICON_NEMURI,			// ����
	APP_COMMON_ST_ICON_DOKU,				// ��
	APP_COMMON_ST_ICON_YAKEDO,			// �Ώ�
	APP_COMMON_ST_ICON_HINSI,				// �m��
	APP_COMMON_ST_ICON_NONE					// �Ȃ��i�A�j���ԍ��I�ɂ��Ȃ��j
};

// �|�P�}�[�N�̃A�j���ԍ�
enum {
  APP_COMMON_POKE_MARK_CIRCLE_WHITE,    //��
  APP_COMMON_POKE_MARK_CIRCLE_BLACK,    //��
  APP_COMMON_POKE_MARK_TRIANGLE_WHITE,  //��
  APP_COMMON_POKE_MARK_TRIANGLE_BLACK,  //�� 
  APP_COMMON_POKE_MARK_SQUARE_WHITE,    //��
  APP_COMMON_POKE_MARK_SQUARE_BLACK,    //��
  APP_COMMON_POKE_MARK_HEART_WHITE,     //�n�[�g(��)
  APP_COMMON_POKE_MARK_HEART_BLACK,     //�n�[�g(��)
  APP_COMMON_POKE_MARK_STAR_WHITE,      //��
  APP_COMMON_POKE_MARK_STAR_BLACK,      //��
  APP_COMMON_POKE_MARK_DIAMOND_WHITE,   //��
  APP_COMMON_POKE_MARK_DIAMOND_BLACK,   //��
  APP_COMMON_POKE_MARK_STAR_RED,        //��(�ԁE���A�p)
  APP_COMMON_POKE_MARK_POKERUSU,        //��(�|�P���X�����}�[�N)
};


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

//�����o�[�{�^���n�f�ގ擾
extern const u32 APP_COMMON_GetBarIconPltArcIdx( void );
extern const u8  APP_COMMON_GetBarIconPltOffset( const APP_COMMON_BARICON type );
extern const u32 APP_COMMON_GetBarIconCharArcIdx();
extern const u32 APP_COMMON_GetBarIconCellArcIdx( const APP_COMMON_MAPPING mapping );
extern const u32 APP_COMMON_GetBarIconAnimeArcIdx( const APP_COMMON_MAPPING mapping );

//�����o�[�f�ގ擾
extern const u32 APP_COMMON_GetBarPltArcIdx( void );
extern const u32 APP_COMMON_GetBarCharArcIdx( void );
extern const u32 APP_COMMON_GetBarScrnArcIdx( void );
extern const u32 APP_COMMON_GetBarScrn_512x256ArcIdx( void );

// �A�C�e���A�C�R���擾�i�|�P�A�C�R���p�j
extern const u32 APP_COMMON_GetPokeItemIconPltArcIdx( void );
extern const u32 APP_COMMON_GetPokeItemIconCharArcIdx( void );
extern const u32 APP_COMMON_GetPokeItemIconCellArcIdx( const APP_COMMON_MAPPING mapping );
extern const u32 APP_COMMON_GetPokeItemIconAnimeArcIdx( const APP_COMMON_MAPPING mapping ); 

//4 x 4 ��f�[�^�擾�i�e�N�X�`���]���p�j
extern const u32 APP_COMMON_GetNull4x4PltArcIdx( void );
extern const u32 APP_COMMON_GetNull4x4CharArcIdx( void );
extern const u32 APP_COMMON_GetNull4x4CellArcIdx( const APP_COMMON_MAPPING mapping );
extern const u32 APP_COMMON_GetNull4x4AnimeArcIdx( const APP_COMMON_MAPPING mapping );

// ��Ԉُ�A�C�R��
extern const u32 APP_COMMON_GetStatusIconPltArcIdx( void );
extern const u32 APP_COMMON_GetStatusIconCharArcIdx( void );
extern const u32 APP_COMMON_GetStatusIconCellArcIdx( const APP_COMMON_MAPPING mapping );
extern const u32 APP_COMMON_GetStatusIconAnimeArcIdx( const APP_COMMON_MAPPING mapping );
extern u32 APP_COMMON_GetStatusIconAnime( POKEMON_PARAM * pp );

//�|�P�����GOBJ�̂��߂̃_�~�[�Z���擾
extern u32 APP_COMMON_GetDummyCellArcIdx( const APP_COMMON_MAPPING mapping );
extern u32 APP_COMMON_GetDummyAnimeArcIdx( const APP_COMMON_MAPPING mapping );

//�g���[�i�[�GOBJ�̂��߂̃_�~�[�Z���擾
extern u32 APP_COMMON_GetTrDummyCellArcIdx( const APP_COMMON_MAPPING mapping );
extern u32 APP_COMMON_GetTrDummyAnimeArcIdx( const APP_COMMON_MAPPING mapping );

//�{�[���A�C�R���擾
//2*2�L�����̃A�C�R���ł��B�傫�����̓A�C�e���A�C�R������擾���Ă�������
extern u32 APP_COMMON_GetBallPltArcIdx( const BALL_ID ball );
extern u32 APP_COMMON_GetBallCharArcIdx( const BALL_ID ball );
extern u32 APP_COMMON_GetBallCellArcIdx( const BALL_ID ball, const APP_COMMON_MAPPING mapping );
extern u32 APP_COMMON_GetBallAnimeArcIdx( const BALL_ID ball, const APP_COMMON_MAPPING mapping );

//�}�[�N�A�C�R���擾
extern u32 APP_COMMON_GetPokeMarkPltArcIdx( void );
extern u32 APP_COMMON_GetPokeMarkCharArcIdx( const APP_COMMON_MAPPING mapping );
extern u32 APP_COMMON_GetPokeMarkCellArcIdx( const APP_COMMON_MAPPING mapping );
extern u32 APP_COMMON_GetPokeMarkAnimeArcIdx( const APP_COMMON_MAPPING mapping );

//HP�o�[�y��
const u32 APP_COMMON_GetHPBarBasePltArcIdx( void );
const u32 APP_COMMON_GetHPBarBaseCharArcIdx( const APP_COMMON_MAPPING mapping );
const u32 APP_COMMON_GetHPBarBaseCellArcIdx( const APP_COMMON_MAPPING mapping );
const u32 APP_COMMON_GetHPBarBaseAnimeArcIdx( const APP_COMMON_MAPPING mapping );

//�|�P���X�擾
extern u32 APP_COMMON_GetPokerusPltArcIdx( void );
extern u32 APP_COMMON_GetPokerusCharArcIdx( const APP_COMMON_MAPPING mapping );
extern u32 APP_COMMON_GetPokerusCellArcIdx( const APP_COMMON_MAPPING mapping );
extern u32 APP_COMMON_GetPokerusAnimeArcIdx( const APP_COMMON_MAPPING mapping );
