//======================================================================
/**
 * @file  scrcmd_sppoke.c
 * @brief  ����|�P�C�x���g�X�N���v�g
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"
#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"
#include "scrcmd_sppoke.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "sp_poke_gimmick.h"

#include "move_pokemon.h"
#include "poke_tool/poke_memo.h"
#include "waza_tool/wazano_def.h"

#define GRID_HALF_SIZE ((FIELD_CONST_GRID_SIZE/2)*FX32_ONE)

static BOOL SearchEventPoke( POKEPARTY* party,
                             const int inMonsNo, const int inSkillNo, const int inEventFlg,
                             u16 *outPos );

//--------------------------------------------------------------
/**
 * �{�[���A�j��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMoveBall( VMHANDLE *core, void *wk )
{
  GMEVENT *event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  u16 type = SCRCMD_GetVMWorkValue( core, work );
  u16 x = SCRCMD_GetVMWorkValue( core, work );
  u16 y = SCRCMD_GetVMWorkValue( core, work );
  u16 z = SCRCMD_GetVMWorkValue( core, work );
  u16 height = SCRCMD_GetVMWorkValue( core, work );
  u16 sync = SCRCMD_GetVMWorkValue( core, work );

  VecFx32 start, end;
  VecFx32 *dst;
  VecFx32 *src;
  fx32 fx_height;

  //�J�n�n�_�͎��@�̈ʒu
  {
    FIELD_PLAYER *fld_player;
    fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    FIELD_PLAYER_GetPos( fld_player, &start );
  }
  //�ړI���W�Z�b�g
  end.x = x * FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE;
  end.y = y * GRID_HALF_SIZE;
  end.z = z * FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE;

  if (type == SCR_BALL_ANM_TYPE_OUT)
  {
    src = &start;
    dst = &end;
  }
  else {
    src = &end;
    dst = &start;
  }

  fx_height = height * GRID_HALF_SIZE;

  event = SPPOKE_GMK_MoveBall( gsys, type, src, dst, fx_height, sync);

  if (event == NULL){
    GF_ASSERT(0);
    return VMCMD_RESULT_SUSPEND;
  }
  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �{�[���A�j��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdStartBallAnm( VMHANDLE *core, void *wk )
{
  GMEVENT *event;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  u16 type = SCRCMD_GetVMWorkValue( core, work );
  u16 x = SCRCMD_GetVMWorkValue( core, work );
  u16 y = SCRCMD_GetVMWorkValue( core, work );
  u16 z = SCRCMD_GetVMWorkValue( core, work );

  VecFx32 pos;
  pos.x = x * FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE;
  pos.y = y * GRID_HALF_SIZE;
  pos.z = z * FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE;

  SPPOKE_GMK_StartBallAnm( gsys, type, &pos );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �{�[���A�j���I���҂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdWaitBallAnm( VMHANDLE *core, void *wk )
{
  GMEVENT *event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  u16 type = SCRCMD_GetVMWorkValue( core, work );

  event = SPPOKE_GMK_WaitBallAnmEnd( gsys, type );

  if (event == NULL){
    GF_ASSERT(0);
    return VMCMD_RESULT_SUSPEND;
  }
  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;

}

//--------------------------------------------------------------
/**
 * �|�P�����\���A��\�����Ă����t���[���܂ő҂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdWaitBallPokeApp( VMHANDLE *core, void *wk )
{
  GMEVENT *event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  u16 type = SCRCMD_GetVMWorkValue( core, work );

  event = SPPOKE_GMK_WaitPokeAppear( gsys, type );

  if (event == NULL){
    GF_ASSERT(0);
    return VMCMD_RESULT_SUSPEND;
  }
  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;

}

//--------------------------------------------------------------
/**
 * �莝���̒��̔z�z�|�P�����̈ʒu���擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 * @note    �����𖞂����|�P�����Ȃ��ꍇ�� ret��FALSE���Z�b�g�����
 * @note�@�@�����͔z�z���ꂽ�|�P�ł��A�ŗL�Z���o���Ă��Ȃ�����
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetEvtPokePos( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY* party    = GAMEDATA_GetMyPokemon( gdata );
  u16 monsno = SCRCMD_GetVMWorkValue( core, work );
  u16 skill_flg = SCRCMD_GetVMWorkValue( core, work );
  u16 *poke_pos = SCRCMD_GetVMWork( core, work );
  u16 *ret = SCRCMD_GetVMWork( core, work );
  int evt_flg;
  int skill_no;

  switch(monsno){
  case MONSNO_654:
    evt_flg = POKE_MEMO_EVENT_DARUTANISU;
    skill_no = WAZANO_SINPINOTURUGI;
    break;
  case MONSNO_655:
    evt_flg = POKE_MEMO_EVENT_MERODHIA;
    skill_no = WAZANO_INISIENOUTA;
    break;
  case MONSNO_SEREBHI:
    evt_flg = POKE_MEMO_EVENT_2010MOVIE_SEREBIXI_BEF;
    skill_no = WAZANO_NULL;
    break;
  case MONSNO_ENTEI:
  case MONSNO_RAIKOU:
  case MONSNO_SUIKUN:
    evt_flg = POKE_MEMO_EVENT_2010MOVIE_ENRAISUI_BEF;
    skill_no = WAZANO_NULL;
    break;
  case MONSNO_656:
    evt_flg = POKE_MEMO_EVENT_INSEKUTA;
    skill_no = WAZANO_NULL;
    break;
  case MONSNO_SHEIMI:
    evt_flg = POKE_MEMO_EVENT_SHEIMI;
    skill_no = WAZANO_NULL;
    break;
  default:
    *ret = FALSE;
    return VMCMD_RESULT_CONTINUE;
  }

  if ( skill_flg == FALSE ) skill_no = WAZANO_NULL;

  *ret = SearchEventPoke(
      party, monsno, skill_no, evt_flg, poke_pos );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �莝���̒��̌ŗL�Z���o���邱�Ƃ��ł������|�P�����̈ʒu��Ԃ��֐�
 *
 * @param   party         �@�Ώۃp�[�e�B
 * @param   inMonsNo        �Ώۃ|�P����
 * @param   inSkillNo       �ΏۋZ
 * @param   inEventFlg      �C�x���g�t���O
 * @param   outPos          �莝���|�P�����ʒu
 *
 * @return  BOOL TRUE�Ŕ���
 * @note    �����𖞂����|�P���������Ȃ��ꍇ�� ret��FALSE���Z�b�g�����
 * @note�@�@�����͔z�z���ꂽ�|�P�����ł��A�ŗL�Z���o���Ă��Ȃ�����
 * @note    ����inSkillNo �� WAZANO_NULL�̏ꍇ�͋Z�`�F�b�N�͂��Ȃ�
 */
//--------------------------------------------------------------
static BOOL SearchEventPoke( POKEPARTY* party,
                             const int inMonsNo, const int inSkillNo, const int inEventFlg,
                             u16 *outPos )
{
  BOOL rc;
  int i,num;
  num = PokeParty_GetPokeCount( party );
  rc = FALSE;
  *outPos = 0;
  //�莝������
  for (i=0;i<num;i++)
  {
    POKEMON_PARAM *pp = PokeParty_GetMemberPointer(party, i);
    //�z�z�`�F�b�N
    if ( !POKE_MEMO_CheckEventPokePP( pp ,inEventFlg ) ) continue;
    //�����X�^�[�i���o�[�`�F�b�N
    if ( PP_Get( pp, ID_PARA_monsno, NULL ) != inMonsNo ) continue;

    if ( inSkillNo != WAZANO_NULL )
    {
      //�Z�`�F�b�N(���Ɋo���Ă��邩)
      if ( PP_Get( pp, ID_PARA_waza1, NULL ) == inSkillNo) continue;
      if ( PP_Get( pp, ID_PARA_waza2, NULL ) == inSkillNo) continue;
      if ( PP_Get( pp, ID_PARA_waza3, NULL ) == inSkillNo) continue;
      if ( PP_Get( pp, ID_PARA_waza4, NULL ) == inSkillNo) continue;
    }
    rc = TRUE;
    *outPos = i;
    NOZOMU_Printf("�Ώۃ|�P���������@�ʒu%d\n",i);
    break;
  }
  return rc;
}

//--------------------------------------------------------------
/**
 * �莝���̎w��ʒu�̔z�z�|�P�����̃C�x���g�t���O���C�x���g�I����Ԃɏ���������
 * �C�x���g�Ώۃ|�P�����łȂ��ꍇ��
 * �C�x���g�Ώۃ|�P���������ǃC�x���g�t���O���������Ă��Ȃ��|�P�����̂Ƃ��͏������X���[
 *
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetEvtPokeAfterFlg( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY* party    = GAMEDATA_GetMyPokemon( gdata );
  u16 monsno = SCRCMD_GetVMWorkValue( core, work );
  u16 poke_pos = SCRCMD_GetVMWorkValue( core, work );
  int evt_flg;

  switch(monsno){
  case MONSNO_SEREBHI:
    evt_flg = POKE_MEMO_EVENT_2010MOVIE_SEREBIXI_BEF;
    break;
  default:
    //�Ώۃ|�P�����ł͂Ȃ�
    return VMCMD_RESULT_CONTINUE;
  }

  {
    POKEMON_PARAM *pp = PokeParty_GetMemberPointer(party, poke_pos);
    //�z�z�`�F�b�N
    if ( !POKE_MEMO_CheckEventPokePP( pp ,evt_flg ) )
    {
      //�t���O�������Ă��Ȃ�
      return VMCMD_RESULT_CONTINUE;
    }

    //��������
    POKE_MEMO_SetEventPoke_AfterEventPP( pp , evt_flg );
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief �ړ��|�P�����̃X�e�[�^�X���擾
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 *
 * MVPOKE_STATE_NONE��
 *
 * ZONE_CHANGE�̃^�C�~���O�Ŏg�������̂�scrcmd_encount.c ���炨���z��
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdGetMovePokemonStatus( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*      work = (SCRCMD_WORK*)wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
	ENC_SV_PTR enc_sv = EncDataSave_GetSaveDataPtr( GAMEDATA_GetSaveControlWork( gdata ) );
	
  u8 move_poke = (u8)SCRCMD_GetVMWorkValue( core, work );
  u16* ret_wk = SCRCMD_GetVMWork( core, work );
	
  *ret_wk = EncDataSave_GetMovePokeState( enc_sv, move_poke );

	return VMCMD_RESULT_CONTINUE;
}


