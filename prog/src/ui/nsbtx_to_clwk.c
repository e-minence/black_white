//=============================================================================
/**
 *
 *	@file		nsbtx_to_clwk.c
 *	@brief  �e�N�X�`��(nsbtx)��OAM�`���ɕϊ����ACLWK�ɓ]��
 *	@author	hosaka genya
 *	@data		2009.10.31
 *
 */
//=============================================================================
#include <gflib.h>
#include "system/gra_tool.h"

#include "ui/nsbtx_to_clwk.h"

#include "arc_def.h"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================

//static void OBJ_CgxChangeTransShiftVramOfs( GFL_CLWK* act, u8 * buf, u32 siz, u32 VramOfs, CLSYS_DRAW_TYPE vram_type );

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================


//-----------------------------------------------------------------------------
/**
 *	@brief  nsbtx���j�g���L�����N�^�A�j�g���p���b�g�ɕϊ�����CLWK�ɓ]��
 *
 *	@param	GFL_CLWK* act
 *	@param  arc_idx
 *	@param	tex_idx
 *	@param	ptn_ofs
 *	@param	sx
 *	@param	sy
 *	@param	VramOfs
 *	@param	vram_type
 *	@param	HeapID 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void CLWK_TransNSBTX( GFL_CLWK* act, u32 arc_idx, u32 tex_idx, u8 ptn_ofs, u16 sx, u16 sy, u32 VramOfs, CLSYS_DRAW_TYPE vram_type, HEAPID HeapID )
{
	enum {
		TRANS_PAL_SIZE = 0x20,  ///< ���
	};

	u32	i;
  u32 trans_size;
	ARCHANDLE*		arc;
	NNSG3dResTex*	pTex;	
	void*			pFile;
	u8*				pStart;
	void*			pRawData;
	NNSG3dResDictTexData* pDictTex;
		
  trans_size = sx * sy * 0x20;
		
	// �e�N�X�`���̃A�[�J�C�u�I�[�v�� (nsbtx)
	arc = GFL_ARC_OpenDataHandle( arc_idx, HeapID );

	// �e�N�X�`���t�@�C�����擾
  pFile = GFL_ARC_LoadDataAllocByHandle( arc, tex_idx, HeapID );
	pTex = NNS_G3dGetTex( (NNSG3dResFileHeader*)pFile );
	pDictTex = NNS_G3dGetTexDataByIdx( pTex, ptn_ofs );

	//�e�N�X�`�����E�f�[�^�擾
	{
		void* pBuf;
		u32 offset = (pDictTex->texImageParam & NNS_G3D_TEXIMAGE_PARAM_TEX_ADDR_MASK)<<3;
		
		HOSAKA_Printf( "tex size = %d \n", pTex->texInfo.sizeTex );

		pStart = (u8*)pTex + pTex->texInfo.ofsTex;
		
		pBuf = GFL_HEAP_AllocMemoryLo( HeapID, trans_size );

		pRawData = &pStart[ offset ];
		ChangesInto_RawData_1D_from_2D( pRawData, sx, 0, 0, sx, sy, pBuf );

    // CLWK�ɗ�������
    {
      u16 res_idx = GFL_CLACT_WK_GetCgrIndex( act ); 
      GFL_CLGRP_CGR_ReplaceEx( res_idx, pBuf, trans_size, VramOfs, vram_type );
    }
//	OBJ_CgxChangeTransShiftVramOfs( act, pBuf, trans_size, VramOfs, vram_type );

		GFL_HEAP_FreeMemory( pBuf );
	}

	//�p���b�g���E�f�[�^�擾
	{
		NNSG2dImagePaletteProxy ipp;
		u32 vram;

		pStart = (u8*)pTex + pTex->plttInfo.ofsPlttData;
		DC_FlushRange( pStart, TRANS_PAL_SIZE );
		
    // CLWK���Q�Ƃ��Ă���p���b�g�v���N�V�ɓ]��
    GFL_CLACT_WK_GetPlttProxy( act, &ipp );

	  if( vram_type == CLSYS_DRAW_MAIN || vram_type == CLSYS_DRAW_MAX )
		{
		  vram = NNS_G2dGetImagePaletteLocation( &ipp, NNS_G2D_VRAM_TYPE_2DMAIN );
			GX_LoadOBJPltt( pStart, vram, TRANS_PAL_SIZE );
		}

    if( vram_type == CLSYS_DRAW_SUB || vram_type == CLSYS_DRAW_MAX )
		{
		  vram = NNS_G2dGetImagePaletteLocation( &ipp, NNS_G2D_VRAM_TYPE_2DSUB );
			GXS_LoadOBJPltt( pStart, vram, TRANS_PAL_SIZE );
		}
	}

	// -- �J������ --
  GFL_HEAP_FreeMemory( pFile );
	GFL_ARC_CloseDataHandle( arc );
}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

// tomoya����� GFL_CLGRP_CGR_ReplaceEx ������Ă����������߁A�s�v�ɂȂ�܂����B
#if 0
//-----------------------------------------------------------------------------
/**
 *	@brief	OBJ�Ɋ��蓖�Ă��Ă���L�����f�[�^�iCGX�j������������ 
 *
 *	@param	GFL_CLWK act
 *	@param	* buf
 *	@param	siz
 *	@param	VramOfs 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void OBJ_CgxChangeTransShiftVramOfs( GFL_CLWK* act, u8 * buf, u32 siz, u32 VramOfs, CLSYS_DRAW_TYPE vram_type )
{
	NNSG2dImageProxy ipc;
	u32	cgx;

  GFL_CLACT_WK_GetImgProxy( act, &ipc );

	DC_FlushRange( buf, siz );

	if( vram_type == CLSYS_DRAW_MAIN || vram_type == CLSYS_DRAW_MAX )
  {
    cgx = NNS_G2dGetImageLocation( &ipc, NNS_G2D_VRAM_TYPE_2DMAIN );
    cgx += VramOfs;
		GX_LoadOBJ( buf, cgx, siz );
  }

  if( vram_type == CLSYS_DRAW_SUB || vram_type == CLSYS_DRAW_MAX )
  { 
    cgx = NNS_G2dGetImageLocation( &ipc, NNS_G2D_VRAM_TYPE_2DSUB );
    cgx += VramOfs;
    GXS_LoadOBJ( buf, cgx, siz );
  }

}
#endif

