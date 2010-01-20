//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_data.h
 *	@brief  sys�ɒu���āAcore�ł��g�p����f�[�^�o�b�t�@
 *	@author	Toru=Nagihashi
 *	@date		2009.12.01
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "poke_tool/pokeparty.h"
#include "buflen.h"
#include "system/pms_data.h"


//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================

//-------------------------------------
/// �ۑ��f�[�^
//=====================================

//�ΐ�ҏ��
typedef struct 
{
  STRCODE   name[PERSON_NAME_SIZE+EOM_SIZE];
  u8        sex;
  u8        trainer_view;
  u8        nation;
  u8        area;
  u32       win_cnt;
  u32       lose_cnt;
  u32       btl_cnt;
  u32       rate;
  PMS_DATA  pms;
  u32       pokeparty[0]; //����Ƀ|�P�p�[�e�B�̃f�[�^����
} WIFIBATTLEMATCH_ENEMYDATA;

//�ΐ�ҏ��̃T�C�Y
#define WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE (sizeof(WIFIBATTLEMATCH_ENEMYDATA) + PokeParty_GetWorkSize())


#include "arc_def.h"
#include "message.naix"
#include "net_app/wifi_country.h"
//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�ҏ�񂪕s���������ꍇ�A������萳��l�ɂ���
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_data ���[�N
 */
//-----------------------------------------------------------------------------
static inline void WIFIBATTLEMATCH_DATA_ModifiEnemyData( WIFIBATTLEMATCH_ENEMYDATA *p_data, HEAPID heapID )
{
  //���O���s��
  { 
    int i;
    BOOL is_darty = TRUE;
    //EOM�������Ă���ΐ���Ƃ݂Ȃ�
    for( i = 0; i < PERSON_NAME_SIZE+EOM_SIZE; i++ )
    { 
      if( p_data->name[i] == GFL_STR_GetEOMCode() )
      { 
        is_darty  = FALSE;
      }
    }

    if( is_darty )
    { 
      STRBUF* p_strbuf;
      GFL_MSGDATA *p_msg;
      p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
												NARC_message_wifi_match_dat, GetHeapLowID(heapID) );
      p_strbuf  = GFL_MSG_CreateString( p_msg, GetHeapLowID(heapID) );

      //�K�薼�ɕύX
      OS_TPrintf( "�ΐ�Җ����s���������̂ŁA������菑�������܂�\n" );
      GFL_STR_GetStringCode( p_strbuf, p_data->name, PERSON_NAME_SIZE+EOM_SIZE );

      GFL_STR_DeleteBuffer( p_strbuf );
      GFL_MSG_Delete( p_msg );
    }
  }

  //���ʂ��s��
  if( p_data->sex != PTL_SEX_MALE && p_data->sex != PTL_SEX_FEMALE )
  { 
    OS_TPrintf( "���ʂ��s���������̂ŁA������菑�������܂� %d\n", p_data->sex );
    p_data->sex = PTL_SEX_MALE;
  }

  //�g���[�i�[�r���[
  //@todo
  if( p_data->trainer_view == 0xFFFF )
  { 
    OS_TPrintf( "�����ڂ��s���������̂ŁA������菑�������܂� \n" );
    p_data->trainer_view  = 0;
  }

  //�ꏊ
  if( p_data->nation >= WIFI_COUNTRY_GetDataLen() )
  { 
    OS_TPrintf( "�����s���������̂ŁA������菑�������܂� %d %d\n", p_data->nation, p_data->area );
    p_data->nation  = 0;
  }

  //�G���A
  { 
    if( p_data->area >= WIFI_COUNTRY_CountryCodeToPlaceIndexMax( p_data->nation ) )
    {
      OS_TPrintf( "�n�悪�s���������̂ŁA������菑�������܂� %d %d\n", p_data->nation, p_data->area );
      p_data->area  = 0;
    }
  }

  //PMS
  if( !PMSDAT_IsValid( &p_data->pms, GetHeapLowID(heapID) ) )
  { 
    OS_TPrintf( "PMS���s���������̂ŁA������菑�������܂�\n" );
    PMSDAT_Init( &p_data->pms, PMS_TYPE_BATTLE_READY ); //�����A�H @todo
  }
}
