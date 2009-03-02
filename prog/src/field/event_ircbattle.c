//============================================================================================
/**
 * @file	event_ircbattle.c
 * @brief	イベント：赤外線バトル
 * @author	tamada GAMEFREAK inc.
 * @date	2008.01.19
 */
//============================================================================================


#include <gflib.h>
#include "net/network_define.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/fieldmap.h"

#include "./event_fieldmap_control.h"
#include "./event_ircbattle.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "ircbattle/ircbattlematch.h"
#include "ircbattle/ircbattlemenu.h"
#include "battle/battle.h"
#include "poke_tool/monsno_def.h"
#include "system/main.h"			//GFL_HEAPID_APP参照

#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"
//============================================================================================
//============================================================================================

//----------------------------------------------------------------
// バトル用定義
extern const NetRecvFuncTable BtlRecvFuncTable[];
//----------------------------------------------------------------


#define	HEAPID_CORE GFL_HEAPID_APP

FS_EXTERN_OVERLAY(battle);
FS_EXTERN_OVERLAY(ircbattlematch);


enum _EVENT_IRCBATTLE {
    _CALL_IRCBATTLE_MENU,
    _WAIT_IRCBATTLE_MENU,
    _FIELD_FADEOUT,
    _CALL_IRCBATTLE_MATCH,
    _WAIT_IRCBATTLE_MATCH,
    _CALL_BATTLE,
    _WAIT_BATTLE,
    _FIELD_OPEN,
    _FIELD_FADEIN,
    _FIELD_END
};

struct _EVENT_IRCBATTLE_WORK{
	GAMESYS_WORK * gsys;
	FIELD_MAIN_WORK * fieldmap;
	BATTLE_SETUP_PARAM para;
    BOOL isEndProc;
};

//============================================================================================
//
//		サブイベント
//
//============================================================================================
static GMEVENT_RESULT EVENT_IrcBattleMain(GMEVENT * event, int *  seq, void * work)
{
	EVENT_IRCBATTLE_WORK * dbw = work;
	GAMESYS_WORK * gsys = dbw->gsys;
	switch (*seq) {
      case _CALL_IRCBATTLE_MENU:
        dbw->isEndProc = FALSE;
        GAMESYSTEM_CallProc(gsys, NO_OVERLAY_ID, &IrcBattleMenuProcData, dbw);
		(*seq)++;
		break;
      case _WAIT_IRCBATTLE_MENU:
      //  if (GAMESYSTEM_IsProcExists(gsys)){
        if(dbw->isEndProc){
            (*seq) ++;
        }
        break;
      case _FIELD_FADEOUT:
        dbw->isEndProc = FALSE;
		GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, dbw->fieldmap));
		(*seq)++;
		break;
      case _CALL_IRCBATTLE_MATCH:
		GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(ircbattlematch), &IrcBattleMatchProcData, dbw);
		(*seq)++;
		break;
      case _WAIT_IRCBATTLE_MATCH:
        if(dbw->isEndProc){
            (*seq) ++;
        }
		break;
      case _CALL_BATTLE:
        dbw->para.netHandle = GFL_NET_HANDLE_GetCurrentHandle();
        dbw->para.netID = GFL_NET_GetNetID( GFL_NET_HANDLE_GetCurrentHandle() );
        dbw->para.commPos = dbw->para.netID;

        GFL_NET_AddCommandTable(GFL_NET_CMD_BATTLE, BtlRecvFuncTable, 5, NULL);
		GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(battle), &BtlProcData, &dbw->para);
        GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);
		(*seq)++;
		break;
      case _WAIT_BATTLE:
        if (GAMESYSTEM_IsProcExists(gsys)){
            break;
        }
		(*seq) ++;
		break;
      case _FIELD_OPEN:
		GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
		(*seq) ++;
		break;
      case _FIELD_FADEIN:
		GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, dbw->fieldmap, 0));
		(*seq) ++;
		break;
      case _FIELD_END:
		return GMEVENT_RES_FINISH;
      default:
        GF_ASSERT(0);
        break;
	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void EVENT_IrcBattle(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap,GMEVENT * event)
{
//	GMEVENT * event;
	BATTLE_SETUP_PARAM * para;
	EVENT_IRCBATTLE_WORK * dbw;
    
//	event = GMEVENT_Create(gsys, NULL, DebugBattleEvent, sizeof(EVENT_IRCBATTLE_WORK));
	GMEVENT_Change( event,EVENT_IrcBattleMain, sizeof(EVENT_IRCBATTLE_WORK) );

   // work = GMEVENT_GetEventWork( event );

    dbw = GMEVENT_GetEventWork(event);
	dbw->gsys = gsys;
	dbw->fieldmap = fieldmap;
	para = &dbw->para;
	{
		para->engine = BTL_ENGINE_ALONE;
		para->rule = BTL_RULE_SINGLE;
		para->competitor = BTL_COMPETITOR_COMM;

        para->commMode = BTL_COMM_DS;
        para->multiMode = 0;

        para->partyPlayer = PokeParty_AllocPartyWork( HEAPID_CORE );	///< プレイヤーのパーティ
        para->partyEnemy1 = NULL;		///< 1vs1時の敵AI, 2vs2時の１番目敵AI用
        para->partyPartner = NULL;	///< 2vs2時の味方AI（不要ならnull）
        para->partyEnemy2 = NULL;		///< 2vs2時の２番目敵AI用（不要ならnull）

		PokeParty_Copy(GAMEDATA_GetMyPokemon(GAMESYSTEM_GetGameData(gsys)), para->partyPlayer);

        
	}

	
//	work = GMEVENT_GetEventWork( event );
//	FIELD_COMM_DEBUG_InitWork( heapID , gameSys , fieldWork , event , work );
    
//	return event;
}


void EVENT_IrcBattle_SetEnd(EVENT_IRCBATTLE_WORK* pWork)
{
    pWork->isEndProc = TRUE;
}



typedef struct{
    int gameNo;   ///< ゲーム種類
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { WB_NET_COMPATI_CHECK };


//--------------------------------------------------------------
/**
 * @brief   ビーコンデータ取得関数
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------

void* IrcBattleBeaconGetFunc(void* pWork)
{
	return &_testBeacon;
}

///< ビーコンデータサイズ取得関数
int IrcBattleBeaconGetSizeFunc(void* pWork)
{
	return sizeof(_testBeacon);
}

///< ビーコンデータ取得関数
BOOL IrcBattleBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
    if(myNo != beaconNo){
        return FALSE;
    }
    return TRUE;
}

