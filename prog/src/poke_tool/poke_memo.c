//======================================================================
/**
 * @file	poke_memo.c
 * @brief	�g���[�i�[�����ݒ�E�\���֐�
 * @author	ariizumi
 * @data	10/03/13
 *
 * ���W���[�����FPOKE_MEMO
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "poke_tool/monsno_def.h"
#include "poke_tool/poke_memo.h"


//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef enum
{
  PMDT_1, //�ꏊ�P�E���ԂP  �ߊl�����`
  PMDT_2, //�ꏊ�Q�E���ԂQ  ���܂ꂽ�`
}POKE_MEMO_DATA_TYPE;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void POKE_MEMO_ClearPlaceTime( POKEMON_PASO_PARAM *ppp , const POKE_MEMO_DATA_TYPE setType );
static void POKE_MEMO_SetPlaceTime( POKEMON_PASO_PARAM *ppp , const u32 place , const POKE_MEMO_DATA_TYPE setType );
static void POKE_MEMO_CopyPlaceTime( POKEMON_PASO_PARAM *ppp , const POKE_MEMO_DATA_TYPE srcSetType );
static void POKE_MEMO_SetMyStatus( POKEMON_PASO_PARAM *ppp , const MYSTATUS* my , const HEAPID heapId );
static void POKE_MEMO_SetGetLevel( POKEMON_PASO_PARAM *ppp );
static void POKE_MEMO_SetRomVersion( POKEMON_PASO_PARAM *ppp );
static void POKE_MEMO_SetRomVersion( POKEMON_PASO_PARAM *ppp );


#pragma mark [>TrainerMemo
//--------------------------------------------------------------
//	�g���[�i�[�����ݒ�
//--------------------------------------------------------------
void POKE_MEMO_SetTrainerMemoPP( POKEMON_PARAM *pp , const POKE_MEMO_SET_TYPE type ,
                                  const MYSTATUS* my, const u32 place , const HEAPID heapId )
{
  POKE_MEMO_SetTrainerMemoPPP( PP_GetPPPPointer(pp) , type , my , place , heapId );
}

void POKE_MEMO_SetTrainerMemoPPP( POKEMON_PASO_PARAM *ppp , const POKE_MEMO_SET_TYPE type ,
                                  const MYSTATUS* my, const u32 place , const HEAPID heapId )
{
  const BOOL fastFlg = PPP_FastModeOn(ppp);
  switch( type )
  {
  case POKE_MEMO_SET_CAPTURE:  //�ߊl
    POKE_MEMO_ClearPlaceTime( ppp , PMDT_2 );
    POKE_MEMO_SetPlaceTime( ppp , place , PMDT_1 );
    POKE_MEMO_SetMyStatus( ppp , my , heapId );
    POKE_MEMO_SetGetLevel( ppp );
    POKE_MEMO_SetRomVersion( ppp );
    break;

  case POKE_MEMO_GAME_TRADE:   //�Q�[��������
    POKE_MEMO_SetPlaceTime( ppp , POKE_MEMO_PLACE_GAME_TRADE , PMDT_2 );
    POKE_MEMO_SetGetLevel( ppp );
    POKE_MEMO_SetRomVersion( ppp );
    break;

  case POKE_MEMO_INCUBATION:   //�^�}�S�z��
    {
      const BOOL isMatchOya = PPP_IsMatchOya( ppp , my );
      if( isMatchOya == TRUE )
      {
        POKE_MEMO_SetPlaceTime( ppp , place , PMDT_2 );
        POKE_MEMO_SetMyStatus( ppp , my , heapId );
      }
      else
      {
        POKE_MEMO_CopyPlaceTime( ppp , PMDT_2 );
        POKE_MEMO_SetPlaceTime( ppp , place , PMDT_2 );
        POKE_MEMO_SetMyStatus( ppp , my , heapId );
      }
      POKE_MEMO_SetRomVersion( ppp );
    }
    break;

  case POKE_MEMO_POKE_SHIFTER: //�|�P�V�t�^�[(�ߋ���]��
    POKE_MEMO_SetTrainerMemoPokeShifter( ppp );
    break;

  case POKE_MEMO_DISTRIBUTION: //�z�z
    POKE_MEMO_SetPlaceTime( ppp , place , PMDT_2 );
    POKE_MEMO_SetGetLevel( ppp );
    POKE_MEMO_SetRomVersion( ppp );
    break;
    
  case POKE_MEMO_EGG_FIRST:
    POKE_MEMO_SetPlaceTime( ppp , place , PMDT_1 );
    POKE_MEMO_SetMyStatus( ppp , my , heapId );
    POKE_MEMO_SetRomVersion( ppp );
    break;
    
  case POKE_MEMO_EGG_TRADE:
    POKE_MEMO_SetPlaceTime( ppp , place , PMDT_2 );
    break;
    
  }
  PPP_FastModeOff(ppp,fastFlg);
}

void POKE_MEMO_SetTrainerMemoPokeShifter( POKEMON_PASO_PARAM *ppp )
{
  const u32 monsNo = PPP_Get( ppp, ID_PARA_monsno , NULL );
  const u32 birthPlace = PPP_Get( ppp, ID_PARA_birth_place , NULL );
  const u32 isEvent = PPP_Get( ppp, ID_PARA_event_get_flag , NULL );
  const BOOL isRare = PPP_CheckRare( ppp );
  
  //�ʏ�Ή�
  POKE_MEMO_SetPlaceTime( ppp , POKE_MEMO_PLACE_POKE_SHIFTER , PMDT_2 );

  //2010�C�x���g�Ή�
  if( monsNo == MONSNO_SEREBHI &&
      birthPlace == POKE_MEMO_2010_MOVIE &&
      isEvent == 1 )
  {
    PPP_Put( ppp, ID_PARA_birth_place, POKE_MEMO_PLACE_SEREBIXI_BEFORE );
  }
  else
  if( ( monsNo == MONSNO_RAIKOU || monsNo == MONSNO_ENTEI || monsNo == MONSNO_SUIKUN ) &&
      birthPlace == POKE_MEMO_2010_MOVIE &&
      isRare == TRUE &&
      isEvent == 1 )
  {
    PPP_Put( ppp, ID_PARA_birth_place, POKE_MEMO_PLACE_ENRAISUI_BEFORE );
  }
}

static void POKE_MEMO_ClearPlaceTime( POKEMON_PASO_PARAM *ppp , const POKE_MEMO_DATA_TYPE setType )
{
  if( setType == PMDT_1 )
  {
    PPP_Put( ppp , ID_PARA_get_place , 0 );
    PPP_Put( ppp , ID_PARA_get_year  , 0 );
    PPP_Put( ppp , ID_PARA_get_month , 0 );
    PPP_Put( ppp , ID_PARA_get_day   , 0 );
  }
  else
  {
    PPP_Put( ppp , ID_PARA_birth_place , 0 );
    PPP_Put( ppp , ID_PARA_birth_year  , 0 );
    PPP_Put( ppp , ID_PARA_birth_month , 0 );
    PPP_Put( ppp , ID_PARA_birth_day   , 0 );
  }
}

static void POKE_MEMO_SetPlaceTime( POKEMON_PASO_PARAM *ppp , const u32 place , const POKE_MEMO_DATA_TYPE setType )
{
  RTCDate date;
  GFL_RTC_GetDate( &date );
  
  if( setType == PMDT_1 )
  {
    PPP_Put( ppp , ID_PARA_get_place , place );
    PPP_Put( ppp , ID_PARA_get_year  , date.year );
    PPP_Put( ppp , ID_PARA_get_month , date.month );
    PPP_Put( ppp , ID_PARA_get_day   , date.day );
  }
  else
  {
    PPP_Put( ppp , ID_PARA_birth_place , place );
    PPP_Put( ppp , ID_PARA_birth_year  , date.year );
    PPP_Put( ppp , ID_PARA_birth_month , date.month );
    PPP_Put( ppp , ID_PARA_birth_day   , date.day );
  }
}

static void POKE_MEMO_CopyPlaceTime( POKEMON_PASO_PARAM *ppp , const POKE_MEMO_DATA_TYPE srcSetType )
{
  if( srcSetType == PMDT_1 )
  {
    const u32 place = PPP_Get( ppp , ID_PARA_get_place , NULL );
    const u32 year  = PPP_Get( ppp , ID_PARA_get_year  , NULL );
    const u32 month = PPP_Get( ppp , ID_PARA_get_month , NULL );
    const u32 day   = PPP_Get( ppp , ID_PARA_get_day   , NULL );

    PPP_Put( ppp , ID_PARA_birth_place , place );
    PPP_Put( ppp , ID_PARA_birth_year  , year );
    PPP_Put( ppp , ID_PARA_birth_month , month );
    PPP_Put( ppp , ID_PARA_birth_day   , day );
  }
  else
  {
    const u32 place = PPP_Get( ppp , ID_PARA_birth_place , NULL );
    const u32 year  = PPP_Get( ppp , ID_PARA_birth_year  , NULL );
    const u32 month = PPP_Get( ppp , ID_PARA_birth_month , NULL );
    const u32 day   = PPP_Get( ppp , ID_PARA_birth_day   , NULL );

    PPP_Put( ppp , ID_PARA_get_place , place );
    PPP_Put( ppp , ID_PARA_get_year  , year );
    PPP_Put( ppp , ID_PARA_get_month , month );
    PPP_Put( ppp , ID_PARA_get_day   , day );
  }
}

static void POKE_MEMO_SetMyStatus( POKEMON_PASO_PARAM *ppp , const MYSTATUS* my , const HEAPID heapId )
{
  STRBUF *name  = MyStatus_CreateNameString( my, heapId );
  const u32 id  = MyStatus_GetID( my );
  const u32 sex = MyStatus_GetMySex( my );

  PPP_Put( ppp , ID_PARA_oyaname , (u32)name );
  PPP_Put( ppp , ID_PARA_oyasex , id );
  PPP_Put( ppp , ID_PARA_id_no , sex );
}

static void POKE_MEMO_SetGetLevel( POKEMON_PASO_PARAM *ppp )
{
  const u32 lv  = PPP_Get( ppp , ID_PARA_level , NULL );
  
  PPP_Put( ppp , ID_PARA_get_level , lv );
}

static void POKE_MEMO_SetRomVersion( POKEMON_PASO_PARAM *ppp )
{
  PPP_Put( ppp , ID_PARA_get_cassette , GET_VERSION() );
}

#pragma mark [>CheckEventPoke
const BOOL POKE_MEMO_CheckEventPokePP( POKEMON_PARAM *pp , const POKE_MEMO_EVENT_TYPE type )
{
  return POKE_MEMO_CheckEventPokePPP(PP_GetPPPPointer(pp),type);
}
const BOOL POKE_MEMO_CheckEventPokePPP( POKEMON_PASO_PARAM *ppp , const POKE_MEMO_EVENT_TYPE type )
{
  const BOOL fastFlg = PPP_FastModeOn(ppp);

  const u32 monsNo = PPP_Get( ppp, ID_PARA_monsno , NULL );
  const u32 birthPlace = PPP_Get( ppp, ID_PARA_birth_place , NULL );
  const u32 isEvent = PPP_Get( ppp, ID_PARA_event_get_flag , NULL );
  const BOOL isRare = PPP_CheckRare( ppp );

  PPP_FastModeOff(ppp,fastFlg);

  switch( type )
  {
  case POKE_MEMO_EVENT_2010MOVIE_SEREBIXI_BEF:   
    //2010�f��Z���r�B(�C�x���g�O
    if( monsNo == MONSNO_SEREBHI &&
        birthPlace == POKE_MEMO_PLACE_SEREBIXI_BEFORE &&
        isEvent == 1 )
    {
      return TRUE;
    }
    break;

  case POKE_MEMO_EVENT_2010MOVIE_SEREBIXI_AFT:   
    //2010�f��Z���r�B(�C�x���g��
    if( monsNo == MONSNO_SEREBHI &&
        birthPlace == POKE_MEMO_PLACE_SEREBIXI_AFTER &&
        isEvent == 1 )
    {
      return TRUE;
    }
    break;

  case POKE_MEMO_EVENT_2010MOVIE_ENRAISUI_BEF:   
    //2010�f��G���e�C�E���C�R�E�E�X�C�N��(�C�x���g�O
    if( ( monsNo == MONSNO_RAIKOU || monsNo == MONSNO_ENTEI || monsNo == MONSNO_SUIKUN ) &&
        birthPlace == POKE_MEMO_PLACE_ENRAISUI_BEFORE &&
        isRare == TRUE &&
        isEvent == 1 )
    {
      return TRUE;
    }
    break;
  case POKE_MEMO_EVENT_2010MOVIE_ENRAISUI_AFT:
    //2010�f��G���e�C�E���C�R�E�E�X�C�N��(�C�x���g��
    if( ( monsNo == MONSNO_RAIKOU || monsNo == MONSNO_ENTEI || monsNo == MONSNO_SUIKUN ) &&
        birthPlace == POKE_MEMO_PLACE_ENRAISUI_AFTER &&
        isRare == TRUE &&
        isEvent == 1 )
    {
      return TRUE;
    }

  case POKE_MEMO_EVENT_DARUTANISU:
    //�z�z�_���^�j�X
    if( monsNo == MONSNO_DARUTANISU &&
        isEvent == 1 )
    {
      return TRUE;
    }
    break;

  case POKE_MEMO_EVENT_MERODHIA:
    //�z�z�����f�B�A
    if( monsNo == MONSNO_MERODHIA &&
        isEvent == 1 )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//�C�x���g��̏������s��(2010�f��z�z�p
void POKE_MEMO_SetEventPoke_AfterEventPP( POKEMON_PARAM *pp , const POKE_MEMO_EVENT_TYPE type )
{
  POKE_MEMO_SetEventPoke_AfterEventPPP( PP_GetPPPPointer(pp),type);
}

void POKE_MEMO_SetEventPoke_AfterEventPPP( POKEMON_PASO_PARAM *ppp , const POKE_MEMO_EVENT_TYPE type )
{
  const BOOL fastFlg = PPP_FastModeOn(ppp);
  const BOOL flg = POKE_MEMO_CheckEventPokePPP( ppp , type );

  if( flg == TRUE )
  {
    switch(type)
    {
    case POKE_MEMO_EVENT_2010MOVIE_SEREBIXI_BEF:
      //2010�f��Z���r�B(�C�x���g�O
      PPP_Put( ppp, ID_PARA_birth_place, POKE_MEMO_PLACE_SEREBIXI_AFTER );
      break;
    case POKE_MEMO_EVENT_2010MOVIE_ENRAISUI_BEF:
      //2010�f��G���e�C�E���C�R�E�E�X�C�N��(�C�x���g�O
      PPP_Put( ppp, ID_PARA_birth_place, POKE_MEMO_PLACE_ENRAISUI_AFTER );
      break;
    }
  }

  PPP_FastModeOff(ppp,fastFlg);
}
