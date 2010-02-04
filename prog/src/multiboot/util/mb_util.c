//======================================================================
/**
 * @file	mb_util_poke.c
 * @brief	�}���`�u�[�g ���[�e�B���e�B
 * @author	ariizumi
 * @data	09/12/06
 *
 * ���W���[�����FMB_UTIL
 */
//======================================================================
#include <gflib.h>
#include "buflen.h"
#include "system/main.h"
#include "system/gfl_use.h"
#include "poke_tool/monsno_def.h"
#include "waza_tool/wazano_def.h"
#include "debug/debug_str_conv.h"

#include "multiboot/mb_util.h"

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

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
const MB_UTIL_CHECK_PLAY_RET MB_UTIL_CheckPlay_PalGate( POKEMON_PASO_PARAM *ppp , const DLPLAY_CARD_TYPE cardType )
{
  MB_UTIL_CHECK_PLAY_RET ret = MUCPR_OK;
  u8 i;
  const BOOL isFastMode = PPP_FastModeOn( ppp );
  u32 wazaArr[4];
  
  PPP_FastModeOff( ppp , isFastMode );
  
  if( PPP_Get( ppp , ID_PARA_tamago_flag , NULL ) == TRUE )
  {
    ret = MUCPR_EGG;
  }
  if( PPP_Get( ppp , ID_PARA_monsno , NULL ) == MONSNO_PITYUU &&
      PPP_Get( ppp , ID_PARA_form_no , NULL ) == 1 )
  {
    ret = MUCPR_GIZAMIMI;
  }

  if( ret == MUCPR_OK )
  {
    for( i=0;i<4;i++ )
    {
      wazaArr[i] = PPP_Get( ppp , ID_PARA_waza1+i , 0 );
    }
    for( i=0;i<4;i++ )
    {
      if( cardType == CARD_TYPE_GS )
      {
        if( wazaArr[i] == WAZANO_IAIGIRI ||
            wazaArr[i] == WAZANO_SORAWOTOBU ||
            wazaArr[i] == WAZANO_KAIRIKI ||
            wazaArr[i] == WAZANO_NAMINORI ||
            wazaArr[i] == WAZANO_UZUSIO ||      //�������Ⴄ
            wazaArr[i] == WAZANO_IWAKUDAKI ||
            wazaArr[i] == WAZANO_TAKINOBORI ||
            wazaArr[i] == WAZANO_ROKKUKURAIMU  )
        {
          ret = MUCPR_HIDEN;
        }
      }
      else
      {
        if( wazaArr[i] == WAZANO_IAIGIRI ||
            wazaArr[i] == WAZANO_SORAWOTOBU ||
            wazaArr[i] == WAZANO_KAIRIKI ||
            wazaArr[i] == WAZANO_NAMINORI ||
            wazaArr[i] == WAZANO_KIRIBARAI ||      //�������Ⴄ
            wazaArr[i] == WAZANO_IWAKUDAKI ||
            wazaArr[i] == WAZANO_TAKINOBORI ||
            wazaArr[i] == WAZANO_ROKKUKURAIMU  )
        {
          ret = MUCPR_HIDEN;
        }
      }
    }
  }

  if( ret == MUCPR_OK )
  {
    if( PPP_Get( ppp , ID_PARA_fusei_tamago_flag , NULL ) ||
        PPP_Get( ppp , ID_PARA_monsno , NULL ) > MONSNO_ARUSEUSU )
    {
      ret = MUCPR_FUSEI;
    }
  }
  return ret;
}

//--------------------------------------------------------------
//	PPP�̕ϊ�
//--------------------------------------------------------------
void MB_UTIL_ConvertPPP( const void *src , POKEMON_PASO_PARAM *dst , const DLPLAY_CARD_TYPE cardType )
{
  GFL_STD_MemCopy( src , dst , POKETOOL_GetPPPWorkSize() );
  //���O�ϊ�
  {
    STRCODE monsName[MONS_NAME_SIZE+EOM_SIZE];
    PPP_Get( dst , ID_PARA_nickname_raw , monsName );
    MB_UTIL_ConvertStr( monsName , monsName , MONS_NAME_SIZE+EOM_SIZE , cardType );
    PPP_Put( dst , ID_PARA_nickname_raw , (u32)monsName );
  }
  //�e�̖��O�ϊ�
  {
    STRCODE oyaName[PERSON_NAME_SIZE+EOM_SIZE];
    PPP_Get( dst , ID_PARA_oyaname_raw , oyaName );
    MB_UTIL_ConvertStr( oyaName , oyaName , SAVELEN_PLAYER_NAME+EOM_SIZE , cardType );
    PPP_Put( dst , ID_PARA_oyaname_raw , (u32)oyaName );
  }
  //���i�̕ϊ�
  {
    const u32 rand = PPP_Get( dst , ID_PARA_personal_rnd , NULL );
    PPP_Put( dst , ID_PARA_seikaku , rand%(PTL_SEIKAKU_MAX) );
  }
  
  //�f�o�b�O�\��
  /*
  {
    char sjisCode[(MONS_NAME_SIZE+EOM_SIZE)*2];
    STRCODE monsName[MONS_NAME_SIZE+EOM_SIZE];
    const u32 monsno = PPP_Get( dst , ID_PARA_monsno , NULL );
    PPP_Get( dst , ID_PARA_nickname_raw , monsName );
    DEB_STR_CONV_StrcodeToSjis( monsName , sjisCode , MONS_NAME_SIZE+EOM_SIZE );
    MB_TPrintf("[%3d][%s]\n",monsno , sjisCode);
  }
  */
}

//--------------------------------------------------------------
//	������̕ϊ�
//--------------------------------------------------------------
#include "./mb_str_arr.cdat"
void MB_UTIL_ConvertStr( const u16 *src , STRCODE *dst , const u8 dstLen , const DLPLAY_CARD_TYPE cardType )
{
  const STRCODE EomCode = GFL_STR_GetEOMCode();
  static const u16 UnknownCode = L'?';
  int i,j;
  BOOL isEnd = FALSE;
  for( i=0;i<dstLen;i++ )
  {
    if( src[i] == 0xFFFF )
    {
      dst[i] = EomCode;
      isEnd = TRUE;
      break;
    }
    for( j=0;j<MB_STR_ARR_NUM;j++ )
    {
      if( src[i] == MB_STR_ARR[j][1] )
      {
        dst[i] = MB_STR_ARR[j][0];
        break;
      }
    }
    if( j == MB_STR_ARR_NUM ){
      //�Y�������Ȃ�
      dst[i] = UnknownCode;
    }
  }
  
  GF_ASSERT_MSG(isEnd,"MB_UTIL_ConvertStr BufferOver!\n");
}

