//==============================================================================
/**
 *
 *@file		obj_graphic_man.c
 *@brief	OBJ�p�O���t�B�b�N�f�[�^�]���A�Ǘ�
 *@author	taya
 *@data		2006.11.28
 *
 */
//==============================================================================
#include "gflib.h"
#include "vman.h"

#include "obj_graphic_man.h"


//==============================================================
//	CGR�f�[�^�Ǘ�
//==============================================================
typedef struct {
	NNSG2dImageProxy			proxy;			///< �]���v���L�V
	void*						loadPtr;		///< �f�[�^�|�C���^
	NNSG2dCharacterData*		g2dCharData;	///< NNS�L�����f�[�^�|�C���^
	GFL_VMAN_RESERVE_INFO		riMain;			///< �\��̈��񃁃C��
	GFL_VMAN_RESERVE_INFO		riSub;			///< �\��̈���T�u
	BOOL						emptyFlag;		///< ���g�p�t���O
}CGR_MAN;

//==============================================================
//	�p���b�g�f�[�^�Ǘ�
//==============================================================
typedef struct {
	NNSG2dImagePaletteProxy		proxy;			///< �]���v���L�V
	BOOL						emptyFlag;		///< ���g�p�t���O
}PLTT_MAN;

//==============================================================
//	�Z���A�j���f�[�^�Ǘ�
//==============================================================
typedef struct {
	void*						cellLoadPtr;	///< �Z���f�[�^�|�C���^
	void*						animLoadPtr;	///< �A�j���f�[�^�|�C���^
	NNSG2dCellDataBank*			cellBankPtr;	///< �Z���o���N�|�C���^
	NNSG2dAnimBankData*			animBankPtr;	///< �A�j���o���N�|�C���^
	BOOL						emptyFlag;		///< ���g�p�t���O
}CELLANIM_MAN;

//==============================================================
//	�V�X�e�����[�N
//==============================================================
static struct {
	GFL_VMAN*	vmanMain;
	GFL_VMAN*	vmanSub;

	CGR_MAN*		cgrMan;
	PLTT_MAN*		plttMan;
	CELLANIM_MAN*	cellAnimMan;

	GFL_OBJGRP_INIT_PARAM		initParam;

}SysWork;

//==============================================================
// Prototype
//==============================================================
static CGR_MAN* create_cgr_man( u16 heapID, u32 num );
static inline u32 search_empty_cgr_pos( void );
static void register_cgr(	u32 idx, void* dataPtr, GFL_VRAM_TYPE targetVram, 
							NNSG2dCellDataBank* cellBankPtr );
static inline void trans_ncgr(	NNSG2dCharacterData* charData, u32 byteOfs, 
								NNS_G2D_VRAM_TYPE vramType, BOOL vramTransferFlag, 
								NNSG2dImageProxy* proxy  );
static void delete_cgr_man( CGR_MAN* man );
static PLTT_MAN* create_pltt_man( u16 heapID, u32 num );
static inline u32 search_empty_pltt_pos( void );
static void register_pltt( u32 idx, void* dataPtr, GFL_VRAM_TYPE vramType, u32 byteOffs );
static void delete_pltt_man( PLTT_MAN* man );
static CELLANIM_MAN* create_cellanim_man( u16 heapID, u32 num );
static inline u32 search_empty_cellanim_pos( void );
static BOOL register_cellanim( u32 idx, void* cellDataPtr, void* animDataPtr );
static void delete_cellanim_man( CELLANIM_MAN* man );

//------------------------------------------------------------------
/**
 * �V�X�e���������i���C���N�����ɂP�񂾂��Ăԁj
 */
//------------------------------------------------------------------
void GFL_OBJGRP_sysInit( void )
{
	SysWork.vmanMain = NULL;
	SysWork.vmanSub = NULL;
	SysWork.cgrMan = NULL;
	SysWork.plttMan = NULL;
	SysWork.cellAnimMan = NULL;
}

void GFL_OBJGRP_sysExit( void )
{
	
}

//==============================================================================================
/**
 * �v���Z�X���̏���������
 *
 * @param   heapID			[in] �g�p�q�[�vID
 * @param   vramBank		[in] VRAM�o���N�ݒ�
 * @param   initParam		[in] �������\����
 *
 */
//==============================================================================================
void GFL_OBJGRP_sysStart
	( u16 heapID, const GFL_BG_DISPVRAM* vramBank, const GFL_OBJGRP_INIT_PARAM* initParam )
{
	GF_ASSERT( SysWork.vmanMain == NULL );
	GF_ASSERT( SysWork.vmanSub == NULL );
	GF_ASSERT( SysWork.cgrMan == NULL );
	GF_ASSERT( SysWork.plttMan == NULL );
	GF_ASSERT( SysWork.cellAnimMan == NULL );

	SysWork.vmanMain = GFL_VMAN_Create( heapID, GFL_VMAN_TYPE_OBJ, vramBank->main_obj );
	SysWork.vmanSub = GFL_VMAN_Create( heapID, GFL_VMAN_TYPE_OBJ, vramBank->sub_obj );
	SysWork.cgrMan = create_cgr_man( heapID, initParam->CGR_RegisterMax );
	SysWork.plttMan = create_pltt_man( heapID, initParam->PLTT_RegisterMax );
	SysWork.cellAnimMan = create_cellanim_man( heapID, initParam->CELL_RegisterMax );

	SysWork.initParam = *initParam;
}

//==============================================================================================
/**
 * �v���Z�X���̏I������
 */
//==============================================================================================
void GFL_OBJGRP_sysEnd( void )
{
	GF_ASSERT( SysWork.vmanMain != NULL );
	GF_ASSERT( SysWork.vmanSub != NULL );
	GF_ASSERT( SysWork.cgrMan != NULL );
	GF_ASSERT( SysWork.plttMan != NULL );
	GF_ASSERT( SysWork.cellAnimMan != NULL );

	delete_cgr_man( SysWork.cgrMan );
	delete_pltt_man( SysWork.plttMan );
	delete_cellanim_man( SysWork.cellAnimMan );
	GFL_VMAN_Delete( SysWork.vmanSub );
	GFL_VMAN_Delete( SysWork.vmanMain );

	SysWork.vmanMain = NULL;
	SysWork.vmanSub = NULL;
	SysWork.cgrMan = NULL;
	SysWork.plttMan = NULL;
	SysWork.cellAnimMan = NULL;
}
//==============================================================================================
// �A�[�J�C�u����̃f�[�^�ǂݏo���E�𓀏���
//==============================================================================================
static void* ReadDataWithUncompress( ARCHANDLE* arc, u32 dataID, BOOL compressFlag, HEAPID heapID )
{
	void* retBuf;

	if( compressFlag )
	{
		void* tmpBuf;
		u32 size;

		tmpBuf = GFL_ARC_DataLoadAllocByHandle( arc, dataID, HeapGetLow(heapID) );
		size = MI_GetUncompressedSize( tmpBuf );
		retBuf = GFL_HEAP_AllocMemory( heapID, size );
		MI_UncompressLZ8( tmpBuf, retBuf );
		GFL_HEAP_FreeMemory( tmpBuf );
	}
	else
	{
		retBuf = GFL_ARC_DataLoadAllocByHandle( arc, dataID, HeapGetLow(heapID) );
	}

	return retBuf;
}

//==============================================================================================
// �f�[�^�o�^����щ���֐��Q
//==============================================================================================

//==============================================================================================
/**
 * CGR�f�[�^�̓o�^�iVRAM�풓�^OBJ�p�j
 *
 * �A�[�J�C�u����CGR�f�[�^�����[�h����VRAM�]�����s���A�v���L�V���쐬���ĕێ�����B
 * CGR�f�[�^�̎��͔̂j������B
 *
 * @param   arcHandle		[in] �A�[�J�C�u�n���h��
 * @param   cgrDataID		[in] �A�[�J�C�u����CGR�f�[�^ID
 * @param   compressedFlag	[in] �f�[�^�����k����Ă��邩
 * @param   targetVram		[in] �]����VRAM�w��
 * @param   heapID			[in] �f�[�^���[�h�p�q�[�v�i�e���|�����j
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_OBJGRP_REGISTER_FAILED�j
 */
//==============================================================================================
u32 GFL_OBJGRP_RegisterCGR( ARCHANDLE* arcHandle, u32 cgrDataID, BOOL compressedFlag, GFL_VRAM_TYPE targetVram, HEAPID heapID )
{
	u32  idx = search_empty_cgr_pos();
	if( idx != GFL_OBJGRP_REGISTER_FAILED )
	{
		CGR_MAN* cgrMan = &SysWork.cgrMan[idx];
		void* loadPtr = ReadDataWithUncompress( arcHandle, cgrDataID, compressedFlag, HeapGetLow(heapID) );

		register_cgr( idx, loadPtr, targetVram, NULL );
		GFL_HEAP_FreeMemory( loadPtr );
		cgrMan->loadPtr = NULL;

		return idx;
	}
	GF_ASSERT(0);
	return GFL_OBJGRP_REGISTER_FAILED;
}

//==============================================================================================
/**
 * CGR�f�[�^�̓o�^�iVRAM�]���^OBJ�p�j
 *
 * �A�[�J�C�u����CGR�f�[�^�����[�h���A�v���L�V���쐬���ĕێ�����B
 * CGR�f�[�^�̎��̂��ێ���������B
 * CGR�f�[�^�Ɋ֘A�t����ꂽ�Z���A�j���f�[�^����ɓo�^����Ă���K�v������B
 *
 * @param   arcHandle		[in] �A�[�J�C�u�n���h��
 * @param   cgrDataID		[in] CGR�f�[�^�̃A�[�J�C�u��ID
 * @param   targetVram		[in] �]����VRAM�w��
 * @param   cellIndex		[in] ���ɓo�^����Ă���Z���f�[�^�̓o�^�C���f�b�N�X
 * @param   heapID			[in] �f�[�^���[�h�p�q�[�v
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_OBJGRP_REGISTER_FAILED�j
 */
//==============================================================================================
u32 GFL_OBJGRP_RegisterCGR_VramTransfer( ARCHANDLE* arcHandle, u32 cgrDataID, BOOL compressedFlag, GFL_VRAM_TYPE targetVram, u32 cellIndex, HEAPID heapID )
{
	GF_ASSERT(SysWork.cellAnimMan[cellIndex].emptyFlag == FALSE);
	GF_ASSERT(NNS_G2dCellDataBankHasVramTransferData(SysWork.cellAnimMan[cellIndex].cellBankPtr));

	{
		u32 idx = search_empty_cgr_pos();
		if( idx != GFL_OBJGRP_REGISTER_FAILED )
		{
			CGR_MAN* cgrMan = &SysWork.cgrMan[idx];

			cgrMan->loadPtr = ReadDataWithUncompress( arcHandle, cgrDataID, compressedFlag, heapID );
			register_cgr(idx, cgrMan->loadPtr, targetVram, SysWork.cellAnimMan[cellIndex].cellBankPtr);
			return idx;
		}

		GF_ASSERT(0);
		return GFL_OBJGRP_REGISTER_FAILED;
	}
}

//==============================================================================================
/**
 * �o�^���ꂽCGR�f�[�^�̉��
 *
 * @param   index		[in] �o�^�C���f�b�N�X
 */
//==============================================================================================
void GFL_OBJGRP_ReleaseCGR( u32 index )
{
	GF_ASSERT( index < SysWork.initParam.CGR_RegisterMax );
	GF_ASSERT( SysWork.cgrMan[index].emptyFlag == FALSE );

	if( GFL_VMAN_IsReserveInfoInitialized(&SysWork.cgrMan[index].riMain) == FALSE )
	{
		GFL_VMAN_Release( SysWork.vmanMain, &SysWork.cgrMan[index].riMain );
	}
	if( GFL_VMAN_IsReserveInfoInitialized(&SysWork.cgrMan[index].riSub) == FALSE )
	{
		GFL_VMAN_Release( SysWork.vmanSub, &SysWork.cgrMan[index].riSub );
	}

	if( SysWork.cgrMan[index].loadPtr )
	{
		GFL_HEAP_FreeMemory( SysWork.cgrMan[index].loadPtr );
		SysWork.cgrMan[index].loadPtr = NULL;
	}

	SysWork.cgrMan[index].emptyFlag = TRUE;
}



//==============================================================================================
/**
 * �Z���A�j���f�[�^�o�^
 *
 * �A�[�J�C�u����Z�����A�j���f�[�^�����[�h���AUnpack���Ď��̂��ƕێ�����B
 *
 * @param   arcHandle		�A�[�J�C�u�n���h��
 * @param   cellDataID		�Z���f�[�^ID
 * @param   animDataID		�A�j���f�[�^ID
 * @param   heapID			�q�[�vID
 *
 * @retval  �o�^�C���f�b�N�X
 *
 */
//==============================================================================================
u32 GFL_OBJGRP_RegisterCellAnim( ARCHANDLE* arcHandle, u32 cellDataID, u32 animDataID, u16 heapID )
{
	u32 idx = search_empty_cellanim_pos();

	{
		CELLANIM_MAN* man = &SysWork.cellAnimMan[idx];

		man->cellLoadPtr = GFL_ARC_DataLoadAllocByHandle( arcHandle, cellDataID, heapID );
		man->animLoadPtr = GFL_ARC_DataLoadAllocByHandle( arcHandle, animDataID, heapID );

		if( register_cellanim( idx, man->cellLoadPtr, man->animLoadPtr ) == FALSE )
		{
			GF_ASSERT(0);
		}

		return idx;
	}
}
//==============================================================================================
/**
 * �o�^���ꂽ�Z���A�j���f�[�^�̉��
 *
 * @param   index		[in] �o�^�C���f�b�N�X
 */
//==============================================================================================
void GFL_OBJGRP_ReleaseCellAnim( u32 index )
{
	GF_ASSERT( index < SysWork.initParam.CELL_RegisterMax );
	GF_ASSERT( SysWork.cellAnimMan[index].emptyFlag == FALSE );

	{
		CELLANIM_MAN* man = &SysWork.cellAnimMan[index];

		if( man->cellLoadPtr )
		{
			GFL_HEAP_FreeMemory( man->cellLoadPtr );
			man->cellLoadPtr = NULL;
		}
		if( man->animLoadPtr )
		{
			GFL_HEAP_FreeMemory( man->animLoadPtr );
			man->animLoadPtr = NULL;
		}

		man->emptyFlag = TRUE;
	}
}

//==============================================================================================
/**
 * �w�肳�ꂽ�Z���f�[�^���AVRAM�]���^���ǂ������`�F�b�N
 *
 * @param   index		[in] �o�^�C���f�b�N�X
 *
 * @retval  BOOL		TRUE�Ȃ�VRAM�]���^�f�[�^
 */
//==============================================================================================
BOOL GFL_OBJGRP_CellBankHasVramTransferData( u32 index )
{
	GF_ASSERT( index < SysWork.initParam.CELL_RegisterMax );
	GF_ASSERT( SysWork.cellAnimMan[index].emptyFlag == FALSE );

	return NNS_G2dCellDataBankHasVramTransferData( SysWork.cellAnimMan[index].cellBankPtr );
}



//==============================================================================================
/**
 * �p���b�g�f�[�^�̓o�^�����VRAM�ւ̓]��
 *
 * �p���b�g�f�[�^���A�[�J�C�u���烍�[�h����VRAM�]�����s���A�v���L�V���쐬���ĕێ�����B
 * ���[�h�����f�[�^���͔̂j������B
 *
 * @param   arcHandle		[in] �A�[�J�C�u�n���h��
 * @param   plttDataID		[in] �p���b�g�f�[�^�̃A�[�J�C�u���f�[�^ID
 * @param   vramType		[in] �]����VRAM�w��
 * @param   byteOffs		[in] �]���I�t�Z�b�g�i�o�C�g�j
 * @param   heapID			[in] �f�[�^���[�h�p�q�[�v�i�e���|�����j
 *
 * OBJ�g���p���b�g���w�肵�����ꍇ�A byteOffs�̒l�� GFL_OBJGRP_EXPLTT_OFFSET + �I�t�Z�b�g�o�C�g�����w�肷��
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_OBJGRP_REGISTER_FAILED�j
 *
 */
//==============================================================================================
u32 GFL_OBJGRP_RegisterPltt( ARCHANDLE* arcHandle, u32 plttDataID, GFL_VRAM_TYPE vramType, u32 byteOffs, u16 heapID )
{
	u32 idx = search_empty_pltt_pos();
	if( idx != GFL_OBJGRP_REGISTER_FAILED )
	{
		void* loadPtr = GFL_ARC_DataLoadAllocByHandle( arcHandle, plttDataID, HeapGetLow(heapID) );
		register_pltt( idx, loadPtr, vramType, byteOffs );
		GFL_HEAP_FreeMemory( loadPtr );
		return idx;
	}

	return idx;
}

//==============================================================================================
/**
 * �o�^���ꂽ�p���b�g�f�[�^�̉��
 *
 * @param   index		[in] �o�^�C���f�b�N�X
 */
//==============================================================================================
void GFL_OBJGRP_ReleasePltt( u32 index )
{
	GF_ASSERT( index < SysWork.initParam.PLTT_RegisterMax );
	GF_ASSERT( SysWork.plttMan[index].emptyFlag == FALSE );

	SysWork.plttMan[index].emptyFlag = TRUE;
}

//-------------------------------------
///	CLWK��������{�f�[�^
//=====================================
/*
typedef struct {
	s16	pos_x;				// �����W
	s16 pos_y;				// �����W
	u16 anmseq;				// �A�j���[�V�����V�[�P���X
	u8	softpri;			// �\�t�g�D�揇��	0>0xff
	u8	bgpri;				// BG�D�揇��
} CLWK_DATA;
*/

//==============================================================================================
// �o�^�f�[�^�ɂ��Z���A�N�^�[�\�z
//==============================================================================================
//==============================================================================================
/**
 * �o�^�f�[�^�ɂ��Z���A�N�^�[�\�z�iVRAM�풓�^�C�v�j
 *
 * @param   actUnit			[in] �A�N�^�[���j�b�g�|�C���^
 * @param   cgrIndex		[in] �g�p����CGR�f�[�^�̓o�^�C���f�b�N�X
 * @param   plttIndex		[in] �g�p����p���b�g�f�[�^�̓o�^�C���f�b�N�X
 * @param   cellAnimIndex	[in] �g�p����Z���A�j���f�[�^�̓o�^�C���f�b�N�X
 * @param   param			[in] �A�N�^�[�o�^�p�����[�^
 * @param   setSerface		[in] �A�N�^�[���Z�b�g����T�[�t�F�C�X�w��iclact.h�Q�Ɓj
 * @param   heapID			[in] �A�N�^�[�쐬�p�q�[�vID
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_OBJGRP_REGISTER_FAILED�j
 */
//==============================================================================================
CLWK* GFL_OBJGRP_CreateClAct
	( CLUNIT* actUnit, u32 cgrIndex, u32 plttIndex, u32 cellAnimIndex, 
	  const CLWK_DATA* param, u16 setSerface, u16 heapID )
{
	GF_ASSERT( cgrIndex < SysWork.initParam.CGR_RegisterMax );
	GF_ASSERT( plttIndex < SysWork.initParam.PLTT_RegisterMax );
	GF_ASSERT( cellAnimIndex < SysWork.initParam.CELL_RegisterMax );
	GF_ASSERT( SysWork.cgrMan[cgrIndex].emptyFlag == FALSE );
	GF_ASSERT_MSG( SysWork.plttMan[plttIndex].emptyFlag == FALSE, "plttIndex=%d", plttIndex );
	GF_ASSERT( SysWork.cellAnimMan[cellAnimIndex].emptyFlag == FALSE );


	{
		CLWK_RES     clactRes;

		GFL_CLACT_WkSetCellResData( &clactRes,
					&(SysWork.cgrMan[cgrIndex].proxy),
					&(SysWork.plttMan[plttIndex].proxy),
					SysWork.cellAnimMan[cellAnimIndex].cellBankPtr,
					SysWork.cellAnimMan[cellAnimIndex].animBankPtr
		);

		return GFL_CLACT_WkAdd( actUnit, param, &clactRes, setSerface, heapID );
	}
}

//==============================================================================================
/**
 * �o�^�f�[�^�ɂ��Z���A�N�^�[�\�z�iVRAM�]���^�C�v�j
 *
 * @param   actUnit			[in] �A�N�^�[���j�b�g�|�C���^
 * @param   cgrIndex		[in] �g�p����CGR�f�[�^�̓o�^�C���f�b�N�X
 * @param   plttIndex		[in] �g�p����p���b�g�f�[�^�̓o�^�C���f�b�N�X
 * @param   cellAnimIndex	[in] �g�p����Z���A�j���f�[�^�̓o�^�C���f�b�N�X
 * @param   param			[in] �A�N�^�[�o�^�p�����[�^
 * @param   setSerface		[in] �A�N�^�[���Z�b�g����T�[�t�F�C�X�w��iclact.h�Q�Ɓj
 * @param   heapID			[in] �A�N�^�[�쐬�p�q�[�vID
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_OBJGRP_REGISTER_FAILED�j
 */
//==============================================================================================

CLWK* GFL_OBJGRP_CreateClActVT
	( CLUNIT* actUnit, u32 cgrIndex, u32 plttIndex, u32 cellAnimIndex, 
	  const CLWK_DATA* param, u16 setSerface, u16 heapID )
{
	GF_ASSERT( cgrIndex < SysWork.initParam.CGR_RegisterMax );
	GF_ASSERT( plttIndex < SysWork.initParam.PLTT_RegisterMax );
	GF_ASSERT( cellAnimIndex < SysWork.initParam.CELL_RegisterMax );
	GF_ASSERT( SysWork.cgrMan[cgrIndex].emptyFlag == FALSE );
	GF_ASSERT_MSG( SysWork.plttMan[plttIndex].emptyFlag == FALSE, "plttIndex=%d", plttIndex );
	GF_ASSERT( SysWork.cellAnimMan[cellAnimIndex].emptyFlag == FALSE );


	{
		CLWK_RES     clactRes;

		GFL_CLACT_WkSetTrCellResData( &clactRes,
					&(SysWork.cgrMan[cgrIndex].proxy),
					&(SysWork.plttMan[plttIndex].proxy),
					SysWork.cellAnimMan[cellAnimIndex].cellBankPtr,
					SysWork.cellAnimMan[cellAnimIndex].animBankPtr,
					SysWork.cgrMan[cgrIndex].g2dCharData
		);

		return GFL_CLACT_WkAdd( actUnit, param, &clactRes, setSerface, heapID );
	}
}


//==============================================================================================
//==============================================================================================
//------------------------------------------------------------------
/**
 * CGR�Ǘ��̈�̍\�z�A������
 *
 * @param   heapID		�q�[�vID
 * @param   num			�Ǘ�����CGR�f�[�^��
 *
 * @retval  CGR_MAN*	�Ǘ��̈�|�C���^
 */
//------------------------------------------------------------------
static CGR_MAN* create_cgr_man( u16 heapID, u32 num )
{
	u32 i;
	CGR_MAN* man = GFL_HEAP_AllocMemory( heapID, sizeof(CGR_MAN)*num );
	for(i=0; i<num; i++)
	{
		man[i].emptyFlag = TRUE;
	}
	return man;
}

//------------------------------------------------------------------
/**
 * CGR�Ǘ��̈悩��󂫂�T��
 *
 * @retval  u32		��index
 */
//------------------------------------------------------------------
static inline u32 search_empty_cgr_pos( void )
{
	u32 i;
	for(i=0; i<SysWork.initParam.CGR_RegisterMax; i++)
	{
		if( SysWork.cgrMan[i].emptyFlag )
		{
			return i;
		}
	}
	return GFL_OBJGRP_REGISTER_FAILED;
}

//------------------------------------------------------------------
/**
 * CGR�o�^�A�]��
 *
 * @param   dataPtr			
 * @param   targetVram		
 * @param   cellDataPtr		VRAM�]���^�̎��A�ő�̈�T�C�Y���擾����̂Ɏg�p�B
 *							VRAM�풓�^�Ȃ�ANULL���w�肷�邱�ƁB
 *
 * @retval  BOOL			������TRUE
 */
//------------------------------------------------------------------
static void register_cgr
	( u32 idx, void* dataPtr, GFL_VRAM_TYPE targetVram, NNSG2dCellDataBank* cellBankPtr )
{
	NNSG2dCharacterData* charData;

	if( NNS_G2dGetUnpackedBGCharacterData( dataPtr, &charData ) )
	{
		CGR_MAN* cgrMan = &SysWork.cgrMan[idx];
		u32 szByte;
		BOOL vramTransferFlag;

		if( cellBankPtr == NULL )
		{
			szByte = charData->szByte;
			vramTransferFlag = FALSE;
		}
		else
		{
			szByte = NNS_G2dGetSizeRequiredVramTransferCellDataBank( cellBankPtr );
			vramTransferFlag = TRUE;
		}

		NNS_G2dInitImageProxy( &cgrMan->proxy );

		if( targetVram & GFL_VRAM_2D_MAIN )
		{
			if( GFL_VMAN_Reserve( SysWork.vmanMain, szByte, &cgrMan->riMain ) )
			{
				u32 byteOfs = GFL_VMAN_GetByteOffset( SysWork.vmanMain, &cgrMan->riMain );
				trans_ncgr( charData, byteOfs, NNS_G2D_VRAM_TYPE_2DMAIN, 
							vramTransferFlag, &cgrMan->proxy );
			}
		}
		if( targetVram & GFL_VRAM_2D_SUB )
		{
			if( GFL_VMAN_Reserve( SysWork.vmanSub, szByte, &cgrMan->riSub ) )
			{
				u32 byteOfs = GFL_VMAN_GetByteOffset( SysWork.vmanSub, &cgrMan->riSub );
				trans_ncgr( charData, byteOfs, NNS_G2D_VRAM_TYPE_2DSUB, 
							vramTransferFlag, &cgrMan->proxy );
			}
		}

		SysWork.cgrMan[idx].g2dCharData = ( vramTransferFlag )? charData : NULL;
		SysWork.cgrMan[idx].emptyFlag = FALSE;

	}
	else
	{
		GF_ASSERT(0);
	}
}

//------------------------------------------------------------------
/**
 * CGR�]���R�A
 *
 * @param   charData			[in] NITROSystem CharData
 * @param   byteOfs				[in] VRAM�擪����̓]���I�t�Z�b�g
 * @param   vramType			[in] �]����VRAM 2D-Main, 2D-Sub
 * @param   vramTransferFlag	[in] VRAM�]���^�Ȃ�TRUE���w��
 * @param   proxy				[out] �\�z����v���L�V�A�h���X
 *
 */
//------------------------------------------------------------------
static inline void trans_ncgr
	( NNSG2dCharacterData* charData, u32 byteOfs, NNS_G2D_VRAM_TYPE vramType, 
	  BOOL vramTransferFlag, NNSG2dImageProxy* proxy  )
{
	if( vramTransferFlag )
	{
//		OS_TPrintf("trans cgr VT  ofs:0x%08x, type:%d\n", byteOfs, vramType);
		NNS_G2dLoadImageVramTransfer( charData, byteOfs, vramType, proxy );
	}
	else
	{
		if( charData->mapingType != GX_OBJVRAMMODE_CHAR_2D )
		{
//			OS_TPrintf("trans cgr 1d  ofs:0x%08x, type:%d\n", byteOfs, vramType);
			NNS_G2dLoadImage1DMapping( charData, byteOfs, vramType, proxy );
		}
		else
		{
//			OS_TPrintf("trans cgr 2d  ofs:0x%08x, type:%d\n", byteOfs, vramType);
			NNS_G2dLoadImage2DMapping( charData, byteOfs, vramType, proxy );
		}
	}
}

//------------------------------------------------------------------
/**
 * CGR�Ǘ��̈�̔j��
 *
 * @param   man			�Ǘ��̈�|�C���^
 */
//------------------------------------------------------------------
static void delete_cgr_man( CGR_MAN* man )
{
	GFL_HEAP_FreeMemory( man );
}

//------------------------------------------------------------------
/**
 * �p���b�g�Ǘ��̈�̍\�z�A������
 *
 * @param   heapID		�q�[�vID
 * @param   num			�Ǘ�����CGR�f�[�^��
 *
 * @retval  CGR_MAN*	�Ǘ��̈�|�C���^
 */
//------------------------------------------------------------------
static PLTT_MAN* create_pltt_man( u16 heapID, u32 num )
{
	u32 i;
	PLTT_MAN* man = GFL_HEAP_AllocMemory( heapID, sizeof(PLTT_MAN)*num );

	for(i=0; i<num; i++)
	{
		man[i].emptyFlag = TRUE;
	}
	return man;
}

//------------------------------------------------------------------
/**
 * �p���b�g�Ǘ��̈悩��󂫂�T��
 *
 * @retval  u32		��index
 */
//------------------------------------------------------------------
static inline u32 search_empty_pltt_pos( void )
{
	u32 i;
	for(i=0; i<SysWork.initParam.PLTT_RegisterMax; i++)
	{
		if( SysWork.plttMan[i].emptyFlag )
		{
			return i;
		}
	}
	return GFL_OBJGRP_REGISTER_FAILED;
}

//------------------------------------------------------------------
/**
 * �p���b�g�f�[�^�̓o�^���]��
 *
 * @param   plttData		[in] �p���b�g�f�[�^
 * @param   vramType		[in] �]����VRAM�w��
 * @param   byteOffs		[in] �]���J�n�I�t�Z�b�g
 *
 */
//------------------------------------------------------------------
static void register_pltt( u32 idx, void* dataPtr, GFL_VRAM_TYPE vramType, u32 byteOffs )
{
	NNSG2dPaletteData*  palData;
	NNSG2dPaletteCompressInfo*  cmpData;
	BOOL  cmpFlag, exFlag;

	PLTT_MAN* man = &SysWork.plttMan[idx];

	// �g���p���b�g����
	if( byteOffs < GFL_OBJGRP_EXPLTT_OFFSET )
	{
		exFlag = FALSE;
	}
	else
	{
		exFlag = TRUE;
		byteOffs -= GFL_OBJGRP_EXPLTT_OFFSET;
	}

	cmpFlag = NNS_G2dGetUnpackedPaletteCompressInfo( dataPtr, &cmpData );

	if( NNS_G2dGetUnpackedPaletteData( dataPtr, &palData ) )
	{
		NNS_G2dInitImagePaletteProxy( &man->proxy );

		if( cmpFlag )
		{
			if( vramType & GFL_VRAM_2D_MAIN )
			{
				/*
					�g���p���b�g�]�������Ɋւ���o������

					NitroSDK�̃}�j���A���iGX_SetBankForOBJExtPltt �Ƃ� GX_SetBankForLCDC �̂�����j��ǂތ���A
					�g���p���b�g�ւ̃o���N���蓖�Ă�������LCDC�Ƀo���N���蓖�ā�
					�p���b�g�f�[�^�]�����g���p���b�g�ւ̃o���N���蓖�ĕ��A
					�Ƃ��Ȃ���΂Ȃ�Ȃ��悤�Ɏv���邪�A
					�g���p���b�g�ւ̃o���N���蓖�Ă����������i��GX_SetBankForOBJExtPltt(0);)
					���s���Ɛ��������삵�Ȃ��B�ȉ��̂悤�ɍŒ���̏����̂ݍs���Ɠ��삷��悤�ł���B
					�T�u2D�G���W�������l�B

					taya 07/02/21
				*/
				int bank = GX_GetBankForOBJExtPltt();
				if( exFlag ){ GX_SetBankForLCDC( bank ); }
				NNS_G2dLoadPaletteEx(	palData, cmpData, byteOffs, 
										NNS_G2D_VRAM_TYPE_2DMAIN, &man->proxy );
				if( exFlag ){ GX_SetBankForOBJExtPltt( bank ); }
			}
			if( vramType & GFL_VRAM_2D_SUB )
			{
				int bank = GX_GetBankForSubOBJExtPltt();
				if( exFlag ){ GX_SetBankForLCDC( bank ); }
				NNS_G2dLoadPaletteEx(	palData, cmpData, byteOffs, 
										NNS_G2D_VRAM_TYPE_2DSUB, &man->proxy );
				if( exFlag ){ GX_SetBankForSubOBJExtPltt( bank ); }
			}
		}
		else
		{
			if( vramType & GFL_VRAM_2D_MAIN )
			{
				int bank = GX_GetBankForOBJExtPltt();
				if( exFlag ){ GX_SetBankForLCDC( bank ); }
				NNS_G2dLoadPalette( palData, byteOffs, NNS_G2D_VRAM_TYPE_2DMAIN, &man->proxy );
				if( exFlag ){ GX_SetBankForOBJExtPltt( bank ); }
			}
			if( vramType & GFL_VRAM_2D_SUB )
			{
				int bank = GX_GetBankForSubOBJExtPltt();
				if( exFlag ){ GX_SetBankForLCDC( bank ); }
				NNS_G2dLoadPalette( palData, byteOffs, NNS_G2D_VRAM_TYPE_2DSUB, &man->proxy );
				if( exFlag ){ GX_SetBankForSubOBJExtPltt( bank ); }
			}
		}

		SysWork.plttMan[idx].emptyFlag = FALSE;
	}
	else
	{
		GF_ASSERT(0);
	}
}

//------------------------------------------------------------------
/**
 * �p���b�g�Ǘ��̈�̔j��
 *
 * @param   man			�Ǘ��̈�|�C���^
 */
//------------------------------------------------------------------
static void delete_pltt_man( PLTT_MAN* man )
{
	GFL_HEAP_FreeMemory( man );
}

//------------------------------------------------------------------
/**
 * �Z���A�j���Ǘ��̈�̍\�z�A������
 *
 * @param   heapID		�q�[�vID
 * @param   num			�Ǘ�����CGR�f�[�^��
 *
 * @retval  CGR_MAN*	�Ǘ��̈�|�C���^
 */
//------------------------------------------------------------------
static CELLANIM_MAN* create_cellanim_man( u16 heapID, u32 num )
{
	u32 i;
	CELLANIM_MAN* man = GFL_HEAP_AllocMemory( heapID, sizeof(CELLANIM_MAN)*num );

	for(i=0; i<num; i++)
	{
		man[i].emptyFlag = TRUE;
	}
	return man;
}

//------------------------------------------------------------------
/**
 * �Z���A�j���Ǘ��̈悩��󂫂�T��
 *
 * @retval  u32		��index
 */
//------------------------------------------------------------------
static inline u32 search_empty_cellanim_pos( void )
{
	u32 i;
	for(i=0; i<SysWork.initParam.CELL_RegisterMax; i++)
	{
		if( SysWork.cellAnimMan[i].emptyFlag )
		{
			return i;
		}
	}
	return GFL_OBJGRP_REGISTER_FAILED;
}

//------------------------------------------------------------------
/**
 * �Z���A�j���f�[�^�̓o�^
 *
 * @param   idx				[in] �o�^�C���f�b�N�X
 * @param   cellDataPtr		[in] �Z���f�[�^�|�C���^
 * @param   animDataPtr		[in] �A�j���f�[�^�|�C���^
 *
 */
//------------------------------------------------------------------
static BOOL register_cellanim( u32 idx, void* cellDataPtr, void* animDataPtr )
{
	CELLANIM_MAN* man = &SysWork.cellAnimMan[idx];

	if(	NNS_G2dGetUnpackedCellBank( cellDataPtr, &(man->cellBankPtr) )
	&&	NNS_G2dGetUnpackedAnimBank( animDataPtr, &(man->animBankPtr) )
	){
		man->emptyFlag = FALSE;
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------
/**
 * �Z���A�j���Ǘ��̈�̔j��
 *
 * @param   man			�Ǘ��̈�|�C���^
 */
//------------------------------------------------------------------
static void delete_cellanim_man( CELLANIM_MAN* man )
{
	GFL_HEAP_FreeMemory( man );
}

