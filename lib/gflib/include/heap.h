#ifdef __cplusplus
extern "C" {
#endif
//==============================================================================
/**
 *
 *@file		heap.h
 *@brief	�q�[�v�̈�Ǘ�
 *
 */
//==============================================================================
#ifndef __HEAP_INTER_H__
#define __HEAP_INTER_H__

typedef	u16 HEAPID;
//----------------------------------------------------------------
/**
 *	�萔
 */
//----------------------------------------------------------------
#define DEFAULT_ALIGN				(4)					// �������m�ێ��̃A���C�����g�l
#define MEMHEADER_SIZE				(28)				// �������w�b�_�T�C�Y
#define MEMHEADER_USERINFO_SIZE		(MEMHEADER_SIZE-2)	// �������w�b�_�テ�[�U�[���̈�T�C�Y
#define HEAPID_MASK					(0x7fff)			// �q�[�v�h�c�擾�}�X�N
#define HEAPDIR_MASK				(0x8000)			// �q�[�v�m�ە����擾�}�X�N

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
inline  HEAPID GetHeapLowID( HEAPID heapID )	
{
	return (( heapID & HEAPID_MASK )|( HEAPDIR_MASK ));
}

//------------------------------------------------------------------------------
/**
 * �V�X�e��������
 *
 * @param   header			���������
 * @param   parentHeapMax	��{�q�[�v�G���A��
 * @param   totalHeapMax	�S�q�[�v�G���A��
 * @param   startOffset		�^����ꂽ�o�C�g�������S�̂̊J�n�ʒu�����炷
 *
 * @retval  BOOL			TRUE�Ő����^FALSE�Ŏ��s
 *							errorCode�F�m�ۂł��Ȃ������e�q�[�v�h�c
 */
//------------------------------------------------------------------------------
extern BOOL
	GFI_HEAP_Init
		(const HEAP_INIT_HEADER* header, u16 parentHeapMax, u16 totalHeapMax, u32 startOffset);

//------------------------------------------------------------------------------
/**
 * �V�X�e���I��
 *
 * @retval  BOOL			TRUE�Ő����^FALSE�Ŏ��s
 */
//------------------------------------------------------------------------------
extern BOOL
	GFI_HEAP_Exit
		( void );

//------------------------------------------------------------------
/**
 * �q�[�v�쐬
 *
 * @param   parentHeapID	�������̈�m�ۗp�q�[�v�h�c�i���ɗL���ł���K�v������j
 * @param   childHeapID		�V�K�ɍ쐬����q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 * @param   size			�q�[�v�T�C�Y
 *
 * @retval  BOOL			TRUE�ō쐬�����^FALSE�Ŏ��s
 *							errorCode�F���s����
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_CREATE_DOUBLE = 1,	//��d�o�^
	HEAP_CANNOT_CREATE_NOPARENT,	//�e�q�[�v����
	HEAP_CANNOT_CREATE_HEAP,		//�������s��
	HEAP_CANNOT_CREATE_HEAPTABLE,	//�q�[�v�쐬�ʃI�[�o�[
	HEAP_CANNOT_CREATE_HEAPHANDLE,	//�q�[�v�n���h���쐬���s
};

extern BOOL 
	GFI_HEAP_CreateHeap
		( HEAPID parentHeapID, HEAPID childHeapID, u32 size );

//------------------------------------------------------------------
/**
 * �q�[�v�j��
 *
 * @param   childHeapID		�j������q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  BOOL			TRUE�Ŕj�������^FALSE�Ŏ��s
 *							errorCode�F���s����
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_DELETE_DOUBLE = 1,	//���d�j��
	HEAP_CANNOT_DELETE_NOPARENT,	//�e�q�[�v����
};

extern BOOL
	GFI_HEAP_DeleteHeap
		( HEAPID childHeapID );

//------------------------------------------------------------------
/**
 * �q�[�v���烁�������m�ۂ���
 *
 * @param   heapID		�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 * @param   size		�m�ۃT�C�Y
 *
 * @retval  void*		�m�ۂ����̈�A�h���X�i���s�Ȃ�NULL�j
 *						errorCode�F���s����
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_ALLOC_NOID = 1,	//�w��h�c������
	HEAP_CANNOT_ALLOC_NOHEAP,	//�w��h�c�q�[�v���o�^����Ă��Ȃ�
	HEAP_CANNOT_ALLOC_MEM,		//�������s��
};

extern void*
	GFI_HEAP_AllocMemory
		( HEAPID heapID, u32 size );

//------------------------------------------------------------------
/**
 * �q�[�v���烁�������������
 *
 * @param   memory		�m�ۂ����������|�C���^
 *
 * @retval  BOOL		TRUE�ŉ�������^FALSE�Ŏ��s
 *						errorCode�F���s����
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_FREE_NOBLOCK = 1,	//�w��|�C���^�ɊԈႢ������
	HEAP_CANNOT_FREE_NOID,			//�ۑ��h�c�q�[�v���ُ�
	HEAP_CANNOT_FREE_NOHEAP,		//�ۑ��h�c�q�[�v���擾�����n���h�������o�^���
	HEAP_CANNOT_FREE_NOMEM,			//�������m�ۃJ�E���^�Ɉُ�i���݂��Ă��Ȃ��j
};

extern BOOL
	GFI_HEAP_FreeMemory
		( void* memory );

//------------------------------------------------------------------
/**
 * NitroSystem ���C�u�����n�֐����v������A���P�[�^���쐬����
 *
 * @param   pAllocator		NNSFndAllocator�\���̂̃A�h���X
 * @param   heapID			�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 * @param   align			�m�ۂ��郁�����u���b�N�ɓK�p����A���C�����g�i���̒l�͐��̒l�ɕϊ��j
 *
 * @retval  BOOL			TRUE�Ő����^FALSE�Ŏ��s
 *							errorCode�F���s����
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_INITALLOCATER_NOID = 1,	//�w��h�c������
};

extern BOOL
	GFI_HEAP_InitAllocator
		( NNSFndAllocator* pAllocator, HEAPID heapID, s32 alignment );

//------------------------------------------------------------------
/**
 * �m�ۂ����������u���b�N�̃T�C�Y��ύX����B
 *
 * @param   memory		�������u���b�N�|�C���^
 * @param   newSize		�ύX��̃T�C�Y�i�o�C�g�P�ʁj
 *
 * @retval  BOOL		TRUE�Ő����^FALSE�Ŏ��s
 *						errorCode�F���s����
 *
 * ��Ԃ�ύX��̃T�C�Y�ɂ���Ă͎��s�A�������͎w��T�C�Y���傫���Ȃ邱�Ƃ�����
 *
 * �Ⴆ�΁y�w�b�_�{���́z�̂悤�Ȍ`���̃O���t�B�b�N�o�C�i�����q�`�l�ɓǂݍ��݁A
 * ���̕���VRAM�ɓ]��������A�w�b�_�݂̂��c�������Ƃ����P�[�X�ȂǂŎg�p���邱�Ƃ�
 * �z�肵�Ă��邪�B�g�p�͐T�d�ɁB
 *
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_MEMRESIZE_NOBLOCK = 1,	//�w��|�C���^�ɊԈႢ������
	HEAP_CANNOT_MEMRESIZE_NOID,			//�ۑ��h�c�q�[�v���ُ�
	HEAP_CANNOT_MEMRESIZE_NOHEAP,		//�ۑ��h�c�q�[�v���擾�����n���h�������o�^���
	HEAP_CANNOT_MEMRESIZE_LARGE,		//�������s���Ŋg��o���Ȃ�
	HEAP_CANNOT_MEMRESIZE_SMALL,		//���������Ȃ����ďk���o���Ȃ�
};

extern BOOL
	GFI_HEAP_ResizeMemory
		( void* memory, u32 newSize );

//------------------------------------------------------------------
/**
 * �q�[�v�̋󂫗̈�T�C�Y��Ԃ�
 * �q�[�v�̊m�ۉ\�ő�̈�T�C�Y��Ԃ�
 *
 * @param   heapID	�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  u32		�󂫗̈�T�C�Y�i�o�C�g�P�ʁj�G���[����0
 *					errorCode�F���s����
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETSIZE_NOID = 1,	//�w��h�c������
};

extern u32
	GFI_HEAP_GetHeapFreeSize
		( HEAPID heapID );

extern u32
	GFI_HEAP_GetHeapAllocatableSize
		( HEAPID heapID );

//------------------------------------------------------------------
/**
 * �n���h�����̎擾
 *
 * @param   heapID				�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  NNSFndHeapHandle*	�Y������n���h���i���s�Ȃ�NULL�j
 *								errorCode�F���s����
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETHANDLE_NOID = 1,	//�w��h�c������
};

extern NNSFndHeapHandle
	GFI_HEAP_GetHandle
		( HEAPID heapID );

//------------------------------------------------------------------
/**
 * �e�n���h�����̎擾
 *
 * @param   heapID				�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  NNSFndHeapHandle*	�Y������q�[�v���������Ă���e�n���h���i���s�Ȃ�NULL�j
 *								errorCode�F���s����
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETPARENTHANDLE_NOID = 1,	//�w��h�c������
};

extern NNSFndHeapHandle
	GFI_HEAP_GetParentHandle
		( HEAPID heapID );

//------------------------------------------------------------------
/**
 * �������u���b�N���̎擾
 *
 * @param   heapID	�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  void*	�Y������q�[�v�̃|�C���^�i���s�Ȃ�NULL�j
 *					errorCode�F���s����
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETHEAPMEMBLOCK_NOID = 1,	//�w��h�c������
};

extern void*
	GFI_HEAP_GetHeapMemBlock
		( HEAPID heapID );

//------------------------------------------------------------------
/**
 * �A���P�[�g���̎擾
 *
 * @param   heapID	�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  u16		�Y������q�[�v�ŃA���P�[�g����Ă��鐔�i���s�Ȃ�0xffff�j
 *					errorCode�F���s����
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETHEAPCOUNT_NOID = 1,	//�w��h�c������
};

extern u16
	GFI_HEAP_GetHeapCount
		( HEAPID heapID );

//------------------------------------------------------------------
/**
 * �������w�b�_��q�[�v�h�c�̎擾
 *
 * @param   memory	�m�ۂ����������A�h���X
 *
 * @retval  u16		�q�[�v�h�c�A�擾���s����0xffff
 *					errorCode�F���s����
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETMEMHEADERHEAPID_NOBLOCK = 1,	//�ۑ��h�c�q�[�v���ُ�
};

extern u16
	GFI_HEAP_GetMemheaderHeapID
		( void* memory );

//------------------------------------------------------------------
/**
 * �������w�b�_�テ�[�U�[���|�C���^�̎擾
 *
 * @param   memory	�m�ۂ����������A�h���X
 *
 * @retval  void*	���[�U�[���|�C���^�A�擾���s����NULL
 *					errorCode�F���s����
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETMEMHEADERUSERINFO_NOBLOCK = 1,	//�ۑ��h�c�q�[�v���ُ�
};

extern void*
	GFI_HEAP_GetMemheaderUserinfo
		( void* memory );

//------------------------------------------------------------------
/**
 * �q�[�v�̈悪�j�󂳂�Ă��Ȃ����`�F�b�N
 *
 * @param   heapID	�q�[�vID�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  BOOL	�j�󂳂�Ă��Ȃ����TRUE
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_CHECKHEAPSAFE_NOID = 1,	//�w��h�c������
	HEAP_CANNOT_CHECKHEAPSAFE_NOHEAP,	//�擾�����n���h�������o�^���
	HEAP_CANNOT_CHECKHEAPSAFE_DESTROY,	//�q�[�v�̈悪�j�󂳂�Ă���
};

extern BOOL
	GFI_HEAP_CheckHeapSafe
		( HEAPID heapID );

//------------------------------------------------------------------
/**
 * �G���[�R�[�h�̎擾
 *
 * @retval  int		�G���[�R�[�h
 */
//------------------------------------------------------------------
extern u32
	GFI_HEAP_GetErrorCode
		( void );


#endif	// __HEAP_INTER_H__

#ifdef __cplusplus
} /* extern "C" */
#endif
