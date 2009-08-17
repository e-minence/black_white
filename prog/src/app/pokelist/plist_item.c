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

#include "plist_sys.h"
#include "plist_plate.h"
#include "plist_message.h"
#include "plist_item.h"

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
//GS�����`�ڐA
typedef enum
{
  ITEM_TYPE_BTL_ST_UP = 0,  // �퓬�p�X�e�[�^�X�A�b�v�n
  ITEM_TYPE_ALLDETH_RCV,    // �S���m����
  ITEM_TYPE_LV_UP,      // LvUp�n
  ITEM_TYPE_NEMURI_RCV,   // �����
  ITEM_TYPE_DOKU_RCV,     // �ŉ�
  ITEM_TYPE_YAKEDO_RCV,   // �Ώ���
  ITEM_TYPE_KOORI_RCV,    // �X��
  ITEM_TYPE_MAHI_RCV,     // ��჉�
  ITEM_TYPE_KONRAN_RCV,   // ������
  ITEM_TYPE_ALL_ST_RCV,   // �S��
  ITEM_TYPE_MEROMERO_RCV,   // ����������
  ITEM_TYPE_HP_RCV,     // HP��
  ITEM_TYPE_DEATH_RCV,  // �m���� (WB�ǉ�
  
  //���b�Z�[�W�̃X�e�[�^�X�Ə��Ԉˑ��I�I
  ITEM_TYPE_HP_UP,      // HP�w�͒lUP
  ITEM_TYPE_ATC_UP,     // �U���w�͒lUP
  ITEM_TYPE_DEF_UP,     // �h��w�͒lUP
  ITEM_TYPE_AGL_UP,     // �f�����w�͒lUP
  ITEM_TYPE_SPA_UP,     // ���U�w�͒lUP
  ITEM_TYPE_SPD_UP,     // ���h�w�͒lUP

  //���b�Z�[�W�̃X�e�[�^�X�Ə��Ԉˑ��I�I
  ITEM_TYPE_HP_DOWN,      // HP�w�͒lDOWN
  ITEM_TYPE_ATC_DOWN,     // �U���w�͒lDOWN
  ITEM_TYPE_DEF_DOWN,     // �h��w�͒lDOWN
  ITEM_TYPE_AGL_DOWN,     // �f�����w�͒lDOWN
  ITEM_TYPE_SPA_DOWN,     // ���U�w�͒lDOWN
  ITEM_TYPE_SPD_DOWN,     // ���h�w�͒lDOWN

  ITEM_TYPE_EVO,        // �i���n
  ITEM_TYPE_PP_UP,      // ppUp�n
  ITEM_TYPE_PP_3UP,     // pp3Up�n
  ITEM_TYPE_PP_RCV,     // pp�񕜌n
  ITEM_TYPE_ETC,        // ���̑�
}PLIST_ITEM_USE_TYPE;


//======================================================================
//	typedef struct
//======================================================================

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static u8 PLIST_ITEM_RecoverCheck( u16 item );

static void PSTATUS_HPANMCB_ReturnRecoverHp( PLIST_WORK *work );

static const u16 PLIST_ITEM_UTIL_GetParamExpSum( POKEMON_PARAM *pp );
static const BOOL PLIST_ITEM_UTIL_CanAddParamExp( POKEMON_PARAM *pp , const int id , u16 item );
static const BOOL PLIST_ITEM_UTIL_CanSubParamExp( POKEMON_PARAM *pp , const int id , u16 item );
static const s32 PLIST_ITEM_UTIL_GetFriendValue( POKEMON_PARAM *pp , u16 itemNo , HEAPID heapId );
static void PLIST_ITEM_UTIL_ItemUseMessageCommon( PLIST_WORK *work , u16 msgId );
static void PLIST_ITEM_UTIL_ItemUseMessageParamExp( PLIST_WORK *work , u16 msgId , u16 statusID );

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

#pragma mark [> msg func
//--------------------------------------------------------------------------
//  �A�C�e���g���Ȃ����b�Z�[�W�̕\��
//--------------------------------------------------------------------------
void PLIST_ITEM_MSG_CanNotUseItem( PLIST_WORK *work )
{
  work->plData->ret_mode = PL_RET_BAG;
  PLIST_MSG_CreateWordSet( work , work->msgWork );
  PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 0 , work->plData->item );
  PLIST_MessageWaitInit( work , mes_pokelist_04_45 , TRUE , PSTATUS_MSGCB_ExitCommon );
  PLIST_MSG_DeleteWordSet( work , work->msgWork );
}

//--------------------------------------------------------------------------
//  �A�C�e���g�������̏���
//--------------------------------------------------------------------------
void PLIST_ITEM_MSG_UseItemFunc( PLIST_WORK *work )
{
  PLIST_ITEM_USE_TYPE useType = PLIST_ITEM_RecoverCheck( work->plData->item );
  
  switch( useType )
  {
  case ITEM_TYPE_BTL_ST_UP:  // �퓬�p�X�e�[�^�X�A�b�v�n
    GF_ASSERT_MSG( 0,"�g���ĂȂ��͂��B\n");
    break;

  case ITEM_TYPE_ALLDETH_RCV:    // �S���m����
    break;

  case ITEM_TYPE_LV_UP:      // LvUp�n
    break;

  case ITEM_TYPE_NEMURI_RCV:   // �����
    GF_ASSERT_MSG( 0,"�g���ĂȂ��͂��B\n");
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
    work->hpAnimeCallBack = PSTATUS_HPANMCB_ReturnRecoverHp;

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
    break;
  case ITEM_TYPE_PP_3UP:     // pp3Up�n
    break;
  case ITEM_TYPE_PP_RCV:     // pp�񕜌n
    break;
  case ITEM_TYPE_ETC:        // ���̑�
    break;
  }
  
}

#pragma mark [> HPAnime CallBack
static void PSTATUS_HPANMCB_ReturnRecoverHp( PLIST_WORK *work )
{
  u16 nowHp = PLIST_PLATE_GetDispHp( work , work->plateWork[work->pokeCursor] );
  
  work->plData->ret_mode = PL_RET_BAG;
  PLIST_MSG_CreateWordSet( work , work->msgWork );
  PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
  if( work->befHp != 0 )
  {
    PLIST_MSG_AddWordSet_Value( work , work->msgWork , 1 , nowHp-work->befHp , 3 );
    PLIST_MessageWaitInit( work , mes_pokelist_04_14 , TRUE , PSTATUS_MSGCB_ExitCommon );
  }
  else
  {
    //�m�����畜��
    PLIST_MessageWaitInit( work , mes_pokelist_04_20 , TRUE , PSTATUS_MSGCB_ExitCommon );
  }
  PLIST_MSG_DeleteWordSet( work , work->msgWork );
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
  PLIST_MessageWaitInit( work , msgId , TRUE , PSTATUS_MSGCB_ExitCommon );
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
  PLIST_MessageWaitInit( work , msgId , TRUE , PSTATUS_MSGCB_ExitCommon );
  PLIST_MSG_DeleteWordSet( work , work->msgWork );
  
}
