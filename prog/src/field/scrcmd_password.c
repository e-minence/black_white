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

#include "../../../resource/quiz/pms_answer.h"

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

  OS_Printf("w1=%d w2=%d\n",word1,word2);

  //������r
  switch(no){
  case 1: //1���
    {
      PMS_WORD pms_word;
      pms_word = password01; //�u�݂�ȁv
      ans_word1 = pms_word & 0x7ff;   //11�r�b�g�}�X�N
      pms_word = password02; //�u�n�b�s�[�v
      ans_word2 = pms_word & 0x7ff;   //11�r�b�g�}�X�N
    }
    break;
  case 2: //2���
    {
      PMS_WORD pms_word;
      pms_word = password03; //�u���񂽂�v
      ans_word1 = pms_word & 0x7ff;   //11�r�b�g�}�X�N
      pms_word = password04; //�u������v
      ans_word2 = pms_word & 0x7ff;   //11�r�b�g�}�X�N
    }
    break;
  default:
    GF_ASSERT(0);
    *ret = FALSE;
    return VMCMD_RESULT_CONTINUE;
  }

  OS_Printf("aw1=%d aw2=%d\n",ans_word1, ans_word2);

  if ( (ans_word1 == word1)&&(ans_word2 == word2) ) *ret = TRUE;
  else *ret = FALSE;
  
  return VMCMD_RESULT_CONTINUE;
}

