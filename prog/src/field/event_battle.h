//======================================================================
/**
 * @file	event_battle.h
 * @brief	�C�x���g�F�o�g���Ăяo��
 * @author	tamada GAMEFREAK inc.
 * @date	2009.01.19
 */
//======================================================================
#pragma once
#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

//======================================================================
//  define
//======================================================================

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef enum {
//�Ő�
BATTLE_ATTR_ID_LAWN,
//�ʏ�n��
BATTLE_ATTR_ID_NORMAL_GROUND,
//�n�ʂP
BATTLE_ATTR_ID_GROUND1,
//�n�ʂQ
BATTLE_ATTR_ID_GROUND2,
//��
BATTLE_ATTR_ID_GRASS,
//����
BATTLE_ATTR_ID_WATER,
//�ጴ
BATTLE_ATTR_ID_SNOW,
//���n
BATTLE_ATTR_ID_SAND,
//�󂢎���
BATTLE_ATTR_ID_MARSH,
//���A
BATTLE_ATTR_ID_CAVE,
//�����܂�
BATTLE_ATTR_ID_POOL,
//��
BATTLE_ATTR_ID_SHOAL,
//�X��
BATTLE_ATTR_ID_ICE,
}BATTLE_ATTR_ID;

//======================================================================
//  struct
//======================================================================

//======================================================================
//  extern
//======================================================================
extern GMEVENT * EVENT_WildPokeBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BATTLE_SETUP_PARAM* bp, BOOL sub_event_f );

extern GMEVENT * EVENT_TrainerBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, int tr_id0, int tr_id1, u32 flags );

//--------------------------------------------------------------
/**
 * @brief �u�s�k�����v�Ƃ���ׂ����ǂ����̔���
 * @param result  �o�g���V�X�e�����Ԃ��퓬����
 * @param competitor  �ΐ푊��̎��
 * @return  BOOL  TRUE�̂Ƃ��A�s�k����������ׂ�
 */
//--------------------------------------------------------------
extern BOOL FIELD_BATTLE_IsLoseResult(BtlResult result, BtlCompetitor competitor);

//--------------------------------------------------------------
/**
 * @brief �쐶��@�Đ�R�[�h�`�F�b�N
 * @param result  �o�g���V�X�e�����Ԃ��퓬����
 * @return  SCR_WILD_BTL_RES_XXXX
 */
//--------------------------------------------------------------
extern u8 FIELD_BATTLE_GetWildBattleRevengeCode(BtlResult result);

//--------------------------------------------------------------
/**
 * @brief   �o�g����ʂł̃A�g���r���[�g�w���Ԃ�
 * @param value   �}�b�v�A�g���r���[�g
 * @retval  BATTLE_ATTR_ID
 */
//--------------------------------------------------------------
extern BATTLE_ATTR_ID FIELD_BATTLE_GetBattleAttrID( MAPATTR_VALUE value );

