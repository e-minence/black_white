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
#include "savedata/mystatus.h"
#include "net/nhttp_rap_evilcheck.h"
#include "pm_define.h"


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
///	���C�u�}�b�`�Ŏg�p����o�g���X�R�A
//=====================================
typedef struct 
{
  BtlResult result;       //�����̏��s
  u8 enemy_rest_poke;     //�ΐ푊��̎c��|�P������
  u8 enemy_rest_hp;       //�ΐ푊��̎c��HP�i�S�Ẵ|�P�����ł̃p�[�Z���e�[�W�j
  BOOL is_error;          //�G���[���N��������
} BATTLEMATCH_BATTLE_SCORE;

//-------------------------------------
/// �ۑ��f�[�^
//=====================================
typedef struct 
{
  u8        mystatus[MYSTATUS_SAVE_SIZE];
  u8        mac_address[ 6 ];
  u8        dummy[ 2 ];
  u32       win_cnt;
  u32       lose_cnt;
  u32       btl_cnt;
  u32       rate;
  PMS_DATA  pms;
  u32       wificup_no;
  u32       btl_server_version;
  u32       profileID;  
  u32       sake_recordID;
  s8        sign[ NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN ]; //�U�̕��̏���
  u8        pokeparty[0]; //����Ƀ|�P�p�[�e�B�̃f�[�^����
} WIFIBATTLEMATCH_ENEMYDATA;

//�ΐ�ҏ��̃T�C�Y
#define WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE (sizeof(WIFIBATTLEMATCH_ENEMYDATA) + PokeParty_GetWorkSize())

//-------------------------------------
///	�ΐ헚��
//=====================================
typedef struct 
{
  //�ΐ푊��̃v���t�B�[��
  u32 your_gamesyncID; //�Q�[���V���NID
  u32 your_profileID;  //PID

  //�ΐ푊��̃|�P����
  u16 your_monsno[TEMOTI_POKEMAX]; //�|�P����NO
  u8  your_form[TEMOTI_POKEMAX];   //�t�H����NO
  u8  your_lv[TEMOTI_POKEMAX];     //���x��
  u8  your_sex[TEMOTI_POKEMAX];    //����      

  //�����̃|�P����
  u16 my_monsno[TEMOTI_POKEMAX]; //�|�P����NO
  u8  my_form[TEMOTI_POKEMAX];   //�t�H����NO
  u8  my_lv[TEMOTI_POKEMAX];     //���x��
  u8  my_sex[TEMOTI_POKEMAX];    //����      

  //�ΐ���
  u32 year  :7;//�N7bit  99
  u32 month :4;//��4bit  12
  u32 day   :5;//��5bit  31
  u32 hour  :5;//��5bit  24
  u32 minute:6;//��6bit  60
  u32 dummy :5;//�]��5bi
  u32 cupNO;    //���No.
  u8  result;   //�ΐ팋��
  u8  btl_type; //�ΐ�^�C�v
  u8  padding2;
  u8  padding3;
} WIFIBATTLEMATCH_RECORD_DATA;

//-------------------------------------
///	�T�[�o�[�����M���Ă��ď풓�ɂ����f�[�^
//=====================================
typedef struct 
{
  u8    record_save_idx;
} WIFIBATTLEMATCH_RECV_DATA;






#include "arc_def.h"
#include "message.naix"
#include "net_app/wifi_country.h"

//----------------------------------------------------------------------------
/**
 *	@brief  ���O��s���������Ƃ��̖��O�ɕύX
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_data   �f�[�^
 *	@param	heapID                              �e���|�����p�q�[�v
 */
//-----------------------------------------------------------------------------
static inline void WIFIBATTLEMATCH_DATA_ModifiName( WIFIBATTLEMATCH_ENEMYDATA *p_data, HEAPID heapID )
{ 
  MYSTATUS  *p_mystatus = (MYSTATUS *)p_data->mystatus;
  STRBUF* p_strbuf;
  GFL_MSGDATA *p_msg;
  p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
      NARC_message_wifi_match_dat, GetHeapLowID(heapID) );
  p_strbuf  = GFL_MSG_CreateString( p_msg, GetHeapLowID(heapID) );

  //�K�薼�ɕύX
  GFL_STR_GetStringCode( p_strbuf, p_mystatus->name, PERSON_NAME_SIZE+EOM_SIZE );

  GFL_STR_DeleteBuffer( p_strbuf );
  GFL_MSG_Delete( p_msg );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�ҏ�񂪕s���������ꍇ�A������萳��l�ɂ���
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_data ���[�N
 */
//-----------------------------------------------------------------------------
static inline u32 WIFIBATTLEMATCH_DATA_ModifiEnemyData( WIFIBATTLEMATCH_ENEMYDATA *p_data, HEAPID heapID )
{
  MYSTATUS  *p_mystatus = (MYSTATUS *)p_data->mystatus;
  u32 dirty = 0;

  //���O���s��
  { 
    int i;
    BOOL is_darty = TRUE;
    //EOM�������Ă���ΐ���Ƃ݂Ȃ�
    //�s�����͕ʂȂƂ���őΏ�
    for( i = 0; i < PERSON_NAME_SIZE+EOM_SIZE; i++ )
    { 
      if( p_mystatus->name[i] == GFL_STR_GetEOMCode() )
      { 
        is_darty  = FALSE;
      }
    }

    if( is_darty )
    { 
      dirty++;
      WIFIBATTLEMATCH_DATA_ModifiName( p_data, heapID );
    }
  }

  //���ʂ��s��
  if( p_mystatus->sex != PTL_SEX_MALE && p_mystatus->sex != PTL_SEX_FEMALE )
  { 
    OS_TPrintf( "���ʂ��s���������̂ŁA������菑�������܂� %d\n", p_mystatus->sex );
    p_mystatus->sex = PTL_SEX_MALE;
    dirty++;
  }

  //�g���[�i�[�r���[
  if( p_mystatus->trainer_view >= 16 )
  { 
    OS_TPrintf( "�����ڂ��s���������̂ŁA������菑�������܂� %d \n", p_mystatus->trainer_view );
    if( p_mystatus->sex == PTL_SEX_MALE )
    { 
      p_mystatus->trainer_view  = 0;
    }
    else
    { 
      p_mystatus->trainer_view  = 8;
    }
    dirty++;
  }

  //�ꏊ
  if( p_mystatus->nation >= WIFI_COUNTRY_GetDataLen() )
  { 
    OS_TPrintf( "�����s���������̂ŁA������菑�������܂� %d %d\n", p_mystatus->nation, p_mystatus->area );
    p_mystatus->nation  = 0;
    dirty++;
  }

  //�G���A
  { 
    if( p_mystatus->nation != 0 && p_mystatus->area >= WIFI_COUNTRY_CountryCodeToPlaceIndexMax( p_mystatus->nation ) )
    {
      OS_TPrintf( "�n�悪�s���������̂ŁA������菑�������܂� %d %d\n", p_mystatus->nation, p_mystatus->area );
      p_mystatus->area  = 0;
    dirty++;
    }
  }

  //PMS
  if( !PMSDAT_IsValid( &p_data->pms, GetHeapLowID(heapID) ) )
  { 
    OS_TPrintf( "PMS���s���������̂ŁA������菑�������܂�\n" );
    PMSDAT_Init( &p_data->pms, PMS_TYPE_BATTLE_READY );
    dirty++;
  }

  return dirty;
}

#include "debug/debug_str_conv.h"
//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�b�O�v�����g�\��
 *
 *	@param	const WIFIBATTLEMATCH_ENEMYDATA *cp_data  ���[�N
 */
//-----------------------------------------------------------------------------
static inline void WIFIBATTLEMATCH_DATA_DebugPrint( const WIFIBATTLEMATCH_ENEMYDATA *cp_data )
{ 
  const MYSTATUS  *cp_mystatus = (const MYSTATUS *)cp_data->mystatus;

  OS_TFPrintf( 3, "�����v���C���[���\n" );
  { 
    char str[PERSON_NAME_SIZE+EOM_SIZE];
    DEB_STR_CONV_StrcodeToSjis( cp_mystatus->name, str, PERSON_NAME_SIZE+EOM_SIZE ); 
    OS_TFPrintf( 3, "name =[%s]\n", str );
  }
  OS_TFPrintf( 3, "name =[%d]\n", cp_mystatus->sex );
  OS_TFPrintf( 3, "view =[%d]\n", cp_mystatus->trainer_view );
  OS_TFPrintf( 3, "nation =[%d]\n", cp_mystatus->nation );
  OS_TFPrintf( 3, "area =[%d]\n", cp_mystatus->area );
  OS_TFPrintf( 3, "win =[%d]\n", cp_data->win_cnt );
  OS_TFPrintf( 3, "lose =[%d]\n", cp_data->lose_cnt );
  OS_TFPrintf( 3, "btl =[%d]\n", cp_data->btl_cnt );
  OS_TFPrintf( 3, "rate =[%d]\n", cp_data->rate );
  OS_TFPrintf( 3, "cup =[%d]\n", cp_data->wificup_no );

  { 
    int i;
    POKEMON_PARAM *p_pp;
    const POKEPARTY *cp_party  = (const POKEPARTY *)cp_data->pokeparty;
    OS_TFPrintf( 3, "poke_cnt =[%d]\n", PokeParty_GetPokeCount( cp_party) );
    for( i = 0; i < PokeParty_GetPokeCount( cp_party); i++ )
    { 
      p_pp  = PokeParty_GetMemberPointer(cp_party, i);
      OS_TFPrintf( 3, "pokemon[%d] \n", i );
      OS_TFPrintf( 3, "monsno =[%d]\n", PP_Get( p_pp, ID_PARA_monsno, NULL ) );
      OS_TFPrintf( 3, "itemno =[%d]\n", PP_Get( p_pp, ID_PARA_item, NULL ) );
    }
  }
} 
