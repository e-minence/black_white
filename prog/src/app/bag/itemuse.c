//============================================================================================
/**
 * @file    itemuse.c
 * @brief   �t�B�[���h�A�C�e���g�p����
 * @author  Hiroyuki Nakamura
 * @date    05.12.12
 */
//============================================================================================
#include "gflib.h"


#include "app/itemuse.h"
#include "itemuse_local.h"



//=============================================================================================
/**
 * @brief �w��̓���g���邩���E�s�������i�[����(itemmenu.h����enum��`ITEMCHECK_ENABLE)
 *
 * @param   param   BAG_PARAM�̃|�C���^
 * @param   type    �g������̎�ށiitemmenu.h����enum��`ITEMCHECK_ENABLE����w��j
 * @param   check   0:�g���Ȃ�  1:�g����
 *
 * @retval  none
 */
//=============================================================================================
void ITEMUSE_SetItemUseCheck( ITEMCHECK_WORK * icwk, ITEMCHECK_ENABLE type, BOOL check )
{
  GF_ASSERT( type<ITEMCHECK_MAX );

  icwk->item_enable |= (check<<type);
}


//=============================================================================================
/**
 * @brief �w��̓���g���邩���E�s�������i�[����(itemmenu.h����enum��`ITEMCHECK_ENABLE)
 *
 * @param   param   BAG_PARAM�̃|�C���^
 * @param   type    �g������̎�ށiitemmenu.h����enum��`ITEMCHECK_ENABLE����w��j
 * @param   check   0:�g���Ȃ�  1:�g����
 *
 * @retval  none
 */
//=============================================================================================
BOOL ITEMUSE_GetItemUseCheck( ITEMCHECK_WORK * icwk, ITEMCHECK_ENABLE type )
{
  GF_ASSERT( type<ITEMCHECK_MAX );
//  return 1;

  return (icwk->item_enable>>type)&1;
}

