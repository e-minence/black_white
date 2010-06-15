//=============================================================================
/**
 *
 *	@file		event_debug_btl_all_waza_check2.c
 *	@brief  �o�g���őS�Z�`�F�b�N(�g���v��)
 *	@author		hosaka genya
 *	@data		2010.06.10
 *
 */
//=============================================================================
#ifdef PM_DEBUG

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "system/main.h"
#include "poke_tool/poke_memo.h"
#include "field/fieldmap_proc.h"
#include "field/field_msgbg.h"
#include "field/zonedata.h"
#include "debug/debug_str_conv.h"
#include "event_debug_local.h"
#include "fieldmap.h"

#include "message.naix"
#include "font/font.naix"
#include "msg/debug/msg_d_field.h"

#include "poke_tool/status_rcv.h" // for STATUS_RCV_PokeParty_RecoverAll

#include "event_fieldmap_control.h"
#include "event_debug_btl_all_waza_check2.h"

#include "field/event_battle.h" // for EVENT_WildPokeBattle
#include "field/field_encount.h" // for BTL_FIELD_SITUATION_SetFromFieldStatus
#include "gamesystem/btl_setup.h" // for BATTLE_PARAM_Create
#include "waza_tool/wazano_def.h" // for WAZANO_
#include "waza_tool/wazadata.h" // for WAZADATA_GetCategory

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================
//--------------------------------------------------------------
///	
//==============================================================
typedef struct {
  GAMESYS_WORK*   gsys;
  int count;
} DEBUG_BTL_ALL_WAZA_CHECK2_WORK;

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static GMEVENT_RESULT BtlAllWazaCheck2( GMEVENT* event, int* seq, void* wk_adrs );

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �S�Z�`�F�b�N�C�x���g�𐶐�����
 *
 *	@param	GAMESYS_WORK* gsys
 *	@param	wk_adrs 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
GMEVENT* EVENT_DEBUG_BtlAllWazaCheck2( GAMESYS_WORK* gsys, void* wk_adrs )
{
  GMEVENT* event;
  DEBUG_BTL_ALL_WAZA_CHECK2_WORK* wk;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gsys );

  event = GMEVENT_Create( gsys, NULL, BtlAllWazaCheck2, sizeof(DEBUG_BTL_ALL_WAZA_CHECK2_WORK) );

  wk = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( wk, sizeof(DEBUG_BTL_ALL_WAZA_CHECK2_WORK) );
  wk->gsys = gsys;

  return event;
}


//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �S�Z�`�F�b�N�C�x���g
 *
 *	@param	GMEVENT* event
 *	@param	seq
 *	@param	wk_adrs 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT BtlAllWazaCheck2( GMEVENT* event, int* seq, void* wk_adrs )
{
  enum
  {
    SEQ_INIT = 0,
    SEQ_SETUP,
    SEQ_RET,
    SEQ_EXIT,
  };
  
  int i;
  DEBUG_BTL_ALL_WAZA_CHECK2_WORK* wk = wk_adrs;
  GAMESYS_WORK* gsys = wk->gsys;
  GAMEDATA* gdata = GAMESYSTEM_GetGameData(gsys);

  switch( *seq )
  {
    case SEQ_INIT:
      wk->count = 1;//35;
      OS_TPrintf("�g���v���S�����S�Z�`�F�b�N�J�n�I\n");
      *seq = SEQ_SETUP;
      break;

    case SEQ_SETUP:

    {
      POKEPARTY* mp;
      
      mp = GAMEDATA_GetMyPokemon(gdata);

      // �S�X�e�[�^�X��
      STATUS_RCV_PokeParty_RecoverAll( mp );

      // �����̃|�P�����������㏑��
      for( i=0; i<PokeParty_GetPokeCount( mp ); i++ )
      {
        POKEMON_PARAM* pp;
        pp = PokeParty_GetMemberPointer( mp, i );

        PP_ChangeMonsNo( pp, 1+i );
        PP_SetWazaPos( pp, wk->count, 0 );
      }
    }

    OS_TPrintf("�Z�b�g�����ZID=%d\n",wk->count);
    
    // �o�g���Ăяo��
    {
      FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
      HEAPID heapID = FIELDMAP_GetHeapID( fieldmap );
      BTL_FIELD_SITUATION sit;
      BATTLE_SETUP_PARAM* bp;
      GMEVENT * new_event;

      bp = BATTLE_PARAM_Create( HEAPID_PROC );

      BTL_FIELD_SITUATION_SetFromFieldStatus( &sit, gdata, fieldmap );
      BTL_SETUP_Triple_Trainer( bp, gdata, &sit, TRID_BIGFOUR3_01, HEAPID_PROC );

      // �G�̋Z���㏑��
      {
        POKEPARTY* ep = bp->party[ BTL_CLIENT_ENEMY1 ];
        for( i=0; i<PokeParty_GetPokeCount( ep ); i++ )
        {
          POKEMON_PARAM* pp;
          pp = PokeParty_GetMemberPointer( ep, i );
          PP_SetWazaPos( pp, wk->count, 0 );
        }
      }

      // HP/PP���� ��Γ�����
      BTL_SETUP_SetDebugFlag( bp, BTL_DEBUGFLAG_HP_CONST );
      BTL_SETUP_SetDebugFlag( bp, BTL_DEBUGFLAG_PP_CONST );
      BTL_SETUP_SetDebugFlag( bp, BTL_DEBUGFLAG_HIT100PER );
      BTL_SETUP_SetDebugFlag( bp, BTL_DEBUGFLAG_AI_CTRL );
      BTL_SETUP_SetDebugFlag( bp, BTL_DEBUGFLAG_SKIP_BTLIN ); // �o��A�j����΂�
      
      new_event = EVENT_WildPokeBattle( gsys, fieldmap, bp, FALSE, ENC_TYPE_NORMAL );
    
      GMEVENT_CallEvent( event, new_event );
    }
      *seq = SEQ_RET;
      break;

    case SEQ_RET:
      // ���̋Z��
      wk->count++;

      // �X�L�b�v ���΂��A�����΂��͂A�ꌂ�K�E�Z
      if( wk->count == WAZANO_ZIBAKU ||
          wk->count == WAZANO_DAIBAKUHATU ||
          wk->count == WAZANO_KANASIBARI ||
          WAZADATA_GetCategory( wk->count ) == WAZADATA_CATEGORY_ICHIGEKI )
      {
        wk->count++;
      }

      if( wk->count < WAZANO_MAX )
      {
        *seq = SEQ_SETUP;
      }
      else
      {
        *seq = SEQ_EXIT;
      }
      break;

    case SEQ_EXIT:
      OS_TPrintf("�g���v���S�����S�Z�`�F�b�N�I���I\n");
      return GMEVENT_RES_FINISH;
    default : GF_ASSERT(0);
  }

  return GMEVENT_RES_CONTINUE;
}

#endif // PM_DEBUG
