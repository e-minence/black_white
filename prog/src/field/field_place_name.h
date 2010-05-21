///////////////////////////////////////////////////////////////////////////////
/**
 * @file   field_place_name.h
 * @brief  �n���\���E�B���h�E
 * @author obata_toshihiro
 * @date   2009.07
 */
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>
#include "field/field_msgbg.h"
#include "gamesystem/gamesystem.h"


#ifdef PM_DEBUG
extern BOOL PlaceNameEnable; // FALSE �ɂ���Ɠ��삵�Ȃ��Ȃ�
#endif 

typedef struct _FIELD_PLACE_NAME FIELD_PLACE_NAME;

// �����E�j��
extern FIELD_PLACE_NAME* FIELD_PLACE_NAME_Create( GAMESYS_WORK* gameSystem, HEAPID heapID, FLDMSGBG* msgbg );
extern void FIELD_PLACE_NAME_Delete( FIELD_PLACE_NAME* system );
// ���C���E�`��
extern void FIELD_PLACE_NAME_Process( FIELD_PLACE_NAME* system );
extern void FIELD_PLACE_NAME_Draw( const FIELD_PLACE_NAME* system );
// ����
extern void FIELD_PLACE_NAME_DisplayOnStanderdRule( FIELD_PLACE_NAME* system, u32 zoneID );
extern void FIELD_PLACE_NAME_DisplayOnPlaceNameFlag( FIELD_PLACE_NAME* system, u32 zoneID );
extern void FIELD_PLACE_NAME_DisplayForce( FIELD_PLACE_NAME* system, u32 zoneID );
extern void FIELD_PLACE_NAME_Hide( FIELD_PLACE_NAME* system );
extern void FIELD_PLACE_NAME_RecoverBG( FIELD_PLACE_NAME* system );
