//==============================================================================
/**
 * @file    event_colosseum_battle.h
 * @brief   �R���V�A���o�g���C�x���g
 * @author  matsuda
 * @date    2009.07.22(��)
 */
//==============================================================================
#pragma once

#include "demo/comm_btl_demo.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///�R���V�A���퓬�p�p�����[�^
typedef struct{
  POKEPARTY*      partyPlayer;  ///< �v���C���[�̃p�[�e�B
  POKEPARTY*      partyPartner; ///< 2vs2���̖���AI�i�s�v�Ȃ�null�j
  POKEPARTY*      partyEnemy1;  ///< 1vs1���̓GAI, 2vs2���̂P�ԖړGAI�p
  POKEPARTY*      partyEnemy2;  ///< 2vs2���̂Q�ԖړGAI�p�i�s�v�Ȃ�null�j
  u16             music_no;     ///< �퓬��
  u8              standing_pos; ///< �����ʒu
  u8              padding;
  REGULATION      *regulation;  ///<�o�g�����M�����[�V����
  
  //�f���p
  COMM_BTL_DEMO_PARAM demo;     ///< �o�g���J�n�E�I���f���pParentWork
}COLOSSEUM_BATTLE_SETUP;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern GMEVENT* EVENT_ColosseumBattle(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, UNION_PLAY_CATEGORY play_category, COLOSSEUM_BATTLE_SETUP *setup);

