//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		nhttp_rap_evilcheck.c
 * @brief	  nhttp�ŕs���`�F�b�N����Ƃ��̃��X�|���X�R�[�h�Ȃ�
 *	@author	Toru=Nagihashi
 *	@data		2010.02.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>
#include <nitro/crypto.h>
#include <nitro/crypto/sign.h>


//�O�����J
#include "net/nhttp_rap_evilcheck.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define NHTTP_RAP_HEAPID_NONE   (0xFFFF)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//CRYPTO���C�u�����v�Z�ŕK�v��Alloc�֐��̂��߃q�[�vID
static HEAPID s_heapID  = NHTTP_RAP_HEAPID_NONE;

//=============================================================================
/**
 *					PRIVATE
*/
//=============================================================================
static void * Nhttp_Rap_EvilCheck_Alloc( u32 size );
static void Nhttp_Rap_EvilCheck_Free( void *p_adrs );

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����s������ �X�e�[�^�X�R�[�h���擾
 * @param   cp_data  NHTTP_RAP_GetRecvBuffer�̒��g
 *
 * @retval  �X�e�[�^�X�R�[�h  �O�Ȃ�ΐ���  �P�Ȃ�Ύ��s
 */
//------------------------------------------------------------------------------
u8 NHTTP_RAP_EVILCHECK_GetStatusCode( const void *cp_data )
{ 
  //�o�C�g�ʒu�O�@�T�C�Y�Pbyte
  return *((const u8*)cp_data);
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����s������ �|�P�����s�������̌���
 * @param   cp_data     NHTTP_RAP_GetRecvBuffer�̒��g
 * @param   poke_index  �s���`�F�b�N�̂Ƃ��ɑ������|�P�����̈ʒu
 *
 * @retval  �s�������̌���  �iNHTTP_RAP_EVILCHECK_RESULT�̍��ڎQ�Ɓj
 */
//------------------------------------------------------------------------------
NHTTP_RAP_EVILCHECK_RESULT NHTTP_RAP_EVILCHECK_GetPokeResult( const void *cp_data, int poke_index )
{ 
  //�o�C�g�ʒu 1+poke_index*4 �T�C�Y4
  const u8 *p_result  = (((const u8*)cp_data)+1+poke_index*4);

  //�|�P�����`�F�b�N�̒l�̓l�b�g���[�N�o�C�g�I�[�_�[�ŋA���Ă���̂�
  //���g���G���f�B�A���ɒ���
  return (u32)( (p_result[0] << 24) | (p_result[1] << 16) | (p_result[2] << 8) | ( p_result[3] << 0 ) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����s������  �������擾
 *
 *  @param    cp_data     NHTTP_RAP_GetRecvBuffer�̒��g
 *  @param    poke_max    �s���`�F�b�N�̂Ƃ��ɑ������|�P�����̍ő吔
 *
 *	@return �����ւ̃A�h���X  �R�s�[����ꍇ�A��������NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN���Amemcpy���Ă�������
 */
//-----------------------------------------------------------------------------
const s8 * NHTTP_RAP_EVILCHECK_GetSign( const void *cp_data, int poke_max )
{ 
  //�o�C�g�ʒu 1+4*(poke_max) �T�C�Y128byte
  return (const s8*)(((const u8*)cp_data)+1+poke_max*4);
}


//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����s������  �����ƃ|�P��������v���Ă��邱�Ƃ��ؖ�����
 *	        �����Ōv�Z�̂��ߍō���4kbyte���x�̃��������g�p����
 *
 *	@param	const void *p_poke_buff �|�P�����f�[�^���i�[����Ă���o�b�t�@
 *	@param	poke_size               �s���`�F�b�N�̎��ɑ������P�̂�����̃T�C�Y
 *	@param	poke_max                �s���`�F�b�N�̎��ɑ������|�P�����̍ő吔
 *	@param	s8 *cp_sign             �s���`�F�b�N�Ŏ󂯎�����T�C��
 *	@param  HEAPID                  �v�Z�̂��߂Ɏg�p����q�[�v
 *
 *	@return TRUE��v���Ă���  FALSE��v���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL NHTTP_RAP_EVILCHECK_VerifySign( const void *p_poke_buff, int poke_size, int poke_max, const s8 *cp_sign, HEAPID heapID )
{ 
  // ���J��
  static unsigned char publicKey[ NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN ] = 
  {
    0xD9, 0x87, 0xD4, 0x65, 0xE4, 0xEE, 0xAE, 0x58,
    0x2D, 0x01, 0x73, 0x15, 0xF0, 0x0E, 0xA3, 0x40,
    0x0C, 0x51, 0x0B, 0x2E, 0x51, 0xE1, 0x5D, 0x77,
    0xD0, 0x3A, 0xDC, 0xB2, 0x5C, 0x83, 0x01, 0x71,
    0xF5, 0x69, 0xFB, 0xD2, 0x6A, 0x78, 0xDC, 0x69,
    0x69, 0x4D, 0xDD, 0x2C, 0xEF, 0xA4, 0xA9, 0xAA,
    0xD1, 0xA0, 0xD9, 0xAA, 0x99, 0x70, 0x5B, 0xF0,
    0x80, 0x38, 0xF5, 0x77, 0x64, 0xEE, 0xA5, 0xAB,
    0x7D, 0x6A, 0x38, 0x38, 0x67, 0x8A, 0xEC, 0x26,
    0x2E, 0x95, 0x2A, 0x1C, 0xDB, 0xB8, 0xE2, 0xFF,
    0x68, 0xDC, 0x93, 0x2E, 0x7F, 0x8E, 0x3A, 0xEC,
    0xD1, 0xFE, 0x52, 0x82, 0xEA, 0xCA, 0x41, 0x61,
    0xC2, 0x20, 0x3F, 0xF0, 0x98, 0xF7, 0x9D, 0x67,
    0x35, 0xE6, 0x44, 0x14, 0xE1, 0x85, 0xFB, 0xB3,
    0xEC, 0x04, 0x3D, 0x83, 0x8D, 0x9B, 0x4B, 0x19,
    0x07, 0x23, 0x31, 0xC3, 0xF7, 0x98, 0x57, 0xE5
  };

  BOOL ret;

  s_heapID  = heapID;

  CRYPTO_SetMemAllocator(
      Nhttp_Rap_EvilCheck_Alloc,
      Nhttp_Rap_EvilCheck_Free,
      NULL  //ReAlloc��RSA�Í����@�\�̂Ƃ������g��Ȃ�(�d�q�����ł͎g��Ȃ�)
      );


  ret = CRYPTO_VerifySignature( p_poke_buff, poke_size * poke_max, cp_sign, publicKey );

  s_heapID  = NHTTP_RAP_HEAPID_NONE;

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����ؖ��̂��߂̃o�b�t�@���쐬����
 *	        NHTTP_RAP_PokemonEvilCheckCreate�͑��M�o�b�t�@�̍쐬�ł����A
 *	        ������́A�ؖ��̂��߂�POKEPARTY����NHTTP_RAP_EVILCHECK_VerifySign�Ŏg�p����o�b�t�@��
 *	        �쐬���邽�߂Ɏg���܂�
 *
 *	@param	poke_size               �s���`�F�b�N�̎��ɑ������P�̂�����̃T�C�Y
 *	@param	poke_max                �s���`�F�b�N�̎��ɑ������|�P�����̍ő吔
 *	@param	HEAPID heapID �o�b�t�@�쐬�̂��߂̃q�[�vID
 *
 *	@return �o�b�t�@
 */
//-----------------------------------------------------------------------------
void * NHTTP_RAP_EVILCHECK_CreateVerifyPokeBuffer( int poke_size, int poke_max, HEAPID heapID )
{
  const u32 size  = poke_max * poke_size;
  void *p_buff;
  p_buff  = GFL_HEAP_AllocMemory( heapID, size );
  GFL_STD_MemClear( p_buff, size );
  return p_buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����ؖ��̂��߂̃o�b�t�@��j������
 *
 *	@param	void *p_buff �o�b�t�@
 */
//-----------------------------------------------------------------------------
void NHTTP_RAP_EVILCHECK_DeleteVerifyPokeBuffer( void *p_buff )
{ 
  GFL_HEAP_FreeMemory( p_buff );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����ؖ��o�b�t�@�փ|�P�����f�[�^��ǉ�����
 *
 *	@param	void *p_buff            �o�b�t�@
 *	@param	void *cp_data           �|�P�����f�[�^
 *	@param	poke_size               �s���`�F�b�N�̎��ɑ������P�̂�����̃T�C�Y
 *	@param	poke_index              �i�[���鏇�ԃC���f�b�N�X
 */
//-----------------------------------------------------------------------------
void NHTTP_RAP_EVILCHECK_AddPokeVerifyPokeBuffer( void *p_buff, const void *cp_data, int poke_size, int poke_index )
{ 
  const u32 offset  = poke_size * poke_index;
  GFL_STD_MemCopy( cp_data, (((u8*)p_buff) + offset ), poke_size );
}

//----------------------------------------------------------------------------
/**
 *	@brief  CRYPT�v�Z�̂��߂̃A���P�[�g�֐�
 *
 *	@param	u32 size  �T�C�Y
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
static void * Nhttp_Rap_EvilCheck_Alloc( u32 size )
{ 
  void *p_adrs;

  GF_ASSERT_MSG( s_heapID != NHTTP_RAP_HEAPID_NONE, "heapID��ݒ肵�Ă�������\n" );
    
  p_adrs  = GFL_HEAP_AllocMemory( s_heapID, size );

  GF_ASSERT_MSG( p_adrs, "CRYPT alloc failed!! size=%d =restHeap%d \n", size, GFL_HEAP_GetHeapFreeSize(s_heapID) );

  return p_adrs;
}
//----------------------------------------------------------------------------
/**
 *	@brief  CRYPT�v�Z�̂��߂̃t���[�֐�
 *
 *	@param	void *p_adrs  ���[�N
 */
//-----------------------------------------------------------------------------
static void Nhttp_Rap_EvilCheck_Free( void *p_adrs )
{ 
  GFL_HEAP_FreeMemory( p_adrs );
}
