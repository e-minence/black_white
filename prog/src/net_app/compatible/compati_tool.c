//==============================================================================
/**
 * @file	compati_tool.c
 * @brief	�����`�F�b�N�F�c�[����
 * @author	matsuda
 * @date	2009.02.10(��)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include "system\main.h"
#include "net\network_define.h"
#include "print\printsys.h"
#include "print\gf_font.h"
#include "arc_def.h"
#include "message.naix"
#include "test_graphic\d_taya.naix"
#include "font/font.naix"
#include "system\gfl_use.h"
#include "compati_check.naix"
#include "system/wipe.h"

#include "compati_types.h"
#include "compati_tool.h"


//==============================================================================
//	�O���f�[�^
//==============================================================================
#include "compati_circle.dat"


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �g�p����T�[�N���f�[�^�������_���Ŏ擾����
 *
 * @param   dest		�����
 */
//--------------------------------------------------------------
void CCTOOL_CircleDataGet(CC_CIRCLE_PACKAGE *dest)
{
	int data_no, rand_range;
	
	rand_range = NELEMS(CompatiCircleData);
	data_no = GFUser_GetPublicRand(rand_range);
	GFL_STD_MemCopy(&CompatiCircleData[data_no], dest, sizeof(CC_CIRCLE_PACKAGE));
}
