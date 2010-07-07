//=============================================================================
/**
 *
 *	@file		event_debug_btl_poke_change.c
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
#include "event_debug_btl_poke_change.h"

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
  u32 poke_list[ MONSNO_END ];
} DEBUG_BTL_POKE_CHANGE_WORK;

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static GMEVENT_RESULT BtlPokeChange( GMEVENT* event, int* seq, void* wk_adrs );

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
GMEVENT* EVENT_DEBUG_BtlPokeChange( GAMESYS_WORK* gsys, void* wk_adrs )
{
  GMEVENT* event;
  DEBUG_BTL_POKE_CHANGE_WORK* wk;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gsys );

  event = GMEVENT_Create( gsys, NULL, BtlPokeChange, sizeof(DEBUG_BTL_POKE_CHANGE_WORK) );

  wk = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( wk, sizeof(DEBUG_BTL_POKE_CHANGE_WORK) );
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
static GMEVENT_RESULT BtlPokeChange( GMEVENT* event, int* seq, void* wk_adrs )
{
  enum
  {
    SEQ_INIT = 0,
    SEQ_SETUP,
    SEQ_RET,
    SEQ_EXIT,
  };
  
  int i;
  DEBUG_BTL_POKE_CHANGE_WORK* wk = wk_adrs;
  GAMESYS_WORK* gsys = wk->gsys;
  GAMEDATA* gdata = GAMESYSTEM_GetGameData(gsys);

  switch( *seq )
  {
    case SEQ_INIT:
      wk->count = 0;

      for( i=0; i<MONSNO_END; i++ )
      {
        wk->poke_list[i] = i+1;
      }
      
      for( i=0; i<10000; i++ )
      {
        u32 n,m;
        u32 temp;
        n = GFUser_GetPublicRand0( MONSNO_END );
        m = GFUser_GetPublicRand0( MONSNO_END );
        temp = wk->poke_list[n];
        wk->poke_list[n] = wk->poke_list[m];
        wk->poke_list[m] = temp;
      }

      OS_TPrintf("�g���v�� �|�P���������e�X�g�J�n�I\n");
      *seq = SEQ_SETUP;
      break;

    case SEQ_SETUP:

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

      // �����^�G���㏑��
      {
        POKEPARTY* mp = bp->party[ BTL_CLIENT_PLAYER ];
        POKEPARTY* ep = bp->party[ BTL_CLIENT_ENEMY1 ];
      
        for( i=0; i<6; i++ )
        {
          POKEMON_PARAM* pp;

          if( PokeParty_GetPokeCount( mp ) <= i )
          {
            pp = GFL_HEAP_AllocMemoryLo( HEAPID_PROC, POKETOOL_GetWorkSize() );
            PP_Clear( pp );
            PP_Setup( pp, 1, 1, PTL_SETUP_ID_AUTO );
            PokeParty_Add( mp, pp );
            GFL_HEAP_FreeMemory( pp );
          }
          
          if( PokeParty_GetPokeCount( ep ) <= i )
          {
            pp = GFL_HEAP_AllocMemoryLo( HEAPID_PROC, POKETOOL_GetWorkSize() );
            PP_Clear( pp );
            PP_Setup( pp, 1, 1, PTL_SETUP_ID_AUTO );
            PokeParty_Add( ep, pp );
            GFL_HEAP_FreeMemory( pp );
          }

          pp = PokeParty_GetMemberPointer( mp, i );
          PP_Clear( pp );
          PP_Setup( pp, wk->poke_list[ wk->count ], 1, PTL_SETUP_ID_AUTO );
          pp = PokeParty_GetMemberPointer( ep, i );
          PP_ChangeMonsNo( pp, wk->poke_list[ wk->count+1 ] );
          
          OS_FPrintf( 1, "[%d]�C�� ���� monsno=%d �G monsno=%d \n",
              i, 
              wk->poke_list[wk->count], 
              wk->poke_list[wk->count+1] );

          wk->count += 2;

          if( wk->count > MONSNO_END )
          {
            break;
          }
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
      if( wk->count <= 300 )
//      if( wk->count <= MONSNO_END )
      {
        *seq = SEQ_SETUP;
      }
      else
      {
        *seq = SEQ_EXIT;
      }
      break;

    case SEQ_EXIT:
      OS_TPrintf("�g���v�� �|�P���������e�X�g�I���I\n");
      return GMEVENT_RES_FINISH;
    default : GF_ASSERT(0);
  }

  return GMEVENT_RES_CONTINUE;
}

#endif // PM_DEBUG
