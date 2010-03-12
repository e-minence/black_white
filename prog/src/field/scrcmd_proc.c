//======================================================================
/**
 * @file    scrcmd_proc.c
 * @brief   �X�N���v�g�R�}���h�p�֐��@�e��A�v���Ăяo���n(�풓)
 * @author  Miyuki Iwasawa 
 * @date    09.10.22
 *
 * �t�F�[�h���t�B�[���h����������܂ł��R�����̃C�x���g�V�[�P���X
 * (EVENT_FieldSubProc,EVENT_FieldSubProc_Callback�Ȃ�)�ɔC����ꍇ�A�v���Z�X�Ăяo���R�}���h���̂�
 * �풓�ɔz�u����K�v���Ȃ��̂ŁAscrcmd_proc_fld.c(�t�B�[���h�풓)�ɒu���Ă�������
 *
 * �t�F�[�h��t�B�[���h�v���Z�X�R���g���[�������O�ōs���v���Z�X�Ăяo���@�����K�v������ꍇ
 * ���̃\�[�X�ɂ����Ă��������B���ɕK�v���Ȃ���΁A�풓�������ߖ�̂��߁AEVENT_FieldSubProc��
 * �p�ӂ��ꂽ�v���Z�X�Ăяo���C�x���g�𗘗p���Ă�������
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"

#include "scrcmd.h"
#include "scrcmd_work.h"

#include "msgdata.h"
#include "print/wordset.h"

#include "sound/pm_sndsys.h"
//#include "sound/wb_sound_data.sadl"

#include "fieldmap.h"
#include "field_sound.h"
#include "field_player.h"
#include "event_fieldmap_control.h"

#include "scrcmd_proc.h"
#include "../../../resource/fldmapdata/script/usescript.h"

#include "app/bag.h"

#include "field_comm\intrude_main.h"
#include "field/field_comm/intrude_work.h"
#include "field/monolith_main.h"
#include "field\field_comm\intrude_mission.h"

#include "demo/demo3d.h"  //Demo3DProcData etc.
#include "app/local_tvt_sys.h"  //LocalTvt_ProcData etc.
#include "app/chihou_zukan_award.h"
#include "app/zenkoku_zukan_award.h"

#include "app/mailbox.h"
FS_EXTERN_OVERLAY(app_mail);

#include "app/waza_oshie.h"
FS_EXTERN_OVERLAY(waza_oshie);



////////////////////////////////////////////////////////////////
//�v���g�^�C�v
////////////////////////////////////////////////////////////////
static BOOL callproc_WaitSubProcEnd( VMHANDLE *core, void *wk );


////////////////////////////////////////////////////////////////
//�t�B�[���h�������j��
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�}�b�v�v���Z�X����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFieldOpen( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  SCRIPT_CallEvent( sc, EVENT_FieldOpen( gsys ));
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�}�b�v�v���Z�X�j��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFieldClose( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  SCRIPT_CallEvent( sc, EVENT_FieldClose(gsys, GAMESYSTEM_GetFieldMapWork(gsys)));
  
  return VMCMD_RESULT_SUSPEND;
}

////////////////////////////////////////////////////////////////
//�T�u�v���Z�X�Ăяo��
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------
/*
 *  @brief  �T�u�v���Z�X�R�[����Wait�֐��̃Z�b�g�A�b�v(����������o�[�W����)
 *
 *  @param  core
 *  @param  work
 *  @param  ov_id         �T�u�v���Z�X�̃I�[�o�[���CID
 *  @param  proc_data     �T�u�v���Z�X�̊֐��w��
 *  @param  proc_work     �T�u�v���Z�X�p���[�N�BNULL�w���
 *  @param  callback      �T�u�v���Z�X�I�����ɌĂяo���R�[���o�b�N�֐��|�C���^�BNULL�w���
 *  @param  callback_work �ėp���[�N�BNULL�w���
 *
 *  @note
 *  callback��NULL���w�肵���ꍇ�A�T�u�v���Z�X�I������
 *  proc_work��callback_work�ɑ΂��� GFL_HEAP_FreeMemory()���Ăяo���܂��B
 *  callback���w�肵���ꍇ�́Acallback���Ŗ����I�ɉ�������Ă�������
 */
//--------------------------------------------------------------
void EVFUNC_CallSubProc( VMHANDLE *core, SCRCMD_WORK *work,
    FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void* proc_work,
    void (*callback)(CALL_PROC_WORK* cpw), void* callback_work )
{
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  void** scr_subproc_work = SCRIPT_SetSubProcWorkPointerAdrs( sc );

  CALL_PROC_WORK* cpw = GFL_HEAP_AllocClearMemory(HEAPID_PROC,sizeof(CALL_PROC_WORK));

  cpw->proc_work = proc_work;
  cpw->cb_work = callback_work;
  cpw->cb_func = callback;
  GAMESYSTEM_CallProc( gsys, ov_id, proc_data, proc_work ); 
 
  *scr_subproc_work = cpw;
  VMCMD_SetWait( core, callproc_WaitSubProcEnd );
}

//--------------------------------------------------------------
/**
 * @brief �T�u�v���Z�X�̏I����҂��܂�(���[�N��������L��)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  TRUE    �I��
 * @retval  FALSE   �҂�
 *
 * EVFUNC_CallSubProc()�ƃZ�b�g�̃T�u�v���Z�XWait�֐��ł�
 */
//--------------------------------------------------------------
static BOOL callproc_WaitSubProcEnd( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  CALL_PROC_WORK* cpw = (CALL_PROC_WORK*)SCRIPT_SetSubProcWorkPointer( sc );

  if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
    return FALSE;
  }
  if(cpw->cb_func != NULL){
    (cpw->cb_func)(cpw);
  }else{
    if(cpw->proc_work != NULL){
      GFL_HEAP_FreeMemory(cpw->proc_work);
    }
    if(cpw->cb_work != NULL){
      GFL_HEAP_FreeMemory(cpw->cb_work);
    }
  }
  SCRIPT_FreeSubProcWorkPointer(sc);

  return TRUE;
}
//--------------------------------------------------------------
/**
 * @brief �T�u�v���Z�X�̏I����҂��܂�(���[�N�����������)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  TRUE    �I��
 * @retval  FALSE   �҂�
 *
 * ���g�p�͋ʓc����̋����ł��B
 * �@�v���Z�X�R�[���͌����Ƃ��āA�Ăяo���`�߂�l�擾�܂ł�One�R�}���h�Ŏ�������݌v�ɂ��邱�ƁI
 *   WaitSubProcEnd�ɃR�[���o�b�N���w��ł��܂�
 * ��������ő҂����ꍇ�A�����I�� EvCmdFreeSubProcWork( VMHANDLE *core, void *wk ) ���g���ĉ�����Ă�������
 */
//--------------------------------------------------------------
BOOL EVFUNC_WaitSubProcEndNonFree( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

  if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief �ėp�T�u�v���Z�X���[�N����R�}���h
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 *
 * ���g�p�͋ʓc����̋����ł��B
 * �@�v���Z�X�R�[���͌����Ƃ��āA�Ăяo���`�߂�l�擾�܂ł�One�R�}���h�Ŏ�������݌v�ɂ��邱�ƁI
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFreeSubProcWork( VMHANDLE *core, void *wk )
{
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( (SCRCMD_WORK*)wk );
  SCRIPT_FreeSubProcWorkPointer(sc);
  return VMCMD_RESULT_SUSPEND;
}


////////////////////////////////////////////////////////////////
//�o�b�O�v���Z�X�Ăяo��
////////////////////////////////////////////////////////////////
typedef struct _BAG_CALLBACK_WORK{
  u16* ret_mode;
  u16* ret_item;
}BAG_CALLBACK_WORK;

static void callback_BagProcFunc( CALL_PROC_WORK* cpw )
{
  BAG_PARAM* bp = (BAG_PARAM*)cpw->proc_work;
  BAG_CALLBACK_WORK* bcw = (BAG_CALLBACK_WORK*)cpw->cb_work;

  if(bp->next_proc == BAG_NEXTPROC_EXIT ){
    *bcw->ret_mode = SCR_PROC_RETMODE_EXIT;
  }else{
    *bcw->ret_mode = SCR_PROC_RETMODE_NORMAL;
  }
  *bcw->ret_item = bp->ret_item;

  GFL_HEAP_FreeMemory( cpw->cb_work );
  GFL_HEAP_FreeMemory( cpw->proc_work );
}

//--------------------------------------------------------------
/**
 * @brief   �o�b�O�v���Z�X���Ăяo���܂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 *
 * ���T�u�v���Z�X���[�N�̉���������s��Ȃ��̂ŁA�K�������I�� EvCmdBagProcWorkFree()���Ăяo������
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCallBagProc( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  BAG_PARAM* bp;
  BAG_CALLBACK_WORK* bcw;

  u16 mode = SCRCMD_GetVMWorkValue( core, work );
  u16* ret_mode = SCRCMD_GetVMWork( core, work );
  u16* ret_item = SCRCMD_GetVMWork( core, work );

  if( mode == SCR_BAG_MODE_SELL ){
    mode = BAG_MODE_SELL;
  }else{
    mode = BAG_MODE_FIELD;
  }
  bp = BAG_CreateParam( SCRCMD_WORK_GetGameData( work ), NULL, mode,  HEAPID_PROC );
  bcw = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(BAG_CALLBACK_WORK) );
  bcw->ret_mode = ret_mode;
  bcw->ret_item = ret_item;

  EVFUNC_CallSubProc( core, work,
    FS_OVERLAY_ID(bag), &ItemMenuProcData, bp, callback_BagProcFunc, bcw);
  
  return VMCMD_RESULT_SUSPEND;
}


//--------------------------------------------------------------
/**
 * @brief ���[���{�b�N�X��ʃv���Z�X�I����̃R�[���o�b�N�֐�
 */
//--------------------------------------------------------------
typedef struct
{
  MAILBOX_PARAM* mailbox_param;

} MAILBOX_CALLBACK_WORK;

static void EvCmdCallMailBoxProc_CallBack( void* work )
{
  MAILBOX_CALLBACK_WORK* cbw = work;
  GFL_HEAP_FreeMemory( cbw->mailbox_param );
}

//--------------------------------------------------------------
/**
 * @brief   ���[���{�b�N�X��ʃv���Z�X���Ăяo���܂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCallMailBoxProc( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = wk;
  SCRIPT_WORK*         sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  MAILBOX_PARAM* param;
  MAILBOX_CALLBACK_WORK* cbw;
  GMEVENT* event;
  
  param = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(MAILBOX_PARAM) ); 
  cbw   = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(MAILBOX_CALLBACK_WORK) );
  param->gamedata    = gdata;
  cbw->mailbox_param = param;

  event = EVENT_FieldSubProcNoFade_Callback( gsys, fieldmap, 
                                       FS_OVERLAY_ID(app_mail), &MailBoxProcData, param,
                                       EvCmdCallMailBoxProc_CallBack, cbw );
  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;
}


//--------------------------------------------------------------
/**
 * @brief �Z�v���o����ʃv���Z�X�I����̃R�[���o�b�N�֐�
 */
//--------------------------------------------------------------
typedef struct
{
  WAZAOSHIE_DATA *waza_remind_dat;
  u16* retWk;  // ���ʂ̊i�[��

} WAZA_REMIND_CALLBACK_WORK;

static void EvCmdCallWazaRemindProc_CallBack( void* work )
{
  WAZA_REMIND_CALLBACK_WORK* wrwk = work;

  // ���ʂ�Ԃ�
  switch( wrwk->waza_remind_dat->ret )
  {
  default:
  case WAZAOSHIE_RET_SET:    *(wrwk->retWk) = SCR_WAZAOSHIE_RET_SET;    break;
  case WAZAOSHIE_RET_CANCEL: *(wrwk->retWk) = SCR_WAZAOSHIE_RET_CANCEL; break;
  }

  GFL_HEAP_FreeMemory( wrwk->waza_remind_dat->waza_tbl );
  WAZAOSHIE_DataFree( wrwk->waza_remind_dat );
  // wrwk��PROC�Ăяo���C�x���g���[�`�����ŉ�������
}

//--------------------------------------------------------------
/**
 * @brief   �Z�v���o����ʃv���Z�X���Ăяo���܂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCallWazaRemindProc( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = wk;
  SCRIPT_WORK*         sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h������(���ʂ̊i�[��)
  u16            poke_pos = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h������(�p�[�e�B�[���ʒu)
  POKEPARTY*        party = GAMEDATA_GetMyPokemon(gdata);
  POKEMON_PARAM*       pp = PokeParty_GetMemberPointer( party, poke_pos);
  MYSTATUS*      mystatus = GAMEDATA_GetMyStatus( gdata );
  WAZAOSHIE_DATA* param;
  WAZA_REMIND_CALLBACK_WORK* wrwk;
  GMEVENT* event;
  u16 *waza_tbl = WAZAOSHIE_GetRemaindWaza( pp, HEAPID_PROC );
  
  param =WAZAOSHIE_DataAlloc( HEAPID_PROC );
  param->pp            = pp;
  param->myst          = mystatus;
  param->cfg           = NULL;
  param->gsys          = gsys;
  param->waza_tbl      = waza_tbl;
  param->pos           = poke_pos;    // �X�N���v�g���牽�C�ڂ̃|�P�������w�肷�邩�Ⴄ
  param->scr           = 0;    // �X�N���[���|�C���g
  param->page          = 0; // �퓬�Z�\��
  param->mode          = WAZAOSHIE_MODE_REMIND;  // �Z�v���o�����[�h

  wrwk  = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(MAILBOX_CALLBACK_WORK) );
  wrwk->waza_remind_dat = param;
  wrwk->retWk = ret_wk;
  
  event = EVENT_FieldSubProc_Callback( gsys, fieldmap, 
                                       FS_OVERLAY_ID(waza_oshie), &WazaOshieProcData, param,
                                       EvCmdCallWazaRemindProc_CallBack, wrwk );
  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief   ���m���X��ʃv���Z�X���Ăяo���܂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCallMonolithProc( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAME_COMM_SYS_PTR game_comm= GAMESYSTEM_GetGameCommSysPtr(gsys);
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  MONOLITH_PARENT_WORK *parent;
  INTRUDE_COMM_SYS_PTR intcomm;
  const MISSION_CHOICE_LIST *list;
  int palace_area;
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  
  parent = GFL_HEAP_AllocClearMemory(HEAPID_PROC, sizeof(MONOLITH_PARENT_WORK));

  if(intcomm != NULL 
      && MISSION_MissionList_CheckOcc(
      &intcomm->mission.list[Intrude_GetPalaceArea(intcomm)]) == TRUE){
    palace_area = Intrude_GetPalaceArea(intcomm);
    list = Intrude_GetChoiceList(intcomm, palace_area);
    parent->list = *list;
    parent->list_occ = TRUE;
  }
  else{
    OS_TPrintf("MonolithProc Call intcomm NULL!!\n");
    palace_area = 0;
    parent->list_occ = FALSE;
  }

  parent->gsys = gsys;
  parent->intcomm = intcomm;
  parent->palace_area = palace_area;
  
  EVFUNC_CallSubProc(core, work, FS_OVERLAY_ID(monolith), &MonolithProcData, parent, NULL, NULL);
  
  return VMCMD_RESULT_SUSPEND;
}

//======================================================================
//
//
//    3D�f���֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief   �f���Ăяo��
 * @param core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDemoScene( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  DEMO3D_PARAM * param = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(DEMO3D_PARAM) );
  u16 demo_id = SCRCMD_GetVMWorkValue( core, wk );

  DEMO3D_PARAM_SetFromEvTime( param, demo_id, 0, SCRCMD_WORK_GetGameData( work ) );
  EVFUNC_CallSubProc( core, work, FS_OVERLAY_ID(demo3d), &Demo3DProcData, param, NULL, NULL );

  return VMCMD_RESULT_SUSPEND;
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief   TV�g�����V�[�o�[�f���Ăяo��
 * @param core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCallTVTDemo( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  u16 demo_id = SCRCMD_GetVMWorkValue( core, wk );
  LOCAL_TVT_INIT_WORK * param = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(LOCAL_TVT_INIT_WORK) );
  param->scriptId = demo_id;
  param->gameData = SCRCMD_WORK_GetGameData( work );
  EVFUNC_CallSubProc( core, work, FS_OVERLAY_ID(local_tvt), &LocalTvt_ProcData, param, NULL, NULL );

  return VMCMD_RESULT_SUSPEND;
}



//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief   �}�ӃR���v���[�g�܏��ʁ��H���}���
 * @param core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCallZukanAward( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  MYSTATUS*      mystatus = GAMEDATA_GetMyStatus( gdata );

  u16 demo_id = SCRCMD_GetVMWorkValue( core, wk );
  u16 patern  = SCRCMD_GetVMWorkValue( core, wk );

  // �R�}���h���O�Ȃ�n���}�ӏ܏�B�P�Ȃ�S���}�ӏ܏�
  switch(demo_id){
  case SCR_ZUKAN_AWARD_CHIHOU:
    {
      CHIHOU_ZUKAN_AWARD_PARAM* param = CHIHOU_ZUKAN_AWARD_AllocParam( HEAPID_PROC, mystatus, TRUE );
      EVFUNC_CallSubProc( core, work, FS_OVERLAY_ID(chihou_zukan_award), &CHIHOU_ZUKAN_AWARD_ProcData, param, NULL, NULL );
    }
    break;
  case SCR_ZUKAN_AWARD_ZENKOKU:
    {
      ZENKOKU_ZUKAN_AWARD_PARAM* param = ZENKOKU_ZUKAN_AWARD_AllocParam( HEAPID_PROC, mystatus, TRUE );
      EVFUNC_CallSubProc( core, work, FS_OVERLAY_ID(chihou_zukan_award), &ZENKOKU_ZUKAN_AWARD_ProcData, param, NULL, NULL );
    }
    break;
  case SCR_ZUKAN_AWARD_SUBWAY_ROUTE_MAP:  //�H���}
    {
      ZENKOKU_ZUKAN_AWARD_PARAM* param = ZENKOKU_ZUKAN_AWARD_AllocParam( HEAPID_PROC, mystatus, TRUE );
      EVFUNC_CallSubProc( core, work, FS_OVERLAY_ID(chihou_zukan_award), &ZENKOKU_ZUKAN_AWARD_ProcData, param, NULL, NULL );
    }
    break;
  }
  return VMCMD_RESULT_SUSPEND;
}


