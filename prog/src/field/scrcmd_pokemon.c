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
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_pokemon.h"

#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"

#include "poke_tool/status_rcv.h" //PokeParty_RecoverAll

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================

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






