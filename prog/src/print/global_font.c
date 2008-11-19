//============================================================================================
/**
 * @file	global_font.c
 * @brief	�O���[�o���t�H���g�f�[�^
 * @author	taya
 * @date	2008.11.19
 */
//============================================================================================
#include <assert.h>

#include "print/global_font.h"

#include "arc_def.h"
#include "test_graphic\d_taya.naix"


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

	{
		ARCHANDLE* arc = GFL_ARC_OpenDataHandle( ARCID_D_TAYA, heapID );
		GFL_FONT_CreateHandle( arc, NARC_d_taya_lc12_2bit_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
	}
}

