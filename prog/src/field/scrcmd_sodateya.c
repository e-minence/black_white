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
#include "savedata/sodateya_work.h"


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
  GAMEDATA*         gdata = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK*  scw = GAMEDATA_GetSaveControlWork( gdata );
  SODATEYA_WORK* sodateya = SODATEYA_WORK_GetSodateyaWork( scw );

  *ret_wk = (u16)SODATEYA_WORK_HaveEgg( sodateya );
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
  GAMEDATA*         gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
  SAVE_CONTROL_WORK*  scw = GAMEDATA_GetSaveControlWork( gdata );
  SODATEYA_WORK* sodateya = SODATEYA_WORK_GetSodateyaWork( scw );

  SODATEYA_WORK_AddEggToParty( sodateya, party ); 
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
  GAMEDATA*         gdata = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK*  scw = GAMEDATA_GetSaveControlWork( gdata );
  SODATEYA_WORK* sodateya = SODATEYA_WORK_GetSodateyaWork( scw );

  SODATEYA_WORK_DeleteEgg( sodateya );
  OBATA_Printf( "EvCmdDeleteSodateyaEgg\n" );
  return VMCMD_RESULT_CONTINUE;
} 

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
  GAMEDATA*         gdata = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK*  scw = GAMEDATA_GetSaveControlWork( gdata );
  SODATEYA_WORK* sodateya = SODATEYA_WORK_GetSodateyaWork( scw );

  *ret_wk = (u16)SODATEYA_WORK_GetPokemonNum( sodateya );
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
  int i;
  u32 monsno[2];
  u32 id_no[2];
  u32 form[2];
  u32 egg_group1[2];
  u32 egg_group2[2];
  POKEMON_PASO_PARAM* paso_param[2];
  POKEMON_PERSONAL_DATA* personal_data[2];
  SCRCMD_WORK*          work = (SCRCMD_WORK*)wk;
  u16*                ret_wk = SCRCMD_GetVMWork( core, work );
  GAMEDATA*            gdata = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK*     scw = GAMEDATA_GetSaveControlWork( gdata );
  SODATEYA_WORK*    sodateya = SODATEYA_WORK_GetSodateyaWork( scw );
  HEAPID             heap_id = SCRCMD_WORK_GetHeapID( work );

  // 2�̂��ꂼ��̃p�����[�^���擾
  paso_param[0] = SODATEYA_WORK_GetPokemonPasoParam( sodateya, SODATE_POKE_FIRST );
  paso_param[1] = SODATEYA_WORK_GetPokemonPasoParam( sodateya, SODATE_POKE_SECOND );
  for( i=0; i<2; i++ )
  {
    monsno[i] = PPP_Get( paso_param[i], ID_PARA_monsno, NULL );
    id_no[i]  = PPP_Get( paso_param[i], ID_PARA_id_no, NULL );
    form[i]   = PPP_Get( paso_param[i], ID_PARA_form_no, NULL );
    personal_data[i] = POKE_PERSONAL_OpenHandle( monsno[i], form[i], GFL_HEAP_LOWID(heap_id) );
    egg_group1[i] = POKE_PERSONAL_GetParam( personal_data[i], POKEPER_ID_egg_group1 );
    egg_group2[i] = POKE_PERSONAL_GetParam( personal_data[i], POKEPER_ID_egg_group2 );
  }

  // ����������
  // �O���[�v���Ⴄ ==> 0%
  if( ( egg_group1[0] != egg_group1[1] ) &&
      ( egg_group1[0] != egg_group2[1] ) &&
      ( egg_group2[0] != egg_group1[1] ) &&
      ( egg_group2[0] != egg_group2[1] )  )
  {
    *ret_wk = 0;
  }
  // �O���[�v������
  else
  {
    // �|�P�����̎�ނ��Ⴄ
    if( monsno[0] != monsno[1] )
    {
      // ID������ ==> 20%
      if( id_no[0] == id_no[1] )
      {
        *ret_wk = 20;
      }
      // ID���Ⴄ ==> 50%
      else
      {
        *ret_wk = 50;
      }
    }
    // �|�P�����̎�ނ�����
    else
    {
      // ID������ ==> 50%
      if( id_no[0] == id_no[1] )
      {
        *ret_wk = 50;
      }
      // ID���Ⴄ ==> 70%
      else
      {
        *ret_wk = 70;
      }
    }
  }

  return VMCMD_RESULT_CONTINUE;
}
