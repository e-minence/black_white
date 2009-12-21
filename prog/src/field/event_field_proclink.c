//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   event_field_proclink.c
 *  @brief  ���j���[������Ă΂��o�b�O�Ȃǂ̃A�v���P�[�V�����̌q����
 *  @author Toru=Nagihashi
 *  @data   2009.10.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "gamesystem/game_comm.h"
#include "gamesystem/game_data.h"
#include "gamesystem/game_event.h"

//�t�B�[���h
#include "field/field_msgbg.h"
#include "field/fieldmap.h"
#include "field/map_attr.h"
#include "field/itemuse_event.h"
#include "field/zonedata.h"
#include "fieldmap_ctrl.h"      //  ���|�[�g�̃C�x���g�p
#include "fieldmap_ctrl_grid.h" //  ���|�[�g�̃C�x���g�p
#include "field_player_grid.h"  //

//�e�v���Z�X
#include "event_fieldmap_control.h" //EVENT_FieldSubProc
#include "app/config_panel.h"   //ConfigPanelProcData
#include "app/trainer_card.h"   //TrainerCardSysProcData
#include "app/bag.h" // BAG_PARAM
#include "app/zukan.h" // �}��
#include "app/pokelist.h"   //PokeList_ProcData�EPLIST_DATA
#include "app/p_status.h"   //PokeList_ProcData�EPLIST_DATA
#include "app/townmap.h" //TOWNMAP_PARAM
#include "app/wifi_note.h" //
#include "app/mailtool.h" //MAIL_PARAM
#include "poke_tool/shinka_check.h" //�i���f��
#include "demo/shinka_demo.h" //�i���f��
#include "net_app/battle_recorder.h"  //�o�g�����R�[�_�[

//�A�[�J�C�u
#include "message.naix"
#include "msg/msg_fldmapmenu.h"

//���̑�
#include "item/item.h"  //ITEM_GetMailDesign

//�����̊O�����J
#include "field/event_field_proclink.h"

//-------------------------------------
/// �e�A�v���̃I�[�o�[���C
//=====================================
FS_EXTERN_OVERLAY(poke_status);
FS_EXTERN_OVERLAY(townmap);
FS_EXTERN_OVERLAY(wifinote);
FS_EXTERN_OVERLAY(pokelist);
FS_EXTERN_OVERLAY(app_mail);
FS_EXTERN_OVERLAY(battle_recorder);

//=============================================================================
/**
 *          �萔�錾
*/
//=============================================================================
//-------------------------------------
/// RETURN�֐��̖߂�l
//=====================================
typedef enum
{
  RETURNFUNC_RESULT_RETURN,     // ���j���[�ɖ߂�
  RETURNFUNC_RESULT_EXIT,       // ���j���[�������ĕ������Ԃ܂Ŗ߂�
  RETURNFUNC_RESULT_USE_SKILL,  // ���j���[�𔲂��ċZ���g��
  RETURNFUNC_RESULT_USE_ITEM,   // ���j���𔲂��ăA�C�e�����g��
  RETURNFUNC_RESULT_NEXT,       // ���̃v���Z�X�֍s��
} RETURNFUNC_RESULT;

typedef enum
{
  PROCLINK_MODE_LIST_TO_MAIL_CREATE,  //�莆���������遨�쐬���
  PROCLINK_MODE_LIST_TO_MAIL_VIEW,    //�莆�����遨View���
  PROCLINK_MODE_BAG_TO_MAIL_CREATE,   //�莆���������遨�쐬���
  PROCLINK_MODE_EVOLUTION_ITEM,       //�A�C�e���i��(�����̐�
  PROCLINK_MODE_EVOLUTION_LEVEL,      //���x���A�b�v�i��(�ӂ����ȃA��
  PROCLINK_MODE_WAZAWASURE_MACHINE,   //���X�g���Z�Y��(�Z�}�V��
  PROCLINK_MODE_WAZAWASURE_LVUP,      //���X�g���Z�Y��(�ӂ����ȃA��

}PROCLINK_TAKEOVER_MODE;  //PROC�ύX�̍ێ����z���Ȃ��l�ۑ��p
//=============================================================================
/**
 *          �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// �֐��^��`
//
//=====================================
typedef struct _PROCLINK_WORK PROCLINK_WORK;
typedef void (* PROCLINK_EVENT_FUNC)( PROCLINK_WORK* wk, u32 param );
typedef void * (* PROCLINK_CALL_PROC_FUNC)( PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs );
typedef RETURNFUNC_RESULT (* PROCLINK_RETURN_PROC_FUNC)( PROCLINK_WORK *wk, void *param_adrs );

//-------------------------------------
/// �J������֐�
//=====================================
typedef struct 
{
  EVENT_PROCLINK_MENUOPEN_FUNC  open_func;    //���j���[�����J���֐�    (NULL�ōs��Ȃ�)
  EVENT_PROCLINK_MENUCLOSE_FUNC close_func;   //���j���[�������֐�  (NULL�ōs��Ȃ�)
  void *wk_adrs;                              //�R�[���o�b�N�ɓn������
  const EVENT_PROCLINK_PARAM *param;          //����
} PROCLINK_CALLBACK_WORK;

//-------------------------------------
/// ���C�����[�N
//=====================================
struct _PROCLINK_WORK
{
  HEAPID                    heapID;

  EVENT_PROCLINK_CALL_TYPE  now_type;     //���ݑI������Ă���v���Z�X
  EVENT_PROCLINK_CALL_TYPE  pre_type;     //��O�̑I������Ă���v���Z�X�i�����Ȃ�΁��Ɠ����j
  EVENT_PROCLINK_CALL_TYPE  next_type;    //���̃��Z�X
  RETURNFUNC_RESULT         result;       //�I�����@

  GMEVENT                   *event;       //�C�x���g

  EVENT_PROCLINK_PARAM      *param;       //����
  void                      *proc_param;  //�v���Z�X�̃��[�N

  PROCLINK_CALLBACK_WORK    callback; //�C�x���g�p�Ƀ��j���[�Ȃǂ������J�����肷��֐��̃R�[���o�b�N�\����

  //�A�v���Ԃň��p�����K�v�ȃp�����[�^
  ITEMCHECK_WORK            icwk;     //�A�C�e���g�p���Ɍ��������񂪊܂܂�Ă���
  FLDSKILL_CHECK_WORK       scwk;     //�t�B�[���h�X�L�����g�p�\���ǂ����̏��
  u8                        sel_poke; //���[����ʂň����p���Ȃ��̂ŗp��
  u8                        item_no;  //���[����ʂň����p���Ȃ��̂ŗp��
  PROCLINK_TAKEOVER_MODE    mode;     //���[����ʂň����p���Ȃ��̂ŗp��
};


//=============================================================================
/**
 *        �v���g�^�C�v
 */
//=============================================================================
//-------------------------------------
/// �C�x���g
//=====================================
static GMEVENT_RESULT ProcLinkEvent( GMEVENT *event, int *seq, void *wk_adrs );

//-------------------------------------
/// FUNCITON
//=====================================
static void PROCLINK_CALLBACK_Set( PROCLINK_CALLBACK_WORK *wk, const EVENT_PROCLINK_PARAM *param, EVENT_PROCLINK_MENUOPEN_FUNC  open_func, EVENT_PROCLINK_MENUCLOSE_FUNC close_func, void *wk_adrs );
static void PROCLINK_CALLBACK_Open( PROCLINK_CALLBACK_WORK *wk );
static void PROCLINK_CALLBACK_Close( PROCLINK_CALLBACK_WORK *wk );

//-------------------------------------
/// �ePROC���Ƃ�CALL�֐���RETURN�֐�
//=====================================
//�|�P���X�g
static void * FMenuCallProc_PokeList(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs );
static RETURNFUNC_RESULT FMenuReturnProc_PokeList(PROCLINK_WORK* wk,void* param_adrs);
//�}��
//static void FMenuEvent_Zukan( PROCLINK_WORK* wk, u32 param );
static void * FMenuCallProc_Zukan(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs );
static RETURNFUNC_RESULT FMenuReturnProc_Zukan(PROCLINK_WORK* wk,void* param_adrs);
//�X�e�[�^�X
static void * FMenuCallProc_PokeStatus(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs );
static RETURNFUNC_RESULT FMenuReturnProc_PokeStatus(PROCLINK_WORK* wk,void* param_adrs);
//�o�b�O
static void * FMenuCallProc_Bag(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs );
static RETURNFUNC_RESULT FMenuReturnProc_Bag(PROCLINK_WORK* wk,void* param_adrs);
//�g���[�i�[�J�[�h
static void * FMenuCallProc_TrainerCard(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs );
static RETURNFUNC_RESULT FMenuReturnProc_TrainerCard(PROCLINK_WORK* wk,void* param_adrs);
//�^�E���}�b�v
static void * FMenuCallProc_TownMap(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs );
static RETURNFUNC_RESULT FMenuReturnProc_TownMap(PROCLINK_WORK* wk,void* param_adrs);
//�R���t�B�O
static void * FMenuCallProc_Config(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs );
static RETURNFUNC_RESULT FMenuReturnProc_Config(PROCLINK_WORK* wk,void* param_adrs);
//���|�[�g
static void FMenuEvent_Report( PROCLINK_WORK* wk, u32 param );
//WiFi�m�[�g
static void * FMenuCallProc_WifiNote(PROCLINK_WORK* wk, u32 param,EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs );
static RETURNFUNC_RESULT FMenuReturnProc_WifiNote(PROCLINK_WORK* wk,void* param_adrs);
//���[�����
static void * FMenuCallProc_Mail(PROCLINK_WORK* wk, u32 param,EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs );
static RETURNFUNC_RESULT FMenuReturnProc_Mail(PROCLINK_WORK* wk,void* param_adrs);
//�i�����
static void * FMenuCallProc_Evolution(PROCLINK_WORK* wk, u32 param,EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs );
static RETURNFUNC_RESULT FMenuReturnProc_Evolution(PROCLINK_WORK* wk,void* param_adrs);
//�o�g�����R�[�_�[
static void * FMenuCallProc_BattleRecorder(PROCLINK_WORK* wk, u32 param,EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs );
static RETURNFUNC_RESULT FMenuReturnProc_BattleRecorder(PROCLINK_WORK* wk,void* param_adrs);

//-------------------------------------
/// ���|�[�g�Ɛ}�ӂ̃C�x���g
//=====================================
static GMEVENT * createFMenuMsgWinEvent(
  GAMESYS_WORK *gsys, u32 heapID, u32 strID, FLDMSGBG *msgBG );
static GMEVENT_RESULT FMenuMsgWinEvent( GMEVENT *event, int *seq, void *wk );

static GMEVENT * createFMenuReportEvent(
  GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, u32 heapID, FLDMSGBG *msgBG );
static GMEVENT_RESULT FMenuReportEvent( GMEVENT *event, int *seq, void *wk );


//=============================================================================
/**
 *        �f�[�^
 */
//=============================================================================
//-------------------------------------
/// �Ăяo�����A�v���e�[�u��
//  EVENT_PROCLINK_CALL_TYPE�̗񋓂�
//=====================================
static const struct 
{
  FSOverlayID               ovId;       //�I�[�o�[���CID
  const GFL_PROC_DATA       *proc_data; //�v���Z�X
  PROCLINK_CALL_PROC_FUNC   call_func;    //�J�n�O�֐�
  PROCLINK_RETURN_PROC_FUNC return_func;  //�I�����֐�
  PROCLINK_EVENT_FUNC       event_func;   //�A�v�����C�x���g�������ꍇ
} ProcLinkData[ EVENT_PROCLINK_CALL_MAX ] =
{ 

  //EVENT_PROCLINK_CALL_POKELIST,
  { 
    FS_OVERLAY_ID(pokelist) , 
    &PokeList_ProcData ,
    FMenuCallProc_PokeList ,
    FMenuReturnProc_PokeList ,
    NULL,
  },
  //EVENT_PROCLINK_CALL_ZUKAN,
  { 
    FS_OVERLAY_ID(zukan),
    &ZUKAN_ProcData,
    FMenuCallProc_Zukan,
    FMenuReturnProc_Zukan,
//    FMenuEvent_Zukan,
    NULL,
  },
  //EVENT_PROCLINK_CALL_BAG,          
  { 
    FS_OVERLAY_ID(bag) , 
    &ItemMenuProcData  ,
    FMenuCallProc_Bag,
    FMenuReturnProc_Bag,
    NULL,
  },
  //EVENT_PROCLINK_CALL_TRAINERCARD,  
  { 
    TRCARD_OVERLAY_ID, 
    &TrCardSysProcData ,
    FMenuCallProc_TrainerCard,
    FMenuReturnProc_TrainerCard,
    NULL,
  },
  //EVENT_PROCLINK_CALL_REPORT
  { 
    0 , NULL , NULL, NULL, 
    FMenuEvent_Report,
  },
  //EVENT_PROCLINK_CALL_CONFIG,       
  { 
    FS_OVERLAY_ID(config_panel) , 
    &ConfigPanelProcData ,
    FMenuCallProc_Config,
    FMenuReturnProc_Config,
    NULL,
  },

  //EVENT_PROCLINK_CALL_STATUS
  { 
    FS_OVERLAY_ID(poke_status),
    &PokeStatus_ProcData,
    FMenuCallProc_PokeStatus,
    FMenuReturnProc_PokeStatus,
    NULL,
  },

  //EVENT_PROCLINK_CALL_TOWNMAP
  { 
    FS_OVERLAY_ID(townmap),
    &TownMap_ProcData,
    FMenuCallProc_TownMap,
    FMenuReturnProc_TownMap,
    NULL,
  },

  //EVENT_PROCLINK_CALL_WIFINOTE
  { 
    FS_OVERLAY_ID(wifinote),
    &WifiNoteProcData,
    FMenuCallProc_WifiNote,
    FMenuReturnProc_WifiNote,
    NULL,
  },
  //EVENT_PROCLINK_CALL_MAIL
  { 
    NO_OVERLAY_ID,
    &MailSysProcData,
    FMenuCallProc_Mail,
    FMenuReturnProc_Mail,
    NULL,
  },
  //EVENT_PROCLINK_CALL_EVOLUTION
  { 
    NO_OVERLAY_ID,
    &ShinkaDemoProcData,
    FMenuCallProc_Evolution,
    FMenuReturnProc_Evolution,
    NULL,
  },
  //EVENT_PROCLINK_CALL_BTLRECORDER
  { 
    FS_OVERLAY_ID(battle_recorder),
    &BattleRecorder_ProcData,
    FMenuCallProc_BattleRecorder,
    FMenuReturnProc_BattleRecorder,
    NULL,
  },
};

//=============================================================================
/**
 *          �O���Q��
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[�̃C�x���g�ȂǃA�v�����N������
 *
 *  @param  EVENT_PROCLINK_PARAM *param �����i�A���P�[�g�������̂�n���Ă��������j
 *  @param  heapID                      �q�[�vID
 *
 *  @return                             �C�x���g
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_ProcLink( EVENT_PROCLINK_PARAM *param, HEAPID heapID )
{ 
  GMEVENT       *event;
  PROCLINK_WORK *wk;

  //�C�x���g�쐬
  event = GMEVENT_Create( param->gsys, param->event, ProcLinkEvent, sizeof(PROCLINK_WORK));

  //���[�N
  wk    = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear( wk, sizeof(PROCLINK_WORK) );
  wk->heapID  = heapID;
  wk->param   = param;
  wk->result  = RETURNFUNC_RESULT_NEXT;
  wk->event   = event;

  wk->now_type  = wk->param->call;
  wk->pre_type  = wk->param->call;
  wk->next_type = wk->param->call;
  
  wk->sel_poke = 0;
  wk->item_no = 0;
  wk->mode = 0;

  PROCLINK_CALLBACK_Set( &wk->callback, wk->param, wk->param->open_func, wk->param->close_func, wk->param->wk_adrs );

  NAGI_Printf(  "init call %d\n", wk->param->call);

  //�A�C�e���g�p�������[�N�ݒ�
  { 
    const VecFx32 *pPos;
    GAMEDATA* pGameData = GAMESYSTEM_GetGameData(wk->param->gsys);
    PLAYER_WORK *playerWork = GAMEDATA_GetMyPlayerWork( pGameData );
    FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer(wk->param->field_wk);

    pPos = PLAYERWORK_getPosition( playerWork );
    wk->icwk.NowAttr = FIELD_PLAYER_GetMapAttr(fld_player);
    wk->icwk.FrontAttr =FIELD_PLAYER_GetDirMapAttr(fld_player, FIELD_PLAYER_GetDir(fld_player));
    wk->icwk.gsys = wk->param->gsys;
    //�]�[���h�c
    wk->icwk.zone_id = PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork(pGameData ));
    //�A�����
    wk->icwk.Companion = FALSE;

    wk->icwk.PlayerForm=0;  //�@���@�̌`��i���]�Ԃɏ���Ă��邩�Ƃ��j
    wk->icwk.SeedInfo=0;  //�g�p�\�ȃA�C�e�����i���̂݊֘A�j  
  }
  FLDSKILL_InitCheckWork( wk->param->field_wk, &wk->scwk );

  return event;
}

//=============================================================================
/**
 *          �C�x���g
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  PROC���q���ł�����ԕψڃC�x���g
 *
 *  @param  GMEVENT *event  �C�x���g
 *  @param  *seq            �V�[�P���X
 *  @param  *wk_adrs        ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT ProcLinkEvent( GMEVENT *event, int *seq, void *wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_JUNCTION,
    SEQ_END,

    //�C�x���g
    SEQ_EVENT_CALL,
    SEQ_EVENT_WAIT,
    SEQ_EVENT_RETURN,

    //�t�B�[���h�����i�v���b�N�ړ��̍ۂ͂�����ʂ�j
    SEQ_FLD_FADEOUT,
    SEQ_FLD_CLOSE,
    SEQ_FLD_OPEN,
    SEQ_FLD_FADEIN,

    //�v���b�N�q����
    SEQ_PROC_CALL,
    SEQ_PROC_WAIT,
    SEQ_PROC_RETURN,
  };
  /*
   *  ����
   *  �A�v���ړ��ɂ͈ȉ��̎�ނ�����
   *  �E�v���b�N
   *  �E�C�x���g
   *  ����āAJUNCTION�łǂ��炩���f��
   *  �v���b�N�̏ꍇ�́A�t�B�[���h�ɂ�����A�t�B�[���h�𔲂��ăv���b�N��
   *  �C�x���g�̏ꍇ�́A�t�B�[���h�ɂȂ�������A�t�B�[���h�ɖ߂��ăC�x���g�w
   *  ����𔻒f����̂�SEQ_JUNCTION
   *
   */

  PROCLINK_WORK *wk = wk_adrs;

  switch( *seq )
  { 
  case SEQ_INIT:
    *seq  = SEQ_JUNCTION;
    break;

  case SEQ_JUNCTION:
    if( wk->result == RETURNFUNC_RESULT_RETURN ||
        wk->result == RETURNFUNC_RESULT_EXIT )
    {
      //�I���̏ꍇ
      *seq  = SEQ_END;
    }
    else if( wk->result == RETURNFUNC_RESULT_NEXT )
    { 
      NAGI_Printf( "now_type  %d\n", wk->now_type );
      //���ɍs���ꍇ
      if( ProcLinkData[ wk->now_type ].event_func == NULL )
      { 
        *seq  = SEQ_FLD_FADEOUT;
      }
      else
      {
        *seq  = SEQ_EVENT_CALL;
      }
    }
    else if( wk->result == RETURNFUNC_RESULT_USE_ITEM )
    { 
      //�A�C�e�����I�����ꂽ�ꍇ
      wk->param->result = EVENT_PROCLINK_RESULT_ITEM;
      *seq  = SEQ_END;
    }
    else if( wk->result == RETURNFUNC_RESULT_USE_SKILL )
    { 
      //�Z���I�����ꂽ�ꍇ
      wk->param->result = EVENT_PROCLINK_RESULT_SKILL;
      *seq  = SEQ_END;
    }
    break;

  case SEQ_END:
    if( wk->result == RETURNFUNC_RESULT_RETURN )
    { 
      wk->param->result = EVENT_PROCLINK_RESULT_RETURN;
    }
    else if( wk->result == RETURNFUNC_RESULT_EXIT )
    { 
      wk->param->result = EVENT_PROCLINK_RESULT_EXIT;
    }
    //�A�C�e���ƋZ�͂��������Ă���
    return GMEVENT_RES_FINISH;

  //-------------------------------------
  /// �C�x���g
  //=====================================
  case SEQ_EVENT_CALL:
    GF_ASSERT( wk->now_type < EVENT_PROCLINK_CALL_MAX );

    PROCLINK_CALLBACK_Close( &wk->callback );

    ProcLinkData[ wk->now_type ].event_func( wk, wk->param->data );
    *seq  = SEQ_EVENT_WAIT;
    break;

  case SEQ_EVENT_WAIT:
    *seq  = SEQ_EVENT_RETURN;
    break;

  case SEQ_EVENT_RETURN:
    PROCLINK_CALLBACK_Open( &wk->callback );
    *seq  = SEQ_JUNCTION;
    break;

  //-------------------------------------
  /// �T�u�v���b�N�Ăяo���̂��߂̃t�B�[���h����
  //=====================================
  case SEQ_FLD_FADEOUT:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut_Black(wk->param->gsys, wk->param->field_wk, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
    *seq = SEQ_FLD_CLOSE;
    break;
    
  case SEQ_FLD_CLOSE:
    PROCLINK_CALLBACK_Close( &wk->callback );
    GMEVENT_CallEvent(event, EVENT_FieldClose(wk->param->gsys, wk->param->field_wk));
    *seq = SEQ_PROC_CALL; //PROC�֍s��
    break;
    
  case SEQ_FLD_OPEN:      //PROC����߂�
    GMEVENT_CallEvent(event, EVENT_FieldOpen(wk->param->gsys));
    *seq = SEQ_FLD_FADEIN;
    break;
    
  case SEQ_FLD_FADEIN:
    //�t�B�[���h����蒼�������߁A�ēx�擾
    wk->param->field_wk = GAMESYSTEM_GetFieldMapWork(wk->param->gsys);
    FIELDMAP_InitBGMode();
    FIELDMAP_InitBG( wk->param->field_wk );
    PROCLINK_CALLBACK_Open( &wk->callback );  
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Menu(wk->param->gsys, wk->param->field_wk, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
    *seq = SEQ_JUNCTION;
    break;

  //-------------------------------------
  /// �T�u�v���b�N�Ăяo������
  //    ��{�I�Ɉȉ��̂R�̃V�[�P���X�����邮����܂�
  //=====================================
  //�v���Z�X�Ăяo��
  case SEQ_PROC_CALL:
    { 
      void *now_param;
      //���݂̃v���Z�X�p�����[�^�쐬
      GF_ASSERT( wk->now_type < EVENT_PROCLINK_CALL_MAX );
      GF_ASSERT( ProcLinkData[ wk->now_type ].call_func );
      now_param = ProcLinkData[ wk->now_type ].call_func( wk, wk->param->data, wk->pre_type, wk->proc_param );

      //�O�̃v���Z�X�p�����[�^�j��
      if( wk->proc_param )
      { 
        GFL_HEAP_FreeMemory(wk->proc_param);
        wk->proc_param = NULL;
      }

      //���̃v���Z�X�p�����[�^�ݒ�
      wk->proc_param  = now_param;

      //���̃v���Z�X�ֈړ�
      if( ProcLinkData[ wk->now_type ].proc_data )
      { 
        GAMESYSTEM_CallProc( wk->param->gsys, 
            ProcLinkData[ wk->now_type ].ovId , 
            ProcLinkData[ wk->now_type ].proc_data, 
            wk->proc_param);
        *seq  = SEQ_PROC_WAIT;
      }
    }
    break;

  //�v���Z�X�҂�
  case SEQ_PROC_WAIT:
    if( GAMESYSTEM_IsProcExists(wk->param->gsys) == GFL_PROC_MAIN_NULL )
    { 
      *seq  = SEQ_PROC_RETURN;
    }
    break;

  //�v���Z�X�߂�
  case SEQ_PROC_RETURN:
    //���ւ̌q������擾
    GF_ASSERT( wk->now_type < EVENT_PROCLINK_CALL_MAX );
    wk->result  = ProcLinkData[ wk->now_type ].return_func( wk, wk->proc_param );

    //�����֑��
    if( wk->result == RETURNFUNC_RESULT_RETURN )
    { 
      wk->param->result = EVENT_PROCLINK_RESULT_RETURN;
    }
    else if( wk->result == RETURNFUNC_RESULT_EXIT )
    { 
      wk->param->result = EVENT_PROCLINK_RESULT_EXIT;
    }
    else if( wk->result == RETURNFUNC_RESULT_USE_ITEM )
    { 
      //�A�C�e�����I�����ꂽ�ꍇ
      wk->param->result = EVENT_PROCLINK_RESULT_ITEM;
    }
    else if( wk->result == RETURNFUNC_RESULT_USE_SKILL )
    { 
      //�Z���I�����ꂽ�ꍇ
      wk->param->result = EVENT_PROCLINK_RESULT_SKILL;
    }

    //����
    if( wk->result == RETURNFUNC_RESULT_NEXT )
    { 
      //�����v���Z�X�̏ꍇ�͂��邮����
      wk->pre_type  = wk->now_type;
      wk->now_type  = wk->next_type;

      NAGI_Printf(  "���̃v���Z�X�ցI pre %d now %d\n", wk->pre_type, wk->now_type );
      *seq = SEQ_PROC_CALL;
    }
    else
    { 
      //�I������̂ŁA������I����Ԃɂ���ĊO���֐ݒ�
      if( wk->proc_param )
      { 
        GFL_HEAP_FreeMemory(wk->proc_param);
        wk->proc_param = NULL;
      }
      *seq = SEQ_FLD_OPEN;
      NAGI_Printf(  "�v���Z�X�I���I \n" );
    }
    break;
  }


  return GMEVENT_RES_CONTINUE;
}
//=============================================================================
/**
 *  CALL�֐���RETURN�֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �֐��\���̐ݒ�
 *
 *  @param    wk          ���[�N
 *  @param    param       ����
 *  @param    open_func   �J���֐�
 *  @param    close_func  ����֐�
 *  @param    wk_adrs     �����ɗ^�������[�N
 */
//-----------------------------------------------------------------------------
static void PROCLINK_CALLBACK_Set( PROCLINK_CALLBACK_WORK *wk, const EVENT_PROCLINK_PARAM *param, EVENT_PROCLINK_MENUOPEN_FUNC  open_func, EVENT_PROCLINK_MENUCLOSE_FUNC close_func, void *wk_adrs )
{ 
  GFL_STD_MemClear( wk, sizeof(PROCLINK_CALLBACK_WORK) );
  wk->open_func   = open_func;
  wk->close_func  = close_func;
  wk->wk_adrs     = wk_adrs;
  wk->param       = param;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �J���֐������s
 *
 *  @param  PROCLINK_CALLBACK_WORK *wk ���[�N
 */
//-----------------------------------------------------------------------------
static void PROCLINK_CALLBACK_Open( PROCLINK_CALLBACK_WORK *wk )
{ 
  if( wk->open_func )
  { 
    wk->open_func( wk->param, wk->wk_adrs );
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  ����֐������s
 *
 *  @param  PROCLINK_CALLBACK_WORK *wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void PROCLINK_CALLBACK_Close( PROCLINK_CALLBACK_WORK *wk )
{ 
  if( wk->close_func )
  { 
    wk->close_func( wk->param, wk->wk_adrs );
  }
}

//=============================================================================
/**
 *  CALL�֐���RETURN�֐�
 */
//=============================================================================
//-------------------------------------
/// �|�P���X�g
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  CALL�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param   �N��������
 *  @param  pre     ��O�̃v���b�N
 *  @param  void* pre_param_adrs  ��O�̃v���b�N�p�����[�^
 *
 *  @return �v���Z�X�p�����[�^
 */
//-----------------------------------------------------------------------------
static void * FMenuCallProc_PokeList(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs )
{ 
  PLIST_DATA *plistData;
  GAMEDATA *gmData = GAMESYSTEM_GetGameData( wk->param->gsys );

  plistData     = GFL_HEAP_AllocClearMemory(HEAPID_PROC, sizeof(PLIST_DATA));

  plistData->pp = GAMEDATA_GetMyPokemon( gmData);
  plistData->scwk = wk->scwk;
  plistData->myitem   = GAMEDATA_GetMyItem( gmData);    // �A�C�e���f�[�^
  plistData->mailblock = GAMEDATA_GetMailBlock( gmData);
  plistData->mode     = PL_MODE_FIELD;
  plistData->ret_sel  = wk->param->select_poke;
  plistData->waza = 0;
  plistData->place    = ZONEDATA_GetPlaceNameID( GAMEDATA_GetMyPlayerWork(gmData)->zoneID ); 

  if( pre == EVENT_PROCLINK_CALL_POKELIST )
  { 
    //���񎞂�������
    plistData->ret_sel  = 0;
  }
  else
  if( pre == EVENT_PROCLINK_CALL_BAG )
  { 
    //�P�O���o�b�O�ŁA�|�P���X�g�ֈڍs�������Ƃ�
    const BAG_PARAM *pBag = pre_param_adrs;

    plistData->item = pBag->ret_item;     //�A�C�e��ID

    switch( pBag->next_proc )
    { 
    case BAG_NEXTPROC_RETURN:
      if(pBag->mode == BAG_MODE_POKELIST){   //���X�g����Ă΂�Ă����ꍇ
        plistData->mode = PL_MODE_ITEMSET;    //�A�C�e�����Z�b�g����Ăяo��
      }
      break;
      
    case BAG_NEXTPROC_EVOLUTION:
      plistData->mode = PL_MODE_SHINKA;    //�i���A�C�e��
      break;
      
    case BAG_NEXTPROC_HAVE:  // �|�P�����ɃA�C�e������������
      plistData->mode = PL_MODE_ITEMSET;    //�A�C�e�����Z�b�g����Ăяo��
      break;

    case BAG_NEXTPROC_ITEMEQUIP:  //�|�P�������X�g�̃A�C�e�������ɖ߂�
      plistData->mode     = PL_MODE_ITEMSET_RET;
      break;

    case BAG_NEXTPROC_ITEMHAVE_RET:  //�|�P�������X�g�̃A�C�e�������ɖ߂邪�A�L�����Z���̂Ƃ�
      plistData->mode     = PL_MODE_ITEMSET_RET;
      break;

    case BAG_NEXTPROC_ITEMUSE:  //�g��
      plistData->mode     = PL_MODE_ITEMUSE;
      break;

    case BAG_NEXTPROC_WAZASET: //�Z�Z�b�g
      plistData->mode     = PL_MODE_WAZASET;
      break;

    default:
      plistData->mode = PL_MODE_ITEMSET;    //�A�C�e�����Z�b�g����Ăяo��
      break;
    }
  }
  else if( pre == EVENT_PROCLINK_CALL_STATUS )
  { 
    //�P�O���X�e�[�^�X�ŁA�|�P���X�g�ֈڍs�������Ƃ�
    const PSTATUS_DATA *psData  = pre_param_adrs;
    
    switch( psData->ret_mode )
    { 
    case PST_RET_DECIDE:
      /* fallthrough */
    case PST_RET_CANCEL:
      if( psData->mode == PST_MODE_WAZAADD )
      {
        if( wk->mode == PROCLINK_MODE_WAZAWASURE_LVUP )
        {
          //�s�v�c�ȃA��
          plistData->mode = PL_MODE_LVUPWAZASET_RET;
        }
        else
        {
          //�Z�}�V��
          plistData->mode = PL_MODE_WAZASET_RET;
        }
        plistData->item = wk->param->select_param;
        plistData->waza = psData->waza;
        if( psData->ret_mode == PST_RET_DECIDE )
        {
          plistData->waza_pos = psData->ret_sel;
        }
        else
        {
          plistData->waza_pos = 0xFF;
        }
      }
      else
      {
        plistData->mode = PL_MODE_FIELD;
        plistData->ret_sel = psData->pos;
      }

      break;

    default:
      GF_ASSERT( 0 );
    }
  }
  else
  if( pre == EVENT_PROCLINK_CALL_MAIL )
  {
    //���[�����������鏈��(�����Ȃ������ꍇ�̓��[����Term�ŕ��򂵂Ă�
    if( wk->mode == PROCLINK_MODE_LIST_TO_MAIL_CREATE )
    {
      plistData->mode = PL_MODE_MAILSET;
      plistData->ret_sel = wk->sel_poke;
      plistData->item = wk->item_no;
    }
    else
    if( wk->mode == PROCLINK_MODE_BAG_TO_MAIL_CREATE )
    {
      plistData->mode    = PL_MODE_MAILSET_BAG;
      plistData->ret_sel = wk->sel_poke;
      plistData->item    = wk->item_no;
    }
    else
    {
      //���[���������Ƃ�
      //���ɖ����B
    }
  }

  return plistData;
}
//----------------------------------------------------------------------------
/**
 *  @brief  RETURN�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param_adrs  �����̃v���b�N�p�����[�^
 *
 *  @return   RETURNFUNC_RESULT_RETURN,     //���j���[�ɖ߂�
 *            RETURNFUNC_RESULT_EXIT,       //���j���[�������ĕ������Ԃ܂Ŗ߂�
 *            RETURNFUNC_RESULT_USE_SKILL,    //���j���[�𔲂��ċZ���g��
 *            RETURNFUNC_RESULT_USE_ITEM,   //���j���𔲂��ăA�C�e�����g��
 *            RETURNFUNC_RESULT_NEXT,       //���̃v���Z�X�֍s��
 */ 
//-----------------------------------------------------------------------------
static RETURNFUNC_RESULT FMenuReturnProc_PokeList(PROCLINK_WORK* wk,void* param_adrs)
{ 
  const PLIST_DATA *plData = param_adrs;
  
  wk->param->select_poke  = plData->ret_sel;
  switch( plData->ret_mode )
  {
  case PL_RET_NORMAL:      // �ʏ�
    if( plData->mode == PL_MODE_WAZASET || wk->param->call == EVENT_PROCLINK_CALL_BAG ) 
    { 
      wk->next_type = EVENT_PROCLINK_CALL_BAG;
      return RETURNFUNC_RESULT_NEXT;
    }
    else
    {
      if(plData->ret_sel == PL_SEL_POS_EXIT2)
      {
        return RETURNFUNC_RESULT_EXIT;
      }
      else
      { 
        return RETURNFUNC_RESULT_RETURN;
      }
    }
    break;
    
  case PL_RET_STATUS:      // ���j���[�u�悳���݂�v
    wk->next_type = EVENT_PROCLINK_CALL_STATUS;
    return RETURNFUNC_RESULT_NEXT;
  
  //��������@�ŃA�C�e����ʂ�
  case PL_RET_ITEMSET:
    wk->next_type = EVENT_PROCLINK_CALL_BAG;
    return RETURNFUNC_RESULT_NEXT;
    
  case PL_RET_BAG:      // �A�C�e�������X�g���A�C�e��
    wk->next_type = EVENT_PROCLINK_CALL_BAG;
    return RETURNFUNC_RESULT_NEXT;
    
  case PL_RET_WAZASET:  //�Y���Z�I��
  case PL_RET_LVUP_WAZASET:
    wk->next_type = EVENT_PROCLINK_CALL_STATUS;
    return RETURNFUNC_RESULT_NEXT;
    
  case PL_RET_MAILSET:
  case PL_RET_MAILREAD:
    wk->next_type = EVENT_PROCLINK_CALL_MAIL;
    return RETURNFUNC_RESULT_NEXT;
    break;

  case PL_RET_ITEMSHINKA:
  case PL_RET_LVUPSHINKA:
    wk->next_type = EVENT_PROCLINK_CALL_EVOLUTION;
    return RETURNFUNC_RESULT_NEXT;
    break;

  case PL_RET_IAIGIRI:     // ���j���[ �Z�F����������
  case PL_RET_NAMINORI:    // ���j���[ �Z�F�Ȃ݂̂�
  case PL_RET_TAKINOBORI:  // ���j���[ �Z�F�����̂ڂ�
  case PL_RET_KIRIBARAI:   // ���j���[ �Z�F����΂炢
  case PL_RET_KAIRIKI:     // ���j���[ �Z�F�����肫

  case PL_RET_IWAKUDAKI:   // ���j���[ �Z�F���킭����
  case PL_RET_ROCKCLIMB:   // ���j���[ �Z�F���b�N�N���C��
  case PL_RET_FLASH:       // ���j���[ �Z�F�t���b�V��
  case PL_RET_TELEPORT:    // ���j���[ �Z�F�e���|�[�g
  case PL_RET_ANAWOHORU:   // ���j���[ �Z�F���Ȃ��ق�
  case PL_RET_AMAIKAORI:   // ���j���[ �Z�F���܂�������
  case PL_RET_OSYABERI:    // ���j���[ �Z�F������ׂ�
    wk->param->select_param = plData->ret_mode-PL_RET_IAIGIRI;
    return RETURNFUNC_RESULT_USE_SKILL;

  case PL_RET_SORAWOTOBU:  // ���j���[ �Z�F������Ƃ�
    wk->next_type = EVENT_PROCLINK_CALL_TOWNMAP;
//    wk->param->select_param = plData->ret_mode-PL_RET_IAIGIRI;
    return RETURNFUNC_RESULT_NEXT;

  default:
    GF_ASSERT(0);
    return RETURNFUNC_RESULT_EXIT;
    
/*
  case PL_RET_NORMAL:      // �ʏ�
  case PL_RET_STATUS:      // ���j���[�u�悳���݂�v
  case PL_RET_CHANGE:      // ���j���[�u���ꂩ����v
  case PL_RET_ITEMSET:     // ���j���[�u��������v

  case PL_RET_WAZASET:     // �Z�I���ցi�Z�}�V���j
  case PL_RET_LVUP_WAZASET:// �Z�I���ցi�s�v�c�ȃA���j
  case PL_RET_MAILSET:     // ���[�����͂�
  case PL_RET_MAILREAD:    // ���[����ǂމ�ʂ�

  case PL_RET_ITEMSHINKA:  // �i����ʂցi�A�C�e���i���j
  case PL_RET_LVUPSHINKA:  // �i����ʂցi���x���i���j
  
  case PL_RET_BAG:         // �o�b�O����Ă΂ꂽ�ꍇ�ŁA�o�b�O�֖߂�

  
  case PL_RET_IAIGIRI:     // ���j���[ �Z�F����������
  case PL_RET_SORAWOTOBU:  // ���j���[ �Z�F������Ƃ�
  case PL_RET_NAMINORI:    // ���j���[ �Z�F�Ȃ݂̂�
  case PL_RET_KAIRIKI:     // ���j���[ �Z�F�����肫
  case PL_RET_KIRIBARAI:   // ���j���[ �Z�F����΂炢
  case PL_RET_IWAKUDAKI:   // ���j���[ �Z�F���킭����
  case PL_RET_TAKINOBORI:  // ���j���[ �Z�F�����̂ڂ�
  case PL_RET_ROCKCLIMB:   // ���j���[ �Z�F���b�N�N���C��

  case PL_RET_FLASH:       // ���j���[ �Z�F�t���b�V��
  case PL_RET_TELEPORT:    // ���j���[ �Z�F�e���|�[�g
  case PL_RET_ANAWOHORU:   // ���j���[ �Z�F���Ȃ��ق�
  case PL_RET_AMAIKAORI:   // ���j���[ �Z�F���܂�������
  case PL_RET_OSYABERI:    // ���j���[ �Z�F������ׂ�
  case PL_RET_MILKNOMI:    // ���j���[ �Z�F�~���N�̂�
  case PL_RET_TAMAGOUMI:   // ���j���[ �Z�F�^�}�S����
*/        
  }


}
//-------------------------------------
/// �}��
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  EVENT�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param   �N��������
 */
//-----------------------------------------------------------------------------
/*
static void FMenuEvent_Zukan( PROCLINK_WORK* wk, u32 param )
{ 
  GMEVENT * subevent = createFMenuMsgWinEvent( wk->param->gsys, wk->heapID,
    FLDMAPMENU_STR08, FIELDMAP_GetFldMsgBG(wk->param->field_wk) );
  GMEVENT_CallEvent(wk->event, subevent);

  wk->result  = RETURNFUNC_RESULT_RETURN;
}
*/
//----------------------------------------------------------------------------
/**
 *  @brief  CALL�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param   �N��������
 *  @param  pre     ��O�̃v���b�N
 *  @param  void* pre_param_adrs  ��O�̃v���b�N�p�����[�^
 *
 *  @return �v���Z�X�p�����[�^
 */
//-----------------------------------------------------------------------------
static void * FMenuCallProc_Zukan(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs )
{
  ZUKAN_PARAM * prm = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(ZUKAN_PARAM) );

  prm->callMode = ZUKAN_MODE_LIST;

  return prm;
}
//----------------------------------------------------------------------------
/**
 *  @brief  RETURN�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param_adrs  �����̃v���b�N�p�����[�^
 *
 *  @return   RETURNFUNC_RESULT_RETURN,     //���j���[�ɖ߂�
 *            RETURNFUNC_RESULT_EXIT,       //���j���[�������ĕ������Ԃ܂Ŗ߂�
 *            RETURNFUNC_RESULT_USE_SKILL,    //���j���[�𔲂��ċZ���g��
 *            RETURNFUNC_RESULT_USE_ITEM,   //���j���𔲂��ăA�C�e�����g��
 *            RETURNFUNC_RESULT_NEXT,       //���̃v���Z�X�֍s��
 */ 
//-----------------------------------------------------------------------------
static RETURNFUNC_RESULT FMenuReturnProc_Zukan(PROCLINK_WORK* wk,void* param_adrs)
{
  ZUKAN_PARAM * prm = param_adrs;

  if( prm->retMode == ZUKAN_RET_NORMAL ){
    return RETURNFUNC_RESULT_RETURN;
  }
  return RETURNFUNC_RESULT_EXIT;
}

//-------------------------------------
/// �X�e�[�^�X
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  CALL�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param   �N��������
 *  @param  pre     ��O�̃v���b�N
 *  @param  void* pre_param_adrs  ��O�̃v���b�N�p�����[�^
 *
 *  @return �v���Z�X�p�����[�^
 */
//-----------------------------------------------------------------------------
static void * FMenuCallProc_PokeStatus(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs )
{ 
  PSTATUS_DATA *psData = GFL_HEAP_AllocClearMemory( HEAPID_PROC , sizeof(PSTATUS_DATA) );
  GAMEDATA *gmData = GAMESYSTEM_GetGameData(wk->param->gsys);
  SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork( gmData );

  psData->ppt = PST_PP_TYPE_POKEPARTY;
  psData->game_data = gmData;
  psData->isExitRequest = FALSE;

  if( pre == EVENT_PROCLINK_CALL_POKELIST )
  { 
    //���X�g����Ăяo��
    const PLIST_DATA *plData    = pre_param_adrs;

    psData->ppd   = (void*)plData->pp;
    psData->cfg   = plData->cfg;
    psData->max   = PokeParty_GetPokeCount( plData->pp );
    psData->pos   = plData->ret_sel;

    //���X�g�̃��[�h�ɂ���āA�X�e�[�^�X�̃��[�h���Ⴄ
    switch( plData->ret_mode )
    { 
    case PL_RET_WAZASET:  //�Y���Z�I�� 
      psData->waza = plData->waza;
      psData->mode  = PST_MODE_WAZAADD;
      psData->page  = PPT_SKILL;
      psData->canExitButton = FALSE;
      wk->mode = PROCLINK_MODE_WAZAWASURE_MACHINE;
      break;

    case PL_RET_LVUP_WAZASET://�Y���Z�I�� 
      psData->waza = plData->waza;
      psData->mode  = PST_MODE_WAZAADD;
      psData->page  = PPT_SKILL;
      psData->canExitButton = FALSE;
      wk->mode = PROCLINK_MODE_WAZAWASURE_LVUP;
      break;

    default:
      psData->page  = PPT_INFO;
      psData->mode  = PST_MODE_NORMAL;
      psData->canExitButton = TRUE;
    }
  }
  else if( pre == EVENT_PROCLINK_CALL_STATUS )
  { 
    //���ڌďo��
    psData->cfg   = SaveData_GetConfig( svWork );
    psData->ppd   = (void*)GAMEDATA_GetMyPokemon( gmData );
    psData->max   = PokeParty_GetPokeCount( GAMEDATA_GetMyPokemon( gmData ) );  
    psData->pos   = 0;
    psData->mode  = PST_MODE_NORMAL;
    psData->canExitButton = TRUE;
    if( param != EVENT_PROCLINK_DATA_NONE )
    { 
      psData->page  = param;
    }
    else
    { 
      psData->page  = PPT_INFO;
    }
  }



  return psData;
}
//----------------------------------------------------------------------------
/**
 *  @brief  RETURN�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param_adrs  �����̃v���b�N�p�����[�^
 *
 *  @return   RETURNFUNC_RESULT_RETURN,     //���j���[�ɖ߂�
 *            RETURNFUNC_RESULT_EXIT,       //���j���[�������ĕ������Ԃ܂Ŗ߂�
 *            RETURNFUNC_RESULT_USE_SKILL,    //���j���[�𔲂��ċZ���g��
 *            RETURNFUNC_RESULT_USE_ITEM,   //���j���𔲂��ăA�C�e�����g��
 *            RETURNFUNC_RESULT_NEXT,       //���̃v���Z�X�֍s��
 */ 
//-----------------------------------------------------------------------------
static RETURNFUNC_RESULT FMenuReturnProc_PokeStatus(PROCLINK_WORK* wk,void* param_adrs)
{ 
  const PSTATUS_DATA *psData = param_adrs;

  switch( psData->ret_mode )
  {
  case PST_RET_DECIDE:
    /* fallthrough */
  case PST_RET_CANCEL:
    if( wk->param->call == EVENT_PROCLINK_CALL_STATUS )
    { 
      return RETURNFUNC_RESULT_RETURN;
    }
    else
    { 
      wk->next_type = EVENT_PROCLINK_CALL_POKELIST;
      return RETURNFUNC_RESULT_NEXT;
    }

  case PST_RET_EXIT:
    return RETURNFUNC_RESULT_EXIT;
  
  default:
    GF_ASSERT(0);
    return RETURNFUNC_RESULT_EXIT;
  }
}
//-------------------------------------
/// �o�b�O
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  CALL�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param   �N��������
 *  @param  pre     ��O�̃v���b�N
 *  @param  void* pre_param_adrs  ��O�̃v���b�N�p�����[�^
 *
 *  @return �v���Z�X�p�����[�^
 */
//-----------------------------------------------------------------------------
static void * FMenuCallProc_Bag(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs )
{ 
  BAG_PARAM* bag;
  GAMEDATA* gmData = GAMESYSTEM_GetGameData( wk->param->gsys );

  switch( pre )
  { 
  case EVENT_PROCLINK_CALL_BAG:
    //�o�b�O���ŏ��ɌĂяo�����Ƃ�
    bag = BAG_CreateParam( gmData, &wk->icwk, BAG_MODE_FIELD, HEAPID_PROC );
    if( param != EVENT_PROCLINK_DATA_NONE )
    { 
      MYITEM_FieldBagPocketSet( bag->p_bagcursor, param );
    }
    break;
    
  case EVENT_PROCLINK_CALL_POKELIST:
    { 
      const PLIST_DATA *plData = pre_param_adrs;
      if( plData->ret_mode == PL_RET_ITEMSET )
      { 
        //��������Ńo�b�O��ʂ�
        bag = BAG_CreateParam( gmData, &wk->icwk, BAG_MODE_POKELIST, HEAPID_PROC );
      }
      else if( plData->ret_mode == PL_RET_BAG )
      { 
        //�A�C�e���[�����X�g�[���A�C�e��
        bag = BAG_CreateParam( gmData, &wk->icwk, BAG_MODE_FIELD, HEAPID_PROC );
      }
      else
      { 
        //�A�C�e���[�����X�g�[���A�C�e��
        bag = BAG_CreateParam( gmData, &wk->icwk, BAG_MODE_FIELD, HEAPID_PROC );
      }
    }
    break;

    //�o�b�O�ɖ߂��Ă���Ƃ�
  case EVENT_PROCLINK_CALL_TOWNMAP:
    /* fallthrough */
  case EVENT_PROCLINK_CALL_WIFINOTE:
    /* fallthrough */
  default:
    bag = BAG_CreateParam( gmData, &wk->icwk, BAG_MODE_FIELD, HEAPID_PROC );
  }

  return bag;
}
//----------------------------------------------------------------------------
/**
 *  @brief  RETURN�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param_adrs  �����̃v���b�N�p�����[�^
 *
 *  @return   RETURNFUNC_RESULT_RETURN,     //���j���[�ɖ߂�
 *            RETURNFUNC_RESULT_EXIT,       //���j���[�������ĕ������Ԃ܂Ŗ߂�
 *            RETURNFUNC_RESULT_USE_SKILL,    //���j���[�𔲂��ċZ���g��
 *            RETURNFUNC_RESULT_USE_ITEM,   //���j���𔲂��ăA�C�e�����g��
 *            RETURNFUNC_RESULT_NEXT,       //���̃v���Z�X�֍s��
 */ 
//-----------------------------------------------------------------------------
static RETURNFUNC_RESULT FMenuReturnProc_Bag(PROCLINK_WORK* wk,void* param_adrs)
{ 
  BAG_PARAM* pBag = param_adrs;
  switch( pBag->next_proc )
  { 
  case BAG_NEXTPROC_EXIT:
    return RETURNFUNC_RESULT_EXIT;

  case BAG_NEXTPROC_RETURN:
    if( pBag->mode == BAG_MODE_POKELIST )
    { 
      wk->next_type = EVENT_PROCLINK_CALL_POKELIST;
      return RETURNFUNC_RESULT_NEXT;
    }
    else
    { 
      return RETURNFUNC_RESULT_RETURN;
    }

  case BAG_NEXTPROC_EVOLUTION:
    wk->next_type = EVENT_PROCLINK_CALL_POKELIST;
    return RETURNFUNC_RESULT_NEXT;

  case BAG_NEXTPROC_RIDECYCLE:
    /* fallthrough */
  case BAG_NEXTPROC_DROPCYCLE:
    wk->param->select_param = EVENT_ITEMUSE_CALL_CYCLE;
    return RETURNFUNC_RESULT_USE_ITEM;

  case BAG_NEXTPROC_PALACEJUMP:
    wk->param->select_param = EVENT_ITEMUSE_CALL_PALACEJUMP;
    return RETURNFUNC_RESULT_USE_ITEM;

  case BAG_NEXTPROC_AMAIMITU:
    wk->param->select_param = EVENT_ITEMUSE_CALL_AMAIMITU;
    return RETURNFUNC_RESULT_USE_ITEM;

  case BAG_NEXTPROC_TURIZAO:
    wk->param->select_param = EVENT_ITEMUSE_CALL_TURIZAO;
    return RETURNFUNC_RESULT_USE_ITEM;

  case BAG_NEXTPROC_ANANUKENOHIMO:
    wk->param->select_param = EVENT_ITEMUSE_CALL_ANANUKENOHIMO;
    return RETURNFUNC_RESULT_USE_ITEM;

  case BAG_NEXTPROC_FRIENDNOTE: 
    wk->next_type = EVENT_PROCLINK_CALL_WIFINOTE;
    return RETURNFUNC_RESULT_NEXT;

  case BAG_NEXTPROC_TOWNMAP:
    wk->next_type = EVENT_PROCLINK_CALL_TOWNMAP;
    return RETURNFUNC_RESULT_NEXT;

  case BAG_NEXTPROC_BATTLERECORDER:
    wk->next_type = EVENT_PROCLINK_CALL_BTLRECORDER;
    return RETURNFUNC_RESULT_NEXT;
  case BAG_NEXTPROC_MAILVIEW:
    wk->next_type = EVENT_PROCLINK_CALL_MAIL;
    return RETURNFUNC_RESULT_NEXT;
    
  default:
    wk->next_type = EVENT_PROCLINK_CALL_POKELIST;
    return RETURNFUNC_RESULT_NEXT;
  }
}
//-------------------------------------
/// �g���[�i�[�J�[�h
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  CALL�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param   �N��������
 *  @param  pre     ��O�̃v���b�N
 *  @param  void* pre_param_adrs  ��O�̃v���b�N�p�����[�^
 *
 *  @return �v���Z�X�p�����[�^
 */
//-----------------------------------------------------------------------------
static void * FMenuCallProc_TrainerCard(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs )
{ 
  TRCARD_CALL_PARAM *tr_param;
  GAMEDATA *gdata;
  
  gdata = GAMESYSTEM_GetGameData( wk->param->gsys );

  tr_param = TRAINERCASR_CreateCallParam_SelfData( gdata, HEAPID_PROC );

  // �\�����̎w���tr_param�Ɋi�[����
  if(param==FALSE || param==EVENT_PROCLINK_DATA_NONE){
    OS_Printf("�g���[�i�[�J�[�h�̕\�Ăяo��\n");
  }else{
    OS_Printf("�g���[�i�[�J�[�h�̗��Ăяo��\n");
  }
  return tr_param;
}
//----------------------------------------------------------------------------
/**
 *  @brief  RETURN�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param_adrs  �����̃v���b�N�p�����[�^
 *
 *  @return   RETURNFUNC_RESULT_RETURN,     //���j���[�ɖ߂�
 *            RETURNFUNC_RESULT_EXIT,       //���j���[�������ĕ������Ԃ܂Ŗ߂�
 *            RETURNFUNC_RESULT_USE_SKILL,    //���j���[�𔲂��ċZ���g��
 *            RETURNFUNC_RESULT_USE_ITEM,   //���j���𔲂��ăA�C�e�����g��
 *            RETURNFUNC_RESULT_NEXT,       //���̃v���Z�X�֍s��
 */ 
//-----------------------------------------------------------------------------
static RETURNFUNC_RESULT FMenuReturnProc_TrainerCard(PROCLINK_WORK* wk,void* param_adrs)
{ 
  TRCARD_CALL_PARAM * tr_param  = param_adrs;
  return RETURNFUNC_RESULT_RETURN;
}
//-------------------------------------
/// �^�E���}�b�v
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  CALL�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param   �N��������
 *  @param  pre     ��O�̃v���b�N
 *  @param  void* pre_param_adrs  ��O�̃v���b�N�p�����[�^
 *
 *  @return �v���Z�X�p�����[�^
 */
//-----------------------------------------------------------------------------
static void * FMenuCallProc_TownMap(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs )
{ 
  TOWNMAP_PARAM*  town_param;
  town_param              = GFL_HEAP_AllocClearMemory( HEAPID_PROC , sizeof(TOWNMAP_PARAM) );
  town_param->mode        = TOWNMAP_MODE_MAP;
  town_param->p_gamesys = wk->param->gsys;


  if( wk->param->call == EVENT_PROCLINK_CALL_POKELIST )
  { 
    //���X�g����Ă΂��Ƃ��́A�����
    town_param->mode        = TOWNMAP_MODE_SKY;
  }
  else
  { 
    //����ȊO�̓^�E���}�b�v
    town_param->mode        = TOWNMAP_MODE_MAP;
  }

  return town_param;
}
//----------------------------------------------------------------------------
/**
 *  @brief  RETURN�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param_adrs  �����̃v���b�N�p�����[�^
 *
 *  @return   RETURNFUNC_RESULT_RETURN,     //���j���[�ɖ߂�
 *            RETURNFUNC_RESULT_EXIT,       //���j���[�������ĕ������Ԃ܂Ŗ߂�
 *            RETURNFUNC_RESULT_USE_SKILL,    //���j���[�𔲂��ċZ���g��
 *            RETURNFUNC_RESULT_USE_ITEM,   //���j���𔲂��ăA�C�e�����g��
 *            RETURNFUNC_RESULT_NEXT,       //���̃v���Z�X�֍s��
 */ 
//-----------------------------------------------------------------------------
static RETURNFUNC_RESULT FMenuReturnProc_TownMap(PROCLINK_WORK* wk,void* param_adrs)
{ 
  const TOWNMAP_PARAM*  param = param_adrs;

  if( wk->param->call == EVENT_PROCLINK_CALL_TOWNMAP )
  { 
    //�^�E���}�b�v�����ڌĂ΂ꂽ
    switch( param->select )
    { 
    case TOWNMAP_SELECT_RETURN: //���������߂�
      return RETURNFUNC_RESULT_RETURN;
    case TOWNMAP_SELECT_CLOSE:    //���������I������
      return RETURNFUNC_RESULT_EXIT;
    default:
      GF_ASSERT(0);
      return RETURNFUNC_RESULT_EXIT;
    }
  }
  else 
  { 
    if( wk->param->call == EVENT_PROCLINK_CALL_BAG )
    { 

      //�o�b�O����Ă΂ꂽ
      switch( param->select )
      { 
      case TOWNMAP_SELECT_RETURN: //���������߂�
        wk->next_type = EVENT_PROCLINK_CALL_BAG;
        return RETURNFUNC_RESULT_NEXT;
      case TOWNMAP_SELECT_CLOSE:    //���������I������
        return RETURNFUNC_RESULT_EXIT;
      default:
        GF_ASSERT(0);
        return RETURNFUNC_RESULT_EXIT;
      }
    }
    else if( wk->param->call == EVENT_PROCLINK_CALL_POKELIST )
    { 
      //���X�g����Ă΂ꂽ
      switch( param->select )
      { 
      case TOWNMAP_SELECT_RETURN: //���������߂�
        wk->next_type = EVENT_PROCLINK_CALL_POKELIST;
        return RETURNFUNC_RESULT_NEXT;
      case TOWNMAP_SELECT_CLOSE:    //���������I������
        return RETURNFUNC_RESULT_EXIT;
      case TOWNMAP_SELECT_SKY:  //�����
        wk->param->select_param = PL_RET_SORAWOTOBU-PL_RET_IAIGIRI;
        wk->param->zoneID       = param->zoneID;
        wk->param->grid_x       = param->grid.x;
        wk->param->grid_y       = param->grid.y;
        wk->param->grid_z       = 0;
        return RETURNFUNC_RESULT_USE_SKILL;
  
      default:
        GF_ASSERT(0);
        return RETURNFUNC_RESULT_EXIT;
      }
    }
  }

  return RETURNFUNC_RESULT_EXIT;
}
//-------------------------------------
/// �R���t�B�O
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  CALL�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param   �N��������
 *  @param  pre     ��O�̃v���b�N
 *  @param  void* pre_param_adrs  ��O�̃v���b�N�p�����[�^
 *
 *  @return �v���Z�X�p�����[�^
 */
//-----------------------------------------------------------------------------
static void * FMenuCallProc_Config(PROCLINK_WORK* wk, u32 param, EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs )
{ 
  CONFIG_PANEL_PARAM  *config_panel;

  GAMEDATA *gameData = GAMESYSTEM_GetGameData( wk->param->gsys );
  SAVE_CONTROL_WORK *saveControl = GAMEDATA_GetSaveControlWork( gameData );
  CONFIG *config = SaveData_GetConfig( saveControl );
  config_panel  = GFL_HEAP_AllocClearMemory(HEAPID_PROC, sizeof(CONFIG_PANEL_PARAM));
  config_panel->p_gamesys = wk->param->gsys;

  return config_panel;
}
//----------------------------------------------------------------------------
/**
 *  @brief  RETURN�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param_adrs  �����̃v���b�N�p�����[�^
 *
 *  @return   RETURNFUNC_RESULT_RETURN,     //���j���[�ɖ߂�
 *            RETURNFUNC_RESULT_EXIT,       //���j���[�������ĕ������Ԃ܂Ŗ߂�
 *            RETURNFUNC_RESULT_USE_SKILL,    //���j���[�𔲂��ċZ���g��
 *            RETURNFUNC_RESULT_USE_ITEM,   //���j���𔲂��ăA�C�e�����g��
 *            RETURNFUNC_RESULT_NEXT,       //���̃v���Z�X�֍s��
 */ 
//-----------------------------------------------------------------------------
static RETURNFUNC_RESULT FMenuReturnProc_Config(PROCLINK_WORK* wk,void* param_adrs)
{ 
  const CONFIG_PANEL_PARAM  *config_panel = param_adrs;
  if( config_panel->is_exit )
  { 
    return RETURNFUNC_RESULT_EXIT;
  }
  else
  { 
    return RETURNFUNC_RESULT_RETURN;
  }
}
//-------------------------------------
/// ���|�[�g
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  EVENT�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param   �N��������
 */
//-----------------------------------------------------------------------------
static void FMenuEvent_Report( PROCLINK_WORK* wk, u32 param )
{ 
  GMEVENT * subevent = createFMenuReportEvent( wk->param->gsys, wk->param->field_wk, wk->heapID,
      FIELDMAP_GetFldMsgBG(wk->param->field_wk ) );
  GMEVENT_CallEvent(wk->event, subevent);

  wk->result  = RETURNFUNC_RESULT_RETURN;
}
//-------------------------------------
/// �F�B�蒠
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  CALL�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param   �N��������
 *  @param  pre     ��O�̃v���b�N
 *  @param  void* pre_param_adrs  ��O�̃v���b�N�p�����[�^
 *
 *  @return �v���Z�X�p�����[�^
 */
//-----------------------------------------------------------------------------
static void * FMenuCallProc_WifiNote(PROCLINK_WORK* wk, u32 param,EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs )
{ 
  WIFINOTE_PROC_PARAM* pFriend;
  
  pFriend = GFL_HEAP_AllocClearMemory( HEAPID_PROC , sizeof(WIFINOTE_PROC_PARAM) );
  pFriend->pGameData = GAMESYSTEM_GetGameData(wk->param->gsys);

  return pFriend;
}
//----------------------------------------------------------------------------
/**
 *  @brief  RETURN�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param_adrs  �����̃v���b�N�p�����[�^
 *
 *  @return   RETURNFUNC_RESULT_RETURN,     //���j���[�ɖ߂�
 *            RETURNFUNC_RESULT_EXIT,       //���j���[�������ĕ������Ԃ܂Ŗ߂�
 *            RETURNFUNC_RESULT_USE_SKILL,    //���j���[�𔲂��ċZ���g��
 *            RETURNFUNC_RESULT_USE_ITEM,   //���j���𔲂��ăA�C�e�����g��
 *            RETURNFUNC_RESULT_NEXT,       //���̃v���Z�X�֍s��
 */ 
//-----------------------------------------------------------------------------
static RETURNFUNC_RESULT FMenuReturnProc_WifiNote(PROCLINK_WORK* wk,void* param_adrs)
{ 
  if( wk->param->call == EVENT_PROCLINK_CALL_WIFINOTE )
  { 
    return RETURNFUNC_RESULT_RETURN;
  }
  else
  { 
    wk->next_type = EVENT_PROCLINK_CALL_BAG;
    return RETURNFUNC_RESULT_NEXT;
  }
}
//-------------------------------------
/// ���[�����
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  CALL�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param   �N��������
 *  @param  pre     ��O�̃v���b�N
 *  @param  void* pre_param_adrs  ��O�̃v���b�N�p�����[�^
 *
 *  @return �v���Z�X�p�����[�^
 */
//-----------------------------------------------------------------------------
static void * FMenuCallProc_Mail(PROCLINK_WORK* wk, u32 param,EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs )
{ 
  GAMEDATA *gmData = GAMESYSTEM_GetGameData( wk->param->gsys );
  MAIL_PARAM *mailParam;
  
  GFL_OVERLAY_Load( FS_OVERLAY_ID(app_mail));

  if( pre == EVENT_PROCLINK_CALL_POKELIST )
  {
    //�o�b�O(���[���I��)�����X�g�����[�����
    const PLIST_DATA *plistData = pre_param_adrs;
    const POKEPARTY *party = GAMEDATA_GetMyPokemon( gmData );
    wk->sel_poke = plistData->ret_sel;
    wk->item_no = plistData->item;
    if( plistData->ret_mode == PL_RET_MAILSET )
    {
      wk->mode = PROCLINK_MODE_LIST_TO_MAIL_CREATE;
      mailParam = MAILSYS_GetWorkCreate( gmData, 
                                         MAILBLOCK_TEMOTI, 
                                         plistData->ret_sel,
                                         ITEM_GetMailDesign( plistData->item ) ,
                                         HEAPID_PROC );
    }
    else
    {
      wk->mode = PROCLINK_MODE_LIST_TO_MAIL_VIEW;
      OS_TPrintf("MailPos[%d]\n",plistData->ret_sel);
      mailParam = MailSys_GetWorkViewPoke( gmData, PokeParty_GetMemberPointer(party,plistData->ret_sel), HEAPID_PROC );
    }

  }else{
    wk->mode = PROCLINK_MODE_LIST_TO_MAIL_VIEW;
    OS_TPrintf("���[���f�U�C�����邾��\n");
    mailParam = MailSys_GetWorkViewPrev( gmData, wk->item_no-ITEM_GURASUMEERU, HEAPID_PROC );
  }

  return mailParam;
}
//----------------------------------------------------------------------------
/**
 *  @brief  RETURN�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param_adrs  �����̃v���b�N�p�����[�^
 *
 *  @return   RETURNFUNC_RESULT_RETURN,     //���j���[�ɖ߂�
 *            RETURNFUNC_RESULT_EXIT,       //���j���[�������ĕ������Ԃ܂Ŗ߂�
 *            RETURNFUNC_RESULT_USE_SKILL,    //���j���[�𔲂��ċZ���g��
 *            RETURNFUNC_RESULT_USE_ITEM,   //���j���𔲂��ăA�C�e�����g��
 *            RETURNFUNC_RESULT_NEXT,       //���̃v���Z�X�֍s��
 */ 
//-----------------------------------------------------------------------------
static RETURNFUNC_RESULT FMenuReturnProc_Mail(PROCLINK_WORK* wk,void* param_adrs)
{ 
  GAMEDATA *gmData = GAMESYSTEM_GetGameData( wk->param->gsys );
  MAIL_PARAM *mailParam = (MAIL_PARAM*)param_adrs;
  
  if( wk->pre_type == EVENT_PROCLINK_CALL_POKELIST )
  { 
    if( wk->mode == PROCLINK_MODE_LIST_TO_MAIL_CREATE )
    {
      const BOOL isCreate = MailSys_IsDataCreate( mailParam );
      if( isCreate == TRUE )
      {
        const POKEPARTY *party = GAMEDATA_GetMyPokemon( gmData );
        MailSys_PushDataToSavePoke( mailParam , PokeParty_GetMemberPointer(party,wk->sel_poke) );
        wk->next_type = EVENT_PROCLINK_CALL_POKELIST;
      }
      else
      {
        wk->next_type = EVENT_PROCLINK_CALL_BAG;
      }
    }
    else
    {
      wk->next_type = EVENT_PROCLINK_CALL_POKELIST;
    }
    // ���[�N���
//    MailSys_ReleaseCallWork( mailParam);

    //�I�[�o�[���C�J���I
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(app_mail));
    return RETURNFUNC_RESULT_NEXT;
  }

  // ���[�N���
//  MailSys_ReleaseCallWork( mailParam);
  wk->next_type = EVENT_PROCLINK_CALL_BAG;
  //�I�[�o�[���C�J���I�I�I
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(app_mail));
  return RETURNFUNC_RESULT_RETURN;
}

//-------------------------------------
/// �i�����
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  CALL�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param   �N��������
 *  @param  pre     ��O�̃v���b�N
 *  @param  void* pre_param_adrs  ��O�̃v���b�N�p�����[�^
 *
 *  @return �v���Z�X�p�����[�^
 */
//-----------------------------------------------------------------------------
static void * FMenuCallProc_Evolution(PROCLINK_WORK* wk, u32 param,EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs )
{ 
  GAMEDATA *gmData = GAMESYSTEM_GetGameData( wk->param->gsys );
  SHINKA_DEMO_PARAM *demoParam = NULL;

  GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo));
  if( pre == EVENT_PROCLINK_CALL_POKELIST )
  {
    const PLIST_DATA *plistData = pre_param_adrs;

    POKEPARTY *party = GAMEDATA_GetMyPokemon( gmData );
    SHINKA_TYPE type;
    SHINKA_COND cond;
    u16 newMonsNo;
    if( plistData->ret_mode == PL_RET_ITEMSHINKA )
    {
      type = SHINKA_TYPE_ITEM_CHECK;
      wk->mode = PROCLINK_MODE_EVOLUTION_ITEM;
    }
    else
    {
      type = SHINKA_TYPE_LEVELUP;
      wk->mode = PROCLINK_MODE_EVOLUTION_LEVEL;
    }
    newMonsNo = SHINKA_Check( party , 
                              PokeParty_GetMemberPointer(party,plistData->ret_sel) , 
                              type , 
                              plistData->item , 
                              &cond , 
                              HEAPID_PROC );

    demoParam = SHINKADEMO_AllocParam(HEAPID_PROC ,
                                      party ,
                                      newMonsNo ,
                                      plistData->ret_sel ,
                                      cond );

  }
  return demoParam;
}
//----------------------------------------------------------------------------
/**
 *  @brief  RETURN�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param_adrs  �����̃v���b�N�p�����[�^
 *
 *  @return   RETURNFUNC_RESULT_RETURN,     //���j���[�ɖ߂�
 *            RETURNFUNC_RESULT_EXIT,       //���j���[�������ĕ������Ԃ܂Ŗ߂�
 *            RETURNFUNC_RESULT_USE_SKILL,    //���j���[�𔲂��ċZ���g��
 *            RETURNFUNC_RESULT_USE_ITEM,   //���j���𔲂��ăA�C�e�����g��
 *            RETURNFUNC_RESULT_NEXT,       //���̃v���Z�X�֍s��
 */ 
//-----------------------------------------------------------------------------
static RETURNFUNC_RESULT FMenuReturnProc_Evolution(PROCLINK_WORK* wk,void* param_adrs)
{ 
  GAMEDATA *gmData = GAMESYSTEM_GetGameData( wk->param->gsys );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );
  
  if( wk->mode == PROCLINK_MODE_EVOLUTION_ITEM )
  {
    wk->next_type = EVENT_PROCLINK_CALL_BAG;
    return RETURNFUNC_RESULT_NEXT;
  }
  else
  { 
    return RETURNFUNC_RESULT_RETURN;
  }
}

//-------------------------------------
/// �o�g�����R�[�_�[���
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  CALL�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param   �N��������
 *  @param  pre     ��O�̃v���b�N
 *  @param  void* pre_param_adrs  ��O�̃v���b�N�p�����[�^
 *
 *  @return �v���Z�X�p�����[�^
 */
//-----------------------------------------------------------------------------
static void * FMenuCallProc_BattleRecorder(PROCLINK_WORK* wk, u32 param,EVENT_PROCLINK_CALL_TYPE pre, const void* pre_param_adrs )
{ 
  BATTLERECORDER_PARAM* p_btlrecorder_param;
  
  p_btlrecorder_param = GFL_HEAP_AllocClearMemory( HEAPID_PROC , sizeof(BATTLERECORDER_PARAM) );
  p_btlrecorder_param->p_gamedata = GAMESYSTEM_GetGameData(wk->param->gsys);

  return p_btlrecorder_param;
}
//----------------------------------------------------------------------------
/**
 *  @brief  RETURN�֐�
 *
 *  @param  wk      ���C�����[�N
 *  @param  param_adrs  �����̃v���b�N�p�����[�^
 *
 *  @return   RETURNFUNC_RESULT_RETURN,     //���j���[�ɖ߂�
 *            RETURNFUNC_RESULT_EXIT,       //���j���[�������ĕ������Ԃ܂Ŗ߂�
 *            RETURNFUNC_RESULT_USE_SKILL,    //���j���[�𔲂��ċZ���g��
 *            RETURNFUNC_RESULT_USE_ITEM,   //���j���𔲂��ăA�C�e�����g��
 *            RETURNFUNC_RESULT_NEXT,       //���̃v���Z�X�֍s��
 */ 
//-----------------------------------------------------------------------------
static RETURNFUNC_RESULT FMenuReturnProc_BattleRecorder(PROCLINK_WORK* wk,void* param_adrs)
{ 
  if( wk->param->call == EVENT_PROCLINK_CALL_BTLRECORDER )
  { 
    return RETURNFUNC_RESULT_RETURN;
  }
  else
  {
    wk->next_type = EVENT_PROCLINK_CALL_BAG;
    return RETURNFUNC_RESULT_NEXT;
  }
}

//=============================================================================
/**
 *  ���|�[�g�Ɛ}�ӂ̃C�x���g
 */
//=============================================================================
//======================================================================
//  ���b�Z�[�W�E�B���h�E�\���C�x���g
//======================================================================
//--------------------------------------------------------------
/// FMENU_MSGWIN_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  u32 heapID;
  u32 strID;
  FLDMSGBG *msgBG;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
}FMENU_MSGWIN_EVENT_WORK;

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * createFMenuMsgWinEvent(
  GAMESYS_WORK *gsys, u32 heapID, u32 strID, FLDMSGBG *msgBG )
{
  GMEVENT * msgEvent;
  FMENU_MSGWIN_EVENT_WORK *work;
  
  msgEvent = GMEVENT_Create(
    gsys, NULL, FMenuMsgWinEvent, sizeof(FMENU_MSGWIN_EVENT_WORK));
  work = GMEVENT_GetEventWork( msgEvent );
  MI_CpuClear8( work, sizeof(FMENU_MSGWIN_EVENT_WORK) );
  work->msgBG = msgBG;
  work->heapID = heapID;
  work->strID = strID;
  return msgEvent;
}

//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT_RESULT FMenuMsgWinEvent( GMEVENT *event, int *seq, void *wk )
{
  FMENU_MSGWIN_EVENT_WORK *work = wk;
  
  switch( (*seq) ){
  case 0:
    work->msgData = FLDMSGBG_CreateMSGDATA(
      work->msgBG, NARC_message_fldmapmenu_dat );
    work->msgWin = FLDMSGWIN_AddTalkWin( work->msgBG, work->msgData );
    FLDMSGWIN_Print( work->msgWin, 0, 0, work->strID );
    GXS_SetMasterBrightness(-16);
    (*seq)++;
    break;
  case 1:
    if( FLDMSGWIN_CheckPrintTrans(work->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 2:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
        (*seq)++;
      }
    }
    break;
  case 3:
    FLDMSGWIN_Delete( work->msgWin );
    GFL_MSG_Delete( work->msgData );
    GXS_SetMasterBrightness(0);
/*    {
      GAMESYS_WORK *gameSys = GMEVENT_GetGameSysWork( event );
      GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
      FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( gameSys );
       
      const FIELD_MENU_ITEM_TYPE type = GAMEDATA_GetSubScreenType( gameData );
      FIELD_SUBSCREEN_SetTopMenuItemNo( FIELDMAP_GetFieldSubscreenWork(fieldWork) , type );
    }
 */   return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  ���|�[�g�\���C�x���g
//======================================================================
//--------------------------------------------------------------
/// FMENU_REPORT_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  u32 heapID;
  FLDMSGBG *msgBG;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  GAMESYS_WORK *gsys;
  FIELDMAP_WORK *fieldWork;
}FMENU_REPORT_EVENT_WORK;

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * createFMenuReportEvent(
  GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, u32 heapID, FLDMSGBG *msgBG )
{
  GMEVENT * msgEvent;
  FMENU_REPORT_EVENT_WORK *work;

  msgEvent = GMEVENT_Create(
    gsys, NULL, FMenuReportEvent, sizeof(FMENU_REPORT_EVENT_WORK));
  work = GMEVENT_GetEventWork( msgEvent );
  MI_CpuClear8( work, sizeof(FMENU_REPORT_EVENT_WORK) );
  work->msgBG = msgBG;
  work->heapID = heapID;
  work->gsys = gsys;
  work->fieldWork = fieldWork;
  return msgEvent;
}

//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT_RESULT FMenuReportEvent( GMEVENT *event, int *seq, void *wk )
{
  FMENU_REPORT_EVENT_WORK *work = wk;
  
  switch( (*seq) ){
  case 0:
    {
      GAME_COMM_SYS_PTR commSys = GAMESYSTEM_GetGameCommSysPtr( work->gsys );
      work->msgData = FLDMSGBG_CreateMSGDATA(
             work->msgBG, NARC_message_fldmapmenu_dat );
      
      work->msgWin = FLDMSGWIN_AddTalkWin( work->msgBG, work->msgData );
      

      if( GameCommSys_BootCheck(commSys) == GAME_COMM_NO_INVASION )
      {
        //�ʐM���Ń��|�[�g�������Ȃ�
        FLDMSGWIN_Print( work->msgWin, 0, 0, FLDMAPMENU_STR16 );
        GXS_SetMasterBrightness(-16);
        (*seq) = 20;
      }
      else
      {
        { //���@���|�[�g��
          FIELD_PLAYER *fld_player =
            FIELDMAP_GetFieldPlayer( work->fieldWork );
          
          if( FIELD_PLAYER_CheckChangeEventDrawForm(fld_player) == TRUE ){
            MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
            FIELD_PLAYER_GRID *gjiki = FIELDMAP_GetPlayerGrid( work->fieldWork );

            FIELD_PLAYER_GRID_SetRequest( gjiki, FIELD_PLAYER_REQBIT_REPORT );
            FIELD_PLAYER_GRID_UpdateRequest( gjiki );
  
            //�|�[�Y���������A�j������悤��
            MMDL_OffStatusBitMoveProcPause( mmdl );
            MMDL_OffStatusBit( mmdl, MMDL_STABIT_PAUSE_ANM );
          }
        }

        FLDMSGWIN_Print( work->msgWin, 0, 0, FLDMAPMENU_STR14 );
        //�{���Ȃ烌�|�[�g�p�ւ̐؂�ւ��H
        //FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork), FIELD_SUBSCREEN_TOPMENU);
        GXS_SetMasterBrightness(-16);
        (*seq)++;
      }
    }
    break;
  case 1:
    if( FLDMSGWIN_CheckPrintTrans(work->msgWin) == TRUE ){
      GAMEDATA_SaveAsyncStart(GAMESYSTEM_GetGameData(work->gsys));
      (*seq)++;
    } 
    break;
  case 2:
    {
      SAVE_RESULT result;
      result = GAMEDATA_SaveAsyncMain(GAMESYSTEM_GetGameData(work->gsys));
      switch(result){
      case SAVE_RESULT_CONTINUE:
      case SAVE_RESULT_LAST:
        break;
      case SAVE_RESULT_OK:
      case SAVE_RESULT_NG:
        (*seq)++;
        break;
      }
    }
    break;
  case 3:
    FLDMSGWIN_Delete( work->msgWin );
    
    { //���@���ɖ߂�
      FIELD_PLAYER *fld_player =
        FIELDMAP_GetFieldPlayer( work->fieldWork );

      if( FIELD_PLAYER_CheckChangeEventDrawForm(fld_player) == TRUE ){
        FIELDMAP_CTRL_GRID *gridmap =
          FIELDMAP_GetMapCtrlWork( work->fieldWork );
        FIELD_PLAYER_GRID *gjiki = FIELDMAP_GetPlayerGrid( work->fieldWork );
        FIELD_PLAYER_GRID_SetRequest(
            gjiki, FIELD_PLAYER_REQBIT_MOVE_FORM_TO_DRAW_FORM );
        FIELD_PLAYER_GRID_UpdateRequest( gjiki );
      }
    }

    PMSND_PlaySE( SEQ_SE_SAVE );
    work->msgWin = FLDMSGWIN_AddTalkWin( work->msgBG, work->msgData );
    FLDMSGWIN_Print( work->msgWin, 0, 0, FLDMAPMENU_STR15 );
    (*seq)++;
    break;
  case 4:
    if( FLDMSGWIN_CheckPrintTrans(work->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 5:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
        (*seq)++;
      }
    }
    break;
  case 6:
    FLDMSGWIN_Delete( work->msgWin );
    GFL_MSG_Delete( work->msgData );

    //�{���Ȃ烁�j���[�ւ̐؂�ւ��H
    //FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork), FIELD_SUBSCREEN_TOPMENU);
    GXS_SetMasterBrightness(0);
/*    {
      GAMESYS_WORK *gameSys = GMEVENT_GetGameSysWork( event );
      GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
      FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( gameSys );
       
      const FIELD_MENU_ITEM_TYPE type = GAMEDATA_GetSubScreenType( gameData );
      FIELD_SUBSCREEN_SetTopMenuItemNo( FIELDMAP_GetFieldSubscreenWork(fieldWork) , type );
    }
*/    return( GMEVENT_RES_FINISH );
    break;
    
    //�Z�[�u�ł��܂���
  case 20:
    if( FLDMSGWIN_CheckPrintTrans(work->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 21:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
        (*seq)++;
      }
    }
    break;
  case 22:
    FLDMSGWIN_Delete( work->msgWin );
    GFL_MSG_Delete( work->msgData );
    GXS_SetMasterBrightness(0);
/*    {
      GAMESYS_WORK *gameSys = GMEVENT_GetGameSysWork( event );
      GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
      FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( gameSys );
      
      const FIELD_MENU_ITEM_TYPE type = GAMEDATA_GetSubScreenType( gameData );
      FIELD_SUBSCREEN_SetTopMenuItemNo( FIELDMAP_GetFieldSubscreenWork(fieldWork) , type );

    }
*/    return( GMEVENT_RES_FINISH );
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}


