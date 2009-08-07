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
  ITEM_TYPE_HP_UP,      // HP�w�͒lUP
  ITEM_TYPE_ATC_UP,     // �U���w�͒lUP
  ITEM_TYPE_DEF_UP,     // �h��w�͒lUP
  ITEM_TYPE_AGL_UP,     // �f�����w�͒lUP
  ITEM_TYPE_SPA_UP,     // ���U�w�͒lUP
  ITEM_TYPE_SPD_UP,     // ���h�w�͒lUP

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

static const u16 PLIST_ITEM_UTIL_GetParamExpSum( POKEMON_PARAM *pp );
static const BOOL PLIST_ITEM_UTIL_CanAddParamExp( POKEMON_PARAM *pp , const int id , u16 item );
static const BOOL PLIST_ITEM_UTIL_CanSubParamExp( POKEMON_PARAM *pp , const int id , u16 item );
static const s32 PLIST_ITEM_UTIL_GetFriendValue( POKEMON_PARAM *pp , u16 itemNo , HEAPID heapId );

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
//  �Ώۂ̃|�P�����ɃA�C�e�����g���邩�H
//--------------------------------------------------------------------------
const PLIST_ITEM_USE_CHECK PLIST_ITEM_CanUseRecoverItem( PLIST_WORK *work , u16 itemNo , POKEMON_PARAM *pp )
{
  PLIST_ITEM_USE_TYPE useType = PLIST_ITEM_RecoverCheck( itemNo );
  
  switch( useType )
  {
  case ITEM_TYPE_BTL_ST_UP:  // �퓬�p�X�e�[�^�X�A�b�v�n
    return PIUC_NG;
    break;

  case ITEM_TYPE_ALLDETH_RCV:    // �S���m����
    //������ʂ鏈������Ȃ�
    return PIUC_NG;
    break;

  case ITEM_TYPE_LV_UP:      // LvUp�n
    if( PP_CalcLevel( pp ) < PTL_LEVEL_MAX )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_NEMURI_RCV:   // �����
    if( PP_GetSick( pp ) == POKESICK_NEMURI )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_DOKU_RCV:     // �ŉ�
    if( PP_GetSick( pp ) == POKESICK_DOKU )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_YAKEDO_RCV:   // �Ώ���
    if( PP_GetSick( pp ) == POKESICK_YAKEDO )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_KOORI_RCV:    // �X��
    if( PP_GetSick( pp ) == POKESICK_KOORI )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_MAHI_RCV:     // ��჉�
    if( PP_GetSick( pp ) == POKESICK_MAHI )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_KONRAN_RCV:   // ������
    //�o�g���ُ݂̂̈�
    return PIUC_NG;
    break;

  case ITEM_TYPE_ALL_ST_RCV:   // �S��
    if( PP_GetSick( pp ) != POKESICK_NULL )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_MEROMERO_RCV:   // ����������
    //�o�g���ُ݂̂̈�
    return PIUC_NG;
    break;
  
  case ITEM_TYPE_DEATH_RCV:  // �m���� (WB�ǉ�
    if( PP_Get( pp , ID_PARA_hp , NULL ) == 0 )
    {
      return PIUC_OK;
    }
    break;
  
  case ITEM_TYPE_HP_RCV:     // HP��
    //�m���͊܂܂Ȃ��I
    {
      const u32 hp = PP_Get( pp , ID_PARA_hp , NULL );
      const u32 hpmax = PP_Get( pp , ID_PARA_hpmax , NULL );
      if( hp != 0 && hpmax != hp )
      {
        return PIUC_OK;
      }
    }
    break;

  case ITEM_TYPE_HP_UP:      // HP�w�͒lUP
    if( PLIST_ITEM_UTIL_CanAddParamExp( pp , ID_PARA_hp_exp , itemNo ) == TRUE )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_ATC_UP:     // �U���w�͒lUP
    if( PLIST_ITEM_UTIL_CanAddParamExp( pp , ID_PARA_pow_exp , itemNo ) == TRUE )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_DEF_UP:     // �h��w�͒lUP
    if( PLIST_ITEM_UTIL_CanAddParamExp( pp , ID_PARA_def_exp , itemNo ) == TRUE )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_AGL_UP:     // �f�����w�͒lUP
    if( PLIST_ITEM_UTIL_CanAddParamExp( pp , ID_PARA_agi_exp , itemNo ) == TRUE )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_SPA_UP:     // ���U�w�͒lUP
    if( PLIST_ITEM_UTIL_CanAddParamExp( pp , ID_PARA_spepow_exp , itemNo ) == TRUE )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_SPD_UP:     // ���h�w�͒lUP
    if( PLIST_ITEM_UTIL_CanAddParamExp( pp , ID_PARA_spedef_exp , itemNo ) == TRUE )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_HP_DOWN:      // HP�w�͒lDOWN
    if( PLIST_ITEM_UTIL_CanSubParamExp(  pp , ID_PARA_hp_exp , itemNo ) == TRUE )
    {
      return PIUC_OK;
    }
    //�Ȃ��x�ω��������
    if( PLIST_ITEM_UTIL_GetFriendValue( pp , itemNo , work->heapId ) != 0 &&
        PP_Get( pp , ID_PARA_friend , NULL ) < PTL_FRIEND_MAX )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_ATC_DOWN:     // �U���w�͒lDOWN
    if( PLIST_ITEM_UTIL_CanSubParamExp(  pp , ID_PARA_pow_exp , itemNo ) == TRUE )
    {
      return PIUC_OK;
    }
    //�Ȃ��x�ω��������
    if( PLIST_ITEM_UTIL_GetFriendValue( pp , itemNo , work->heapId ) != 0 &&
        PP_Get( pp , ID_PARA_friend , NULL ) < PTL_FRIEND_MAX )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_DEF_DOWN:     // �h��w�͒lDOWN
    if( PLIST_ITEM_UTIL_CanSubParamExp(  pp , ID_PARA_def_exp , itemNo ) == TRUE )
    {
      return PIUC_OK;
    }
    //�Ȃ��x�ω��������
    if( PLIST_ITEM_UTIL_GetFriendValue( pp , itemNo , work->heapId ) != 0 &&
        PP_Get( pp , ID_PARA_friend , NULL ) < PTL_FRIEND_MAX )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_AGL_DOWN:     // �f�����w�͒lDOWN
    if( PLIST_ITEM_UTIL_CanSubParamExp(  pp , ID_PARA_agi_exp , itemNo ) == TRUE )
    {
      return PIUC_OK;
    }
    //�Ȃ��x�ω��������
    if( PLIST_ITEM_UTIL_GetFriendValue( pp , itemNo , work->heapId ) != 0 &&
        PP_Get( pp , ID_PARA_friend , NULL ) < PTL_FRIEND_MAX )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_SPA_DOWN:     // ���U�w�͒lDOWN
    if( PLIST_ITEM_UTIL_CanSubParamExp(  pp , ID_PARA_spepow_exp , itemNo ) == TRUE )
    {
      return PIUC_OK;
    }
    //�Ȃ��x�ω��������
    if( PLIST_ITEM_UTIL_GetFriendValue( pp , itemNo , work->heapId ) != 0 &&
        PP_Get( pp , ID_PARA_friend , NULL ) < PTL_FRIEND_MAX )
    {
      return PIUC_OK;
    }
    break;

  case ITEM_TYPE_SPD_DOWN:     // ���h�w�͒lDOWN
    if( PLIST_ITEM_UTIL_CanSubParamExp(  pp , ID_PARA_spedef_exp , itemNo ) == TRUE )
    {
      return PIUC_OK;
    }
    //�Ȃ��x�ω��������
    if( PLIST_ITEM_UTIL_GetFriendValue( pp , itemNo , work->heapId ) != 0 &&
        PP_Get( pp , ID_PARA_friend , NULL ) < PTL_FRIEND_MAX )
    {
      return PIUC_OK;
    }
    break;


  case ITEM_TYPE_EVO:        // �i���n
    //�����ōs�������ł͂Ȃ��I
    return PIUC_NG;
    break;

  case ITEM_TYPE_PP_UP:      // ppUp�n
    //�Ƃ肠�����Z�̃`�F�b�N
    return PIUC_SELECT_SKILL;
    break;

  case ITEM_TYPE_PP_3UP:     // pp3Up�n
    //�Ƃ肠�����Z�̃`�F�b�N
    return PIUC_SELECT_SKILL;
    break;

  case ITEM_TYPE_PP_RCV:     // pp�񕜌n
    if( ITEM_GetParam( itemNo , ITEM_PRM_PP_RCV , work->heapId ) != 0 )
    {
      //�Ƃ肠�����Z�̃`�F�b�N
      return PIUC_SELECT_SKILL;
    }
    else
    if( ITEM_GetParam( itemNo , ITEM_PRM_ALL_PP_RCV , work->heapId ) != 0 )
    {
      const BOOL isChangeMode = PP_FastModeOn( pp );
      if( PP_Get( pp , ID_PARA_pp1 , NULL ) != PP_Get( pp , ID_PARA_pp_max1 , NULL ) ||
          PP_Get( pp , ID_PARA_pp2 , NULL ) != PP_Get( pp , ID_PARA_pp_max2 , NULL ) ||
          PP_Get( pp , ID_PARA_pp3 , NULL ) != PP_Get( pp , ID_PARA_pp_max3 , NULL ) ||
          PP_Get( pp , ID_PARA_pp4 , NULL ) != PP_Get( pp , ID_PARA_pp_max4 , NULL ) )
        {
          PP_FastModeOff( pp , isChangeMode );
          return PIUC_OK;
        }
      PP_FastModeOff( pp , isChangeMode );
    }
    GF_ASSERT_MSG(0,"ITEM_TYPE_PP_RCV item param invalid!!\n");
    break;

  case ITEM_TYPE_ETC:        // ���̑�
    return PIUC_NG;
    break;
  }
  return PIUC_NG;
  
}

#pragma mark [> use func
//--------------------------------------------------------------------------
//  �S�̕����A�C�e�� �ƁA���������Ȃ�D�̎g�p
//--------------------------------------------------------------------------
void PLIST_ITEM_UseAllDeathRecoverItem( PLIST_WORK *work )
{
  /*
  PLIST_ITEM_USE_TYPE useType = PLIST_ITEM_RecoverCheck( itemNo );
  
  switch( useType )
  {
  case ITEM_TYPE_BTL_ST_UP:  // �퓬�p�X�e�[�^�X�A�b�v�n
    break;

  case ITEM_TYPE_ALLDETH_RCV:    // �S���m����
    break;

  case ITEM_TYPE_LV_UP:      // LvUp�n
    break;

  case ITEM_TYPE_NEMURI_RCV:   // �����
    break;

  case ITEM_TYPE_DOKU_RCV:     // �ŉ�
    break;

  case ITEM_TYPE_YAKEDO_RCV:   // �Ώ���
    break;

  case ITEM_TYPE_KOORI_RCV:    // �X��
    break;

  case ITEM_TYPE_MAHI_RCV:     // ��჉�
    break;

  case ITEM_TYPE_KONRAN_RCV:   // ������
    break;

  case ITEM_TYPE_ALL_ST_RCV:   // �S��
    break;

  case ITEM_TYPE_MEROMERO_RCV:   // ����������
    break;
  
  case ITEM_TYPE_DEATH_RCV:  // �m���� (WB�ǉ�
    break;
  
  case ITEM_TYPE_HP_RCV:     // HP��
    break;

  case ITEM_TYPE_HP_UP:      // HP�w�͒lUP
    break;

  case ITEM_TYPE_ATC_UP:     // �U���w�͒lUP
    break;

  case ITEM_TYPE_DEF_UP:     // �h��w�͒lUP
    break;

  case ITEM_TYPE_AGL_UP:     // �f�����w�͒lUP
    break;

  case ITEM_TYPE_SPA_UP:     // ���U�w�͒lUP
    break;

  case ITEM_TYPE_SPD_UP:     // ���h�w�͒lUP
    break;

  case ITEM_TYPE_HP_DOWN:      // HP�w�͒lDOWN
    break;

  case ITEM_TYPE_ATC_DOWN:     // �U���w�͒lDOWN
    break;

  case ITEM_TYPE_DEF_DOWN:     // �h��w�͒lDOWN
    break;

  case ITEM_TYPE_AGL_DOWN:     // �f�����w�͒lDOWN
    break;

  case ITEM_TYPE_SPA_DOWN:     // ���U�w�͒lDOWN
    break;

  case ITEM_TYPE_SPD_DOWN:     // ���h�w�͒lDOWN
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
  
  GF_ASSERT_MSG
  PLIST_ITEM_MSG_CanNotUseItem( work );
  */
}

//--------------------------------------------------------------------------
//  �ʏ�A�C�e���̎d�l(�Z�P�̎g�p�E�S�̕����ȊO
//--------------------------------------------------------------------------
void PLIST_ITEM_UseRecoverItem( PLIST_WORK *work )
{
  
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
