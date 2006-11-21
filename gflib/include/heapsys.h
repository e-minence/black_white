//==============================================================================
/**
 *
 *@file		heapsys.h
 *@brief	�q�[�v�̈�Ǘ�
 *
 */
//==============================================================================
#ifndef __HEAPSYS_H__
#define __HEAPSYS_H__

#include "heap.h"	//�����̒��̊֐��𒼐ڌĂяo���̂͋֎~

#define HEAPSYS_DEBUG

#if 0	/* ��heap_inter.h ���ŊO���ł��g�p����錾 */
//----------------------------------------------------------------
/**
 *	��{�q�[�v�쐬�p�����[�^�\����
 */
//----------------------------------------------------------------
typedef struct {
	u32        size;		///< �q�[�v�T�C�Y
	OSArenaId  arenaID;		///< �쐬��A���[�iID
}HEAP_INIT_HEADER;

//----------------------------------------------------------------
/**
 *	�C�����C���֐�
 */
//----------------------------------------------------------------
static inline  u32 HeapGetLow( u32 heapID )	
{
	return (( heapID & HEAPID_MASK )|( HEAPDIR_MASK ));
}
#endif

//------------------------------------------------------------------------------
/**
 * �V�X�e���������i�v���O�����N�����ɂP�x�����Ă΂��j
 *
 * @param   header			�e�q�[�v�������\���̂ւ̃|�C���^
 * @param   baseHeapMax		�e�q�[�v����
 * @param   heapMax			�e�q�[�v�E�q�q�[�v���v��
 * @param   startOffset		�q�[�v�̈�J�n�I�t�Z�b�g�i�v�S�o�C�g�A���C���j
 */
//------------------------------------------------------------------------------
extern void
	GFL_HEAP_sysInit
		(const HEAP_INIT_HEADER* header, u32 parentHeapMax, u32 totalHeapMax, u32 startOffset);

//------------------------------------------------------------------------------
/**
 * �V�X�e���I��
 */
//------------------------------------------------------------------------------
extern void
	GFL_HEAP_sysExit
		( void );

//------------------------------------------------------------------
/**
 * �q�[�v�쐬
 *
 * @param   parentHeapID		�������̈�m�ۗp�q�[�v�h�c�i���ɗL���ł���K�v������j
 * @param   childHeapID			�V�K�ɍ쐬����q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 * @param   size				�q�[�v�T�C�Y
 */
//------------------------------------------------------------------
extern void 
	GFL_HEAP_CreateHeap
		( u32 parentHeapID, u32 childHeapID, u32 size );

extern BOOL sys_CreateHeapLo( u32 parentHeapID, u32 childHeapID, u32 size );

//------------------------------------------------------------------
/**
 * �q�[�v�j��
 *
 * @param   childHeapID			�j������q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 */
//------------------------------------------------------------------
extern void
	GFL_HEAP_DeleteHeap
		( u32 childHeapID );

//------------------------------------------------------------------
/**
 * �q�[�v���烁�������m�ۂ���
 *
 * @param   heapID		�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 * @param   size		�m�ۃT�C�Y
 *
 * @retval  void*		�m�ۂ����̈�A�h���X
 *
 *	�e���|�����̈�ȂǁA�����ɉ������̈��
 *	����擾�}�N���i�q�[�v�h�c�����H�j�Ŋm�ۂ���Η̈�̒f�Љ����N����Â炭�Ȃ�B
 *
 *�@���f�o�b�O�r���h���ɂ̓}�N���Ń��b�v���ČĂяo���\�[�X����n���Ă���
 */
//------------------------------------------------------------------
#ifndef HEAPSYS_DEBUG

extern void*
	GFL_HEAP_AllocMemoryblock	//���̊֐��𒼐ڌĂяo���̂͋֎~
		( u32 heapID, u32 size );

#define GFL_HEAP_AllocMemory( ID, siz )		\
			GFL_HEAP_AllocMemoryblock( ID, siz )

#define GFL_HEAP_AllocMemoryLow( ID, siz )	\
			GFL_HEAP_AllocMemoryblock( HeapGetLow(ID), siz )

#else

extern void*
	GFL_HEAP_AllocMemoryblock	//���̊֐��𒼐ڌĂяo���̂͋֎~
		( u32 heapID, u32 size, const char* filename, u32 linenum );

#define GFL_HEAP_AllocMemory( ID, siz )		\
			GFL_HEAP_AllocMemoryblock( ID, siz, __FILE__, __LINE__)

#define GFL_HEAP_AllocMemoryLow( ID, siz )	\
			GFL_HEAP_AllocMemoryblock( HeapGetLow(ID), siz, __FILE__, __LINE__)

#endif

//------------------------------------------------------------------
/**
 * �q�[�v���烁�������������
 *
 * @param   memory		�m�ۂ����������A�h���X
 */
//------------------------------------------------------------------
extern void
	GFL_HEAP_FreeMemoryblock	//���̊֐��𒼐ڌĂяo���̂͋֎~
		( void* memory );

#define GFL_HEAP_FreeMemory( mem )	GFL_HEAP_FreeMemoryblock( mem )

//------------------------------------------------------------------
/**
 * NitroSystem ���C�u�����n�֐����v������A���P�[�^���쐬����
 *
 * @param   pAllocator		NNSFndAllocator�\���̂̃A�h���X
 * @param   heapID			�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 * @param   align			�m�ۂ��郁�����u���b�N�ɓK�p����A���C�����g�i���̒l�͐��̒l�ɕϊ��j
 */
//------------------------------------------------------------------
extern void
	GFL_HEAP_InitAllocator
		( NNSFndAllocator* pAllocator, u32 heapID, s32 alignment );

//------------------------------------------------------------------
/**
 * �m�ۂ����������u���b�N�̃T�C�Y��ύX����B
 *
 * @param   memory		�������u���b�N�|�C���^
 * @param   newSize		�ύX��̃T�C�Y�i�o�C�g�P�ʁj
 */
//------------------------------------------------------------------
extern void
	GFL_HEAP_MemoryResize
		( void* memory, u32 newSize );

//------------------------------------------------------------------
/**
 * �q�[�v�̋󂫗̈�T�C�Y��Ԃ�
 *
 * @param   heapID	�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  u32		�󂫗̈�T�C�Y�i�o�C�g�P�ʁj�G���[����0
 *					errorCode�F���s����
 */
//------------------------------------------------------------------
extern u32
	GFL_HEAP_GetHeapFreeSize
		( u32 heapID );

//------------------------------------------------------------------
/**
 * �q�[�v�̈悪�j�󂳂�Ă��Ȃ����`�F�b�N
 *
 * @param   heapID	�q�[�vID�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 */
//------------------------------------------------------------------
extern void
	GFL_HEAP_CheckHeapSafe
		( u32 heapID );





#if 0
#ifdef HEAP_DEBUG
extern void sys_PrintHeapFreeSize( u32 heapID );
extern void sys_PrintHeapExistMemoryInfo( u32 heapID );
extern u64 sys_GetHeapState( u32 heapID );
extern void sys_PrintHeapConflict( u32 heap, u32 assertionMemSize ); 
extern u32 sys_GetMemoryBlockSize( const void* memBlock );
extern void sys_CheckHeapFullReleased( u32 heapID );
#else
#define sys_PrintHeapFreeSize(h)		/* */
#define sys_PrintHeapExistMemoryInfo(h)	/* */
#define sys_GetHeapState(h)				/* */
#define sys_PrintHeapConflict(h, s)		/* */
#define sys_GetMemoryBlockSize(b)		/* */
#define sys_CheckHeapFullReleased(h)	/* */
#endif


//------------------------------------------------------------------
/*
 * 	�q�[�v���擾�i�f�o�b�O���̂ݗL���j
 */
//------------------------------------------------------------------
typedef struct _HEAP_STATE_STACK	HEAP_STATE_STACK;

#ifdef HEAP_DEBUG
extern HEAP_STATE_STACK*  HSS_Create( u32 heapID, u32 stackNum );
extern void HSS_Push( HEAP_STATE_STACK* hss );
extern void HSS_Pop( HEAP_STATE_STACK* hss );
extern void HSS_Delete( HEAP_STATE_STACK* hss );
#else
#define HSS_Create(a,b,c)	((void)0);
#define HSS_Push(p)			((void)0);
#define HSS_Pop(p)			((void)0);
#define HSS_Delete(p)		((void)0);
#endif

//------------------------------------------------------------------
/*
 * 	�ȒP���������[�N�`�F�b�N�i���݂͖����j
 */
//------------------------------------------------------------------
#define HeapStatePush()		/* */
#define HeapStatePop()		/* */
#define HeapStateCheck(h)	/* */
#endif


#endif	// __HEAPSYS_H__
