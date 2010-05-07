///////////////////////////////////////////////////////////////////////////
/**
 * @brief  �Q�[�����}�j���A���̌Ăяo���C�x���g
 * @file   event_game_manual.c
 * @author obata
 * @date   2010.04.27
 */
///////////////////////////////////////////////////////////////////////////
#pragma once
#include "gamesystem/gamesystem.h" 
#include "gamesystem/game_event.h" 


//-------------------------------------------------------------------------
/**
 * @brief �Q�[�����}�j���A���̌Ăяo���C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param ret_wk     �I�����ʂ��i�[���郏�[�N�ւ̃|�C���^
 *
 * @return �Q�[�����}�j���A���Ăяo���C�x���g
 */
//-------------------------------------------------------------------------
extern GMEVENT* EVENT_GameManual( GAMESYS_WORK* gameSystem, u16* ret_wk );
