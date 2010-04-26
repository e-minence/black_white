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
#include "event_battle.h" //for EVENT_TrialHouseTrainerBattle

#include "script_def.h"

#include "savedata/battle_box_save.h"   //for BATTLE_BOX_SAVE
#include "app/th_award.h"   //for TH_AWARD_PARAM

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
  u16*            RetDecide;  // �I���������̃`�F�b�N
  PL_LIST_TYPE ListType;
  int Reg;
  POKEPARTY* Party;         //�w��|�P�p�[�e�B�|�C���^�i�A���b�N�����A�|�C���^�Ƃ��Ďg�p�j
  POKEPARTY* BoxParty;      //�o�g���{�b�N�X�p�|�P�p�[�e�B�i�A���b�N����j
  PL_SELECT_POS SelectPos;
  PL_RETURN_TYPE RetType;
  u8 SelAry[6];
  u8 dummy[2];
  TRIAL_HOUSE_WORK_PTR HouseWorkPtr;
}TH_POKESEL_WORK;

//------------------------------------------------------------------
/**
 * @brief �]����ʌĂяo�����[�N
 */
//------------------------------------------------------------------
typedef struct TH_RANK_WORK_tag
{
  GAMESYS_WORK*      gsys;  // �Q�[���V�X�e��
	FIELDMAP_WORK* fieldmap;  // �t�B�[���h�}�b�v
  TH_AWARD_PARAM Param;
}TH_RANK_WORK;

static GMEVENT_RESULT PokeSelEvt(GMEVENT * event, int * seq, void * work);
static GMEVENT_RESULT CallRankAppEvt( GMEVENT* event, int* seq, void* work );

//--------------------------------------------------------------
/**
 * �|�P�����I���C�x���g�쐬
 * @param gsys          �Q�[���V�X�e���|�C���^
 * @param inRegType    �o�g�����M�����[�V�����^�C�v�@�V���O���n�q�_�u��
 * @param inPartyType   �p�[�e�B�^�C�v�@�莝���n�q�o�g���{�b�N�X
 * @param outRet        ���X�g�̌���    ���~�̂Ƃ�FALSE
 *
 * @retval GMEVENT      �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *TRIAL_HOUSE_CreatePokeSelEvt(  GAMESYS_WORK * gsys, TRIAL_HOUSE_WORK_PTR ptr,
                                        const int inRegType, const int inPartyType, u16 *outRet )
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
  work->HouseWorkPtr = ptr;

  if ( (inRegType == REG_SUBWAY_SINGLE)||(inRegType == REG_SUBWAY_DOUBLE) )
  {
    if (inRegType == REG_SUBWAY_SINGLE)
    {
      work->ListType = PL_TYPE_SINGLE;
      work->Reg = REG_SUBWAY_SINGLE;
    }
    else  //REG_SUBWAY_DOUBLE
    {
      work->ListType = PL_TYPE_DOUBLE;
      work->Reg = REG_SUBWAY_DOUBLE;
    }
  }
  else
  {
    GF_ASSERT_MSG(0,"reg error %d",inRegType);
    work->ListType = PL_TYPE_SINGLE;
    work->Reg = REG_SUBWAY_SINGLE;
  }

  if ( inPartyType == SCR_BTL_PARTY_SELECT_TEMOTI)
  {
    work->BoxParty = NULL;
    work->Party = GAMEDATA_GetMyPokemon( gdata );  //�莝��
  }
  else
  {
    SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork( gdata );
    BATTLE_BOX_SAVE* box = BATTLE_BOX_SAVE_GetBattleBoxSave( sv );
    work->BoxParty = BATTLE_BOX_SAVE_MakePokeParty( box, HEAPID_APP_CONTROL );
    work->Party = work->BoxParty; //�o�g���{�b�N�X
  }
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
          evt_wk->ListType, PL_MODE_BATTLE_EXAMINATION, evt_wk->Reg, evt_wk->Party, evt_wk->SelAry,
          &evt_wk->SelectPos, &evt_wk->RetType, evt_wk->HouseWorkPtr->Party );
      GMEVENT_CallEvent(event, call_event);
    }
    *seq = 1;
		break;
  case 1:
    //�o�g���{�b�N�X�p�[�e�B�����
    if (evt_wk->BoxParty != NULL)
      GFL_HEAP_FreeMemory(evt_wk->BoxParty);
    
    //���ʂ̕���
    if( evt_wk->RetType != PL_RET_NORMAL ||
      evt_wk->SelectPos == PL_SEL_POS_EXIT ||
      evt_wk->SelectPos == PL_SEL_POS_EXIT2 )
    {
      *(evt_wk->RetDecide) = FALSE;          //�L�����Z��
    }
    else *(evt_wk->RetDecide) = TRUE;           //�|�P������I�����āA�����Ă�����
    
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

    bp = FBI_TOOL_CreateBattleParam( gsys, party, mode, tr_data, NULL, num  );
  }

  //�g���C�A���n�E�X�퓬�C�x���g���쐬
  event = EVENT_TrialHouseTrainerBattle( gsys, fieldmap, bp );

  return event;
}

//--------------------------------------------------------------
/**
 * @brief	�̓_�ɕK�v�ȏ����W�߂�  �t�B�[���h���A�O�ɃR�[�����ꂦ��̂ł��̃t�@�C���ɒu��
 * @param	ptr       ���[�N�|�C���^
 * @param param     �o�g���Z�b�g�A�b�v�p�����[�^�|�C���^
 * @retval	none
*/
//--------------------------------------------------------------
void TRIAL_HOUSE_AddBtlPoint( TRIAL_HOUSE_WORK_PTR ptr, BATTLE_SETUP_PARAM *prm )
{
  ptr->PointWork.TurnNum += prm->TurnNum;              //���������^�[����
  ptr->PointWork.PokeChgNum += prm->PokeChgNum;        //����
  ptr->PointWork.VoidAtcNum += prm->VoidAtcNum;        //���ʂ��Ȃ��Z���o������
  ptr->PointWork.WeakAtcNum += prm->WeakAtcNum;        //�΂���̋Z���o������
  ptr->PointWork.ResistAtcNum += prm->ResistAtcNum;    //���܂ЂƂ̋Z���o������
  ptr->PointWork.VoidNum += prm->VoidNum;              //���ʂ��Ȃ��Z���󂯂���
  ptr->PointWork.ResistNum += prm->ResistNum;          //���܂ЂƂ̋Z���󂯂���
  ptr->PointWork.WinTrainerNum += prm->WinTrainerNum;  //�|�����g���[�i�[��
  ptr->PointWork.WinPokeNum += prm->WinPokeNum;        //�|�����|�P������
  ptr->PointWork.LosePokeNum += prm->LosePokeNum;      //�|���ꂽ�|�P������
  ptr->PointWork.UseWazaNum += prm->UseWazaNum;        //�g�p�����Z�̐�
  //�g�o�����v�Z
  {
    int i;
    int poke_num;
    int hp, hp_max;
    int per;
    POKEPARTY* party;
    party = prm->party[BTL_CLIENT_PLAYER];
    //�|�P���������擾
    poke_num = PokeParty_GetPokeCount(party);
    //�g�o�i�[�o�b�t�@������
    hp = 0;
    hp_max = 0;
    for (i=0;i<poke_num;i++)
    {
      int pokehp, pokehp_max;
      POKEMON_PARAM *pp;
      pp = PokeParty_GetMemberPointer(party, i); 
      //�c��g�o�擾
      pokehp = PP_Get( pp, ID_PARA_hp, NULL);
      //�ő�g�o�擾
      pokehp_max = PP_Get( pp, ID_PARA_hpmax, NULL);
      //��������
      hp += pokehp;
      hp_max += pokehp_max;
    }
    per = (hp * 100) / hp_max; //�c��g�o�p�[�Z���g
    ptr->PointWork.RestHpPer += per;      //�T�킠��̂ő�������ł����ƍő��500���ɂȂ�
    NOZOMU_Printf( "per = %d, total_per = %d\n",per, ptr->PointWork.RestHpPer );
  }
}

//--------------------------------------------------------------
/**
 * �����L���O�m�F�A�v���C�x���g
 * @param gsys          �Q�[���V�X�e���|�C���^
 * @param inIsDL    �m�F����̂̓_�E�����[�h�f�[�^���H
 * @retval GMEVENT      �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *TRIAL_HOUSE_CreateRankAppEvt( GAMESYS_WORK * gsys, const BOOL inIsDL )
{
  GMEVENT* event;
	TH_RANK_WORK* work;
  GAMEDATA*  gdata = GAMESYSTEM_GetGameData( gsys );

  // �C�x���g����
  event = GMEVENT_Create(gsys, NULL, CallRankAppEvt, sizeof(TH_RANK_WORK));
  work = GMEVENT_GetEventWork(event);
	work->gsys = gsys;
	work->fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  //�A�v���󂯓n�����[�N�Z�b�g
  {
    //�����̐���
    {
      MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gdata );
      work->Param.sex = MyStatus_GetMySex(mystatus);
    }
    //�Z�[�u�f�[�^�|�C���^
    {
      SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gdata);
      work->Param.thsv = THSV_GetSvPtr(sv);
    }
    //�_�E�����[�h�t���O
    work->Param.b_download = inIsDL;
  }
  return event;
}

//--------------------------------------------------------------
/**
 * @brief   ���I���A�v���R�[���C�x���g
 * @param	  event   �C�x���g�|�C���^
 * @param   seq     �V�[�P���T
 * @param   work    ���[�N�|�C���^
 * @return  GMEVENT_RESULT  �C�x���g����
*/
//--------------------------------------------------------------
static GMEVENT_RESULT CallRankAppEvt( GMEVENT* event, int* seq, void* work )
{
  TH_RANK_WORK *evt_work;
  evt_work = GMEVENT_GetEventWork(event);

  switch(*seq){
  case 0:
    //�f���v���b�N
    GMEVENT_CallProc( event, FS_OVERLAY_ID(th_award), &TH_AWARD_ProcData, &evt_work->Param );
    (*seq)++;
    break;
  case 1:
    //�C�x���g�I��
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

