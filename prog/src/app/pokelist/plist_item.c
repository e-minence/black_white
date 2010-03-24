//======================================================================
/**
 * @file  plist_item.c
 * @brief �|�P�������X�g �A�C�e���`�F�b�N�n
 * @author  ariizumi
 * @data  09/08/05
 *
 * ���W���[�����FPLIST_ITEM
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "item/item.h"
#include "msg/msg_pokelist.h"
#include "print/wordset.h"
#include "print/str_tool.h"
#include "system/bmp_winframe.h"
#include "poke_tool/shinka_check.h"

#include "plist_sys.h"
#include "plist_plate.h"
#include "plist_message.h"
#include "plist_item.h"
#include "poke_tool/status_rcv.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//GS�����`�ڐA
#define RCV_FLG_SLEEP   ( 0x01 )  // �����
#define RCV_FLG_POISON    ( 0x02 )  // �ŉ�
#define RCV_FLG_BURN    ( 0x04 )  // �Ώ���
#define RCV_FLG_ICE     ( 0x08 )  // �X��
#define RCV_FLG_PARALYZE  ( 0x10 )  // ��჉�
#define RCV_FLG_PANIC   ( 0x20 )  // ������
#define RCV_FLG_ALL     ( 0x3f )  // �S��

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static u8 PLIST_ITEM_RecoverCheck( u16 item );

static const u16 PLIST_ITEM_UTIL_GetParamExpSum( POKEMON_PARAM *pp );
static const BOOL PLIST_ITEM_UTIL_CanAddParamExp( POKEMON_PARAM *pp , const int id , u16 item );
static const BOOL PLIST_ITEM_UTIL_CanSubParamExp( POKEMON_PARAM *pp , const int id , u16 item );
static const s32 PLIST_ITEM_UTIL_GetFriendValue( POKEMON_PARAM *pp , u16 itemNo , HEAPID heapId );
static void PLIST_ITEM_UTIL_ItemUseMessageCommon( PLIST_WORK *work , u16 msgId );
static void PLIST_ITEM_UTIL_ItemUseMessageParamExp( PLIST_WORK *work , u16 msgId , u16 statusID );
static void PLIST_ITEM_UTIL_ItemUseMessageLvUp( PLIST_WORK *work );
static void PLIST_MSGCB_LvUp( PLIST_WORK *work );

//--------------------------------------------------------------------------------------------
/**
 * �񕜃^�C�v�`�F�b�N
 *
 * @param item  �A�C�e���ԍ�
 *
 * @return  �񕜃^�C�v
 */
//--------------------------------------------------------------------------------------------
static u8 PLIST_ITEM_RecoverCheck( u16 item )
{
  ITEMDATA * dat;
  s32 prm;

  dat = ITEM_GetItemArcData( item, ITEM_GET_DATA, HEAPID_POKELIST );

  // �p�����[�^�������Ă��Ȃ��A�C�e���͂��̑�
  if( ITEM_GetBufParam( dat, ITEM_PRM_W_TYPE ) != ITEM_WKTYPE_POKEUSE )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_ETC;
  }

  // �퓬�p�X�e�[�^�X�A�b�v�n
  if( ITEM_GetBufParam( dat, ITEM_PRM_ATTACK_UP ) != 0 ||
    ITEM_GetBufParam( dat, ITEM_PRM_DEFENCE_UP ) != 0 ||
    ITEM_GetBufParam( dat, ITEM_PRM_SP_ATTACK_UP ) != 0 ||
    ITEM_GetBufParam( dat, ITEM_PRM_SP_DEFENCE_UP ) != 0 ||
    ITEM_GetBufParam( dat, ITEM_PRM_AGILITY_UP ) != 0 ||
    ITEM_GetBufParam( dat, ITEM_PRM_HIT_UP ) != 0 ||
    ITEM_GetBufParam( dat, ITEM_PRM_CRITICAL_UP ) != 0 )
  {

    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_BTL_ST_UP;
  }

  // �S���m����
  if( ITEM_GetBufParam( dat, ITEM_PRM_ALL_DEATH_RCV ) != 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_ALLDETH_RCV;
  }

  // LvUp�n
  if( ITEM_GetBufParam( dat, ITEM_PRM_LV_UP ) != 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_LV_UP;
  }

  // Status�񕜌n
  prm = ITEM_GetBufParam( dat, ITEM_PRM_SLEEP_RCV );        // ����
  prm += ( ITEM_GetBufParam( dat, ITEM_PRM_POISON_RCV ) << 1 ); // ��
  prm += ( ITEM_GetBufParam( dat, ITEM_PRM_BURN_RCV ) << 2 );   // �Ώ�
  prm += ( ITEM_GetBufParam( dat, ITEM_PRM_ICE_RCV ) << 3 );    // �X
  prm += ( ITEM_GetBufParam( dat, ITEM_PRM_PARALYZE_RCV ) << 4 ); // ���
  prm += ( ITEM_GetBufParam( dat, ITEM_PRM_PANIC_RCV ) << 5 );    // ����
  switch( prm )
  {
  case RCV_FLG_SLEEP:   // ����
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_NEMURI_RCV;
  case RCV_FLG_POISON:  // ��
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_DOKU_RCV;
  case RCV_FLG_BURN:    // �Ώ�
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_YAKEDO_RCV;
  case RCV_FLG_ICE:   // �X
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_KOORI_RCV;
  case RCV_FLG_PARALYZE:  // ���
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_MAHI_RCV;
  case RCV_FLG_PANIC:   // ����
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_KONRAN_RCV;
  case RCV_FLG_ALL:   // �S��
    if( ITEM_GetBufParam( dat, ITEM_PRM_HP_RCV ) != 0 )
    {
      GFL_HEAP_FreeMemory( dat );
      return ITEM_TYPE_HP_RCV;  // HP�� ( �m���� )
    }
    else
    {
      GFL_HEAP_FreeMemory( dat );
      return ITEM_TYPE_ALL_ST_RCV;
    }
  }
  // ����������
  if( ITEM_GetBufParam( dat, ITEM_PRM_MEROMERO_RCV ) != 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_MEROMERO_RCV;
  }

  // HP��
  if( ITEM_GetBufParam( dat, ITEM_PRM_HP_RCV ) != 0 )
  {
    if( ITEM_GetBufParam( dat, ITEM_PRM_DEATH_RCV ) != 0)
    {
      GFL_HEAP_FreeMemory( dat );
      return ITEM_TYPE_DEATH_RCV;
    }
    else
    {
      GFL_HEAP_FreeMemory( dat );
      return ITEM_TYPE_HP_RCV;
    }
  }

  // �h�[�s���O�n
  // HP�w�͒l
  prm = ITEM_GetBufParam( dat, ITEM_PRM_HP_EXP_POINT );
  if( prm > 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_HP_UP;
  }
  if( prm < 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_HP_DOWN;
  }
  // �U���w�͒l
  prm = ITEM_GetBufParam( dat, ITEM_PRM_POWER_EXP_POINT );
  if( prm > 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_ATC_UP;
  }
  if( prm < 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_ATC_DOWN;
  }
  // �h��w�͒l
  prm = ITEM_GetBufParam( dat, ITEM_PRM_DEFENCE_EXP_POINT );
  if( prm > 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_DEF_UP;
  }
  if( prm < 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_DEF_DOWN;
  }
  // �f�����w�͒l
  prm = ITEM_GetBufParam( dat, ITEM_PRM_AGILITY_EXP_POINT );
  if( prm > 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_AGL_UP;
  }
  if( prm < 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_AGL_DOWN;
  }
  // ���U�w�͒l
  prm = ITEM_GetBufParam( dat, ITEM_PRM_SP_ATTACK_EXP_POINT );
  if( prm > 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_SPA_UP;
  }
  if( prm < 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_SPA_DOWN;
  }
  // ���h�w�͒l
  prm = ITEM_GetBufParam( dat, ITEM_PRM_SP_DEFENCE_EXP_POINT );
  if( prm > 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_SPD_UP;
  }
  if( prm < 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_SPD_DOWN;
  }

  // �i���n
  if( ITEM_GetBufParam( dat, ITEM_PRM_EVOLUTION ) != 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_EVO;
  }

  // ppUp�n
  if( ITEM_GetBufParam( dat, ITEM_PRM_PP_UP ) != 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_PP_UP;
  }

  // pp3Up�n
  if( ITEM_GetBufParam( dat, ITEM_PRM_PP_3UP ) != 0 )
  {
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_PP_3UP;
  }

  // pp�񕜌n
  if( ITEM_GetBufParam( dat, ITEM_PRM_PP_RCV ) != 0 ||
    ITEM_GetBufParam( dat, ITEM_PRM_ALL_PP_RCV ) != 0 )
  {

    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_PP_RCV;
  }

  GFL_HEAP_FreeMemory( dat );
  return ITEM_TYPE_ETC;
}

//--------------------------------------------------------------------------
//  �Z�I�����K�v���H
//--------------------------------------------------------------------------
const BOOL PLIST_ITEM_IsNeedSelectSkill( PLIST_WORK *work , u16 itemNo )
{
  PLIST_ITEM_USE_TYPE useType = PLIST_ITEM_RecoverCheck( itemNo );
  
  switch( useType )
  {
  case ITEM_TYPE_PP_UP:      // ppUp�n
    return TRUE;
    break;

  case ITEM_TYPE_PP_3UP:     // pp3Up�n
    return TRUE;
    break;

  case ITEM_TYPE_PP_RCV:     // pp�񕜌n
    //�P�̎g�p���H
    if( ITEM_GetParam( itemNo , ITEM_PRM_PP_RCV , work->heapId ) != 0 )
    {
      return TRUE;
    }
    break;  
  }
  return FALSE;
}

//--------------------------------------------------------------------------
//  �S�̕����A�C�e�����H �ƁA���������Ȃ�D���H
//--------------------------------------------------------------------------
const BOOL PLIST_ITEM_IsDeathRecoverAllItem( PLIST_WORK *work , u16 itemNo )
{
  if( ITEM_GetParam( itemNo , ITEM_PRM_ALL_DEATH_RCV , work->heapId ) != 0 )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

//--------------------------------------------------------------------------
//  �S�̕����A�C�e���Ώۂ����邩(-1�ŋ��Ȃ�
//--------------------------------------------------------------------------
const int PLIST_ITEM_CanUseDeathRecoverAllItem( PLIST_WORK *work )
{
  u8 i;
  const u8 partyMax = PokeParty_GetPokeCount( work->plData->pp );
  for( i=0;i<partyMax;i++ )
  {
    POKEMON_PARAM *pp = PokeParty_GetMemberPointer(work->plData->pp, i);
    const BOOL canUse = STATUS_RCV_RecoverCheck( pp , work->plData->item , 0 , work->heapId );
    if( canUse == TRUE )
    {
      return i;
    }
  }
  return -1;
}

//--------------------------------------------------------------------------
//  �A�C�e���g�p���A�Z�I����MSG�ԍ��̎擾
//--------------------------------------------------------------------------
u32 PLIST_ITEM_GetWazaListMessage( PLIST_WORK *work , u16 itemNo )
{
  PLIST_ITEM_USE_TYPE useType = PLIST_ITEM_RecoverCheck( itemNo );
  
  switch( useType )
  {
  case ITEM_TYPE_PP_UP:      // ppUp�n
  case ITEM_TYPE_PP_3UP:     // pp3Up�n
    return mes_pokelist_03_05;  //�ǂ̋Z�𑝂₷�H
    break;

  case ITEM_TYPE_PP_RCV:     // pp�񕜌n
    return mes_pokelist_03_04;  //�ǂ̋Z���񕜂���H
    break;  
  }
  return mes_pokelist_03_04;
}

void PLIST_ITEM_CangeAruseusuForm( PLIST_WORK *work , POKEMON_PARAM *pp , const u16 itemNo )
{
  const u32 monsNo = PP_Get( pp , ID_PARA_monsno , NULL );
  if( monsNo == MONSNO_ARUSEUSU )
  {
    const u32 formNo =  PP_Get( pp , ID_PARA_form_no , NULL );
    const PokeType newForm =  POKETOOL_GetPokeTypeFromItem( itemNo );
    OS_TPrintf("ARUSEUSU:[%d]->[%d]\n",formNo,newForm);
    if( formNo != newForm )
    {
      PP_ChangeFormNo( pp , newForm );
    }
  }
}

#pragma mark [> msg func
//--------------------------------------------------------------------------
//  �A�C�e���g���Ȃ����b�Z�[�W�̕\��
//--------------------------------------------------------------------------
void PLIST_ITEM_MSG_CanNotUseItem( PLIST_WORK *work )
{
  work->plData->ret_mode = PL_RET_BAG;
  PLIST_MSG_CreateWordSet( work , work->msgWork );
  PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 0 , work->plData->item );
  PLIST_MessageWaitInit( work , mes_pokelist_04_45 , TRUE , PLIST_MSGCB_ExitCommon );
  PLIST_MSG_DeleteWordSet( work , work->msgWork );
}

//--------------------------------------------------------------------------
//  �A�C�e���g�������̏���
//--------------------------------------------------------------------------
const PLIST_ITEM_USE_TYPE PLIST_ITEM_MSG_UseItemFunc( PLIST_WORK *work )
{
  PLIST_ITEM_USE_TYPE useType = PLIST_ITEM_RecoverCheck( work->plData->item );
  
  switch( useType )
  {
  case ITEM_TYPE_BTL_ST_UP:  // �퓬�p�X�e�[�^�X�A�b�v�n
    GF_ASSERT_MSG( 0,"�g���ĂȂ��͂��B\n");
    break;

  case ITEM_TYPE_ALLDETH_RCV:    // �S���m����
    work->mainSeq = PSMS_INIT_HPANIME;
    work->befHp = PLIST_PLATE_GetDispHp( work , work->plateWork[work->pokeCursor] );
    work->hpAnimeCallBack = PLIST_HPANMCB_ReturnRecoverAllDeath;

    break;

  case ITEM_TYPE_LV_UP:      // LvUp�n
    {
      u8 i;
      const u32 lv = PP_CalcLevel( work->selectPokePara ); 
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
      PLIST_MSG_AddWordSet_Value( work , work->msgWork , 1 , lv+1 , 3 );
      PLIST_MessageWaitInit( work , mes_pokelist_08_09 , TRUE , PLIST_MSGCB_LvUp );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
      
      //�\���p�Ƀ��x���A�b�v�O�̂̃p�����[�^��ۑ�
      for( i=0;i<6;i++ )
      {
        work->befParam[i] = PP_Get( work->selectPokePara , ID_PARA_hpmax+i , NULL );
      }
      
    }
    break;

  case ITEM_TYPE_NEMURI_RCV:   // �����
    PLIST_ITEM_UTIL_ItemUseMessageCommon( work , mes_pokelist_04_35 );
    break;

  case ITEM_TYPE_DOKU_RCV:     // �ŉ�
    PLIST_ITEM_UTIL_ItemUseMessageCommon( work , mes_pokelist_04_15 );
    break;

  case ITEM_TYPE_YAKEDO_RCV:   // �Ώ���
    PLIST_ITEM_UTIL_ItemUseMessageCommon( work , mes_pokelist_04_17 );
    break;

  case ITEM_TYPE_KOORI_RCV:    // �X��
    PLIST_ITEM_UTIL_ItemUseMessageCommon( work , mes_pokelist_04_18 );
    break;

  case ITEM_TYPE_MAHI_RCV:     // ��჉�
    PLIST_ITEM_UTIL_ItemUseMessageCommon( work , mes_pokelist_04_16 );
    break;

  case ITEM_TYPE_KONRAN_RCV:   // ������
    GF_ASSERT_MSG( 0,"�g���ĂȂ��͂��B\n");
    break;

  case ITEM_TYPE_ALL_ST_RCV:   // �S��
    PLIST_ITEM_UTIL_ItemUseMessageCommon( work , mes_pokelist_04_21 );
    break;

  case ITEM_TYPE_MEROMERO_RCV:   // ����������
    GF_ASSERT_MSG( 0,"�g���ĂȂ��͂��B\n");
    break;

  case ITEM_TYPE_HP_RCV:     // HP��
  case ITEM_TYPE_DEATH_RCV:  // �m���� (WB�ǉ�
    work->mainSeq = PSMS_INIT_HPANIME;
    work->befHp = PLIST_PLATE_GetDispHp( work , work->plateWork[work->pokeCursor] );
    work->hpAnimeCallBack = PLIST_HPANMCB_ReturnRecoverHp;

    break;
    
  case ITEM_TYPE_HP_UP:      // HP�w�͒lUP
  case ITEM_TYPE_ATC_UP:     // �U���w�͒lUP
  case ITEM_TYPE_DEF_UP:     // �h��w�͒lUP
  case ITEM_TYPE_AGL_UP:     // �f�����w�͒lUP
  case ITEM_TYPE_SPA_UP:     // ���U�w�͒lUP
  case ITEM_TYPE_SPD_UP:     // ���h�w�͒lUP
    {
      u16 statusId = useType-ITEM_TYPE_HP_UP;
      
      PLIST_ITEM_UTIL_ItemUseMessageParamExp( work , mes_pokelist_04_25 , statusId );
    }
    break;

  case ITEM_TYPE_HP_DOWN:      // HP�w�͒lDOWN
  case ITEM_TYPE_ATC_DOWN:     // �U���w�͒lDOWN
  case ITEM_TYPE_DEF_DOWN:     // �h��w�͒lDOWN
  case ITEM_TYPE_AGL_DOWN:     // �f�����w�͒lDOWN
  case ITEM_TYPE_SPA_DOWN:     // ���U�w�͒lDOWN
  case ITEM_TYPE_SPD_DOWN:     // ���h�w�͒lDOWN
    {
      u16 statusId = useType-ITEM_TYPE_HP_DOWN;
      
      if( PLIST_ITEM_UTIL_CanSubParamExp( work->selectPokePara , ID_PARA_hp_exp+statusId , work->plData->item ) == FALSE )
      {
        //��b�|�C���g�͉�����Ȃ��I
        PLIST_ITEM_UTIL_ItemUseMessageParamExp( work , mes_pokelist_04_53 , statusId );
      }
      else
      if( PP_Get( work->selectPokePara , ID_PARA_friend , NULL ) == PTL_FRIEND_MAX )
      {
        //�Ȃ��x�͏オ��Ȃ��I
        PLIST_ITEM_UTIL_ItemUseMessageParamExp( work , mes_pokelist_04_52 , statusId );
      }
      else
      {
        PLIST_ITEM_UTIL_ItemUseMessageParamExp( work , mes_pokelist_04_51 , statusId );
      }
    }
    break;

  case ITEM_TYPE_EVO:        // �i���n
    break;
  case ITEM_TYPE_PP_UP:      // ppUp�n
  case ITEM_TYPE_PP_3UP:     // pp3Up�n
    PLIST_ITEM_UTIL_ItemUseMessageCommon( work , mes_pokelist_04_22 );
    break;
  case ITEM_TYPE_PP_RCV:     // pp�񕜌n
    PLIST_ITEM_UTIL_ItemUseMessageCommon( work , mes_pokelist_04_19 );
    break;
  case ITEM_TYPE_ETC:        // ���̑�
    break;
  }
  return useType;
}

#pragma mark [> HPAnime CallBack
void PLIST_HPANMCB_ReturnRecoverHp( PLIST_WORK *work )
{
  u16 nowHp = PLIST_PLATE_GetDispHp( work , work->plateWork[work->pokeCursor] );
  
  work->plData->ret_mode = PL_RET_BAG;
  PLIST_MSG_CreateWordSet( work , work->msgWork );
  PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
  if( work->befHp != 0 )
  {
    PLIST_MSG_AddWordSet_Value( work , work->msgWork , 1 , nowHp-work->befHp , 3 );
    PLIST_MessageWaitInit( work , mes_pokelist_04_14 , TRUE , PLIST_MSGCB_ExitCommon );
  }
  else
  {
    //�m�����畜��
    PLIST_MessageWaitInit( work , mes_pokelist_04_20 , TRUE , PLIST_MSGCB_ExitCommon );
  }
  PLIST_MSG_DeleteWordSet( work , work->msgWork );
}

void PLIST_HPANMCB_ReturnRecoverAllDeath( PLIST_WORK *work )
{
  
  work->plData->ret_mode = PL_RET_BAG;
  PLIST_MSG_CreateWordSet( work , work->msgWork );
  PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
  if( PLIST_ITEM_CanUseDeathRecoverAllItem( work ) != -1 )
  {
    //���ɑΏۂ�����̂ő���
    PLIST_MessageWaitInit( work , mes_pokelist_04_20 , TRUE , PLIST_MSGCB_RecoverAllDeath_NextPoke );
  }
  else
  {
    //���ɑΏۂ����Ȃ��̂ŏI��
    PLIST_MessageWaitInit( work , mes_pokelist_04_20 , TRUE , PLIST_MSGCB_ExitCommon );
  }
  PLIST_MSG_DeleteWordSet( work , work->msgWork );
}

void PLIST_MSGCB_RecoverAllDeath_NextPoke( PLIST_WORK *work )
{
  const int target = PLIST_ITEM_CanUseDeathRecoverAllItem( work );
  
  PLIST_MSG_CloseWindow( work , work->msgWork );
  PLIST_PLATE_ChangeColor( work , work->plateWork[work->pokeCursor] , PPC_NORMAL );

  work->pokeCursor = target;
  work->selectPokePara = PokeParty_GetMemberPointer(work->plData->pp, target);

  PLIST_ITEM_MSG_UseItemFunc( work );
  STATUS_RCV_Recover( work->selectPokePara , work->plData->item , 0 , work->plData->zone_id , work->heapId );
  PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );
}


#pragma mark [> lv up Func

#define PLIST_ITEM_PARAMWIN_LEFT (1)
#define PLIST_ITEM_PARAMWIN_TOP  (1)
#define PLIST_ITEM_PARAMWIN_WIDTH (14)
#define PLIST_ITEM_PARAMWIN_HEIGHT (12)
typedef enum
{
  PIPS_CREATE_WIN_1 = 0,
  PIPS_DISP_WIN_1,
  PIPS_WAIT_KEY_1,
  PIPS_CREATE_WIN_2,
  PIPS_DISP_WIN_2,
  PIPS_WAIT_KEY_2,

  PIPS_CHECK_SHINKA,

}PLIST_ITEM_PARAM_SEQ;

//�ӂ����ȃA�����x���A�b�v�p
static void PLIST_MSGCB_LvUp( PLIST_WORK *work )
{
  work->mainSeq = PSMS_DISP_PARAM;
  work->subSeq = 0;
  //�E�B���h�E���Ȃ��I
  //PLIST_MSG_CloseWindow( work , work->msgWork );

}
void PLIST_MSGCB_LvUp_EvoCheck( PLIST_WORK *work )
{
  work->mainSeq = PSMS_DISP_PARAM;
  work->subSeq = PIPS_CHECK_SHINKA;
  PLIST_MSG_CloseWindow( work , work->msgWork );

}

void PLIST_UpdateDispParam( PLIST_WORK *work )
{
  switch( work->subSeq )
  {
  //WinOpen + �㏸�ʂ̕\��
  case PIPS_CREATE_WIN_1:
    {
      u8 i;
      BOOL isDouble = FALSE;  //2�������������H
      work->paramWin = GFL_BMPWIN_Create( PLIST_BG_MENU , 
                PLIST_ITEM_PARAMWIN_LEFT , PLIST_ITEM_PARAMWIN_TOP , 
                PLIST_ITEM_PARAMWIN_WIDTH , PLIST_ITEM_PARAMWIN_HEIGHT ,
                PLIST_BG_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
      BmpWinFrame_Write( work->paramWin , WINDOW_TRANS_ON_V , 
                          PLIST_BG_WINCHAR_TOP , PLIST_BG_PLT_BMPWIN );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->paramWin ) , 0xf );
      for( i=0;i<6;i++ )
      {
        const u32 param = PP_Get( work->selectPokePara , ID_PARA_hpmax+i , NULL );
        const u32 sub = param - work->befParam[i];
        if( sub >= 10 )
        {
          isDouble = TRUE;
        }
        
        //�p�����[�^����
        {
          STRBUF *str = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_08_01+i ); 
          PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( work->paramWin ) , 
                  0 , i*16 , str , work->fontHandle , PLIST_FONT_COLOR_BLACK );
          GFL_STR_DeleteBuffer( str );
        }
        //���l����
        {
          STRBUF *str = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_08_08 ); 
          STRBUF *workStr = GFL_STR_CreateBuffer( 32 , work->heapId );
          WORDSET *wordSet = WORDSET_Create( work->heapId );
          u16 len;
          
          WORDSET_RegisterNumber( wordSet , 0 , sub , 3 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
          WORDSET_ExpandStr( wordSet , workStr , str );
          
          len = PRINTSYS_GetStrWidth( workStr , work->fontHandle , 0 );
          OS_TPrintf("[%d]\n",len);
          PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( work->paramWin ) , 
                  (PLIST_ITEM_PARAMWIN_WIDTH-1)*8-len , i*16 , workStr , work->fontHandle , PLIST_FONT_COLOR_BLACK );

          GFL_STR_DeleteBuffer( str );
          GFL_STR_DeleteBuffer( workStr );
          WORDSET_Delete( wordSet );
        }
      }
      for( i=0;i<6;i++ )
      {
        //+����
        const u8 posX = (isDouble==TRUE ? (PLIST_ITEM_PARAMWIN_WIDTH-5)*8 : (PLIST_ITEM_PARAMWIN_WIDTH-4)*8 );
        STRBUF *str = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_08_07 ); 
        PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( work->paramWin ) , 
                posX , i*16 , str , work->fontHandle , PLIST_FONT_COLOR_BLACK );
        GFL_STR_DeleteBuffer( str );
      }
      
      work->subSeq = PIPS_DISP_WIN_1;
    }
    break;
    
  case PIPS_DISP_WIN_1:
    if( PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( work->paramWin )) == FALSE )
    {
      GFL_BMPWIN_MakeTransWindow_VBlank( work->paramWin );
      work->subSeq = PIPS_WAIT_KEY_1;
    }
    break;

  case PIPS_WAIT_KEY_1:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ||
        GFL_UI_TP_GetTrg() == TRUE )
    {
      work->subSeq = PIPS_CREATE_WIN_2;
    }
    break;
    
  case PIPS_CREATE_WIN_2:
    {
      u8 i;
      //�����l����
      GFL_BMP_Fill( GFL_BMPWIN_GetBmp( work->paramWin ) ,
                    (PLIST_ITEM_PARAMWIN_WIDTH-4)*8 , 0 ,
                    4*8 , PLIST_ITEM_PARAMWIN_HEIGHT*8 , 0xf );
      for( i=0;i<6;i++ )
      {
        const u32 param = PP_Get( work->selectPokePara , ID_PARA_hpmax+i , NULL );
        //���l����
        STRBUF *str = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_08_08 ); 
        STRBUF *workStr = GFL_STR_CreateBuffer( 32 , work->heapId );
        WORDSET *wordSet = WORDSET_Create( work->heapId );
        u16 len;
        
        WORDSET_RegisterNumber( wordSet , 0 , param , 3 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
        WORDSET_ExpandStr( wordSet , workStr , str );
        
        len = PRINTSYS_GetStrWidth( workStr , work->fontHandle , 0 );
        
        PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( work->paramWin ) , 
                (PLIST_ITEM_PARAMWIN_WIDTH-1)*8-len , i*16 , workStr , work->fontHandle , PLIST_FONT_COLOR_BLACK );

        GFL_STR_DeleteBuffer( str );
        GFL_STR_DeleteBuffer( workStr );
        WORDSET_Delete( wordSet );
        
      }
    }
    work->subSeq = PIPS_DISP_WIN_2;
    break;
  case PIPS_DISP_WIN_2:
    if( PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( work->paramWin )) == FALSE )
    {
      GFL_BMPWIN_MakeTransWindow_VBlank( work->paramWin );
      work->subSeq = PIPS_WAIT_KEY_2;
    }
    break;

  case PIPS_WAIT_KEY_2:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ||
        GFL_UI_TP_GetTrg() == TRUE )
    {
      int idx = 0;
      //�Z�o���`�F�b�N
      WazaID wazaNo = PP_CheckWazaOboe( work->selectPokePara , &idx, work->heapId );
      PLIST_MSG_CloseWindow( work , work->msgWork );
      if( wazaNo == PTL_WAZAOBOE_NONE )
      {
        //����
        work->subSeq = PIPS_CHECK_SHINKA;
      }
      else if( wazaNo == PTL_WAZASET_SAME )
      { 
        //���łɊo���Ă���
        work->subSeq = PIPS_CHECK_SHINKA;
      }
      else if( wazaNo & PTL_WAZAOBOE_FULL )
      {
        work->plData->waza = wazaNo - PTL_WAZAOBOE_FULL;
        //�Z�������ς�
        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
        PLIST_MessageWaitInit( work , mes_pokelist_04_06 , FALSE , PLIST_MSGCB_ForgetSkill_ForgetCheck );
        PLIST_MSG_DeleteWordSet( work , work->msgWork );
      }
      else
      {
        //�Z���o����
        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , wazaNo );
        PLIST_MessageWaitInit( work , mes_pokelist_04_11 , TRUE , PLIST_MSGCB_LvUp_EvoCheck );
        PLIST_MSG_DeleteWordSet( work , work->msgWork );
        //work->msgCallBackSe = PLIST_SND_WAZA_LEARN;
      }
      
    	GFL_BMPWIN_ClearScreen( work->paramWin );
      BmpWinFrame_Clear( work->paramWin , WINDOW_TRANS_ON_V );
      GFL_BMPWIN_Delete( work->paramWin );
      work->paramWin = NULL;
      
    }
    break;

  case PIPS_CHECK_SHINKA:
    {
      const u32 monsNo = PP_Get( work->selectPokePara , ID_PARA_monsno , NULL );
      const u16 evoMonsNo = SHINKA_Check( work->plData->pp , work->selectPokePara , SHINKA_TYPE_LEVELUP , 0 , NULL , work->heapId );
      OS_TFPrintf(3,"[%d][%d]\n",monsNo,evoMonsNo);
      if( evoMonsNo != 0 )
      {
        work->mainSeq = PSMS_FADEOUT;
        work->plData->ret_sel = work->pokeCursor;
        work->plData->ret_mode = PL_RET_LVUPSHINKA;
      }
      else
      {
        work->mainSeq = PSMS_FADEOUT;
        work->plData->ret_sel = work->pokeCursor;
        work->plData->ret_mode = PL_RET_BAG;
      }
      
    }
    break;
  }
}

#pragma mark [> util
//--------------------------------------------------------------------------
//  ��b�|�C���g�̍��v�l�����߂�
//--------------------------------------------------------------------------
static const u16 PLIST_ITEM_UTIL_GetParamExpSum( POKEMON_PARAM *pp )
{
  u16 sum = 0;
  const BOOL isChangeMode = PP_FastModeOn( pp );
  
  sum += PP_Get( pp , ID_PARA_hp_exp , NULL );
  sum += PP_Get( pp , ID_PARA_pow_exp , NULL );
  sum += PP_Get( pp , ID_PARA_def_exp , NULL );
  sum += PP_Get( pp , ID_PARA_agi_exp , NULL );
  sum += PP_Get( pp , ID_PARA_spepow_exp , NULL );
  sum += PP_Get( pp , ID_PARA_spedef_exp , NULL );
  
  PP_FastModeOff( pp , isChangeMode );
  return sum;
}

//--------------------------------------------------------------------------
//  ��b�|�C���g���㏸�ł��邩�H
//--------------------------------------------------------------------------
static const BOOL PLIST_ITEM_UTIL_CanAddParamExp( POKEMON_PARAM *pp , const int id , u16 item )
{
  GF_ASSERT_MSG( id >= ID_PARA_hp_exp && id <= ID_PARA_spedef_exp , "id is invalid!![%d]\n",id );
  
  if( PLIST_ITEM_UTIL_GetParamExpSum(pp) < PARA_EXP_TOTAL_MAX &&
      PP_Get( pp , id , NULL ) < PARA_EXP_ITEM_MAX )
  {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------
//  ��b�|�C���g�������ł��邩�H
//--------------------------------------------------------------------------
static const BOOL PLIST_ITEM_UTIL_CanSubParamExp( POKEMON_PARAM *pp , const int id , u16 item )
{
  GF_ASSERT_MSG( id >= ID_PARA_hp_exp && id <= ID_PARA_spedef_exp , "id is invalid!![%d]\n",id );
  
  if( PP_Get( pp , id , NULL ) > 0 )
  {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------
//  ���݂̂Ȃ��x����Ή�����Ȃ��x�����l���擾
//--------------------------------------------------------------------------
static const s32 PLIST_ITEM_UTIL_GetFriendValue( POKEMON_PARAM *pp , u16 itemNo , HEAPID heapId )
{
  const u32 friend = PP_Get( pp , ID_PARA_friend , NULL );
  if( friend < 100 )
  {
    return ITEM_GetParam( itemNo , ITEM_PRM_FRIEND1 , heapId );
  }
  else
  if( friend < 200 )
  {
    return ITEM_GetParam( itemNo , ITEM_PRM_FRIEND2 , heapId );
  }
  else
  {
    return ITEM_GetParam( itemNo , ITEM_PRM_FRIEND3 , heapId );
  }
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W�\���ėp(WORDSET�P�Ƀj�b�N�l�[�� �\�����ďI��
//--------------------------------------------------------------------------
static void PLIST_ITEM_UTIL_ItemUseMessageCommon( PLIST_WORK *work , u16 msgId )
{
  work->plData->ret_mode = PL_RET_BAG;
  PLIST_MSG_CreateWordSet( work , work->msgWork );
  PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
  PLIST_MessageWaitInit( work , msgId , TRUE , PLIST_MSGCB_ExitCommon );
  PLIST_MSG_DeleteWordSet( work , work->msgWork );
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W�\���w�͒l(WORDSET�P�Ƀj�b�N�l�[�� WORDSET�Q�ɓw�͒l��� �\�����ďI��
//--------------------------------------------------------------------------
static void PLIST_ITEM_UTIL_ItemUseMessageParamExp( PLIST_WORK *work , u16 msgId , u16 statusID )
{
  work->plData->ret_mode = PL_RET_BAG;
  PLIST_MSG_CreateWordSet( work , work->msgWork );
  PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
  PLIST_MSG_AddWordSet_StatusName( work , work->msgWork , 1 , statusID );
  PLIST_MessageWaitInit( work , msgId , TRUE , PLIST_MSGCB_ExitCommon );
  PLIST_MSG_DeleteWordSet( work , work->msgWork );
  
}

