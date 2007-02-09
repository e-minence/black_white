//==============================================================================
/**
 *
 *@file		heapsys.c
 *@brief	�q�[�v�̈�Ǘ�
 *
 */
//==============================================================================
#include "gflib.h"

#ifdef HEAP_DEBUG
//#include  "system\heapdefine.h"
//#define  ALLOCINFO_PRINT_HEAPID   HEAPID_BASE_APP	// ���̃q�[�v�h�c�Ɋւ��Ă̂ݏڍׂȏ����o��
#endif

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
 *	�v���g�^�C�v�錾
 */
//----------------------------------------------------------------
#ifdef HEAPSYS_DEBUG
static void HeaderDebugParamSet( void* memory, const char* filename, u16 lineNum );
static void PrintShortHeap( HEAPID heapID, u32 size, const char* filename, u32 linenum );
static void PrintAllocInfo( void* memory, u32 size );
static void PrintFreeInfo( void* memory );
#endif

//----------------------------------------------------------------
/**
 *	���[�U�[�������u���b�N�w�b�_��`�i�T�C�Y:MEMHEADER_USERINFO_SIZE = 26 �j
 */
//----------------------------------------------------------------
#define FILENAME_LEN	(24/2)
typedef struct {
	char	filename[ FILENAME_LEN ];	///< �Ăяo����\�[�X��
	u16		lineNum;
}DEBUG_MEMHEADER;


//------------------------------------------------------------------------------
/**
 * �V�X�e��������
 *
 * @param   header			�e�q�[�v�������\���̂ւ̃|�C���^
 * @param   baseHeapMax		�e�q�[�v����
 * @param   heapMax			�e�q�[�v�E�q�q�[�v���v��
 * @param   startOffset		�q�[�v�̈�J�n�I�t�Z�b�g�i�v�S�o�C�g�A���C���j
 */
//------------------------------------------------------------------------------
void
	GFL_HEAP_sysInit
		(const HEAP_INIT_HEADER* header, u32 parentHeapMax, u32 totalHeapMax, u32 startOffset)
{
	BOOL result = GFI_HEAP_sysInit( header, parentHeapMax, totalHeapMax, startOffset );
	u32	 remainsize = (u32)(OS_GetMainArenaHi())-(u32)(OS_GetMainArenaLo());

	OS_Printf( "remains of MainRAM = 0x%08x bytes.\n", remainsize ); 
	if( result == FALSE )
	{
		u32 errorCode = GFI_HEAP_ErrorCodeGet();
		OS_Panic( "Create ParentHeap FAILED. ID = %d size = %x\n" , 
					errorCode, header[errorCode].size );
	}
}


//------------------------------------------------------------------------------
/**
 * �V�X�e���I��
 */
//------------------------------------------------------------------------------
void
	GFL_HEAP_sysExit
		( void )
{
	BOOL result = GFI_HEAP_sysExit();

	if( result == FALSE )
	{
		OS_Panic( "Delete ParentHeap FAILED.\n" );
	}
}


//------------------------------------------------------------------
/**
 * �q�[�v�쐬
 *
 * @param   parentHeapID		�������̈�m�ۗp�q�[�v�h�c�i���ɗL���ł���K�v������j
 * @param   childHeapID			�V�K�ɍ쐬����q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 * @param   size				�q�[�v�T�C�Y
 */
//------------------------------------------------------------------
void 
	GFL_HEAP_CreateHeap
		( HEAPID parentHeapID, HEAPID childHeapID, u32 size )
{
	BOOL result = GFI_HEAP_CreateHeap( parentHeapID, childHeapID, size );

	if( result == FALSE )
	{
		u32 errorCode = GFI_HEAP_ErrorCodeGet();

		OS_Printf( "Create ChildHeap FAILED. ID = %d size = %x\n", childHeapID, size );

		switch( errorCode )
		{
			case HEAP_CANNOT_CREATE_DOUBLE:
				OS_Panic( "already exist.\n" );
				break;
			case HEAP_CANNOT_CREATE_NOPARENT:
				OS_Panic( "ParentHeap not exist.\n" );
				break;
			case HEAP_CANNOT_CREATE_HEAP:
				OS_Panic( "not enough memory. ParentHeap ID = %d remains = %x\n",
							parentHeapID, GFI_HEAP_GetHeapFreeSize( parentHeapID ) );
				break;
			case HEAP_CANNOT_CREATE_HEAPTABLE:
				OS_Panic( "no more create heap.\n" );
				break;
			case HEAP_CANNOT_CREATE_HEAPHANDLE:
				OS_Panic( "make handle FAILED.\n" );
				break;
		}
	} else {
		OS_Printf( "Create ChildHeap. ID = %d size = %x\n", childHeapID, size );
	}
}


//------------------------------------------------------------------
/**
 * �q�[�v�j��
 *
 * @param   childHeapID			�j������q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 */
//------------------------------------------------------------------
void
	GFL_HEAP_DeleteHeap
		( HEAPID childHeapID )
{
	BOOL result = GFI_HEAP_DeleteHeap( childHeapID );
#ifdef HEAPSYS_DEBUG
	GFL_HEAP_DEBUG_PrintUnreleasedMemoryCheck( childHeapID );
#endif

	if( result == FALSE )
	{
		u32 errorCode = GFI_HEAP_ErrorCodeGet();

		OS_Printf( "Delete ChildHeap FAILED. ID = %d\n", childHeapID );

		switch( errorCode )
		{
			case HEAP_CANNOT_DELETE_DOUBLE:
				OS_Panic( "not exist.\n" );
				break;
			case HEAP_CANNOT_DELETE_NOPARENT:
				OS_Panic( "ParentHeap cannot search.\n" );
				break;
		}
	} else {
		OS_Printf( "Delete ChildHeap. ID = %d\n", childHeapID );
	}
}


//------------------------------------------------------------------
/**
 * �q�[�v���烁�������m�ۂ���
 *
 * @param   heapID		�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 * @param   size		�m�ۃT�C�Y
 *
 * @retval  void*		�m�ۂ����̈�A�h���X
 */
//------------------------------------------------------------------
#ifndef HEAPSYS_DEBUG
void*
	GFL_HEAP_AllocMemoryblock
		( HEAPID heapID, u32 size )
#else
void*
	GFL_HEAP_AllocMemoryblock
		( HEAPID heapID, u32 size, const char* filename, u16 linenum )
#endif
{
	void* memory = GFI_HEAP_AllocMemory( heapID, size );

	if( memory == NULL )
	{
		u32 errorCode = GFI_HEAP_ErrorCodeGet();

		OS_Printf( "Alloc Memory FAILED. heapID = %d. allocsize = %x\n", heapID, size );

		switch( errorCode )
		{
			case HEAP_CANNOT_ALLOC_NOID:
				OS_Printf( "heapID is not exist.\n" );
				break;
			case HEAP_CANNOT_ALLOC_NOHEAP:
				OS_Printf( "heap is not exist.\n" );
				break;
			case HEAP_CANNOT_ALLOC_MEM:
				OS_Printf( "not enough memory. heapID = %d remains = %x\n",
							heapID,	GFI_HEAP_GetHeapFreeSize( heapID ));
				break;
		}
		#ifdef HEAPSYS_DEBUG
		PrintShortHeap( heapID, size, filename, linenum );
		#endif
		OS_Panic( "....................................\n" );
	} else {
		#ifdef HEAPSYS_DEBUG
		HeaderDebugParamSet( memory, filename, linenum );
		#endif
	}
	//���K�v�ɉ����ď��̕\��������i�Ăяo�����񐔂������̂�Default�ł͕\�����Ȃ��j
	//OS_Printf( "Alloc Memory. Heap ID = %d. allocsize = %x\n", heapID, size );
	return memory;
}


//------------------------------------------------------------------
/**
 * �q�[�v���烁�������������
 *
 * @param   memory		�m�ۂ����������A�h���X
 */
//------------------------------------------------------------------
void
	GFL_HEAP_FreeMemoryblock
		( void* memory )
{
	BOOL result = GFI_HEAP_FreeMemory( memory );

	if( result == FALSE )
	{
		u32 errorCode = GFI_HEAP_ErrorCodeGet();

		OS_Printf( "Free Memory FAILED. memory = %08x\n", memory );

		switch( errorCode )
		{
			case HEAP_CANNOT_FREE_NOBLOCK:
				OS_Panic( "this pointer is not memory_block.\n" );
				break;
			case HEAP_CANNOT_FREE_NOID:
				OS_Panic( "system_backup_heapID is not exist.\n" );
				break;
			case HEAP_CANNOT_FREE_NOHEAP:
				OS_Panic( "system_backup_heap is not exist.\n" );
				break;
			case HEAP_CANNOT_FREE_NOMEM:
				OS_Printf( "this memory_block is not exist\n" );
				GFL_HEAP_CheckHeapSafe( GFI_HEAP_GetMemheaderHeapID( memory ));
				break;
		}
	}
	//���K�v�ɉ����ď��̕\��������i�Ăяo�����񐔂������̂�Default�ł͕\�����Ȃ��j
	//OS_Printf( "Free Memory.\n", heapID, size );
}


//------------------------------------------------------------------
/**
 * NitroSystem ���C�u�����n�֐����v������A���P�[�^���쐬����
 *
 * @param   pAllocator		NNSFndAllocator�\���̂̃A�h���X
 * @param   heapID			�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 * @param   align			�m�ۂ��郁�����u���b�N�ɓK�p����A���C�����g�i���̒l�͐��̒l�ɕϊ��j
 */
//------------------------------------------------------------------
void
	GFL_HEAP_InitAllocator
		( NNSFndAllocator* pAllocator, HEAPID heapID, s32 alignment )
{
	BOOL result = GFI_HEAP_InitAllocator( pAllocator, heapID, alignment );

	if( result == FALSE )
	{
		OS_Printf( "InitAllocator FAILED.\n" );
		OS_Panic( "heapID is not exist. ID = %d\n", heapID );
	}
}


//------------------------------------------------------------------
/**
 * �m�ۂ����������u���b�N�̃T�C�Y��ύX����B
 *
 * @param   memory		�������u���b�N�|�C���^
 * @param   newSize		�ύX��̃T�C�Y�i�o�C�g�P�ʁj
 */
//------------------------------------------------------------------
void
	GFL_HEAP_MemoryResize
		( void* memory, u32 newSize )
{
	BOOL result = GFI_HEAP_MemoryResize( memory, newSize );

	if( result == FALSE )
	{
		u32 errorCode = GFI_HEAP_ErrorCodeGet();

		OS_Printf( "MemoryResize FAILED.\n" );

		switch( errorCode )
		{
			case HEAP_CANNOT_MEMRESIZE_NOBLOCK:
				OS_Panic( "this pointer is not memory_block.\n" );
				break;
			case HEAP_CANNOT_MEMRESIZE_NOID:
				OS_Panic( "system_backup_heapID is crushed.\n" );
				break;
			case HEAP_CANNOT_MEMRESIZE_NOHEAP:
				OS_Panic( "system_backup_heap is not exist.\n" );
				break;
			case HEAP_CANNOT_MEMRESIZE_LARGE:
				OS_Printf( "cannot large\n" );
				break;
			case HEAP_CANNOT_MEMRESIZE_SMALL:
				OS_Printf( "cannot small\n" );
				break;
		}
	}
}


//------------------------------------------------------------------
/**
 * �q�[�v�̋󂫗̈�T�C�Y��Ԃ�
 *
 * @param   heapID	�q�[�v�h�c�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  u32		�󂫗̈�T�C�Y�i�o�C�g�P�ʁj
 */
//------------------------------------------------------------------
u32
	GFL_HEAP_GetHeapFreeSize
		( HEAPID heapID )
{
	u32 result =  GFI_HEAP_GetHeapFreeSize( heapID );

	if( result == 0 )
	{
		OS_Printf( "GetHeapFreeSize FAILED.\n" );
		OS_Panic( "heapID is not exist. ID = %d\n", heapID );
	}
	return result;
}


//------------------------------------------------------------------
/**
 * �q�[�v�̈悪�j�󂳂�Ă��Ȃ����`�F�b�N
 *
 * @param   heapID	�q�[�vID�i�ŏ�ʃr�b�g�͎擾�����t���O�j
 *
 * @retval  BOOL	�j�󂳂�Ă��Ȃ����TRUE
 */
//------------------------------------------------------------------
void
	GFL_HEAP_CheckHeapSafe
		( HEAPID heapID )
{
	BOOL result = GFI_HEAP_CheckHeapSafe( heapID );

	if( result == FALSE )
	{
		u32 errorCode = GFI_HEAP_ErrorCodeGet();

		OS_Printf( "Memory Check FAILED.\n" );

		switch( errorCode )
		{
			case HEAP_CANNOT_CHECKHEAPSAFE_NOID:
				OS_Printf( "heapID is not exist.\n" );
				break;
			case HEAP_CANNOT_CHECKHEAPSAFE_NOHEAP:
				OS_Printf( "heap is not exist.\n" );
				break;
			case HEAP_CANNOT_CHECKHEAPSAFE_DESTROY:
				OS_Panic( "memory is Crushed\n" );
				break;
		}
	}
}




#ifdef HEAPSYS_DEBUG
//----------------------------------------------------------------
/**
 *	�f�o�b�O�p�֐��i���[�J���j
 */
//----------------------------------------------------------------
//------------------------------------------------------------------
/**
 * �u���b�N�w�b�_�Ƀf�o�b�O������������
 *
 * @param   header		�w�b�_�A�h���X
 * @param   filename	�t�@�C����
 * @param   line_no		�s�ԍ�
 */
//------------------------------------------------------------------
static void HeaderDebugParamSet( void* memory, const char* filename, u16 lineNum )
{
	DEBUG_MEMHEADER* header = (DEBUG_MEMHEADER*)GFI_HEAP_GetMemheaderUserinfo( memory );

	int i;
	for(i = 0; i < FILENAME_LEN; i++)
	{
		header->filename[i] = filename[i];
		if( filename[i] == '\0' ){ break; }
	}
	header->lineNum = lineNum;
}

//------------------------------------------------------------------
/**
 * �������u���b�N�w�b�_�ɕۑ�����Ă���t�@�C�������o�b�t�@�ɃR�s�[
 *
 * @param   dst			�R�s�[��o�b�t�@
 * @param   header		�������u���b�N�w�b�_
 *
 */
//------------------------------------------------------------------
static void GetFileName( char* dst, char* src )
{
	int i;

	// �I�[�R�[�h�i�V�ŗ̈�߂����ς��t�@�C�����Ɏg���Ă邽�߁A���������������K�v
	for(i = 0; i < FILENAME_LEN; i++)
	{
		if( src[i] == '\0' ){ break; }
		dst[i] = src[i];
	}
	dst[i] = '\0';
}

//------------------------------------------------------------------
/**
 * �c�胁�������s�����Ċm�ۂł��Ȃ����b�Z�[�W�擾
 *
 * @param   heapID		�q�[�v�h�c
 * @param   size		�m�ۂ��悤�Ƃ����T�C�Y
 *
 */
//------------------------------------------------------------------
static void PrintShortHeap( HEAPID heapID, u32 size, const char* filename, u32 linenum )
{
	NNSFndHeapHandle handle = GFI_HEAP_GetHandle(heapID);
	u32 freeAreaSize = NNS_FndGetTotalFreeSizeForExpHeap( handle );
	u32 allocatableMaxSize = NNS_FndGetAllocatableSizeForExpHeapEx( handle, 4 );

	OS_Printf("Can't alloc %ldbytes memory from Heap(%d)\n", size, heapID);
	OS_Printf("This Heap have %ldbytes Free Area\n", freeAreaSize );
	OS_Printf("and %ldbytes Allocatable Area\n", allocatableMaxSize );
	OS_Printf("%s(%d)\n", filename, linenum);

	OS_Printf("these Memoryblocks haven't released\n");
	GFL_HEAP_DEBUG_PrintExistMemoryBlocks( heapID );
}

//------------------------------------------------------------------
/**
 * �������m�ێ��̏ڍ׏��\��
 *
 * @param   header		�������u���b�N�w�b�_
 * @param   handle		�������u���b�N���܂܂��q�[�v�̃n���h��
 * @param   size		�m�ۃT�C�Y�i���N�G�X�g���ꂽ�T�C�Y�j
 *
 */
//------------------------------------------------------------------
static void PrintAllocInfo( void* memory, u32 size )
{
	DEBUG_MEMHEADER* header = (DEBUG_MEMHEADER*)GFI_HEAP_GetMemheaderUserinfo( memory );
	HEAPID	heapID = GFI_HEAP_GetMemheaderHeapID( memory );
	u16		allocCount = GFI_HEAP_GetHeapCount( heapID );
	char	filename[FILENAME_LEN+1];

	GetFileName( filename, header->filename );

	OS_Printf("[HEAP] ALLOC count=%3d rest=0x%08x adrs:0x%08x size:0x%05x %s(%d)\n",
				allocCount, GFI_HEAP_GetHeapFreeSize( heapID ), (u32)memory - MEMHEADER_SIZE,
				size + MEMHEADER_SIZE, filename, header->lineNum );
}

//------------------------------------------------------------------
/**
 * ������������̏ڍ׏��\��
 *
 * @param   heapID			�q�[�vID
 * @param   size			�m�ۂ����T�C�Y
 * @param   handle			�q�[�v�n���h��
 * @param   filename		�Ăяo�����\�[�X�t�@�C����
 * @param   line			�Ăяo�����\�[�X�t�@�C���s�ԍ�
 *
 */
//------------------------------------------------------------------
static void PrintFreeInfo( void* memory )
{
	DEBUG_MEMHEADER* header = (DEBUG_MEMHEADER*)GFI_HEAP_GetMemheaderUserinfo( memory );
	HEAPID	heapID = GFI_HEAP_GetMemheaderHeapID( memory );
	u16		allocCount = GFI_HEAP_GetHeapCount( heapID );
	char	filename[FILENAME_LEN+1];
	u32		blockSize, restSize;

	GetFileName( filename, header->filename );
				
	blockSize = NNS_FndGetSizeForMBlockExpHeap((void*)((u32)memory - MEMHEADER_SIZE))
					+ sizeof(NNSiFndExpHeapMBlockHead);
	// �c��T�C�Y�͌��T�C�Y�{���ꂩ�������悤�Ƃ��郁�����u���b�N�̃T�C�Y�ɂȂ�͂�
	restSize =  GFI_HEAP_GetHeapFreeSize( heapID ) + blockSize;
				
	OS_Printf("[HEAP] FREE  count=%3d rest=0x%08x adrs:0x%08x size:0x%05x %s(%d)\n",
				allocCount, restSize, (u32)memory - MEMHEADER_SIZE, blockSize,
				filename, header->lineNum );
}

//----------------------------------------------------------------
/**
 *	�f�o�b�O�p�֐��i�O���[�o���j
 */
//----------------------------------------------------------------
//------------------------------------------------------------------
/**
 * ������������̏����v�����g
 *
 * @param   heapID		
 * ������̈悪����΂b�o�t��~
 */
//------------------------------------------------------------------
void GFL_HEAP_DEBUG_PrintUnreleasedMemoryCheck( HEAPID heapID )
{
	u16	restheap_count = GFI_HEAP_GetHeapCount( heapID );

	if( restheap_count ){
		OS_Printf( "these Memoryblocks haven't released\n" );
		OS_Printf( "HeapID = %d  restcnt = %d .....\n", heapID, restheap_count );
		OS_Printf( "freesize = 0x%x bytes \n", GFI_HEAP_GetHeapFreeSize(heapID) );

		GFL_HEAP_DEBUG_PrintExistMemoryBlocks( heapID );
		OS_Panic( "....................................\n" );
	}
}

//------------------------------------------------------------------
/**
 * ����q�[�v�̑S�������u���b�N����\��
 *
 * @param   heapID				�q�[�vID
 */
//------------------------------------------------------------------
static void HeapConflictVisitorFunc(void* memBlock, NNSFndHeapHandle heapHandle, u32 param)
{
	void*	memory = (u8*)memBlock + MEMHEADER_SIZE;
	DEBUG_MEMHEADER* header = (DEBUG_MEMHEADER*)GFI_HEAP_GetMemheaderUserinfo( memory );
	char	filename[FILENAME_LEN+1];

	GetFileName( filename, header->filename );

	OS_Printf(filename);
	OS_Printf(" (%d)  adr:%08x  siz:%05x\n", 
			header->lineNum, (u32)memory, NNS_FndGetSizeForMBlockExpHeap(memBlock));
}

void GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID heapID )
{
	//�g���q�[�v����m�ۂ����������u���b�N�S�āiAlloc���ꂽ���́j�ɑ΂��A�w�肵���֐����Ă΂���
	NNS_FndVisitAllocatedForExpHeap( GFI_HEAP_GetHandle(heapID), HeapConflictVisitorFunc, 0 );
}

//------------------------------------------------------------------
/**
 * �q�[�v����m�ۂ����������u���b�N�̎��T�C�Y�擾�i�f�o�b�O�p�j
 *
 * @param   memBlock		
 *
 * @retval  u32		�������u���b�N�T�C�Y
 */
//------------------------------------------------------------------
u32 GFL_HEAP_DEBUG_GetMemoryBlockSize( const void* memory )
{
	((u8*)memory) -= MEMHEADER_SIZE;
	return NNS_FndGetSizeForMBlockExpHeap( memory );
}

#endif



#if 0

//==============================================================================================
// �f�o�b�O�p�q�[�v��ԃX�^�b�N
//==============================================================================================

struct _HEAP_STATE_STACK {
	u16		sp;
	u16		stackNum;
	HEAPID  heapID;
	u64		stack[0];
};

//------------------------------------------------------------------
/**
 * �q�[�v��ԃX�^�b�N��V�K�쐬
 *
 * @param   heapID			�`�F�b�N����q�[�vID
 * @param   stackNum		�X�^�b�N�v�f��
 *
 * @retval  HEAP_STATE_STACK*		�쐬�����X�^�b�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
HEAP_STATE_STACK*  HSS_Create( HEAPID heapID, u32 stackNum )
{
	HEAP_STATE_STACK* hss;
	u32 size;

	size = sizeof(HEAP_STATE_STACK) + sizeof(u64)*stackNum;
	hss = sys_AllocMemoryLo( HEAPID_BASE_DEBUG, size );
	hss->sp = 0;
	hss->heapID = heapID;
	return hss;
}

//------------------------------------------------------------------
/**
 * �q�[�v��� Push
 *
 * @param   hss		�X�^�b�N�ւ̃|�C���^
 *
 */
//------------------------------------------------------------------
void HSS_Push( HEAP_STATE_STACK* hss )
{
	GF_ASSERT( hss->sp < hss->stackNum );
	hss->stack[hss->sp++] = sys_GetHeapState( hss->heapID );
}

//------------------------------------------------------------------
/**
 * �q�[�v��� Pop
 *
 * @param   hss		�X�^�b�N�ւ̃|�C���^
 *
 */
//------------------------------------------------------------------
void HSS_Pop( HEAP_STATE_STACK* hss )
{
	GF_ASSERT( hss->sp );
	hss->sp--;
	GF_ASSERT( hss->stack[hss->sp] == sys_GetHeapState(hss->heapID) );
}


//------------------------------------------------------------------
/**
 * �q�[�v��ԃX�^�b�N��j������
 *
 * @param   hss		�X�^�b�N�ւ̃|�C���^
 *
 */
//------------------------------------------------------------------
void HSS_Delete( HEAP_STATE_STACK* hss )
{
	sys_FreeMemoryEz( hss );
}

#endif










//==============================================================================
/**
 *
 *
 *
 *
 *	�c�s�b�l�Ǘ�
 *
 *
 *
 *
 */
//==============================================================================
#include "heapDTCM.h"

//------------------------------------------------------------------------------
/**
 * �V�X�e��������
 *
 * @param	size		�g�p�T�C�Y			
 */
//------------------------------------------------------------------------------
void
	GFL_HEAP_DTCM_sysInit
		( u32 size )
{
	if( GFI_HEAP_DTCM_sysInit( size ) == FALSE ){
		OS_Panic("cannot create heap from DTCM size = %x\n", size );
	}
}


//------------------------------------------------------------------------------
/**
 * �V�X�e���I��
 */
//------------------------------------------------------------------------------
void
	GFL_HEAP_DTCM_sysExit
		( void )
{
	if( GFI_HEAP_DTCM_sysExit() == FALSE ){
		OS_Panic("cannot delete heap for DTCM\n" );
	}
}


//------------------------------------------------------------------
/**
 * �q�[�v���烁�������m�ۂ���
 *
 * @param   size		�m�ۃT�C�Y
 *
 * @retval  void*		�m�ۂ����̈�A�h���X�i���s�Ȃ�NULL�j
 */
//------------------------------------------------------------------
void*
	GFL_HEAP_DTCM_AllocMemory
		( u32 size )
{
	void* mem = GFI_HEAP_DTCM_AllocMemory( size );

	if( mem == NULL )
	{
		OS_Printf( "Alloc DTCM_Memory FAILED. allocsize = %x\n", size );
	} else {
//		OS_Printf( "Alloc DTCM_Memory. size = %x\n", size );
	}
	return mem;
}


//------------------------------------------------------------------
/**
 * �q�[�v���烁�������������
 *
 * @param   memory		�m�ۂ����������A�h���X
 */
//------------------------------------------------------------------
void
	GFL_HEAP_DTCM_FreeMemory
		( void* memory )
{
	BOOL result = GFI_HEAP_DTCM_FreeMemory( memory );

	if( result == NULL )
	{
		OS_Printf( "Free DTCM_Memory FAILED. memory = %08x\n", memory );
	} else {
//		OS_Printf( "Free DTCM_Memory. memory = %08x\n", memory );
	}
}







