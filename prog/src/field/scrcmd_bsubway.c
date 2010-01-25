//======================================================================
/**
 * @file  scrcmd_bsybway.c
 * @brief  �o�g���T�u�E�F�C�@�X�N���v�g�R�}���h��p�\�[�X
 * @author  Miyuki Iwasawa
 * @date  2006.05.23
 *
 * 2007.05.24  Satoshi Nohara*
 *
 * @note pl����ڐA kagaya
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "poke_tool/poke_regulation.h"
#include "savedata/save_tbl.h"
#include "../../../resource/fldmapdata/flagwork/flag_define.h" //SYS_FLAG_SPEXIT_REQUEST
#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "bsubway_scr.h"
#include "bsubway_scrwork.h"
#include "scrcmd_bsubway.h"

#include "event_bsubway.h"

#include "fldmmdl.h"

#include "../../../resource/fldmapdata/zonetable/zone_id.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
static BOOL bsway_CheckEntryPokeNum(
    u16 num, GAMESYS_WORK *gsys, BOOL item_flag );
static BOOL bsway_CheckRegulation( int mode, GAMESYS_WORK *gsys );

//======================================================================
//  proto
//======================================================================

//======================================================================
//  �o�g���T�u�E�F�C�@�X�N���v�g�֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C�p���[�N�쐬�Ə�����
 *  @param  init  ���������[�h�w��
 *          BSWAY_PLAY_NEW:new game
 *          BSWAY_PLAY_CONTINUE:new game
 *  @param  mode  �v���C���[�h�w��:BSWAY_MODE_�`
 *  @return 0
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBSubwayWorkCreate( VMHANDLE* core, void *wk )
{
  u16 init = VMGetU16(core);
  u16 mode = VMGetU16(core);
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  BSUBWAY_SCRWORK *bsw_scr;
  bsw_scr = BSUBWAY_SCRWORK_CreateWork( gsys, init, mode );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C����p���[�N�|�C���^������
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBSubwayWorkClear( VMHANDLE* core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  BSUBWAY_SCRWORK_ClearWork( gsys );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C�p���[�N���
 *  @return 0
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBSubwayWorkRelease(VMHANDLE* core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  BSUBWAY_SCRWORK *bsw_scr;
  
  bsw_scr = GAMEDATA_GetBSubwayScrWork( gdata );
  BSUBWAY_SCRWORK_ReleaseWork( gsys, bsw_scr );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C�p�R�}���h�Q�Ăяo���C���^�[�t�F�[�X
 *  @param  com_id    u16:�R�}���hID
 *  @param  retwk_id  u16:�Ԃ�l���i�[���郏�[�N��ID
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBSubwayTool( VMHANDLE *core, void *wk )
{
  void **buf;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( gdata );
  EVENTWORK *event = GAMEDATA_GetEventWork( gdata );
  BSUBWAY_SCRWORK *bsw_scr = GAMEDATA_GetBSubwayScrWork( gdata );
  BSUBWAY_PLAYDATA *playData =
    SaveControl_DataPtrGet( save, GMDATA_ID_BSUBWAY_PLAYDATA );
  u32 play_mode = (u32)BSUBWAY_PLAYDATA_GetData(
        playData, BSWAY_PLAYDATA_ID_playmode, NULL );
  BSUBWAY_SCOREDATA *scoreData = SaveControl_DataPtrGet(
      save, GMDATA_ID_BSUBWAY_SCOREDATA );
  FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  u16 com_id = VMGetU16( core );
  u16 param = SCRCMD_GetVMWorkValue( core, work );
  u16 retwk_id = VMGetU16( core );
  u16 *ret_wk = SCRIPT_GetEventWork( sc, gdata, retwk_id );
  
  if( bsw_scr == NULL && //�o�O�`�F�b�N�@���[�N�ˑ��R�}���h
      com_id >= BSWSUB_START && com_id < BSWSUB_END ){
    GF_ASSERT( 0 );
    return( VMCMD_RESULT_CONTINUE );
  }
  
  switch( com_id ){
  //���Z�b�g
  case BSWTOOL_SYSTEM_RESET:
    OS_ResetSystem( 0 );
    break;
  //�v���C�f�[�^�N���A
  case BSWTOOL_CLEAR_PLAY_DATA:
    BSUBWAY_PLAYDATA_Init( playData );
    break;
  //�莝���|�P�������`�F�b�N
  case BSWTOOL_CHK_ENTRY_POKE_NUM:
    if( param == 0 ){
      GF_ASSERT( bsw_scr != NULL );
      *ret_wk = bsway_CheckEntryPokeNum( bsw_scr->member_num, gsys, 1 );
    }else{
      *ret_wk = bsway_CheckEntryPokeNum( param, gsys, 1 );
    }
    break;
  //�Z�[�u����Ă��邩
  case BSWTOOL_IS_SAVE_DATA_ENABLE:
    *ret_wk = BSUBWAY_PLAYDATA_GetSaveFlag( playData );
    break;
  //���A�ʒu�ݒ�
  case BSWTOOL_PUSH_NOW_LOCATION:
    {
      VecFx32 pos;
      LOCATION loc;
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      
      FIELD_PLAYER_GetPos( fld_player, &pos );
      LOCATION_SetDirect( &loc, FIELDMAP_GetZoneID(fieldmap),
          FIELD_PLAYER_GetDir(fld_player), pos.x, pos.y, pos.z );
      GAMEDATA_SetSpecialLocation( gdata, &loc );
      EVENTWORK_SetEventFlag( event, SYS_FLAG_SPEXIT_REQUEST );
    }
    break;
  //���A�ʒu����
  case BSWTOOL_POP_NOW_LOCATION:
    EVENTWORK_ResetEventFlag( event, SYS_FLAG_SPEXIT_REQUEST );
    break;
  //�G���[���̃X�R�A�Z�b�g
  case BSWTOOL_SET_NG_SCORE:
    *ret_wk = BSUBWAY_SCRWORK_SetNGScore( gsys );
    break;
  //�A�����擾
  case BSWTOOL_GET_RENSHOU_CNT:
    *ret_wk = BSUBWAY_SCOREDATA_GetRenshou( scoreData, param );
    if( (s16)(*ret_wk) < 0 ){ 
      *ret_wk = 0;
    }
    break;
  //���@OBJ�R�[�h�擾
  case BSWTOOL_GET_MINE_OBJ:
    {
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      int sex = FIELD_PLAYER_GetSex( fld_player );
      *ret_wk = FIELD_PLAYER_GetDrawFormToOBJCode(
          sex, PLAYER_DRAW_FORM_NORMAL );
    }
    break;
  //���@��\��
  case BSWTOOL_PLAYER_VANISH:
    {
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
      MMDL_SetStatusBitVanish( mmdl, TRUE );
    }
    break;
  //���M�����[�V�����`�F�b�N
  case BSWTOOL_CHK_REGULATION:
    *ret_wk = 0; //ok
    if( bsway_CheckRegulation(param,gsys) == FALSE ){
      *ret_wk = 1;
    }
    break;
  //�v���C���[�h�擾
  case BSWTOOL_GET_PLAY_MODE:
    *ret_wk = BSUBWAY_PLAYDATA_GetData(
        playData, BSWAY_PLAYDATA_ID_playmode, NULL );
    break;
  //���݃��E���h���擾
  case BSWTOOL_GET_NOW_ROUND:
    *ret_wk = BSUBWAY_PLAYDATA_GetRoundNo( playData );
    break;
  //���E���h������
  case BSWTOOL_INC_ROUND:
    BSUBWAY_PLAYDATA_IncRoundNo( playData );
    BSUBWAY_SCOREDATA_IncRenshou( scoreData, play_mode );
    *ret_wk = BSUBWAY_SCOREDATA_GetRenshou( scoreData, play_mode );
    break;
  //���̃��E���h���擾
  case BSWTOOL_GET_NEXT_ROUND:
    *ret_wk = BSUBWAY_PLAYDATA_GetRoundNo( bsw_scr->playData ) + 1;
    break;
  //Wifi�����N�_�E��
  case BSWTOOL_WIFI_RANK_DOWN:
    #if 0
    *ret_wk = TowerScr_SetWifiRank(NULL,core->fsys->savedata,2);
    #else
    GF_ASSERT( 0 && "BSWTOOL_WIFI_RANK_DOWN WB���쐬" );
    #endif
    break;
  //Wifi�����N�擾
  case BSWTOOL_GET_WIFI_RANK:
    #if 0
    *ret_wk = TowerScr_SetWifiRank(NULL,core->fsys->savedata,0);
    #else
    GF_ASSERT( 0 && "BSWTOOL_GET_WIFI_RANK WB���쐬" );
    #endif
    break;
  //Wifi�A�b�v���[�h�t���O���Z�b�g
  case BSWTOOL_SET_WIFI_UPLOAD_FLAG:
    #if 0
    TowerScrTools_SetWifiUploadFlag(core->fsys->savedata,param);
    #else
    GF_ASSERT( 0 && "BSWTOOL_SET_WIFI_UPLOAD_FLAG WB���쐬" );
    #endif
    break;
  //Wifi�A�b�v���[�h�t���O���擾
  case BSWTOOL_GET_WIFI_UPLOAD_FLAG:
    #if 0
    *ret_wk = TowerScrTools_GetWifiUploadFlag(core->fsys->savedata);
    #else
    GF_ASSERT( 0 && "BSWTOOL_GET_WIFI_UPLOAD_FLAG WB���쐬" );
    #endif
    break;
  //Wifi�ڑ�
  case BSWTOOL_WIFI_CONNECT:
    #if 0
    EventCmd_BTowerWifiCall(core->event_work,param,retwk_id,*ret_wk);
    return 1;
    #else
    GF_ASSERT( 0 && "BSWTOOL_WIFI_CONNECT WB���쐬�R�}���h�ł�" );
    #endif
    break;
  //----���[�N�ˑ�
  //�v���C���[�h�ʕ��A�ʒu�Z�b�g
  case BSWSUB_SET_PLAY_MODE_LOCATION:
    {
      VecFx32 pos;
      LOCATION loc;
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      
      FIELD_PLAYER_GetPos( fld_player, &pos );
      
      switch( bsw_scr->play_mode ){
      case BSWAY_MODE_SINGLE:
        LOCATION_SetDirect( &loc, ZONE_ID_C04R0102, DIR_RIGHT, 13, 0, 13 );
        break;
      case BSWAY_MODE_DOUBLE:
      default:
        LOCATION_SetDirect( &loc, ZONE_ID_C04R0104, DIR_RIGHT, 13, 0, 13 );
        break;
      }
      
      GAMEDATA_SetSpecialLocation( gdata, &loc );
      EVENTWORK_SetEventFlag( event, SYS_FLAG_SPEXIT_REQUEST );
    }
    break;
  //�|�P�����I����ʂ�
  case BSWSUB_SELECT_POKE:
    SCRIPT_CallEvent( sc, BSUBWAY_EVENT_SetSelectPokeList(bsw_scr,gsys) );
    return VMCMD_RESULT_SUSPEND;
  //�I���|�P�����擾
  case BSWSUB_GET_ENTRY_POKE:
    *ret_wk = BSUBWAY_SCRWORK_GetEntryPoke( bsw_scr, gsys );
    break;
  //�Q���w�肵���莝���|�P�����̏����`�F�b�N
  case BSWSUB_CHK_ENTRY_POKE:
    #if 0
    *ret_wk = TowerScr_CheckEntryPoke(bsw_scr,core->fsys->savedata);
    #else
    OS_Printf( "BSWSUB_GET_ENTRY_POKE WB���쐬" );
    *ret_wk = 0; //0=OK 1=�����|�P���� 2=�����A�C�e��
    #endif
    break;
  //�N���A���Ă��邩
  case BSWSUB_IS_CLEAR:
    if( BSUBWAY_SCRWORK_IsClear(bsw_scr) == TRUE ){
      *ret_wk = TRUE;
    }else{
      *ret_wk = FALSE;
    }
    break;
  //�s�k���̃X�R�A�Z�b�g
  case BSWSUB_SET_LOSE_SCORE:
    BSUBWAY_SCRWORK_SetLoseScore( gsys, bsw_scr );
    break;
  //�N���A���̃X�R�A�Z�b�g
  case BSWSUB_SET_CLEAR_SCORE:
    BSUBWAY_SCRWORK_SetClearScore( bsw_scr, gsys );
    break;
  //�x�ލۂ̃v���C�f�[�^����
  case BSWSUB_SAVE_REST_PLAY_DATA:
    BSUBWAY_SCRWORK_SaveRestPlayData( bsw_scr );
    break;
  //�ΐ�g���[�i�[���I
  case BSWSUB_CHOICE_BTL_PARTNER:
    BSUBWAY_SCRWORK_ChoiceBattlePartner( bsw_scr );
    break;
  //�G�g���[�i�[OBJ�R�[�h�擾
  case BSWSUB_GET_ENEMY_OBJ:
    *ret_wk = BSUBWAY_SCRWORK_GetTrainerOBJCode( bsw_scr, param );
    break;
  //�o�g���Ăяo��
	case BSWSUB_LOCAL_BTL_CALL:
    SCRIPT_CallEvent(
        sc, BSUBWAY_EVENT_TrainerBattle(bsw_scr,gsys,fieldmap) );
    return VMCMD_RESULT_SUSPEND;
  //���݂̃v���C���[�h���擾
  case BSWSUB_GET_PLAY_MODE:
    *ret_wk = bsw_scr->play_mode;
    break;
  //���[�_�[�N���A�t���O���Z�b�g
  case BSWSUB_SET_LEADER_CLEAR_FLAG:
    bsw_scr->leader_f = param;
    break;
  //���[�_�[�N���A�t���O���擾
  case BSWSUB_GET_LEADER_CLEAR_FLAG:
    *ret_wk = bsw_scr->leader_f;
    break;
  //�o�g���|�C���g���Z
  case BSWSUB_ADD_BATTLE_POINT:
    *ret_wk = BSUBWAY_SCRWORK_AddBattlePoint( bsw_scr );
    break;
  //�p�[�g�i�[�ԍ��Z�b�g
  case BSWSUB_SET_PARTNER_NO:
    bsw_scr->partner = param;
    break;
  //�p�[�g�i�[�ԍ��擾
  case BSWSUB_GET_PARTNER_NO:
    *ret_wk = bsw_scr->partner;
    break;
  //�퓬�g���[�i�[�Z�b�g
  case BSWSUB_BTL_TRAINER_SET:
    BSUBWAY_SCRWORK_SetBtlTrainerNo( bsw_scr );
    break;
  //�I���|�P�����ԍ��擾
  case BSWSUB_GET_SELPOKE_IDX:
    *ret_wk = bsw_scr->member[param];
    break;
  //(BTS�ʐM142)�ύX�̑Ώ�
  case BSWSUB_WIFI_RANK_UP:
    #if 0
    *ret_wk = TowerScr_SetWifiRank(bsw_scr,core->fsys->savedata,1);
    #else
    GF_ASSERT( 0 && "BSWSUB_WIFI_RANK_UP WB���쐬" );
    #endif
    break;
  //��M�o�b�t�@�N���A
  case BSWSUB_RECV_BUF_CLEAR:
    #if 0
    MI_CpuClear8(wk->recv_buf,BSWAY_SIO_BUF_LEN);
    #else
    GF_ASSERT( 0 && "BSWSUB_RECV_BUF_CLEAR WB���쐬" );
    #endif
    break;
  //�g���[�i�[�ΐ�O���b�Z�[�W�\��
  case BSWSUB_TRAINER_BEFORE_MSG:
    SCRIPT_CallEvent(
        sc, BSUBWAY_EVENT_TrainerBeforeMsg(bsw_scr,gsys,param) );
    return( VMCMD_RESULT_SUSPEND );
  //�Q�[���N���A���̃v���C�f�[�^���Z�[�u
  case BSWSUB_SAVE_GAMECLEAR:
    BSUBWAY_SCRWORK_SaveGameClearPlayData( bsw_scr );
    break;
  //�|�P���������o�[���[�h
  case BSWSUB_LOAD_POKEMON_MEMBER:
    BSUBWAY_SCRWORK_LoadPokemonMember( bsw_scr, gsys );
    break;
  //�G���[
  default:
    OS_Printf( "�n���ꂽcom_id = %d\n", com_id );
    GF_ASSERT( 0 && "com_id�����Ή��ł��I" );
    *ret_wk = 0;
    break;
  }
  
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * �Q���\�ȃ|�P�������̃`�F�b�N
 * @param  num      �Q���ɕK�v�ȃ|�P������
 * @param  item_flag  �A�C�e���`�F�b�N���邩�t���O
 * @retval BOOL TRUE=�Q���\
 */
//--------------------------------------------------------------
static BOOL bsway_CheckEntryPokeNum(
    u16 num, GAMESYS_WORK *gsys, BOOL item_flag )
{
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  const POKEPARTY *party =  GAMEDATA_GetMyPokemon( gdata );
  
  if( PokeParty_GetPokeCount(party) < num ){
    return FALSE;
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���M�����[�V�����`�F�b�N
 * @param mode �v���C���[�h
 * @param gsys GAMESYS_WORK
 * @retval BOOL TRUE=OK
 */
//--------------------------------------------------------------
static BOOL bsway_CheckRegulation( int mode, GAMESYS_WORK *gsys )
{
  int reg_type,ret;
  GAMEDATA *gdata;
  POKEPARTY *party;
  REGULATION *reg_data;
  
  switch( mode ){
  case BSWAY_MODE_SINGLE:
    reg_type = REG_SUBWAY_SINGLE;
    break;
  case BSWAY_MODE_DOUBLE:
    reg_type = REG_SUBWAY_DOUBLE;
    break;
  default:
    GF_ASSERT( 0 );
    reg_type = REG_SUBWAY_DOUBLE;
  }

  reg_data = (REGULATION*)PokeRegulation_LoadDataAlloc(
      reg_type, HEAPID_PROC );
  
  gdata = GAMESYSTEM_GetGameData( gsys );
  party =  GAMEDATA_GetMyPokemon( gdata );
  ret = PokeRegulationMatchPartialPokeParty( reg_data, party );
  GFL_HEAP_FreeMemory( reg_data );
  
  if( ret == POKE_REG_OK || POKE_REG_TOTAL_LV_FAILED ){
    return( TRUE );
  }
  
  return( FALSE );
}
