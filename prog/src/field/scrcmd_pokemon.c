//======================================================================
/**
 * @file	scrcmd_pokemon.c
 * @brief	�X�N���v�g�R�}���h�F�|�P�����֘A
 * @date  2009.09.15
 * @author	tamada GAMEFREAK inc.
 *
 * @todo  �|�P���X�`�F�b�N�̊m�F���s���Ă��Ȃ�
 * @todo  status_rcv.c�̖����K���C�����ł�����񕜏������Ăяo��
 */
//======================================================================

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_def.h"
#include "script_local.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_pokemon.h"

#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"

#include "poke_tool/status_rcv.h" //PokeParty_RecoverAll

#include "savedata/box_savedata.h"

#include "event_name_input.h" // EVENT_PartyPokeNameInput

#include "savedata/sodateya_work.h"
#include "sodateya.h" // for POKEMON_EGG_Birth

#include "event_poke_status.h" // for EvCmdPartyPokeSelect

#include "fieldmap.h" 


//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================
static int GetPokeCount_NOT_EGG( const POKEPARTY* party );
static int GetPokeCount_BATTLE_ENABLE( const POKEPARTY* party );
static int GetPokeCount_ONLY_EGG( const POKEPARTY* party );
static int GetPokeCount_ONLY_DAME_EGG( const POKEPARTY* party );

static GMEVENT_RESULT EVENT_FUNC_PokeSelect(GMEVENT * event, int * seq, void * work);

//======================================================================
//  �|�P�����֘A
//======================================================================
//--------------------------------------------------------------
/**
 * �莝���Ƀ|�P���X��Ԃ̃|�P���������邩�H�̃`�F�b�N
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckTemotiPokerus( VMHANDLE * core, void *wk )
{
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  *ret_wk = FALSE;

  {
    GAMEDATA *gamedata = SCRCMD_WORK_GetGameData( wk );
    POKEPARTY * party = GAMEDATA_GetMyPokemon( gamedata );
    u32 max = PokeParty_GetPokeCount( party );
    int idx;

    for (idx = 0; idx < max; idx ++)
    {
      u32 result;
      POKEMON_PARAM * pp = PokeParty_GetMemberPointer( party, idx );
      result = PP_Get( pp, ID_PARA_pokerus, NULL );
      if (result != 0)
      {
        *ret_wk = TRUE;
        break;
      }
    }
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �莝���|�P������
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPokemonRecover( VMHANDLE * core, void *wk )
{
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  POKEPARTY * party = GAMEDATA_GetMyPokemon( gamedata );

  PokeParty_RecoverAll( party );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �莝���|�P������HP�`�F�b�N
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckPokemonHP( VMHANDLE * core, void *wk )
{

  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );  //���ʊi�[���[�N
  u16 pos = SCRCMD_GetVMWorkValue( core, wk );  //�|�P�����̈ʒu

  {
    GAMEDATA *gamedata = SCRCMD_WORK_GetGameData( wk );
    POKEPARTY * party = GAMEDATA_GetMyPokemon( gamedata );
    u32 max = PokeParty_GetPokeCount( party );
    POKEMON_PARAM * pp;
    u16 nowHP, maxHP;

    if (pos >= max)
    {
      GF_ASSERT_MSG(0, "Temoti Pos %d over max(%d)!!\n", pos, max);
      pos = 0;
    }
    pp = PokeParty_GetMemberPointer( party, pos );
    nowHP = PP_Get( pp, ID_PARA_hp, NULL );
    maxHP = PP_Get( pp, ID_PARA_hpmax, NULL );
    *ret_wk = FALSE;
    if (nowHP == maxHP)
    {
      *ret_wk = TRUE;
    }
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �莝���|�P�����̃^�}�S���ǂ����̔���
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckPokemonEgg( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*   work = (SCRCMD_WORK*)wk;
  u16*         ret_wk = SCRCMD_GetVMWork( core, wk );       // ���ʊi�[�惏�[�N
  u16             pos = SCRCMD_GetVMWorkValue( core, wk );  // ����|�P�����w��
  GAMEDATA*     gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*    party = GAMEDATA_GetMyPokemon( gdata );
  int             max = PokeParty_GetPokeCountMax( party );
  u32     tamago_flag = 0;
  POKEMON_PARAM* param = NULL;

  // �|�P�����w��ɑ΂����O����
  if( (pos < 0) || (max <= pos) )
  {
    *ret_wk = FALSE;
    return VMCMD_RESULT_CONTINUE;
  }

  // �^�}�S�t���O�擾
  param       = PokeParty_GetMemberPointer( party, pos );
  tamago_flag = PP_Get( param, ID_PARA_tamago_flag, NULL );

  // ���ʂ��i�[
  if( tamago_flag == 0 ) *ret_wk = FALSE;
  else                   *ret_wk = TRUE;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �莝���|�P�����̂Ȃ��x���擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPokemonFriendValue( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*    work = (SCRCMD_WORK*)wk;
  u16*            ret_wk = SCRCMD_GetVMWork( core, wk );       // ���ʊi�[�惏�[�N
  u16             pos = SCRCMD_GetVMWorkValue( core, wk );  // ����|�P�����w��

  POKEPARTY*    party = GAMEDATA_GetMyPokemon( SCRCMD_WORK_GetGameData(work) );
  int             max = PokeParty_GetPokeCountMax( party );
  u8            friend = 0;
  POKEMON_PARAM* param = NULL;

  // �|�P�����w��ɑ΂����O����
  if( (pos < 0) || (PokeParty_GetPokeCountMax(party) <= pos) )
  {
    *ret_wk = 0;
    return VMCMD_RESULT_CONTINUE;
  }

  // �Ȃ��x�擾
  param       = PokeParty_GetMemberPointer( party, pos );
  if(PP_Get( param, ID_PARA_tamago_flag, NULL ) == TRUE){
    friend = PP_Get( param, ID_PARA_friend, NULL); 
  }

  // ���ʂ��i�[
  *ret_wk = friend;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �莝���|�P�����̂Ȃ��x���Z�b�g
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetPokemonFriendValue( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*    work = (SCRCMD_WORK*)wk;
  u16             pos = SCRCMD_GetVMWorkValue( core, wk );  // ����|�P�����w��
  u16             value = SCRCMD_GetVMWorkValue( core, wk );  // �ݒ�l
  u8              mode = VMGetU8( core );  // �ݒ胂�[�h(0:�Z�b�g,1:����,2:����)

  POKEPARTY*    party = GAMEDATA_GetMyPokemon( SCRCMD_WORK_GetGameData(work) );
  u8            friend = 0;
  POKEMON_PARAM* param = NULL;

  // �|�P�����w��ɑ΂����O����
  if( (pos < 0) || (PokeParty_GetPokeCountMax(party) <= pos) )
  {
    return VMCMD_RESULT_CONTINUE;
  }

  // �Ȃ��x�擾
  param       = PokeParty_GetMemberPointer( party, pos );
  if(PP_Get( param, ID_PARA_tamago_flag, NULL ) == TRUE)  //�^�}�S�Ȃ�Ȃɂ����Ȃ�
  {
    return VMCMD_RESULT_CONTINUE;
  }
  friend = PP_Get( param, ID_PARA_friend, NULL); 

  switch(mode)
  {
  case 0: //�Z�b�g
    if(value > 256){
      friend = 255;
    }else if(value < 0){
      friend = 0;
    }else{
      friend = value;
    }
    break;
  case 1: //���Z
    if((friend + value) > 256) {
      friend = 255;
    }else{
      friend += value;
    }
    break;
  case 2: //���Z
    if(friend < value){
      friend = 0;
    }else{
      friend -= value;
    }
  }
  PP_Put( param, ID_PARA_friend, friend); 
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �莝���|�P�����̐����擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPartyPokeMonsNo( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*    work = (SCRCMD_WORK*)wk;
  u16*          ret_wk = SCRCMD_GetVMWork( core, wk );       // ���ʊi�[�惏�[�N
  u16              pos = SCRCMD_GetVMWorkValue( core, wk );  // ����|�P�����w��
  GAMEDATA*      gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*     party = GAMEDATA_GetMyPokemon( gdata );
  POKEMON_PARAM* param = PokeParty_GetMemberPointer( party, pos );

  *ret_wk = (u16)PP_Get( param, ID_PARA_monsno, NULL );
  OBATA_Printf( "EvCmdGetPartyPokeMonsNo : %d\n", *ret_wk );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �莝���|�P�����̌`��i���o�[���擾����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPartyPokeFormNo( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*    work = (SCRCMD_WORK*)wk;
  u16*          ret_wk = SCRCMD_GetVMWork( core, wk );       // ���ʊi�[�惏�[�N
  u16              pos = SCRCMD_GetVMWorkValue( core, wk );  // ����|�P�����w��
  GAMEDATA*      gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*     party = GAMEDATA_GetMyPokemon( gdata );
  POKEMON_PARAM* param = PokeParty_GetMemberPointer( party, pos );

  *ret_wk = (u16)PP_Get( param, ID_PARA_form_no, NULL );
  OBATA_Printf( "EvCmdGetPartyPokeFormNo : %d\n", *ret_wk );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �莝���|�P�����̐����擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPartyPokeCount( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = (SCRCMD_WORK*)wk;
  u16*       ret_wk = SCRCMD_GetVMWork( core, work );
  u16          mode = SCRCMD_GetVMWorkValue( core, wk );
  GAMEDATA*   gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*  party = GAMEDATA_GetMyPokemon( gdata );
  int           num = 0;

  // �w�肳�ꂽ���[�h�ɉ������|�P���������J�E���g
  switch( mode )
  {
  case POKECOUNT_MODE_TOTAL:
    num = PokeParty_GetPokeCount( party );
    break;
  case POKECOUNT_MODE_NOT_EGG:
    num = GetPokeCount_NOT_EGG( party );
    break;
  case POKECOUNT_MODE_BATTLE_ENABLE:
    num = GetPokeCount_BATTLE_ENABLE( party );
    break;
  case POKECOUNT_MODE_ONLY_EGG:
    num = GetPokeCount_ONLY_EGG( party );
    break;
  case POKECOUNT_MODE_ONLY_DAME_EGG:
    num = GetPokeCount_ONLY_DAME_EGG( party );
    break;
  default:
    num = 0;
    break;
  }

  *ret_wk = (u16)num;
  return VMCMD_RESULT_CONTINUE;
} 

//--------------------------------------------------------------
/**
 * @brief �{�b�N�X���̃|�P���������擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdGetBoxPokeCount( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = (SCRCMD_WORK*)wk;
  u16*       ret_wk = SCRCMD_GetVMWork( core, work );     // �R�}���h��1����
  u16          mode = SCRCMD_GetVMWorkValue( core, wk );  // �R�}���h��2����
  GAMEDATA*   gdata = SCRCMD_WORK_GetGameData( work );
  BOX_MANAGER*  box = GAMEDATA_GetBoxManager( gdata );
  POKEPARTY*  party = GAMEDATA_GetMyPokemon( gdata );
  int           num = 0;

  // �w�肳�ꂽ���[�h�ɉ������|�P���������J�E���g
  switch( mode )
  {
  case POKECOUNT_MODE_TOTAL:  // �^�}�S����
    num = BOXDAT_GetPokeExistCountTotal( box );
    break;
  case POKECOUNT_MODE_NOT_EGG:  // �^�}�S������
  case POKECOUNT_MODE_BATTLE_ENABLE:  // �킦�� = �^�}�S�ȊO
    num = BOXDAT_GetPokeExistCount2Total( box );
    break;
  case POKECOUNT_MODE_ONLY_EGG: // �^�}�S�̂�
    num  = BOXDAT_GetPokeExistCountTotal( box );
    num -= BOXDAT_GetPokeExistCount2Total( box );
    break;
  default:
    num = 0;
    break;
  }

  *ret_wk = (u16)num;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
// �^�}�S�������莝���|�P�����̐����擾����
static int GetPokeCount_NOT_EGG( const POKEPARTY* party )
{
  int i;
  int num = 0;
  int max = PokeParty_GetPokeCount( party );  // �S�|�P������
  POKEMON_PARAM* param;
  u32 tamago_flag;

  // �^�}�S�ȊO�̃|�P���������J�E���g
  for( i=0; i<max; i++ )
  {
    param       = PokeParty_GetMemberPointer( party, i );
    tamago_flag = PP_Get( param, ID_PARA_tamago_flag, NULL );
    if( tamago_flag != TRUE ) num++;
  }
  return num;
}

//--------------------------------------------------------------
// �킦��(�^�}�S�ƕm����������)�|�P���������擾����
static int GetPokeCount_BATTLE_ENABLE( const POKEPARTY* party )
{
  int i;
  int num = 0;
  int max = PokeParty_GetPokeCount( party );  // �S�|�P������
  POKEMON_PARAM* param;
  u32 tamago_flag;
  u32 hp;

  // �킦��|�P���������J�E���g
  for( i=0; i<max; i++ )
  {
    param       = PokeParty_GetMemberPointer( party, i );
    tamago_flag = PP_Get( param, ID_PARA_tamago_flag, NULL );
    hp          = PP_Get( param, ID_PARA_hp, NULL );
    if( ( tamago_flag != TRUE ) && ( 0 < hp ) ) num++;
  }
  return num;
}

//--------------------------------------------------------------
// �^�}�S�̐�(�ʖڃ^�}�S������)���擾����
static int GetPokeCount_ONLY_EGG( const POKEPARTY* party )
{
  int i;
  int num = 0;
  int max = PokeParty_GetPokeCount( party );  // �S�|�P������
  POKEMON_PARAM* param;
  u32 tamago_flag;
  u32 fusei_tamago_flag;

  // �ʖڂ���Ȃ��^�}�S�̐����J�E���g
  for( i=0; i<max; i++ )
  {
    param             = PokeParty_GetMemberPointer( party, i );
    tamago_flag       = PP_Get( param, ID_PARA_tamago_flag, NULL );
    fusei_tamago_flag = PP_Get( param, ID_PARA_fusei_tamago_flag, NULL );
    if( ( tamago_flag == TRUE ) && ( fusei_tamago_flag == FALSE ) ) num++;
  }
  return num;
}

//--------------------------------------------------------------
// �ʖڃ^�}�S�̐����擾����
static int GetPokeCount_ONLY_DAME_EGG( const POKEPARTY* party )
{
  int i;
  int num = 0;
  int max = PokeParty_GetPokeCount( party );  // �S�|�P������
  POKEMON_PARAM* param;
  u32 fusei_tamago_flag;

  // �ʖڃ^�}�S�̐����J�E���g
  for( i=0; i<max; i++ )
  {
    param             = PokeParty_GetMemberPointer( party, i );
    fusei_tamago_flag = PP_Get( param, ID_PARA_fusei_tamago_flag, NULL );
    if( fusei_tamago_flag == TRUE ) num++;
  }
  return num;
}

//--------------------------------------------------------------
/**
 * �莝���|�P�������w�肳�ꂽ�Z���o���Ă��邩�`�F�b�N
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdChkPokeWaza( VMHANDLE *core, void *wk )
{
  POKEMON_PARAM *pp;
  GAMEDATA *gamedata = SCRCMD_WORK_GetGameData( wk );
  POKEPARTY *party = GAMEDATA_GetMyPokemon( gamedata );
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  u16 waza = SCRCMD_GetVMWorkValue( core, wk );
  u16 tno = SCRCMD_GetVMWorkValue( core, wk );
  
  *ret_wk = 0;
  pp = PokeParty_GetMemberPointer( party, tno );
  
  //���܂��`�F�b�N
  if( PP_Get(pp,ID_PARA_tamago_flag,NULL) != 0 ){
    return VMCMD_RESULT_CONTINUE;
  }
  
  //�Z���X�g����`�F�b�N
  if( PP_Get(pp,ID_PARA_waza1,NULL) == waza ||
      PP_Get(pp,ID_PARA_waza2,NULL) == waza ||
      PP_Get(pp,ID_PARA_waza3,NULL) == waza ||
      PP_Get(pp,ID_PARA_waza4,NULL) == waza ){
    *ret_wk = 1;
  }
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �莝���|�P�������w�肳�ꂽ�Z���o���Ă��邩�`�F�b�N�@�莝���|�P�����S��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdChkPokeWazaGroup( VMHANDLE *core, void *wk )
{
  int i,max;
  POKEMON_PARAM *pp;
  GAMEDATA *gamedata = SCRCMD_WORK_GetGameData( wk );
  POKEPARTY *party = GAMEDATA_GetMyPokemon( gamedata );
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  u16 waza = SCRCMD_GetVMWorkValue( core, wk );
  
  max = PokeParty_GetPokeCount( party );

#ifdef DEBUG_ONLY_FOR_kagaya  //test
  *ret_wk = 0;
  return VMCMD_RESULT_CONTINUE;
#endif
  
  for( i = 0, *ret_wk = 6; i < max; i++ ){
    pp = PokeParty_GetMemberPointer( party, i );
    
    //���܂��`�F�b�N
    if( PP_Get(pp,ID_PARA_tamago_flag,NULL) != 0 ){
      continue;
    }
    
    //�Z���X�g����`�F�b�N
    if( PP_Get(pp,ID_PARA_waza1,NULL) == waza ||
        PP_Get(pp,ID_PARA_waza2,NULL) == waza ||
        PP_Get(pp,ID_PARA_waza3,NULL) == waza ||
        PP_Get(pp,ID_PARA_waza4,NULL) == waza ){
      *ret_wk = i;
      break;
    }
  }
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �|�P�������莝���ɒǉ�
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdAddPokemonToParty( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*    gdata = SCRCMD_WORK_GetGameData( work );
  HEAPID     heap_id = SCRCMD_WORK_GetHeapID( work );
  POKEPARTY*   party = GAMEDATA_GetMyPokemon( gdata );
  MYSTATUS*   status = GAMEDATA_GetMyStatus( gdata );
  u16*        ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h��1����
  u16         monsno = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��2����
  u16         formno = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��3����
  u16        tokusei = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��4����
  u16          level = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��5����
  u16         itemno = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��6����
  POKEMON_PARAM*  pp = NULL;

  // �莝���������ς��Ȃ�ǉ����Ȃ�
  if( PokeParty_GetPokeCountMax(party) <= PokeParty_GetPokeCount(party) )
  {
    *ret_wk = FALSE;
    return VMCMD_RESULT_CONTINUE;
  }

  // �ǉ�����|�P�������쐬
  pp = PP_Create( monsno, level, PTL_SETUP_ID_AUTO, heap_id );
  PP_Put( pp, ID_PARA_form_no, formno );    // �t�H�[��
  PP_Put( pp, ID_PARA_speabino, tokusei );  // ����
  PP_Put( pp, ID_PARA_item, itemno );       // �����A�C�e��
  { // �e�̖��O
    STRBUF* buf = MyStatus_CreateNameString( status, heap_id );
    PP_Put( pp, ID_PARA_oyaname, (u32)buf );
    GFL_STR_DeleteBuffer( buf );
  }

  // �莝���ɒǉ�
  PokeParty_Add( party, pp );

  GFL_HEAP_FreeMemory( pp );
  *ret_wk = TRUE;
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * �莝���|�P�����̖��O���͉�ʂ��Ăяo��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdPartyPokeNameInput( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*   scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  u16*        ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h��1����
  u16          index = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��2����

  // �C�x���g���Ăяo��
  SCRIPT_CallEvent( scw, EVENT_NameInput_PartyPoke(gsys, ret_wk, index) );
  return VMCMD_RESULT_SUSPEND;
}


//--------------------------------------------------------------
/**
 * �莝���̃^�}�S��z��������
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdPartyPokeEggBirth( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h��1����
  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  HEAPID          heap_id = FIELDMAP_GetHeapID( fieldmap );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  MYSTATUS*            my = GAMEDATA_GetMyStatus( gdata );
  POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
  int          poke_count = PokeParty_GetPokeCount( party );

  for( i=0; i<poke_count; i++ )
  {
    POKEMON_PARAM* param = PokeParty_GetMemberPointer( party, i );
    u32      tamago_flag = PP_Get( param, ID_PARA_tamago_flag, NULL );
    if( tamago_flag == TRUE )
    {
      POKEMON_EGG_Birth( param, my, heap_id );
      *ret_wk = i;
      break;
    }
  } 
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * �|�P������I������
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPartyPokeSelect( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*         ret_decide = SCRCMD_GetVMWork( core, work );       // �R�}���h��1����
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h��2����
  u16               value = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��3����(�\���l)
  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  
  {
    GMEVENT *event = EVENT_CreatePokeSelect( gsys , fieldmap , ret_decide , ret_wk );
    SCRIPT_CallEvent( scw, event );
  }
  
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �莝���|�P�����̋Z���擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPokemonWazaNum( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h��1����
  u16                 pos = SCRCMD_GetVMWorkValue( core, work );       // �R�}���h��2����

  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
  int          waza_count = 0;
  
  POKEMON_PARAM *pp = PokeParty_GetMemberPointer( party , pos );
  for( i=0; i<PTL_WAZA_MAX; i++ )
  {
    const u32 wazaNo = PP_Get( pp , ID_PARA_waza1+i , NULL );
    if( wazaNo != 0 )
    {
      waza_count++;
    }
  }
  *ret_wk = waza_count;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �Z��I������
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPartyPokeWazaSelect( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*         ret_decide = SCRCMD_GetVMWork( core, work );       // �R�}���h��1����
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h��2����
  u16             pokePos = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��3����
  u16               value = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��4����(�\���l)
  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  
  {
    GMEVENT *event = EVENT_CreateWazaSelect( gsys , fieldmap , pokePos , ret_decide , ret_wk );
    SCRIPT_CallEvent( scw, event );
  }
  
  return VMCMD_RESULT_SUSPEND;
}


//--------------------------------------------------------------
/**
 * �ZID�̎擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPokemonWazaID( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h��1����
  u16            poke_pos = SCRCMD_GetVMWorkValue( core, work );       // �R�}���h��2����
  u16            waza_pos = SCRCMD_GetVMWorkValue( core, work );       // �R�}���h��3����

  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
  
  const POKEMON_PARAM *pp = PokeParty_GetMemberPointer( party , poke_pos );
  const u32 wazaNo = PP_Get( pp , ID_PARA_waza1+waza_pos , NULL );
  *ret_wk = wazaNo;
  return VMCMD_RESULT_CONTINUE;
  
}


//--------------------------------------------------------------
/**
 * �Z�̒ǉ�
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetPokemonWaza( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16            poke_pos = SCRCMD_GetVMWorkValue( core, work );       // �R�}���h��2����
  u16            waza_pos = SCRCMD_GetVMWorkValue( core, work );       // �R�}���h��3����
  u16             waza_id = SCRCMD_GetVMWorkValue( core, work );       // �R�}���h��3����

  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
  
  POKEMON_PARAM *pp = PokeParty_GetMemberPointer( party , poke_pos );
  BOOL fastMode = PP_FastModeOn( pp );
  if( waza_pos > PTL_WAZA_MAX )
  {
    //�����o�����[�h
    PP_SetWazaPush( pp , waza_id );
  }
  else
  if( waza_id == 0 )
  {
    //�Y�ꃂ�[�h
    u8 i;
    u8 forgetPos = waza_pos;
    if( waza_pos < PTL_WAZA_MAX-1 )
    {
      //�l�߂�
      for( i=waza_pos ; i<PTL_WAZA_MAX-1 ; i++ )
      {
        const u32 temp_waza_no = PP_Get( pp , ID_PARA_waza1+i+1 , NULL );
        forgetPos = i+1;
        if( temp_waza_no != 0 )
        {
          const u32 temp_waza_cnt = PP_Get( pp , ID_PARA_pp_count1+i+1 , NULL );
          const u32 temp_waza_pp = PP_Get( pp , ID_PARA_pp1+i+1 , NULL );
          PP_Put( pp , ID_PARA_waza1+i , temp_waza_no );
          PP_Put( pp , ID_PARA_pp_count1+i , temp_waza_cnt );
          PP_Put( pp , ID_PARA_pp1+i , temp_waza_pp );
        }
        else
        {
          break;
        }
      }
    }
    PP_Put( pp , ID_PARA_waza1+forgetPos , 0 );
  }
  else
  {
    //�u���������[�h
    PP_SetWazaPos( pp , waza_id , waza_pos );
    
  }
  PP_FastModeOff( pp , fastMode );
  return VMCMD_RESULT_CONTINUE;
  
}


