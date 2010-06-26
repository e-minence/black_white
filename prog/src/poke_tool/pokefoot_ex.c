//============================================================================
/**
 *  @file   pokefoot_ex.h
 *  @brief  �|�P�������ՃO���t�B�b�N�f�[�^�擾�֐�
 *  @author Koji Kawada
 *  @data   2010.06.25
 *  @note   
 *
 *  ���W���[�����FPOKEFOOT_EX
 */
//============================================================================
// �C���N���[�h
#include <gflib.h>

#include "poke_tool/monsno_def.h"

#include "poke_tool/pokefoot_ex.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "pokefoot_ex.naix"


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define  POKEFOOT_EX_ARC_CHAR_GIRATHINA_FORM_01  NARC_pokefoot_ex_poke_foot_487_001_NCGR    // 487 �M���e�B�i �t�H����01
#define  POKEFOOT_EX_ARC_CHAR_SHEIMI_FORM_01     NARC_pokefoot_ex_poke_foot_492_001_NCGR    // 492 �V�F�C�~   �t�H����01


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief           �A�[�J�C�u�t�@�C��IDX�擾
 *
 *  @param[in]       none
 *
 *  @retval          �A�[�J�C�u�t�@�C��IDX
 */
//------------------------------------------------------------------
int POKEFOOT_EX_ArcFileGet(void)
{
  return  ARCID_POKEFOOT_EX_GRA;
}

//------------------------------------------------------------------
/**
 *  @brief           �L�����N�^�f�[�^IDX�擾
 *
 *  @param[in]       monsno      monsno
 *  @param[in]       formno      formno
 *
 *  @retval          �L�����N�^�f�[�^IDX
 */
//------------------------------------------------------------------
int POKEFOOT_EX_CharDataIdxGet(int monsno, int formno)
{
  if( monsno == MONSNO_GIRATHINA && formno == FORMNO_GIRATHINA_ORIGIN )
  {
    return  POKEFOOT_EX_ARC_CHAR_GIRATHINA_FORM_01;
  }
  else if( monsno == MONSNO_SHEIMI && formno == FORMNO_SHEIMI_SKY )
  {
    return  POKEFOOT_EX_ARC_CHAR_SHEIMI_FORM_01;
  }
  else
  {
    return  POKEFOOT_EX_CHAR_DATA_IDX_NULL;
  }
}

