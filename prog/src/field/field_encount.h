//======================================================================
/**
 * @file  field_encount.h
 * @brief	�t�B�[���h�@�G���J�E���g����
 * @author kagaya
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"

#include "battle/battle.h"
#include "fieldmap.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  proto
//======================================================================
///FIELD_ENCOUNT
typedef struct _TAG_FIELD_ENCOUNT FIELD_ENCOUNT;

//======================================================================
//  extern
//======================================================================
//�����A�폜
extern FIELD_ENCOUNT * FIELD_ENCOUNT_Create( FIELDMAP_WORK *fwork );
extern void FIELD_ENCOUNT_Delete( FIELD_ENCOUNT *enc );

//�`�F�b�N
extern GMEVENT* FIELD_ENCOUNT_CheckEncount( FIELD_ENCOUNT *enc );
//extern BATTLE_SETUP_PARAM* FIELD_ENCOUNT_GetBattleParamPointer( FIELD_ENCOUNT *enc );

extern void FIELD_ENCOUNT_SetTrainerBattleParam(
    FIELD_ENCOUNT *enc, BATTLE_SETUP_PARAM *setup, int tr_id, HEAPID heapID );
