//=============================================================================================
/**
 * @file	g3d_tool.c
 * @brief	g3d�֘A�T�|�[�g�c�[��
 * @date	2008.12.09
 * @author	tamada GAME FREAK inc.
 */
//=============================================================================================
#include <gflib.h>

#include "system/g3d_tool.h"

#ifdef	PM_DEBUG
//=============================================================================================
//=============================================================================================

//------------------------------------------------------------------
//------------------------------------------------------------------
//g3d_system.c�̒�`����قڃR�s�[
enum {
	TEX_SLOTSIZ	=	0x20000,
	PLT_SLOTSIZ =	0x2000,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	int index;
	u32 start_addr;
	u32 end_addr;
	u32 blockMax;
	BOOL active;
}FRM_TEX_DUMP_PARAM;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GFL_G3D_VMAN_TEXSIZE texsize;
	GFL_G3D_VMAN_PLTSIZE pltsize;
	u32 frm_plt_free;
}DEBUG_GFL_G3D_INFO;

static DEBUG_GFL_G3D_INFO DebugVManConfigValue;

//=============================================================================================
//
//		�����p�֐�
//
//=============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
u32 DEBUG_GFL_G3D_GetVManTextureSize(void)
{
	return DebugVManConfigValue.texsize * TEX_SLOTSIZ;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
u32 DEBUG_GFL_G3D_GetVManPaletteSize(void)
{
	return DebugVManConfigValue.pltsize * PLT_SLOTSIZ;
}

//------------------------------------------------------------------
/**
 * @brief	�����N�hVRAM�}�l�[�W���̓��������擾���邽�߂̃R�[���o�b�N
 */
//------------------------------------------------------------------
static void LinkDumpCallBack(u32 addr, u32 szByte, void* pUserData )
{
	// ���v�T�C�Y���v�Z
    (*((u32*)pUserData)) += szByte;
}

//------------------------------------------------------------------
/**
 * @brief	�t���[��VRAM�}�l�[�W���̓��������擾���邽�߂̃R�[���o�b�N
 */
//------------------------------------------------------------------
static void FrmTextureDumpCallBack(int index, u32 start_addr, u32 end_addr,
		u32 blockMax, BOOL active, void *userContext)
{
	FRM_TEX_DUMP_PARAM * p = (FRM_TEX_DUMP_PARAM*)userContext;
	p->index = index;
	p->start_addr = start_addr;
	p->end_addr = end_addr;
	p->blockMax = blockMax;
	p->active = active;
}

//------------------------------------------------------------------
/**
 * @brief	�t���[���p���b�gVRAM�}�l�[�W���̓������擾�̂��߂̃R�[���o�b�N
 */
//------------------------------------------------------------------
static void FrmPaletteDumpCallBack(u32 loAddr, u32 hiAddr, u32 szFree, u32 szTotal)
{
	DebugVManConfigValue.frm_plt_free = szFree;
}

//==============================================================================
/**
 * @brief	�������p�֐�
 */
//==============================================================================
void DEBUG_GFL_G3D_SetVManSize(GFL_G3D_VMAN_TEXSIZE texsize, GFL_G3D_VMAN_PLTSIZE pltsize)
{
	DebugVManConfigValue.texsize = texsize;
	DebugVManConfigValue.pltsize = pltsize;
	DebugVManConfigValue.frm_plt_free = 0;
}

//==============================================================================
/**
 * @brief	VRAM�ւ̃e�N�X�`��]����Ԃ��擾����
 *
 * @return  u32		�u�q�`�l�]��
 */
//==============================================================================
u32  DEBUG_GFL_G3D_GetBlankTextureSize(void)
{
	u32 size;
	size = 0;
	if (GFL_G3D_GetTextureManagerMode() == GFL_G3D_VMANLNK) {
		NNS_GfdDumpLnkTexVramManagerEx( LinkDumpCallBack, NULL , &size );
	} else {
		FRM_TEX_DUMP_PARAM param;
		NNS_GfdDumpFrmTexVramManagerEx( FrmTextureDumpCallBack, &param );
		size = param.end_addr - param.start_addr;
	}
	return size;
}

//==============================================================================
/**
 * @brief	VRAM�ւ̃e�N�X�`��]����Ԃ��擾����
 *
 * @return  u32		�g�p���Ă���u�q�`�l�e��
 */
//==============================================================================
u32  DEBUG_GFL_G3D_GetUseTextureSize(void)
{
	u32 size;
	size = DEBUG_GFL_G3D_GetBlankTextureSize();
	// VRAM�}�l�[�W���[���ۑ����Ă���̂́u�g�p���Ă��Ȃ��ʁv�Ȃ̂ő��ʂ�������ĕԂ�
	return DEBUG_GFL_G3D_GetVManTextureSize() - size;
}

//==============================================================================
/**
 * @brief	�����N�hPAL�}�l�[�W���[�̓]����Ԃ��擾����
 *
 * @return  u32		�p���b�g�]��T�C�Y
 */
//==============================================================================
u32  DEBUG_GFL_G3D_GetBlankPaletteSize(void)
{
	u32 size = 0;
	if (GFL_G3D_GetPaletteManagerMode() == GFL_G3D_VMANLNK) {
		NNS_GfdDumpLnkPlttVramManagerEx( LinkDumpCallBack, &size );
	} else {
		NNS_GfdDumpFrmPlttVramManagerEx( FrmPaletteDumpCallBack );
		size = DebugVManConfigValue.frm_plt_free;
	}
	return size;	
}

//==============================================================================
/**
 * @brief	�����N�hPAL�}�l�[�W���[�̓]����Ԃ��擾����
 *
 * @return  u32		�����N�hPAL�}�l�[�W���[�̓]���o�C�g��
 */
//==============================================================================
u32  DEBUG_GFL_G3D_GetUsePaletteSize(void)
{
	u32 size;
	size = DEBUG_GFL_G3D_GetBlankPaletteSize();
	// PAL�}�l�[�W���[���ۑ����Ă���̂́u�g�p���Ă��Ȃ��ʁv�Ȃ̂ő��ʂ�������ĕԂ�
	return DEBUG_GFL_G3D_GetVManPaletteSize() - size;	
}

#endif	//PM_DEBUG

