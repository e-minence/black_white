//==============================================================================
/**
 *
 *@file		heapsys.c
 *@brief	�q�[�v�̈�Ǘ�
 *
 */
//==============================================================================
#include "gflib.h"
#include "heap_inter.h"

#ifdef HEAP_DEBUG
//#include  "system\heapdefine.h"
//#define  ALLOCINFO_PRINT_HEAPID   HEAPID_BASE_APP	// ���̃q�[�v�h�c�Ɋւ��Ă̂ݏڍׂȏ����o��
#endif

//==============================================================
// Prototype
//==============================================================
#if 0
static int SearchEmptyHandleIndex( void );
static BOOL CreateHeapCore( u32 parentHeapID, u32 childHeapID, u32 size, s32 align );
static void* AllocMemoryCore( NNSFndHeapHandle heapHandle, u32 size, s32 alignment, u32 heapID );
static void PrintAllocInfo( const MEMORY_BLOCK_HEADER* header, NNSFndHeapHandle handle, u32 size );
static void PrintFreeInfo( const MEMORY_BLOCK_HEADER* header, NNSFndHeapHandle handle);
static void HeaderDebugParamSet( MEMORY_BLOCK_HEADER* header, const char* filename, u32 line_no );
static void PrintShortHeap( u32 heapID, u32 size, const char* filename, u32 line_num );
static void PrintExistMemoryBlocks( u32 heapID );
static void HeapConflictVisitorFunc(void* memBlock, NNSFndHeapHandle heapHandle, u32 param);
static void CopyFileName( char* dst, const MEMORY_BLOCK_HEADER* header );
static void PrintUnreleasedMemoryInfo( u32 heapID );





#ifdef HEAP_DEBUG
static void HeaderDebugParamSet( MEMORY_BLOCK_HEADER* header, const char* filename, u32 line_no );
static void PrintExistMemoryBlocks( u32 heapID );
static void HeapConflictVisitorFunc(void* memBlock, NNSFndHeapHandle heapHandle, u32 param);
static void CopyFileName( char* dst, const MEMORY_BLOCK_HEADER* header );
static void PrintUnreleasedMemoryInfo( u32 heapID );
#endif
#endif


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
		( u32 parentHeapID, u32 childHeapID, u32 size )
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
		( u32 childHeapID )
{
#ifdef HEAPSYS_DEBUG
	{
		u16	restheap_count = GFI_HEAP_GetHeapCount( childHeapID );

		if( restheap_count ){
			OS_Printf( "these Memoryblocks haven't released\n" );
			OS_Printf( "HeapID = %d  restcnt = %d .....\n", childHeapID, restheap_count );

			OS_Panic( "\n" );
		}
	}
#endif
	{
		BOOL result = GFI_HEAP_DeleteHeap( childHeapID );

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
		( u32 heapID, u32 size )
#else
void*
	GFL_HEAP_AllocMemoryblock
		( u32 heapID, u32 size, const char* filename, u32 linenum )
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
				OS_Panic( "heapID is not exist.\n" );
				break;
			case HEAP_CANNOT_ALLOC_NOHEAP:
				OS_Panic( "heap is not exist.\n" );
				break;
			case HEAP_CANNOT_ALLOC_MEM:
				OS_Panic( "not enough memory. heapID = %d remains = %x\n",
							heapID,	GFI_HEAP_GetHeapFreeSize( heapID ));
				break;
		}
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
		( NNSFndAllocator* pAllocator, u32 heapID, s32 alignment )
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
		( u32 heapID )
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
		( u32 heapID )
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





#if 0
#ifndef HEAP_DEBUG
#else	// #ifndef HEAP_DEBUG

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
static void PrintAllocInfo( const MEMORY_BLOCK_HEADER* header, NNSFndHeapHandle handle, u32 size )
{
	char filename[MEMBLOCK_FILENAME_AREASIZE+1];

	CopyFileName( filename, header );

	OS_TPrintf("[HEAP] ALLOC count=%3d rest=0x%08x adrs:0x%08x size:0x%05x %s(%d)\n",
				GetHeapCount(header->heapID), NNS_FndGetTotalFreeSizeForExpHeap(handle), 
				(u32)header, size+sizeof(MEMORY_BLOCK_HEADER), filename, header->lineNum );
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
static void PrintFreeInfo( const MEMORY_BLOCK_HEADER* header, NNSFndHeapHandle handle)
{
	char filename[MEMBLOCK_FILENAME_AREASIZE+1];
	u32  blockSize, restSize;

	CopyFileName( filename, header );

	blockSize = NNS_FndGetSizeForMBlockExpHeap(header) + sizeof(NNSiFndExpHeapMBlockHead);

	// �c��T�C�Y�͌��T�C�Y�{���ꂩ�������悤�Ƃ��郁�����u���b�N�̃T�C�Y�ɂȂ�͂�
	restSize = NNS_FndGetTotalFreeSizeForExpHeap(handle) + blockSize;

	OS_TPrintf("[HEAP] FREE  count=%3d rest=0x%08x adrs:0x%08x size:0x%05x %s(%d)\n",
				GetHeapCount(header->heapID), restSize, (u32)header, blockSize,
				filename, header->lineNum );
}



//------------------------------------------------------------------
/**
 * sys_AllocMemory�̃f�o�b�O�p���b�p�[�֐�
 *
 * @param   heapID			�q�[�vID
 * @param   size			�m�ۃT�C�Y
 * @param   filename		�Ăяo���\�[�X�̃t�@�C����
 * @param   line_num		�Ăяo���\�[�X�̍s�ԍ�
 *
 * @retval  void*			�m�ۂ����̈�A�h���X�i���s�Ȃ�NULL�j
 */
//------------------------------------------------------------------
void* sys_AllocMemoryDebug( u32 heapID, u32 size, const char* filename, u32 line_num )
{
	GF_ASSERT_MSG((OS_GetProcMode() != OS_PROCMODE_IRQ), "Alloc in IRQ\n %s(%d)", filename, line_num);

	if( heapID < HeapSys.heapMax )
	{
		NNSFndHeapHandle  h = GetHeapHandle( heapID );
		void* mem = AllocMemoryCore( h, size, DEFAULT_ALIGN, heapID );

		if( mem != NULL )
		{
			HeaderDebugParamSet((MEMORY_BLOCK_HEADER*)( (u8*)mem - sizeof(MEMORY_BLOCK_HEADER) ),
									filename, line_num);
			GetHeapCount(heapID)++;

			#ifdef ALLOCINFO_PRINT_HEAPID
			if( ALLOCINFO_PRINT_HEAPID == heapID )
			{
				const MEMORY_BLOCK_HEADER* mh = 
					(const MEMORY_BLOCK_HEADER*)((u8*)mem - sizeof(MEMORY_BLOCK_HEADER));
				PrintAllocInfo( mh, h, size );
				GF_ASSERT( sys_CheckHeapSafe( heapID ) );
			}
			#endif
		}
		else
		{
			PrintShortHeap( heapID, size, filename, line_num );
			GF_ASSERT(0);
		}

		return mem;
	}
	else
	{
		GF_ASSERT_MSG(0, "heapID = %d\n", heapID);
		return NULL;
	}
}
//------------------------------------------------------------------
/**
 * sys_AllocMemoryLo�̃f�o�b�O�p���b�p�[�֐�
 *
 * @param   heapID			�q�[�vID
 * @param   size			�m�ۃT�C�Y
 * @param   filename		�Ăяo���\�[�X�̃t�@�C����
 * @param   line_num		�Ăяo���\�[�X�̍s�ԍ�
 *
 * @retval  void*			�m�ۂ����̈�A�h���X�i���s�Ȃ�NULL�j
 */
//------------------------------------------------------------------
void* sys_AllocMemoryLoDebug( u32 heapID, u32 size, const char* filename, u32 line_num )
{
	GF_ASSERT_MSG((OS_GetProcMode() != OS_PROCMODE_IRQ), "Alloc in IRQ\n %s(%d)", filename, line_num);

	if( heapID < HeapSys.heapMax )
	{
		NNSFndHeapHandle  h = GetHeapHandle( heapID );
		void* mem = AllocMemoryCore( h, size, -DEFAULT_ALIGN, heapID );

		if( mem != NULL )
		{
			HeaderDebugParamSet((MEMORY_BLOCK_HEADER*)( (u8*)mem - sizeof(MEMORY_BLOCK_HEADER) ),
									filename, line_num);
			GetHeapCount(heapID)++;

			#ifdef ALLOCINFO_PRINT_HEAPID
			if( ALLOCINFO_PRINT_HEAPID == heapID )
			{
				const MEMORY_BLOCK_HEADER* mh = 
					(const MEMORY_BLOCK_HEADER*)((u8*)mem - sizeof(MEMORY_BLOCK_HEADER));
				PrintAllocInfo( mh, h, size );
				GF_ASSERT( sys_CheckHeapSafe( heapID ) );
			}
			#endif
		}
		else
		{
			PrintShortHeap( heapID, size, filename, line_num );
			GF_ASSERT(0);
		}
		return mem;
	}
	else
	{
		GF_ASSERT(0);
		return NULL;
	}
}
//------------------------------------------------------------------
/**
 * �u���b�N�w�b�_�Ƀf�o�b�O������������
 *
 * @param   header		�w�b�_�A�h���X
 * @param   filename	�t�@�C����
 * @param   line_no		�s�ԍ�
 *
 */
//------------------------------------------------------------------
static void HeaderDebugParamSet( MEMORY_BLOCK_HEADER* header, const char* filename, u32 line_no )
{
	int i;
	for(i = 0; i < MEMBLOCK_FILENAME_AREASIZE; i++)
	{
		header->filename[i] = filename[i];
		if( filename[i] == '\0' ){ break; }
	}
	header->lineNum = line_no;
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
static void PrintShortHeap( u32 heapID, u32 size, const char* filename, u32 line_num )
{
	NNSFndHeapHandle h;
	u32 freeAreaSize, allocatableMaxSize;

	h = GetHeapHandle(heapID);
	freeAreaSize = NNS_FndGetTotalFreeSizeForExpHeap( h );
	allocatableMaxSize = NNS_FndGetAllocatableSizeForExpHeapEx( h, DEFAULT_ALIGN );

	GF_ASSERT_Printf("Can't alloc %ldbytes memory from Heap(%d)\n", size, heapID);
	GF_ASSERT_Printf("This Heap have %ldbytes Free Area\n", freeAreaSize );
	GF_ASSERT_Printf("and %ldbytes Allocatable Area\n", allocatableMaxSize );
	GF_ASSERT_Printf("%s(%d)\n", filename, line_num);

	GF_ASSERT_Printf("these Memoryblocks haven't released\n");
	PrintExistMemoryBlocks( heapID );
}
#endif	// #ifndef HEAP_DEBUG #else


//------------------------------------------------------------------
/**
 * �q�[�v����m�ۂ������������������
 *
 * @param   memory		�m�ۂ����������A�h���X
 *
 */
//------------------------------------------------------------------
void sys_FreeMemoryEz( void* memory )
{
	#ifdef HEAP_DEBUG
	if( OS_GetProcMode() == OS_PROCMODE_IRQ )
	{
		char filename[MEMBLOCK_FILENAME_AREASIZE+1];
		MEMORY_BLOCK_HEADER* header;

		header = (MEMORY_BLOCK_HEADER*)((u8*)memory - sizeof(MEMORY_BLOCK_HEADER));
		CopyFileName( filename, header );
		GF_ASSERT_MSG(0, "Free in IRQ\n %s(%d) siz:%x", filename, header->lineNum, 
						NNS_FndGetSizeForMBlockExpHeap(header) );
	}
	#endif

	{
		u32 heapID;

		(u8*)memory -= sizeof(MEMORY_BLOCK_HEADER);
		heapID = ((MEMORY_BLOCK_HEADER*)memory)->heapID;

		if( heapID < HeapSys.heapMax )
		{
			NNSFndHeapHandle  h = GetHeapHandle(heapID);

			GF_ASSERT(h != NNS_FND_HEAP_INVALID_HANDLE);
			if( GetHeapCount(heapID) == 0 )
			{
				sys_CheckHeapSafe(heapID);
			}
			GF_ASSERT_MSG( GetHeapCount(heapID), "heapID = %d", heapID );

			GetHeapCount(heapID)--;

			#ifdef ALLOCINFO_PRINT_HEAPID
			if( ALLOCINFO_PRINT_HEAPID == heapID )
			{
				GF_ASSERT( sys_CheckHeapSafe(heapID) );
				PrintFreeInfo( (const MEMORY_BLOCK_HEADER*)memory, h );
			}
			#endif

			{
				OSIntrMode old;
				old = OS_DisableInterrupts();
				NNS_FndFreeToExpHeap( h, memory );
				OS_RestoreInterrupts( old );
			}

		}
		else
		{
			GF_ASSERT_MSG(0, "heapID = %d\n", heapID);
		}
	}
}
//------------------------------------------------------------------
/**
 * �q�[�v����m�ۂ������������������i�q�[�vID�w��Łj
 *
 * ��������������q�[�vID���w�肷�鍇���I�ȗ��R�����邩������Ȃ��̂�
 *   �c���Ă����B���ʂ� FreeMemoryEz ���g���Ζ��Ȃ��͂��B
 *
 * @param   heapID		�q�[�vID
 * @param   memory		�m�ۂ����������|�C���^
 *
 */
//------------------------------------------------------------------
void sys_FreeMemory( u32 heapID, void* memory )
{
	ASSERT_IRQ_ENABLED();

	if( heapID < HeapSys.heapMax )
	{
		NNSFndHeapHandle h = GetHeapHandle(heapID);

		GF_ASSERT(h != NNS_FND_HEAP_INVALID_HANDLE);

		(u8*)memory -= sizeof(MEMORY_BLOCK_HEADER);
		if( ((MEMORY_BLOCK_HEADER*)memory)->heapID != heapID )
		{
			GF_ASSERT_MSG(0, "�m�ێ��ƈႤ�q�[�vID�ŉ������悤�Ƃ��Ă���\n");
		}
		NNS_FndFreeToExpHeap( h, memory );

		GF_ASSERT( GetHeapCount(heapID) );
		GetHeapCount(heapID)--;

		#ifdef ALLOCINFO_PRINT_HEAPID
		if( ALLOCINFO_PRINT_HEAPID == heapID )
		{
			GF_ASSERT( sys_CheckHeapSafe( heapID ) );
			PrintFreeInfo( (const MEMORY_BLOCK_HEADER*)memory, h );
		}
		#endif

	}
	else
	{
		GF_ASSERT(0);
	}
}
//------------------------------------------------------------------
/**
 * �q�[�v�̋󂫗̈�T�C�Y��Ԃ�
 *
 * @param   heapID	�q�[�vID
 *
 * @retval  u32		�󂫗̈�T�C�Y�i�o�C�g�P�ʁj
 */
//------------------------------------------------------------------
u32 sys_GetHeapFreeSize( u32 heapID )
{
	if( heapID < HeapSys.heapMax )
	{
		NNSFndHeapHandle h = GetHeapHandle(heapID);
		return NNS_FndGetTotalFreeSizeForExpHeap( h );
	}
	GF_ASSERT(0);
	return 0;
}
//------------------------------------------------------------------
/**
 * NitroSystem ���C�u�����n�֐����v������A���P�[�^���쐬����
 *
 * @param   pAllocator		NNSFndAllocator�\���̂̃A�h���X
 * @param   heapID			�q�[�vID
 * @param   alignment		�m�ۂ��郁�����u���b�N�ɓK�p����A���C�����g
 *
 */
//------------------------------------------------------------------
void sys_InitAllocator( NNSFndAllocator* pAllocator, u32 heapID, int alignment)
{
	if( heapID < HeapSys.heapMax )
	{
		NNSFndHeapHandle h = GetHeapHandle(heapID);
		NNS_FndInitAllocatorForExpHeap( pAllocator, h, alignment );
	}
	else
	{
		GF_ASSERT(0);
	}
}

//------------------------------------------------------------------
/**
 * �m�ۂ����������u���b�N�̃T�C�Y���k������B
 *
 * @param   memBlock		�������u���b�N�|�C���^
 * @param   newSize			�k����̃T�C�Y�i�o�C�g�P�ʁj
 *
 *
 * �k���́A�������u���b�N�̌��������烁������������邱�Ƃōs���B
 * ������ꂽ���̓V�X�e���ɕԊ҂���A�V���ȃA���P�[�g�̈�Ƃ��Ďg�p�ł���B
 *
 * �Ⴆ�΁y�w�b�_�{���́z�̂悤�Ȍ`���̃O���t�B�b�N�o�C�i�����q�`�l�ɓǂݍ��݁A
 * ���̕���VRAM�ɓ]��������A�w�b�_�݂̂��c�������Ƃ����P�[�X�ȂǂŎg�p���邱�Ƃ�
 * �z�肵�Ă���B�g�p�͐T�d�ɁB
 *
 */
//------------------------------------------------------------------
void sys_CutMemoryBlockSize( void* memBlock, u32 newSize )
{
	ASSERT_IRQ_ENABLED();

	{
		u32 oldSize;

		(u8*)memBlock -= sizeof(MEMORY_BLOCK_HEADER);
		oldSize = NNS_FndGetSizeForMBlockExpHeap( memBlock );
		newSize += sizeof(MEMORY_BLOCK_HEADER);	// �Ăяo�����̓w�b�_���ӎ����Ă��Ȃ��̂�

		if( oldSize >= newSize )
		{
			u32 heapID;
			NNSFndHeapHandle handle;

			heapID = ((MEMORY_BLOCK_HEADER*)memBlock)->heapID;
			handle = GetHeapHandle(heapID);

			NNS_FndResizeForMBlockExpHeap( handle, memBlock, newSize );
		}
		else
		{
			GF_ASSERT(0);
		}
	}
}



//------------------------------------------------------------------
/**
 * �q�[�v�̈悪�j�󂳂�Ă��Ȃ����`�F�b�N�i�f�o�b�O�p�j
 *
 * @param   heapID		�q�[�vID
 *
 * @retval  BOOL		�j�󂳂�Ă��Ȃ����TRUE���Ԃ�
 */
//------------------------------------------------------------------
BOOL sys_CheckHeapSafe( u32 heapID )
{
	#ifdef HEAP_DEBUG
	if( heapID < HeapSys.heapMax )
	{
		NNSFndHeapHandle h = GetHeapHandle(heapID);
		if( h != NNS_FND_HEAP_INVALID_HANDLE )
		{
			return NNS_FndCheckExpHeap( h, NNS_FND_HEAP_ERROR_PRINT );
		}
	}
	return TRUE;
	#else

	// �{�����i�łɂ͑��݂��Ȃ��͂��̊֐������A
	// GF_ASSERT�𐻕i�łł��L���ɂ������߁AASSERT�`�F�b�N���ɌĂ΂�邱�Ƃ̂���
	// ���̊֐����폜�ł��Ȃ��Ȃ����B���̂��߁A���ASSERT���X���[�ł���悤��
	// TRUE��Ԃ��֐��Ƃ��Ďc���Ă����B
	return TRUE;
	#endif
}

#ifdef HEAP_DEBUG
//------------------------------------------------------------------
/**
 * �S�������u���b�N��������Ă��邩�`�F�b�N�i�f�o�b�O�p�j
 *�i�����̊֐����Ă΂ꂽ���ɂ܂��g�p���������c���Ă����ASSERT�Ŏ~�܂�j
 *
 * @param   heapID		�q�[�v�h�c
 *
 */
//------------------------------------------------------------------
void sys_CheckHeapFullReleased( u32 heapID )
{
	if( GetHeapCount(heapID) )
	{
		PrintUnreleasedMemoryInfo( heapID );
		GF_ASSERT(0);
	}
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
u32 sys_GetMemoryBlockSize( const void* memBlock )
{
	((u8*)memBlock) -= sizeof(MEMORY_BLOCK_HEADER);
	return NNS_FndGetSizeForMBlockExpHeap( memBlock );
}

//------------------------------------------------------------------
/**
 * �w��q�[�v�̃������A���P�[�g�񐔁��󂫗̈�T�C�Y��64bit�Ƀp�b�N���ĕԂ�
 *�i�q�[�v�j�����Ƃ͕ʂɃ��[�N�`�F�b�N���s�����߁j
 *
 * @param   heapID		�q�[�vID
 *
 * @retval  u64		
 */
//------------------------------------------------------------------
u64 sys_GetHeapState( u32 heapID )
{
	if( heapID < HeapSys.heapMax )
	{
		u64  ret;
		NNSFndHeapHandle h;

		h = GetHeapHandle(heapID);
		ret = (GetHeapCount(heapID) << 32) | NNS_FndGetTotalFreeSizeForExpHeap(h);
		return ret;
	}
	GF_ASSERT(0);
	return 0;
}

/*---------------------------------------------------------------------------*
 * @brief	�f�o�b�O�p�������󋵕\��
*---------------------------------------------------------------------------*/

//------------------------------------------------------------------
/**
 * ����q�[�v�̃������󂫗e�ʍ��v��\��
 *
 * @param   heapID		
 *
 */
//------------------------------------------------------------------
void sys_PrintHeapFreeSize( u32 heapID )
{
	if( heapID < HeapSys.heapMax )
	{
		NNSFndHeapHandle h = GetHeapHandle(heapID);
		OS_TPrintf("HEAP(ID :%d) FreeArea = %ld bytes\n", heapID, NNS_FndGetTotalFreeSizeForExpHeap(h) );
	}
}
//------------------------------------------------------------------
/**
 * ����q�[�v�̎g�p���������u���b�N����\��
 *
 * @param   heapID		
 *
 */
//------------------------------------------------------------------
void sys_PrintHeapExistMemoryInfo( u32 heapID )
{
	PrintExistMemoryBlocks( heapID );
}
//------------------------------------------------------------------
/**
 * ����q�[�v�̑S�������u���b�N����\��
 *
 * @param   heapID				�q�[�vID
 *
 */
//------------------------------------------------------------------
static void PrintExistMemoryBlocks( u32 heapID )
{
	NNSFndHeapHandle h = GetHeapHandle(heapID);
	NNS_FndVisitAllocatedForExpHeap( h, HeapConflictVisitorFunc, 0 );
}
//------------------------------------------------------------------
/**
 * sys_PrintHeapConflict�̃`�F�b�N���ʂ��G���[�Ȃ�A
 * �S�������u���b�N���������ɂ��Ă��̊֐����Ă΂��
 *
 * @param   memBlock		
 * @param   heapHandle		
 * @param   param		
 *
 */
//------------------------------------------------------------------
static void HeapConflictVisitorFunc(void* memBlock, NNSFndHeapHandle heapHandle, u32 param)
{
	char  filename[MEMBLOCK_FILENAME_AREASIZE + 1];
	int i;

	MEMORY_BLOCK_HEADER* head = (MEMORY_BLOCK_HEADER*)memBlock;

	CopyFileName( filename, head );

	GF_ASSERT_Printf(filename);
	GF_ASSERT_Printf(" (%d)", head->lineNum );
	GF_ASSERT_Printf(" adr:%08x", (u8*)memBlock + sizeof(MEMORY_BLOCK_HEADER));
	GF_ASSERT_Printf(" siz:%05x\n", NNS_FndGetSizeForMBlockExpHeap(head));
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
static void CopyFileName( char* dst, const MEMORY_BLOCK_HEADER* header )
{
	int i;

	// �I�[�R�[�h�i�V�ŗ̈�߂����ς��t�@�C�����Ɏg���Ă邽��
	// ���������������K�v...
	for(i = 0; i < MEMBLOCK_FILENAME_AREASIZE; i++)
	{
		if( header->filename[i] == '\0' ){ break; }
		dst[i] = header->filename[i];
	}
	dst[i] = '\0';

}

//------------------------------------------------------------------
/**
 * ������������̏����v�����g
 *
 * @param   heapID		
 *
 */
//------------------------------------------------------------------
static void PrintUnreleasedMemoryInfo( u32 heapID )
{
	GF_ASSERT_Printf("these Memoryblocks haven't released\n");
	GF_ASSERT_Printf("HeapID:%d  restcnt = %d .....\n", heapID, GetHeapCount(heapID));
	#ifdef HEAP_DEBUG
	{
		NNSFndHeapHandle  handle = GetHeapHandle(heapID);
		u32  siz = NNS_FndGetTotalFreeSizeForExpHeap( handle );
		GF_ASSERT_Printf(" ID %d  freesize = 0x%x bytes \n", heapID, siz );
	}
	#endif
	PrintExistMemoryBlocks( heapID );
}

//==============================================================================================
// �f�o�b�O�p�q�[�v��ԃX�^�b�N
//==============================================================================================

struct _HEAP_STATE_STACK {
	u16   sp;
	u16   stackNum;
	u32   heapID;
	u64   stack[0];
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
HEAP_STATE_STACK*  HSS_Create( u32 heapID, u32 stackNum )
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


#endif	// HEAP_DEBUG

#endif

