//======================================================================
/**
 * @file	plist_item.c
 * @brief	|PXg ACe`FbNn
 * @author	ariizumi
 * @data	09/08/05
 *
 * W[ΌFPLIST_ITEM
 */
//======================================================================

#pragma once

#include "poke_tool\poke_tool.h"
#include "plist_local_def.h"
//GS©ηθ`ΪA
typedef enum
{
  ITEM_TYPE_BTL_ST_UP = 0,  // ν¬pXe[^XAbvn
  ITEM_TYPE_ALLDETH_RCV,    // Sυmρ
  ITEM_TYPE_LV_UP,      // LvUpn
  ITEM_TYPE_NEMURI_RCV,   // °θρ
  ITEM_TYPE_DOKU_RCV,     // Ερ
  ITEM_TYPE_YAKEDO_RCV,   // Ξρ
  ITEM_TYPE_KOORI_RCV,    // Xρ
  ITEM_TYPE_MAHI_RCV,     // αρ
  ITEM_TYPE_KONRAN_RCV,   // ¬ρ
  ITEM_TYPE_ALL_ST_RCV,   // Sυ
  ITEM_TYPE_MEROMERO_RCV,   // ρ
  ITEM_TYPE_HP_RCV,     // HPρ
  ITEM_TYPE_DEATH_RCV,  // mρ (WBΗΑ
  
  //bZ[WΜXe[^XΖΤΛΆII
  ITEM_TYPE_HP_UP,      // HPwΝlUP
  ITEM_TYPE_ATC_UP,     // UwΝlUP
  ITEM_TYPE_DEF_UP,     // hδwΝlUP
  ITEM_TYPE_AGL_UP,     // f³wΝlUP
  ITEM_TYPE_SPA_UP,     // ΑUwΝlUP
  ITEM_TYPE_SPD_UP,     // ΑhwΝlUP

  //bZ[WΜXe[^XΖΤΛΆII
  ITEM_TYPE_HP_DOWN,      // HPwΝlDOWN
  ITEM_TYPE_ATC_DOWN,     // UwΝlDOWN
  ITEM_TYPE_DEF_DOWN,     // hδwΝlDOWN
  ITEM_TYPE_AGL_DOWN,     // f³wΝlDOWN
  ITEM_TYPE_SPA_DOWN,     // ΑUwΝlDOWN
  ITEM_TYPE_SPD_DOWN,     // ΑhwΝlDOWN

  ITEM_TYPE_EVO,        // i»n
  ITEM_TYPE_PP_UP,      // ppUpn
  ITEM_TYPE_PP_3UP,     // pp3Upn
  ITEM_TYPE_PP_RCV,     // ppρn
  ITEM_TYPE_ETC,        // »ΜΌ
}PLIST_ITEM_USE_TYPE;


extern void PLIST_UpdateDispParam( PLIST_WORK *work );

extern const BOOL PLIST_ITEM_IsDeathRecoverAllItem( PLIST_WORK *work , u16 itemNo );
extern const BOOL PLIST_ITEM_IsNeedSelectSkill( PLIST_WORK *work , u16 itemNo );
//-1ΕΞΫ³΅»κΘOΝΞΫΤ
extern const int PLIST_ITEM_CanUseDeathRecoverAllItem( PLIST_WORK *work );

extern u32 PLIST_ITEM_GetWazaListMessage( PLIST_WORK *work , u16 itemNo );
extern void PLIST_ITEM_CangeAruseusuForm( PLIST_WORK *work , POKEMON_PARAM *pp , const u16 itemNo );
extern void PLIST_ITEM_CangeInsekutaForm( PLIST_WORK *work , POKEMON_PARAM *pp , const u16 itemNo );

extern void PLIST_ITEM_MSG_CanNotUseItem( PLIST_WORK *work );
extern void PLIST_ITEM_MSG_CanNotUseItemContinue( PLIST_WORK *work );
extern const PLIST_ITEM_USE_TYPE PLIST_ITEM_MSG_UseItemFunc( PLIST_WORK *work , const u32 value );

extern void PLIST_MSGCB_LvUp_EvoCheck( PLIST_WORK *work );
extern void PLIST_MSGCB_LvUp_CheckLearnWaza( PLIST_WORK *work );
extern void PLIST_HPANMCB_ReturnRecoverHp( PLIST_WORK *work );
extern void PLIST_HPANMCB_ReturnRecoverAllDeath( PLIST_WORK *work );
extern void PLIST_MSGCB_RecoverAllDeath_NextPoke( PLIST_WORK *work );
