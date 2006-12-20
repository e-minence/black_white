//==============================================================================
/**
 *
 *@file		vman.c
 *@brief	VRAM�̈�}�l�[�W��
 *@author	taya
 *@data		2006.11.28
 *
 */
//==============================================================================

#include "heapsys.h"
#include "assert.h"
#include "areaman.h"

#include "vman.h"


#define NELEMS(ary)		(sizeof(ary)/sizeof(ary[0]))


enum {
	RESERVEINFO_INIT_POS = 0xffffffff,
	RESERVEINFO_INIT_SIZE = 0xffffffff,
};


//==============================================================
// Prototype
//==============================================================
static u32 calc_objtype_unit_bytesize( u32 vramSize );
static inline u32 calc_block_size( u32 byteSize, u32 unitSize );



u32 GFI_VRAM_CalcVramSize( u32 vramBank )
{
	static const struct {
		u32  bank;
		u32  size;
	}VramState[] = {
		{ GX_VRAM_A, 128 },
		{ GX_VRAM_B, 128 },
		{ GX_VRAM_C, 128 },
		{ GX_VRAM_D, 128 },
		{ GX_VRAM_E,  64 },
		{ GX_VRAM_F,  16 },
		{ GX_VRAM_G,  16 },
		{ GX_VRAM_H,  32 },
		{ GX_VRAM_I,  16 },
	};

	int i;
	u32 size = 0;

	for(i=0; i<NELEMS(VramState); i++)
	{
		if( vramBank & VramState[i].bank )
		{
			size += VramState[i].size;
		}
	}

	return size * 1024;
}


//==============================================================================================
// �}�l�[�W�����̒�`
//==============================================================================================
struct _GFL_VMAN{
	AREAMAN*   areaMan;
	u32        unitByteSize;
};

//==============================================================================================
// ���C���N�����̏�����
//==============================================================================================
void GFL_VMAN_sysInit( void )
{

}
void GFL_VMAN_sysExit( void )
{
	
}
//==============================================================================================
//==============================================================================================

//==============================================================================================
/**
 * �}�l�[�W���쐬
 *
 * @param   heapID			[in] �q�[�vID
 * @param   type			[in] �}�l�[�W���^�C�v�iBG or OBJ�j
 * @param   vramBank		[in] VRAM�o���N�w��
 *
 * @retval  GFL_VMAN*	�쐬�����}�l�[�W���I�u�W�F�N�g�ւ̃|�C���^
 */
////==============================================================================================
GFL_VMAN* GFL_VMAN_Create( u32 heapID, GFL_VMAN_TYPE type, GXVRamOBJ vramBank )
{
	GFL_VMAN*  vm;
	u32 vramSize;

	vm = sys_AllocMemory( heapID, sizeof(GFL_VMAN) );
	vramSize = GFI_VRAM_CalcVramSize( vramBank );

	if( type == GFL_VMAN_TYPE_OBJ )
	{
		vm->unitByteSize = calc_objtype_unit_bytesize( vramSize );
	}
	else
	{
		vm->unitByteSize = 0x20;
	}

	vm->areaMan = AREAMAN_Create( vramSize / vm->unitByteSize, heapID );

	return vm;
}
//==============================================================================================
/**
 * �}�l�[�W���j��
 *
 * @param   man		[in] �}�l�[�W���I�u�W�F�N�g�ւ̃|�C���^
 *
 */
//==============================================================================================
void GFL_VMAN_Delete( GFL_VMAN* man )
{
	AREAMAN_Delete( man->areaMan );
	sys_FreeMemoryEz( man );
}

//==============================================================================================
/**
 * �̈�m�ۏ��̃f�o�b�K�o��ON/OFF
 *
 * @param   man		[in] �}�l�[�W���I�u�W�F�N�g�ւ̃|�C���^
 *
 */
//==============================================================================================
void GFL_VMAN_SetPrintDebug( GFL_VMAN* man, BOOL flag )
{
	AREAMAN_SetPrintDebug( man->areaMan, flag );
}

//------------------------------------------------------------------
/**
 * VRAM�̃g�[�^���T�C�Y����A�Ǘ�����P�ʃo�C�g�����v�Z
 *
 * @param   vramSize		VRAM�g�[�^���T�C�Y�i�o�C�g�j
 *
 * @retval  u32		�Ǘ��P�ʃo�C�g��
 */
//------------------------------------------------------------------
static u32 calc_objtype_unit_bytesize( u32 vramSize )
{
	vramSize /= 1024;

	if( vramSize <= 32 )
	{
		return 32;
	}
	return vramSize;
}


//==============================================================================================
//==============================================================================================


//==============================================================================================
/**
 * �\��̈���\���̂̏������i���������������Ȓl���ۑ�����Ă����ԁj
 *
 * @param   reserveInfo		[in] ����������\���̂ւ̃|�C���^
 *
 */
//==============================================================================================
void GFL_VMAN_InitReserveInfo( GFL_VMAN_RESERVE_INFO* reserveInfo )
{
	reserveInfo->pos = RESERVEINFO_INIT_POS;
	reserveInfo->size = RESERVEINFO_INIT_SIZE;
}
//==============================================================================================
/**
 * �\��̈���\���̂�����������Ă��邩���ׂ�
 *
 * @param   reserveInfo		[in] �\���̂ւ̃|�C���^
 *
 * @retval  BOOL			TRUE�Ȃ珉��������Ă���
 */
//==============================================================================================
BOOL GFL_VMAN_IsReserveInfoInitialized( const GFL_VMAN_RESERVE_INFO* reserveInfo )
{
	return (reserveInfo->pos==RESERVEINFO_INIT_POS) && (reserveInfo->size==RESERVEINFO_INIT_SIZE);
}



//==============================================================================================
/**
 * �̈��\�񂷂�i�擪����I�[�܂ŁA�󂢂Ă��鏊��T���j
 *
 * @param   man			[in]  �}�l�[�W��
 * @param   size		[in]  �\�񂵂����o�C�g�T�C�Y
 * @param   reserveInfo	[out] �\��̈����ۑ����邽�߂̍\����
 *
 * @retval  u32			�\��ł�����TRUE, �ł��Ȃ�������FALSE
 */
//==============================================================================================
BOOL GFL_VMAN_Reserve( GFL_VMAN* man, u32 byteSize, GFL_VMAN_RESERVE_INFO* reserveInfo )
{
	GF_ASSERT( man != NULL );
	GF_ASSERT( reserveInfo != NULL );
	GF_ASSERT( (byteSize % man->unitByteSize) == 0 );

	{
		AREAMAN_POS pos;
		u32 blocks;

		blocks = byteSize / man->unitByteSize;
		pos = AREAMAN_ReserveAuto( man->areaMan, blocks );

		if( pos != AREAMAN_POS_NOTFOUND )
		{
			reserveInfo->pos = pos;
			reserveInfo->size = blocks;
			return TRUE;
		}

		return FALSE;
	}
}
//==============================================================================================
/**
 * �̈��\�񂷂�i�w��͈͓��ŁA�󂢂Ă��鏊��T���j
 *
 * @param   man				[in]  �}�l�[�W��
 * @param   reserveSize		[in]  �\�񂵂����o�C�g�T�C�Y
 * @param   startOffset		[in]  �T���J�n�̈�o�C�g�I�t�Z�b�g
 * @param   areaSize		[in]  �T���͈̓o�C�g�T�C�Y
 * @param   reserveInfo		[out] �\��̈����ۑ����邽�߂̍\���̃|�C���^
 *
 * @retval  u32			�\��ł�����TRUE, �ł��Ȃ�������FALSE
 */
//==============================================================================================
BOOL GFL_VMAN_ReserveAssignArea( GFL_VMAN* man, u32 reserveSize, u32 startOffset, u32 areaSize, GFL_VMAN_RESERVE_INFO* reserveInfo )
{
	GF_ASSERT( man != NULL );
	GF_ASSERT( reserveInfo != NULL );
	GF_ASSERT( (reserveSize % man->unitByteSize) == 0 );
	GF_ASSERT( (startOffset % man->unitByteSize) == 0 );
	GF_ASSERT( (areaSize % man->unitByteSize) == 0 );

	{
		AREAMAN_POS pos;

		startOffset /= man->unitByteSize;
		areaSize /= man->unitByteSize;
		reserveSize /= man->unitByteSize;

		pos = AREAMAN_ReserveAssignArea( man->areaMan, startOffset, areaSize, reserveSize );

		if( pos != AREAMAN_POS_NOTFOUND )
		{
			reserveInfo->pos = pos;
			reserveInfo->size = reserveSize;
			return TRUE;
		}
	}

	return FALSE;
}

//==============================================================================================
/**
 * 
 *
 * @param   man				
 * @param   reserveSize		
 * @param   offset			
 * @param   reserveInfo		
 *
 * @retval  BOOL		
 */
//==============================================================================================
BOOL GFL_VMAN_ReserveFixPos( GFL_VMAN* man, u32 reserveSize, u32 offset, GFL_VMAN_RESERVE_INFO* reserveInfo )
{
	GF_ASSERT( man != NULL );
	GF_ASSERT( reserveInfo != NULL );
	GF_ASSERT( (reserveSize % man->unitByteSize) == 0 );
	GF_ASSERT( (offset % man->unitByteSize) == 0 );

	offset /= man->unitByteSize;
	reserveSize /= man->unitByteSize;

	if( AREAMAN_ReserveAssignPos( man->areaMan, offset, reserveSize ) )
	{
		reserveInfo->pos = offset;
		reserveInfo->size = reserveSize;
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------
/**
 * �o�C�g�����Ǘ��u���b�N���֕ϊ�
 *
 * @param   byteSize		�o�C�g��
 * @param   unitSize		�P�ʃo�C�g��
 *
 * @retval  u32		�Ǘ��u���b�N��
 */
//------------------------------------------------------------------
static inline u32 calc_block_size( u32 byteSize, u32 unitSize )
{
	if( byteSize <= unitSize )
	{
		return 1;
	}
	else
	{
		return byteSize / unitSize;
	}
}

//==============================================================================================
/**
 * �\�񂵂��̈�̉��
 *
 * @param   man			[in] �}�l�[�W��
 * @param   reserveInfo	[in] �\��̈���
 *
 */
//==============================================================================================
void GFL_VMAN_Release( GFL_VMAN* man, GFL_VMAN_RESERVE_INFO* reserveInfo )
{
	GF_ASSERT(man != NULL);
	GF_ASSERT(reserveInfo!=NULL);
	GF_ASSERT(GFL_VMAN_IsReserveInfoInitialized(reserveInfo)==FALSE);

	AREAMAN_Release( man->areaMan, reserveInfo->pos, reserveInfo->size );
	GFL_VMAN_InitReserveInfo( reserveInfo );
}

//==============================================================================================
/**
 * �\�񂵂��̈�́AVRAM�擪����̃o�C�g�I�t�Z�b�g�����߂�
 *
 * @param   man				[in] �}�l�[�W��
 * @param   reserveInfo		[in] �\��̈���
 *
 * @retval  u32				VRAM�擪����̃o�C�g�I�t�Z�b�g
 */
//==============================================================================================
u32 GFL_VMAN_GetByteOffset( GFL_VMAN* man, const GFL_VMAN_RESERVE_INFO* reserveInfo )
{
	GF_ASSERT(man != NULL);
	GF_ASSERT(reserveInfo!=NULL);
	GF_ASSERT(reserveInfo->pos != RESERVEINFO_INIT_POS);

	return reserveInfo->pos * man->unitByteSize;
}
