//======================================================================
/**
 * @file  scrcmd_password.c
 * @brief  �p�X���[�h������r
 * @author  Nozomu Satio
 *
 */
//======================================================================
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

#include "event_fieldmap_control.h"

#include "scrcmd_password.h"

#include "message.naix"
#include "msg\msg_pms_word06.h"
#include "msg\msg_pms_word07.h"
#include "msg\msg_pms_word08.h"
#include "msg\msg_pms_word09.h"
#include "msg\msg_pms_word10.h"
#include "msg\msg_pms_word11.h"
#include "msg\msg_pms_word12.h"


//--------------------------------------------------------------
/**
 * �񓚃`�F�b�N
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckPassword( VMHANDLE *core, void *wk )
{
  u16 ans_word1;
  u16 ans_word2;
  SCRCMD_WORK*   work     = (SCRCMD_WORK*)wk;
  
  u16 no = SCRCMD_GetVMWorkValue( core, work ); //1 or 2
  u16 word1 = SCRCMD_GetVMWorkValue( core, work );
  u16 word2 = SCRCMD_GetVMWorkValue( core, work );
  u16 *ret = SCRCMD_GetVMWork( core, work );

  //�P�P�r�b�g�}�X�N���āA�����ȒP��R�[�h�ɂ���
  {
    word1 &= 0x7ff;   //11�r�b�g�}�X�N
    word2 &= 0x7ff;   //11�r�b�g�}�X�N
  }
  NOZOMU_Printf("w1=%d w2=%d\n",word1,word2);

  //������r
  switch(no){
  case 1:     //1���
    {
      PMS_WORD pms_word;
      pms_word = PMSW_GetWordNumberByGmmID( NARC_message_pms_word07_dat, pms_word07_35 ); //�u�݂�ȁv
      ans_word1 = pms_word & 0x7ff;   //11�r�b�g�}�X�N
      pms_word = PMSW_GetWordNumberByGmmID( NARC_message_pms_word10_dat, pms_word10_33 ); //�u�n�b�s�[�v
      ans_word2 = pms_word & 0x7ff;   //11�r�b�g�}�X�N
    }
    break;
  case 2:   //2���
    {
      PMS_WORD pms_word;
      pms_word = PMSW_GetWordNumberByGmmID( NARC_message_pms_word10_dat, pms_word10_10 ); //�u���񂽂�v
      ans_word1 = pms_word & 0x7ff;   //11�r�b�g�}�X�N
      pms_word = PMSW_GetWordNumberByGmmID( NARC_message_pms_word12_dat, pms_word12_16 ); //�u������v
      ans_word2 = pms_word & 0x7ff;   //11�r�b�g�}�X�N
    }
    break;
  default:
    GF_ASSERT(0);
    *ret = FALSE;
    return VMCMD_RESULT_CONTINUE;
  }

  NOZOMU_Printf("aw1=%d aw2=%d\n",ans_word1, ans_word2);

  if ( (ans_word1 == word1)&&(ans_word2 == word2) ) *ret = TRUE;
  else *ret = FALSE;
  
  return VMCMD_RESULT_CONTINUE;
}

