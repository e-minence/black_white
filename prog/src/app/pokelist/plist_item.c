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
  ITEM_TYPE_HP_RCV,     // HP�� ( �m���� )
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
  if( ITEM_GetBufParam( dat, ITEM_PRM_W_TYPE ) != ITEM_WKTYPE_POKEUSE ){
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
    ITEM_GetBufParam( dat, ITEM_PRM_CRITICAL_UP ) != 0 ){

    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_BTL_ST_UP;
  }

  // �S���m����
  if( ITEM_GetBufParam( dat, ITEM_PRM_ALL_DEATH_RCV ) != 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_ALLDETH_RCV;
  }

  // LvUp�n
  if( ITEM_GetBufParam( dat, ITEM_PRM_LV_UP ) != 0 ){
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
  switch( prm ){
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
    if( ITEM_GetBufParam( dat, ITEM_PRM_HP_RCV ) != 0 ){
      GFL_HEAP_FreeMemory( dat );
      return ITEM_TYPE_HP_RCV;  // HP�� ( �m���� )
    }else{
      GFL_HEAP_FreeMemory( dat );
      return ITEM_TYPE_ALL_ST_RCV;
    }
  }
  // ����������
  if( ITEM_GetBufParam( dat, ITEM_PRM_MEROMERO_RCV ) != 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_MEROMERO_RCV;
  }

  // HP�� ( �m���� )
  if( ITEM_GetBufParam( dat, ITEM_PRM_HP_RCV ) != 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_HP_RCV;
  }

  // �h�[�s���O�n
  // HP�w�͒l
  prm = ITEM_GetBufParam( dat, ITEM_PRM_HP_EXP_POINT );
  if( prm > 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_HP_UP;
  }
  if( prm < 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_HP_DOWN;
  }
  // �U���w�͒l
  prm = ITEM_GetBufParam( dat, ITEM_PRM_POWER_EXP_POINT );
  if( prm > 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_ATC_UP;
  }
  if( prm < 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_ATC_DOWN;
  }
  // �h��w�͒l
  prm = ITEM_GetBufParam( dat, ITEM_PRM_DEFENCE_EXP_POINT );
  if( prm > 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_DEF_UP;
  }
  if( prm < 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_DEF_DOWN;
  }
  // �f�����w�͒l
  prm = ITEM_GetBufParam( dat, ITEM_PRM_AGILITY_EXP_POINT );
  if( prm > 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_AGL_UP;
  }
  if( prm < 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_AGL_DOWN;
  }
  // ���U�w�͒l
  prm = ITEM_GetBufParam( dat, ITEM_PRM_SP_ATTACK_EXP_POINT );
  if( prm > 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_SPA_UP;
  }
  if( prm < 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_SPA_DOWN;
  }
  // ���h�w�͒l
  prm = ITEM_GetBufParam( dat, ITEM_PRM_SP_DEFENCE_EXP_POINT );
  if( prm > 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_SPD_UP;
  }
  if( prm < 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_SPD_DOWN;
  }

  // �i���n
  if( ITEM_GetBufParam( dat, ITEM_PRM_EVOLUTION ) != 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_EVO;
  }

  // ppUp�n
  if( ITEM_GetBufParam( dat, ITEM_PRM_PP_UP ) != 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_PP_UP;
  }

  // pp3Up�n
  if( ITEM_GetBufParam( dat, ITEM_PRM_PP_3UP ) != 0 ){
    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_PP_3UP;
  }

  // pp�񕜌n
  if( ITEM_GetBufParam( dat, ITEM_PRM_PP_RCV ) != 0 ||
    ITEM_GetBufParam( dat, ITEM_PRM_ALL_PP_RCV ) != 0 ){

    GFL_HEAP_FreeMemory( dat );
    return ITEM_TYPE_PP_RCV;
  }

  GFL_HEAP_FreeMemory( dat );
  return ITEM_TYPE_ETC;
}
