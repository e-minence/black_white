//======================================================================
/**
 * @file	field_skill_check.c
 * @brief	�t�B�[���h�Z�̏�ԃ`�F�b�N(�Z�Y��悤
 * @author	ariizumi
 * @data	10/03/25
 *
 * ���W���[�����FFIELD_SKILL_CHECK
 */
//======================================================================
#include <gflib.h>
#include "field/field_skill_check.h"
#include "eventwork_def.h"
#include "field/field_status_local.h"   //FIELD_STATUS

#include "waza_tool/wazano_def.h"
#include "item/itemsym.h"

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

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static const u16 HidenTable[] = {
  WAZANO_IAIGIRI,
  WAZANO_KAIRIKI,
  WAZANO_NAMINORI,
  WAZANO_SORAWOTOBU,
  WAZANO_DAIBINGU,
  WAZANO_TAKINOBORI,
};


//--------------------------------------------------------------
//	���g���Ă���Z�r�b�g�̎擾
//--------------------------------------------------------------
const u16 FIELD_SKILL_CHECK_GetUseSkillBit( GAMEDATA *gameData )
{
  u16 bit = 0;

  //�����肫
  {
    EVENTWORK *ev = GAMEDATA_GetEventWork(gameData);
    if( EVENTWORK_CheckEventFlag(ev,SYS_FLAG_KAIRIKI) == TRUE )
    {
      bit += FSCB_KAIRIKI;
    }
  }
  //�g��聕�_�C�r���O
  {
    const PLAYER_WORK *playerWork = GAMEDATA_GetMyPlayerWork(gameData);
    const PLAYER_MOVE_FORM form = PLAYERWORK_GetMoveForm( playerWork );
    if( form == PLAYER_MOVE_FORM_SWIM )
    {
      bit += FSCB_NAMINORI;
    }
    if( form == PLAYER_MOVE_FORM_DIVING )
    {
      bit += FSCB_DIVING;
    }
  }
  //�t���b�V��
  {
    FIELD_STATUS *fldStatus = GAMEDATA_GetFieldStatus( gameData );
    if( FIELD_STATUS_IsFieldSkillFlash( fldStatus ) == TRUE )
    {
      bit += FSCB_FLASH;
    }
  }

  return bit;
}

//--------------------------------------------------------------
//	��`�Z�`�F�b�N
//--------------------------------------------------------------
static BOOL isHiden( u16 wazano )
{
  int i;

  for(i = 0;i < NELEMS(HidenTable);i++){
    if(HidenTable[i] == wazano){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------
//	�w��Z���Y����邩�H
//--------------------------------------------------------------
const FIELD_SKILL_CHECK_RET FIELD_SKILL_CHECK_CheckForgetSkill( GAMEDATA *gameData , const u16 wazaNo , HEAPID heapId )
{
  return (FSCR_OK+isHiden( wazaNo ));
}

//--------------------------------------------------------------
//	�����ł��邩�H
//--------------------------------------------------------------
const BOOL FIELD_SKILL_CHECK_CanTradePoke( POKEMON_PASO_PARAM *ppp , HEAPID heapId )
{
  int i;
  for( i=0;i<4;i++ )
  {
    const u32 wazaNo = PPP_Get( ppp , ID_PARA_waza1+i , NULL );
    if( isHiden( wazaNo )){
      return FALSE;
    }
  }
  return TRUE;
}
