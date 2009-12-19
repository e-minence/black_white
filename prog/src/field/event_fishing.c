/**
 *  @file   event_fishing.c
 *  @brief  �ނ�C�x���g
 *  @author Miyuki Iwasawa
 *  @datge  09.12.04
 */

#include <gflib.h>
#include "system/gfl_use.h"

#include "system/main.h"  //HEAPID_PROC
#include "fieldmap.h"
#include "weather.h"

#include "field_skill.h"
#include "eventwork.h"

#include "script.h"
#include "encount_data.h"
#include "field_encount.h"
#include "effect_encount.h"
#include "fldmmdl.h"
#include "fldeff_gyoe.h"
#include "fldeff_fishing.h"

#include "event_fishing.h"

#include "../../../resource/fldmapdata/script/field_ev_scr_def.h" // for SCRID_FLE_EV_AMAIKAORI_ENC_FAILED

typedef enum{
 SEQ_FISHING_START,
 SEQ_START_WAIT,
 SEQ_HIT_WAIT,
 SEQ_HIT,
 SEQ_HIT_SUCCESS,
 SEQ_ENCOUNT_FAILED,
 SEQ_TOO_EARLY,
 SEQ_TOO_SLOW,
 SEQ_END,
}AMAIKAORI_SEQ;

enum{
  CODE_NONE,
  CODE_HIT,
  CODE_END,
};

#define TIME_START_WAIT (15)
#define TIME_ENCOUNT_FAILED_WAIT (30*4)
#define TIME_HIT_WAIT_MIN (30)
#define TIME_HIT_WAIT_MAX (120-TIME_HIT_WAIT_MIN)
#define TIME_HIT_COUNT (30)

typedef struct _FISHING_WORK{
  GAMESYS_WORK* gsys;
  GAMEDATA* gdata;
  FIELDMAP_WORK *fieldWork;
  FIELD_ENCOUNT* fld_enc;
  const FLDMAPPER* g3dMapper;
  MMDLSYS* mmdl_sys;

  FIELD_PLAYER *fplayer;
  MMDL* player_mmdl;
  MMDL_GRIDPOS  pos;

  FLDEFF_TASK*  task_gyoe;
  FLDEFF_TASK*  task_lure;

  VecFx32 player_pos;
  VecFx32 target_pos;

  u16     enc_type;
  u16     enc_error;
  u32     time;
  u32     count;
  GMEVENT* enc_event;
}FISHING_WORK;

static GMEVENT_RESULT FieldFishingEvent(GMEVENT * event, int * seq, void *work);
static BOOL sub_TimeWait( FISHING_WORK* wk, u32 time );
static int sub_TimeKeyWait( FISHING_WORK* wk, u32 time );
static void sub_SetGyoeAnime( FISHING_WORK* wk );
static void sub_DelGyoeAnime( FISHING_WORK* wk );
static void sub_SetLureAnime( FISHING_WORK* wk );
static void sub_DelLureAnime( FISHING_WORK* wk );

static const VecFx32 DATA_FishingFormOfs[] = {
 { FX32_CONST(0),0,FX32_CONST(0) }, //up
 { FX32_CONST(0),0,FX32_CONST(0) }, //down
 { FX32_CONST(-0),0,FX32_CONST(0) }, //left
 { FX32_CONST(0),0,FX32_CONST(0) }, //right
};

/*
 *  @brief  �ނ肪�ł���|�W�V�������`�F�b�N
 */
static BOOL FieldFishingCheckPos( GAMEDATA* gdata, FIELDMAP_WORK* fieldmap, VecFx32* outPos )
{
  u8 dir;
  VecFx32 pos;
  MMDL* mmdl;
  FIELD_PLAYER* player;
  FLDMAPPER_GRIDINFODATA gridData;

  FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( fieldmap );

  player = FIELDMAP_GetFieldPlayer( fieldmap );
  mmdl = FIELD_PLAYER_GetMMdl( player );
  dir = MMDL_GetDirDisp( mmdl );

  FIELD_PLAYER_GetDirPos( player, dir, &pos );
  if( FLDMAPPER_GetGridData( mapper, &pos, &gridData) == FALSE){
    return FALSE;
  }
  //�݃`�F�b�N
  if( MAPATTR_VALUE_CheckShore( MAPATTR_GetAttrValue( gridData.attr ) ) == TRUE ){ //��
    MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 ); //���������
    if( FLDMAPPER_GetGridData( mapper, &pos, &gridData) == FALSE){
      return FALSE;
    }
  }
  //�A�g���r���[�g�`�F�b�N
  if( !MAPATTR_VALUE_CheckWaterTypeSeason(
        MAPATTR_GetAttrValue( gridData.attr ),GAMEDATA_GetSeasonID( gdata ))){
    return FALSE;
  }
  outPos->x = pos.x;
  outPos->y = gridData.height;
  outPos->z = pos.z;

  return TRUE;
}


//------------------------------------------------------------------
/*
 *  @brief  �ނ�C�x���g�N��
 */
//------------------------------------------------------------------
GMEVENT * EVENT_FieldFishing( FIELDMAP_WORK* fieldmap, GAMESYS_WORK* gsys )
{
	GMEVENT * event = GMEVENT_Create( gsys, NULL, FieldFishingEvent, sizeof(FISHING_WORK));
	FISHING_WORK * wk = GMEVENT_GetEventWork(event);
  
  MI_CpuClear8(wk,sizeof(FISHING_WORK));
  wk->gsys = gsys;
  wk->fieldWork = fieldmap;
  wk->gdata = GAMESYSTEM_GetGameData(gsys);
  wk->fld_enc = FIELDMAP_GetEncount( wk->fieldWork );

  wk->g3dMapper = (const FLDMAPPER*)FIELDMAP_GetFieldG3Dmapper( wk->fieldWork ); 
  wk->mmdl_sys = FIELDMAP_GetMMdlSys( wk->fieldWork );
  wk->fplayer = FIELDMAP_GetFieldPlayer( fieldmap );
  wk->player_mmdl = FIELD_PLAYER_GetMMdl( wk->fplayer );

  FIELD_PLAYER_GetPos( wk->fplayer, &wk->player_pos);

  //�|�W�V�����`�F�b�N
  if( FieldFishingCheckPos( wk->gdata, wk->fieldWork, &wk->target_pos ) == FALSE){
    wk->enc_error = TRUE;
  }
  wk->pos.gx = SIZE_GRID_FX32(wk->target_pos.x);
  wk->pos.gy = SIZE_GRID_FX32(wk->target_pos.y);
  wk->pos.gz = SIZE_GRID_FX32(wk->target_pos.z);

  //�G�t�F�N�g�G���J�E���g�`�F�b�N
  if( EFFECT_ENC_CheckEffectPos( wk->fld_enc, &wk->pos )){
    wk->enc_type = ENC_TYPE_EFFECT;
    wk->enc_event = FIELD_ENCOUNT_CheckFishingEncount( wk->fld_enc, ENC_TYPE_EFFECT );
  }else{
    wk->enc_type = ENC_TYPE_NORMAL;
    wk->enc_event = FIELD_ENCOUNT_CheckFishingEncount( wk->fld_enc, ENC_TYPE_NORMAL );
  }

  IWASAWA_Printf("PlayerPos ( 0x%08x, 0x%08x, 0x%08x)\n", wk->player_pos.x, wk->player_pos.y, wk->player_pos.z );
  IWASAWA_Printf("TargetPos ( 0x%08x, 0x%08x, 0x%08x)\n", wk->target_pos.x, wk->target_pos.y, wk->target_pos.z );
  return event;
}

static GMEVENT_RESULT FieldFishingEvent(GMEVENT * event, int * seq, void *work)
{
  int ret;
	FISHING_WORK * wk = work;
	
  switch (*seq) {
  case SEQ_FISHING_START:
    if(wk->enc_error){ //������
      *seq = SEQ_END;
      break;
    }
    MMDLSYS_PauseMoveProc( wk->mmdl_sys );

    //�G���J�E���g�`�F�b�N
    wk->enc_event = FIELD_ENCOUNT_CheckFishingEncount( wk->fld_enc, wk->enc_type );
    //�t�H�����`�F���W
    FIELD_PLAYER_ChangeDrawForm( wk->fplayer, PLAYER_DRAW_FORM_FISHING );
    MMDL_SetDrawStatus( wk->player_mmdl, DRAW_STA_FISH_START );
    MMDL_SetVectorDrawOffsetPos( wk->player_mmdl, &DATA_FishingFormOfs[MMDL_GetDirDisp( wk->player_mmdl )]);
    (*seq)++;
    break;

  case SEQ_START_WAIT:
    if( !sub_TimeWait(wk,TIME_START_WAIT )){
      break;
    }
    //���A�[�Z�b�g
    sub_SetLureAnime(wk);

    if( wk->enc_event == NULL ){  //�G���J�E�g���s
      *seq = SEQ_ENCOUNT_FAILED;
      break;
    }
    wk->time = GFUser_GetPublicRand0(TIME_HIT_WAIT_MAX)+TIME_HIT_WAIT_MIN;
    (*seq)++;
    break;
  
  //������҂�
  case SEQ_HIT_WAIT:
    ret = sub_TimeKeyWait(wk, wk->time );
    if( ret == CODE_HIT ){
      *seq = SEQ_TOO_EARLY;
    }else if( ret == CODE_END ){
      sub_SetGyoeAnime( wk );
      (*seq)++;
    }
    break;

  //�ނ�グ�`�F�b�N
  case SEQ_HIT:
    ret = sub_TimeKeyWait(wk, TIME_HIT_COUNT );
    if( ret == CODE_NONE){
      break;
    }
    sub_DelGyoeAnime( wk );
    if( ret == CODE_HIT){
      sub_DelLureAnime( wk );
      MMDL_SetDrawStatus( wk->player_mmdl, DRAW_STA_FISH_HIT );
      SCRIPT_CallScript( event,SCRID_FLD_EV_FISHING_SUCCESS, NULL, NULL, HEAPID_FIELDMAP );
      *seq = SEQ_HIT_SUCCESS;
    }else if( ret == CODE_END){
      *seq = SEQ_TOO_SLOW;
    }
    break;

  case SEQ_HIT_SUCCESS:
    GMEVENT_CallEvent( event, wk->enc_event );
		(*seq) = SEQ_END;
		break;

  //�G���J�E���g���s
  case SEQ_ENCOUNT_FAILED:
    if( !sub_TimeWait(wk,TIME_ENCOUNT_FAILED_WAIT )){
      break;
    }
    //�u���C�N�X���[
  case SEQ_TOO_EARLY:
  case SEQ_TOO_SLOW:
    sub_DelLureAnime( wk );
    MMDL_SetDrawStatus( wk->player_mmdl, DRAW_STA_FISH_END );
    SCRIPT_CallScript( event,SCRID_FLD_EV_FISHING_FAILED_ENCOUNT+(*seq-SEQ_ENCOUNT_FAILED), NULL, NULL, HEAPID_FIELDMAP );
		(*seq) = SEQ_END;
    break;

	case SEQ_END:
    FIELD_PLAYER_ChangeDrawForm( wk->fplayer, PLAYER_DRAW_FORM_NORMAL );
    MMDL_SetVectorDrawOffsetPos( wk->player_mmdl, &DATA_FishingFormOfs[0]);
    MMDLSYS_ClearPauseMoveProc( wk->mmdl_sys );
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}

static BOOL sub_TimeWait( FISHING_WORK* wk, u32 time )
{
  if(wk->count++ < time){
    return FALSE;
  }
  wk->count = 0;
  return TRUE;
}

static int sub_TimeKeyWait( FISHING_WORK* wk, u32 time )
{
  if(wk->count++ < time){
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
      wk->count = 0;
      return CODE_HIT;
    }
  }else{
    wk->count = 0;
    return CODE_END;
  }
  return CODE_NONE;
}

/*
 *  @brief  �r�b�N���}�[�N�A�j��
 */
static void sub_SetGyoeAnime( FISHING_WORK* wk )
{
  FLDEFF_CTRL *fectrl =  FIELDMAP_GetFldEffCtrl( wk->fieldWork );
  
  MMDL_SetDrawStatus( wk->player_mmdl, DRAW_STA_FISH_ON );
  wk->task_gyoe = FLDEFF_GYOE_SetMMdl(
      fectrl, wk->player_mmdl, FLDEFF_GYOETYPE_GYOE, TRUE );

  FLDEFF_FISHING_LURE_ChangeAnime( wk->task_lure, FISHING_LURE_ANM_HIT );
}

/*
 *  @brief  �r�b�N���}�[�N�A�j���j��
 */
static void sub_DelGyoeAnime( FISHING_WORK* wk )
{
  if(wk->task_gyoe != NULL){
    FLDEFF_TASK_CallDelete( wk->task_gyoe );
    wk->task_gyoe = NULL;
  }
}

/*
 *  @brief  ���A�[�A�j��
 */
static void sub_SetLureAnime( FISHING_WORK* wk )
{
  FLDEFF_CTRL *fectrl =  FIELDMAP_GetFldEffCtrl( wk->fieldWork );
  wk->task_lure = FLDEFF_FISHING_LURE_Set( fectrl, &wk->target_pos );
}

/*
 *  @brief  �r�b�N���}�[�N�A�j���j��
 */
static void sub_DelLureAnime( FISHING_WORK* wk )
{
  if(wk->task_lure != NULL){
    FLDEFF_TASK_CallDelete( wk->task_lure );
    wk->task_lure = NULL;
  }
}

