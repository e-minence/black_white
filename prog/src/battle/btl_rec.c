//=============================================================================================
/**
 * @file  btl_rec.c
 * @brief �|�P����WB �o�g���V�X�e�� �^��f�[�^����
 * @author  taya
 *
 * @date  2009.12.06  �쐬
 */
//=============================================================================================


#include "btl_common.h"
#include "btl_rec.h"

enum {
  BTL_REC_SIZE = 4096,
};


struct _BTL_REC {
//  u16  bufSize;
  u16  writePtr;
  u8   fSizeOver;
  u8   dmy;
  u8   buf[ BTL_REC_SIZE ];
};


//=============================================================================================
/**
 * �C���X�^���X����
 *
 * @param   heapID
 *
 * @retval  BTL_REC*
 */
//=============================================================================================
BTL_REC* BTL_REC_Create( HEAPID heapID )
{
  BTL_REC* wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_REC) );
  return wk;
}
//=============================================================================================
/**
 * �C���X�^���X�j��
 *
 * @param   wk
 */
//=============================================================================================
void BTL_REC_Delete( BTL_REC* wk )
{
  GFL_HEAP_FreeMemory( wk );
}

//=============================================================================================
/**
 * �f�[�^��������
 *
 * @param   wk
 * @param   data
 * @param   size
 */
//=============================================================================================
void BTL_REC_Write( BTL_REC* wk, const void* data, u32 size )
{
  if( wk->fSizeOver == FALSE )
  {
    u32 endPtr = wk->writePtr + size;
    if( endPtr <= sizeof(wk->buf) )
    {
      GFL_STD_MemCopy( data, &wk->buf[wk->writePtr], size );
      wk->writePtr = endPtr;
    }
    else
    {
      wk->fSizeOver = TRUE;
    }
  }
}

//=============================================================================================
/**
 * �Ō�܂Ő���ɏ������܂�Ă��邩�`�F�b�N
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_REC_IsCorrect( const BTL_REC* wk )
{
  return !(wk->fSizeOver);
}
