//==============================================================================
/**
 *
 *@file		heap.c
 *@brief	�q�[�v�̈�Ǘ�
 *
 * Description:  �V�X�e���q�[�v�ƃA�v���P�[�V�����q�[�v���쐬���܂��B���q�[�v
 * 				�ɂ�NITRO-System�̊g���q�[�v���g�p���Ă��܂��B
 *
 * 				�V�X�e���q�[�v�p�̃������Ƃ��āASYSTEM_HEAP_SIZE�������C���A
 * 				���[�i����m�ۂ��A���C���A���[�i�̎c���S�ăA�v���P�[�V����
 * 				�q�[�v�p�̃������Ɋm�ۂ��Ă��܂��B
 *
 * 				�V�X�e���q�[�v�́A�Q�[���V�X�e�����̃V�X�e���v���O�����Ŏg�p
 * 				���邱�Ƃ�z�肵�Ă��܂��B�A�v���P�[�V�����q�[�v�ɂ́A�Q�[��
 * 				�Ŏg�p����f�[�^�����[�h����ׂɎg�p���܂��B
 *
 */
//==============================================================================
#include "gflib.h"
#include "heap_inter.h"

//----------------------------------------------------------------
/**
 *	�萔
 */
//----------------------------------------------------------------
#define DEFAULT_ALIGN_MASK			(3)			// �������m�ێ��̃A���C�����g�p�}�X�N
#define MEMBLOCK_FILENAME_AREASIZE	(12)		// �f�o�b�O�p�w�b�_�Ɋi�[����t�@�C�����̈�T�C�Y
#define HEAP_MAX					(24)		// ��x�ɍ쐬�\�Ȏq�q�[�v�̐�
#define INVALID_HANDLE_IDX			(255)		// �q�[�v�n���h�������l
//#define MAGICNUM					(6478)		// �������Ǘ��w�b�_�}�W�b�N�i���o�[

//----------------------------------------------------------------
/**
 *	���[�N��`
 */
//----------------------------------------------------------------
typedef struct {
	NNSFndHeapHandle*	handle;
	NNSFndHeapHandle*	parentHandle;
	void**				heapMemBlock;
	u16*				count;

}HEAP_HANDLE_DATA;

typedef struct {
	HEAP_HANDLE_DATA*	hhd;
	u8*					handleIdxTbl;

	u16					heapMax;
	u16					parentHeapMax;
	u16					usableHeapMax;

	u16					errorCode;
}HEAP_SYS;

//----------------------------------------------------------------
/**
 *	�������u���b�N�w�b�_��`
 *
 *	�S�o�C�g���E���ӎ�����B�c�r�̓��C�u�����ŃP�A����Ă���̂�
 *	�}�W�b�N�i���o�[�͎g��Ȃ��B�i��p�֐����g���ă������j��`�F�b�N���s���j
 */
//----------------------------------------------------------------
typedef struct {
	u32		heapID;									///< u32
//	u16		magicnum;								///< �}�W�b�N�i���o�[
	u8		userInfo[22];							///< �O���g�p�̈�
}MEMBLOCK_HEADER;

//----------------------------------------------------------------
/**
 *	�O���[�o��
 */
//----------------------------------------------------------------
static HEAP_SYS  HeapSys = { 0 };

//----------------------------------------------------------------
/**
 *	�n���h���擾�}�N��
 */
//----------------------------------------------------------------
#define HeapHandleIdxTbl( idx )			(HeapSys.handleIdxTbl[ idx ])

#define HeapHandle( num )				(HeapSys.hhd->handle[ num ])
#define ParentHeapHandle( num )			(HeapSys.hhd->parentHandle[ num ])
#define HeapMemBlock( num )				(HeapSys.hhd->heapMemBlock[ num ])
#define HeapCount( num )				(HeapSys.hhd->count[ num ])

#define GetHeapHandle( idx )			(HeapHandle( HeapHandleIdxTbl( idx ) ))
#define GetParentHeapHandle( idx )		(ParentHeapHandle( HeapHandleIdxTbl( idx ) ))
#define GetHeapMemBlock( idx )			(HeapMemBlock( HeapHandleIdxTbl( idx ) ))
#define GetHeapCount( idx )				(HeapCount( HeapHandleIdxTbl( idx ) ))

#define SetHeapHandle( idx, val )		(HeapHandle( HeapHandleIdxTbl( idx ) ) = ( val ))
#define SetParentHeapHandle( idx, val )	(ParentHeapHandle( HeapHandleIdxTbl( idx ) ) = ( val ))
#define SetHeapMemBlock( idx, val )		(HeapMemBlock( HeapHandleIdxTbl( idx ) ) = ( val ))
#define SetHeapCount( idx, val )		(HeapCount( HeapHandleIdxTbl( idx ) ) = ( val ))
		
//==============================================================
// �v���g�^�C�v�錾
//==============================================================

//------------------------------------------------------------------------------
/**
 * �V�X�e��������
 *
 * @param   header			���������
 * @param   parentHeapMax	�e�i��{�j�q�[�v�G���A��
 * @param   totalHeapMax	�S�q�[�v�G���A��
 * @param   startOffset		�^����ꂽ�o�C�g�������S�̂̊J�n�ʒu�����炷
 *
 * @retval  BOOL			TRUE�Ő����^FALSE�Ŏ��s
 *							errorCode�F�m�ۂł��Ȃ������e�q�[�v�h�c
 */
//------------------------------------------------------------------------------
BOOL
	GFI_HEAP_sysInit
		(const HEAP_INIT_HEADER* header, u32 parentHeapMax, u32 totalHeapMax, u32 startOffset)
{
	void *mem;
	u32  usableHeapMax, i;

	//�����g�p�q�[�v�ʂ̐ݒ�
	usableHeapMax = parentHeapMax + HEAP_MAX;
	//�g�p�q�[�v�����Ȃ��Ă����ʂ̊m�ۂ͍s��
	if( totalHeapMax < usableHeapMax )
	{
		totalHeapMax = usableHeapMax;
	}

	// �J�n�I�t�Z�b�g���A���C�������g(4byte)�ɂ��킹���炷(��������͂��Â炭�����邽��)
	if( startOffset )
	{
		while( startOffset & DEFAULT_ALIGN_MASK )
		{
			startOffset++;
		}
		OS_AllocFromMainArenaLo( startOffset, DEFAULT_ALIGN );
	}

	// �q�[�v�n���h���f�[�^�̈�m��
	HeapSys.hhd = OS_AllocFromMainArenaLo( sizeof(HEAP_HANDLE_DATA)*(usableHeapMax), DEFAULT_ALIGN);

	for(i = 0; i < usableHeapMax; i++ )	// �n���h���f�[�^�̏�����(�����l�ŃN���A)
	{
		HeapHandle(i)		= NNS_FND_HEAP_INVALID_HANDLE;
		ParentHeapHandle(i)	= NNS_FND_HEAP_INVALID_HANDLE;
		HeapMemBlock(i)		= 0;
		HeapCount(i)		= 0;
	}

	// �q�[�vINDEX�̈�m��
	HeapSys.handleIdxTbl = OS_AllocFromMainArenaLo( totalHeapMax, DEFAULT_ALIGN);
	for(i = 0; i < totalHeapMax; i++ )	// INDEX�̏�����(�����l�ŃN���A)
	{
		HeapHandleIdxTbl(i)	= INVALID_HANDLE_IDX;	
	}

	HeapSys.heapMax			= totalHeapMax;
	HeapSys.parentHeapMax	= parentHeapMax;
	HeapSys.usableHeapMax	= usableHeapMax;
	HeapSys.errorCode		= 0;

	// ��{�q�[�v���C���f�b�N�X�쐬�A(i�̓q�[�v�h�c�Ɠ��`)
	for(i = 0; i < parentHeapMax; i++)
	{
		mem = OS_AllocFromMainArenaLo( header[i].size, DEFAULT_ALIGN );

		if( mem != NULL )
		{
			HeapHandle(i) = NNS_FndCreateExpHeap( mem, header[i].size );//�q�[�v�m�ہ��n���h���擾
			HeapHandleIdxTbl(i)	= i;//INDEX�e�[�u���̐ݒ�
		} else {
			HeapSys.errorCode	= i;
			return FALSE;
		}
	}
	return TRUE;
}


//------------------------------------------------------------------------------
/**
 * �V�X�e���I��
 *
 * @retval  BOOL			TRUE�Ő����^FALSE�Ŏ��s
 */
//------------------------------------------------------------------------------
BOOL
	GFI_HEAP_sysExit
		( void )
{
	return TRUE;
}


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
BOOL 
	GFI_HEAP_CreateHeap
		( u32 parentHeapID, u32 childHeapID, u32 size )
{
	s32 align;

	if( childHeapID & HEAPDIR_MASK )	//�m�ە��@�̎擾
	{
		align = -DEFAULT_ALIGN;	//�������m��
	} else {
		align = DEFAULT_ALIGN;	//�O������m��
	}

	childHeapID &= HEAPID_MASK;	//���q�[�v�h�c�̎擾

	if( HeapHandleIdxTbl( childHeapID ) != INVALID_HANDLE_IDX )
	{	//��d�o�^
		HeapSys.errorCode = HEAP_CANNOT_CREATE_DOUBLE;
	} else {
		NNSFndHeapHandle  parentHeap = GetHeapHandle( parentHeapID );//�e�q�[�v�n���h���擾

		if( parentHeap == NNS_FND_HEAP_INVALID_HANDLE )
		{	//�e�q�[�v����
			HeapSys.errorCode = HEAP_CANNOT_CREATE_NOPARENT;
		} else {
			void* mem = NNS_FndAllocFromExpHeapEx( parentHeap, size, align );	//�������m��
			if( mem == NULL )
			{	//�������s��
				HeapSys.errorCode = HEAP_CANNOT_CREATE_HEAP;
			} else {
				int i;
				// ��{�q�[�v�͉������Ȃ��O��ŁA�ȍ~���T�[�`
				for( i = HeapSys.parentHeapMax; i < HeapSys.usableHeapMax; i++ )
				{
					if( HeapHandle(i) == NNS_FND_HEAP_INVALID_HANDLE )
					{
						HeapHandle(i) = NNS_FndCreateExpHeap( mem, size );	//�n���h���쐬
						if( HeapHandle(i) == NNS_FND_HEAP_INVALID_HANDLE )
						{	//�q�[�v�n���h���쐬���s
							HeapSys.errorCode = HEAP_CANNOT_CREATE_HEAPHANDLE;
						} else {
							ParentHeapHandle(i) = parentHeap;	//�e�n���h���̕ۑ�
							HeapMemBlock(i) = mem;				//�̈�|�C���^�ۑ�
							HeapHandleIdxTbl(childHeapID) = i;	//INDEX�e�[�u���̐ݒ�
	
							HeapSys.errorCode = 0;
							return TRUE;
						}
					}
				}
				HeapSys.errorCode = HEAP_CANNOT_CREATE_HEAPTABLE;	//�q�[�v�쐬�ʃI�[�o�[
			}
		}
	}
	return FALSE;
}


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
BOOL
	GFI_HEAP_DeleteHeap
		( u32 childHeapID )
{
	NNSFndHeapHandle  handle;

	childHeapID &= HEAPID_MASK;	//���q�[�v�h�c�̎擾

	handle = GetHeapHandle( childHeapID );	//�j���q�[�v�n���h���擾

	if( handle == NNS_FND_HEAP_INVALID_HANDLE )
	{	//���d�j��
		HeapSys.errorCode = HEAP_CANNOT_DELETE_DOUBLE;
	} else {
		NNSFndHeapHandle	parentHandle = GetParentHeapHandle( childHeapID );
		void*				heapMemBlock = GetHeapMemBlock( childHeapID );

		NNS_FndDestroyExpHeap( handle );	//�q�[�v�̔j��

		if(( parentHandle == NNS_FND_HEAP_INVALID_HANDLE )||( heapMemBlock == NULL ))
		{	//�e�q�[�v����
			HeapSys.errorCode = HEAP_CANNOT_DELETE_NOPARENT;
		} else {
			NNS_FndFreeToExpHeap( parentHandle, heapMemBlock );	//�������u���b�N�̉��

			// �n���h���f�[�^�̏�����(�����l�ŃN���A)
			SetHeapHandle( childHeapID, NNS_FND_HEAP_INVALID_HANDLE );
			SetParentHeapHandle( childHeapID, NNS_FND_HEAP_INVALID_HANDLE );
			SetHeapMemBlock( childHeapID, NULL );
			SetHeapCount( childHeapID, 0 );

			// INDEX�̏�����(�����l�ŃN���A)
			HeapHandleIdxTbl( childHeapID ) = INVALID_HANDLE_IDX;
			return TRUE;
		}
	}
	return FALSE;
}


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
void*
	GFI_HEAP_AllocMemory
		( u32 heapID, u32 size )
{
	s32 align;

	if( heapID & HEAPDIR_MASK )	//�m�ە��@�̎擾
	{
		align = -DEFAULT_ALIGN;	//�������m��
	} else {
		align = DEFAULT_ALIGN;	//�O������m��
	}

	heapID &= HEAPID_MASK;	//���q�[�v�h�c�̎擾

	if( heapID >= HeapSys.heapMax )
	{	//�w��h�c������
		HeapSys.errorCode = HEAP_CANNOT_ALLOC_NOID;
	} else {
		NNSFndHeapHandle  handle = GetHeapHandle( heapID );	//�q�[�v�n���h���擾
		if( handle == NNS_FND_HEAP_INVALID_HANDLE )
		{	//�w��h�c�q�[�v���o�^����Ă��Ȃ�
			HeapSys.errorCode = HEAP_CANNOT_ALLOC_NOHEAP;
		} else {
			u32			memsiz	= size + sizeof(MEMBLOCK_HEADER);	//�������Ǘ��w�b�_�ǉ�
			OSIntrMode	irqold	= OS_DisableInterrupts();	//���荞�݂��֎~
			void*		memory	= NNS_FndAllocFromExpHeapEx( handle, memsiz, align );//�������m��

			if( memory == NULL )
			{	//�������s��
				HeapSys.errorCode = HEAP_CANNOT_ALLOC_MEM;
			} else {
				MEMBLOCK_HEADER* memheader = (MEMBLOCK_HEADER*)memory;

//				memheader->magicnum = MAGICNUM;				//�w�b�_�Ƀ}�W�b�N�i���o�[��ݒ�
				memheader->heapID = heapID;					//�w�b�_�Ƀq�[�v�h�c��ۑ�
				(u8*)memory += sizeof(MEMBLOCK_HEADER);		//���������̈�փ|�C���^�ړ�
				GetHeapCount(heapID)++;						//�������m�ۃJ�E���^�̃C���N�������g
			} 
			OS_RestoreInterrupts( irqold );	//���荞�݂𕜋A

			HeapSys.errorCode = 0;
			return memory;
		}
	}
	return NULL;
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
	GFI_HEAP_FreeMemory
		( void* memory )
{
	MEMBLOCK_HEADER* memheader = (MEMBLOCK_HEADER*)((u8*)memory - sizeof(MEMBLOCK_HEADER));
//	u16	magicnum	= memheader->magicnum;	//�������Ǘ��w�b�_�}�W�b�N�i���o�[�̎擾
	u32	heapID		= memheader->heapID;	//�Ώۃq�[�v�h�c�̎擾
	
//	if( magicnum != MAGICNUM )
//	{	//�w��|�C���^�ɊԈႢ������
//		HeapSys.errorCode = HEAP_CANNOT_FREE_NOBLOCK;
//	} else {
		if( heapID >= HeapSys.heapMax )
		{	//�ۑ��h�c�q�[�v���ُ�
			HeapSys.errorCode = HEAP_CANNOT_FREE_NOID;
		} else {
			NNSFndHeapHandle  handle = GetHeapHandle( heapID );	//�q�[�v�n���h���擾
			if( handle == NNS_FND_HEAP_INVALID_HANDLE)
			{	//�ۑ��h�c�q�[�v���擾�����n���h�������o�^���
				HeapSys.errorCode = HEAP_CANNOT_FREE_NOHEAP;
			} else {
				if( GetHeapCount(heapID) == 0 )
				{	//�������m�ۃJ�E���^�Ɉُ�i���݂��Ă��Ȃ��j
					HeapSys.errorCode = HEAP_CANNOT_FREE_NOMEM;
				} else {
					OSIntrMode	irqold	= OS_DisableInterrupts();	//���荞�݂��֎~
	
					GetHeapCount(heapID)--;	//�������m�ۃJ�E���^�̃f�N�������g
					NNS_FndFreeToExpHeap( handle, memory );	//�������u���b�N�̉��
					OS_RestoreInterrupts( irqold );	//���荞�݂𕜋A

					HeapSys.errorCode = 0;
					return TRUE;
				}
			}
		}
//	}
	return FALSE;
}


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
BOOL
	GFI_HEAP_InitAllocator
		( NNSFndAllocator* pAllocator, u32 heapID, s32 alignment )
{
	s32 align;

	if( alignment < 0 )
	{
		alignment = -alignment;	//���̒l�ɋ����ϊ�
	}
	if( heapID & HEAPDIR_MASK )	//�m�ە��@�̎擾
	{
		align = -alignment;	//�������m��
	} else {
		align = alignment;	//�O������m��
	}

	heapID &= HEAPID_MASK;	//���q�[�v�h�c�̎擾

	if( heapID >= HeapSys.heapMax )
	{	//�w��h�c������
		HeapSys.errorCode = HEAP_CANNOT_INITALLOCATER_NOID;
	} else {
		NNS_FndInitAllocatorForExpHeap( pAllocator, GetHeapHandle( heapID ), align );

		HeapSys.errorCode = 0;
		return TRUE;
	}
	return FALSE;
}


//------------------------------------------------------------------
/**
 * �m�ۂ����������u���b�N�̃T�C�Y��ύX����B
 *
 * @param   memory		�������u���b�N�|�C���^
 * @param   newSize		�ύX��̃T�C�Y�i�o�C�g�P�ʁj
 *
 * @retval  BOOL		TRUE�Ő����^FALSE�Ŏ��s
 *						errorCode�F���s����
 */
//------------------------------------------------------------------
BOOL
	GFI_HEAP_MemoryResize
		( void* memory, u32 newSize )
{
	MEMBLOCK_HEADER* memheader = (MEMBLOCK_HEADER*)((u8*)memory - sizeof(MEMBLOCK_HEADER));
//	u16	magicnum	= memheader->magicnum;	//�������Ǘ��w�b�_�}�W�b�N�i���o�[�̎擾
	u32	heapID		= memheader->heapID;	//�Ώۃq�[�v�h�c�̎擾

	newSize += sizeof(MEMBLOCK_HEADER);	// �w�b�_�̈�ǉ�

//	if( magicnum != MAGICNUM )
//	{	//�w��|�C���^�ɊԈႢ������
//		HeapSys.errorCode = HEAP_CANNOT_MEMRESIZE_NOBLOCK;
//	} else {
		if( heapID >= HeapSys.heapMax )
		{	//�ۑ��h�c�q�[�v���ُ�
			HeapSys.errorCode = HEAP_CANNOT_MEMRESIZE_NOID;	
		} else {
			NNSFndHeapHandle  handle = GetHeapHandle( heapID );	//�q�[�v�n���h���擾
			if( handle == NNS_FND_HEAP_INVALID_HANDLE)
			{	//�ۑ��h�c�q�[�v���擾�����n���h�������o�^���
				HeapSys.errorCode = HEAP_CANNOT_MEMRESIZE_NOHEAP;
			} else {
				u32		oldSize		= NNS_FndGetSizeForMBlockExpHeap( memheader );
				u32		resultsize	= NNS_FndResizeForMBlockExpHeap( handle, memheader, newSize );
				BOOL	result		= TRUE;
				HeapSys.errorCode = 0;

				if( oldSize < newSize )
				{
					if( resultsize == 0 )
					{	//�������s���Ŋg��o���Ȃ�
						HeapSys.errorCode = HEAP_CANNOT_MEMRESIZE_LARGE;
						result = FALSE;
					}
				} else {
					if( resultsize == oldSize )
					{	//���������Ȃ����ďk���o���Ȃ�
						HeapSys.errorCode = HEAP_CANNOT_MEMRESIZE_SMALL;
						result = FALSE;
					}
				}
				return result;
			}
		}
//	}
	return FALSE;
}


//------------------------------------------------------------------
/**
 * �q�[�v�̋󂫗̈�T�C�Y��Ԃ�
 *
 * @param   heapID	�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  u32		�󂫗̈�T�C�Y�i�o�C�g�P�ʁj�A�擾���s����0
 *					errorCode�F���s����
 */
//------------------------------------------------------------------
u32
	GFI_HEAP_GetHeapFreeSize
		( u32 heapID )
{
	heapID &= HEAPID_MASK;	//���q�[�v�h�c�̎擾

	if( heapID >= HeapSys.heapMax )
	{	//�w��h�c������
		HeapSys.errorCode = HEAP_CANNOT_GETSIZE_NOID;
	} else {
		HeapSys.errorCode = 0;
		return NNS_FndGetTotalFreeSizeForExpHeap( GetHeapHandle( heapID ) );
	}
	return 0;
}


//------------------------------------------------------------------
/**
 * �n���h�����̎擾
 *
 * @param   heapID				�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  NNSFndHeapHandle*	�Y������n���h���A�擾���s����NULL
 *								errorCode�F���s����
 */
//------------------------------------------------------------------
NNSFndHeapHandle
	GFI_HEAP_GetHandle
		( u32 heapID )
{
	heapID &= HEAPID_MASK;	//���q�[�v�h�c�̎擾

	if( heapID >= HeapSys.heapMax )
	{	//�w��h�c������
		HeapSys.errorCode = HEAP_CANNOT_GETHANDLE_NOID;
	} else {
		HeapSys.errorCode = 0;
		return GetHeapHandle( heapID );
	}
	return NULL;
}


//------------------------------------------------------------------
/**
 * �e�n���h�����̎擾
 *
 * @param   heapID				�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  NNSFndHeapHandle*	�Y������q�[�v���������Ă���e�n���h���A�擾���s����NULL
 *								errorCode�F���s����
 */
//------------------------------------------------------------------
NNSFndHeapHandle
	GFI_HEAP_GetParentHandle
		( u32 heapID )
{
	heapID &= HEAPID_MASK;	//���q�[�v�h�c�̎擾

	if( heapID >= HeapSys.heapMax )
	{	//�w��h�c������
		HeapSys.errorCode = HEAP_CANNOT_GETPARENTHANDLE_NOID;
	} else {
		HeapSys.errorCode = 0;
		return GetParentHeapHandle( heapID );
	}
	return NULL;
}


//------------------------------------------------------------------
/**
 * �������u���b�N���̎擾
 *
 * @param   heapID	�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  void*	�Y������q�[�v�̃|�C���^�A�擾���s����NULL
 *					errorCode�F���s����
 */
//------------------------------------------------------------------
void*
	GFI_HEAP_GetHeapMemBlock
		( u32 heapID )
{
	heapID &= HEAPID_MASK;	//���q�[�v�h�c�̎擾

	if( heapID >= HeapSys.heapMax )
	{	//�w��h�c������
		HeapSys.errorCode = HEAP_CANNOT_GETHEAPMEMBLOCK_NOID;
	} else {
		HeapSys.errorCode = 0;
		return GetHeapMemBlock( heapID );
	}
	return NULL;
}


//------------------------------------------------------------------
/**
 * �A���P�[�g���̎擾
 *
 * @param   heapID	�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  u16		�Y������q�[�v�ŃA���P�[�g����Ă��鐔�A�擾���s����0xff
 *					errorCode�F���s����
 */
//------------------------------------------------------------------
u16
	GFI_HEAP_GetHeapCount
		( u32 heapID )
{
	heapID &= HEAPID_MASK;	//���q�[�v�h�c�̎擾

	if( heapID >= HeapSys.heapMax )
	{	//�w��h�c������
		HeapSys.errorCode = HEAP_CANNOT_GETHEAPCOUNT_NOID;
	} else {
		HeapSys.errorCode = 0;
		return GetHeapCount( heapID );
	}
	return 0xff;
}


//------------------------------------------------------------------
/**
 * �������w�b�_��q�[�v�h�c�̎擾
 *
 * @param   memory	�m�ۂ����������A�h���X
 *
 * @retval  u32		�q�[�v�h�c�A�擾���s����0xff
 *					errorCode�F���s����
 */
//------------------------------------------------------------------
u32
	GFI_HEAP_GetMemheaderHeapID
		( void* memory )
{
	MEMBLOCK_HEADER* memheader = (MEMBLOCK_HEADER*)((u8*)memory - sizeof(MEMBLOCK_HEADER));
//	u16	magicnum	= memheader->magicnum;	//�������Ǘ��w�b�_�}�W�b�N�i���o�[�̎擾
	
//	if( magicnum != MAGICNUM )
//	{	//�w��|�C���^�ɊԈႢ������
//		HeapSys.errorCode = HEAP_CANNOT_GETMEMHEADERHEAPID_NOBLOCK;
//	} else {
		return memheader->heapID;
//	}
	return 0xff;
}


//------------------------------------------------------------------
/**
 * �������w�b�_�テ�[�U�[���|�C���^�̎擾
 *
 * @param   memory	�m�ۂ����������A�h���X
 *
 * @retval  u32		���[�U�[���|�C���^�A�擾���s����NULL
 *					errorCode�F���s����
 */
//------------------------------------------------------------------
void*
	GFI_HEAP_GetMemheaderUserinfo
		( void* memory )
{
	MEMBLOCK_HEADER* memheader = (MEMBLOCK_HEADER*)((u8*)memory - sizeof(MEMBLOCK_HEADER));
//	u16	magicnum	= memheader->magicnum;	//�������Ǘ��w�b�_�}�W�b�N�i���o�[�̎擾
	
//	if( magicnum != MAGICNUM )
//	{	//�w��|�C���^�ɊԈႢ������
//		HeapSys.errorCode = HEAP_CANNOT_GETMEMHEADERUSERINFO_NOBLOCK;
//	} else {
		return memheader->userInfo;
//	}
	return NULL;
}


//------------------------------------------------------------------
/**
 * �q�[�v�̈悪�j�󂳂�Ă��Ȃ����`�F�b�N
 *
 * @param   heapID		�q�[�vID
 *
 * @retval  BOOL		�j�󂳂�Ă��Ȃ����TRUE
 */
//------------------------------------------------------------------
BOOL
	GFI_HEAP_CheckHeapSafe
		( u32 heapID )
{
	heapID &= HEAPID_MASK;	//���q�[�v�h�c�̎擾

	if( heapID >= HeapSys.heapMax )
	{	//�w��h�c������
		HeapSys.errorCode = HEAP_CANNOT_CHECKHEAPSAFE_NOID;
	} else {
		NNSFndHeapHandle handle = GetHeapHandle( heapID );
		if( handle != NNS_FND_HEAP_INVALID_HANDLE )
		{	//�擾�����n���h�������o�^���
			HeapSys.errorCode = HEAP_CANNOT_CHECKHEAPSAFE_NOHEAP;
		} else {
			BOOL result = NNS_FndCheckExpHeap( handle, NNS_FND_HEAP_ERROR_PRINT );
			if( result == FALSE )
			{ 	//�q�[�v�̈悪�j�󂳂�Ă���
				HeapSys.errorCode = HEAP_CANNOT_CHECKHEAPSAFE_DESTROY;
			} else {
				HeapSys.errorCode = 0;
				return TRUE;
			}
		}
	}
	return FALSE;
}


//------------------------------------------------------------------
/**
 * �G���[�R�[�h�̎擾
 *
 * @retval  int		�G���[�R�[�h
 */
//------------------------------------------------------------------
u32
	GFI_HEAP_ErrorCodeGet
		( void )
{
	return (u32)HeapSys.errorCode;
}




