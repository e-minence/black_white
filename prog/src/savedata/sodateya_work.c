/////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  ��ĉ����[�N
 * @file   sodateya_work.c
 * @author obata
 * @date   2009.09.24
 *
 */
/////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"
#include "poke_tool/pokeparty.h"
#include "savedata/save_tbl.h"
#include "savedata/save_control.h"
#include "savedata/sodateya_work.h"


//=======================================================================
/**
 * @brief �a�����Ă���|�P������̕��̃f�[�^
 */
//=======================================================================
typedef struct 
{
  POKEMON_PASO_PARAM  pasoParam;  // �p�[�\�i���f�[�^
  u32                 walkCount;  // �����J�E���^
} SODATEYA_POKE_DATA;

//=======================================================================
/**
 * @brief ��ĉ����[�N
 */
//=======================================================================
struct _SODATEYA_WORK
{
  SODATEYA_POKE_DATA pokemon[ SODATE_POKE_MAX ];  // �a���|�P����
  BOOL                                  haveEgg;  // �^�}�S�̗L��
  u8                                eggLayCount;  // �Y���J�E���^
};


//-----------------------------------------------------------------------
/**
 * @brief	��ĉ����[�N�̃T�C�Y�擾
 *
 * @return ��ĉ����[�N�̃T�C�Y
 */
//-----------------------------------------------------------------------
u32 SODATEYA_WORK_GetWorkSize(void)
{
	return sizeof(SODATEYA_WORK);
}

//-----------------------------------------------------------------------
/**
 * @brief	��ĉ����[�N�̏�����
 *
 * @param	sodateya ���������郏�[�N
 */
//-----------------------------------------------------------------------
void SODATEYA_WORK_InitWork( void* work )
{
  SODATEYA_WORK* sodateya = (SODATEYA_WORK*)work;

  // ������
	memset( sodateya, 0, sizeof(SODATEYA_WORK) ); 
	sodateya->haveEgg     = FALSE;
	sodateya->eggLayCount = 0;
}

//-----------------------------------------------------------------------
/**
 * @brief ���[�N�ւ̃|�C���^���擾����
 *
 * @param scw �Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 *
 * @return ��ĉ����[�N�ւ̃|�C���^
 */
//-----------------------------------------------------------------------
SODATEYA_WORK* SODATEYA_WORK_GetSodateyaWork( SAVE_CONTROL_WORK* scw )
{
  return SaveControl_DataPtrGet( scw, GMDATA_ID_SODATEYA_WORK );
}

//-----------------------------------------------------------------------
/**
 * @brief �^�}�S�̗L�����擾����
 *
 * @param sodateya �擾�Ώۃ��[�N
 * 
 * @return �^�}�S������ꍇ TRUE
 */
//-----------------------------------------------------------------------
BOOL SODATEYA_WORK_HaveEgg( SODATEYA_WORK* sodateya )
{
  return sodateya->haveEgg;
}

//-----------------------------------------------------------------------
/**
 * @brief �^�}�S���폜����
 *
 * @param sodateya ����Ώۃ��[�N
 */
//-----------------------------------------------------------------------
void SODATEYA_WORK_DeleteEgg( SODATEYA_WORK* sodateya )
{
  // �^�}�S���폜��, �Y���J�E���^��������
  sodateya->haveEgg     = FALSE;
  sodateya->eggLayCount = 0;
}

//-----------------------------------------------------------------------
/**
 * @brief �^�}�S���󂯎��
 *
 * @param sodateya �^�}�S�����������ĉ�����
 * @param party    �^�}�S��ǉ�����莝���|�P�p�[�e�B�[
 */
//-----------------------------------------------------------------------
void SODATEYA_WORK_AddEggToParty( SODATEYA_WORK* sodateya, POKEPARTY* party )
{
  /*
  POKEMON_PARAM param;

  // TEMP:�^�}�S�̃p�����[�^�v�Z
  PP_Setup( &param, 1, 1, 0 );
  PP_Put( &param, ID_PARA_tamago_flag, 1 );

  // �p�[�e�B�[�ɒǉ�
  PokeParty_Add( party, &param );
  SODATEYA_WORK_DeleteEgg( sodateya );
  */

  // TEMP: �擪�|�P������ǉ�
  POKEMON_PARAM* param;
  param = PokeParty_GetMemberPointer( party, 0 );
  PokeParty_Add( party, param );
  SODATEYA_WORK_DeleteEgg( sodateya );
}

//-----------------------------------------------------------------------
/**
 * @brief �a���|�P�����̐����擾����
 *
 * @param sodateya �擾�Ώۃ��[�N
 * 
 * @return ��ĉ����a�����Ă���|�P�����̐�
 */
//-----------------------------------------------------------------------
u8 SODATEYA_WORK_GetPokemonNum( SODATEYA_WORK* sodateya )
{
  return 0;
}

//-----------------------------------------------------------------------
/**
 * @brief �a���|�P�����̃p�[�\�i���p�����[�^���擾����
 *
 * @param sodateya �擾�Ώۃ��[�N
 * @param poke_id  �p�����[�^�擾�Ώۃ|�P�����̔ԍ�(0 or 1)
 *
 * @return �w��|�P�����̃p�[�\�i���p�����[�^
 */
//-----------------------------------------------------------------------
POKEMON_PASO_PARAM* SODATEYA_WORK_GetPokemonPasoParam( 
    SODATEYA_WORK* sodateya, SODATE_POKE_ID poke_id )
{
  // �p�����[�^�擾�Ώۃ|�P�����ԍ����͈͊O�̏ꍇ
  if( (poke_id < 0) || (SODATE_POKE_MAX <= poke_id) )
  {
    OBATA_Printf( "--------------------------------------------------\n" );
    OBATA_Printf( "error: �p�����[�^�擾�Ώۃ|�P�����ԍ����͈͊O�ł��B\n" );
    OBATA_Printf( "--------------------------------------------------\n" );
    return 0;
  }

  // �w��p�����[�^���擾����
  return &sodateya->pokemon[ poke_id ].pasoParam; }
