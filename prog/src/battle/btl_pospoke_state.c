//=============================================================================================
/**
 * @file  btl_pospoke_state.c
 * @brief �|�P����WB �o�g���V�X�e��  ��ɂ���|�P�����̐����m�F�p�\���̂Ə������[�`��
 * @author  taya
 *
 * @date  2009.07.27  �쐬
 */
//=============================================================================================


#include "btl_pospoke_state.h"


void BTL_POSPOKE_InitWork( BTL_POSPOKE_WORK* wk, const BTL_POKE_CONTAINER* pokeCon, BtlRule rule )
{

}
void BTL_POSPOKE_Compair( const BTL_POSPOKE_WORK* wk1, const BTL_POSPOKE_WORK* wk2, BTL_POSPOKE_COMPAIR_RESULT* result )
{

}
void BTL_POSPOKE_CopyWork( const BTL_POSPOKE_WORK* src, BTL_POSPOKE_WORK* dst )
{
  GFL_STD_MemCopy( src, dst, sizeof(BTL_POSPOKE_WORK) );
}


