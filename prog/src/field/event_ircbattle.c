//============================================================================================
/**
 * @file	event_ircbattle.c
 * @brief	�C�x���g�F�ԊO���o�g��
 * @author	k.ohno
 * @date	2009.03.24
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
#include "ircbattle/ircbattlefriend.h"
#include "sound/pm_sndsys.h"
#include "battle/battle.h"
#include "poke_tool/monsno_def.h"
#include "system/main.h"			//GFL_HEAPID_APP�Q��

#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"
//============================================================================================
//============================================================================================

//----------------------------------------------------------------
// �o�g���p��`
extern const NetRecvFuncTable BtlRecvFuncTable[];
//----------------------------------------------------------------


#define	HEAPID_CORE GFL_HEAPID_APP

FS_EXTERN_OVERLAY(battle);
FS_EXTERN_OVERLAY(ircbattlematch);

#define _LOCALMATCHNO (100)

enum _EVENT_IRCBATTLE {
    _CALL_IRCBATTLE_MENU,
    _WAIT_IRCBATTLE_MENU,
    _FIELD_FADEOUT,
    _CALL_IRCBATTLE_MATCH,
    _WAIT_IRCBATTLE_MATCH,
    _TIMING_SYNC_CALL_BATTLE,
    _CALL_BATTLE,
    _WAIT_BATTLE,
    _CALL_IRCBATTLE_FRIEND,
    _WAIT_IRCBATTLE_FRIEND,
    _CALL_NET_END,
    _WAIT_NET_END,
    _FIELD_OPEN,
    _FIELD_FADEIN,
    _FIELD_END
};

struct _EVENT_IRCBATTLE_WORK{
	GAMESYS_WORK * gsys;
	FIELD_MAIN_WORK * fieldmap;
    SAVE_CONTROL_WORK *ctrl;
    BATTLE_SETUP_PARAM para;
    BOOL isEndProc;
    int selectType;
};

//============================================================================================
//
//		�T�u�C�x���g
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
        if (!GAMESYSTEM_IsProcExists(gsys)){
            if(dbw->selectType == EVENTIRCBTL_ENTRYMODE_EXIT){
                return GMEVENT_RES_FINISH;
            }
            else if(dbw->selectType == EVENTIRCBTL_ENTRYMODE_FRIEND){
                *seq = _CALL_IRCBATTLE_FRIEND;
                return GMEVENT_RES_CONTINUE;
            }
            (*seq) ++;
            GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );
            GFL_NET_AddCommandTable(GFL_NET_CMD_BATTLE, BtlRecvFuncTable, 5, NULL);
            GFL_NET_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_LOCALMATCHNO);
        }
		break;
      case _TIMING_SYNC_CALL_BATTLE:
        if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),_LOCALMATCHNO)){
            (*seq) ++;
        }
        break;
      case _CALL_BATTLE:
        switch(dbw->selectType){
          case EVENTIRCBTL_ENTRYMODE_SINGLE:
            dbw->para.rule = BTL_RULE_SINGLE;
            break;
          case EVENTIRCBTL_ENTRYMODE_DOUBLE:
            dbw->para.rule = BTL_RULE_DOUBLE;
            break;
          case EVENTIRCBTL_ENTRYMODE_TRI:
            dbw->para.rule = BTL_RULE_TRIPLE;
            break;
          case EVENTIRCBTL_ENTRYMODE_MULTH:
            dbw->para.multiMode = 1;
            dbw->para.rule = BTL_RULE_DOUBLE;
            NET_PRINT("multiMode\n");
            break;
          default:
            GF_ASSERT(0);
            break;
        }
        dbw->para.netHandle = GFL_NET_HANDLE_GetCurrentHandle();
        dbw->para.netID = GFL_NET_GetNetID( GFL_NET_HANDLE_GetCurrentHandle() );
        dbw->para.commPos = dbw->para.netID;
        PMSND_PlayBGM(dbw->para.musicDefault);

        GAMESYSTEM_CallProc(gsys, NO_OVERLAY_ID, &BtlProcData, &dbw->para);
        GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);
		(*seq)++;
		break;
      case _WAIT_BATTLE:
        if (GAMESYSTEM_IsProcExists(gsys)){
            break;
        }
        NET_PRINT("�o�g������ event_ircbattle\n");
        GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
		(*seq) = _CALL_NET_END;
		break;
      case _CALL_IRCBATTLE_FRIEND:  //  �Ƃ������R�[�h����
		GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(ircbattlematch), &IrcBattleFriendProcData, dbw);
		(*seq)++;
		break;
      case _WAIT_IRCBATTLE_FRIEND:
        if (!GAMESYSTEM_IsProcExists(gsys)){
            NET_PRINT("�Ƃ������R�[�h���������\n");
            (*seq) = _CALL_NET_END;
        }
		break;
      case _CALL_NET_END:
        if(GFL_NET_IsParentMachine()){
            GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),GFL_NET_CMD_EXIT_REQ,0,NULL);
        }
		(*seq) ++;
        break;
      case _WAIT_NET_END:
        if(GFL_NET_IsExit()){
            (*seq) ++;
        }
        break;
      case _FIELD_OPEN:
        OS_TPrintf("_FIELD_OPEN\n");
		GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
		(*seq) ++;
		break;
      case _FIELD_FADEIN:
        OS_TPrintf("_FIELD_FADEIN\n");
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
	GMEVENT * child_event;
	BATTLE_SETUP_PARAM * para;
	EVENT_IRCBATTLE_WORK * dbw;

    GMEVENT_Change( event,EVENT_IrcBattleMain, sizeof(EVENT_IRCBATTLE_WORK) );
    dbw = GMEVENT_GetEventWork(event);
    dbw->ctrl = SaveControl_GetPointer();
	dbw->gsys = gsys;
	dbw->fieldmap = fieldmap;
	para = &dbw->para;
	{
		para->engine = BTL_ENGINE_ALONE;
		para->rule = BTL_RULE_SINGLE;
		para->competitor = BTL_COMPETITOR_COMM;

        para->commMode = BTL_COMM_DS;
        para->multiMode = 0;

        para->partyPlayer = PokeParty_AllocPartyWork( HEAPID_CORE );	///< �v���C���[�̃p�[�e�B
        para->partyEnemy1 = NULL;		///< 1vs1���̓GAI, 2vs2���̂P�ԖړGAI�p
        para->partyPartner = NULL;	///< 2vs2���̖���AI�i�s�v�Ȃ�null�j
        para->partyEnemy2 = NULL;		///< 2vs2���̂Q�ԖړGAI�p�i�s�v�Ȃ�null�j


        para->musicDefault = SEQ_WB_BA_TEST_250KB;		///< �f�t�H���g����BGM�i���o�[
        para->musicPinch = SEQ_WB_BA_PINCH_TEST_150KB;			///< �s���`����BGM�i���o�[
        
		PokeParty_Copy(GAMEDATA_GetMyPokemon(GAMESYSTEM_GetGameData(gsys)), para->partyPlayer);
	}
}


void EVENT_IrcBattle_SetEnd(EVENT_IRCBATTLE_WORK* pWork)
{
    pWork->isEndProc = TRUE;
}



typedef struct{
    int gameNo;   ///< �Q�[�����
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { WB_NET_COMPATI_CHECK };


//--------------------------------------------------------------
/**
 * @brief   �r�[�R���f�[�^�擾�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------

void* IrcBattleBeaconGetFunc(void* pWork)
{
	return &_testBeacon;
}

///< �r�[�R���f�[�^�T�C�Y�擾�֐�
int IrcBattleBeaconGetSizeFunc(void* pWork)
{
	return sizeof(_testBeacon);
}

///< �r�[�R���f�[�^�擾�֐�
BOOL IrcBattleBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
    if(myNo != beaconNo){
        return FALSE;
    }
    return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief   �ԊO�����[�h�̐ݒ�
 * @param   pWork      ���[�N�G���A
 * @param   type       �^�C�v
 * @retval  none  
 */
//--------------------------------------------------------------
void EVENT_IrcBattleSetType(EVENT_IRCBATTLE_WORK* pWork, int type)
{
    pWork->selectType = type;
}

int EVENT_IrcBattleGetType(EVENT_IRCBATTLE_WORK* pWork)
{
    return pWork->selectType;
}

//--------------------------------------------------------------
/**
 * @brief   EVENT_IRCBATTLE_WORK����GAMESYS_WORK�𓾂�
 * @param   pWork      EVENT_IRCBATTLE_WORK
 * @retval  GAMESYS_WORK
 */
//--------------------------------------------------------------
GAMESYS_WORK* IrcBattle_GetGAMESYS_WORK(EVENT_IRCBATTLE_WORK* pWork)
{
    return pWork->gsys;
}

//--------------------------------------------------------------
/**
 * @brief   EVENT_IRCBATTLE_WORK����SAVE_CONTROL_WORK������
 * @param   pWork      EVENT_IRCBATTLE_WORK
 * @retval  SAVE_CONTROL_WORK
 */
//--------------------------------------------------------------
SAVE_CONTROL_WORK* IrcBattle_GetSAVE_CONTROL_WORK(EVENT_IRCBATTLE_WORK* pWork)
{
    return pWork->ctrl;
}



