//======================================================================
/**
 * @file  scrcmd_sppoke_trio.c
 * @brief  �_���^�j�X�Z�o���֘A�X�N���v�g�֐�
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
#include "scrcmd_sppoke_trio.h"

#include "poke_tool/poke_memo.h"
#include "waza_tool/wazano_def.h"

//--------------------------------------------------------------
/**
 * �莝���̒��̌ŗL�Z���o���邱�Ƃ��ł���_���^�j�X�̈ʒu��Ԃ��֐�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 * @note    �����𖞂����_���^�j�X�����Ȃ��ꍇ�� ret��FALSE���Z�b�g�����
 * @note�@�@�����͔z�z���ꂽ�_���^�j�X�ł��A�ŗL�Z���o���Ă��Ȃ�����
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetEvtDarutanisPos( VMHANDLE *core, void *wk )
{
  int i,num;
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY* party    = GAMEDATA_GetMyPokemon( gdata );
  u16 *poke_pos = SCRCMD_GetVMWork( core, work );
 	u16 *ret = SCRCMD_GetVMWork( core, work );

  int skill_no = WAZANO_KISEKINOTURUGI;

  *ret = FALSE;
  *poke_pos = 0;

  num = PokeParty_GetPokeCount( party );
  //�莝������
  for (i=0;i<num;i++)
  {
    POKEMON_PARAM *pp = PokeParty_GetMemberPointer(party, i);
    //�z�z�`�F�b�N
//    if ( !POKE_MEMO_CheckEventPokePP( pp ,POKE_MEMO_EVENT_DARUTANISU ) ) continue;
    //�����X�^�[�i���o�[�`�F�b�N
    if ( PP_Get( pp, ID_PARA_monsno, NULL ) != MONSNO_DARUTANISU ) continue;
    //�Z�`�F�b�N(���Ɋo���Ă��邩)
    if ( PP_Get( pp, ID_PARA_waza1, NULL ) == skill_no) continue;
    if ( PP_Get( pp, ID_PARA_waza2, NULL ) == skill_no) continue;
    if ( PP_Get( pp, ID_PARA_waza3, NULL ) == skill_no) continue;
    if ( PP_Get( pp, ID_PARA_waza4, NULL ) == skill_no) continue;
    
    *ret = TRUE;
    *poke_pos = i;
    NOZOMU_Printf("�_���^�j�X�����@�ʒu%d\n",i);
    break;
  }
  return VMCMD_RESULT_CONTINUE;
}

