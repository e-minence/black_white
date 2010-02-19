//======================================================================
/**
 * @file  trial_house_st.c
 * @brief �g���C�A���n�E�X�@�풓��
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "trial_house.h"
#include "trial_house_def.h"
#include "system/main.h"  //for HEAPID_APP_CONTROL

#include "fld_btl_inst_tool.h"
#include "poke_tool/regulation_def.h"
#include "event_battle.h" //for EVENT_BSubwayTrainerBattle

//------------------------------------------------------------------
/**
 * @brief �|�P�����I�����[�N
 */
//------------------------------------------------------------------
typedef struct 
{
	GAMESYS_WORK*      gsys;  // �Q�[���V�X�e��
	FIELDMAP_WORK* fieldmap;  // �t�B�[���h�}�b�v
  PLIST_DATA*      PokeListlData;  // �|�P�������X�g
  BOOL*            RetDecide;  // �I���������̃`�F�b�N
  PL_LIST_TYPE ListType;
  PL_MODE_TYPE ModeType;
  int Reg;
  POKEPARTY* Party;
  PL_SELECT_POS Select;
  PL_RETURN_TYPE RetType;
  u8 SelAry[6];
  u8 dummy[2];
  TRIAL_HOUSE_WORK_PTR HouseWorkPtr;
}TH_POKESEL_WORK;

static GMEVENT_RESULT PokeSelEvt(GMEVENT * event, int * seq, void * work);

//--------------------------------------------------------------
/**
 * �|�P�����I���C�x���g�쐬
 * @param gsys          �Q�[���V�X�e���|�C���^
 * @param inListType    �o�g���^�C�v�@�V���O���n�q�_�u��
 * @param inPartyType   �p�[�e�B�^�C�v�@�莝���n�q�o�g���{�b�N�X
 * @param outRet        ���X�g�̌���    ���~�̂Ƃ�FALSE
 *
 * @retval GMEVENT      �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *TRIAL_HOUSE_CreatePokeSelEvt(  GAMESYS_WORK * gsys, TRIAL_HOUSE_WORK_PTR ptr,
                                        const int inBtlType, const int inPartyType, BOOL *outRet )
{
  GMEVENT* event;
	TH_POKESEL_WORK* work;
  GAMEDATA*  gdata = GAMESYSTEM_GetGameData( gsys );

  // �C�x���g����
  event = GMEVENT_Create(gsys, NULL, PokeSelEvt, sizeof(TH_POKESEL_WORK));
  work = GMEVENT_GetEventWork(event);
	work->gsys = gsys;
	work->fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  work->RetDecide = outRet;
  work->ModeType = PL_MODE_BATTLE;

  work->HouseWorkPtr = ptr;
  if (inBtlType)
  {
    work->ListType = PL_TYPE_SINGLE;
    work->Reg = REG_SUBWAY_SINGLE;
  }
  else
  {
    work->ListType = PL_TYPE_DOUBLE;
    work->Reg = REG_SUBWAY_DOUBLE;
  }

  if ( inPartyType ) work->Party = GAMEDATA_GetMyPokemon( gdata );  //�莝��
  else work->Party = GAMEDATA_GetMyPokemon( gdata );                //�o�g���{�b�N�X @todo

  return event;
}

static GMEVENT_RESULT PokeSelEvt(GMEVENT * event, int * seq, void * work)
{
	TH_POKESEL_WORK *evt_wk = work;
	GAMESYS_WORK *gsys = evt_wk->gsys;
  FIELDMAP_WORK *fieldmap = evt_wk->fieldmap;

	switch( *seq ) 
  {
  case 0:
    //�|�P�������X�g�C�x���g�R�[��
    {
      GMEVENT* call_event;
      call_event = FBI_TOOL_CreatePokeListEvt(
          gsys,
          evt_wk->ListType, evt_wk->Reg, evt_wk->Party, evt_wk->SelAry,
          &evt_wk->Select, &evt_wk->RetType );
      GMEVENT_CallEvent(event, call_event);
    }
    *seq = 1;
		break;
  case 1:
    //���ʂ̊i�[
    if( evt_wk->RetType != PL_RET_NORMAL ||
      evt_wk->Select == PL_SEL_POS_EXIT ||
      evt_wk->Select == PL_SEL_POS_EXIT2 )
    {
      *(evt_wk->RetDecide) = FALSE;          //�L�����Z��
    }
    else
    {
      *(evt_wk->RetDecide) = TRUE;           //�|�P������I�����āA�����Ă�����
      //�V���O���R�́A�_�u���S�̂��g���C�A���n�E�X�|�P�p�[�e�B�ɃZ�b�g
      ;
    }
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}


//--------------------------------------------------------------
/**
 * �퓬�C�x���g�쐬
 * @param gsys          �Q�[���V�X�e���|�C���^
 * @param inListType    �o�g���^�C�v�@�V���O���n�q�_�u��
 * @retval GMEVENT      �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *TRIAL_HOUSE_CreateBtlEvt( GAMESYS_WORK * gsys, TRIAL_HOUSE_WORK_PTR ptr )
{
  GMEVENT* event;
	BATTLE_SETUP_PARAM *bp;
  FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  {
    POKEPARTY *party = ptr->Party;
    BSWAY_PLAYMODE mode = ptr->PlayMode;
    BSUBWAY_PARTNER_DATA *tr_data = &ptr->TrData;
    int num = ptr->MemberNum;

    bp = FBI_TOOL_CreateBattleParam( gsys, party, mode, tr_data, num  );
  }

  //�o�g���T�u�E�F�C�̐퓬�C�x���g���쐬
  event = EVENT_BSubwayTrainerBattle( gsys, fieldmap, bp );

  return event;
}

