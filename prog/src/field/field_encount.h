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
#include "field/fieldmap_proc.h"

#include "field/encount_data.h"   //ENCOUNT_TYPE

//======================================================================
//  define
//======================================================================

//======================================================================
//  proto
//======================================================================
///FIELD_ENCOUNT
typedef struct _TAG_FIELD_ENCOUNT FIELD_ENCOUNT;
typedef struct _TAG_ENCOUNT_WORK ENCOUNT_WORK;

//======================================================================
//  extern
//======================================================================
//--------------------------------------------------------------
/**
 * @brief   ENCOUNT_WORK�擾
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @return  ENCOUNT_WORK*
 *
 * ���Ԃ�src/gamesystem/game_data.c�ɂ��邪�A���B����
 * �l�����ăA�N�Z�X�֐��Q�������w�b�_�ɔz�u���Ă���
 */
//--------------------------------------------------------------
extern ENCOUNT_WORK* GAMEDATA_GetEncountWork( GAMEDATA *gamedata );

/////////////////////////////////////////////////////////////
//�ʏ�G���J�E���g�n field_encount.c

//�����A�폜
extern FIELD_ENCOUNT * FIELD_ENCOUNT_Create( FIELDMAP_WORK *fwork );
extern void FIELD_ENCOUNT_Delete( FIELD_ENCOUNT *enc );
extern ENCOUNT_WORK * ENCOUNT_WORK_Create( HEAPID heapID );
extern void ENCOUNT_WORK_Delete( ENCOUNT_WORK* wp );

//�`�F�b�N
extern void* FIELD_ENCOUNT_CheckEncount( FIELD_ENCOUNT *enc, ENCOUNT_TYPE enc_type );
extern void* FIELD_ENCOUNT_SetWildEncount( FIELD_ENCOUNT *enc, u16 mons_no, u8 mons_lv, u16 flags );
extern void* FIELD_ENCOUNT_CheckFishingEncount( FIELD_ENCOUNT *enc, ENCOUNT_TYPE enc_type );

extern void FIELD_ENCOUNT_SetTrainerBattleParam(
    FIELD_ENCOUNT *enc, BATTLE_SETUP_PARAM *setup, int tr_id0, int tr_id1, HEAPID heapID );


/////////////////////////////////////////////////////////////
//�G�t�F�N�g�G���J�E���g�n effect_encount.c
extern void EFFECT_ENC_CheckEffectEncountStart( FIELD_ENCOUNT* enc);


