//============================================================================================
/**
 * @file    shinka_check.h
 * @bfief   �i���`�F�b�N�֘A�֐��Q
 * @author  HisashiSogabe
 * @date    09.12.02
 */
//============================================================================================
#pragma once

#include "poke_tool/pokeparty.h"

///<�|�P�����i���f�[�^�̍\����
typedef struct pokemon_shinka_data POKEMON_SHINKA_DATA;
typedef struct pokemon_shinka_table POKEMON_SHINKA_TABLE;

typedef enum{
  SHINKA_PARAM_COND=0,   //�i������
  SHINKA_PARAM_DATA,     //�i�������ɕt������f�[�^�i�A�C�e���i���Ȃ�A�C�e���i���o�[�Ȃǁj
  SHINKA_PARAM_MONS      //�i���|�P����
}SHINKA_PARAM;

typedef enum{
  SHINKA_TYPE_LEVELUP = 0,  //���x���A�b�v�ɂ��i��
  SHINKA_TYPE_TUUSHIN,      //�ʐM�ɂ��i��
  SHINKA_TYPE_ITEM,         //�A�C�e���ɂ��i��
  SHINKA_TYPE_ITEM_CHECK,   //�A�C�e���ɂ��i���i����炸�̂����𑕔����Ă��Ă��`�F�b�N�͂���j

  SHINKA_TYPE_MAX,

  SHINKA_COND_START = 0,
  SHINKA_COND_MAX = 7,
}SHINKA_TYPE;

//=========================================================================
//		�i��������`
//=========================================================================
typedef enum
{ 
  SHINKA_COND_NONE = 0,
  SHINKA_COND_FRIEND_HIGH,
  SHINKA_COND_FRIEND_HIGH_NOON,
  SHINKA_COND_FRIEND_HIGH_NIGHT,
  SHINKA_COND_LEVELUP,
  SHINKA_COND_TUUSHIN,
  SHINKA_COND_TUUSHIN_ITEM,
  SHINKA_COND_TUUSHIN_YUUGOU,
  SHINKA_COND_ITEM,
  SHINKA_COND_SPECIAL_POW,
  SHINKA_COND_SPECIAL_EVEN,
  SHINKA_COND_SPECIAL_DEF,
  SHINKA_COND_SPECIAL_RND_EVEN,
  SHINKA_COND_SPECIAL_RND_ODD,
  SHINKA_COND_SPECIAL_LEVELUP,
  SHINKA_COND_SPECIAL_NUKENIN,
  SHINKA_COND_SPECIAL_BEAUTIFUL,
  SHINKA_COND_ITEM_MALE,
  SHINKA_COND_ITEM_FEMALE,
  SHINKA_COND_SOUBI_NOON,
  SHINKA_COND_SOUBI_NIGHT,
  SHINKA_COND_WAZA,
  SHINKA_COND_POKEMON,
  SHINKA_COND_MALE,
  SHINKA_COND_FEMALE,
  SHINKA_COND_PLACE_TENGANZAN,
  SHINKA_COND_PLACE_KOKE,
  SHINKA_COND_PLACE_ICE,
  SHINKA_COND_PLACE_DENKIDOUKUTSU,
}SHINKA_COND;

#define	SHINKA_MAX	    ( 7 )	  //�i�������MAX
#define SHINKA_FRIEND   ( 220 ) //�i���ɕK�v�ȂȂ��x

extern  u16	        SHINKA_Check( POKEPARTY *ppt, POKEMON_PARAM *pp, SHINKA_TYPE type, u32 param, u8 season,
                                  SHINKA_COND *cond, HEAPID heapID );
extern  ARCHANDLE*  SHINKA_GetArcHandle( HEAPID heapID );
extern  u16         SHINKA_GetParamByHandle( ARCHANDLE* handle, int mons_no, int index, SHINKA_PARAM param );
