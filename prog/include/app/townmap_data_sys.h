//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		townmap_data_sys.h
 *	@brief	�^�E���}�b�v�f�[�^�ǂݍ���
 *	@author	Toru=Nagihashi
 *	@date		2009.07.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "townmap_data.h"
#include "townmap_replace.cdat"  //arc/

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�^�E���}�b�v�p�p�����[�^
//=====================================
typedef enum 
{
	TOWNMAP_DATA_PARAM_ZONE_ID,
	TOWNMAP_DATA_PARAM_ZONE_SEARCH, //����Ȃ�����
	TOWNMAP_DATA_PARAM_POS_X,
	TOWNMAP_DATA_PARAM_POS_Y,
	TOWNMAP_DATA_PARAM_CURSOR_X,
	TOWNMAP_DATA_PARAM_CURSOR_Y,
	TOWNMAP_DATA_PARAM_HIT_START_X,
	TOWNMAP_DATA_PARAM_HIT_START_Y,
	TOWNMAP_DATA_PARAM_HIT_END_X,
	TOWNMAP_DATA_PARAM_HIT_END_Y,
	TOWNMAP_DATA_PARAM_HIT_WIDTH,
	TOWNMAP_DATA_PARAM_PLACE_TYPE,
	TOWNMAP_DATA_PARAM_SKY_FLAG,
	TOWNMAP_DATA_PARAM_WARP_X,
	TOWNMAP_DATA_PARAM_WARP_Y,
	TOWNMAP_DATA_PARAM_ARRIVE_FLAG,
	TOWNMAP_DATA_PARAM_HIDE_FLAG,
	TOWNMAP_DATA_PARAM_GUIDE_MSGID,
	TOWNMAP_DATA_PARAM_PLACE1_MSGID,
	TOWNMAP_DATA_PARAM_PLACE2_MSGID,
	TOWNMAP_DATA_PARAM_PLACE3_MSGID,
	TOWNMAP_DATA_PARAM_PLACE4_MSGID,
	TOWNMAP_DATA_PARAM_PLACE5_MSGID,
	TOWNMAP_DATA_PARAM_PLACE6_MSGID,
	TOWNMAP_DATA_PARAM_ZKN_ANM,
	TOWNMAP_DATA_PARAM_ZKN_POS_X,
	TOWNMAP_DATA_PARAM_ZKN_POS_Y,

	TOWNMAP_DATA_PARAM_MAX,
} TOWNMAP_DATA_PARAM;


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�^�E���}�b�v�f�[�^
//=====================================
typedef struct _TOWNMAP_DATA TOWNMAP_DATA;

//=============================================================================
/**
 *					�^�E���}�b�v�f�[�^�f�[�^�擾
*/
//=============================================================================
extern TOWNMAP_DATA *TOWNMAP_DATA_Alloc( HEAPID heapID );
extern void TOWNMAP_DATA_Free( TOWNMAP_DATA *p_wk );

extern u16 TOWNMAP_DATA_GetParam( const TOWNMAP_DATA *cp_wk, u16 idx, TOWNMAP_DATA_PARAM param );

//field_townmap.h��FIELD_TOWNMAP_GetRootZoneID�֐��œ���ZONEID���ȉ��ɓn����
//��L�^�E���}�b�v�f�[�^��idx��Ԃ�
extern u16 TOWNMAP_DATA_SearchRootZoneID( const TOWNMAP_DATA *cp_wk, u16 zoneID );



//=============================================================================
/**
 *					�u�������f�[�^�擾
*/
//=============================================================================
//-------------------------------------
///	�u���������
//=====================================
#if 0 //cdat���������ߕK�v���Ȃ��Ȃ�܂���
typedef struct _TOWNMAP_REPLACE_DATA TOWNMAP_REPLACE_DATA;

extern TOWNMAP_REPLACE_DATA *TOWNMAP_REPLACE_DATA_Alloc( HEAPID heapID );
extern void TOWNMAP_REPLACE_DATA_Free( TOWNMAP_REPLACE_DATA *p_wk );
extern u16 TOWNMAP_REPLACE_DATA_GetReplace( const TOWNMAP_REPLACE_DATA *cp_wk, u16 zoneID );
#endif

//----------------------------------------------------------------------------
/**
 *	@brief  �u�������֐�
 *
 *	@param	zoneID  �u���������]�[��ID
 *
 *	@return �u��������]�[��ID  ���������Ă��Ȃ����TOWNMAP_DATA_ERROR
 */
//-----------------------------------------------------------------------------
static inline u16 TOWNMAP_REPLACE_DATA_GetReplace( u16 zoneID )
{
  {	
    int i;

    for( i = 0; i < TOWNMAP_REPLACE_MAX; i++ )
    { 

      if( sc_townmap_replace_data[i][TOWNMAP_REPLACE_SRC00] == zoneID )
      {
        return sc_townmap_replace_data[i][TOWNMAP_REPLACE_DST];
      }
    }

		return TOWNMAP_DATA_ERROR;
	}
}
