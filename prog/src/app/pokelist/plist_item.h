//======================================================================
/**
 * @file	plist_item.c
 * @brief	�|�P�������X�g �A�C�e���`�F�b�N�n
 * @author	ariizumi
 * @data	09/08/05
 *
 * ���W���[�����FPLIST_ITEM
 */
//======================================================================

#pragma once

#include "poke_tool\poke_tool.h"
#include "plist_local_def.h"

extern const BOOL PLIST_ITEM_IsDeathRecoverAllItem( PLIST_WORK *work , u16 itemNo );
extern const BOOL PLIST_ITEM_IsNeedSelectSkill( PLIST_WORK *work , u16 itemNo );
//-1�őΏۖ�������ȊO�͑Ώ۔ԍ�
extern const int PLIST_ITEM_CanUseDeathRecoverAllItem( PLIST_WORK *work );

extern u32 PLIST_ITEM_GetWazaListMessage( PLIST_WORK *work , u16 itemNo );

extern void PLIST_ITEM_MSG_CanNotUseItem( PLIST_WORK *work );
extern void PLIST_ITEM_MSG_UseItemFunc( PLIST_WORK *work );

extern void PLIST_HPANMCB_ReturnRecoverHp( PLIST_WORK *work );
extern void PLIST_HPANMCB_ReturnRecoverAllDeath( PLIST_WORK *work );
extern void PLIST_MSGCB_RecoverAllDeath_NextPoke( PLIST_WORK *work );
