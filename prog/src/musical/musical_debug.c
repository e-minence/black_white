//======================================================================
/**
 * @file	musical_debug.c
 * @brief	�~���[�W�J���f�o�b�O�p
 * @author	ariizumi
 * @data	100322
 *
 * ���W���[�����FMUSICAL_DEBUG
 */
//======================================================================

#include "musical_entry.cdat"
#include "musical_debug.h"
#include "poke_tool/poke_tool.h"

#if PM_DEBUG
const BOOL MUSICAL_DEBUG_CreateDummyData( MUSICAL_SHOT_DATA* shotData , const u16 monsNo , const HEAPID heapId )
{
  u8 i;
  RTCDate date;
  POKEMON_PERSONAL_DATA *perData;

  perData = POKE_PERSONAL_OpenHandle( monsNo , 0 ,heapId );

  GFL_RTC_GetDate( &date );
  shotData->bgNo = 0;
  shotData->year = date.year;
  shotData->month = date.month;
  shotData->day = date.day;
  shotData->title[0] = L'�X';
  shotData->title[1] = L'�g';
  shotData->title[2] = L'��';
  shotData->title[3] = L'�[';
  shotData->title[4] = L'�~';
  shotData->title[5] = L'��';
  shotData->title[6] = L'�O';
  shotData->title[7] = L'��';
  shotData->title[8] = L'��';
  shotData->title[9] = L'��';
  shotData->title[10] = GFL_STR_GetEOMCode();
  shotData->player = 0;
  
  shotData->musVer = MUSICAL_VERSION;
  shotData->pmVersion = VERSION_BLACK;
  shotData->pmLang = LANG_JAPAN;
  shotData->spotBit = 1;
  
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    u8 j;
    shotData->shotPoke[i].monsno = monsNo;
    switch( POKE_PERSONAL_GetParam(perData,POKEPER_ID_sex) )
    {
    case POKEPER_SEX_MALE:
      shotData->shotPoke[i].sex = PTL_SEX_MALE;
      break;
    case POKEPER_SEX_FEMALE:
      shotData->shotPoke[i].sex = PTL_SEX_FEMALE;
      break;
    case POKEPER_SEX_UNKNOWN:
      shotData->shotPoke[i].sex = PTL_SEX_UNKNOWN;
      break;
    default:
      shotData->shotPoke[i].sex = PTL_SEX_MALE;
      break;
    }
    shotData->shotPoke[i].rare = 0;
    shotData->shotPoke[i].form = 0;
    shotData->shotPoke[i].trainerName[0] = L'�g';
    shotData->shotPoke[i].trainerName[1] = L'��';
    shotData->shotPoke[i].trainerName[2] = L'�[';
    shotData->shotPoke[i].trainerName[3] = L'�i';
    shotData->shotPoke[i].trainerName[4] = L'�P'+i;
    shotData->shotPoke[i].trainerName[5] = 0;
    //�����ӏ��̏�����
    for( j=0;j<MUSICAL_ITEM_EQUIP_MAX;j++ )
    {
      shotData->shotPoke[i].equip[j].itemNo = 0;  //65535;
      shotData->shotPoke[i].equip[j].angle = 0;
      shotData->shotPoke[i].equip[j].equipPos = MUS_POKE_EQU_EAR_R; //MUS_POKE_EQU_INVALID;
    }
    shotData->shotPoke[i].equip[0].itemNo = 19;
    shotData->shotPoke[i].equip[0].equipPos = MUS_POKE_EQU_HAND_L;
  }
  
  POKE_PERSONAL_CloseHandle( perData );
  return TRUE;
}
#endif
