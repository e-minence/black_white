//============================================================================================
/**
 * @file	global_font.c
 * @brief	�O���[�o���t�H���g�f�[�^
 * @author	taya
 * @date	2008.11.19
 */
//============================================================================================
#include <assert.h>
#include <heapsys.h>

#include "print\global_font.h"

#include "arc_def.h"
#include "font\font.naix"


//--------------------------------------------------------------
/**
 *	�O���[�o�����b�Z�[�W�f�[�^
 */
//--------------------------------------------------------------
const GFL_FONT* GlobalFont_Default = NULL;


//=============================================================================================
/**
 * �O���[�o�����b�Z�[�W�f�[�^�n���h���𐶐��i�v���O�����N����ɂP�x�����Ăԁj
 *
 * @param   heapID		�����p�q�[�vID
 */
//=============================================================================================
void GLOBALFONT_Init( HEAPID heapID )
{
	GF_ASSERT(GlobalFont_Default == NULL);

	OS_TPrintf("GLOBANFONT_Init : HeapSize = %08x ->", GFL_HEAP_GetHeapFreeSize( heapID ));

	{
		GFL_FONT_Create( ARCID_FONT, NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
	}

	OS_TPrintf(" %08x\n", GFL_HEAP_GetHeapFreeSize( heapID ));
}

