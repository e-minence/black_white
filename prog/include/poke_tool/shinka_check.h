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
  SHINKA_COND_LEVELUP = 0,  //���x���A�b�v�ɂ��i��
  SHINKA_COND_TUUSHIN,      //�ʐM�ɂ��i��
  SHINKA_COND_ITEM,         //�A�C�e���ɂ��i��
  SHINKA_COND_ITEM_CHECK,   //�A�C�e���ɂ��i���i����炸�̂����𑕔����Ă��Ă��`�F�b�N�͂���j
}SHINKA_COND;

extern  u16	        SHINKA_Check( POKEPARTY *ppt, POKEMON_PARAM *pp, SHINKA_COND cond, u16 param,
                                  SHINKA_COND *shinka_cond, HEAPID heapID );
extern  ARCHANDLE*  SHINKA_GetArcHandle( HEAPID heapID );
extern  u16         SHINKA_GetParamByHandle( ARCHANDLE* handle, int mons_no, int index, SHINKA_PARAM param );
