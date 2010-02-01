//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   worldtrade_adapter.c
 *  @brief  GTS���ڐA����ۂ̃p�b�N�֐��Q
 *  @author Toru=Nagihashi
 *  @data   2009.08.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//lib
#include <gflib.h>
//system
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "gamesystem/msgspeed.h"
#include <dpw_tr.h>

//mine
#include "worldtrade_adapter.h"
//=============================================================================
/**
 *          �萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *          �\���̐錾
*/
//=============================================================================
#define POKEMON_DATA_SIZE   (236)
//POKETOOL_GetWorkSize()
//savedata\worldtrade\worldtrade_data.c  15

//=============================================================================
/**
 *          �v���g�^�C�v�錾
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  PPP����j�b�N�l�[����o�^����֐�
 *
 *  @param  WORDSET* wordset  ���[�h�Z�b�g
 *  @param  bufID             �o�b�t�@ID
 *  @param  POKEMON_PASO_PARAM * ppp  PPP
 */
//-----------------------------------------------------------------------------
void WT_WORDSET_RegisterPokeNickNamePPP( WORDSET* wordset, u32 bufID, const POKEMON_PASO_PARAM * ppp )
{
  POKEMON_PARAM *pp = PP_CreateByPPP( ppp, HEAPID_WORLDTRADE );
  WORDSET_RegisterPokeNickName( wordset, bufID, pp );
  GFL_HEAP_FreeMemory( pp );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �|�P�����p�����[�^���̃������m��
 *
 *  @param  HEAPID heapID   �q�[�vID
 *
 *  @return PP�\���̕��̃�����
 */
//-----------------------------------------------------------------------------
POKEMON_PARAM *PokemonParam_AllocWork( HEAPID heapID )
{
  return GFL_HEAP_AllocMemory( heapID, POKEMON_DATA_SIZE );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �|�P�����p�����[�^�̃R�s�[
 *
 *  @param  const POKEMON_PARAM *pp_src
 *  @param  *pp_dest
 */
//-----------------------------------------------------------------------------
void WT_PokeCopyPPtoPP( const POKEMON_PARAM *pp_src, POKEMON_PARAM *pp_dest )
{
  GFL_STD_MemCopy( pp_src, pp_dest, POKEMON_DATA_SIZE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �N���A
 *
 *  @param  GFL_BMPWIN * winGFLBMPWIN
 *  @param  trans_sw        �]���t���O
 */
//-----------------------------------------------------------------------------
void BmpMenuWinClear( GFL_BMPWIN * win, u8 trans_sw )
{
  GFL_BMPWIN_ClearScreen( win );
  switch( trans_sw )
  {
  case WINDOW_TRANS_ON:
    GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(win) );
    break;
  case WINDOW_TRANS_ON_V:
    GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(win) );
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ������W�J����MSG���擾
 *
 *  @param  WORDSET *wordset  ���[�h�Z�b�g
 *  @param  *MsgManager       ���b�Z�[�W
 *  @param  strID             ����ID
 *  @param  heapID            �e���|�����q�[�vID
 *
 *  @return ������W�J���STRBUF
 */
//-----------------------------------------------------------------------------
STRBUF * MSGDAT_UTIL_AllocExpandString( const WORDSET *wordset, GFL_MSGDATA *MsgManager, u32 strID, HEAPID heapID )
{
  STRBUF  *src;
  STRBUF  *dst;
  src = GFL_MSG_CreateString( MsgManager, strID );
  dst = GFL_STR_CreateBuffer( GFL_STR_GetBufferLength( src ), heapID );

  WORDSET_ExpandStr( wordset, src, dst );

  GFL_STR_DeleteBuffer( src );

  return dst;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ppp����pp���쐬���A�R�s�[����
 *
 *  @param  const POKEMON_PASO_PARAM *ppp
 *  @param  *pp
 */
//-----------------------------------------------------------------------------
void PokeReplace( const POKEMON_PASO_PARAM *ppp, POKEMON_PARAM *pp )
{
  POKEMON_PARAM*  src = PP_CreateByPPP( ppp, HEAPID_WORLDTRADE );
  WT_PokeCopyPPtoPP( src, pp );
  GFL_HEAP_FreeMemory( src );
}


//----------------------------------------------------------------------------
/**
 *  @brief  PP2PPP
 *
 *  @param  POKEMON_PARAM *pp
 *
 *  @return PPP
 */
//-----------------------------------------------------------------------------
POKEMON_PASO_PARAM* PPPPointerGet( POKEMON_PARAM *pp )
{
  return (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst( pp );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���t�@�C�����쐬
 *
 *  @param  SAVE_CONTROL_WORK * savedata  �Z�[�u�f�[�^
 *  @param  * profile                     �v���t�@�C��
 *
 */
//-----------------------------------------------------------------------------
static void EMAILSAVE_DCProfileCreateCommon( SAVE_CONTROL_WORK *sv, Dpw_Common_Profile *dc_profile )
{
  WIFI_HISTORY *wh = SaveData_GetWifiHistory(sv);
  MYSTATUS *my = SaveData_GetMyStatus(sv);
  //char *email_address = EMAILSAVE_AddressGet(sv);

  GFL_STD_MemClear(dc_profile, sizeof(Dpw_Common_Profile));

  dc_profile->version = PM_VERSION;
  dc_profile->language = PM_LANG;
  dc_profile->countryCode = MyStatus_GetMyNation(my);
  dc_profile->localCode = MyStatus_GetMyArea(my);
  dc_profile->playerId = MyStatus_GetID(my);

  STRTOOL_Copy( MyStatus_GetMyName(my), dc_profile->playerName, DPW_TR_NAME_SIZE );

  dc_profile->flag = 0; //�n���O��������\���ł��邩
//  dc_profile->macAddr   ���C�u�������Ŋi�[����̂ŃZ�b�g�̕K�v�Ȃ�

  //strcpy(dc_profile->mailAddr, email_address);

#if 0
  dc_profile->mailRecvFlag = FALSE;////EMAILSAVE_ParamGet(sv, EMAIL_PARAM_RECV_FLAG);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_EMAIL)
  SVLD_SetCrc(GMDATA_ID_EMAIL);
#endif //CRC_LOADCHECK
#endif
}
void EMAILSAVE_DCProfileCreate_Update( SAVE_CONTROL_WORK * savedata, Dpw_Common_Profile * profile )
{
  EMAILSAVE_DCProfileCreateCommon( savedata, profile );
}

//=============================================================================
/**
 *          �v�����g�t�H���g����
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  ������̕���Ԃ�
 *
 *  @param  WT_PRINT *wk  �v�����g���
 *  @param  *buf    ������
 *  @param  magin   �}�[�W��
 *
 *  @return ��
 */
//-----------------------------------------------------------------------------
int FontProc_GetPrintStrWidth( WT_PRINT *wk, u8 font_idx, STRBUF *buf, int magin )
{
  return PRINTSYS_GetStrWidth( buf, wk->font, magin );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ������\���ɕK�v�ȏ���������
 *
 *  @param  WT_PRINT *wk  ���[�N
 *  @param  CONFIG *cfg   �R���t�B�O
 */
//-----------------------------------------------------------------------------
void WT_PRINT_Init( WT_PRINT *wk, const CONFIG *cfg )
{
  GFL_STD_MemClear( wk, sizeof(WT_PRINT) );

  wk->tcbsys  = GFL_TCBL_Init( HEAPID_WORLDTRADE, HEAPID_WORLDTRADE, 32, 32 );
  wk->cfg     = cfg;
  wk->font    = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WORLDTRADE );


  {
    int i;
    WT_PRINT_QUE  *p_one;
    for( i = 0; i < WT_PRINT_BUFF_MAX; i++ )
    {
      p_one = &wk->one[i];
      p_one->que    = PRINTSYS_QUE_Create( HEAPID_WORLDTRADE );
      GFL_STD_MemClear( &p_one->util, sizeof(PRINT_UTIL) );
      p_one->use    = FALSE;
    }
  }

}

//----------------------------------------------------------------------------
/**
 *  @brief  ��������ɕK�v�ȏ���j��
 *
 *  @param  WT_PRINT *wk  ���[�N
 */
//-----------------------------------------------------------------------------
void WT_PRINT_Exit( WT_PRINT *wk )
{
  {
    int i;
    WT_PRINT_QUE  *p_one;
    for( i = 0; i < WT_PRINT_BUFF_MAX; i++ )
    {
      p_one = &wk->one[i];
      PRINTSYS_QUE_Delete( p_one->que );
    }
  }

  GFL_FONT_Delete( wk->font );
  GFL_TCBL_Exit( wk->tcbsys );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ������\��ɕK�v�ȏ��  ���C������
 *
 *  @param  WT_PRINT *wk ���[�N
 */
//-----------------------------------------------------------------------------
void WT_PRINT_Main( WT_PRINT *wk )
{
  {
    int i;
    WT_PRINT_QUE  *p_one;
    for( i = 0; i < WT_PRINT_BUFF_MAX; i++ )
    {
      p_one = &wk->one[i];
      if( p_one->use )
      {
        if( PRINT_UTIL_Trans( &p_one->util, p_one->que ) )
        {
          p_one->use  = FALSE;
        }
      }

      if( wk->stream[i] != NULL )
      {
        if( PRINTSYS_PrintStreamGetState(wk->stream[i]) == PRINTSTREAM_STATE_DONE )
        {
          PRINTSYS_PrintStreamDelete( wk->stream[i] );
          wk->stream[i] = NULL;
        }
      }
    }
  }


  GFL_TCBL_Main( wk->tcbsys );
}

//----------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  PRINT_STREAM *stream  �X�g���[��
 *
 *  @return FALSE�Ȃ�Ε�����`��I�� TRUE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
BOOL GF_MSG_PrintEndCheck( WT_PRINT *setup )
{
  int i;
  for( i = 0; i < WT_PRINT_BUFF_MAX; i++ )
  {

    if( setup->stream[i] != NULL )
    {
      if( PRINTSYS_PrintStreamGetState(setup->stream[i]) == PRINTSTREAM_STATE_DONE )
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  GFL_BMPWIN *bmpwin  BMPWIN
 *  @param  *str  ������
 *  @param  x     �\���ʒuX
 *  @param  y     �\���ʒuY
 *  @param  WT_PRINT *setup   ������ɕK�v�ȏ��
 *
 *  @return �v�����g�X�g���[��
 */
//-----------------------------------------------------------------------------
void GF_STR_PrintSimple( GFL_BMPWIN *bmpwin, u8 font_idx, STRBUF *str, int x, int y, WT_PRINT *setup )
{
  int i;
  {
    for( i = 0; i < WT_PRINT_BUFF_MAX; i++ )
    {
      if( setup->stream[i] == NULL )
      {
        break;
      }
    }
    GF_ASSERT( i < WT_PRINT_BUFF_MAX );
  }

  setup->stream[i]  = PRINTSYS_PrintStream(
    bmpwin, x, y, str, setup->font,
    MSGSPEED_GetWait(), setup->tcbsys, 0, HEAPID_WORLDTRADE, 0x0f );

}
//----------------------------------------------------------------------------
/**
 *  @brief  �F���ꊇ�\��
 *
 *  @param  GFL_BMPWIN *bmpwin  ��������BMPWIN
 *  @param  *str  ������
 *  @param  x     X
 *  @param  y     Y
 *  @param  color �F
 *  @param  *setup  ������p���
 */
//-----------------------------------------------------------------------------
void GF_STR_PrintColor( GFL_BMPWIN *bmpwin, u8 font_idx, STRBUF *str, int x, int y, int put_type, PRINTSYS_LSB color, WT_PRINT *setup )
{
  WT_PRINT_QUE  *p_one  = NULL;

  {
    int i;
    for( i = 0; i < WT_PRINT_BUFF_MAX; i++ )
    {
      if( !setup->one[i].use )
      {
        p_one = &setup->one[i];
      }
    }
  }

  GF_ASSERT( p_one != NULL );

  PRINT_UTIL_Setup( &p_one->util, bmpwin );
  PRINT_UTIL_PrintColor( &p_one->util, p_one->que, x, y, str, setup->font, color );

  p_one->use  = TRUE;
}

