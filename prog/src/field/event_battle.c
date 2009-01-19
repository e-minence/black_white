//============================================================================================
/**
 * @file	event_battle.c
 * @brief	�C�x���g�F�t�B�[���h�}�b�v����c�[��
 * @author	tamada GAMEFREAK inc.
 * @date	2008.01.19
 */
//============================================================================================


#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/fieldmap.h"

#include "./event_fieldmap_control.h"
#include "./event_battle.h"

//============================================================================================
//============================================================================================

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "battle/battle.h"
#include "poke_tool/monsno_def.h"
#include "system/main.h"

#define	HEAPID_CORE GFL_HEAPID_APP

FS_EXTERN_OVERLAY(battle);


extern const GFL_PROC_DATA DebugSogabeMainProcData;
//============================================================================================
//
//		�T�u�C�x���g
//
//============================================================================================

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_Battle(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap)
{
	GMEVENT * event;
	event = EVENT_FieldSubProc(gsys, fieldmap,NO_OVERLAY_ID, &DebugSogabeMainProcData, NULL);
	return event;

#if 0
	BATTLE_SETUP_PARAM* para = getGenericWork( wk, sizeof(BATTLE_SETUP_PARAM) );
	{
			para->engine = BTL_ENGINE_ALONE;
			para->rule = BTL_RULE_SINGLE;
			para->competitor = BTL_COMPETITOR_WILD;

			para->netHandle = NULL;
			para->commMode = BTL_COMM_NONE;
			para->commPos = 0;
			para->netID = 0;


			para->partyPlayer = PokeParty_AllocPartyWork( HEAPID_CORE );	///< �v���C���[�̃p�[�e�B
			para->partyEnemy1 = PokeParty_AllocPartyWork( HEAPID_CORE );	///< 1vs1���̓GAI, 2vs2���̂P�ԖړGAI�p
			para->partyPartner = NULL;	///< 2vs2���̖���AI�i�s�v�Ȃ�null�j
			para->partyEnemy2 = NULL;	///< 2vs2���̂Q�ԖړGAI�p�i�s�v�Ȃ�null�j

		#ifdef DEBUG_ONLY_FOR_taya
			setup_party( HEAPID_CORE, para->partyPlayer, MONSNO_GYARADOSU, MONSNO_PIKATYUU, MONSNO_RIZAADON, 0 );
			setup_party( HEAPID_CORE, para->partyEnemy1, MONSNO_YADOKINGU, MONSNO_METAGUROSU, MONSNO_SUTAAMII, 0 );
		#else
			setup_party( HEAPID_CORE, para->partyPlayer, MONSNO_AUSU + 2, MONSNO_AUSU + 1, 0 );
			setup_party( HEAPID_CORE, para->partyEnemy1, MONSNO_AUSU + 1, MONSNO_AUSU + 2, 0 );
		#endif

			GFL_PROC_SysCallProc( FS_OVERLAY_ID(battle), &BtlProcData, para );
			(*seq)++;
		}
#endif
}


