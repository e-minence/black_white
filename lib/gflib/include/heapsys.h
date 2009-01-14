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

#include <gf_standard.h>
#include <heap.h>	//�����̒��̊֐��𒼐ڌĂяo���̂͋֎~

#ifdef __cplusplus
extern "C" {
#endif

#define HEAPSYS_DEBUG

//==============================================================================
/**
 *
 *
 *
 *
 *	���C���A���[�i�Ǘ�
 *
 *
 *
 *
 */
//==============================================================================
//----------------------------------------------------------------
/**
 *	�萔
 */
//----------------------------------------------------------------
#define GFL_HEAPID_SYSTEM	(0)	// ��{�q�[�v�h�c�iINDEX:0 �͕K���V�X�e����`�ɂ��邱�Ɓj

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
static inline  u16 HeapGetLow( HEAPID heapID )	
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
	GFL_HEAP_Init
		(const HEAP_INIT_HEADER* header, u32 parentHeapMax, u32 totalHeapMax, u32 startOffset);

//------------------------------------------------------------------------------
/**
 * �V�X�e���I��
 */
//------------------------------------------------------------------------------
extern void
	GFL_HEAP_Exit
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
		( HEAPID parentHeapID, HEAPID childHeapID, u32 size );

inline void 
	GFL_HEAP_CreateHeapLo
		( HEAPID parentHeapID, HEAPID childHeapID, u32 size )
{
	GFL_HEAP_CreateHeap( parentHeapID, GetHeapLowID(childHeapID), size );
}

//------------------------------------------------------------------
/**
 * �q�[�v�j��
 *
 * @param   childHeapID			�j������q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 */
//------------------------------------------------------------------
extern void
	GFL_HEAP_DeleteHeap
		( HEAPID childHeapID );

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
		( HEAPID heapID, u32 size );

#define GFL_HEAP_AllocMemory( heapID, size ) \
			GFL_HEAP_AllocMemoryblock( heapID, size )

#define GFL_HEAP_AllocMemoryLo( heapID, size )	\
			GFL_HEAP_AllocMemoryblock( GetHeapLowID(heapID), size )

#else

extern void*
	GFL_HEAP_AllocMemoryblock	//���̊֐��𒼐ڌĂяo���̂͋֎~
		( HEAPID heapID, u32 size, const char* filename, u16 linenum );

#define GFL_HEAP_AllocMemory( heapID, size )	\
			GFL_HEAP_AllocMemoryblock( heapID, size, __FILE__, __LINE__)

#define GFL_HEAP_AllocMemoryLo( heapID, size )	\
			GFL_HEAP_AllocMemoryblock( GetHeapLowID(heapID), size, __FILE__, __LINE__)

#endif

inline  void*
	GFL_HEAP_AllocClearMemoryInline
		( HEAPID heapID, u32 size, const char* filename, u16 linenum )
{
	void* memory = GFL_HEAP_AllocMemoryblock( heapID, size,filename, linenum );
	GFL_STD_MemClear32( memory, size );
	return memory;
}

#define GFL_HEAP_AllocClearMemory( heapID, size )	\
			GFL_HEAP_AllocClearMemoryInline( heapID, size, __FILE__, __LINE__)

inline  void*
	GFL_HEAP_AllocClearMemoryLo
		( HEAPID heapID, u32 size )
{
	void* memory = GFL_HEAP_AllocMemory( GetHeapLowID(heapID), size );
	GFL_STD_MemClear32( memory, size );
	return memory;
}

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
		( NNSFndAllocator* pAllocator, HEAPID heapID, s32 alignment );

//------------------------------------------------------------------
/**
 * �m�ۂ����������u���b�N�̃T�C�Y��ύX����B
 *
 * @param   memory		�������u���b�N�|�C���^
 * @param   newSize		�ύX��̃T�C�Y�i�o�C�g�P�ʁj
 */
//------------------------------------------------------------------
extern void
	GFL_HEAP_ResizeMemory
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
		( HEAPID heapID );

//------------------------------------------------------------------
/**
 * �q�[�v�̈悪�j�󂳂�Ă��Ȃ����`�F�b�N
 *
 * @param   heapID	�q�[�vID�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 */
//------------------------------------------------------------------
extern void
	GFL_HEAP_CheckHeapSafe
		( HEAPID heapID );

#ifdef HEAPSYS_DEBUG
//------------------------------------------------------------------
//------------------------------------------------------------------
/*
 * 	�q�[�v���擾�i�f�o�b�O���̂ݗL���j
 */
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
/**
 * ������������̏󋵂��`�F�b�N
 *
 * @param   heapID		
 * ������̈悪����΂b�o�t��~
 */
//------------------------------------------------------------------
extern void GFL_HEAP_DEBUG_PrintUnreleasedMemoryCheck ( HEAPID heapID );
//------------------------------------------------------------------
/**
 * ����q�[�v�̑S�������u���b�N����\��
 *
 * @param   heapID				�q�[�vID
 */
//------------------------------------------------------------------
extern void GFL_HEAP_DEBUG_PrintExistMemoryBlocks ( HEAPID heapID );
//------------------------------------------------------------------
/**
 * �q�[�v����m�ۂ����������u���b�N�̎��T�C�Y�擾�i�f�o�b�O�p�j
 *
 * @param   memBlock		
 *
 * @retval  u32		�������u���b�N�T�C�Y
 */
//------------------------------------------------------------------
u32 GFL_HEAP_DEBUG_GetMemoryBlockSize ( const void* memory );


extern void GFL_HEAP_DEBUG_StartPrint( HEAPID heapID );
extern void GFL_HEAP_DEBUG_EndPrint( void );

#endif




//==============================================================================
/**
 *
 *
 *
 *
 *	�c�s�b�l�Ǘ�
 *				DTCM�G���A�̎g�p�ݒ�����܂��B
 * 				�̈敝�����Ȃ��̂Łi16k=0x4000�j�A
 * 				���C���A���[�i���g�p����heap.c�Ƃ͈قȂ�A
 * 				�P��q�[�v�݂̂̊ȈՓI�ȍ��ɂ��Ă���܂��B
 * 				���������Ɏg�p�T�C�Y�̐ݒ���s���܂���
 * 				��{�I�ɃX�^�b�N����ъ֐����������Ɋ��蓖�Ă���G���A�Ȃ̂�
 * 				�傫�ȃT�C�Y�͍T����悤�ɂ��Ă��������B
 * 				�����0x3000-0x80���ő�T�C�Y�Ƃ��Ă��܂��B
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * �V�X�e��������
 *
 * @param	size		�g�p�T�C�Y			
 */
//------------------------------------------------------------------------------
extern void
	GFL_HEAP_DTCM_Init
		( u32 size );

//------------------------------------------------------------------------------
/**
 * �V�X�e���I��
 */
//------------------------------------------------------------------------------
extern void
	GFL_HEAP_DTCM_Exit
		( void );

//------------------------------------------------------------------
/**
 * �q�[�v���烁�������m�ۂ���
 *
 * @param   size		�m�ۃT�C�Y
 *
 * @retval  void*		�m�ۂ����̈�A�h���X�i���s�Ȃ�NULL�j
 */
//------------------------------------------------------------------
extern void*
	GFL_HEAP_DTCM_AllocMemory
		( u32 size );

//------------------------------------------------------------------
/**
 * �q�[�v���烁�������������
 *
 * @param   memory		�m�ۂ����������A�h���X
 */
//------------------------------------------------------------------
extern void
	GFL_HEAP_DTCM_FreeMemory
		( void* memory );










#if 0
typedef struct _HEAP_STATE_STACK	HEAP_STATE_STACK;

#ifdef HEAP_DEBUG
extern HEAP_STATE_STACK*  HSS_Create( HEAPID heapID, u32 stackNum );
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

#ifdef __cplusplus
}/* extern "C" */
#endif


#endif	// __HEAPSYS_H__
