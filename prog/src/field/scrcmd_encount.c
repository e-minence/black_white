//======================================================================
/*
 *  @file scrcmd_encount.c
 *  @brief  �X�N���v�g�@�쐶��G���J�E���g��
 *  @author Miyuki Iwasawa
 *  @date   09.10.28
 */
//======================================================================

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"
#include "system/main.h"  //HEAPID_PROC

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"
#include "event_battle.h"

#include "move_pokemon.h"
#include "field/zonedata.h"
#include "arc/fieldmap/zone_id.h"

#include "scrcmd_encount.h"
#include "effect_encount.h"
#include "encount_data.h"

#include "event_field_fade.h" //EVENT_FieldFadeIn_Black


//======================================================================
//======================================================================

//--------------------------------------------------------------
/**
 * �쐶�퓬�Ăяo��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdWildBattleSet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	
	u16 mons_no = SCRCMD_GetVMWorkValue( core, work );
	u16 mons_lv = SCRCMD_GetVMWorkValue( core, work );
  u16 flags = SCRCMD_GetVMWorkValue( core, work );
  
  {
    GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
    SCRIPT_FLDPARAM * fparam = SCRIPT_GetFieldParam( sc );
    GMEVENT *ev_battle =
      (GMEVENT*)FIELD_ENCOUNT_SetWildEncount(
          FIELDMAP_GetEncount( fparam->fieldMap ), mons_no, mons_lv, flags );
    SCRIPT_CallEvent( sc, ev_battle );
  }
	return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �쐶�폟��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	VMCMD_RESULT_SUSPEND
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdWildWin( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK * gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  GMEVENT* fade_event;
  fade_event = EVENT_FieldFadeIn_Black(gsys, fieldmap, FIELD_FADE_WAIT);
  SCRIPT_CallEvent( sc, fade_event );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �쐶�퓬 �Đ�R�[�h�`�F�b�N
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdWildBattleRetryCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  BtlResult res = GAMEDATA_GetLastBattleResult(gdata);
	
	u16* ret_wk = SCRCMD_GetVMWork( core, work );
  *ret_wk = FIELD_BATTLE_GetWildBattleRevengeCode( res );
	return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �ߊl�f���Ăяo��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCaptureDemoBattleSet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fwork = GAMESYSTEM_GetFieldMapWork( gsys );
	
  {
    GMEVENT *ev_battle =
      EVENT_CaptureDemoBattle( gsys, fwork, HEAPID_PROC );
    SCRIPT_CallEvent( sc, ev_battle );
  }
	return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------------
/**
 * @brief   �G�t�F�N�g�G���J�E�g�A�C�e���擾�C�x���g�Ŏ�ɓ����ItemNo���擾����
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 *
 * �R�}���h�̖߂�l game_comm.h GAME_COMM_NO_NULL ��
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdGetEffectEncountItem( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*      work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fwork = GAMESYSTEM_GetFieldMapWork( gsys );
 
  u16*  ret_work = SCRCMD_GetVMWork( core, work );
  *ret_work = EFFECT_ENC_GetEffectEncountItem( FIELDMAP_GetEncount(fwork) );
  IWASAWA_Printf("GetEffectEncountItem %d\n",*ret_work);
	return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief �ړ��|�P�����@�ړ��J�n 
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 *
 * �R�}���h�̖߂�l game_comm.h GAME_COMM_NO_NULL ��
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdAddMovePokemon( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*      work = (SCRCMD_WORK*)wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
	ENC_SV_PTR enc_sv = EncDataSave_GetSaveDataPtr( GAMEDATA_GetSaveControlWork( gdata ) );
	
  u8 move_poke = (u8)SCRCMD_GetVMWorkValue( core, work );
	
  if ( EncDataSave_IsMovePokeValid( enc_sv, move_poke ) == FALSE ){
    MP_AddMovePoke( gdata, move_poke ); 
  }

#ifdef PM_DEBUG
  {
    u16 zone_id;
    char buf[ZONEDATA_NAME_LENGTH];
    MPD_PTR mpd = EncDataSave_GetMovePokeDataPtr( enc_sv, move_poke );
    zone_id = EncDataSave_GetMovePokeDataParam(mpd, MP_PARAM_ZONE_ID );
    if( zone_id != MVPOKE_ZONE_NULL){
      ZONEDATA_DEBUG_GetZoneName(buf, zone_id);
      OS_Printf( "move_poke %d\n->ZONE:%s\n", move_poke, buf );
    }else{
      OS_Printf( "move_poke %d\n->ZONE:�B��Ă���\n", move_poke, buf );
    }
    OS_Printf("->HP�F%d\n",EncDataSave_GetMovePokeDataParam(mpd,MP_PARAM_HP));
  }
#endif
	return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief �ނ�G���J�E���g����|�P����No�𒊑I�Ŏ擾���� 
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 *
 * MVPOKE_STATE_NONE��
 */
//--------------------------------------------------------------------
#define FISHING_ENC_POKE_ZONE_MAX  (5)
#define FISHING_ENC_POKE_MAX  (FISHING_ENC_POKE_ZONE_MAX*(ENC_MONS_NUM_FISHING+ENC_MONS_NUM_FISHING_SP))

/*
 *  @brief  �w��̃����X�^�[No�����ɓo�^�ς݂��`�F�b�N
 */
static u16 sub_EntryFishingEncMonsNo( u16* ary, u16 num, u16 monsno)
{
  int i;

  for(i = 0;i < num;i++){
    if( ary[i] == monsno ){
      return 0;
    }
  }
  ary[num] = monsno;
  
  return (num+1);
}

VMCMD_RESULT EvCmdGetFishingEncountMonsNo( VMHANDLE* core, void* wk )
{
  u8                season;
  u16               *tbl, i,j,count = 0;
  ENCOUNT_DATA      data;
  ARCHANDLE*        handle;
  SCRCMD_WORK*      work = (SCRCMD_WORK*)wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );

  static const u16 zone_tbl[FISHING_ENC_POKE_ZONE_MAX] = {
    ZONE_ID_H04,
    ZONE_ID_R01,
    ZONE_ID_R03R0301,
    ZONE_ID_R13,
    ZONE_ID_W17
  };

  u16* ret_wk = SCRCMD_GetVMWork( core, work );

  season = GAMEDATA_GetSeasonID( gdata );
  handle = GFL_ARC_OpenDataHandle( ARCID_ENCOUNT, GFL_HEAP_LOWID(HEAPID_FIELDMAP) );
  tbl = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(HEAPID_FIELDMAP), sizeof(u16)*FISHING_ENC_POKE_MAX);

  for(i = 0;i < NELEMS(zone_tbl);i++){
    if( ENCOUNT_DATA_Load( &data, handle, zone_tbl[i], season ) == FALSE ){
      continue;
    }
    if( data.probFishing > 0 ){
      for(j = 0;j < ENC_MONS_NUM_FISHING;j++){
        count = sub_EntryFishingEncMonsNo( tbl, count, data.fishingMons[j].monsNo);
      }
    }
    if( data.probFishingSp > 0 ){
      for(j = 0;j < ENC_MONS_NUM_FISHING_SP;j++){
        count = sub_EntryFishingEncMonsNo( tbl, count, data.fishingSpMons[j].monsNo);
      }
    }
  }
  *ret_wk = tbl[ GFUser_GetPublicRand0( count ) ];

  GFL_HEAP_FreeMemory(tbl);
  GFL_ARC_CloseDataHandle( handle );

	return VMCMD_RESULT_CONTINUE;
}

