//=============================================================================
/**
 * @file  amprotect.c
 * @brief �}�W�R���΍􃉃C�u�����p�\�[�X
 * @author  2010.06.19 mori / GAME FREAK Inc. 
 */
//=============================================================================
#include <gflib.h>

static const u8 *protect_adr = NULL;
void test_protect(void);

void test_protect(void)
{
  // ���̃R�[�h�ɈӖ��͖����i!NULL�ɂȂ�Ȃ��̂�OS_Halt�͎��s����Ȃ��j
  if(protect_adr!=NULL){
    OS_Halt();
  }
  return;
}
