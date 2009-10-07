//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �X�N���v�g�R�}���h�F��ĉ��֘A
 * @file   scrcmd_sodateya.c
 * @author obata
 * @date   2009.09.24
 *
 */
//////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "system/vm_cmd.h"
#include "scrcmd.h"
#include "scrcmd_sodateya.h"
#include "app/pokelist.h"
#include "system/main.h"
#include "script_local.h"
#include "app/p_status.h"
#include "event_fieldmap_control.h"
#include "savedata/sodateya_work.h"
#include "sodateya.h"
#include "fieldmap.h"

FS_EXTERN_OVERLAY(pokelist);


//====================================================================
// ���v���g�^�C�v�錾
//====================================================================
static GMEVENT* EVENT_SodatePokeSelect( 
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, u16* ret_wk );


//====================================================================
// ���^�}�S�֘A
//====================================================================

//--------------------------------------------------------------------
/**
 * @brief ��ĉ��̃|�P�����ɗ������܂ꂽ���ǂ����̃`�F�b�N
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdCheckSodateyaHaveEgg( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SODATEYA*      sodateya = FIELDMAP_GetSodateya( fieldmap );

  *ret_wk = (u16)SODATEYA_HaveEgg( sodateya );
  OBATA_Printf( "EvCmdCheckSodateyaHaveEgg : %d\n", *ret_wk );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief ��ĉ��̃^�}�S���󂯎��
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdGetSodateyaEgg( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*         gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SODATEYA*      sodateya = FIELDMAP_GetSodateya( fieldmap );

  SODATEYA_TakeBackEgg( sodateya, party ); 
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief ��ĉ��̃^�}�S���폜����
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdDeleteSodateyaEgg( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SODATEYA*      sodateya = FIELDMAP_GetSodateya( fieldmap );

  SODATEYA_DeleteEgg( sodateya );
  OBATA_Printf( "EvCmdDeleteSodateyaEgg\n" );
  return VMCMD_RESULT_CONTINUE;
} 


//====================================================================
// ���|�P�����a����E�������
//====================================================================

//--------------------------------------------------------------------
/**
 * @brief �|�P�����̑I����ʂ��Ăяo��
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdSodateyaSelectTemotiPokemon( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*         sw = SCRCMD_WORK_GetScriptWork( work );
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );
  GAMEDATA*         gdata = SCRCMD_WORK_GetGameData( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
  SAVE_CONTROL_WORK*  scw = GAMEDATA_GetSaveControlWork( gdata );
  SODATEYA_WORK* sodateya = SODATEYA_WORK_GetSodateyaWork( scw );

  // �C�x���g�Ăяo��
  {
    GMEVENT* event; 
    event = EVENT_SodatePokeSelect( gsys, fieldmap, ret_wk );
    SCRIPT_CallEvent( sw, event );
  }

  OBATA_Printf( "EvcmdSodateyaSelectTemotiPokemon\n" );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------------
/**
 * @brief ��ĉ��Ƀ|�P������a����
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdTrustPokeToSodateya( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16                 pos = SCRCMD_GetVMWorkValue( core, work );
  GAMEDATA*         gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SODATEYA*      sodateya = FIELDMAP_GetSodateya( fieldmap );

  SODATEYA_TakePokemon( sodateya, party, pos );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief ��ĉ�����|�P�������󂯎��
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdCheckOutPokeFromSodateya( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16                 pos = SCRCMD_GetVMWorkValue( core, work );
  GAMEDATA*         gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SODATEYA*      sodateya = FIELDMAP_GetSodateya( fieldmap );

  SODATEYA_TakeBackPokemon( sodateya, pos, party );
  return VMCMD_RESULT_CONTINUE;
}


//====================================================================
// ���a���|�P�����֘A
//====================================================================

//--------------------------------------------------------------------
/**
 * @breif ��ĉ��ɗa���Ă���|�P�����̐����擾����
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdGetSodateyaPokemonNum( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SODATEYA*      sodateya = FIELDMAP_GetSodateya( fieldmap );

  *ret_wk = (u16)SODATEYA_GetPokemonNum( sodateya );
  OBATA_Printf( "EvCmdGetSodateyaPokemonNum : %d\n", *ret_wk );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @breif �a���|�P����2�̂̑������`�F�b�N����
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdSodateyaLoveCheck( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SODATEYA*      sodateya = FIELDMAP_GetSodateya( fieldmap );

  *ret_wk = (u16)SODATEYA_GetLoveLevel( sodateya );
  return VMCMD_RESULT_CONTINUE;
}



//--------------------------------------------------------------------
/**
 * @brief ��ĉ��ɗa���Ă���|�P�����̃����X�^�[�i���o�[���擾����
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdGetSodateyaPokeMonsNo( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );
  u16                 pos = SCRCMD_GetVMWorkValue( core, work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SODATEYA*      sodateya = FIELDMAP_GetSodateya( fieldmap );
  const POKEMON_PARAM* param = SODATEYA_GetPokemonParam( sodateya, pos );

  *ret_wk = (u16)PP_Get( param, ID_PARA_monsno, NULL );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief ��ĉ��ɗa���Ă���|�P�����̌`��i���o�[���擾����
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdGetSodateyaPokeFormNo( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );
  u16                 pos = SCRCMD_GetVMWorkValue( core, work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SODATEYA*      sodateya = FIELDMAP_GetSodateya( fieldmap );
  const POKEMON_PARAM* param = SODATEYA_GetPokemonParam( sodateya, pos );

  *ret_wk = (u16)PP_Get( param, ID_PARA_form_no, NULL );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief ��ĉ��ɗa���Ă���|�P�����ɂ���, ���ʂ��擾����
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdGetSodateyaPokeSex( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );
  u16                 pos = SCRCMD_GetVMWorkValue( core, work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SODATEYA*      sodateya = FIELDMAP_GetSodateya( fieldmap );
  const POKEMON_PARAM* param = SODATEYA_GetPokemonParam( sodateya, pos );

  *ret_wk = (u16)PP_Get( param, ID_PARA_sex, NULL );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief ��ĉ��ɗa���Ă���|�P�����ɂ���, ���݂̃��x�����擾����
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdGetSodateyaPokeLv( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );
  u16                 pos = SCRCMD_GetVMWorkValue( core, work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SODATEYA*      sodateya = FIELDMAP_GetSodateya( fieldmap );
  const POKEMON_PARAM* param = SODATEYA_GetPokemonParam( sodateya, pos );

  *ret_wk = (u16)SODATEYA_GetPokeLv_Current( sodateya, pos );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief ��ĉ��ɗa���Ă���|�P�����ɂ���, �����������x�����擾����
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdGetSodateyaPokeGrowUpLv( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );
  u16                 pos = SCRCMD_GetVMWorkValue( core, work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SODATEYA*      sodateya = FIELDMAP_GetSodateya( fieldmap );
  const POKEMON_PARAM* param = SODATEYA_GetPokemonParam( sodateya, pos );

  *ret_wk = (u16)SODATEYA_GetPokeLv_GrowUp( sodateya, pos );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief ��ĉ��ɗa���Ă���|�P�����ɂ���, ������藿�����擾����
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdGetSodateyaPokeTakeBackCharge( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );
  u16                 pos = SCRCMD_GetVMWorkValue( core, work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SODATEYA*      sodateya = FIELDMAP_GetSodateya( fieldmap );
  const POKEMON_PARAM* param = SODATEYA_GetPokemonParam( sodateya, pos );

  *ret_wk = (u16)SODATEYA_GetCharge( sodateya, pos );
  return VMCMD_RESULT_CONTINUE;
}



//====================================================================
// ������J�֐�
//====================================================================

//------------------------------------------------------------------
/**
 * @brief ��ă|�P�����I�����[�N
 */
//------------------------------------------------------------------
typedef struct 
{
	GAMESYS_WORK*      gsys;  // �Q�[���V�X�e��
	FIELDMAP_WORK* fieldmap;  // �t�B�[���h�}�b�v
  PLIST_DATA*      plData;  // �|�P�������X�g
  PSTATUS_DATA*    psData;  // �|�P�����X�e�[�^�X
  u16*            retWork;  // �I���|�P�����ԍ��̊i�[�惏�[�N
} POKE_SELECT_WORK;

//------------------------------------------------------------------
/**
 * @brief �C�x���g����֐� (��ă|�P�����I���C�x���g)
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_SodatePokeSelect(GMEVENT * event, int * seq, void * work)
{
	POKE_SELECT_WORK * psw = work;
	GAMESYS_WORK *gsys = psw->gsys;

  // �V�[�P���X�̒�`
  enum
  {
    SEQ_FADE_OUT,
    SEQ_FIELDMAP_CLOSE,
    SEQ_SELECT_POKEMON,
    SEQ_FIELDMAP_OPEN,
    SEQ_FADE_IN,
    SEQ_END,
  };

	switch( *seq ) 
  {
	case SEQ_FADE_OUT: //// �t�F�[�h�A�E�g
		GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, psw->fieldmap, 0));
    *seq = SEQ_FIELDMAP_CLOSE;
		break;
	case SEQ_FIELDMAP_CLOSE: //// �t�B�[���h�}�b�v�I��
		GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, psw->fieldmap));
    psw->plData->ret_sel = psw->psData->pos;  // �J�[�\�������ʒu�Z�b�g
    *seq = SEQ_SELECT_POKEMON;
		break;
  case SEQ_SELECT_POKEMON:
		GMEVENT_CallEvent(event, EVENT_PokeSelect(gsys, psw->fieldmap, psw->plData, psw->psData));
    *seq = SEQ_FIELDMAP_OPEN;
    break;
	case SEQ_FIELDMAP_OPEN: //// �t�B�[���h�}�b�v���A
		GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    *seq = SEQ_FADE_IN;
		break;
	case SEQ_FADE_IN: //// �t�F�[�h�C��
		GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, psw->fieldmap, 0));
    *seq = SEQ_END;
		break;
	case SEQ_END: //// �C�x���g�I��
    *psw->retWork = psw->plData->ret_sel; // �I�����ʂ����[�N�Ɋi�[
    GFL_HEAP_FreeMemory( psw->plData );
    GFL_HEAP_FreeMemory( psw->psData );
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
} 

//------------------------------------------------------------------
/**
 * @brief ��ă|�P�����I���C�x���g
 *
 * @param gsys      �Q�[���V�X�e��
 * @param fieldmap  �t�B�[���h�}�b�v
 * @param ret_wk    �I�����ʂ̊i�[�惏�[�N
 */
//------------------------------------------------------------------
static GMEVENT * EVENT_SodatePokeSelect( 
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, u16* ret_wk )
{
	GMEVENT* event;
	POKE_SELECT_WORK* psw;
  PLIST_DATA* pl_data;
  PSTATUS_DATA* ps_data;
  GAMEDATA*  gdata = GAMESYSTEM_GetGameData( gsys );
  POKEPARTY* party = GAMEDATA_GetMyPokemon( gdata );

  // �|�P�������X�g����
  pl_data    = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(PLIST_DATA) );
  pl_data->pp = party;
  pl_data->mode = PL_MODE_SODATEYA;
  pl_data->type = PL_TYPE_SINGLE;
  pl_data->ret_sel = 0;

  // �|�P�����X�e�[�^�X����
  ps_data = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(PSTATUS_DATA) );
  ps_data->ppd = pl_data->pp;
  ps_data->cfg = pl_data->cfg;
  ps_data->ppt = PST_PP_TYPE_POKEPARTY;
  ps_data->max = PokeParty_GetPokeCount( party );
  ps_data->mode = PST_MODE_NORMAL;
  ps_data->pos = 0;

  // �C�x���g����
  event = GMEVENT_Create(gsys, NULL, EVENT_FUNC_SodatePokeSelect, sizeof(POKE_SELECT_WORK));
  psw   = GMEVENT_GetEventWork(event);
	psw->gsys      = gsys;
	psw->fieldmap  = fieldmap;
  psw->plData    = pl_data;
  psw->psData    = ps_data;
  psw->retWork   = ret_wk;
	return event;
}
