//==============================================================================
/**
 *
 *@file		heapDTCM.c
 *@brief	�q�[�v�̈�Ǘ�(�X�^�b�N�G���A)
 *
 * Description:  DTCM�G���A�̎g�p�ݒ�����܂��B
 * 				�̈敝�����Ȃ��̂Łi16k=0x4000�j�A
 * 				���C���A���[�i���g�p����heap.c�Ƃ͈قȂ�A
 * 				�P��q�[�v�݂̂̊ȈՓI�ȍ��ɂ��Ă���܂��B
 * 				���������Ɏg�p�T�C�Y�̐ݒ���s���܂���
 * 				��{�I�ɃX�^�b�N����ъ֐����������Ɋ��蓖�Ă���G���A�Ȃ̂�
 * 				�傫�ȃT�C�Y�͍T����悤�ɂ��Ă��������B
 */
//==============================================================================
#include <nitro.h>
#include <nnsys.h>
#include "heapDTCM.h"

//----------------------------------------------------------------
/**
 *	�萔
 */
//----------------------------------------------------------------
#define INVALID_HANDLE_IDX	(255)			// �q�[�v�n���h�������l
#define AREA_MAX_SIZE		(0x3000 - 0x80)	// �ő�0x3000+�A���[�i�w�b�_

//----------------------------------------------------------------
/**
 *	���[�N��`
 */
//----------------------------------------------------------------
typedef struct {
	NNSFndHeapHandle	handle;
	OSHeapHandle		heaphandle;
	u16					count;
	u16					errorCode;
}HEAP_SYS_DTCM;

//----------------------------------------------------------------
/**
 *	�������u���b�N�w�b�_��`
 *
 *	�g�p���Ȃ�
 */
//----------------------------------------------------------------

//----------------------------------------------------------------
/**
 *	�ϐ��錾
 */
//----------------------------------------------------------------
static HEAP_SYS_DTCM  HeapSysDTCM = { 0 };

//==============================================================
// �v���g�^�C�v�錾
//==============================================================

//------------------------------------------------------------------------------
/**
 * �V�X�e��������
 *
 * @param	size		�g�p�T�C�Y			
 *
 * @retval  BOOL		TRUE�Ő����^FALSE�Ŏ��s
 */
//------------------------------------------------------------------------------
BOOL
	GFI_HEAP_DTCM_Init
		( u32 size )
{
	void	*memLo, *memHi, *mem;

	//�T�C�Y����i����AREA_MAX_SIZE�ȏ�͊m�ۂł��Ȃ��悤�ɂ���j
	if( size > AREA_MAX_SIZE ){
		return FALSE;
	}
	//�A���[�i�̏���擾
	memLo = OS_GetDTCMArenaLo();
	//�A���[�i�̉����ݒ�
	OS_SetDTCMArenaHi( (void*)( HW_DTCM_END - 0x1000 ) );

	memHi = (void*)((u32)memLo + size);
	mem = OS_InitAlloc( OS_ARENA_DTCM, memLo, memHi, 1 );

	//�A���[�i�̏���Đݒ�
	OS_SetDTCMArenaLo( mem );

	HeapSysDTCM.heaphandle = OS_CreateHeap( OS_ARENA_DTCM, memLo, memHi );
	if( (int)HeapSysDTCM.heaphandle == -1 ){
		return FALSE;
	}
	return TRUE;
}


//------------------------------------------------------------------------------
/**
 * �V�X�e���I��
 *
 * @retval  BOOL		TRUE�Ő����^FALSE�Ŏ��s
 */
//------------------------------------------------------------------------------
BOOL
	GFI_HEAP_DTCM_Exit
		( void )
{
	OS_DestroyHeap( OS_ARENA_DTCM, HeapSysDTCM.heaphandle );
	OS_ClearAlloc( OS_ARENA_DTCM );

	return TRUE;
}


//------------------------------------------------------------------
/**
 * �q�[�v���烁�������m�ۂ���
 *
 * @param   size		�m�ۃT�C�Y
 *
 * @retval  void*		�m�ۂ����̈�A�h���X�i���s�Ȃ�NULL�j
 *						errorCode�F���s����
 */
//------------------------------------------------------------------
void*
	GFI_HEAP_DTCM_AllocMemory
		( u32 size )
{
	return OS_AllocFromHeap( OS_ARENA_DTCM, HeapSysDTCM.heaphandle, size );
}


//------------------------------------------------------------------
/**
 * �q�[�v���烁�������������
 *
 * @param   memory		�m�ۂ����������A�h���X
 *
 * @retval  BOOL		TRUE�ŉ�������^FALSE�Ŏ��s
 *						errorCode�F���s����
 */
//------------------------------------------------------------------
BOOL
	GFI_HEAP_DTCM_FreeMemory
		( void* memory )
{
	OS_FreeToHeap( OS_ARENA_DTCM, HeapSysDTCM.heaphandle, memory );
	return TRUE;
}






