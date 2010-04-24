//==============================================================================
/**
 * @file    scrcmd_symbol.c
 * @brief   �N���F�V���{���|�P����
 * @author  matsuda --> tamada
 * @date    2010.03.16(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_symbol.h"
#include "savedata/symbol_save.h"
#include "savedata/symbol_save_field.h"
#include "event_symbol.h"

#include "symbol_map.h"
#include "symbol_pokemon.h"

#include "field/intrude_symbol.h"
#include "field_comm/intrude_work.h"    //Intrude_Check_CommConnect

#include "arc/fieldmap/zone_id.h"
#include "event_mapchange.h"

#include "../../../resource/fldmapdata/flagwork/work_define.h"  //SCWK_TARGET_OBJID

//==============================================================================
//==============================================================================
static MMDL * getPokeMMdl( SCRCMD_WORK * work, u16 obj_id );
static void getFrontSymPoke( SCRCMD_WORK * work, SYMBOL_POKEMON * sympoke );
static POKEMON_PARAM * createPokemon( SCRCMD_WORK * work, MMDL * mmdl, HEAPID heapID, SYMBOL_POKEMON *sympoke );
static void sendDataChange( SCRCMD_WORK * work );


//==============================================================================
//==============================================================================
//==================================================================
/**
 * �V���{���|�P�����o�g��
 *
 * @param   core		
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 *
 * �ߊl�����ꍇ�A���ʂ�TRUE���B�ߊl���Ȃ������ꍇ�AFALSE�ɂȂ�܂�
 */
//==================================================================
VMCMD_RESULT EvCmdSymbolPokeBattle( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  u16 obj_id = SCRCMD_GetVMWorkValue(core,work);
  u16* ret_wk = SCRCMD_GetVMWork(core,work);  //���ʂ�Ԃ����[�N
  SYMBOL_POKEMON sympoke;

  MMDL * mmdl = getPokeMMdl( work, obj_id );
  if ( mmdl )
  {
    POKEMON_PARAM * pp = createPokemon( work, mmdl, HEAPID_PROC, &sympoke );
    if ( pp )
    {
      SCRIPT_WORK*   scw = SCRCMD_WORK_GetScriptWork( work );
      GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
      FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
      GMEVENT * event = EVENT_SymbolPokeBattle( gsys, fieldmap, pp, sympoke, ret_wk, HEAPID_PROC );
      SCRIPT_CallEvent( scw, event );
    }
  }

  return VMCMD_RESULT_SUSPEND;
}

//==================================================================
/**
 * �V���{���|�P�����Z�b�e�B���O
 *
 * @param   core		���z�}�V������\���̂ւ̃|�C���^
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 */
//==================================================================
VMCMD_RESULT EvCmdSymbolMapPokeSet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  HEAPID heapID = SCRCMD_WORK_GetHeapID( work );
  INTRUDE_SYMBOL_WORK * isw;
  u32 start_no;
  
  isw = SYMBOLMAP_AllocSymbolWork( heapID, gsys, &start_no );
  SYMBOLPOKE_Add( fieldmap, start_no, isw->spoke_array, isw->num, isw->symbol_map_id );
  GFL_HEAP_FreeMemory( isw );
  
  return VMCMD_RESULT_CONTINUE;
}

//==================================================================
/**
 * @brief �V���{���|�P��������肷��
 */
//==================================================================
VMCMD_RESULT EvCmdSymbolPokeGet( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  u16 obj_id = SCRCMD_GetVMWorkValue(core,work);

  GAMEDATA *  gamedata = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(gamedata);
  SYMBOL_SAVE_WORK *symbol_save = SymbolSave_GetSymbolData(sv_ctrl);

  MMDL * mmdl = getPokeMMdl( work, obj_id );

  if ( mmdl ) {
    u32 no = SYMBOLPOKE_GetSymbolNo( mmdl );
    SymbolSave_DataShift( symbol_save, no );
    sendDataChange( work ); //�V���{���|�P�����̍X�V��ʐM�Œʒm����
  }

  return VMCMD_RESULT_CONTINUE;
}

//==================================================================
/**
 * @brief �V���{���|�P�������ړ�����
 */
//==================================================================
VMCMD_RESULT EvCmdSymbolMapMovePokemon( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  u16 obj_id = SCRCMD_GetVMWorkValue( core, wk );
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );

  GAMEDATA *  gamedata = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(gamedata);
  SYMBOL_SAVE_WORK *symbol_save = SymbolSave_GetSymbolData(sv_ctrl);

  MMDL * mmdl = getPokeMMdl( work, obj_id );

  if ( mmdl ) {
    u32 no = SYMBOLPOKE_GetSymbolNo( mmdl );
    *ret_wk = SymbolSave_Field_MoveAuto( symbol_save, no );
    sendDataChange( work ); //�V���{���|�P�����̍X�V��ʐM�Œʒm����
  } else {
    *ret_wk = FALSE;
  }

  return VMCMD_RESULT_CONTINUE;
}

//==================================================================
/**
 * @brief �V���{���G���J�E���g�F�Ώۃ|�P�����̖��O���Z�b�g
 */
//==================================================================
VMCMD_RESULT EvCmdSymbolMapSetMonsName( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK *work = wk;
  u16 obj_id = SCRCMD_GetVMWorkValue( core, wk );
  u16 idx = SCRCMD_GetVMWorkValue( core, wk );
  SYMBOL_POKEMON sympoke;
  
  MMDL * mmdl = getPokeMMdl( work, obj_id );

  if ( mmdl )
  {
    HEAPID     heap_id = SCRCMD_WORK_GetHeapID( work );
    POKEMON_PARAM * pp = createPokemon( work, mmdl, heap_id, &sympoke );
    if ( pp )
    {
      SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
      WORDSET *wordset    = SCRIPT_GetWordSet( sc );

      WORDSET_RegisterPokeMonsName( wordset, idx, pp );
      GFL_HEAP_FreeMemory( pp );
    }
  }
  return VMCMD_RESULT_CONTINUE;
}

//==================================================================
/**
 * @brief �V���{���G���J�E���g�}�b�v�F���擾
 */
//==================================================================
VMCMD_RESULT EvCmdSymbolMapGetInfo( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK * work = wk;
  u16 mode = SCRCMD_GetVMWorkValue( core, wk );
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );

  GAMESYS_WORK *  gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *  gamedata = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(gamedata);
  SYMBOL_SAVE_WORK *symbol_save = SymbolSave_GetSymbolData(sv_ctrl);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	 
  switch ( mode )
  {
  case SCR_SYMMAP_INFO_IS_MINE:
    //�����L�֐��ŒʐM������ʐM�����������ǂ����̔��肪�ł���
    if ( IntrudeSymbol_CheckIntrudeNetID( game_comm, gamedata ) == INTRUDE_NETID_NULL ) {
      *ret_wk = TRUE;
    } else {
      *ret_wk = FALSE;
    }
    break;
  case SCR_SYMMAP_INFO_IS_KEEPZONE:
    {
      SYMBOL_MAP_ID sid = GAMEDATA_GetSymbolMapID( gamedata );
      *ret_wk = SYMBOLMAP_IsKeepzoneID( sid );
    }
    break;
  case SCR_SYMMAP_INFO_IS_ENTRANCE:
    {
      SYMBOL_MAP_ID sid = GAMEDATA_GetSymbolMapID( gamedata );
      *ret_wk = SYMBOLMAP_IsEntranceID( sid );
    }
    break;
  case SCR_SYMMAP_INFO_FRONT_MONSNO:
    {
      SYMBOL_POKEMON sympoke;
      getFrontSymPoke( work, &sympoke );
      *ret_wk = sympoke.monsno;
    }
    break;
  case SCR_SYMMAP_INFO_FRONT_FORMNO:
    {
      SYMBOL_POKEMON sympoke;
      getFrontSymPoke( work, &sympoke );
      *ret_wk = sympoke.form_no;
    }
    break;
  }
  return VMCMD_RESULT_CONTINUE;
}

//==================================================================
/**
 * @brief �V���{���G���J�E���g�}�b�v�F�}�b�v�J��
 */
//==================================================================
VMCMD_RESULT EvCmdSymbolMapWarp( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *   work = wk;
  SCRIPT_WORK *     sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *  gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *  gamedata = SCRCMD_WORK_GetGameData( work );
  u16         warp_dir = SCRCMD_GetVMWorkValue( core, work );

  SYMBOL_MAP_ID sid;
  GMEVENT * new_event;

  sid = GAMEDATA_GetSymbolMapID( gamedata );
  OS_TPrintf( "SCRCMD:SYMMAP_WARP:id=%2d, dir=%d\n", sid, warp_dir );

  if ( warp_dir == DIR_DOWN && SYMBOLMAP_IsEntranceID(sid) )
  { //�������Ƃ���̃}�b�v�ŉ��������p���X�ɖ߂�
    VecFx32 pos;
    GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
    IntrudeSymbol_GetPosPalaceForestDoorway( game_comm, gamedata, &pos );
    new_event = EVENT_ChangeMapPos( gsys, GAMESYSTEM_GetFieldMapWork( gsys ),
        ZONE_ID_PALACE01, &pos, DIR_DOWN, FALSE );
  }
  else
  { //�p���X�̐X�̒����ړ�
    if ( warp_dir != DIR_NOT ) {
      sid = SYMBOLMAP_GetNextSymbolMapID( gsys, GAMEDATA_GetSymbolMapID( gamedata ), warp_dir );
    }
    new_event = EVENT_SymbolMapWarpEasy( gsys, warp_dir, sid );
    OS_TPrintf("SCRCMD:SYMMAP_WARP:next id = %d\n", sid );
  }
  SCRIPT_CallEvent( sc, new_event );
  return VMCMD_RESULT_SUSPEND;
}


//==============================================================================
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief �b�������Ώۂ̃|�P�������샂�f���擾
 */
//--------------------------------------------------------------
static MMDL * getPokeMMdl( SCRCMD_WORK * work, u16 obj_id )
{
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  MMDLSYS *mmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
  MMDL *mmdl = MMDLSYS_SearchOBJID( mmdlsys, obj_id );
  GF_ASSERT( mmdl != NULL );
  return mmdl;
}

//--------------------------------------------------------------
/**
 * @brief �b�������Ώۂ�SYMBOL_POKEMON�f�[�^���擾
 */
//--------------------------------------------------------------
static void getFrontSymPoke( SCRCMD_WORK * work, SYMBOL_POKEMON * sympoke )
{
  SCRIPT_WORK * sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( work );
  u16 obj_id = SCRIPT_GetEventWorkValue( sc, gamedata, SCWK_TARGET_OBJID );
  MMDL * mmdl = getPokeMMdl( work, obj_id );
  SYMBOLPOKE_GetParam( sympoke, mmdl );
}

//--------------------------------------------------------------
/**
 * @brief �V���{���|�P�����f�[�^����|�P��������
 */
//--------------------------------------------------------------
static POKEMON_PARAM * createPokemon( SCRCMD_WORK * work, MMDL * mmdl, HEAPID heapID, SYMBOL_POKEMON *sympoke )
{
    GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( work );
    POKEMON_PARAM * pp;
    SYMBOLPOKE_GetParam( sympoke, mmdl );
    pp = SYMBOLPOKE_PP_Create( HEAPID_PROC, gamedata, sympoke );
    return pp;
}

//--------------------------------------------------------------
/**
 * @brief �V���{���|�P�����̏󋵂��X�V���ꂽ���Ƃ�ʒm����
 */
//--------------------------------------------------------------
static void sendDataChange( SCRCMD_WORK * work )
{
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(gamedata);
  SYMBOL_SAVE_WORK *symbol_save = SymbolSave_GetSymbolData(sv_ctrl);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr( gsys );

  if ( 1 )//IntrudeSymbol_CheckIntrudeNetID( game_comm, gamedata ) != INTRUDE_NETID_NULL )
  {
    INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect( game_comm );
    if ( intcomm == NULL )
    { //�ʐM�G���[
      return;
    }
    IntrudeSymbol_SendSymbolDataChange( intcomm, GAMEDATA_GetSymbolMapID( gamedata ) );
    OS_TPrintf( "���M���N�G�X�g�F�V���{���|�P�����X�V\n" );
  }
}





